#include "version.h"
#include "autil.h"
#include "buf.h"
#include "config_file.h"
#include "util.h"
#include <ctype.h>
#include <math.h>
#include <strings.h>
#include "char.h"
#include "enemy.h"
#include "family.h"
#include "item.h"
#include "item_gen.h"
#include "log.h"
#include "magic_base.h"
#include "saacproto_cli.h"
#include "utils/file.h"
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif
#define DEBUGPRINT 0
#define DEBUGPRINT_DETAIL 0

#define FOOD_HP_RATE 200
#define FOOD_MP_RATE 200
#define FOOD_SUCCESS_RATE 150
#define FOOD_TURN_ADDPOINT 1

static int ITEM_getTableNum(int num);

#define ITEM_ATOMIND_MAX 1000
#define ITEM_ATOMIND_FM_MAX 4000

#define ITEM_RANDRANGEDOM 1000
#define ITEM_RANDRANGEDOM_BASE 0
#define ITEM_FM_RANDRANGEDOM 4000

#define ITEM_GEN_RAND_MIN 700
#define ITEM_GEN_RAND_MAX 1200

#define ITEN_GEN_SEARCH_MIN (0.7)
#define ITEN_GEN_SEARCH_MAX (1.10)

static struct tagItemSearchRangeTable {
  double searchmin;
  double searchmax;
} ItemSearchTable[2] = {
    //  { 0.7, 1.1},
    //  { 0.7, 1.3},
    //  { 0.5, 1.5}
    {0.8, 1.2},
    {0.7, 1.3}};

static struct tagItemRandRangeTable {
  int randmin;
  int randmax;
} ItemRandTable[2] = {
    //  { 700, 1200},
    {700, 1300},
    {900, 1100}};

#define ITEM_GEN_RATE 0.7

static struct _tagItemRandRangeTableForItem {
  int num;    /* */
  int minnum; /* Min */
  int maxnum; /* MAX  num + *ITEM_GEN_RATE) */
  double rate; /* maxnum / num*/
} ItemRandTableForItem[] = {{10, 0, 0, 0},   {30, 0, 0, 0},   {65, 0, 0, 0},
                            {125, 0, 0, 0},  {205, 0, 0, 0},  {305, 0, 0, 0},
                            {425, 0, 0, 0},  {565, 0, 0, 0},  {725, 0, 0, 0},
                            {905, 0, 0, 0},  {1125, 0, 0, 0}, // shan add
                            {1354, 0, 0, 0}, {1594, 0, 0, 0}, {1825, 0, 0, 0},
                            {2105, 0, 0, 0}, {2405, 0, 0, 0}, {2725, 0, 0, 0},
                            {3065, 0, 0, 0}, {3425, 0, 0, 0}, {3805, 0, 0, 0}};

#define ATOM_LEVEL_MAX 16
struct item_atom {
  char name[32];
  unsigned int name_hash;
  int magicflg;
};

struct item_atom *item_atoms;
int item_atoms_size;
#define MAX_ITEM_ATOMS_SIZE 256

struct item_ingindtable {
  double data[MAX_ITEM_ATOMS_SIZE];
  int index;
  int num;
};

// static limit the function effective region.
static int ITEM_getAtomIndexByName(const char *atom_name)
{
  int i;
  unsigned int h = hashpjw(atom_name);
  for (i = 0; i < item_atoms_size; i++) {
    if (item_atoms[i].name_hash == h && strcmp(item_atoms[i].name, atom_name) == 0) {
      return i;
    }
  }
  return -1;
}

extern ITEM_Table *ITEM_gTable;
extern ITEM_Index *ITEM_gIndex;

struct ingcache {
  int use;
#define MAXING_ONE 5
  int inguse;
  int canmergefrom;
  int canmergeto;
  int hitnum;
  int ingind[MAXING_ONE];
  int ingval[MAXING_ONE];
};

int ITEM_initRandTable(void) {
  int i;
  for (i = 0; i < arraysizeof(ItemRandTableForItem); i++) {
    if (i == 0)
      ItemRandTableForItem[i].minnum = 0;
    else {
      ItemRandTableForItem[i].minnum = ItemRandTableForItem[i - 1].maxnum + 1;
    }
    if (i + 1 != arraysizeof(ItemRandTableForItem)) {
      ItemRandTableForItem[i].maxnum =
          ItemRandTableForItem[i].num +
          (ItemRandTableForItem[i + 1].num - ItemRandTableForItem[i].num) *
              ITEM_GEN_RATE;

    } else {
      // shan add
      ItemRandTableForItem[i].maxnum = 4000;
      // ItemRandTableForItem[i].maxnum = 1000;
    }
    ItemRandTableForItem[i].rate =
        ItemRandTableForItem[i].maxnum / (double)ItemRandTableForItem[i].num;
  }
  return TRUE;
}

static struct ingcache *icache;
int icache_num;

int ITEM_initItemIngCache(void) {
  int i;
  icache_num = ITEM_getItemMaxIdNum();
  print("icache_num: %d ", icache_num);
  icache = calloc(1, sizeof(struct ingcache) * icache_num);
  if (icache == NULL) {
    print("icache is NULL.\n");
    return FALSE;
  }
  print("IngCache %4.2f MB ...\n",
        sizeof(struct ingcache) * icache_num / 1024.0 / 1024.0);
  remove("old_icache.txt");
  memset(icache, 0, icache_num * sizeof(struct ingcache));
  for (i = 0; i < icache_num; i++) {
    if (ITEM_gIndex[i].use) { // new
      int k = 0;
#define ADD_ICACHE_INGRED(nm, vl)                                              \
  if (ITEM_gTable[ITEM_gIndex[i].index].item.string[nm].string[0]) {                  \
    icache[i].ingind[k] = ITEM_getAtomIndexByName(                             \
        ITEM_gTable[ITEM_gIndex[i].index].item.string[nm].string);                    \
    if (icache[i].ingind[k] < 0) {                                             \
        print("fucking[%s][%d] for %d %s\n",                                   \
            ITEM_gTable[ITEM_gIndex[i].index].item.string[nm].string,                 \
            ITEM_gTable[ITEM_gIndex[i].index].item.data[vl],                          \
            ITEM_gTable[i].item.data[ITEM_ID],                                     \
            ITEM_gTable[ITEM_gIndex[i].index].item.string[ITEM_NAME].string);         \
    } else {                                                                   \
      icache[i].ingval[k] = ITEM_gTable[ITEM_gIndex[i].index].item.data[vl];          \
      k++;                                                                     \
    }                                                                          \
  }
      ADD_ICACHE_INGRED(ITEM_INGNAME0, ITEM_INGVALUE0);
      ADD_ICACHE_INGRED(ITEM_INGNAME1, ITEM_INGVALUE1);
      ADD_ICACHE_INGRED(ITEM_INGNAME2, ITEM_INGVALUE2);
      ADD_ICACHE_INGRED(ITEM_INGNAME3, ITEM_INGVALUE3);
      ADD_ICACHE_INGRED(ITEM_INGNAME4, ITEM_INGVALUE4);
      icache[i].inguse = k;

      if (k == 0) {
        if (ITEM_gTable[ITEM_gIndex[i].index].item.data[ITEM_CANMERGEFROM] ==
                TRUE || // new
            ITEM_gTable[ITEM_gIndex[i].index].item.data[ITEM_CANMERGETO] ==
                TRUE) { // new
          print(
              "ID%d (%s)��δ�趨�ɷ�\n",
              ITEM_gTable[ITEM_gIndex[i].index].item.data[ITEM_ID],             // new
              ITEM_gTable[ITEM_gIndex[i].index].item.string[ITEM_NAME].string); // new
        }
      } else {
        FILE *fp;
        icache[i].use = 1;
        icache[i].canmergefrom =
            ITEM_gTable[ITEM_gIndex[i].index].item.data[ITEM_CANMERGEFROM]; // new
        icache[i].canmergeto =
            ITEM_gTable[ITEM_gIndex[i].index].item.data[ITEM_CANMERGETO]; // new
        if ((fp = fopen("old_icache.txt", "a+")) != NULL) {
          fprintf(fp, "[%s] - %s+%s+%s+%s+%s \n", ITEMTBL_getChar(i, ITEM_NAME),
                  ITEMTBL_getChar(i, ITEM_INGNAME0),
                  ITEMTBL_getChar(i, ITEM_INGNAME1),
                  ITEMTBL_getChar(i, ITEM_INGNAME2),
                  ITEMTBL_getChar(i, ITEM_INGNAME3),
                  ITEMTBL_getChar(i, ITEM_INGNAME4));
          fclose(fp);
        }
      }
    }
  }
  return TRUE;
}

