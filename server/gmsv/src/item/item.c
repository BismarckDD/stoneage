#include "version.h"
#include "autil.h"
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "config_file.h"
#include "function.h"
#include "magic_base.h"
#include <strings.h>
#ifdef _PROFESSION_SKILL
#include "profession_skill.h"
#endif
#ifdef _ALLBLUES_LUA_1_2
ITEM_LuaFunc ITEM_luaFunc;
#endif
static int ITEM_sTableLen = 0;
static int ITEM_sIndexLen = 0;
static int ITEM_sItemNum = 0;
static int ITEM_sUseItemNum = 0;
ITEM_Exists *ITEM_gExists = NULL;
ITEM_Table *ITEM_gTable = NULL;
ITEM_Index *ITEM_gIndex = NULL;
static char *ITEM_checkString(char *string);
static int ITEM_getRandomValue(char *string, int *randomwidth, int num);
static int ITEM_isstring1or0(char *string, int *randomwidth, int num);
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif

#ifdef _SIMPLIFY_ITEMSTRING

ITEM_intDataSetting ITEM_setIntData[] = {
    {"id", 0, ITEM_ID},
    {"bi", 0, ITEM_BASEIMAGENUMBER},
    {"tg", 0, ITEM_TARGET},
#ifdef _ITEM_MAXUSERNUM
    {"dib", -1, ITEM_DAMAGEBREAK},
#endif
#ifdef _ITEMSET4_TXT
    {"upin", 1, ITEM_USEPILENUMS},
//{"canpile",	0,		ITEM_CANBEPILE },
#endif
    {"dmce", 0, ITEM_DAMAGECRUSHE},
    {"mdmce", 0, ITEM_MAXDAMAGECRUSHE},
#ifdef _ADD_DEAMGEDEFC
#endif
    {"ann", 0, ITEM_ATTACKNUM_MIN},
    {"anx", 0, ITEM_ATTACKNUM_MAX},
    {"ma", 0, ITEM_MODIFYATTACK},
    {"md", 0, ITEM_MODIFYDEFENCE},
    {"mh", 0, ITEM_MODIFYQUICK}, // ITEM_MODIFYHP }, ITEM_MODIFYQUICK
    {"mm", 0, ITEM_MODIFYHP},    // ITEM_MODIFYMP }, ITEM_MODIFYHP
    {"mq", 0, ITEM_MODIFYMP},    // ITEM_MODIFYQUICK }, ITEM_MODIFYMP
    {"ml", 0, ITEM_MODIFYLUCK},
    {"mc", 0, ITEM_MODIFYCHARM},
    {"mv", 0, ITEM_MODIFYAVOID},
    {"mat", 0, ITEM_MODIFYATTRIB},
    {"mav", 0, ITEM_MODIFYATTRIBVALUE},
    {"mid", -1, ITEM_MAGICID},
    {"mpr", 0, ITEM_MAGICPROB},
    {"mu", 0, ITEM_MAGICUSEMP},
    {"ll", 0, ITEM_LEAKLEVEL},
    {"mrf", 0, ITEM_MERGEFLG},
    {"npo", 0, ITEM_POISON},
    {"npa", 0, ITEM_PARALYSIS},
    {"nsl", 0, ITEM_SLEEP},
    {"nst", 0, ITEM_STONE},
    {"ndr", 0, ITEM_DRUNK},
    {"nco", 0, ITEM_CONFUSION},
#ifdef _ITEM_COLOER
    {"coloer", 0, ITEM_COLOER}, /*  ITEM_COLOER  */
#endif
#ifdef _ITEM_USE_TIME
    {"usetime", 0, ITEM_USETIME}, /*  ITEM_USETIME  */
#endif
};

ITEM_charDataSetting ITEM_setCharData[] = {
    {"na", "", ITEM_NAME},         /*  ITEM_NAME   */
    {"sn", "", ITEM_SECRETNAME},   /*  ITEM_SCRETNAME  */
    {"en", "", ITEM_EFFECTSTRING}, /*  ITEM_EFFECTSTRING  */
    {"ar", "", ITEM_ARGUMENT},     /*  ITEM_ARGUMENT   */
#ifdef _ITEM_INSLAY
    {"acode", "", ITEM_TYPECODE},      /*ITEM_TYPECODE,*/
    {"inlaycode", "", ITEM_INLAYCODE}, /*ITEM_INLAYCODE,*/
#endif
    {"cdk", "", ITEM_CDKEY}, /*  ITEM_CDKEY */
#ifdef _ITEM_FORUSERNAMES
    {"forname", "", ITEM_FORUSERNAME},   // ITEM_FORUSERNAME,
    {"forcdkey", "", ITEM_FORUSERCDKEY}, // ITEM_FORUSERCDKEY
#endif

#ifdef _ANGEL_SUMMON
    {"mindex", "", ITEM_ANGELMISSION},
    {"angel", "", ITEM_ANGELINFO},
    {"hero", "", ITEM_HEROINFO},
#endif

#ifdef _UNIQUE_P_I
    {"ucode", "", ITEM_UNIQUECODE}, /* ITEM_UNIQUECODE */
#endif
};

static ITEM_charDataSetting ITEM_setMemData = {
    "", "", -1 /*  ITEM_MEMO  */
};

#else

ITEM_intDataSetting ITEM_setIntData[ITEM_DATA_ENUM_MAX] = {
    {"id", 0},          /*  ITEM_ID */
    {"bi", 0},          /*  ITEM_ITEMBASEIMAGENUMBER */
    {"cs", 0},          /*  ITEM_COST    */
    {"ep", ITEM_OTHER}, /*  ITEM_TYPE   */
    {"ft", 0},          /*  ITEM_ABLEUSEFIELD */
    {"tg", 0},          /*  ITEM_TARGET */
    {"lv", 0},          /*  ITEM_LEVEL  */

#ifdef _ITEM_MAXUSERNUM
    {"dib", -1},        /*ITEM_DAMAGEBREAK*/
#endif

#ifdef _ITEMSET4_TXT
    {"upin", 1},        // ITEM_USEPILENUMS,
    {"canpile", 0},     // ITEM_CANBEPILE
    {"nestr", 0},       // ITEM_NEEDSTR,
    {"nedex", 0},       // ITEM_NEEDDEX,
    {"netra", 0},       // ITEM_NEEDTRANS,
    {"neprof", 0},      // ITEM_NEEDPROFESSION,
#endif

    {"dmce", 0},   /*ITEM_DAMAGECRUSHE*/
    {"mdmce", 0},  // ITEM_MAXDAMAGECRUSHE

#ifdef _ADD_DEAMGEDEFC
    {"otdmag", 0}, // ITEM_OTHERDAMAGE,
    {"otdefc", 0}, // ITEM_OTHERDEFC,
#endif

#ifdef _SUIT_ITEM
    {"nsuit", 0},  // ITEM_SUITCODE,
#endif
    {"ann", 0},    /*  ITEM_ATTACKNUM_MIN  */
    {"anx", 0},    /*  ITEM_ATTACKNUM_MAX  */
    {"ma", 0},     /*  ITEM_MODIFYATTACK    */
    {"md", 0},     /*  ITEM_MODIFYDEFENCE  */
    {"mh", 0},     /*  ITEM_MODIFYHP    */
    {"mm", 0},     /*  ITEM_MODIFYMP    */

    {"mq", 0}, /*  ITEM_MODIFYQUICK */

    {"ml", 0},  /*  ITEM_MODIFYLUCK */
    {"mc", 0},  /*  ITEM_MODIFYCHARM */
    {"mv", 0},  /*  ITEM_MODIFYAVOID */
    {"mat", 0}, /*  ITEM_MODIFYATTRIB */
    {"mav", 0}, /*  ITEM_MODIFYATTRIBVALUE */

    {"mid", -1},    /*  ITEM_MAGICID	*/
    {"mpr", 0},     /*  ITEM_MAGICPROB  */
    {"mu", 0},      /*  ITEM_MAGICUSEMP  */
#ifdef _ITEMSET5_TXT
    {"arr", 0},     /*ITEM_MODIFYARRANGE*/
    {"seqce", 0},   // ITEM_MODIFYSEQUENCE,
    {"iapi", 0},    // ITEM_ATTACHPILE
    {"hirt", 0},    // ITEM_HITRIGHT
#endif

#ifdef _ITEMSET6_TXT
    {"neguard", 0}, // ITEM_NEGLECTGUARD
//	{"bemerge", 0},	// ITEM_BEMERGE
#endif
    {"mpo", 0},      /* ITEM_POISON  */
    {"mpa", 0},      /* ITEM_PARALYSIS  */
    {"msl", 0},      /* ITEM_SLEEP  */
    {"mst", 0},      /* ITEM_STONE  */
    {"mdr", 0},      /* ITEM_DRUNK  */
    {"mco", 0},      /* ITEM_CONFUSION  */
    {"mcr", 0},      /* ITEM_CRITICAL  */
    {"ua", -1},      /* ITEM_USEACTION */
    {"drlo", FALSE}, /* ITEM_DROPATLOGOUT   */
    {"valo", FALSE}, /* ITEM_VANISHATLOGOUT */
    {"ioed", TRUE},  /* ITEM_ISOVERED   */
    {"cpm", TRUE},   /* ITEM_CANPETMAIL   */
    {"cmf", TRUE},   /* ITEM_CANMERGEFROM   */
    {"cmt", TRUE},   /* ITEM_CANMERGETO   */
    {"ingv0", 0},    /* ITEM_INGVALUE0 */
    {"ingv1", 0}, 
    {"ingv2", 0},
    {"ingv3", 0},
    {"ingv4", 0},
    {"ipt", 0}, /*  ITEM_PUTTIME    */
    {"ll", 0},  /*  ITEM_LEAKLEVEL  */
    {"mrf", 0}, /*  ITEM_MERGEFLG  */
    {"clv", 0}, /*	ITEM_CRUSHLEVEL */

    {"v1", 0}, /*  ITEM_VAR1  */
    {"v2", 0}, /*  ITEM_VAR2  */
    {"v3", 0}, /*  ITEM_VAR3  */
    {"v4", 0}, /*  ITEM_VAR4  */
};

ITEM_charDataSetting ITEM_setCharData[ITEM_CHAR_DATA_ENUM_MAX] = {
    {"na", ""},        /*  ITEM_NAME   */
    {"sn", ""},        /*  ITEM_SCRETNAME  */
    {"en", ""},        /*  ITEM_EFFECTSTRING  */
    {"ar", ""},        /*  ITEM_ARGUMENT   */
#ifdef _ITEM_INSLAY
    {"acode", ""},     /*ITEM_TYPECODE,*/
    {"inlaycode", ""}, /*ITEM_INLAYCODE,*/
#endif
    {"cdk", ""},       /*  ITEM_CDKEY   */
#ifdef _ITEM_FORUSERNAMES
    {"forname", ""},   // ITEM_FORUSERNAME,
    {"forcdkey", ""},  // ITEM_FORUSERCDKEY
#endif
                       // CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
    {"ucode", ""},     /*  ITEM_UNIQUECODE  */
#endif
    {"ingn0", ""},     /*  ITEM_INGNAME0 */
    {"ingn1", ""},
    {"ingn2", ""},
    {"ingn3", ""},
    {"ingn4", ""},

    {"inif", ""},                     /*  ITEM_PREOVERFUNC    */
    {"preof", ""},                    /*  ITEM_PREOVERFUNC    */
    {"posof", ""},                    /*  ITEM_POSTOVERFUNC    */
    {"wf", "ITEM_DeleteTimeWatched"}, /*  ITEM_WATCHFUNC  */
    {"usef", ""},                     /*  ITEM_USEFUNC  */
    {"atchf", ""},                    /*  ITEM_ATTACH  */
    {"dtchf", ""},                    /*  ITEM_DETACH  */
    {"attaf", ""},                    /*  ITEM_ATTACHFUNC */
    {"pickupf", ""},                  /*  ITEM_PICKUPFUNC  */
#ifdef _Item_ReLifeAct
    {"relifes", ""},                  /*ITEM_DIERELIFEFUNC*/
#endif
};

static ITEM_charDataSetting ITEM_setMemData = {
    "", "" /*  ITEM_MEMO  */
};
#endif

