#include "version.h"
#include <limits.h>
#include "common.h"
#include "buf.h"
#include "char.h"
#include "char_data.h"
#include "handletime.h"
#include "item.h"
#include "map_deal.h"
#include "npccreate.h"
#include "npctemplate.h"
#include "object.h"
#include "readmap.h"
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif
typedef struct tagNPC_searchPoint {
  int floor;
  int x;
  int y;
} NPC_searchPoint;

#define NPC_CREATECHALLENGETIME 1
int all_nosee = 0;
int all_nobody = 0;    /* ㄠ及凛｛蝈化及衬毛 no_body 卞 */
int one_loop_born = 1; /* 每个LOOP生成NPC的数量限制 */

typedef enum {
  NPC_GEN_OK = 0,
  NPC_GEN_INVALID_CONFIG,
  NPC_GEN_NO_POSITION,
  NPC_GEN_DEFAULT_CHAR_FAILED,
  NPC_GEN_CHAR_ARRAY_FULL,
  NPC_GEN_OBJECT_ARRAY_FULL
} NPC_GenerateResult;

static NPC_GenerateResult npc_generate_result = NPC_GEN_OK;

/*------------------------------------------------------------
 * 衬毛丹心分允桦赭毛瑁烂允月
 * 娄醒
 *  nobody  int
 * 忒曰袄
 *  综木月凛反  TRUE
 *  综木卅中凛反  FALSE
 ------------------------------------------------------------*/
static BOOL NPC_searchCreatePoint(NPC_Create *cr, int nobody, int nosee,
                                  NPC_searchPoint *point, BOOL isflying) {
  int floor = cr->intdata[NPC_CREATEFLOORID];
  int x = cr->intdata[NPC_CREATEBORNLEFTUPX];
  int y = cr->intdata[NPC_CREATEBORNLEFTUPY];
  int width;
  int height;
  int area;
  int loop;
  BOOL ret = FALSE;
  int i, j;

  width = cr->intdata[NPC_CREATEBORNRIGHTDOWNX] -
          cr->intdata[NPC_CREATEBORNLEFTUPX] + 1;
  height = cr->intdata[NPC_CREATEBORNRIGHTDOWNY] -
           cr->intdata[NPC_CREATEBORNLEFTUPY] + 1;
  if (width <= 0 || height <= 0 || width > INT_MAX / height) {
    printEx("Invalid NPC create area: floor=%d x=%d y=%d width=%d height=%d\n",
           floor, x, y, width, height);
    return FALSE;
  }
  area = width * height;
  if (nobody == 0 && all_nobody == 0) {
    BOOL found = FALSE;
    for (i = x; i < x + width && found == FALSE; i++) {
      for (j = y; j < y + height && found == FALSE; j++) {
        OBJECT object;
        for (object = MAP_getTopObj(floor, i, j); object;
             object = NEXT_OBJECT(object)) {
          int objindex = GET_OBJINDEX(object);
          if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
            if (CHAR_getInt(OBJECT_getIndex(objindex), CHAR_WHICHTYPE) ==
                CHAR_TYPEPLAYER) {
              found = TRUE;
              break;
            }
          }
        }
      }
    }
    if (found == FALSE) {
      return (FALSE);
    }
  }
  for (loop = 0; loop < NPC_CREATECHALLENGETIME; loop++) {
    int startpoint;
    int crx;
    int cry;

    startpoint = RAND(0, area - 1);
    crx = startpoint % width + x;
    cry = startpoint / width + y;

    if (cr->intdata[NPC_CREATEIGNOREINVINCIBLE] == 0 &&
        CHAR_isInvincibleArea(floor, crx, cry)) {
      continue;
    }
    if (nosee == 0 && all_nosee == 0) {
      BOOL found = FALSE;
      for (i = crx - CHAR_DEFAULTSEESIZ / 2;
           i <= crx + CHAR_DEFAULTSEESIZ / 2 && found == FALSE; i++) {
        for (j = cry - CHAR_DEFAULTSEESIZ / 2;
             j <= cry + CHAR_DEFAULTSEESIZ / 2 && found == FALSE; j++) {
          OBJECT object;
          for (object = MAP_getTopObj(floor, i, j); object;
               object = NEXT_OBJECT(object)) {
            int objindex = GET_OBJINDEX(object);
            if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
              if (CHAR_getInt(OBJECT_getIndex(objindex), CHAR_WHICHTYPE) ==
                  CHAR_TYPEPLAYER) {
                found = TRUE;
                break;
              }
            }
          }
        }
      }
      if (found == TRUE) {
        return FALSE;
      }
    }
    if (MAP_walkAbleFromPoint(floor, crx, cry, isflying) == TRUE) {
      point->floor = cr->intdata[NPC_CREATEFLOORID];
      point->x = crx;
      point->y = cry;
      ret = TRUE;
      break;
    } else {
    }
  }

  return (ret);
}