int init_item_atom_callback(int *line_num, const char *line) {
  char tk[1024];
  getStringFromIndexWithDelim(line, ",", 1, tk, sizeof(tk));
  snprintf(item_atoms[*line_num].name, sizeof(item_atoms[*line_num].name), "%s", tk);
  item_atoms[*line_num].name_hash = hashpjw(tk);
  getStringFromIndexWithDelim(line, ",", 2, tk, sizeof(tk));
  item_atoms[*line_num].magicflg = isstring1or0(tk);
  ++(*line_num);
}

int ITEM_initItemAtom(const char *filename) {

  const int line_num = get_file_line_num(filename);
  if (line_num == 0) {
    print("Empty item atom file. Plz check: %s\n", filename);
    return FALSE;
  }
  const int total_item_atoms_size = line_num * sizeof(struct item_atom);
  item_atoms = (struct item_atom *)allocateMemory(total_item_atoms_size);
  if (item_atoms == NULL) {
    print("Cannot allocate memory: %d\n", total_item_atoms_size);
    return FALSE;
  }
  memset(item_atoms, 0, total_item_atoms_size);

  int read_line_num = 0;
  get_file_lines(filename, &read_line_num, init_item_atom_callback);
  if (read_line_num >= MAX_ITEM_ATOMS_SIZE) {
    print("Item Atom Count is too big to use. %d\n", read_line_num);
    return FALSE;
  } else {
    item_atoms_size = read_line_num;
    print("Effective Item Atom Count: %d\n", read_line_num);
    return TRUE;
  }
}

static int ITEM_randRange(int base, int min_rate, int max_rate) {
  int minnum;
  int maxnum;
  int range;

  if (min_rate > max_rate) {
    int tmp;
    tmp = min_rate;
    min_rate = max_rate;
    max_rate = tmp;
  }
  minnum = rint((double)base / ITEM_RANDRANGEDOM * min_rate);
  maxnum = rint((double)base / ITEM_RANDRANGEDOM * max_rate);
  range = (maxnum - minnum);

  if (min_rate == max_rate && min_rate == 0)
    return 0;
  if (range == 0)
    return base;
  if (range < 0)
    return 0;
  return minnum + RAND(0, range);
}

typedef int (*FUNC)(const void *, const void *);

static int cmprutine(double *p1, double *p2) {
  if (*p1 < *p2)
    return -1;
  if (*p1 > *p2)
    return 1;
  return 0;
}

static void ITEM_simplify_atoms(struct item_ingindtable *inds, int num,
                                int *retinds, int *retvals, int petindex,
                                int alchemist) {
  int i;
  double oddstable[] = {0.1,  0.25, 0.35, 0.4, 0.42, 0.44, 0.46,
                        0.47, 0.48, 0.49, 0.5, 0.51, 0.52, 0.53};

  for (i = 0; i < num; i++) {
    int datacnt = (inds + i)->num;
    if (datacnt > 1) {
      int j;
      qsort((inds + i)->data, datacnt, sizeof(double), (FUNC)cmprutine);
      for (j = 1; j < datacnt; j++) {
        int tablenum;
        double rate;
        tablenum = ITEM_getTableNum((inds + i)->data[j - 1]);
        rate =
            ItemRandTableForItem[tablenum].rate / ItemRandTableForItem[0].rate;
        (inds + i)->data[j] +=
            (inds + i)->data[j - 1] * oddstable[j - 1] * rate;
      }
    }
    *(retinds + i) = (inds + i)->index;
    *(retvals + i) = (int)(inds + i)->data[datacnt - 1];
    // shan add begin
    if (petindex != -1) {
      if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
        if (*(retvals + i) > ITEM_ATOMIND_FM_MAX) {
          *(retvals + i) = ITEM_ATOMIND_FM_MAX;
        }
      } else {
        if (*(retvals + i) > ITEM_ATOMIND_MAX) {
          *(retvals + i) = ITEM_ATOMIND_MAX;
        }
      }
    }
    // shan end
  }
}

static void ITEM_merge_getPetFix(int petid, int *fixuse, int *fixatom,
                                 int *baseup, int *minadd, int *maxadd,
                                 int petindex, int *fm_fix) {
  int i;
  int ingnum = 0;
  int petarray;

// shan begin
#define FamilyLv 11
  // int PetLv[FamilyLv];
  // for(i=0; i<FamilyLv; i++){
  //     if(i==0) PetLv[i] = 0;
  //     else PetLv[i] = 3*pow(i,2)+10*i+20;
  // }
  int PetLv[] = {0, 33, 52, 77, 108, 145, 188, 237, 292, 353, 420, 493};
  // int PetLv[] = { 0, 33, 52, 77, 108, 145, 188, 1100, 1300, 1500, 1700 };
  // shan end

  if (petid < 0) {
    *fixuse = 0;
    return;
  }
  petarray = ENEMYTEMP_getEnemyTempArrayFromTempNo(petid);
  if (petarray == -1) {
    print("Cannot get enemy array. [%s][%d]\n", __FILE__, __LINE__);
    return;
  }
  for (i = 0; i < 5; i++) {
#ifdef _FMVER21
#define PET_ADD_INGRED(nm, vl1, vl2, vl3)                                      \
  if (strlen(ENEMYTEMP_getChar(petarray, nm)) != 0) {                          \
    fixatom[ingnum] =                                                          \
        ITEM_getAtomIndexByName(ENEMYTEMP_getChar(petarray, nm));              \
    if (fixatom[ingnum] < 0) {                                                 \
      print("\nfucking atom:[%s] for pet id %d",                               \
            ENEMYTEMP_getChar(petarray, nm), petid);                           \
      continue;                                                                \
    }                                                                          \
    baseup[ingnum] = ENEMYTEMP_getInt(petarray, vl1);                          \
    minadd[ingnum] = ITEM_RANDRANGEDOM_BASE + ENEMYTEMP_getInt(petarray, vl2); \
    maxadd[ingnum] = ITEM_RANDRANGEDOM_BASE + ENEMYTEMP_getInt(petarray, vl3); \
    if (CHAR_CHECKINDEX(petindex)) {                                           \
      if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {                        \
        int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);      \
        if (!CHAR_CHECKINDEX(ownerindex)) {                                    \
          print("ownerindex err!\n");                                          \
          return;                                                              \
        }                                                                      \
        if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) {   \
          print("\n Pet Ownerindex Error");                                    \
          return;                                                              \
        }                                                                      \
        baseup[ingnum] = PetLv[getFmLv(ownerindex)];                           \
      }                                                                        \
    }                                                                          \
    if (minadd[ingnum] > maxadd[ingnum]) {                                     \
      int tmp = minadd[ingnum];                                                \
      minadd[ingnum] = maxadd[ingnum];                                         \
      maxadd[ingnum] = tmp;                                                    \
    }                                                                          \
    if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {                          \
      if (minadd[ingnum] < 0)                                                  \
        minadd[ingnum] = ITEM_FM_RANDRANGEDOM;                                 \
      if (maxadd[ingnum] < 0)                                                  \
        maxadd[ingnum] = ITEM_FM_RANDRANGEDOM;                                 \
    } else {                                                                   \
      if (minadd[ingnum] < 0)                                                  \
        minadd[ingnum] = ITEM_RANDRANGEDOM;                                    \
      if (maxadd[ingnum] < 0)                                                  \
        maxadd[ingnum] = ITEM_RANDRANGEDOM;                                    \
    }                                                                          \
    ingnum++;                                                                  \
  }