typedef enum {
  ITEM_INTENTRY = 0,
  ITEM_CHARENTRY = 1,
  ITEM_INTFUNC = 2,
  ITEM_CHARFUNC = 3,
} ITEM_DATATYPECATEGORY;

static struct ITEM_tagItemConfentry {
  char *entryname;
  ITEM_DATATYPECATEGORY type;
  int index;
  void *func;
} ITEM_itemDescriptors[] = {
    {"name", ITEM_CHARFUNC, ITEM_NAME, ITEM_checkString},
    {"secretname", ITEM_CHARFUNC, ITEM_SECRETNAME, ITEM_checkString},
    {"effectstring", ITEM_CHARFUNC, ITEM_EFFECTSTRING, ITEM_checkString},
    {"argument", ITEM_CHARENTRY, ITEM_ARGUMENT, NULL},
#ifdef _ITEM_INSLAY
    {"acode", ITEM_CHARENTRY, ITEM_TYPECODE, NULL},
    {"inlaycode", ITEM_CHARENTRY, ITEM_INLAYCODE, NULL},
#endif
    {"initfunc", ITEM_CHARENTRY, ITEM_INITFUNC, NULL},
    {"preoverfunc", ITEM_CHARENTRY, ITEM_PREOVERFUNC, NULL},
    {"postoverfunc", ITEM_CHARENTRY, ITEM_POSTOVERFUNC, NULL},
    {"watchfunc", ITEM_CHARENTRY, ITEM_WATCHFUNC, NULL},
    {"usefunc", ITEM_CHARENTRY, ITEM_USEFUNC, NULL},
    {"attachfunc", ITEM_CHARENTRY, ITEM_ATTACHFUNC, NULL},
    {"detachfunc", ITEM_CHARENTRY, ITEM_DETACHFUNC, NULL},
    {"dropfunc", ITEM_CHARENTRY, ITEM_DROPFUNC, NULL},
    {"pickupfunc", ITEM_CHARENTRY, ITEM_PICKUPFUNC, NULL},
#ifdef _Item_ReLifeAct
    {"relifefunc", ITEM_CHARENTRY, ITEM_DIERELIFEFUNC, NULL},
#endif
    {"id", ITEM_INTENTRY, ITEM_ID, NULL},
    {"imagenumber", ITEM_INTENTRY, ITEM_BASEIMAGENUMBER, NULL},
    {"cost", ITEM_INTENTRY, ITEM_COST, NULL},
    {"type", ITEM_INTENTRY, ITEM_TYPE, NULL},
    {"fieldtype", ITEM_INTENTRY, ITEM_ABLEUSEFIELD, NULL},
    {"target", ITEM_INTENTRY, ITEM_TARGET, NULL},

    {"level", ITEM_INTENTRY, ITEM_LEVEL, NULL},
#ifdef _ITEM_MAXUSERNUM
    {"dambreak", ITEM_INTENTRY, ITEM_DAMAGEBREAK, NULL},
#endif

#ifdef _ITEMSET4_TXT
    {"upinums", ITEM_INTENTRY, ITEM_USEPILENUMS, NULL},
    {"campile", ITEM_INTENTRY, ITEM_CANBEPILE, NULL},
    {"nestr", ITEM_INTENTRY, ITEM_NEEDSTR, NULL},
    {"nedex", ITEM_INTENTRY, ITEM_NEEDDEX, NULL},
    {"netra", ITEM_INTENTRY, ITEM_NEEDTRANS, NULL},
    {"neprof", ITEM_INTENTRY, ITEM_NEEDPROFESSION, NULL},
#endif

    {"damcrushe", ITEM_INTENTRY, ITEM_DAMAGECRUSHE, NULL},
    {"maxdmce", ITEM_INTENTRY, ITEM_MAXDAMAGECRUSHE, NULL},

#ifdef _ADD_DEAMGEDEFC
    {"otdmags", ITEM_INTENTRY, ITEM_OTHERDAMAGE, NULL},
    {"otdefcs", ITEM_INTENTRY, ITEM_OTHERDEFC, NULL},
#endif
#ifdef _SUIT_ITEM
    {"nsuit", ITEM_INTENTRY, ITEM_SUITCODE, NULL}, // ITEM_SUITCODE,
#endif
    {"attacknum_min", ITEM_INTENTRY, ITEM_ATTACKNUM_MIN, NULL},
    {"attacknum_max", ITEM_INTENTRY, ITEM_ATTACKNUM_MAX, NULL},
    {"attack", ITEM_INTFUNC, ITEM_MODIFYATTACK, ITEM_getRandomValue},
    {"defence", ITEM_INTFUNC, ITEM_MODIFYDEFENCE, ITEM_getRandomValue},
    {"quick", ITEM_INTFUNC, ITEM_MODIFYQUICK, ITEM_getRandomValue},
    {"hp", ITEM_INTFUNC, ITEM_MODIFYHP, ITEM_getRandomValue},
    {"mp", ITEM_INTFUNC, ITEM_MODIFYMP, ITEM_getRandomValue},
    {"luck", ITEM_INTFUNC, ITEM_MODIFYLUCK, ITEM_getRandomValue},
    {"charm", ITEM_INTFUNC, ITEM_MODIFYCHARM, ITEM_getRandomValue},
    {"avoid", ITEM_INTFUNC, ITEM_MODIFYAVOID, ITEM_getRandomValue},
    {"attrib", ITEM_INTENTRY, ITEM_MODIFYATTRIB, NULL},
    {"attribvalue", ITEM_INTENTRY, ITEM_MODIFYATTRIBVALUE, NULL},
    {"magicid", ITEM_INTENTRY, ITEM_MAGICID, NULL},
    {"magicprob", ITEM_INTENTRY, ITEM_MAGICPROB, NULL},
    {"magicusemp", ITEM_INTENTRY, ITEM_MAGICUSEMP, NULL},
#ifdef _ITEMSET5_TXT
    {"arr", ITEM_INTENTRY, ITEM_MODIFYARRANGE, NULL},
    {"seqce", ITEM_INTENTRY, ITEM_MODIFYSEQUENCE, NULL},
    {"iapi", ITEM_INTENTRY, ITEM_ATTACHPILE, NULL},
    {"hirt", ITEM_INTENTRY, ITEM_HITRIGHT, NULL},
#endif
#ifdef _ITEMSET6_TXT
    {"neguard", ITEM_INTENTRY, ITEM_NEGLECTGUARD, NULL},
//	{"bemerge",		ITEM_INTENTRY,	ITEM_BEMERGE,		NULL},
#endif

#ifdef _VERSION_GF
    {"unkown1", ITEM_INTENTRY, ITEM_UNKNOWN1, NULL},
    {"unkown2", ITEM_INTENTRY, ITEM_UNKNOWN2, NULL},
    {"unkown3", ITEM_INTENTRY, ITEM_UNKNOWN3, NULL},
    {"unkown4", ITEM_INTENTRY, ITEM_UNKNOWN4, NULL},
    {"unkown5", ITEM_INTENTRY, ITEM_UNKNOWN5, NULL},
    {"unkown6", ITEM_INTENTRY, ITEM_UNKNOWN6, NULL},
    {"unkown7", ITEM_INTENTRY, ITEM_UNKNOWN7, NULL},
    {"unkown8", ITEM_INTENTRY, ITEM_UNKNOWN8, NULL},
    {"unkown9", ITEM_INTENTRY, ITEM_UNKNOWN9, NULL},
    {"unkown10", ITEM_CHARENTRY, ITEM_UNKNOWN10, NULL},
#endif

    {"poison", ITEM_INTFUNC, ITEM_POISON, ITEM_getRandomValue},
    {"paralysis", ITEM_INTFUNC, ITEM_PARALYSIS, ITEM_getRandomValue},
    {"sleep", ITEM_INTFUNC, ITEM_SLEEP, ITEM_getRandomValue},
    {"stone", ITEM_INTFUNC, ITEM_STONE, ITEM_getRandomValue},
    {"drunk", ITEM_INTFUNC, ITEM_DRUNK, ITEM_getRandomValue},
    {"confusion", ITEM_INTFUNC, ITEM_CONFUSION, ITEM_getRandomValue},
    {"critical", ITEM_INTFUNC, ITEM_CRITICAL, ITEM_getRandomValue},
    {"useaction", ITEM_INTENTRY, ITEM_USEACTION, NULL},

    {"dropatlogout", ITEM_INTFUNC, ITEM_DROPATLOGOUT, ITEM_isstring1or0},
    {"vanishatdrop", ITEM_INTFUNC, ITEM_VANISHATDROP, ITEM_isstring1or0},
    {"isovered", ITEM_INTFUNC, ITEM_ISOVERED, ITEM_isstring1or0},
    {"canpetmail", ITEM_INTFUNC, ITEM_CANPETMAIL, ITEM_isstring1or0},
    {"canmergefrom", ITEM_INTFUNC, ITEM_CANMERGEFROM, ITEM_isstring1or0},
    {"canmergeto", ITEM_INTFUNC, ITEM_CANMERGETO, ITEM_isstring1or0},

    {"ingname0", ITEM_CHARENTRY, ITEM_INGNAME0, NULL},
    {"ingvalue0", ITEM_INTENTRY, ITEM_INGVALUE0, NULL},
    {"ingname1", ITEM_CHARENTRY, ITEM_INGNAME1, NULL},
    {"ingvalue1", ITEM_INTENTRY, ITEM_INGVALUE1, NULL},
    {"ingname2", ITEM_CHARENTRY, ITEM_INGNAME2, NULL},
    {"ingvalue2", ITEM_INTENTRY, ITEM_INGVALUE2, NULL},
    {"ingname3", ITEM_CHARENTRY, ITEM_INGNAME3, NULL},
    {"ingvalue3", ITEM_INTENTRY, ITEM_INGVALUE3, NULL},
    {"ingname4", ITEM_CHARENTRY, ITEM_INGNAME4, NULL},
    {"ingvalue4", ITEM_INTENTRY, ITEM_INGVALUE4, NULL},
};


static INLINE BOOL ITEM_CHECKARRAYINDEX(int index) {
  if (ITEM_sItemNum <= (index) || (index) < 0)
    return FALSE;
  return TRUE;
}

INLINE BOOL _ITEM_CHECKINDEX(char *file, int line, int index) {
  if (!ITEM_CHECKARRAYINDEX(index)) {
    if (index != -1) {
      print("err %s:%d \n", file, line);
    }
    return FALSE;
  }
  if (ITEM_gExists[index].use == FALSE) {
    return FALSE;
  }
  return TRUE;
}

static INLINE int ITEM_CHECKINTDATAINDEX(int index) {
  if (ITEM_DATA_ENUM_MAX <= index || index < 0)
    return FALSE;
  return TRUE;
}

static INLINE int ITEM_CHECKCHARDATAINDEX(int index) {
  if (ITEM_CHAR_DATA_ENUM_MAX <= index || index < 0)
    return FALSE;
  return TRUE;
}

BOOL ITEM_initExistItemsArray(int num) {
  int i;
  if (ITEM_checksetdata() == FALSE)
    return FALSE;
  ITEM_sItemNum = num;
  ITEM_gExists = allocateMemory(sizeof(ITEM_Exists) * num);
  if (ITEM_gExists == NULL)
    return FALSE;
#ifdef _ALLBLUES_LUA_1_2
  memset(&ITEM_luaFunc, 0, sizeof(ITEM_LuaFunc));
#endif

  for (i = 0; i < num; i++) {
    memset(&ITEM_gExists[i], 0, sizeof(ITEM_Exists));
    ITEM_gExists[i].use = FALSE;
  }

  print("初始化已有物品数组，分配 %4.2f MB 空间......",
        sizeof(ITEM_Exists) * num / 1024.0 / 1024.0);
  return TRUE;
}

BOOL ITEM_endExistItemsArray(ITEM_Table *item_table) {
  freeMemory(item_table);
  return TRUE;
}

BOOL ITEM_endExistItemsIndexArray(ITEM_Index *item_index) {
  freeMemory(item_index);
  return TRUE;
}