typedef struct tagNPC_Correspondfunction {
  int template;
  int ch;
} NPC_Correspondfunction;

static NPC_Correspondfunction correspondfunction[] = {
    {NPC_TEMPLATEINITFUNC, CHAR_INITFUNC},
    {NPC_TEMPLATEWALKPREFUNC, CHAR_WALKPREFUNC},
    {NPC_TEMPLATEWALKPOSTFUNC, CHAR_WALKPOSTFUNC},
    {NPC_TEMPLATEPREOVERFUNC, CHAR_PREOVERFUNC},
    {NPC_TEMPLATEPOSTOVERFUNC, CHAR_POSTOVERFUNC},
    {NPC_TEMPLATEWATCHFUNC, CHAR_WATCHFUNC},
    {NPC_TEMPLATELOOPFUNC, CHAR_LOOPFUNC},
    {NPC_TEMPLATETALKEDFUNC, CHAR_TALKEDFUNC},
    {NPC_TEMPLATEDYINGFUNC, CHAR_DYINGFUNC},
    {NPC_TEMPLATEPREATTACKEDFUNC, CHAR_PREATTACKEDFUNC},
    {NPC_TEMPLATEPOSTATTACKEDFUNC, CHAR_POSTATTACKEDFUNC},
    {NPC_TEMPLATEOFFFUNC, CHAR_OFFFUNC},
    {NPC_TEMPLATELOOKEDFUNC, CHAR_LOOKEDFUNC},
    {NPC_TEMPLATEITEMPUTFUNC, CHAR_ITEMPUTFUNC},
    {NPC_TEMPLATESPECIALTALKEDFUNC, CHAR_SPECIALTALKEDFUNC},
    {NPC_TEMPLATEWINDOWTALKEDFUNC, CHAR_WINDOWTALKEDFUNC},
#ifdef _USER_CHARLOOPS
    {NPC_TEMPLATELOOPFUNCTEMP1, CHAR_LOOPFUNCTEMP1},
    {NPC_TEMPLATELOOPFUNCTEMP2, CHAR_LOOPFUNCTEMP2},
    {NPC_TEMPLATEBATTLEPROPERTY, CHAR_BATTLEPROPERTY},
#endif
};

/*------------------------------------------------------------
 * 楮醒戊疋□允月
 * 参数
 *  Char *ch, NPC_Template *temp
 * 返回值
 *  无
 ------------------------------------------------------------*/
static void NPC_copyFunction(Char *ch, NPC_Template *temp) {
  int i;
  for (i = 0; i < arraysizeof(correspondfunction); i++)
    if (temp->chardata[correspondfunction[i].template].string[0] != '\0')
      strncpysafe(ch->charfunctable[correspondfunction[i].ch].string,
                 sizeof(ch->charfunctable[correspondfunction[i].ch].string),
                 temp->chardata[correspondfunction[i].template].string);
}