#else
#define PET_ADD_INGRED(nm, vl1, vl2, vl3)                                      \
  if (strlen(ENEMYTEMP_getChar(petarray, nm)) != 0) {                          \
    fixatom[ingnum] =                                                          \
        ITEM_getAtomIndexByName(ENEMYTEMP_getChar(petarray, nm));              \
    if (fixatom[ingnum] < 0) {                                                 \
      print("\nfucking atom:[%s] for pet id %d",                               \
            ENEMYTEMP_getChar(petarray, nm), petid);                           \
      continue;                                                                \
    }                                                                          \
    baseup[ingnum] = ENEMYTEMP_getInt(petarray, vl1);                          \
    minadd[ingnum] = ENEMYTEMP_getInt(petarray, vl2);                          \
    maxadd[ingnum] = ENEMYTEMP_getInt(petarray, vl3);                          \
    if (petindex != -1) {                                                      \
      if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {                        \
        int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);      \
        if (!CHAR_CHECKINDEX(ownerindex)) {                                    \
          print("ownerindex err!\n");                                          \
          return;                                                              \
        }                                                                      \
        if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_MEMBER) {   \
          print("\n Pet Ownerindex Error");                                    \
          return;                                                              \
        }                                                                      \
        baseup[ingnum] = PetLv[getFmLv(ownerindex)];                           \
      }                                                                        \
    }                                                                          \
    if (minadd[ingnum] > maxadd[ingnum]) {                                     \
      int tmp = minadd[ingnum];                                                \
      minadd[ingnum] = maxadd[ingnum];                                         \
      maxadd[ingnum] = tmp;                                                    \
    }                                                                          \
    if (CHAR_getInt(petid, CHAR_PETFAMILY) == 1) {                             \
      if (minadd[ingnum] < 0)                                                  \
        minadd[ingnum] = ITEM_FM_RANDRANGEDOM;                                 \
      if (maxadd[ingnum] < 0)                                                  \
        maxadd[ingnum] = ITEM_FM_RANDRANGEDOM;                                 \
    } else {                                                                   \
      if (minadd[ingnum] < 0)                                                  \
        minadd[ingnum] = ITEM_RANDRANGEDOM;                                    \
      if (maxadd[ingnum] < 0)                                                  \
        maxadd[ingnum] = ITEM_RANDRANGEDOM;                                    \
    }                                                                          \
    ingnum++;                                                                  \
  }
#endif

    PET_ADD_INGRED(E_T_ATOMFIXNAME1, E_T_ATOMBASEADD1, E_T_ATOMFIXMIN1,
                   E_T_ATOMFIXMAX1);
    PET_ADD_INGRED(E_T_ATOMFIXNAME2, E_T_ATOMBASEADD2, E_T_ATOMFIXMIN2,
                   E_T_ATOMFIXMAX2);
    PET_ADD_INGRED(E_T_ATOMFIXNAME3, E_T_ATOMBASEADD3, E_T_ATOMFIXMIN3,
                   E_T_ATOMFIXMAX3);
    PET_ADD_INGRED(E_T_ATOMFIXNAME4, E_T_ATOMBASEADD4, E_T_ATOMFIXMIN4,
                   E_T_ATOMFIXMAX4);
    PET_ADD_INGRED(E_T_ATOMFIXNAME5, E_T_ATOMBASEADD5, E_T_ATOMFIXMIN5,
                   E_T_ATOMFIXMAX5);
  }

  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
    int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
    if (CHAR_CHECKINDEX(ownerindex)) {
      *fm_fix = PetLv[getFmLv(ownerindex)];
    }
  }

  *fixuse = ingnum;
  return;
}

static BOOL ITEM_merge_checkitem(ITEM_Item *items, int itemsnum, int id) {
  int loop;
  for (loop = 0; loop < itemsnum; loop++) {
    if (items[loop].data[ITEM_ID] == id)
      break;
  }
  if (loop == itemsnum)
    return TRUE;
  return FALSE;
}

static int ITEM_merge_with_retry(ITEM_Item *items, int itemsnum,
                                 int *ingindtable, int ingnum, int *ingtable,
                                 int searchtable, int petindex) {

  int ideal;
  int idel_num[5][5] = {
      {0}, {250, 0}, {400, 150, 0}, {700, 260, 70, 0}, {740, 500, 200, 40, 0}};
  int endflg[5];
  BOOL first = TRUE;
  int extractcnt = 0;
#define MAXMATCH 2048
  int matchid[MAXMATCH];
  int i, j, k;
  if (ingnum > 5)
    ideal = 5;
  else
    ideal = ingnum;
  memset(endflg, 0, sizeof(endflg));
  while (1) {
    int r = RAND(0, 999);
    int extractnum;
    int match = 0;
    if (extractcnt >= ideal)
      break;
    // ����ϳɵ��ߵı��������
    for (extractnum = 0; extractnum < ideal; extractnum++) {
      if (r >= idel_num[ideal - 1][extractnum]) {
        break;
      }
    }
    if (endflg[extractnum] == TRUE)
      continue;
    endflg[extractnum] = TRUE;
    extractcnt++;
    extractnum = ideal - (extractnum);

    if (first) {
      for (i = 0; i < icache_num; i++) {             //  ��Ѱȫ��ITEM
        if (icache[i].use && icache[i].canmergeto) { // ����ǿɺϳɵ�ITEM
          icache[i].hitnum = 0;
          for (j = 0; j < icache[i].inguse; j++) { // ���ITEM�Ĳ�����
            for (k = 0; k < ingnum; k++) {         // �ϳɵĲ�����
              if (icache[i].ingind[j] == ingindtable[k]) { // ���Ϸ���
                if (searchtable == 0) { // ���ߺϳɵĻ�
                  int tablenum = ITEM_getTableNum(ingtable[k]); // ���ϵȼ�
                  // shan begin
                  if (petindex != -1) {
                    if (CHAR_getInt(petindex, CHAR_PETFAMILY) ==
                        1) { // ������ػ��޵Ļ�
                      int ownerindex =
                          CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
                      int familyLevel = getFmLv(ownerindex);
                      if (!CHAR_CHECKINDEX(ownerindex)) {
                        print("������������!\n");
                        return -1;
                      }
#ifdef _FMVER21
                      if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) !=
                          FMMEMBER_LEADER) {
#else
                      if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) !=
                          FMMEMBER_MEMBER) {
#endif
                        print("\n ����������������");
                        return -1;
                      }
                      // �����������
                      if (icache[i].ingval[j] <=
                              ingtable[k] *
                                  ItemRandTableForItem[tablenum].rate &&
                          icache[i].ingval[j] >=
                              ingtable[k] *
                                  (1 / ItemRandTableForItem[tablenum].rate) &&
                          icache[i].ingval[j] <=
                              ItemRandTableForItem[familyLevel + 9].maxnum) {
                        icache[i].hitnum++;
                        break;
                      }
                    } else { // ���ػ��޵Ļ�
                      float toplimit =
                          ingtable[k] * ItemRandTableForItem[tablenum].rate;
                      if (toplimit > 2500)
                        toplimit = 2500;
                      // �����������
                      if (icache[i].ingval[j] <= toplimit &&
                          icache[i].ingval[j] >=
                              ingtable[k] *
                                  (1 / ItemRandTableForItem[tablenum].rate)) {
                        icache[i].hitnum++;
                        break;
                      }
                    }
                  }
                  // shan end
                } else { // ʳ��ϳɵĻ�
                  // shan begin
                  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
                    int ownerindex =
                        CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
                    int familyLevel = getFmLv(ownerindex);
                    if (!CHAR_CHECKINDEX(ownerindex)) {
                      print("������������!\n");
                      return -1;
                    }
#ifdef _FMVER21
                    if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) !=
                        FMMEMBER_LEADER) {
#else
                    if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) !=
                        FMMEMBER_MEMBER) {
#endif
                      print("\n ����������������");
                      return -1;
                    }
                    if (ingtable[k] >
                        ItemRandTableForItem[familyLevel + 9].maxnum /
                            ItemSearchTable[searchtable].searchmax) {
                      ingtable[k] =
                          ItemRandTableForItem[familyLevel + 9].maxnum /
                          ItemSearchTable[searchtable].searchmax;
                    }
                  } else {
                    if (ingtable[k] >
                        ItemRandTableForItem[9].maxnum /
                            ItemSearchTable[searchtable].searchmax) {
                      ingtable[k] = ItemRandTableForItem[9].maxnum /
                                    ItemSearchTable[searchtable].searchmax;
                    }
                  }
                  // shan end

                  if (icache[i].ingval[j] <=
                          ingtable[k] *
                              ItemSearchTable[searchtable].searchmax &&
                      icache[i].ingval[j] >=
                          ingtable[k] *
                              ItemSearchTable[searchtable].searchmin) {
                    icache[i].hitnum++;
                    break;
                  }
                }
              }
            }
          }
          // ���ϵĲ�����������ͬ
          if (icache[i].hitnum == icache[i].inguse &&
              icache[i].hitnum == extractnum) {
            if (ITEM_merge_checkitem(items, itemsnum, i)) {
              matchid[match++] = i;
            }
          }
        }
      }
      first = FALSE;
    } else {
      for (i = 0; i < icache_num; i++) {
        if (icache[i].use && icache[i].canmergeto) {
          if (icache[i].hitnum == icache[i].inguse &&
              icache[i].hitnum == extractnum) {
            if (ITEM_merge_checkitem(items, itemsnum, i)) {
              matchid[match++] = i;
            }
          }
        }
      }
    }
    if (match > 0) {
      return matchid[random() % match];
    } else {
    }
  }
  return -1;
}