int _ITEM_initExistItemsOne(char *file, int line, ITEM_Item *item) {
  int i;
  int item_id = -1;
  static int Sindex = 1;
  item_id = item->data[ITEM_ID];
  if (ITEM_CHECKITEMTABLE(item->data[ITEM_ID]) == FALSE) {
    print("CHAR itemData err Item_id:%d=%s%d!!\n", item->data[ITEM_ID], file,
          line);
    return -1;
  }
  for (i = 0; i < ITEM_sItemNum; i++) {
    Sindex++;
    if (Sindex >= ITEM_sItemNum)
      Sindex = 1;
    if (Sindex < 1)
      Sindex = 1;
    if (!ITEM_gExists[Sindex].use) {
      int charaindex;
      charaindex = ITEM_gExists[Sindex].item.workint[ITEM_WORKCHARAINDEX];
      if (CHAR_CHECKINDEX(charaindex) &&
          CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        int j;
        for (j = 0; j < CheckCharMaxItem(charaindex); j++) {
          if (CHAR_getItemIndex(charaindex, j) == Sindex) {
            print("item.c: error! chara have this item char_index[%d] "
                  "item_index[%d] Name(%s)POS(%d)NAME(%s)\n",
                  charaindex, Sindex, CHAR_getUseName(charaindex), j,
                  ITEM_gExists[Sindex].item.string[ITEM_NAME].string);
            print("from %s:%d\n", file, line);
            ITEM_gExists[Sindex].use = TRUE;
            break;
          }
        }
        if (j != CheckCharMaxItem(charaindex))
          continue;
      }
      memcpy(&ITEM_gExists[Sindex].item, item, sizeof(ITEM_Item));
      ITEM_gExists[Sindex].use = TRUE;
      {
        BOOL (*initfunc)(ITEM_Item *) = NULL;
        initfunc = (BOOL(*)(ITEM_Item *))getFunctionPointerFromName(
            item->string[ITEM_INITFUNC].string);
        if (initfunc) {
          if (initfunc(&ITEM_gExists[Sindex].item) == FALSE) {
            ITEM_gExists[Sindex].use = FALSE;
            fprint("Error:Can't init item\n");
            return -1;
          }
        }
      }
      ITEM_sUseItemNum++;
      ITEM_constructFunctable(Sindex);
      return Sindex;
    }
  }
  fprint("InitExistOne结束.\n");
  return -1;
}

void _ITEM_endExistItemsOne(int index, char *file, int line) {
  int i, j;
  int hitcnt = 0;
  int playernum = CHAR_getPlayerMaxNum();

  if (!ITEM_CHECKINDEX(index))
    return;

  for (i = 0; i < playernum; i++) {
    if (CHAR_CHECKINDEX(i)) {
      for (j = 0; j < CheckCharMaxItem(i); j++) {
        if (CHAR_getItemIndex(i, j) == index) {
          print(
              "warning !! player have this item:%d call from [%s:%d](%s)(%s)\n",
              index, file, line, CHAR_getUseName(i),
              ITEM_gExists[index].item.string[ITEM_NAME].string);
          hitcnt++;
          if (hitcnt > 1) {
            print("ITEM_INDEX(%d) duplicate!!\n", index);
          }
        }
      }
    }
  }
  if (hitcnt < 1) {
    ITEM_gExists[index].use = FALSE;
    ITEM_gExists[index].item.workint[ITEM_WORKCHARAINDEX] = -1;
    ITEM_sUseItemNum--;
  }
}

INLINE int ITEM_getIntStrict(int index, ITEM_DATA_ENUM element, int *error) {
  if (!ITEM_CHECKINDEX(index))
    return *error = FALSE;
  if (!ITEM_CHECKINTDATAINDEX(element))
    return *error = FALSE;
  return ITEM_gExists[index].item.data[element];
}

INLINE int _ITEM_getInt(char *file, int line, int index, ITEM_DATA_ENUM element) {
  if (!ITEM_CHECKINDEX(index)) {
    return -1;
  }
  if (!ITEM_CHECKINTDATAINDEX(element)) {
    return -1;
  }
  return ITEM_gExists[index].item.data[element];
}

INLINE int ITEM_setIntStrict(int index, ITEM_DATA_ENUM element, int data,
                             int *error) {
  int buf;
  if (!ITEM_CHECKINDEX(index))
    return *error = FALSE;
  if (!ITEM_CHECKINTDATAINDEX(element))
    return *error = FALSE;
  buf = ITEM_gExists[index].item.data[element];
  ITEM_gExists[index].item.data[element] = data;
  return buf;
}

INLINE int _ITEM_setInt(char *file, int line, int index, ITEM_DATA_ENUM element,
                        int data) {
  int buf;
  if (!ITEM_CHECKINDEX(index)) {
    return -1;
  }

  if (!ITEM_CHECKINTDATAINDEX(element)) {
    return -1;
  }

  buf = ITEM_gExists[index].item.data[element];
  ITEM_gExists[index].item.data[element] = data;
  return buf;
}

INLINE char *ITEM_getChar(int index, ITEM_CHAR_DATA_ENUM element) {
  if (!ITEM_CHECKINDEX(index))
    return "\0";
  if (!ITEM_CHECKCHARDATAINDEX(element))
    return "\0";
  return ITEM_gExists[index].item.string[element].string;
}

INLINE BOOL ITEM_setChar(int index, ITEM_CHAR_DATA_ENUM element, char *new) {
  if (!ITEM_CHECKINDEX(index))
    return FALSE;
  if (!ITEM_CHECKCHARDATAINDEX(element))
    return FALSE;
  strcpysafe(ITEM_gExists[index].item.string[element].string,
             sizeof(ITEM_gExists[index].item.string[element].string), new);
  return TRUE;
}

INLINE int ITEM_getWorkInt(int index, ITEM_WORKDATAINT element) {
  if (!ITEM_CHECKINDEX(index))
    return -1;
  if (element >= ITEM_WORKDATAINTNUM || element < 0)
    return -1;
  return ITEM_gExists[index].item.workint[element];
}

INLINE int ITEM_setWorkInt(int index, ITEM_WORKDATAINT element, int data) {
  int buf;
  if (!ITEM_CHECKINDEX(index))
    return -1;
  if (element >= ITEM_WORKDATAINTNUM || element < 0)
    return -1;

  buf = ITEM_gExists[index].item.workint[element];
  ITEM_gExists[index].item.workint[element] = data;
  return buf;
}

INLINE int ITEM_getITEM_sItemNum(void) { return ITEM_sItemNum; }

INLINE int ITEM_getITEM_sUseItemNum(void) { return ITEM_sUseItemNum; }

INLINE BOOL ITEM_getITEM_use(int index) {
  if (!ITEM_CHECKINDEX(index))
    return FALSE;
  return ITEM_gExists[index].use;
}

void ITEM_constructFunctable(int item_index) {
  int i;
  if (!ITEM_CHECKINDEX(item_index))
    return;

  for (i = ITEM_FIRSTFUNCTION; i < ITEM_LASTFUNCTION; i++) {
    ITEM_gExists[item_index].item.functable[i - ITEM_FIRSTFUNCTION] =
        getFunctionPointerFromName(ITEM_getChar(item_index, i));
#ifdef _ALLBLUES_LUA_1_2
    if (ITEM_gExists[item_index].item.functable[i - ITEM_FIRSTFUNCTION] == NULL) {
      ITEM_setLUAFunction(item_index, i, ITEM_getChar(item_index, i));
    }
#endif
  }
}

void *_ITEM_getFunctionPointer(int item_index, int functype, char *file,
                               int line) {
  if (!ITEM_CHECKINDEX(item_index))
    return NULL;
  if (functype < ITEM_FIRSTFUNCTION || functype >= ITEM_LASTFUNCTION) {
    print("GetFunctionFailed. func_type:%d, file:%s, line:%d\n", functype, file, line);
    return NULL;
  }
  return ITEM_gExists[item_index].item.functable[functype - ITEM_FIRSTFUNCTION];
}

#ifdef _ALLBLUES_LUA_1_2
INLINE BOOL ITEM_setLUAFunction(int item_index, int functype,
                                const char *luafuncname) {
  if (!ITEM_CHECKINDEX(item_index))
    return FALSE;
  if (functype < ITEM_FIRSTFUNCTION || functype >= ITEM_LASTFUNCTION) {
    print("FunctionType is illegal :%d\n", functype);
    return FALSE;
  }
  ITEM_LuaFunc *luaFunc = &ITEM_luaFunc;

  while (luaFunc->next != NULL) {
    if (strcmp(luaFunc->luafuncname, luafuncname) == 0) {
      ITEM_gExists[item_index].item.lua[functype - ITEM_FIRSTFUNCTION] =
          luaFunc->lua;
      ITEM_gExists[item_index].item.luafunctable[functype - ITEM_FIRSTFUNCTION] =
          allocateMemory(strlen(luaFunc->luafunctable));
      memset(
          ITEM_gExists[item_index].item.luafunctable[functype - ITEM_FIRSTFUNCTION],
          0, strlen(luaFunc->luafunctable));
      strcpy(
          ITEM_gExists[item_index].item.luafunctable[functype - ITEM_FIRSTFUNCTION],
          luaFunc->luafunctable);
      return TRUE;
    }
    luaFunc = luaFunc->next;
  }

  ITEM_gExists[item_index].item.lua[functype - ITEM_FIRSTFUNCTION] = NULL;
  ITEM_gExists[item_index].item.luafunctable[functype - ITEM_FIRSTFUNCTION] = NULL;

  return FALSE;
}

BOOL ITEM_addLUAListFunction(lua_State *L, const char *luafuncname,
                             const char *luafunctable) {
  ITEM_LuaFunc *luaFunc = &ITEM_luaFunc;

  while (luaFunc->next != NULL) {
    luaFunc = luaFunc->next;
  }

  luaFunc->next = allocateMemory(sizeof(ITEM_LuaFunc));
  memset(luaFunc->next, 0, sizeof(ITEM_luaFunc));
  if (luaFunc->next == NULL)
    return FALSE;

  luaFunc->lua = L;
  strcpysafe(luaFunc->luafuncname, 128, luafuncname);
  strcpysafe(luaFunc->luafunctable, 128, luafunctable);

  return TRUE;
}

INLINE lua_State *ITEM_getLUAFunction(int item_index, int functype) {
  if (!ITEM_CHECKINDEX(item_index))
    return NULL;
  if (functype < ITEM_FIRSTFUNCTION || functype >= ITEM_LASTFUNCTION) {
    print("���ʹ���2:%d\n", functype);
    return NULL;
  }
  if (ITEM_gExists[item_index].item.lua[functype - ITEM_FIRSTFUNCTION] == NULL) {
    return NULL;
  }
  lua_getglobal(
      ITEM_gExists[item_index].item.lua[functype - ITEM_FIRSTFUNCTION],
      ITEM_gExists[item_index].item.luafunctable[functype - ITEM_FIRSTFUNCTION]);
  return ITEM_gExists[item_index].item.lua[functype - ITEM_FIRSTFUNCTION];
}

#endif

int ITEM_getItemMaxIdNum(void) { return ITEM_sIndexLen; }

BOOL ITEM_checksetdata(void) {
  int i;
  char *strings[ITEM_DATA_ENUM_MAX + ITEM_CHAR_DATA_ENUM_MAX + 1];
#ifdef _SIMPLIFY_ITEMSTRING
  int num = 0;
  for (i = 0; i < arraysizeof(ITEM_setIntData); i++) {
    strings[num++] = ITEM_setIntData[i].dumpchar;
  }
  for (i = 0; i < arraysizeof(ITEM_setCharData); i++) {
    strings[num++] = ITEM_setCharData[i].dumpchar;
  }
  strings[ITEM_DATA_ENUM_MAX + ITEM_CHAR_DATA_ENUM_MAX] = ITEM_setMemData.dumpchar;

  if (!checkStringsUnique(strings, num, 1)) {
    fprint("ITEM_set????data is overlapped.\nIt is not allowed\n");
    return FALSE;
  }
#else
  for (i = 0; i < ITEM_DATA_ENUM_MAX; i++)
    strings[i] = ITEM_setIntData[i].dumpchar;
  for (i = 0; i < ITEM_CHAR_DATA_ENUM_MAX; i++)
    strings[ITEM_DATA_ENUM_MAX + i] = ITEM_setCharData[i].dumpchar;

  strings[ITEM_DATA_ENUM_MAX + ITEM_CHAR_DATA_ENUM_MAX] = ITEM_setMemData.dumpchar;

  if (!checkStringsUnique(strings, arraysizeof(strings), 1)) {
    fprint("ITEM_set????data is overlapped.\nIt is not allowed\n");
    return FALSE;
  }
#endif
  return TRUE;
}