static BOOL NPC_generateNPC(int createindex, int createtemplateindex) {
  Char one;
  NPC_searchPoint sp;
  int i;
  NPC_Template *template;
  NPC_Create *cr;

  npc_generate_result = NPC_GEN_OK;
  if (NPC_CHECKCREATEINDEX(createindex) && 0 <= createtemplateindex &&
      createtemplateindex <
          arraysizeof(NPC_create[createindex].templateindex) &&
      NPC_CHECKTEMPLATEINDEX(
          NPC_create[createindex].templateindex[createtemplateindex])) {
    cr = &NPC_create[createindex];
    template = &NPC_template[NPC_create[createindex]
                                 .templateindex[createtemplateindex]];
  } else {
    npc_generate_result = NPC_GEN_INVALID_CONFIG;
    return FALSE;
  }
  if (NPC_searchCreatePoint(cr, template->intdata[NPC_TEMPLATEMAKEATNOBODY],
                            template->intdata[NPC_TEMPLATEMAKEATNOSEE], &sp,
                            template->intdata[NPC_TEMPLATEISFLYING]) == FALSE) {
    npc_generate_result = NPC_GEN_NO_POSITION;
    return FALSE;
  }

  if (CHAR_getDefaultChar(&one, template->intdata[NPC_TEMPLATETYPE]) == FALSE) {
    npc_generate_result = NPC_GEN_DEFAULT_CHAR_FAILED;
    return FALSE;
  }
  one.data[CHAR_FLOOR] = sp.floor;
  one.data[CHAR_X] = sp.x;
  one.data[CHAR_Y] = sp.y;
  one.data[CHAR_DIR] = cr->intdata[NPC_CREATEDIR];
#ifdef _ADD_ACTION
  one.data[CHAR_ACTIONSTYLE] = cr->intdata[NPC_CREATEACTION];
#endif

  one.data[CHAR_BASEBASEIMAGENUMBER] = one.data[CHAR_BASEIMAGENUMBER] =
      template->intdata[NPC_TEMPLATEIMAGENUMBER];
  if (cr->intdata[NPC_CREATEBASEIMAGENUMBER] != -1)
    one.data[CHAR_BASEBASEIMAGENUMBER] = one.data[CHAR_BASEIMAGENUMBER] =
        cr->intdata[NPC_CREATEBASEIMAGENUMBER];

  strncpysafe(one.string[CHAR_NAME].string,
              sizeof(one.string[CHAR_NAME].string),
              template->chardata[NPC_TEMPLATECHARNAME].string);

  if (cr->chardata[NPC_CREATENAME].string[0] != '\0') {
    strncpysafe(one.string[CHAR_NAME].string,
                sizeof(one.string[CHAR_NAME].string),
                cr->chardata[NPC_CREATENAME].string);
  }
  // Robin 0731
  one.data[CHAR_FMINDEX] = cr->intdata[NPC_CREATEFAMILY];
  NPC_copyFunctionSetToChar(template->intdata[NPC_TEMPLATEFUNCTIONINDEX], &one);
  NPC_copyFunction(&one, template);
  one.data[CHAR_LOOPINTERVAL] = template->intdata[NPC_TEMPLATELOOPFUNCTIME];
  if (cr->arg[createtemplateindex].string[0] != '\0') {
    strncpysafe(one.string[CHAR_NPCARGUMENT].string,
               sizeof(one.string[CHAR_NPCARGUMENT].string),
               cr->arg[createtemplateindex].string);
  } else {
    one.string[CHAR_NPCARGUMENT].string[0] = '\0';
  }
  one.data[CHAR_WHICHTYPE] = CHAR_TYPEENEMY;
  one.data[CHAR_NPCCREATEINDEX] = createindex;
  {
    static struct NPC_Paramtable {
      int templateindex;
      int charadataindex;
    } paramtbl[] = {
        {NPC_TEMPLATEMINMP, CHAR_MAXMP},
        {NPC_TEMPLATEMINSTR, CHAR_STR},
        {NPC_TEMPLATEMINTOUGH, CHAR_TOUGH},
    };
    for (i = 0; i < arraysizeof(paramtbl); i++) {
      int randomvalue;
      randomvalue = RAND(0, template->randomdata[paramtbl[i].templateindex]);
      one.data[paramtbl[i].charadataindex] =
          one.data[paramtbl[i].charadataindex] +
          template->intdata[paramtbl[i].templateindex] + randomvalue;
    }
    one.data[CHAR_MP] = one.data[CHAR_MAXMP];
  }
  {
    extern int CheckCharMaxItemChar(Char * ch);
    int item_index = CHAR_STARTITEMARRAY;

    int itemMax = CheckCharMaxItemChar(&one);
    for (i = 0; i < template->intdata[NPC_TEMPLATEITEMNUM]; i++) {
      if (RAND(0, 32767) <= template->haveitem[i].haverate) {
        if (template->haveitem[i].itemnumber == -1 &&
            template->haveitem[i].havenum > 0) {
          int randomvalue = template->haveitem[i].havenum;
          randomvalue *= (RAND(90, 110)) / 100.0;
          one.data[CHAR_GOLD] = min(CHAR_MAXGOLDHAVE, randomvalue);
        } else if (item_index + i < itemMax) {
          one.indexOfExistItems[item_index + i] =
              ITEM_makeItemAndRegist(template->haveitem[i].itemnumber);
        }
      }
    }
  }
  {
    int char_index;
    int objindex;
    Object obj;
    char_index = CHAR_initCharOneArray(&one);
    if (char_index == -1) {
      npc_generate_result = NPC_GEN_CHAR_ARRAY_FULL;
      return FALSE;
    }
    if (template->intdata[NPC_TEMPLATEISFLYING])
      CHAR_setFlg(char_index, CHAR_ISFLYING, 1);
    obj.type = OBJTYPE_CHARA;
    obj.index = char_index;
    obj.x = CHAR_getInt(char_index, CHAR_X);
    obj.y = CHAR_getInt(char_index, CHAR_Y);
    obj.floor = CHAR_getInt(char_index, CHAR_FLOOR);
    objindex = initObjectOne(&obj);
    if (objindex == -1) {
      npc_generate_result = NPC_GEN_OBJECT_ARRAY_FULL;
      CHAR_endCharOneArray(char_index);
      return FALSE;
    } else {
    }
    CHAR_setWorkInt(char_index, CHAR_WORKOBJINDEX, objindex);
#ifdef _MO_RELOAD_NPC
    CHAR_setWorkInt(char_index, CHAR_WORKNPCTYPE, 1);
#endif
#ifdef _ADD_ACTION
    // CHAR_WORKACTION
    CHAR_setWorkInt(char_index, CHAR_WORKACTION,
                    CHAR_getInt(char_index, CHAR_ACTIONSTYLE));

    CHAR_sendWatchEvent(objindex, CHAR_getWorkInt(char_index, CHAR_WORKACTION),
                        NULL, 0, TRUE);
#else
    CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
#endif
    CHAR_complianceParameter(char_index);
  }
  return TRUE;
}