#ifdef _NEW_MANOR_LAW
extern struct FM_POINTLIST fmpointlist;
#endif

#ifdef _COMPOUND_EXP
char mergech[][16] = {"ʯ",   "ľ",    "��",   "��",  "Ƥ",   "����", "��",
                      "צ",   "��",   "Ҷ",    "��",  "���", "��",   "��",
                      "ʯ��", "����", "����", "˯��", "�ظ�", "����", "ˮ��",
                      "��",  "ˮ",    "��",   "��"};
#endif

int ITEM_mergeItem(int charaindex, ITEM_Item *items, int num, int money,
                   int petid, int searchtable, int petindex, int alchemist) {
  int i;
  // shan add
  int item_type = -1;
  int ingnum = 0;
  struct item_ingindtable ingindtable[MAX_ITEM_ATOMS_SIZE];
  int sortedingindtable[MAX_ITEM_ATOMS_SIZE];
  int sortedingtable[MAX_ITEM_ATOMS_SIZE];
  int pet_fixatom[MAX_ITEM_ATOMS_SIZE];
  int pet_baseup[MAX_ITEM_ATOMS_SIZE];
  int pet_minadd[MAX_ITEM_ATOMS_SIZE];
  int pet_maxadd[MAX_ITEM_ATOMS_SIZE];
  int pet_fixuse = MAX_ITEM_ATOMS_SIZE;
  int fm_fix;
  int nowtime = time(NULL);

  // #ifdef _VERSION_80
  if (nowtime - CHAR_getWorkInt(charaindex, CHAR_WORKLASTMERGETIME) < 1) {
    CHAR_setWorkInt(charaindex, CHAR_WORKLASTMERGETIME, nowtime);
    CHAR_talkToCli(charaindex, -1, "合成料理过于频繁，休息一下比较好哟。",
                   CHAR_COLORRED);
    // print(" �ϳ�Ƶ�� ");
    return items[RAND(0, (num - 1))].data[ITEM_ID];
  }
  // #endif

  CHAR_setWorkInt(charaindex, CHAR_WORKLASTMERGETIME, nowtime);
  for (i = 0; i < arraysizeof(ingindtable); i++) {
    int j;
    for (j = 0; j < MAX_ITEM_ATOMS_SIZE; j++) {
      ingindtable[i].data[j] = 0.0;
    }
    ingindtable[i].num = 0;
    ingindtable[i].index = -1;
  }
  for (i = 0; i < num; i++) {
    if (!items[i].data[ITEM_CANMERGEFROM])
      continue;

    // �����ʳ������߲�����ʧ��
    if (item_type == -1)
      item_type = items[i].data[ITEM_TYPE];
    else if (item_type == ITEM_DISH /*20*/) {
      if (items[i].data[ITEM_TYPE] != ITEM_DISH /*20*/)
        return -10;
    } else if (item_type != 20) {
      if (items[i].data[ITEM_TYPE] == 20)
        return -10;
    }

#ifdef _COMPOUND_EXP
    int k;
    for (k = 0; k < 25; k++) {
      if (strcmp(items[i].string[ITEM_INGNAME0].string, mergech[k]) == 0) {
        int value = CHAR_getInt(charaindex, CHAR_MATERIAL01 + k) +
                    (items[i].data[ITEM_INGVALUE0] / (150 - (rand() % 100)));
        if (value > 100000)
          value = 100000;
        CHAR_setInt(charaindex, CHAR_MATERIAL01 + k, value);
      }
      if (strcmp(items[i].string[ITEM_INGNAME1].string, mergech[k]) == 0) {
        int value = CHAR_getInt(charaindex, CHAR_MATERIAL01 + k) +
                    (items[i].data[ITEM_INGVALUE1] / (150 - (rand() % 100)));
        if (value > 100000)
          value = 100000;
        CHAR_setInt(charaindex, CHAR_MATERIAL01 + k, value);
      }
      if (strcmp(items[i].string[ITEM_INGNAME2].string, mergech[k]) == 0) {
        int value = CHAR_getInt(charaindex, CHAR_MATERIAL01 + k) +
                    (items[i].data[ITEM_INGVALUE2] / (150 - (rand() % 100)));
        if (value > 100000)
          value = 100000;
        CHAR_setInt(charaindex, CHAR_MATERIAL01 + k, value);
      }
      if (strcmp(items[i].string[ITEM_INGNAME3].string, mergech[k]) == 0) {
        int value = CHAR_getInt(charaindex, CHAR_MATERIAL01 + k) +
                    (items[i].data[ITEM_INGVALUE3] / (150 - (rand() % 100)));
        if (value > 100000)
          value = 100000;
        CHAR_setInt(charaindex, CHAR_MATERIAL01 + k, value);
      }
      if (strcmp(items[i].string[ITEM_INGNAME4].string, mergech[k]) == 0) {
        int value = CHAR_getInt(charaindex, CHAR_MATERIAL01 + k) +
                    (items[i].data[ITEM_INGVALUE4] / (150 - (rand() % 100)));
        if (value > 100000)
          value = 100000;
        CHAR_setInt(charaindex, CHAR_MATERIAL01 + k, value);
      }
    }
#endif

#define ADD_INGRED(nm, vl)                                                     \
  if (items[i].string[nm].string[0]) {                                         \
    int j;                                                                     \
    int index = ITEM_getAtomIndexByName(items[i].string[nm].string);           \
    if (index < 0) {                                                           \
      print("\nfucking atom:[%s] for item id %d", items[i].string[nm].string,  \
            items[i].data[ITEM_ID]);                                           \
      continue;                                                                \
    }                                                                          \
    for (j = 0; j < ingnum; j++) {                                             \
      if (ingindtable[j].index == index) {                                     \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (j == ingnum)                                                           \
      ingnum++;                                                                \
    ingindtable[j].data[ingindtable[j].num] = items[i].data[vl];               \
    ingindtable[j].index = index;                                              \
    ingindtable[j].num++;                                                      \
  }
    ADD_INGRED(ITEM_INGNAME0, ITEM_INGVALUE0);
    ADD_INGRED(ITEM_INGNAME1, ITEM_INGVALUE1);
    ADD_INGRED(ITEM_INGNAME2, ITEM_INGVALUE2);
    ADD_INGRED(ITEM_INGNAME3, ITEM_INGVALUE3);
    ADD_INGRED(ITEM_INGNAME4, ITEM_INGVALUE4);
  }
  ITEM_simplify_atoms(ingindtable, ingnum, sortedingindtable, sortedingtable,
                      petindex, alchemist);
  for (i = 0; i < MAX_ITEM_ATOMS_SIZE; i++) {
    pet_fixatom[i] = -1;
    pet_baseup[i] = pet_minadd[i] = pet_maxadd[i] = 0;
  }
  if (petid != -1) {
    ITEM_merge_getPetFix(petid, &pet_fixuse, pet_fixatom, pet_baseup,
                         pet_minadd, pet_maxadd, petindex, &fm_fix);
  }
#define ITEM_MERGE_RANGEWIDTH_FORMIN 0.87
#define ITEM_MERGE_RANGEWIDTH_FORMAX 1.05

  for (i = 0; i < ingnum; i++) {
    int j;
    int tablenum = 0;
    if (searchtable == 0) {
      tablenum = ITEM_getTableNum(sortedingtable[i]);
    }
    if (petid != -1) {
      for (j = 0; j < pet_fixuse; j++) {
        if (pet_fixatom[j] == sortedingindtable[i]) {
          int fixedmin = pet_minadd[j];
          int fixedmax = pet_maxadd[j];
          if (fixedmin < 0)
            fixedmin = 0;
          if (fixedmax < 0)
            fixedmax = 0;
          if (searchtable == 0) { // �ϳ�
            sortedingtable[i] =
                ITEM_randRange(sortedingtable[i] + pet_baseup[j],
                               (((1 / ItemRandTableForItem[tablenum].rate) *
                                 ITEM_MERGE_RANGEWIDTH_FORMIN) *
                                (fixedmin / (double)ITEM_RANDRANGEDOM)) *
                                   ITEM_RANDRANGEDOM,
                               ((ItemRandTableForItem[tablenum].rate *
                                 ITEM_MERGE_RANGEWIDTH_FORMAX) *
                                (fixedmax / (double)ITEM_RANDRANGEDOM)) *
                                   ITEM_RANDRANGEDOM);
          } else { // ����
            sortedingtable[i] =
                ITEM_randRange(sortedingtable[i] + pet_baseup[j],
                               ItemRandTable[searchtable].randmin *
                                   ((double)fixedmin / ITEM_RANDRANGEDOM),
                               ItemRandTable[searchtable].randmax *
                                   ((double)fixedmin / ITEM_RANDRANGEDOM));
          }
          break;
        }
      }
      if (j == pet_fixuse) { // ����زĳɷ���������Բ���
        if (searchtable == 0) {
          sortedingtable[i] = ITEM_randRange(
              sortedingtable[i],
              ((1 / ItemRandTableForItem[tablenum].rate) *
               ITEM_MERGE_RANGEWIDTH_FORMIN) *
                  ITEM_RANDRANGEDOM,
              ItemRandTableForItem[tablenum].rate *
                  ITEM_MERGE_RANGEWIDTH_FORMAX * ITEM_RANDRANGEDOM);
        } else {
          sortedingtable[i] = ITEM_randRange(
              sortedingtable[i], ItemRandTable[searchtable].randmin,
              ItemRandTable[searchtable].randmax);
        }
      }
    } else {
      // shan begin
      if (petindex != -1) {
        if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
          int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
          if (!CHAR_CHECKINDEX(ownerindex)) {
            print("ownerindex err!\n");
            return -1;
          }
#ifdef _FMVER21
          if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) {
#else
          if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_MEMBER) {
#endif
            print("\n Pet Ownerindex Error");
            return -1;
          }
          if (searchtable == 0) {
            int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
            int familyLevel = getFmLv(ownerindex);
            for (i = 0; i < ingnum; i++)
              if (sortedingtable[i] >
                  ItemRandTableForItem[familyLevel + 9].maxnum) {
                sortedingtable[i] =
                    RAND(ItemRandTableForItem[familyLevel + 9].minnum,
                         ItemRandTableForItem[familyLevel + 9].maxnum);
              }
          } else {
            sortedingtable[i] = ITEM_randRange(
                sortedingtable[i], ItemRandTable[searchtable].randmin,
                ItemRandTable[searchtable].randmax);
          }
        } else {
          if (searchtable == 0) {
            sortedingtable[i] = ITEM_randRange(
                sortedingtable[i],
                ((1 / ItemRandTableForItem[tablenum].rate) *
                 ITEM_MERGE_RANGEWIDTH_FORMIN) *
                    ITEM_RANDRANGEDOM,
                ItemRandTableForItem[tablenum].rate *
                    ITEM_MERGE_RANGEWIDTH_FORMAX * ITEM_RANDRANGEDOM);
            if (sortedingtable[i] > 905)
              sortedingtable[i] = 905;
          } else {
            sortedingtable[i] = ITEM_randRange(
                sortedingtable[i], ItemRandTable[searchtable].randmin,
                ItemRandTable[searchtable].randmax);
          }
        }
      }
      // shan end
    }
  }

  if (ingnum == 0)
    return -1;
  {
    int created;
    for (i = 0; i < 5; i++) {
      created = ITEM_merge_with_retry(items, num, sortedingindtable, ingnum,
                                      sortedingtable, searchtable, petindex);
      if (created >= 0) {
        // shan begin
        if (petindex != -1) {
#ifdef _FMVER21
#else
          if (CHAR_getInt(CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX),
                          CHAR_FMLEADERFLAG) > 0) {
#endif
          int kind_num = 0;
          int syndp = 0;
          int fooddp = 0;
          int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
          if (!CHAR_CHECKINDEX(ownerindex)) {
            print("ownerindex err!\n");
            return -1;
          }
          if (searchtable == 1) {

            if (ITEMTBL_getInt(created, ITEM_INGVALUE0) != 0)
              kind_num++;
            if (ITEMTBL_getInt(created, ITEM_INGVALUE1) != 0)
              kind_num++;
            if (ITEMTBL_getInt(created, ITEM_INGVALUE2) != 0)
              kind_num++;
            if (ITEMTBL_getInt(created, ITEM_INGVALUE3) != 0)
              kind_num++;
            if (ITEMTBL_getInt(created, ITEM_INGVALUE4) != 0)
              kind_num++;

            fooddp = ITEMTBL_getInt(created, ITEM_INGVALUE0) +
                     ITEMTBL_getInt(created, ITEM_INGVALUE1) +
                     ITEMTBL_getInt(created, ITEM_INGVALUE2) +
                     ITEMTBL_getInt(created, ITEM_INGVALUE3) +
                     ITEMTBL_getInt(created, ITEM_INGVALUE4);
#ifndef _NEW_MANOR_LAW
#ifdef _PERSONAL_FAME // Arminius: �����������
            fooddp = sqrt(fooddp) * pow(2, kind_num - 2) * RAND(0, 6) / 200;
#else
            fooddp = sqrt(fooddp) * pow(2, kind_num - 2) * RAND(0, 6) / 100;
#endif
            char buf[64];
            // print("FoodDP: num->%d sumdp->%d", kind_num, fooddp);
            sprintf(buf, "%d", fooddp);

#ifdef _PERSONAL_FAME // Arminius 8.30: �����������
            CHAR_earnFame(ownerindex, fooddp);
#endif

#ifdef _FMVER21
            if (CHAR_getInt(CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX),
                            CHAR_FMLEADERFLAG) > 0 &&
                CHAR_getInt(CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX),
                            CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) {
              // CoolFish: 2001/10/03
              int fd = getfdFromCharaIndex(ownerindex);
              saacproto_ACFixFMData_send(
                  acfd, CHAR_getChar(ownerindex, CHAR_FMNAME),
                  CHAR_getInt(ownerindex, CHAR_FMINDEX),
                  CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
                  FM_FIX_FMDEALFOOD, buf, "",
                  // CoolFish: 2001/10/03
                  CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX),
                  CONNECT_getFdid(fd));
              // CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX), 0);
            }
#else
            saacproto_ACFixFMData_send(
                acfd, CHAR_getChar(ownerindex, CHAR_FMNAME),
                CHAR_getInt(ownerindex, CHAR_FMINDEX),
                CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
                FM_FIX_FMDEALFOOD, buf, "",
                CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX), 0);
#endif
#endif
          } else {
            syndp = ITEMTBL_getInt(created, ITEM_INGVALUE0) +
                    ITEMTBL_getInt(created, ITEM_INGVALUE1) +
                    ITEMTBL_getInt(created, ITEM_INGVALUE2) +
                    ITEMTBL_getInt(created, ITEM_INGVALUE3) +
                    ITEMTBL_getInt(created, ITEM_INGVALUE4);
#ifndef _NEW_MANOR_LAW
#ifdef _PERSONAL_FAME // Arminius: �����������
            syndp = syndp / 400;
#else
            syndp = syndp / 200;
#endif
            char buf[64];

            sprintf(buf, "%d", syndp);

#ifdef _PERSONAL_FAME // Arminius 8.30: �����������
            CHAR_earnFame(ownerindex, syndp);
#endif

#ifdef _FMVER21
            if (CHAR_getInt(CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX),
                            CHAR_FMLEADERFLAG) > 0 &&
                CHAR_getInt(CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX),
                            CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) {
              // CoolFish: 2001/10/03
              int fd = getfdFromCharaIndex(ownerindex);
              saacproto_ACFixFMData_send(
                  acfd, CHAR_getChar(ownerindex, CHAR_FMNAME),
                  CHAR_getInt(ownerindex, CHAR_FMINDEX),
                  CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
                  FM_FIX_FMSYNTHESIZE, buf, "",
                  // CoolFish: 2001/10/03
                  CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX),
                  CONNECT_getFdid(fd));
            }
#else
            saacproto_ACFixFMData_send(
                acfd, CHAR_getChar(ownerindex, CHAR_FMNAME),
                CHAR_getInt(ownerindex, CHAR_FMINDEX),
                CHAR_getWorkInt(ownerindex, CHAR_WORKFMINDEXI),
                FM_FIX_FMSYNTHESIZE, buf, "",
                CHAR_getWorkInt(ownerindex, CHAR_WORKFMCHARINDEX), 0);
#endif
#endif
          }