static char ITEM_dataString[STRINGBUFSIZ];
char *ITEM_makeStringFromItemIndex(int index, int mode) {
  if (0 <= index && index < ITEM_sItemNum && ITEM_gExists[index].use == TRUE)
    ;
  else
    return "\0";
  return ITEM_makeStringFromItemData(&ITEM_gExists[index].item, mode);
}

#ifdef _SIMPLIFY_ITEMSTRING2
BOOL CHECK_HaveBeSave(int item_id, int oneNum, int type) {
  if (ITEM_ID == type)
    return TRUE;
  if (ITEMTBL_getInt(item_id, type) == oneNum)
    return FALSE;
  return TRUE;
}
#endif

char *ITEM_makeStringFromItemData(ITEM_Item *one, int mode) {
  int i;
  char delim1 = '\0';
  char delim2 = '\0';
  int strlength = 0;

#ifdef _SIMPLIFY_ITEMSTRING2
  int item_id = one->data[ITEM_ID];
  if (!ITEM_CHECKITEMTABLE(item_id))
    return "\0";
#endif
  if (mode == 0) {
    delim1 = '=';
    delim2 = '|';
  } else {
    delim1 = ':';
    delim2 = ',';
  }

#ifdef _SIMPLIFY_ITEMSTRING
  for (i = 0; i < arraysizeof(ITEM_setIntData); i++) {
    char linedata[128];
#ifdef _SIMPLIFY_ITEMSTRING2
    if (!CHECK_HaveBeSave(item_id, one->data[ITEM_setIntData[i].table],
                          ITEM_setIntData[i].table))
      continue;
#endif
    snprintf(linedata, sizeof(linedata), "%s%c%d%c",
             ITEM_setIntData[i].dumpchar, delim1,
             one->data[ITEM_setIntData[i].table], delim2);
    strcpysafe(&ITEM_dataString[strlength], sizeof(ITEM_dataString) - strlength,
               linedata);
    strlength += strlen(linedata);
  }

  for (i = 0; i < arraysizeof(ITEM_setCharData); i++) {
    char linedata[128];
    char escapebuffer[128];
    if (strlen(one->string[ITEM_setCharData[i].table].string) == 0)
      continue;

    snprintf(linedata, sizeof(linedata), "%s%c%s%c",
             ITEM_setCharData[i].dumpchar, delim1,
             makeEscapeString(one->string[ITEM_setCharData[i].table].string,
                              escapebuffer, sizeof(escapebuffer)),
             delim2);
    strcpysafe(&ITEM_dataString[strlength], sizeof(ITEM_dataString) - strlength,
               linedata);
    strlength += strlen(linedata);
  }
#else
  for (i = 0; i < ITEM_DATA_ENUM_MAX; i++) {
    char linedata[128];
    snprintf(linedata, sizeof(linedata), "%s%c%d%c",
             ITEM_setIntData[i].dumpchar, delim1, one->data[i], delim2);
    strcpysafe(&ITEM_dataString[strlength], sizeof(ITEM_dataString) - strlength,
               linedata);
    strlength += strlen(linedata);
  }

  for (i = 0; i < ITEM_CHAR_DATA_ENUM_MAX; i++) {
    char linedata[128];
    char escapebuffer[128];
    if (strlen(one->string[i].string) != 0) {
      snprintf(linedata, sizeof(linedata), "%s%c%s%c",
               ITEM_setCharData[i].dumpchar, delim1,
               makeEscapeString(one->string[i].string, escapebuffer,
                                sizeof(escapebuffer)),
               delim2);
      strcpysafe(&ITEM_dataString[strlength],
                 sizeof(ITEM_dataString) - strlength, linedata);
      strlength += strlen(linedata);
    }
  }
#endif
  dchop(ITEM_dataString, (mode == 0) ? "|" : ",");
  return ITEM_dataString;
}

BOOL ITEM_makeExistItemsFromStringToArg(char *src, ITEM_Item *item, int mode) {
  int readindex = 1;
  char delim1[2];
  char delim2[2];

  if (mode == 0) {
    strcpy(delim1, "=");
    strcpy(delim2, "|");
  } else {
    strcpy(delim1, ":");
    strcpy(delim2, ",");
  }
#ifdef _SIMPLIFY_ITEMSTRING
  {

    int item_id = -1;
    while (1) {
      char linebuf[512];
      char firstToken[256];
      char secondToken[256];
      if (getStringFromIndexWithDelim(src, delim2, readindex, linebuf,
                                      sizeof(linebuf)) == FALSE)
        break;

      readindex++;
      if (getStringFromIndexWithDelim(linebuf, delim1, 1, firstToken,
                                      sizeof(firstToken)) == FALSE)
        return FALSE;
      if (strcmp(firstToken, "id"))
        continue;
      strcpysafe(secondToken, sizeof(secondToken),
                 linebuf + strlen(firstToken) + strlen(delim1));
      item_id = atoi(secondToken);
      if (!ITEM_CHECKITEMTABLE(item_id))
        return FALSE;
      ITEM_getDefaultItemData(item_id, item);
      break;
    }
    readindex = 1;
  }
#else
  ITEM_getDefaultItemSetting(item);
#endif

  while (1) {
    BOOL ret;
    char linebuf[512];

    char firstToken[256];
    char secondToken[256];
    int i;

    ret = getStringFromIndexWithDelim(src, delim2, readindex, linebuf,
                                      sizeof(linebuf));
    if (ret == FALSE)
      break;
    ret = getStringFromIndexWithDelim(linebuf, delim1, 1, firstToken,
                                      sizeof(firstToken));
    if (ret == FALSE)
      return FALSE;
    strcpysafe(secondToken, sizeof(secondToken),
               linebuf + strlen(firstToken) + strlen(delim1));
#ifdef _SIMPLIFY_ITEMSTRING
    for (i = 0; i < arraysizeof(ITEM_setIntData); i++) {
      if (strcmp(firstToken, ITEM_setIntData[i].dumpchar))
        continue;
      item->data[ITEM_setIntData[i].table] = atoi(secondToken);
      goto NEXT;
    }

    for (i = 0; i < arraysizeof(ITEM_setCharData); i++) {
      if (strcmp(firstToken, ITEM_setCharData[i].dumpchar))
        continue;
      strcpysafe(item->string[ITEM_setCharData[i].table].string,
                 sizeof(item->string[ITEM_setCharData[i].table].string),
                 makeStringFromEscaped(secondToken));
      goto NEXT;
    }
#else
    for (i = 0; i < ITEM_DATA_ENUM_MAX; i++) {
      if (strcmp(firstToken, ITEM_setIntData[i].dumpchar) == 0) {
        item->data[i] = atoi(secondToken);
        goto NEXT;
      }
    }

    for (i = 0; i < ITEM_CHAR_DATA_ENUM_MAX; i++) {
      if (strcmp(firstToken, ITEM_setCharData[i].dumpchar) == 0) {
        strcpysafe(item->string[i].string, sizeof(item->string[i].string),
                   makeStringFromEscaped(secondToken));
        goto NEXT;
      }
    }
#endif

  NEXT:
    readindex++;
  }

  if (item->data[ITEM_CANBEPILE] == 0) {
    item->data[ITEM_USEPILENUMS] = 1;
  }
  return TRUE;
}

void ITEM_getDefaultItemSetting(ITEM_Item *item) {
  int i;
  memset(item, 0, sizeof(ITEM_Item));
#ifdef _SIMPLIFY_ITEMSTRING
  for (i = 0; i < arraysizeof(ITEM_setIntData); i++) {
    item->data[ITEM_setIntData[i].table] = ITEM_setIntData[i].defaults;
  }
  for (i = 0; i < arraysizeof(ITEM_setCharData); i++) {
    strcpysafe(item->string[ITEM_setCharData[i].table].string,
               sizeof(item->string[ITEM_setCharData[i].table].string),
               ITEM_setCharData[i].defaults);
  }
#else
  for (i = 0; i < ITEM_DATA_ENUM_MAX; i++)
    item->data[i] = ITEM_setIntData[i].defaults;
  for (i = 0; i < ITEM_CHAR_DATA_ENUM_MAX; i++)
    strcpysafe(item->string[i].string, sizeof(item->string[i].string),
               ITEM_setCharData[i].defaults);
#endif

  strcpysafe(item->string[ITEM_WATCHFUNC].string,
             sizeof(item->string[ITEM_WATCHFUNC].string),
             "ITEM_DeleteTimeWatched");

  for (i = 0; i < ITEM_WORKDATAINTNUM; i++) {
    item->workint[i] = -1;
  }
}

#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData(int item_id, ITEM_Item *item) {
  int i;
  for (i = 0; i < ITEM_DATA_ENUM_MAX; i++) {
    item->data[i] = ITEMTBL_getInt(item_id, i);
  }
  for (i = 0; i < ITEM_CHAR_DATA_ENUM_MAX; i++) {
    strcpysafe(item->string[i].string, sizeof(item->string[i].string),
               ITEMTBL_getChar(item_id, i));
  }
  for (i = 0; i < ITEM_WORKDATAINTNUM; i++) {
    item->workint[i] = -1;
  }
  strcpysafe(item->string[ITEM_WATCHFUNC].string,
             sizeof(item->string[ITEM_WATCHFUNC].string),
             "ITEM_DeleteTimeWatched");
}
#endif

static int ITEM_getRandomValue(char *string, int *randomwidth, int num) {
  int minvalue;
  int maxvalue;
  char token[64];
  int ret;
  *randomwidth = 0;
  ret = getStringFromIndexWithDelim(string, ",", num - 1, token, sizeof(token));
  if (ret == FALSE) {
    return 0;
  }
  minvalue = atoi(token);
  ret = getStringFromIndexWithDelim(string, ",", num, token, sizeof(token));
  if (ret == FALSE) {
    return minvalue;
  }
  maxvalue = atoi(token);
  *randomwidth = ABS(maxvalue - minvalue);
  return min(minvalue, maxvalue);
}
static int ITEM_isstring1or0(char *string, int *randomwidth, int num) {
  char token[64];
  int ret;

  ret = getStringFromIndexWithDelim(string, ",", num - 1, token, sizeof(token));
  if (ret == FALSE) {
    return FALSE;
  }

  return isstring1or0(token);
}

static char *ITEM_checkString(char *string) {
#define ITEM_STRINGLEN 64
  if (strlen(string) >= ITEM_STRINGLEN)
    print("Over %d:[%s]\n", ITEM_STRINGLEN, string);
  return string;
#undef ITEM_STRINGLEN
}

// 记录当前配置文件中最大的ItemId.
int max_item_id = 0;

// 主要用于寻找 max_item_id 和 line_num;
void callbackReadItemConfigFile(int *line_num, const char *line) {
#ifdef _ITEMSET2_ITEM
#define ITEM_ID_TOKEN_INDEX 17
#else
#ifdef _ITEM_MAXUSERNUM
#define ITEM_ID_TOKEN_INDEX 15
#endif
#endif
  char token[64];
  int ret = getStringFromIndexWithDelim(line, ",", ITEM_ID_TOKEN_INDEX, token,
                                    sizeof(token));
  if (ret != TRUE) {
    print("Failed to parse %s, %d\n", line, ITEM_ID_TOKEN_INDEX);
    return;
  }
  const int item_id = atoi(token);
  if (max_item_id < item_id) {
    max_item_id = item_id;
  }
  ++(*line_num);
}