void NPC_generateLoop(BOOL checkall) {
  int i, j;
  int npcCreated = 0;
  int templateAttempts = 0;
  int timeRejected = 0;
  int invalidEnemyCount = 0;
  int failureCounts[NPC_GEN_OBJECT_ARRAY_FULL + 1] = {0};
  static int npcCreateIndex = 0;
  static struct timeval lastNpcGenerationTime;
  if (checkall == FALSE) {
    if (time_diff_us(NowTime, lastNpcGenerationTime) < (1000 * 1000)) {
      return;
    } else {
      lastNpcGenerationTime = NowTime;
    }
  }
  if (NPC_create == NULL || NPC_createnum <= 0) {
    printEx("NPC NPC_create is null or create_num:%d.\n", NPC_createnum);
    return;
  }
  for (i = 0; i < NPC_createnum; i++) {
    if (!NPC_CHECKCREATEINDEX(npcCreateIndex)) {
      npcCreateIndex = 0;
    }
    const int cEnemyNpcNum = NPC_getCreateInt(npcCreateIndex, NPC_CREATEENEMYNUM);
    if (cEnemyNpcNum < 0 ||
        cEnemyNpcNum > arraysizeof(NPC_create[npcCreateIndex].templateindex)) {
      printEx("Invalid enemy count: npcCreateIndex=%d count=%d max=%d\n",
            npcCreateIndex, cEnemyNpcNum,
            arraysizeof(NPC_create[npcCreateIndex].templateindex));
      invalidEnemyCount++;
      // 这个index的NPC没有属于敌人的范畴
      ++npcCreateIndex;
      continue;
    }
    for (j = 0; j < cEnemyNpcNum; j++) {
      templateAttempts++;
      if (NPC_createCheckGenerateFromTime(npcCreateIndex) == TRUE) {
        if (NPC_generateNPC(npcCreateIndex, j) == TRUE) {
          ++npcCreated;
          NPC_createInitTime(npcCreateIndex);
          NPC_createIncreaseEnemynum(npcCreateIndex);
        } else if (npc_generate_result >= NPC_GEN_INVALID_CONFIG &&
                   npc_generate_result <= NPC_GEN_OBJECT_ARRAY_FULL) {
          failureCounts[npc_generate_result]++;
        }
      } else {
        timeRejected++;
      }
    }
    ++npcCreateIndex;
    if (checkall == FALSE && npcCreated >= one_loop_born) {
      break;
    }
  }
  if (checkall == TRUE) {
    print("[NPC_GEN_TRACE] rules=%d template_attempts=%d generated=%d "
          "time_rejected=%d invalid_enemy_count=%d invalid_config=%d "
          "no_position=%d default_char_failed=%d char_array_full=%d "
          "object_array_full=%d\n",
          NPC_createnum, templateAttempts, npcCreated, timeRejected,
          invalidEnemyCount, failureCounts[NPC_GEN_INVALID_CONFIG],
          failureCounts[NPC_GEN_NO_POSITION],
          failureCounts[NPC_GEN_DEFAULT_CHAR_FAILED],
          failureCounts[NPC_GEN_CHAR_ARRAY_FULL],
          failureCounts[NPC_GEN_OBJECT_ARRAY_FULL]);
  }
}