#ifdef _FMVER21
#else
          }
#endif
        }
#ifdef _MAX_MERGE_LEVEL
        if (ITEMTBL_getInt(created, ITEM_LEVEL) >= getMaxMergeLevel()) {
          CHAR_talkToCli(charaindex, -1,
                         "�޷��ϳɳ�Խ�趨��װ��Ŷ��",
                         CHAR_COLORRED);
          return items[RAND(0, (num - 1))].data[ITEM_ID];
        } else {
          return created;
        }
#else
        return created;
#endif
      }
    }
    return items[RAND(0, (num - 1))].data[ITEM_ID];
  }
  return -3;
}

int ITEM_canDigest(ITEM_Item *item) {
  /* 1��  ����  ��
     󡻥ɬ�ý�ľ��������1�� ����ƥئ��ئ��0 */
  if (item->string[ITEM_INGNAME0].string[0])
    return 1;
  else
    return 0;
}

int ITEM_merge_test(void) {
  int iid, i, k;
  ITEM_Item item[1];

  for (k = 0; k < 10; k++) {
    iid = 1;
    for (i = 0; i < 20; i++) {
      ITEM_makeItem(&item[0], iid);
      print("%d RETURN: %d\n", i,
            // shan
            iid = ITEM_mergeItem(-1, item, 1, 0, -1, 0, -1, 0));
    }
    print("------\n");
  }
  return 0;
}