void callbackReadItemConfigFile2(int *line_num, const char *line)
{
  char token[256];
  int intdata[ITEM_DATA_ENUM_MAX];
  memset(intdata, 0, sizeof(intdata));
  int ret, i, item_id;
  int read_pos = 1;
  BOOL data_error = FALSE;
  ITEM_Item item;
  ITEM_getDefaultItemSetting(&item);
  for (i = 0; i < arraysizeof(ITEM_itemDescriptors); i++) {
    ret = getStringFromIndexWithDelim(line, ",", read_pos, token, sizeof(token));
    if (ret == FALSE) {
      data_error = TRUE;
      return;
    }
    if (read_pos == ITEM_ID_TOKEN_INDEX)
      item_id = atoi(token);
    ++read_pos;
    if (strlen(token) != 0) {
      switch (ITEM_itemDescriptors[i].type) {
      case ITEM_INTENTRY:
        item.data[ITEM_itemDescriptors[i].index] = atoi(token);
        break;
      case ITEM_CHARENTRY:
        strcpysafe(item.string[ITEM_itemDescriptors[i].index].string,
                   sizeof(item.string[ITEM_itemDescriptors[i].index].string),
                   token);
        break;
      case ITEM_INTFUNC: {
        int (*int_function)(char *, int *, int);
        int_function = ITEM_itemDescriptors[i].func;
        item.data[ITEM_itemDescriptors[i].index] = int_function(
            line, &intdata[ITEM_itemDescriptors[i].index], read_pos);
        if (int_function == ITEM_getRandomValue)
          ++read_pos;

      } break;
      case ITEM_CHARFUNC: {
        char *(*char_function)(char *);
        char_function = ITEM_itemDescriptors[i].func;
        strcpysafe(item.string[ITEM_itemDescriptors[i].index].string,
                   sizeof(item.string[ITEM_itemDescriptors[i].index].string),
                   char_function(token));
        break;
      }
      default:
        break;
      }
    }
  }
  if (!data_error) {
    if (item_id >= ITEM_sIndexLen) {
      print("ITEM_gTable full:%d err!!!\n", item_id);
    } else if (ITEM_gIndex[item_id].use == TRUE) {
      print("Duplicate ItemId %d.ignore.\n", item_id);
    } else {
      if (item.string[ITEM_SECRETNAME].string[0] == '\0') {
        fprint("ERROR: ID %d item doesn't have secretname\n", item_id);
        memcpy(&item.string[ITEM_SECRETNAME].string,
               &item.string[ITEM_NAME].string,
               sizeof(item.string[ITEM_NAME].string));
      }
      
      int attacknum_min, attacknum_max;
      attacknum_min = item.data[ITEM_ATTACKNUM_MIN];
      attacknum_max = item.data[ITEM_ATTACKNUM_MAX];
      if (attacknum_min == 0)
        attacknum_min = attacknum_max;
      item.data[ITEM_ATTACKNUM_MIN] = min(attacknum_min, attacknum_max);
      item.data[ITEM_ATTACKNUM_MAX] = max(attacknum_min, attacknum_max);
      
      memcpy(&ITEM_gTable[*line_num].item, &item, sizeof(ITEM_Item));
      ITEM_gIndex[item_id].use = TRUE;
      ITEM_gIndex[item_id].index = *line_num;
      for (i = 0; i < ITEM_DATA_ENUM_MAX; i++) {
        ITEM_gTable[*line_num].randomdata[i] = intdata[i];
      }
    }
  } else {
    print("Item Data Error.\n");
  }
}

BOOL ITEM_readItemConfFile(char *filename) {
  max_item_id = 0;
  int line_num, i;
  get_file_lines(filename, &line_num, callbackReadItemConfigFile);
  if (max_item_id <= 0) {
    print("Max item ID is illegal.\n");
    return FALSE;
  }
  print("Max Item ID: %d...", max_item_id);
  ITEM_sTableLen = line_num + 1;
  ITEM_sIndexLen = max_item_id + 1;
  if (ITEM_gTable != NULL)
    ITEM_endExistItemsArray(ITEM_gTable);
  ITEM_gTable = allocateMemory(sizeof(ITEM_Table) * ITEM_sTableLen);
  if (ITEM_gIndex != NULL)
    ITEM_endExistItemsIndexArray(ITEM_gIndex);
  ITEM_gIndex = allocateMemory(sizeof(ITEM_Index) * ITEM_sIndexLen);
  if (ITEM_gTable == NULL) {
    fprint("gItemTable is not allocated. %d\n",
           sizeof(ITEM_Table) * ITEM_sTableLen);
    return FALSE;
  }
  if (ITEM_gIndex == NULL) {
    fprint("gItemIndex is not allocated. %d\n",
           sizeof(ITEM_Index) * ITEM_sIndexLen);
    return FALSE;
  }
  print("gItemTable cost %4.2f MB ......",
        sizeof(ITEM_Table) * ITEM_sTableLen / 1024.0 / 1024.0);
  print("gItemIndex cost %4.2f MB ......",
        sizeof(ITEM_Index) * ITEM_sIndexLen / 1024.0 / 1024.0);
  for (i = 0; i < ITEM_sIndexLen; i++) {
    ITEM_gIndex[i].use = FALSE;
  }
  get_file_lines(filename, &line_num, callbackReadItemConfigFile2);
  return TRUE;
}



CHAR_EquipPlace ITEM_getEquipPlace(int charaindex, int itemid) {
  ITEM_CATEGORY cat;
  cat = ITEM_getInt(itemid, ITEM_TYPE);
  switch (cat) {
  case ITEM_FIST:
  case ITEM_SPEAR:
  case ITEM_AXE:
  case ITEM_CLUB:
    //	case ITEM_SHIELD:
  case ITEM_BOOMERANG:
  case ITEM_BREAKTHROW:
  case ITEM_BOUNDTHROW:

#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
  {
    int i = 0;
    for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
      char *skill_name = "\0";
      int skillid = CHAR_getCharSkill(charaindex, i);
      int Pskillid = PROFESSION_SKILL_getskillArray(skillid);
      if (Pskillid <= 0)
        continue;
      skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
      if (skill_name == "\0")
        continue;

      if ((strcmp(skill_name, "PROFESSION_DUAL_WEAPON")) == 0) {
        int get_item = BATTLE_GetWepon(charaindex);

        if (get_item != ITEM_BOW) {
          int right_hand = CHAR_getItemIndex(charaindex, CHAR_ARM);
          int left_hand = CHAR_getItemIndex(charaindex, CHAR_EQSHIELD);

          if (right_hand < 0)
            return CHAR_ARM;
          else {
            if (left_hand < 0)
              return CHAR_EQSHIELD;
            else
              return CHAR_ARM;
          }
        }
      }
    }
  }
#endif
    return CHAR_ARM;
    break;
  case ITEM_HELM:
    return CHAR_HEAD;
    break;
  case ITEM_ARMOUR:
    return CHAR_BODY;
    break;
  case ITEM_BRACELET:
  case ITEM_MUSIC:
  case ITEM_NECKLACE:
  case ITEM_RING:
  case ITEM_BELT:
  case ITEM_EARRING:
  case ITEM_NOSERING:
  case ITEM_AMULET:
    return CHAR_DECORATION1;
    break;
  case ITEM_BOW:
#ifdef _ITEM_EQUITSPACE
#ifndef _TRUMP_EQUIPMENT
    if (CHAR_getItemIndex(charaindex, CHAR_EQSHIELD) > 0)
      return -1;
#endif
#endif
    return CHAR_ARM;
    break;

#ifdef _ITEM_EQUITSPACE
  case ITEM_WBELT:
    return CHAR_EQBELT;
    break;
  case ITEM_WSHIELD:
#ifndef _TRUMP_EQUIPMENT
    if (BATTLE_GetWepon(charaindex) != ITEM_BOW)
#endif
      return CHAR_EQSHIELD;
    break;
  case ITEM_WSHOES:
    return CHAR_EQSHOES;
    break;
#endif

#ifdef _EQUIT_NEWGLOVE
  case ITEM_WGLOVE:
    return CHAR_EQGLOVE;
    break;
#endif
  default:
    break;
  }
  return -1;
}
#define ITEMSTRINGBUFSIZ 512
static char ITEM_itemStatusStringBuffer[ITEMSTRINGBUFSIZ];
char *ITEM_makeItemStatusString(int haveitem_index, int item_index) {
  char escapename[256];
  char escapeeffectstring[256];
  char paramshow[256];
  int leaklevel;
  int itemcolor = 0;
  int flg;

  if (!ITEM_CHECKINDEX(item_index))
    return "\0";
  leaklevel = ITEM_getInt(item_index, ITEM_LEAKLEVEL);
  // if( leaklevel >= 1 ) 	// Nuke debug
  {
    makeEscapeString(ITEM_getChar(item_index, ITEM_SECRETNAME), escapename,
                     sizeof(escapename));
    paramshow[0] = '\0';
  }
  makeEscapeString(ITEM_getChar(item_index, ITEM_EFFECTSTRING),
                   escapeeffectstring, sizeof(escapeeffectstring));
#ifdef _ITEM_COLOER
  itemcolor = ITEM_getInt(item_index, ITEM_COLOER);
#else
  itemcolor = CHAR_COLORWHITE;
#endif
  if (ITEM_getInt(item_index, ITEM_MERGEFLG)) {
    itemcolor = CHAR_COLORYELLOW;
  }
#ifndef _PET_AND_ITEM_UP
  else if (strlen(ITEM_getChar(item_index, ITEM_CDKEY)) != 0) {
    itemcolor = CHAR_COLORGREEN;
  } else
#endif
#ifdef _ITEM_COLOER_DIFFER
    else if (ITEM_getInt(item_index, ITEM_VANISHATDROP) > 0) {
      itemcolor = CHAR_COLORCYAN;
    }
#endif
  flg = 0;
  if (ITEM_getInt(item_index, ITEM_CANPETMAIL)) {
    flg |= 1 << 0;
  }
  if (ITEM_getInt(item_index, ITEM_CANMERGEFROM)) {
    flg |= 1 << 1;
  }
  if (ITEM_getInt(item_index, ITEM_TYPE) == ITEM_DISH) {
    flg |= 1 << 2;
  }
#ifdef _ITEM_INSLAY
  if (ITEM_getInt(item_index, ITEM_TYPE) == ITEM_METAL) {
    flg |= 1 << 3;
  } else if (ITEM_getInt(item_index, ITEM_TYPE) == ITEM_JEWEL) {
    flg |= 1 << 4;
  }
#endif
#ifdef _PETSKILL_FIXITEM
  if (ITEM_getInt(item_index, ITEM_DAMAGECRUSHE) !=
      ITEM_getInt(item_index, ITEM_MAXDAMAGECRUSHE)) {
    flg |= 1 << 5;
  }
#endif

#ifdef _ALCHEMIST
  char INGNAME0[16];
  char INGNAME1[16];
  strcpy(INGNAME0, ITEM_getChar(item_index, ITEM_INGNAME0));
  strcpy(INGNAME1, ITEM_getChar(item_index, ITEM_INGNAME1));
  if ((INGNAME0[0] != '\0' && INGNAME1[0] != '\0') || (INGNAME0[0] == '\0')) {
    strcpy(INGNAME0, "��");
  }
#endif
  if (haveitem_index == -1) {
#ifdef _ADD_SHOW_ITEMDAMAGE //
    char buff1[256];
    int crushe = ITEM_getInt(item_index, ITEM_DAMAGECRUSHE);
    int maxcrushe = ITEM_getInt(item_index, ITEM_MAXDAMAGECRUSHE);
    if (crushe < 1)
      crushe = 1;
    if (maxcrushe < 1) {
      sprintf(buff1, "������");
    } else {
      maxcrushe = maxcrushe / 1000;
      crushe = crushe / 1000;
      if (maxcrushe <= 0)
        maxcrushe = 1;
      snprintf(buff1, sizeof(buff1), "%d%%", (int)((crushe * 100) / maxcrushe));
    }
    snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),

#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
            "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%s|%d|",
#else
            "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%s",
#endif
#else
            "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s",
#endif
            escapename, paramshow, itemcolor, escapeeffectstring,
            ITEM_getInt(item_index, ITEM_BASEIMAGENUMBER),
            ITEM_getInt(item_index, ITEM_ABLEUSEFIELD),
            ITEM_getInt(item_index, ITEM_TARGET),
            ITEM_getInt(item_index, ITEM_LEVEL), flg, buff1
#ifdef _ITEM_PILENUMS
            ,
            ITEM_getInt(item_index, ITEM_USEPILENUMS)
#ifdef _ALCHEMIST
            ,
            INGNAME0 //,ITEM_getInt( item_index, ITEM_ALCHEMIST)
            ,
            ITEM_getInt(item_index, ITEM_TYPE)
#endif
#endif
    );
#else
    snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
            "%s|%s|%d|%s|%d|%d|%d|%d|%d", escapename, paramshow, itemcolor,
            escapeeffectstring, ITEM_getInt(item_index, ITEM_BASEIMAGENUMBER),
            ITEM_getInt(item_index, ITEM_ABLEUSEFIELD),
            ITEM_getInt(item_index, ITEM_TARGET),
            ITEM_getInt(item_index, ITEM_LEVEL), flg);
#endif
  } else {
#ifdef _ADD_SHOW_ITEMDAMAGE
    char buff1[256];
    int crushe = ITEM_getInt(item_index, ITEM_DAMAGECRUSHE);
    int maxcrushe = ITEM_getInt(item_index, ITEM_MAXDAMAGECRUSHE);
    if (crushe < 1)
      crushe = 1;
    if (maxcrushe < 1) {
      sprintf(buff1, "������");
    } else {
      maxcrushe = maxcrushe / 1000;
      crushe = crushe / 1000;
      if (maxcrushe <= 0)
        maxcrushe = 1;
      snprintf(buff1, sizeof(buff1), "%d%%", (int)((crushe * 100) / maxcrushe));
    }
    snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),

#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
             "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%s|%d|",
#else
             "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d",
#endif
#else
             "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s",
#endif
             escapename, paramshow, itemcolor, escapeeffectstring,
             ITEM_getInt(item_index, ITEM_BASEIMAGENUMBER),
             ITEM_getInt(item_index, ITEM_ABLEUSEFIELD),
             ITEM_getInt(item_index, ITEM_TARGET),
             ITEM_getInt(item_index, ITEM_LEVEL), flg, buff1
#ifdef _ITEM_PILENUMS
             ,
             ITEM_getInt(item_index, ITEM_USEPILENUMS)
#ifdef _ALCHEMIST
                 ,
             INGNAME0 //,ITEM_getInt( item_index, ITEM_ALCHEMIST)
             ,
             ITEM_getInt(item_index, ITEM_TYPE)
#endif
#endif
    );
#else
    snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
             "%d|%s|%s|%d|%s|%d|%d|%d|%d|%d", haveitem_index, escapename,
             paramshow, itemcolor, escapeeffectstring,
             ITEM_getInt(item_index, ITEM_BASEIMAGENUMBER),
             ITEM_getInt(item_index, ITEM_ABLEUSEFIELD),
             ITEM_getInt(item_index, ITEM_TARGET),
             ITEM_getInt(item_index, ITEM_LEVEL), flg);
#endif
  }
  return ITEM_itemStatusStringBuffer;
}

char *ITEM_makeItemFalseString(void) {

#ifdef _ADD_SHOW_ITEMDAMAGE // WON ADD ��ʾ��Ʒ�;ö�
  strcpysafe(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
             "||||||||||||||"
#else
             "|||||||||||"
#endif
#else
             "||||||||||"
#endif
  );

#else
  strcpysafe(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
             "||||||||");
#endif
  return ITEM_itemStatusStringBuffer;
}

char *ITEM_makeItemFalseStringWithNum(int haveitem_index) {
#ifdef _ADD_SHOW_ITEMDAMAGE // WON ADD ��ʾ��Ʒ�;ö�
  snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),

#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
           "%d||||||||||||||",
#else
           "%d||||||||||",
#endif
#else
           "%d|||||||||",
#endif
           haveitem_index);
#else
  snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
           "%d|||||||||", haveitem_index);
#endif
  return ITEM_itemStatusStringBuffer;
}

BOOL ITEM_makeItem(ITEM_Item *item, int number) {
  int i;
  if (ITEM_CHECKITEMTABLE(number) == FALSE) {
    // print(" Can't makeItem for itemid:%d!!\n", number);
    return FALSE;
  }
  memcpy(item, &ITEM_gTable[ITEM_gIndex[number].index].item, sizeof(ITEM_Item)); // new
  for (i = 0; i < ITEM_DATA_ENUM_MAX; i++) {
    int randomvalue;
    randomvalue =
        RAND(0, ITEM_gTable[ITEM_gIndex[number].index].randomdata[i]); // new
    item->data[i] =
        ITEM_gTable[ITEM_gIndex[number].index].item.data[i] + randomvalue; // new
  }
  item->data[ITEM_LEAKLEVEL] = 0;
  return TRUE;
}

int ITEM_makeItemAndRegist(int number) {
  ITEM_Item item;
  memset(&item, 0, sizeof(item));
  if (ITEM_makeItem(&item, number) == FALSE)
    return -1;
  return ITEM_initExistItemsOne(&item);
}

#define EQUIP_FIX_MAX 10000000
void ITEM_equipEffect(int index) {
  int i;
  int attribaccum[4] = {0, 0, 0, 0};
#ifdef _ANGEL_SUMMON
  int angelmode, angelequip = 0;
#endif
  static struct itemeffectstruct {
    int itemdataintindex;
    int charmodifyparamindex;
    int accumulation;
    int min;
    int max;
    int type;
  } itemEffect[] = {
      {ITEM_MODIFYATTACK, CHAR_WORKATTACKPOWER, 0, 0, EQUIP_FIX_MAX, 0},
      {ITEM_MODIFYDEFENCE, CHAR_WORKDEFENCEPOWER, -100, 0, EQUIP_FIX_MAX, 0},
      {ITEM_MODIFYQUICK, CHAR_WORKQUICK, -100, 0, EQUIP_FIX_MAX, 0},
      {ITEM_MODIFYHP, CHAR_WORKMAXHP, 0, 0, EQUIP_FIX_MAX, 0},
      {ITEM_MODIFYMP, CHAR_WORKMAXMP, 0, 0, 1000, 0}, // MP ��100��ƥ
      {ITEM_MODIFYLUCK, CHAR_WORKFIXLUCK, 0, 1, 5, 0},
      {ITEM_MODIFYCHARM, CHAR_WORKFIXCHARM, 0, 0, 100, 0},
      {ITEM_MODIFYAVOID, CHAR_WORKFIXAVOID, 0, 0, EQUIP_FIX_MAX, 0},
      {ITEM_POISON, CHAR_WORKMODPOISON, 0, -100, 100, 0},
      {ITEM_PARALYSIS, CHAR_WORKMODPARALYSIS, 0, -100, 100, 0},
      {ITEM_SLEEP, CHAR_WORKMODSLEEP, 0, -100, 100, 0},
      {ITEM_STONE, CHAR_WORKMODSTONE, 0, -100, 100, 0},
      {ITEM_DRUNK, CHAR_WORKMODDRUNK, 0, -100, 100, 0},
      {ITEM_CONFUSION, CHAR_WORKMODCONFUSION, 0, -100, 100, 0},
      {ITEM_CRITICAL, CHAR_WORKMODCRITICAL, 0, -100, 100, 0},
#ifdef _ADD_DEAMGEDEFC
      {ITEM_OTHERDAMAGE, CHAR_WORKOTHERDMAGE, 0, -100, 100, 0},
      {ITEM_OTHERDEFC, CHAR_WORKOTHERDEFC, 0, -100, 100, 0},
#endif
#ifdef _ITEMSET5_TXT
      {ITEM_MODIFYARRANGE, CHAR_WORKFIXARRANGE, 0, 0, 1000, 0},
      {ITEM_MODIFYSEQUENCE, CHAR_WORKFIXSEQUENCE, 0, 0, 100, 1},
      {ITEM_ATTACHPILE, CHAR_WORKATTACHPILE, 0, 0, 10, 1},
      {ITEM_HITRIGHT, CHAR_WORKHITRIGHT, 0, 0, 40, 1},
#endif
#ifdef _ITEMSET6_TXT
      {ITEM_NEGLECTGUARD, CHAR_WORKNEGLECTGUARD, 0, 0, 30, 1},
#endif
  };

  for (i = 0; i < arraysizeof(itemEffect); i++)
    itemEffect[i].accumulation = 0;

  if (!CHAR_CHECKINDEX(index))
    return;

#ifdef _ANGEL_SUMMON
  angelmode = CHAR_getWorkInt(index, CHAR_WORKANGELMODE);
#endif

  for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
#ifdef _ALLBLUES_LUA_1_9
    if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      if (EquipEffectFunction(index, i) == FALSE) {
        continue;
      }
    }
#endif
    int id = CHAR_getItemIndex(index, i);
    if (ITEM_CHECKINDEX(id)) {
      int j;
      int attrib = 0;
      char *arg = ITEM_getChar(id, ITEM_ARGUMENT);
#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
      if (i == CHAR_ARM)
        CHAR_sendStatusString(index, "S");
#endif
      for (j = 0; j < arraysizeof(itemEffect); j++) {
#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
        if (i == CHAR_EQSHIELD &&
            CHAR_getInt(index, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
          int item_type = ITEM_FIST;
          item_type = ITEM_getInt(id, ITEM_TYPE);
          // ������װ��������ʱ��
          if (item_type != ITEM_WSHIELD) {
            int k;
            // ��ʿְҵ���ܶ�����
            for (k = 0; k < CHAR_SKILLMAXHAVE; k++) {
              char *skill_name = "\0";
              // SKILL ID
              int skill_id = CHAR_getCharSkill(index, k);
              int Pskillid = PROFESSION_SKILL_getskillArray(skill_id);
              if (Pskillid <= 0)
                continue;
              // SKILL NAME
              skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
              if (skill_name == "\0")
                continue;

              if ((strcmp(skill_name, "PROFESSION_DUAL_WEAPON")) == 0) {
                int skill = k, skill_level = 0, rate = 0, value = 0;
                CHAR_HaveSkill *hskill;
                hskill = CHAR_getCharHaveSkill(index, skill);
                skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);
                rate = (skill_level * 3 + 20);
                value = ITEM_getInt(id, itemEffect[j].itemdataintindex) * rate /
                        100;
                itemEffect[j].accumulation += value;
              }
            }
          } else
            itemEffect[j].accumulation +=
                ITEM_getInt(id, itemEffect[j].itemdataintindex);
        } else
#endif
#ifdef _ITEM_PERCENTAGE
            if ((ITEM_MODIFYATTACK == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYDEFENCE == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYQUICK == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYHP == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYMP == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYLUCK == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYCHARM == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL) ||
                (ITEM_MODIFYAVOID == itemEffect[j].itemdataintindex &&
                 strstr(arg, "��") != NULL)) {

          itemEffect[j].accumulation +=
              CHAR_getWorkInt(index, itemEffect[j].charmodifyparamindex) *
              ITEM_getInt(id, itemEffect[j].itemdataintindex) / 100;
        } else
#endif
          itemEffect[j].accumulation +=
              ITEM_getInt(id, itemEffect[j].itemdataintindex);
      }

      attrib = ITEM_getInt(id, ITEM_MODIFYATTRIB);
      if (attrib > 0 && attrib < 5) {
        attribaccum[attrib - 1] += ITEM_getInt(id, ITEM_MODIFYATTRIBVALUE);
      }

#ifdef _ANGEL_SUMMON
      // if( !strcmp( ITEM_getChar( id, ITEM_USEFUNC), "ITEM_AngelToken") ) {
      if (ITEM_getInt(id, ITEM_ID) == ANGELITEM) {
        // print("Angel Euip:%d, %d.", id, ITEM_ID);
        angelequip = TRUE;
      }
#endif

#ifdef _ITEM_UPLEVEL
      int level = ITEM_getInt(id, ITEM_LEAKLEVEL);

      if (ITEM_getInt(id, ITEM_MODIFYATTACK) > 0) {
        CHAR_setWorkInt(index, CHAR_WORKFIXSTR,
                        CHAR_getWorkInt(index, CHAR_WORKFIXSTR) + level * 1);
      }
      if (ITEM_getInt(id, ITEM_MODIFYDEFENCE) > 0) {
        CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH,
                        CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) + level * 1);
      }
      if (ITEM_getInt(id, ITEM_MODIFYQUICK) > 0) {
        CHAR_setWorkInt(index, CHAR_WORKFIXDEX,
                        CHAR_getWorkInt(index, CHAR_WORKFIXDEX) + level * 1);
      }
      if (ITEM_getInt(id, ITEM_MODIFYHP) > 0) {
        CHAR_setWorkInt(index, CHAR_WORKMAXHP,
                        CHAR_getWorkInt(index, CHAR_WORKMAXHP) + level * 5);
      }
      if (ITEM_getInt(id, ITEM_MODIFYMP) > 0) {
        CHAR_setWorkInt(index, CHAR_WORKMAXMP,
                        CHAR_getWorkInt(index, CHAR_WORKMAXMP) + level * 2);
      }

      if (level == 20) {
        int armtype = ITEM_getInt(id, ITEM_TYPE);
        if (armtype == ITEM_FIST || armtype == ITEM_AXE ||
            armtype == ITEM_CLUB || armtype == ITEM_SPEAR ||
            armtype == ITEM_BOW || armtype == ITEM_BOOMERANG ||
            armtype == ITEM_BOUNDTHROW || armtype == ITEM_BREAKTHROW
#ifdef _PET_ITEM
            || armtype == ITEM_PET_HEAD || armtype == ITEM_PET_TOOTH ||
            armtype == ITEM_PET_CLAW || armtype == ITEM_PET_BREAST ||
            armtype == ITEM_PET_BACK || armtype == ITEM_PET_WING ||
            armtype == ITEM_PET_FEET
#endif
        ) {
          CHAR_setWorkInt(index, CHAR_WORKMAXMP,
                          CHAR_getWorkInt(index, CHAR_WORKMAXMP) + 20);
        } else if (armtype == ITEM_HELM || armtype == ITEM_ARMOUR) {
          CHAR_setWorkInt(index, CHAR_WORKMAXHP,
                          CHAR_getWorkInt(index, CHAR_WORKMAXHP) + 30);
        }
      }