int ITEM_mergeItem_merge(int charaindex, int petid, char *data, int petindex,
                         int alchemist) {
  BOOL result = FALSE;
  int i;
  int ret;
  int cnt = 0;
  int itemindexs[CHAR_MAXITEMHAVE - CHAR_STARTITEMARRAY];
  int haveitemindexs[CHAR_MAXITEMHAVE - CHAR_STARTITEMARRAY];
  // Nuke +2 1026: Avoid merge cheat
  int itemflag[CHAR_MAXITEMHAVE - CHAR_STARTITEMARRAY];
  int j, collision = 0;

  char buff[64];

  ITEM_Item items[CHAR_MAXITEMHAVE - CHAR_STARTITEMARRAY];
  // Nuke +1 1026
  memset(itemflag, 0, sizeof(itemflag));

  {
    int emptyindex = CHAR_findEmptyItemBox(charaindex);
    if (emptyindex == -1) {
      CHAR_talkToCli(charaindex, -1, "合成时，最少需要一格空物品栏位。",
                     CHAR_COLORYELLOW);
      return -1;
    }
  }

  for (i = CHAR_STARTITEMARRAY; i < CheckCharMaxItem(charaindex); i++) {
    int haveitemindex;
    ret = getStringFromIndexWithDelim(data, "|", i - CHAR_STARTITEMARRAY + 1,
                                      buff, sizeof(buff));
    if (ret == FALSE)
      break;
    haveitemindex = atoi(buff);
    if (CHAR_CHECKITEMINDEX(charaindex, haveitemindex)) {
      int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
      if (ITEM_CHECKINDEX(itemindex)) {
#ifdef _ITEM_INSLAY
        char *code;
        if ((code = ITEM_getChar(itemindex, ITEM_TYPECODE)) == "\0") {
          if (strcmp(code, "\0")) {
            char token[256];
            snprintf(token, sizeof(token), "�ƺ���%sû����Ȥ��",
                     ITEM_getChar(itemindex, ITEM_NAME));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            return FALSE;
          }
        }
#endif
        if (ITEM_getmergeItemFromFromITEMtabl(
                ITEM_getInt(itemindex, ITEM_ID)) == TRUE) { // ����Ƿ�ɺϳ�
          ITEM_Item *item;
          item = ITEM_getItemPointer(itemindex);
          if (item != NULL) {
            ITEM_makeItem(&items[cnt], ITEM_getInt(itemindex, ITEM_ID));
            itemindexs[cnt] = itemindex;
            haveitemindexs[cnt] = haveitemindex;
            // Nuke start 1026
            for (j = 0; j < cnt; j++)
              if (itemflag[j] == itemindex) {
                print("\nMerge Collision!!!");
                collision = 1;
              }
            itemflag[cnt] = itemindex;
            // Nuke end
            cnt++;
          }
        } else {
          char msgbuf[128];
          snprintf(msgbuf, sizeof(msgbuf), "�ƺ���%sû����Ȥ��",
                   ITEM_getChar(itemindex, ITEM_NAME));
          CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
        }
      }
    }
  }
  // Nuke start 1026
  if (collision) {
    return FALSE;
  }
  // Nuke end
  if (cnt > 1) {
    int randtable = 0;
    if (items[0].data[ITEM_TYPE] == ITEM_DISH) {
      randtable = 1;
    }
    ret = ITEM_mergeItem(charaindex, items, cnt, 0, petid, randtable, petindex,
                         alchemist);
    if (ret == -10)
      CHAR_talkToCli(charaindex, -1, "非法的合成方法", CHAR_COLORWHITE);
    CHAR_setInt(charaindex, CHAR_MERGEITEMCOUNT,
                CHAR_getInt(charaindex, CHAR_MERGEITEMCOUNT) + 1);
    for (i = 0; i < cnt; i++) {
#ifdef _ITEM_PILENUMS
      int pilenum = 0;
      int itemindex = CHAR_getItemIndex(charaindex, haveitemindexs[i]);
      if (!ITEM_CHECKINDEX(itemindex))
        continue;
      pilenum = ITEM_getInt(itemindex, ITEM_USEPILENUMS);
      pilenum -= 1;
      ITEM_setInt(itemindex, ITEM_USEPILENUMS, pilenum);
      if (pilenum <= 0) {
#endif
        CHAR_setItemIndex(charaindex, haveitemindexs[i], -1);
        CHAR_sendItemDataOne(charaindex, haveitemindexs[i]);
        LogItem(CHAR_getChar(charaindex, CHAR_NAME), /* ƽ�ҷ�   */
                CHAR_getChar(charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD ��item��log������item����
                itemindexs[i],
#else
               ITEM_getInt(itemindexs[i], ITEM_ID), /* ʧ��  ة  į */
#endif
                "mergedel(�ϳ���ɾ���ĵ���)", CHAR_getInt(charaindex, CHAR_FLOOR),
                CHAR_getInt(charaindex, CHAR_X),
                CHAR_getInt(charaindex, CHAR_Y),
                ITEM_getChar(itemindexs[i], ITEM_UNIQUECODE),
                ITEM_getChar(itemindexs[i], ITEM_NAME),
                ITEM_getInt(itemindexs[i], ITEM_ID));
        ITEM_endExistItemsOne(itemindexs[i]);
#ifdef _ITEM_PILENUMS
      }
#endif
    }
    CHAR_sendItemData(charaindex, haveitemindexs, cnt);
    if (ret >= 0) {
      int rc;
      int createitemindex = ITEM_makeItemAndRegist(ret);
      // shan begin
      if (createitemindex != -1) {
        if (petindex != -1) {
          if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
            int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
            if (!CHAR_CHECKINDEX(ownerindex)) {
              print("ownerindex err!\n");
              return -1;
            }
#ifdef _FMVER21
            if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
#else
            if (CHAR_getInt(ownerindex, CHAR_FMLEADERFLAG) != FMMEMBER_MEMBER)
#endif
            {
              print("\n Pet Ownerindex Error");
              return -1;
            }
            if (randtable == 1) {
              char *p;
              char charbuf1[512] = "", charbuf2[1024] = "", charbuf[128] = "";
              char argbuf[256];
              int arg1 = 0, arg2 = 0;
              if (ITEM_getInt(createitemindex, ITEM_TYPE) == 20) {
                if ((p = strstr(ITEM_getChar(createitemindex, ITEM_ARGUMENT),
                                "��"))) {
                  strcpy(argbuf, (p + 2));
                  arg1 = atoi(argbuf);
                  arg1 = arg1 * FOOD_MP_RATE / 100;
                }
                if ((p = strstr(ITEM_getChar(createitemindex, ITEM_ARGUMENT),
                                "��"))) {
                  strcpy(argbuf, (p + 2));
                  arg2 = atoi(argbuf);
                  arg2 = arg2 * FOOD_HP_RATE / 100;
                }
                if (arg1 != 0 && arg2 != 0) {
                  sprintf(charbuf1, "��%d��%d", arg1, arg2);
                } else if (arg1 != 0) {
                  sprintf(charbuf1, "��%d", arg1);
                } else if (arg2 != 0) {
                  sprintf(charbuf1, "��%d", arg2);
                }
                sprintf(charbuf, "(��������)");
                strcpy(charbuf2,
                       ITEM_getChar(createitemindex, ITEM_EFFECTSTRING));
                strncat(charbuf2, charbuf, 10);
                ITEM_setChar(createitemindex, ITEM_ARGUMENT, charbuf1);
                ITEM_setChar(createitemindex, ITEM_EFFECTSTRING, charbuf2);
              }
            }
          }
        }
      }
      // shan end
      ITEM_setInt(createitemindex, ITEM_MERGEFLG, TRUE);
      if (createitemindex != -1) {
        rc = CHAR_addItemSpecificItemIndex(charaindex, createitemindex);
        if (rc >= 0 && rc < CheckCharMaxItem(charaindex)) {
          CHAR_sendItemDataOne(charaindex, rc);
          LogItem(CHAR_getChar(charaindex, CHAR_NAME),
                  CHAR_getChar(charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD ��item��log������item����
                  createitemindex,
#else
                   ITEM_getInt(createitemindex, ITEM_ID),
#endif
                  "mergeadd(�ϳɳ����ĵ���)", CHAR_getInt(charaindex, CHAR_FLOOR),
                  CHAR_getInt(charaindex, CHAR_X),
                  CHAR_getInt(charaindex, CHAR_Y),
                  ITEM_getChar(createitemindex, ITEM_UNIQUECODE),
                  ITEM_getChar(createitemindex, ITEM_NAME),
                  ITEM_getInt(createitemindex, ITEM_ID));
          result = TRUE;
        } else {
          ITEM_endExistItemsOne(createitemindex);
        }
      }
    }
  }
  int MAX = 27;
  for (i = 0; i <= MAX; i++) {
    CHAR_sendItemDataOne(charaindex, i);
  }
  return result;
}

static int ITEM_getTableNum(int num) {
  int i;
  for (i = 0; i < arraysizeof(ItemRandTableForItem); i++) {
    if (num <= ItemRandTableForItem[i].maxnum) {
      break;
    }
  }
  if (i == arraysizeof(ItemRandTableForItem))
    i--;
  return i;
}

#ifdef _ITEM_INSLAY
#ifdef _EXPANSION_ITEM_INSLAY
int PETSKILL_ITEM_inslay(int charindex, int inslayindex, int itemindex, int num)
#else
int PETSKILL_ITEM_inslay(int charindex, int inslayindex, int itemindex)
#endif
{
  char *inslaystr, *code = NULL;
  char buf1[256], buf2[256];
  char codeTemp[][32] = {"NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL",
                         "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL",
                         "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL",
                         "NULL", "NULL", "NULL", "NULL"};
  char TypeName[][256] = {"��",   "��", "��", "HP",   "MP",
                          "����", "��", "��", "ħ��ID"};
  int worktyp[] = {ITEM_MODIFYATTACK, ITEM_MODIFYDEFENCE, ITEM_MODIFYQUICK,
                   ITEM_MODIFYHP,     ITEM_MODIFYMP,      ITEM_MODIFYLUCK,
                   ITEM_OTHERDAMAGE,  ITEM_OTHERDEFC,     ITEM_MAGICID};
  int worknum[arraysizeof(worktyp)];

  int i;
#ifdef _EXPANSION_ITEM_INSLAY
  int MAXCANINSLAY = num;
#else
#define MAXCANINSLAY 3
#endif
  memset(worknum, 0, sizeof(worknum));
  if (!CHAR_CHECKINDEX(charindex))
    return FALSE;
  if (!ITEM_CHECKINDEX(inslayindex))
    return FALSE;
  if (!ITEM_CHECKINDEX(itemindex))
    return FALSE;
  if (itemindex == inslayindex)
    return FALSE;
  if ((code = ITEM_getChar(itemindex, ITEM_TYPECODE)) == "\0") {
    print(" ITEM_TYPECODE == NULL error !!\n");
    return FALSE;
  }
  if (!strcmp(code, "NULL"))
    return FALSE;
  if ((inslaystr = ITEM_getChar(inslayindex, ITEM_INLAYCODE)) == "\0") {
    print(" ITEM_INLAYCODE == NULL error !!\n");
    return FALSE;
  }

  for (i = 0; i < MAXCANINSLAY; i++) {
    if (getStringFromIndexWithDelim(inslaystr, "|", i + 1, buf1,
                                    sizeof(buf1)) == TRUE) {
      if (strcmp(buf1, "\0")) {
        sprintf(codeTemp[i], "%s", buf1);
      }
      continue;
    }
    break;
  }

  for (i = 0; i < MAXCANINSLAY; i++) {
    if (strcmp(codeTemp[i], "NULL"))
      continue;
    sprintf(codeTemp[i], "%s", code);
    break;
  }
  if (i == MAXCANINSLAY) {
    char token[256];
    sprintf(token, "����������Ѿ�����%d�Ż�%d�����ϡ�", MAXCANINSLAY,
            MAXCANINSLAY);

    CHAR_talkToCli(charindex, -1, token, CHAR_COLORYELLOW);
    return FALSE;
  }

  memset(buf2, 0, sizeof(buf2));
  sprintf(buf2,
          "%s|%s|%s|%s|%s|"
          "%s|%s|%s|%s|%s|"
          "%s|%s|%s|%s|%s|"
          "%s|%s|%s|%s|%s|"
          "%s|%s|%s|%s|%s",
          codeTemp[0], codeTemp[1], codeTemp[2], codeTemp[3], codeTemp[4],
          codeTemp[5], codeTemp[6], codeTemp[7], codeTemp[8], codeTemp[9],
          codeTemp[10], codeTemp[11], codeTemp[12], codeTemp[13], codeTemp[14],
          codeTemp[15], codeTemp[16], codeTemp[17], codeTemp[18], codeTemp[19],
          codeTemp[20], codeTemp[21], codeTemp[22], codeTemp[23], codeTemp[24]);
  //  print("buf2:%s\n", buf2);
  ITEM_setChar(inslayindex, ITEM_INLAYCODE, buf2);

  for (i = 0; i < arraysizeof(worktyp) - 1; i++) {
    int pwork1, pwork2;
    pwork1 = ITEM_getInt(inslayindex, worktyp[i]);
    pwork2 = ITEM_getInt(itemindex, worktyp[i]);
    ITEM_setInt(inslayindex, worktyp[i], pwork1 + pwork2);
    worknum[i] = pwork1 + pwork2;
  }

  {
    char secretname[256], Mname[256];
    char *funstr; // ITEM_MAGICID
    char *magicname;

    if (ITEM_getInt(itemindex, ITEM_MAGICID) > 0) { // ħ�� ID
      int magicid, mp;
      magicid = ITEM_getInt(itemindex, ITEM_MAGICID);
      ITEM_setInt(inslayindex, ITEM_MAGICID, magicid);
      mp = ITEM_getInt(itemindex, ITEM_MAGICUSEMP);
      ITEM_setInt(inslayindex, ITEM_MAGICUSEMP, mp);
    }
    for (i = ITEM_FIRSTFUNCTION; i < ITEM_LASTFUNCTION; i++) { // function
      ITEM_setChar(inslayindex, i, "\0");
      if ((funstr = ITEM_getChar(itemindex, i)) != NULL) {
        ITEM_setChar(inslayindex, i, funstr);
      }
    }

    if ((funstr = ITEM_getChar(itemindex, ITEM_ARGUMENT)) != NULL) { // ����
      ITEM_setChar(inslayindex, ITEM_ARGUMENT, "\0");
      ITEM_setChar(inslayindex, ITEM_ARGUMENT, funstr);
    }

    memset(Mname, 0, sizeof(Mname));
    if (ITEM_getInt(inslayindex, ITEM_MAGICID) > 0) { // ȡ��ħ������
      int marray = MAGIC_getMagicArray(ITEM_getInt(inslayindex, ITEM_MAGICID));
      if (marray != -1) {
        if ((magicname = MAGIC_getChar(marray, MAGIC_NAME)) != NULL) {
          sprintf(Mname, "%s", magicname);
        }
      }
      ITEM_setInt(inslayindex, ITEM_MAGICID,
                  ITEM_getInt(inslayindex, ITEM_MAGICID));
    }
    sprintf(secretname, "%s%s%d %s%s%d %s%s%d %s%s%d %s%s%d %s%s%d %s%s%d %s",
            TypeName[0], (worknum[0] >= 0) ? "+" : "", worknum[0], TypeName[1],
            (worknum[1] >= 0) ? "+" : "", worknum[1], TypeName[2],
            (worknum[2] >= 0) ? "+" : "", worknum[2], TypeName[3],
            (worknum[3] >= 0) ? "+" : "", worknum[3], TypeName[4],
            (worknum[4] >= 0) ? "+" : "", worknum[4], TypeName[6],
            (worknum[6] >= 0) ? "+" : "", worknum[6], TypeName[7],
            (worknum[7] >= 0) ? "+" : "", worknum[7], Mname);
    ITEM_setChar(inslayindex, ITEM_EFFECTSTRING, secretname);
  }

  return TRUE;
}

#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_ITEM_FixItem(int charindex, int fixindex, int *itemindex) {
  int index, i;
  char *buf2;
  int crushes, maxcrushes;
#define MAXFIXNUM 2
  index = 0;
  for (i = 0; i < MAXFIXNUM; i++) {
    if (fixindex != itemindex[i]) {
      index = itemindex[i];
      break;
    }
  }
  if (i == MAXFIXNUM) {
    print("PETSKILL_ITEM_FixItem error !!\n");
    return FALSE;
  }

  if ((buf2 = ITEM_getChar(index, ITEM_INGNAME0)) == "\0")
    return FALSE;

  for (i = 0; i < 5; i++) { // ITEM_INGVALUE0
    char *buf1;
    if ((buf1 = ITEM_getChar(fixindex, ITEM_INGNAME0 + i)) == NULL)
      continue;
    if (!strcmp(buf2, "") || !strcmp(buf1, ""))
      continue;
    if (!strcmp(buf1, buf2)) {
      break;
    }
  }
  if (i >= 5) {
#ifdef _ITEM_FIXALLBASE
    char *ITEM_ARG = ITEM_getChar(index, ITEM_ARGUMENT);
    if (ITEM_ARG != "\0" && !strcmp(ITEM_ARG, "FIXITEMALL")) {
    } else {
#endif
      CHAR_talkToCli(charindex, -1, "", CHAR_COLORYELLOW);
      return FALSE;
#ifdef _ITEM_FIXALLBASE
    }
#endif
  }

  crushes = ITEM_getInt(fixindex, ITEM_DAMAGECRUSHE);
  maxcrushes = ITEM_getInt(fixindex, ITEM_MAXDAMAGECRUSHE);
  if (crushes >= (maxcrushes * 0.80)) {
    char buff[256];
    sprintf(buff, "%s并没有损坏到需要修复", ITEM_getChar(fixindex, ITEM_NAME));
    CHAR_talkToCli(charindex, -1, buff, CHAR_COLORYELLOW);
    return FALSE;
  } else {
    char *buf1;
    if (maxcrushes < 500) {
      CHAR_talkToCli(charindex, -1, "此物品不能修复", CHAR_COLORYELLOW);
      return FALSE;
    }
    if (crushes <= 0)
      return FALSE;
    // maxcrushes = maxcrushes*0.99;
    crushes = maxcrushes;
    ITEM_setInt(fixindex, ITEM_DAMAGECRUSHE, crushes);
    // ITEM_setInt( fixindex, ITEM_MAXDAMAGECRUSHE, maxcrushes);

    buf1 = ITEM_getChar(fixindex, ITEM_SECRETNAME);
    if (strstr(buf1, "(") != 0) {
      char buf5[256];
      if (getStringFromIndexWithDelim(buf1, "(", 1, buf5, sizeof(buf5)) !=
          FALSE) {
        sprintf(buf1, buf5);
      }
    }
    ITEM_setChar(fixindex, ITEM_SECRETNAME, buf1);
    ITEM_setInt(fixindex, ITEM_CRUSHLEVEL, 0);
  }
  return TRUE;
}
#endif