#endif
    }
  }
#ifdef _PLAYER_EFFECT
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getWorkInt(index, CHAR_PLAYER_EFFECT) > 0) {
      char msg[256];
      sprintf(msg, "2|%d", CHAR_getWorkInt(index, CHAR_PLAYER_EFFECT));
      lssproto_CHAREFFECT_send(getfdFromCharaIndex(index), msg);
    }
  }
#endif
#ifdef FAMILY_MANOR_
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getWorkInt(index, CHAR_PLAYER_EFFECT_MANOR) > 0) {
      char msg[256];
      sprintf(msg, "3|%d", CHAR_getWorkInt(index, CHAR_PLAYER_EFFECT_MANOR));
      lssproto_CHAREFFECT_send(getfdFromCharaIndex(index), msg);
    }
  }
#endif
#ifdef _TRUMP_EQUIPMENT
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getInt(index, CHAR_TRUMP_EFFECT) > 0) {
      char msg[256];
      sprintf(msg, "5|%d", CHAR_getInt(index, CHAR_TRUMP_EFFECT));
      lssproto_CHAREFFECT_send(getfdFromCharaIndex(index), msg);
    }
  }
#endif

#ifdef _NEW_UPDATETITLE
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getInt(index, CHAR_TITLE_DEFAULT) > 0) {
      char msg[256];
      sprintf(msg, "4|%d", CHAR_getInt(index, CHAR_TITLE_DEFAULT));
      lssproto_CHAREFFECT_send(getfdFromCharaIndex(index), msg);
    }
  }
#endif

#ifdef _FAMILYBADGE_
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    extern int getFamilyBadge(int index);
    int badge = getFamilyBadge(index);
    if (badge) {
      char msg[256];
      sprintf(msg, "1|%d", badge);
      lssproto_CHAREFFECT_send(getfdFromCharaIndex(index), msg);
    }
  }
#endif

#ifdef _ANGEL_SUMMON
  if (angelmode == TRUE && angelequip == FALSE) {
    CHAR_setWorkInt(index, CHAR_WORKANGELMODE, FALSE);
    CHAR_sendAngelMark(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX), 0);
    print(" ж��ʹ������ ");
  }
  if (angelmode == FALSE && angelequip == TRUE) {
    CHAR_setWorkInt(index, CHAR_WORKANGELMODE, TRUE);
    CHAR_sendAngelMark(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX), 1);
    CHAR_talkToCli(index, -1, "���ܵ��˾��鱣�������ᱻ���˹�����", CHAR_COLORYELLOW);
    print(" װ��ʹ������ ");
  }
#endif

#ifdef _FIX_MAXCHARMP
  for (i = 3; i < arraysizeof(itemEffect); i++) {
#else
  for (i = 5; i < arraysizeof(itemEffect); i++) {
#endif
    int work = 0;
    if (itemEffect[i].type == 1) {
      work = itemEffect[i].accumulation;
    } else {
      work = CHAR_getWorkInt(index, itemEffect[i].charmodifyparamindex) +
             itemEffect[i].accumulation;
      work = min(itemEffect[i].max, work);
      work = max(itemEffect[i].min, work);
    }

    CHAR_setWorkInt(index, itemEffect[i].charmodifyparamindex, work);
  }

  for (i = 0; i < 4; i++) {
    CHAR_setWorkInt(index, CHAR_WORKFIXEARTHAT + i,
                    CHAR_getWorkInt(index, CHAR_WORKFIXEARTHAT + i) +
                        attribaccum[i]);
  }
  for (i = 0; i < 4; i++) {
    int j;
    for (j = 0; j < 4; j++) {
      if (i != j) {
        CHAR_setWorkInt(index, CHAR_WORKFIXEARTHAT + j,
                        CHAR_getWorkInt(index, CHAR_WORKFIXEARTHAT + j) -
                            attribaccum[i]);
      }
    }
  }
  for (i = 0; i < 4; i++) {
    if (CHAR_getWorkInt(index, CHAR_WORKFIXEARTHAT + i) > CHAR_MAXATTRIB) {
      CHAR_setWorkInt(index, CHAR_WORKFIXEARTHAT + i, CHAR_MAXATTRIB);
    }
  }

  {
    int work;
#ifdef _BT_ITEM
    work = CHAR_getWorkInt(index, CHAR_WORKFIXSTR) +
           itemEffect[0].accumulation * getBtItem();
#else
    work = CHAR_getWorkInt(index, CHAR_WORKFIXSTR) + itemEffect[0].accumulation;
#endif
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR, max(0, work));
#ifdef _BT_ITEM
    work = CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) +
           itemEffect[1].accumulation * getBtItem();
#else
    work =
        CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) + itemEffect[1].accumulation;
#endif
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH, max(-100, work));
#ifdef _BT_ITEM
    work = CHAR_getWorkInt(index, CHAR_WORKFIXDEX) +
           itemEffect[2].accumulation * getBtItem();
#else
    work = CHAR_getWorkInt(index, CHAR_WORKFIXDEX) + itemEffect[2].accumulation;
#endif
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX, max(-100, work));
  }
}

void Other_DefcharWorkInt(int index) {

#ifdef _SUIT_ITEM
  int mfix, mtgh, mdex, mods, maxhp;
  mfix = CHAR_getWorkInt(index, CHAR_WORKFIXSTR);
  mtgh = CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH);
  mdex = CHAR_getWorkInt(index, CHAR_WORKFIXDEX);
  maxhp = CHAR_getWorkInt(index, CHAR_WORKMAXHP);

  mods = CHAR_getWorkInt(index, CHAR_WORKSUITMODSTR);
  mfix = mfix + ((mfix * mods) / 100);

  CHAR_setWorkInt(index, CHAR_WORKFIXSTR,
                  mfix + CHAR_getWorkInt(index, CHAR_WORKSUITSTR));
  CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH,
                  mtgh + CHAR_getWorkInt(index, CHAR_WORKSUITTGH));
  CHAR_setWorkInt(index, CHAR_WORKFIXDEX,
                  mdex + CHAR_getWorkInt(index, CHAR_WORKSUITDEX));
  CHAR_setWorkInt(index, CHAR_WORKMAXHP,
                  maxhp + CHAR_getWorkInt(index, CHAR_WORKSUITVIT));

#ifdef _SUIT_ADDPART3
  if (mfix > 0)
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR,
                    CHAR_getWorkInt(index, CHAR_WORKFIXSTR) +
                        mfix * CHAR_getWorkInt(index, CHAR_WORKSUITSTR_P) /
                            100.0);
  if (mtgh > 0)
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH,
                    CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) +
                        mtgh * CHAR_getWorkInt(index, CHAR_WORKSUITTGH_P) /
                            100.0);
  if (mdex > 0)
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX,
                    CHAR_getWorkInt(index, CHAR_WORKFIXDEX) +
                        mdex * CHAR_getWorkInt(index, CHAR_WORKSUITDEX_P) /
                            100.0);
#endif
#endif //_SUIT_ITEM

#ifdef _MAGIC_RESIST_EQUIT // WON ADD ְҵ����װ��
                           /*{
                                   int f_res = -1, i_res = -1, t_res = -1;
                                   f_res = CHAR_getWorkInt( index, CHAR_WORK_F_RESIST );
                                   i_res = CHAR_getWorkInt( index, CHAR_WORK_I_RESIST );
                                   t_res = CHAR_getWorkInt( index, CHAR_WORK_T_RESIST );
                       
                                   CHAR_setWorkInt( index, CHAR_WORK_F_RESIST, f_res+CHAR_getWorkInt(
                           index, CHAR_WORK_F_SUIT ) );                        CHAR_setWorkInt( index,
                           CHAR_WORK_I_RESIST,                        i_res+CHAR_getWorkInt( index, CHAR_WORK_I_SUIT ) );
                           CHAR_setWorkInt( index,                        CHAR_WORK_T_RESIST, t_res+CHAR_getWorkInt( index,
                           CHAR_WORK_T_SUIT ) );
                           }*/
#endif

#ifdef _PETSKILL_SETDUCK
    // profession fix
    // ʹ�ûر���ʽʱ,�Ὣ��ֵ��ȥ30%Ȼ���趨�ɻر�ֵ,��ֵ��趨,Ҳ���ԭ���趨�Ļر�ֵ��û��һ��,�����(Change)�Ȱ������õ�
    /*if( CHAR_getWorkInt( index, CHAR_MYSKILLDUCK) > 0 ){
            int mtgh = CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH);
            mtgh -= (mtgh*30)/100;
            if( mtgh < 0 ) mtgh=0;
            CHAR_setWorkInt( index, CHAR_MYSKILLDUCKPOWER, mtgh);
    }*/
#endif
#ifdef _MAGICPET_SKILL
  if (CHAR_getWorkInt(index, CHAR_MYSKILLSTR) > 0) {
    int mpower, mdef;
    mpower = CHAR_getWorkInt(index, CHAR_WORKFIXSTR);
    mdef = CHAR_getWorkInt(index, CHAR_MYSKILLSTRPOWER);
    mpower += (mtgh * mdef) / 100;
    if (mtgh < 0)
      mtgh = 0;
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR, mpower);
  }
  if (CHAR_getWorkInt(index, CHAR_MYSKILLTGH) > 0) {
    int mpower, mdef;
    mpower = CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH);
    mdef = CHAR_getWorkInt(index, CHAR_MYSKILLTGHPOWER);
    mpower += (mtgh * mdef) / 100;
    if (mtgh < 0)
      mtgh = 0;
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH, mpower);
  }
  if (CHAR_getWorkInt(index, CHAR_MYSKILLDEX) > 0) {
    int mpower, mdef;
    mpower = CHAR_getWorkInt(index, CHAR_WORKFIXDEX);
    mdef = CHAR_getWorkInt(index, CHAR_MYSKILLDEXPOWER);
    mpower += (mtgh * mdef) / 100;
    if (mtgh < 0)
      mtgh = 0;
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX, mpower);
  }
#endif

#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
  if (CHAR_getWorkInt(index, CHAR_MYSKILLHIT) > 0) {
    int mpower, mdef;
    mpower = CHAR_getWorkInt(index, CHAR_MYSKILLHIT);
    mdef = CHAR_getWorkInt(index, CHAR_WORKHITRIGHT);
    mpower += (mtgh * mdef) / 100;
    if (mtgh < 0)
      mtgh = 0;
    CHAR_setWorkInt(index, CHAR_MYSKILLHIT, mpower);
  }
  if (CHAR_getWorkInt(index, CHAR_WORK_WEAPON) > 0) { // ����ר��
    int mpower, mdef;
    mpower = CHAR_getWorkInt(index, CHAR_WORKFIXSTR);
    mdef = CHAR_getWorkInt(index, CHAR_WORKMOD_WEAPON);
    mpower = mpower * (100 + mdef) / 100;
    if (mtgh < 0)
      mtgh = 0;
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR, mpower);
  }
#endif

#ifdef _VARY_WOLF
  if (CHAR_getInt(index, CHAR_BASEIMAGENUMBER) == 101428
#ifdef _EXPANSION_VARY_WOLF
      || CHAR_getInt(index, CHAR_BASEIMAGENUMBER) == 104109
#endif
  ) {
    int Rands = 0, power;
    Rands = CHAR_getWorkInt(index, CHAR_SKILLSTRPOWER);
    power = CHAR_getWorkInt(index, CHAR_WORKFIXSTR);
    power = power + (power * Rands) / 100;
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR, power);

    Rands = CHAR_getWorkInt(index, CHAR_SKILLTGHPOWER);
    power = CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH);
    power = power + (power * Rands) / 100;
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH, power);

    Rands = CHAR_getWorkInt(index, CHAR_SKILLDEXPOWER);
    power = CHAR_getWorkInt(index, CHAR_WORKFIXDEX);
    power = power + (power * Rands) / 100;
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX, power);
  }
#endif

#ifdef _PROFESSION_ADDSKILL
  if (CHAR_getWorkInt(index, CHAR_WORKFEAR) > 0) {
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR,
                    CHAR_getWorkInt(index, CHAR_WORKFIXSTR) - mfix * 0.1);
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH,
                    CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) - mtgh * 0.1);
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX,
                    CHAR_getWorkInt(index, CHAR_WORKFIXDEX) - mdex * 0.2);
  }
#endif

#ifdef _MAGIC_WEAKEN
  if (CHAR_getWorkInt(index, CHAR_WORKWEAKEN) > 0) {
    CHAR_setWorkInt(index, CHAR_WORKFIXSTR,
                    CHAR_getWorkInt(index, CHAR_WORKFIXSTR) * 0.8);
    CHAR_setWorkInt(index, CHAR_WORKFIXTOUGH,
                    CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH) * 0.8);
    CHAR_setWorkInt(index, CHAR_WORKFIXDEX,
                    CHAR_getWorkInt(index, CHAR_WORKFIXDEX) * 0.8);
    CHAR_setWorkInt(index, CHAR_WORKWEAKEN,
                    CHAR_getWorkInt(index, CHAR_WORKWEAKEN) - 1);
  }
#endif
#ifdef _MAGIC_BARRIER // vincent  ����:ħ��
  if (CHAR_getWorkInt(index, CHAR_WORKBARRIER) > 0) {
    CHAR_setWorkInt(index, CHAR_WORKBARRIER,
                    CHAR_getWorkInt(index, CHAR_WORKBARRIER) - 1);
  }
#endif

  CHAR_setWorkInt(index, CHAR_WORKATTACKPOWER,
                  CHAR_getWorkInt(index, CHAR_WORKFIXSTR));
  CHAR_setWorkInt(index, CHAR_WORKDEFENCEPOWER,
                  CHAR_getWorkInt(index, CHAR_WORKFIXTOUGH));
  CHAR_setWorkInt(index, CHAR_WORKQUICK,
                  CHAR_getWorkInt(index, CHAR_WORKFIXDEX));
}

char *ITEM_getAppropriateName(int item_index) {
  int nameindex;
  if (!ITEM_CHECKINDEX(item_index))
    return "\0";
  if (ITEM_getInt(item_index, ITEM_LEAKLEVEL) <= 0)
    nameindex = ITEM_NAME;
  else
    nameindex = ITEM_SECRETNAME;
  return ITEM_getChar(item_index, nameindex);
}

char *ITEM_getEffectString(int item_index) {
  if (!ITEM_CHECKINDEX(item_index))
    return "\0";
  return ITEM_gExists[item_index].item.string[ITEM_EFFECTSTRING].string;
}

char *ITEM_getItemInfoFromNumber(int itemid) {
  return ITEMTBL_getChar(itemid, ITEM_EFFECTSTRING);
}

INLINE char *_ITEM_getArgumentString(char *file, int line, int itemid) {
  if (ITEM_CHECKITEMTABLE(itemid) == FALSE) {
    print("Can't Get ItemTable for:%d! - %s:%d\n", itemid, file, line);
    return "\0";
  }
  return ITEMTBL_getChar(itemid, ITEM_ARGUMENT);
}

INLINE char *_ITEM_getNameFromNumber(char *file, int line, int itemid) {
  if (ITEM_CHECKITEMTABLE(itemid) == FALSE) {
    print("Can't Get ItemTable for:%d! - %s:%d\n", itemid, file, line);
    return "\0";
  }
  return ITEMTBL_getChar(itemid, ITEM_NAME);
}

int ITEM_getcostFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_COST);
}

int ITEM_getlevelFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_LEVEL);
}

int ITEM_getgraNoFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_BASEIMAGENUMBER);
}

int ITEM_getcanpetmailFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_CANPETMAIL);
}

int ITEM_getvanishatdropFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_VANISHATDROP);
}

int ITEM_getdropatlogoutFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_DROPATLOGOUT);
}

int ITEM_getmergeItemFromFromITEMtabl(int itemid) {
  return ITEMTBL_getInt(itemid, ITEM_CANMERGEFROM);
}

BOOL ITEM_canuseMagic(int item_index) {
  if (!ITEM_CHECKINDEX(item_index))
    return FALSE;
  if (MAGIC_getMagicArray(ITEM_gExists[item_index].item.data[ITEM_MAGICID]) != -1) {
    return TRUE;
  }
  return FALSE;
}

INLINE ITEM_Item *ITEM_getItemPointer(int index) {
  if (!ITEM_CHECKINDEX(index))
    return NULL;
  return &ITEM_gExists[index].item;
}

int ITEM_isTargetValid(int charaindex, int item_index, int toindex) {
  int itemtarget;
  int Myside;
  itemtarget = ITEM_getInt(item_index, ITEM_TARGET);

  Myside = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);
  if ((toindex >= 0x0) && (toindex <= 0x13))
    return 0;

  if (toindex == 0x14) { // 20 ����ȫ 0
    if (itemtarget == ITEM_TARGET_ALLMYSIDE) {
      if (Myside == 0)
        return 0;
    } else if (itemtarget == ITEM_TARGET_ALLOTHERSIDE) {
      if (Myside == 1)
        return 0;
    }
  } else if (toindex == 0x15) { // 21 ����ȫ 1
    if (itemtarget == ITEM_TARGET_ALLMYSIDE) {
      if (Myside == 1)
        return 0;
    } else if (itemtarget == ITEM_TARGET_ALLOTHERSIDE) {
      if (Myside == 0)
        return 0;
    }
  } else if (toindex == 0x16) {
    if (itemtarget == ITEM_TARGET_ALL)
      return 0;
  }
  return -1;
}
// Nuke end

#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares(int charaindex, int flg) {
  int item_index, i;
  char token[256];

  for (i = 0; i < CheckCharMaxItem(charaindex); i++) {
    item_index = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(item_index))
      continue;
    if (ITEM_getInt(item_index, ITEM_TYPE) == ITEM_WARES) {
      if (flg == 0) {
        return FALSE;
      }
      sprintf(token, "����%s", ITEM_getChar(item_index, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

      CHAR_setItemIndex(charaindex, i, -1);
      ITEM_endExistItemsOne(item_index);
      CHAR_sendItemDataOne(charaindex, i);
    }
  }
  return TRUE;
}
#endif

int ITEMTBL_getInt(int ItemID, ITEM_DATA_ENUM datatype) {
  if (datatype >= ITEM_DATA_ENUM_MAX || datatype < 0)
    return -1;
  if (ITEM_CHECKITEMTABLE(ItemID) == FALSE)
    return -1;
  return ITEM_gTable[ITEM_gIndex[ItemID].index].item.data[datatype]; // new
}

char *ITEMTBL_getChar(int ItemID, ITEM_CHAR_DATA_ENUM datatype) {
  if (datatype >= ITEM_CHAR_DATA_ENUM_MAX || datatype < 0)
    return "\0";
  if (ITEM_CHECKITEMTABLE(ItemID) == FALSE)
    return "\0";
  return ITEM_gTable[ITEM_gIndex[ItemID].index].item.string[datatype].string; // new
}

INLINE BOOL ITEM_CHECKITEMTABLE(int number) {
  if (number < 0 || number >= ITEM_sIndexLen) {
    // andy_log
    print("ITEM_CHECKITEMTABLE() number:%d ITEM_sTableLen:%d !!\n", number,
          ITEM_sIndexLen);
    return FALSE;
  }
  return ITEM_gIndex[number].use; // new
}
#ifdef _TAKE_ITEMDAMAGE
int ITEM_getItemDamageCrusheED(int item_index) {
  int crushe = ITEM_getInt(item_index, ITEM_DAMAGECRUSHE);
  int maxcrushe = ITEM_getInt(item_index, ITEM_MAXDAMAGECRUSHE);
  if (maxcrushe < 1)
    return -1;
  if (maxcrushe > 1000000)
    maxcrushe = 1000000;
  if (crushe > 1000000)
    crushe = 1000000;

  return ((int)(crushe * 100) / maxcrushe);
}
#endif
void ITEM_RsetEquit(int charaindex) {
  int i, item_index, ti = -1;
  for (i = 0; i < CHAR_STARTITEMARRAY; i++) {
    item_index = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(item_index))
      continue;
    if (i == ITEM_getEquipPlace(charaindex, item_index))
      continue;
    if ((ti = CHAR_findEmptyItemBox(charaindex)) == -1)
      return;
    CHAR_setItemIndex(charaindex, i, -1);
    CHAR_setItemIndex(charaindex, ti, item_index);
  }
}

void ITEM_reChangeItemToPile(int item_index) {
  int item_id;
  if (!ITEM_CHECKINDEX(item_index))
    return;

  if (ITEM_getInt(item_index, ITEM_USEPILENUMS) <= 0)
    ITEM_setInt(item_index, ITEM_USEPILENUMS, 1);
  item_id = ITEM_getInt(item_index, ITEM_ID);
  if (!ITEM_CHECKITEMTABLE(item_id))
    return;
  if (item_id == 20284)
    return;
  if (ITEM_getInt(item_index, ITEM_CANBEPILE) !=
      ITEMTBL_getInt(item_id, ITEM_CANBEPILE))
    ITEM_setInt(item_index, ITEM_CANBEPILE,
                ITEMTBL_getInt(item_id, ITEM_CANBEPILE));
}

void ITEM_reChangeItemName(int item_index) {
}

#ifdef _PET_ITEM
char *ITEM_petmakeItemStatusString(int petindex, int itemid) {
  char escapeeffectstring[256];
  char buff1[256];

  int itemcolor = 0;

  int item_index = CHAR_getItemIndex(petindex, itemid);

  if (!ITEM_CHECKINDEX(item_index)) {
    snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
             "%d||||0|0|", itemid);

    return ITEM_itemStatusStringBuffer;
  }

  makeEscapeString(ITEM_getChar(item_index, ITEM_EFFECTSTRING),
                   escapeeffectstring, sizeof(escapeeffectstring));
#ifdef _ITEM_COLOER
  itemcolor = ITEM_getInt(item_index, ITEM_COLOER);
#else
  itemcolor = CHAR_COLORWHITE;
#endif
#ifndef _PET_AND_ITEM_UP
  if (strlen(ITEM_getChar(item_index, ITEM_CDKEY)) != 0) {
    itemcolor = CHAR_COLORGREEN;
  } else if (ITEM_getInt(item_index, ITEM_MERGEFLG)) {
    itemcolor = CHAR_COLORYELLOW;
  }
#endif
  int crushe = ITEM_getInt(item_index, ITEM_DAMAGECRUSHE);
  int maxcrushe = ITEM_getInt(item_index, ITEM_MAXDAMAGECRUSHE);
  if (maxcrushe < 1) {
    sprintf(buff1, "������");
  } else {
    maxcrushe = maxcrushe / 1000;
    crushe = crushe / 1000;
    if (maxcrushe <= 0)
      maxcrushe = 1;
    snprintf(buff1, sizeof(buff1), "%d%%", (int)((crushe * 100) / maxcrushe));
  }

  snprintf(ITEM_itemStatusStringBuffer, sizeof(ITEM_itemStatusStringBuffer),
           "%d|%s|%s|%s|%d|%d|", itemid, ITEM_getChar(item_index, ITEM_NAME),
           escapeeffectstring, buff1, itemcolor,
           ITEM_getInt(item_index, ITEM_BASEIMAGENUMBER));

  return ITEM_itemStatusStringBuffer;
}
#endif
