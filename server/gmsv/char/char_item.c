#include "version.h"
//
#include "gmsv_server.h"
//
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "handletime.h"
#include "item.h"
#include "item_event.h"
#include "log.h"
#include "map_deal.h"
#include "object.h"
#include "petmail.h"
#include "readmap.h"
#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif
extern int itemLine;

#ifdef _DROPSTAKENEW
#define CASINOTOKENID 20121     // 游乐场彩券代号
#define MAXCASINOMAPNUM 150     // 游乐场地图上限
#define MAXACCUMULATION 1000000 // 积分上限
typedef struct tagCasinoMapTable {
  int casinofl;        // 游乐场图层
  int casinox;         // 游乐场Ｘ座标
  int casinoy;         // 游乐场Ｙ座标
  int casinotype;      // 赌注类型
  int dropflag;        // 是否可下注 flag
  char casinoinfo[64]; // 赌注类型说明
  int accumulation;    // 积分
  int stakenum;        // 彩券数量
} CasinoMapTable;
CasinoMapTable casinomap[MAXCASINOMAPNUM] = {
    {7008, 13, 13, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 14, 13, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 15, 13, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 16, 13, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 17, 13, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 13, 14, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 14, 14, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 15, 14, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 16, 14, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 17, 14, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 13, 15, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 14, 15, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 15, 15, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 16, 15, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 17, 15, DICE_BIG, 1, "骰子－大", 1, 0},
    {7008, 8, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 9, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 10, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 11, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 12, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 8, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 9, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 10, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 11, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 12, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 8, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 9, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 10, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 11, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
    {7008, 12, 15, DICE_SMALL, 1, "骰子－小", 1, 0},

    {7005, 22, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 23, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 24, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 25, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 26, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 27, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 28, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
    {7005, 22, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 23, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 24, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 25, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 26, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 27, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 28, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
    {7005, 22, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 23, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 24, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 25, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 26, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 27, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
    {7005, 28, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},

    {7003, 21, 15, ROULETTE1, 1, "轮盘红１", 1, 0},
    {7003, 17, 15, ROULETTE3, 1, "轮盘红３", 1, 0},
    {7003, 13, 15, ROULETTE5, 1, "轮盘红５", 1, 0},
    {7003, 21, 21, ROULETTE6, 1, "轮盘红６", 1, 0},
    {7003, 17, 21, ROULETTE8, 1, "轮盘红８", 1, 0},
    {7003, 15, 21, ROULETTE9, 1, "轮盘红９", 1, 0},
    {7003, 13, 21, ROULETTE10, 1, "轮盘红１０", 1, 0},
    {7003, 13, 18, ROULETTE13, 1, "轮盘红１３", 1, 0},
    {7003, 9, 21, ROULETTE17, 1, "轮盘红１７", 1, 0},
    {7003, 21, 18, ROULETTE18, 1, "轮盘红１８", 1, 0},

    {7003, 19, 15, ROULETTE22, 1, "轮盘绿２", 1, 0},
    {7003, 15, 15, ROULETTE24, 1, "轮盘绿４", 1, 0},
    {7003, 19, 21, ROULETTE27, 1, "轮盘绿７", 1, 0},
    {7003, 11, 15, ROULETTE31, 1, "轮盘绿１１", 1, 0},
    {7003, 9, 15, ROULETTE32, 1, "轮盘绿１２", 1, 0},
    {7003, 11, 18, ROULETTE34, 1, "轮盘绿１４", 1, 0},
    {7003, 9, 18, ROULETTE35, 1, "轮盘绿１５", 1, 0},
    {7003, 11, 21, ROULETTE36, 1, "轮盘绿１６", 1, 0},
    {7003, 19, 18, ROULETTE39, 1, "轮盘绿１９", 1, 0},
    {7003, 17, 18, ROULETTE40, 1, "轮盘绿２０", 1, 0},
    {7003, 17, 24, ROULETTE41, 1, "轮盘红", 1, 0},
    {7003, 16, 24, ROULETTE41, 1, "轮盘红", 1, 0},
    {7003, 17, 25, ROULETTE41, 1, "轮盘红", 1, 0},
    {7003, 16, 25, ROULETTE41, 1, "轮盘红", 1, 0},
    {7003, 14, 24, ROULETTE42, 1, "轮盘绿", 1, 0},
    {7003, 13, 24, ROULETTE42, 1, "轮盘绿", 1, 0},
    {7003, 14, 25, ROULETTE42, 1, "轮盘绿", 1, 0},
    {7003, 13, 25, ROULETTE42, 1, "轮盘绿", 1, 0},

    // WON ADD 修正地图上单双的错误
    {7003, 11, 24, ROULETTE43, 1, "轮盘单", 1, 0},
    {7003, 10, 24, ROULETTE43, 1, "轮盘单", 1, 0},
    {7003, 11, 25, ROULETTE43, 1, "轮盘单", 1, 0},
    {7003, 10, 25, ROULETTE43, 1, "轮盘单", 1, 0},
    {7003, 20, 24, ROULETTE44, 1, "轮盘双", 1, 0},
    {7003, 19, 24, ROULETTE44, 1, "轮盘双", 1, 0},
    {7003, 20, 25, ROULETTE44, 1, "轮盘双", 1, 0},
    {7003, 19, 25, ROULETTE44, 1, "轮盘双", 1, 0},

    {7003, 20, 15, (ROULETTE1 << 16) + ROULETTE22, 1, "双号红１绿２", 1, 0},
    {7003, 18, 15, (ROULETTE3 << 16) + ROULETTE22, 1, "双号红３绿２", 1, 0},
    {7003, 16, 15, (ROULETTE3 << 16) + ROULETTE24, 1, "双号红３绿４", 1, 0},
    {7003, 14, 15, (ROULETTE5 << 16) + ROULETTE24, 1, "双号红５绿４", 1, 0},
    {7003, 12, 15, (ROULETTE5 << 16) + ROULETTE31, 1, "双号红５绿１１", 1, 0},
    {7003, 10, 15, (ROULETTE32 << 16) + ROULETTE31, 1, "双号绿１２绿１１", 1, 0},

    {7003, 20, 18, (ROULETTE18 << 16) + ROULETTE39, 1, "双号红１８绿１９", 1, 0},
    {7003, 18, 18, (ROULETTE40 << 16) + ROULETTE39, 1, "双号绿２０绿１９", 1, 0},
    {7003, 16, 18, (ROULETTE40 << 16) + ROULETTE45, 1, "双号绿２０黑００", 1, 0},
    {7003, 14, 18, (ROULETTE13 << 16) + ROULETTE45, 1, "双号红１３黑００", 1, 0},
    {7003, 12, 18, (ROULETTE13 << 16) + ROULETTE34, 1, "双号红１３绿１４", 1, 0},
    {7003, 10, 18, (ROULETTE35 << 16) + ROULETTE34, 1, "双号绿１５绿１４", 1, 0},

    {7003, 20, 21, (ROULETTE6 << 16) + ROULETTE27, 1, "双号红６绿７", 1, 0},
    {7003, 18, 21, (ROULETTE8 << 16) + ROULETTE27, 1, "双号红８绿７", 1, 0},
    {7003, 16, 21, (ROULETTE8 << 16) + ROULETTE9, 1, "双号红８红９", 1, 0},
    {7003, 14, 21, (ROULETTE10 << 16) + ROULETTE9, 1, "双号红１０红９", 1, 0},
    {7003, 12, 21, (ROULETTE10 << 16) + ROULETTE36, 1, "双号红１０绿１６", 1, 0},
    {7003, 10, 21, (ROULETTE17 << 16) + ROULETTE36, 1, "双号红１７绿１６", 1, 0},
    {7003, 8, 15, ROULETTE51, 1, "横列１", 1, 0}, // 横列
    {7003, 8, 18, ROULETTE52, 1, "横列２", 1, 0},
    {7003, 8, 21, ROULETTE53, 1, "横列３", 1, 0},
    {7003, 6, 15, ROULETTE61, 1, "号码１－１０", 1, 0}, // 1到10
    {7003, 5, 15, ROULETTE61, 1, "号码１－１０", 1, 0},
    {7003, 6, 16, ROULETTE61, 1, "号码１－１０", 1, 0},
    {7003, 5, 16, ROULETTE61, 1, "号码１－１０", 1, 0},
    {7003, 24, 15, ROULETTE62, 1, "号码１１－２０", 1, 0}, // 10到11
    {7003, 23, 15, ROULETTE62, 1, "号码１１－２０", 1, 0},
    {7003, 24, 16, ROULETTE62, 1, "号码１１－２０", 1, 0},
    {7003, 23, 16, ROULETTE62, 1, "号码１１－２０", 1, 0},
    {7003, 15, 18, ROULETTE45, 1, "轮盘(００)", 1, 0},

};
#endif

static char CHAR_sendItemBuffer[STRINGBUFSIZ];
BOOL CHAR_sendItemDataOne(int char_index, int haveitem_index) {
  int itemgrp[1];
  itemgrp[0] = haveitem_index;
  return CHAR_sendItemData(char_index, itemgrp, 1);
}

BOOL CHAR_sendItemData(int char_index, int *itemgroup, int num) {
  int i;
  int strlength = 0;
  int fd;
#ifdef _NEW_ITEM_
  if (num <= 0 || num > CheckCharMaxItem(char_index))
    return FALSE;
#else
  if (num <= 0 || num > CHAR_MAXITEMHAVE)
    return FALSE;

#endif

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return FALSE;
  }

  for (i = 0; i < num; i++) {
    char token[512];
    int item_index;
    if (!CHAR_CHECKITEMINDEX(char_index, itemgroup[i])) {
      continue;
    }
    item_index = CHAR_getItemIndex(char_index, itemgroup[i]);
    if (item_index >= 0) {

      snprintf(token, sizeof(token), "%d|%s|", itemgroup[i],
               ITEM_makeItemStatusString(-1, item_index));
    } else {
      snprintf(token, sizeof(token), "%s|",
               ITEM_makeItemFalseStringWithNum(itemgroup[i]));
    }
    strncpysafe(CHAR_sendItemBuffer + strlength,
               sizeof(CHAR_sendItemBuffer) - strlength, token);
    strlength += strlen(token);
    if (strlength >= arraysizeof(CHAR_sendItemBuffer)) {
      break;
    }
  }
  dchop(CHAR_sendItemBuffer, "|");
  fd = getfdFromCharaIndex(char_index);
  if (fd != -1) {
    GmsvServer_I_send(fd, CHAR_sendItemBuffer);
    return TRUE;
  }
  return FALSE;
}

static void CHAR_sendItemDetachEvent(int char_index, int item_index) {
  char mesg[256];
#ifdef _ITEM_USE_TIME
  if (ITEM_getInt(item_index, ITEM_USETIME) > 0) {
    time_t usetime = ITEM_getInt(item_index, ITEM_USETIME);
    struct tm *tm1 = localtime(&usetime);
    snprintf(mesg, sizeof(mesg), "卸下%s 有效使用期至 %d年%d月%d日 %d:%d:%d",
             ITEM_getAppropriateName(item_index), tm1->tm_year + 1900,
             tm1->tm_mon + 1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min,
             tm1->tm_sec);

  } else
#endif
  {
    snprintf(mesg, sizeof(mesg), "卸下%s ", ITEM_getAppropriateName(item_index));
  }
  CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
#ifdef _ITEM_METAMO
  if (ITEM_getEquipPlace(char_index, item_index) == CHAR_BODY &&
      CHAR_getWorkInt(char_index, CHAR_WORKITEMMETAMO) != 0) {
    CHAR_setWorkInt(char_index, CHAR_WORKITEMMETAMO, 0);
    CHAR_setWorkInt(char_index, CHAR_WORKNPCMETAMO, 0); //与npc对话後的变身也要变回来
    CHAR_complianceParameter(char_index);
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
    CHAR_send_P_StatusString(char_index, CHAR_P_STRING_BASEBASEIMAGENUMBER);
    CHAR_talkToCli(char_index, -1, "变身失效了！", CHAR_COLORWHITE);
  }
#endif
#ifdef _ITEM_RIDE
  if (CHAR_getInt(char_index, CHAR_RIDEPET) != -1) {
    if (!strcmp(ITEM_getChar(item_index, ITEM_USEFUNC), "ITEM_RIDE")) {
      char ridemetamo[12];
      char *itemarg = ITEM_getChar(item_index, ITEM_ARGUMENT);
      getStringFromIndexWithDelim(itemarg, "|", 2, ridemetamo,
                                  sizeof(ridemetamo));
      if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == atoi(ridemetamo)) {
        CHAR_setInt(char_index, CHAR_RIDEPET, -1);
        CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                    CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER));
        CHAR_complianceParameter(char_index);
        CHAR_sendCToArroundCharacter(
            CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
        CHAR_send_P_StatusString(char_index, CHAR_P_STRING_RIDEPET);
      }
    }
  }
#endif
  {
    typedef void (*DETACHFUNC)(int, int);
    DETACHFUNC def;
    def = (DETACHFUNC)ITEM_getFunctionPointer(item_index, ITEM_DETACHFUNC);
    if (def) {
      def(char_index, item_index);
    }
  }
}

static void CHAR_sendItemAttachEvent(int char_index, int item_index) {
  char mesg[512];
#ifdef _ITEM_USE_TIME
  if (ITEM_getInt(item_index, ITEM_USETIME) > 0) {
    time_t usetime = ITEM_getInt(item_index, ITEM_USETIME);
    struct tm *tm1 = localtime(&usetime);
    snprintf(mesg, sizeof(mesg), "装备%s 有效使用期至 %d年%d月%d日 %d:%d:%d",
             ITEM_getAppropriateName(item_index), tm1->tm_year + 1900,
             tm1->tm_mon + 1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min,
             tm1->tm_sec);

  } else
#endif
  {
    snprintf(mesg, sizeof(mesg), "装备%s ", ITEM_getAppropriateName(item_index));
  }

  CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
  {
    typedef void (*ATTACHFUNC)(int, int);
    ATTACHFUNC atf;
    atf = (ATTACHFUNC)ITEM_getFunctionPointer(item_index, ITEM_ATTACHFUNC);
    if (atf) {
      atf(char_index, item_index);
    }
  }
}

int CHAR_findSurplusItemBox(int char_index) {
  int i, remnants = 0, item_index;
  for (i = CHAR_STARTITEMARRAY; i < CheckCharMaxItem(char_index); i++) {
    item_index = CHAR_getItemIndex(char_index, i);
    if (item_index == -1)
      remnants++;
  }
  return remnants;
}
extern int CheckCharMaxItemChar(Char *ch);

static int CHAR_findEmptyItemBoxFromChar(Char *ch) {
  int i;
  int itemMax = CheckCharMaxItemChar(ch);
  for (i = CHAR_STARTITEMARRAY; i < itemMax; i++) {
    int item_index;
    item_index = ch->indexOfExistItems[i];
    if (item_index == -1)
      return i;
  }

  return -1;
}

static int CHAR_findEmptyPoolItemBoxFromChar(Char *ch) {
  int i;
  for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++) {
    int item_index;
    item_index = ch->indexOfExistPoolItems[i];
    if (item_index == -1)
      return i;
  }
  return -1;
}

int CHAR_findEmptyPoolItemBox(int index) {
  Char *ch;
  if (!CHAR_CHECKINDEX(index))
    return -1;
  ch = CHAR_getCharPointer(index);
  if (ch != NULL)
    return CHAR_findEmptyPoolItemBoxFromChar(ch);
  else
    return -1;
}

int CHAR_findEmptyItemBox(int index) {
  Char *ch;

  if (!CHAR_CHECKINDEX(index))
    return -1;
  ch = CHAR_getCharPointer(index);
  if (ch != NULL)
    return CHAR_findEmptyItemBoxFromChar(ch);
  else
    return -1;
}

#define LACKOFCLASS                                                            \
  {                                                                            \
    CHAR_talkToCli(index, -1,                                                  \
                   "能力不足无法装备。",           \
                   CHAR_COLORWHITE);                                           \
  }
#define CANNOTEQUIP                                                            \
  {                                                                            \
    CHAR_talkToCli(index, -1, "无法装备该项物品。",    \
                   CHAR_COLORWHITE);                                           \
  }
#define CANNOTEQUIP2                                                           \
  {                                                                            \
    CHAR_talkToCli(                                                            \
        index, -1,                                                             \
        "无法同时装备两项同种类的物品。",     \
        CHAR_COLORWHITE);                                                      \
  }
#define CANNOTEXCHANGE                                                         \
  {                                                                            \
    CHAR_talkToCli(index, -1,                                                  \
                   "无法交换此两项物品。",     \
                   CHAR_COLORWHITE);                                           \
  }
#define CANNOTMOVE                                                             \
  {                                                                            \
    CHAR_talkToCli(index, -1, "无法移动该项物品。",    \
                   CHAR_COLORWHITE);                                           \
  }
#ifdef _ROOKIE_ITEM
#define ROOKIEITEM                                                             \
  {                                                                            \
    CHAR_talkToCli(index, -1, "您已经超过120级，此装备已经不适合您了。",            \
                   CHAR_COLORWHITE);                                           \
  }
#endif
#ifdef _TRUMP_EQUIPMENT
#define NOTRUMP                                                                \
  {                                                                            \
    CHAR_talkToCli(index, -1, "您的法宝道具栏尚未开启。", CHAR_COLORWHITE);      \
  }
#endif
static BOOL CHAR_sendSIToCli(int charindex, int from, int to) {
  int fd = getfdFromCharaIndex(charindex);
  if (fd != -1) {
    if (from == -1 && to == -1)
      return FALSE;
    GmsvServer_SI_send(fd, from, to);
    return TRUE;
  }
  return FALSE;
}

static BOOL CHAR_moveItemFromItemBoxToEquip(int index, int fromindex,
                                            int toindex) {
  int fromid, toid;
  int fromeqplace;
  BOOL flg = FALSE;

  if (!CHAR_CHECKINDEX(index))
    return FALSE;

  fromid = CHAR_getItemIndex(index, fromindex);
  toid = CHAR_getItemIndex(index, toindex);
  if (!ITEM_CHECKINDEX(fromid))
    return FALSE;
#ifdef _TRUMP_EQUIPMENT
#ifndef _BRAND_NEW_25
  if (toindex == CHAR_EQSHIELD && CHAR_getInt(index, CHAR_TRUMPFLG) <= 0) {
    NOTRUMP;
    return FALSE;
  }
#endif
#endif

  if (CHAR_getInt(index, CHAR_TRANSMIGRATION) <= 0) {
    if (ITEM_getInt(fromid, ITEM_LEVEL) > CHAR_getInt(index, CHAR_LV)) {
      LACKOFCLASS;
      return FALSE;
    }
  }

#ifdef _ROOKIE_ITEM
  if (CHAR_getInt(index, CHAR_TRANSMIGRATION) > 0 ||
      CHAR_getInt(index, CHAR_LV) >= 120) {
    if (ITEM_getInt(fromid, ITEM_ID) == getRookieItem(0) ||
        ITEM_getInt(fromid, ITEM_ID) == getRookieItem(1) ||
        ITEM_getInt(fromid, ITEM_ID) == getRookieItem(2) ||
        ITEM_getInt(fromid, ITEM_ID) == getRookieItem(3) ||
        ITEM_getInt(fromid, ITEM_ID) == getRookieItem(4)) {
      ROOKIEITEM;
      return FALSE;
    }
  }
#endif

#ifdef _ITEMSET4_TXT
  if (CHAR_getInt(index, CHAR_STR) < ITEM_getInt(fromid, ITEM_NEEDSTR))
    return FALSE;
  if (CHAR_getInt(index, CHAR_DEX) < ITEM_getInt(fromid, ITEM_NEEDDEX))
    return FALSE;
  if (CHAR_getInt(index, CHAR_TRANSMIGRATION) <
      ITEM_getInt(fromid, ITEM_NEEDTRANS))
    return FALSE;
#ifdef _CHAR_PROFESSION // WON ADD 人物职业
  if (ITEM_getInt(fromid, ITEM_NEEDPROFESSION) != 0) {
    if (CHAR_getInt(index, PROFESSION_CLASS) !=
        ITEM_getInt(fromid, ITEM_NEEDPROFESSION))
      return FALSE;
  }
#endif
#endif

#ifdef _FIXBUG_ATTACKBOW
  {
    if (CHAR_getWorkInt(index, CHAR_WORKITEMMETAMO) > 0 ||
        CHAR_getWorkInt(index, CHAR_WORKNPCMETAMO) > 0 ||
        CHAR_getWorkInt(index, CHAR_WORKFOXROUND) != -1) { //若是变成小狐狸

      int armtype = ITEM_getInt(fromid, ITEM_TYPE);

      if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
          armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
        CHAR_talkToCli(
            index, -1,
            "变身无法使用远距离武器！",
            CHAR_COLORYELLOW);
        return FALSE;
      }
    }
  }
#endif

#ifdef _ANGEL_SUMMON
  // if( !strcmp( ITEM_getChar( fromid, ITEM_USEFUNC), "ITEM_AngelToken") ) {
  if (ITEM_getInt(fromid, ITEM_ID) == ANGELITEM) {
    int mindex;
    char nameinfo[64];

    mindex = checkIfAngel(index);
    getMissionNameInfo(index, nameinfo);
    if (mindex < 0 ||
        strcmp(ITEM_getChar(fromid, ITEM_ANGELINFO),
               missiontable[mindex].angelinfo) ||
        strcmp(ITEM_getChar(fromid, ITEM_HEROINFO),
               missiontable[mindex].heroinfo) ||
        strcmp(ITEM_getChar(fromid, ITEM_ANGELINFO), nameinfo)) {

      CHAR_talkToCli(
          index, -1,
          "这并不是属於你的信物，无法装备。"
          "这并不是属於你的信物，无法装备。",
          CHAR_COLORYELLOW);
      return FALSE;
    }
  }
#endif

  fromeqplace = ITEM_getEquipPlace(index, fromid);
  if (fromeqplace == -1) {
    CANNOTEQUIP;
    return FALSE;
  }
  // if( BATTLE_GetWepon( char_index ) != ITEM_BOW )

  if (fromeqplace == CHAR_DECORATION1) {
    int checkplace =
        (toindex == CHAR_DECORATION1) ? CHAR_DECORATION2 : CHAR_DECORATION1;
    int otheritem_index;

    if (toindex == CHAR_DECORATION1 || toindex == CHAR_DECORATION2) {
      otheritem_index = CHAR_getItemIndex(index, checkplace);
      if (ITEM_CHECKINDEX(otheritem_index)) {
        int otheritemtype = ITEM_getInt(otheritem_index, ITEM_TYPE);
        int myitemtype =
            ITEM_getInt(CHAR_getItemIndex(index, fromindex), ITEM_TYPE);
        if (otheritemtype == myitemtype) {
          if (toid < 0) {
            CANNOTEQUIP2;
          } else {
            CANNOTEXCHANGE;
          }
          return FALSE;
        }
      }
      flg = TRUE;
    }
  } else {
    if (toindex == fromeqplace)
      flg = TRUE;
  }
  if (flg) {
    CHAR_setItemIndex(index, toindex, fromid);
    CHAR_setItemIndex(index, fromindex, toid);

    if (toid != -1) {
      CHAR_sendItemDetachEvent(index, toid);
    }
    CHAR_sendItemAttachEvent(index, fromid);
    CHAR_sendSIToCli(index, fromindex, toindex);
  } else {
    CANNOTEQUIP;
    return FALSE;
  }
  return TRUE;
}

static BOOL CHAR_moveItemFromEquipToItemBox(int index, int fromindex,
                                            int toindex) {
  int fromid, toid;

  if (!CHAR_CHECKINDEX(index))
    return FALSE;

  fromid = CHAR_getItemIndex(index, fromindex);
  toid = CHAR_getItemIndex(index, toindex);
  if (fromid < 0)
    return FALSE;
  else if (toid < 0) {
    CHAR_setItemIndex(index, toindex, fromid);
    CHAR_setItemIndex(index, fromindex, -1);
    CHAR_sendItemDetachEvent(index, fromid);
    CHAR_sendSIToCli(index, fromindex, toindex);
    return TRUE;
  }

  return CHAR_moveItemFromItemBoxToEquip(index, toindex, fromindex);
}

#ifdef _ITEM_PILENUMS
int CHAR_getMyMaxPilenum(int char_index) {
  int maxpile;
  maxpile = CHAR_getInt(char_index, CHAR_TRANSMIGRATION) +
            (CHAR_getInt(char_index, CHAR_TRANSMIGRATION) / 5) * 2 + 3;

#ifdef _EQUIT_ADDPILE
  maxpile += CHAR_getWorkInt(char_index, CHAR_WORKATTACHPILE);
  maxpile = (maxpile < 0) ? 0 : maxpile;
#endif
  return maxpile;
}

BOOL CHAR_PileItemFromItemBoxToItemBox(int char_index, int fromindex,
                                       int toindex) {
  int maxpile, fromid, toid;
  if (!CHAR_CHECKINDEX(char_index))
    return FALSE;
  fromid = CHAR_getItemIndex(char_index, fromindex);
  toid = CHAR_getItemIndex(char_index, toindex);

  maxpile = CHAR_getMyMaxPilenum(char_index);

  if (ITEM_CHECKINDEX(toid) && ITEM_CHECKINDEX(fromid) &&
      (ITEM_getInt(toid, ITEM_ID) == ITEM_getInt(fromid, ITEM_ID)) &&
      (ITEM_getInt(toid, ITEM_CANBEPILE) == 1) &&
      (ITEM_getInt(toid, ITEM_USEPILENUMS) < maxpile) &&
      (ITEM_getInt(fromid, ITEM_USEPILENUMS) < maxpile)) {
    int formpilenum, pilenum, defpilenum;

    pilenum = ITEM_getInt(toid, ITEM_USEPILENUMS);
    formpilenum = ITEM_getInt(fromid, ITEM_USEPILENUMS);
    defpilenum =
        ((maxpile - pilenum) > formpilenum) ? formpilenum : (maxpile - pilenum);
    formpilenum = formpilenum - defpilenum;
    pilenum = pilenum + defpilenum;
    ITEM_setInt(toid, ITEM_USEPILENUMS, pilenum);
    ITEM_setInt(fromid, ITEM_USEPILENUMS, formpilenum);

    if (formpilenum <= 0) {
      CHAR_setItemIndex(char_index, fromindex, -1);
      ITEM_endExistItemsOne(fromid);
    }
    return TRUE;
  }
  return FALSE;
}
#endif

static BOOL CHAR_moveItemFromItemBoxToItemBox(int index, int fromindex,
                                              int toindex) {
  int fromid, toid;

  if (!CHAR_CHECKINDEX(index))
    return FALSE;
#ifdef _ITEM_PILENUMS
  if (CHAR_PileItemFromItemBoxToItemBox(index, fromindex, toindex) == TRUE) {
    CHAR_sendItemDataOne(index, toindex);
    CHAR_sendItemDataOne(index, fromindex);
    return TRUE;
  }
#endif
  fromid = CHAR_getItemIndex(index, fromindex);
  toid = CHAR_getItemIndex(index, toindex);

  CHAR_setItemIndex(index, toindex, CHAR_setItemIndex(index, fromindex, toid));
  CHAR_sendSIToCli(index, fromindex, toindex);

  return TRUE;
}

void CHAR_moveEquipItem(int index, int fromindex, int toindex) {
#define MOVEITEM_NONE 0
#define MOVEITEM_EQUIPTOITEM 1 << 0
#define MOVEITEM_ITEMTOEQUIP 1 << 1
#define MOVEITEM_ITEMTOITEM 1 << 2

  int fromid; /*  item    匹及奶件犯永弁旦    */

  unsigned int moved_any = 0;
  while (1) {
    if (!CHAR_CHECKINDEX(index))
      break;
    if (CHAR_getFlg(index, CHAR_ISDIE))
      break;
    if (fromindex < 0 || fromindex >= CheckCharMaxItem(index) || toindex < 0 ||
        toindex >= CheckCharMaxItem(index))
      break;

    fromid = CHAR_getItemIndex(index, fromindex);
    if (fromid == -1 || fromid == -2)
      break;
    if (fromindex == toindex)
      break;
    if (fromindex < CHAR_EQUIPPLACENUM) {
      if (toindex < CHAR_EQUIPPLACENUM) {
        if (CHAR_getItemIndex(index, toindex) < 0) {
          CANNOTMOVE;
        } else {
          CANNOTEXCHANGE;
        }

      } else {
        moved_any |= CHAR_moveItemFromEquipToItemBox(index, fromindex, toindex)
                         ? MOVEITEM_EQUIPTOITEM
                         : MOVEITEM_NONE;
        if (moved_any & MOVEITEM_EQUIPTOITEM) {
          char category[3];
          snprintf(category, sizeof(category), "J%d", fromindex);
          CHAR_sendStatusString(index, category);
        }
      }
    } else {
      if (toindex < CHAR_EQUIPPLACENUM) {
        moved_any |= CHAR_moveItemFromItemBoxToEquip(index, fromindex, toindex)
                         ? MOVEITEM_ITEMTOEQUIP
                         : MOVEITEM_NONE;
        if (moved_any & MOVEITEM_ITEMTOEQUIP) {
          char category[3];
          snprintf(category, sizeof(category), "J%d", toindex);
          CHAR_sendStatusString(index, category);
        }
      } else {
        moved_any |=
            CHAR_moveItemFromItemBoxToItemBox(index, fromindex, toindex)
                ? MOVEITEM_ITEMTOITEM
                : MOVEITEM_NONE;
      }
    }
    CHAR_complianceParameter(index);
    CHAR_sendCToArroundCharacter(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX));
    break;
  }

  if ((moved_any & MOVEITEM_EQUIPTOITEM) ||
      (moved_any & MOVEITEM_ITEMTOEQUIP)) {
    CHAR_send_P_StatusString(
        index, CHAR_P_STRING_HP | CHAR_P_STRING_MAXHP | CHAR_P_STRING_MP |
                   CHAR_P_STRING_MAXMP | CHAR_P_STRING_ATK | CHAR_P_STRING_DEF |
                   CHAR_P_STRING_QUICK | CHAR_P_STRING_CHARM |
                   CHAR_P_STRING_LUCK | CHAR_P_STRING_EARTH |
                   CHAR_P_STRING_WATER | CHAR_P_STRING_FIRE |
                   CHAR_P_STRING_WIND);
  }
  if (!moved_any) {
    CHAR_sendSIToCli(index, -1, -1);
  }
}

int CHAR_addItemToChar(Char *ch, int itemid) {
  int itembox;
  itembox = CHAR_findEmptyItemBoxFromChar(ch);
  if (itembox == -1)
    return -1;

  return ch->indexOfExistItems[itembox] = ITEM_makeItemAndRegist(itemid);
}

int CHAR_addItem(int char_index, int itemid) {
  Char *ch;
  ch = CHAR_getCharPointer(char_index);
  if (ch != NULL) {
    int ret = CHAR_addItemToChar(ch, itemid);
    if (ret == -1)
      return ret;
    ITEM_setWorkInt(ret, ITEM_WORKCHARAINDEX, char_index);
    ITEM_setWorkInt(ret, ITEM_WORKOBJINDEX, -1);
    return ret;
  } else
    return -1;
}

void CHAR_ItemUse(int char_index, int to_char_index, int haveitem_index) {
  void (*usefunc)(int, int, int) = NULL;
  int item_index;

  if (!CHAR_CHECKINDEX(char_index)) {
    print("ANDY char_index=%d err\n", char_index);
    return;
  }

  if (CHAR_CHECKITEMINDEX(char_index, haveitem_index) == FALSE) {
    print("ANDY haveitem_index=%d err\n", haveitem_index);
    return;
  }
#ifdef _NO_USE_PACKET_MAP
  int index;
  for (index = 0; index < 30; index++) {
    if (CHAR_getInt(char_index, CHAR_FLOOR) == getNoPacketMap(index)) {
      if (haveitem_index >= CHAR_STARTITEMARRAY + CHAR_MAXITEMNUM) {
        char ansmsg[256];
        strncpysafe(ansmsg, sizeof(ansmsg), "该地图禁止使用额外的包裹。");
        CHAR_talkToCli(char_index, -1, ansmsg, CHAR_COLORWHITE);
        return;
      }
    }
  }
#endif
  item_index = CHAR_getItemIndex(char_index, haveitem_index);
  if (!ITEM_CHECKINDEX(item_index)) {
    //		print( "ANDY item_index=%d err\n", item_index);
    return;
  }
  if (CHAR_getFlg(char_index, CHAR_ISDIE)) {
    print("ANDY char_index=%d is CHAR_ISDIE err\n", char_index);
    return;
  }
#ifndef _OFFLINE_SYSTEM
#ifdef _STREET_VENDOR
  // 摆摊中不可使用道具
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) > -1) {
    print("StreetVendor use item char_index=%d,name=%s\n", char_index,
          CHAR_getChar(char_index, CHAR_NAME));
    return;
  }
#endif
#endif
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
      BATTLE_CHARMODE_NONE) {
    if (CHAR_CHECKINDEX(to_char_index)) {
      if (CHAR_getInt(to_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        int action = ITEM_getInt(item_index, ITEM_USEACTION);
        if (action != -1) {
          CHAR_sendAction(to_char_index, action, TRUE);
        }
      }
    }
  }
  // 不等於料理 和 其他
  if (ITEM_getInt(item_index, ITEM_TYPE) != ITEM_OTHER &&
      ITEM_getInt(item_index, ITEM_TYPE) != ITEM_DISH) {
    CHAR_EquipPlace ep;

    ep = ITEM_getEquipPlace(char_index, item_index);
    if (ep == -1)
      return;
    if (ep == CHAR_DECORATION1) {
      int flg = FALSE;
      int toitem_index1 = CHAR_getItemIndex(char_index, ep);
      int toitem_index2 = CHAR_getItemIndex(char_index, CHAR_DECORATION2);
      int searchindex = -1;
      int fromitemtype = ITEM_getInt(item_index, ITEM_TYPE);

      if (!ITEM_CHECKINDEX(toitem_index1))
        searchindex = toitem_index2;
      else if (!ITEM_CHECKINDEX(toitem_index2))
        searchindex = toitem_index1;
      if (searchindex != -1) {
        if (fromitemtype != ITEM_getInt(searchindex, ITEM_TYPE)) {
          flg = TRUE;
          ep = (searchindex == toitem_index1) ? CHAR_DECORATION2
                                              : CHAR_DECORATION1;
        }
      }
      if (!flg) {
        if (fromitemtype != ITEM_getInt(toitem_index2, ITEM_TYPE)) {
          ep = CHAR_DECORATION1;
        } else if (fromitemtype != ITEM_getInt(toitem_index1, ITEM_TYPE)) {
          ep = CHAR_DECORATION2;
        } else {
          ep = CHAR_DECORATION1;
        }
      }
    }
    CHAR_moveEquipItem(char_index, haveitem_index, ep);
    return;
  }

#ifdef _ITEM_TALK_CHECK
  {
    char *tmp = ITEM_getChar(item_index, ITEM_NAME);
    if (tmp[0] == '@') {
      if (ITEM_getWorkInt(item_index, ITEM_WORKTALKCHECK) <= 0) {
        ITEM_setWorkInt(item_index, ITEM_WORKTALKCHECK, getTalkCheck());
      }

      if (ITEM_getWorkInt(item_index, ITEM_WORKTALKCHECK) > 0) {
        if (ITEM_getWorkInt(item_index, ITEM_WORKTALKCHECK) !=
            CHAR_getWorkInt(char_index, CHAR_WORK_TALKCHECK)) {
          char buf[256];
          ITEM_setWorkInt(item_index, ITEM_WORKTALKCHECK, getTalkCheck());
          sprintf(buf, "%s：请输入验证数字:%d",
                  ITEM_getChar(item_index, ITEM_NAME),
                  ITEM_getWorkInt(item_index, ITEM_WORKTALKCHECK));
          CHAR_talkToCli(char_index, -1, buf, CHAR_COLORYELLOW);
          return;
        } else {
          ITEM_setWorkInt(item_index, ITEM_WORKTALKCHECK, getTalkCheck());
        }
      }
    }
  }
#endif

#ifdef _ITEM_LUA
  char *voidname = ITEM_getChar(item_index, ITEM_USEFUNC);
  if (strstr(voidname, "ITEM_USE_LUA")) {
    usefunc = (void (*)(int, int, int))ITEM_Lua;
  } else
#endif
    usefunc = (void (*)(int, int, int))ITEM_getFunctionPointer(item_index,
                                                               ITEM_USEFUNC);
  if (usefunc) {
    {
      LogItem(CHAR_getChar(char_index, CHAR_NAME),
              CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name
              item_index,
#else
              ITEM_getInt(item_index, ITEM_ID),
#endif
              "Use(使用道具)", CHAR_getInt(char_index, CHAR_FLOOR),
              CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
              ITEM_getChar(item_index, ITEM_UNIQUECODE),
              ITEM_getChar(item_index, ITEM_NAME),
              ITEM_getInt(item_index, ITEM_ID));
    }
    usefunc(char_index, to_char_index, haveitem_index);
  } else {
    char ansmsg[256];
    strncpysafe(ansmsg, sizeof(ansmsg), "什麽也没有发生。");
    CHAR_talkToCli(char_index, -1, ansmsg, CHAR_COLORWHITE);
  }
}

BOOL CHAR_DropItemFXY(int char_index, int itemchar_index, int fl, int x, int y,
                      int *objindex) {
  int item_index;
  item_index = CHAR_getItemIndex(char_index, itemchar_index);
  if (ITEM_CHECKINDEX(item_index) == FALSE)
    return -3;
  if (MAP_walkAbleFromPoint(fl, x, y, FALSE) == FALSE) {
    return -2;
  }

  {
    OBJECT object;
    for (object = MAP_getTopObj(fl, x, y); object;
         object = NEXT_OBJECT(object)) {
      int objindex = GET_OBJINDEX(object);
      switch (OBJECT_getType(objindex)) {
      case OBJTYPE_NOUSE:
        break;
      case OBJTYPE_ITEM:
#ifdef _DROPSTAKENEW
        if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
          int i = 0;
          int dropflag = 0;
          ITEM_Item *itm;
          for (i = 0; i < arraysizeof(casinomap); i++) {
            if (CHAR_getInt(char_index, CHAR_FLOOR) == casinomap[i].casinofl) {
              if (x == casinomap[i].casinox && y == casinomap[i].casinoy) {
                dropflag = 1;
                break;
              }
            }
          }
          if (dropflag == 1) {
            if ((itm = ITEM_getItemPointer(item_index)) == NULL)
              return -1;
            strncpysafe(itm->string[ITEM_WATCHFUNC].string,
                       sizeof(itm->string[ITEM_WATCHFUNC].string),
                       "ITEM_DeleteTimeWatched");
            break;
          } else {
            return -1;
          }
        }
#endif
      case OBJTYPE_GOLD: {
        return -1;
      }
      case OBJTYPE_CHARA: {
        typedef BOOL (*ITEMPUTFUNC)(int, int);
        ITEMPUTFUNC ipfunc;
        ipfunc = (ITEMPUTFUNC)CHAR_getFunctionPointer(OBJECT_getIndex(objindex),
                                                      CHAR_ITEMPUTFUNC);

        if (ipfunc && ipfunc(OBJECT_getIndex(objindex), item_index) == TRUE)
          return -3;
        break;
      }
      default:
        break;
      }
    }

    *objindex = CHAR_DropItemAbsolute(item_index, fl, x, y, FALSE);

    if (*objindex == -1)
      return -3;
    {
      LogItem(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
              CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
              item_index,
#else
              ITEM_getInt(item_index, ITEM_ID), /* 失奶  丞  寞 */
#endif
              "Drop(丢出道具)", CHAR_getInt(char_index, CHAR_FLOOR),
              CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
              ITEM_getChar(item_index, ITEM_UNIQUECODE),
              ITEM_getChar(item_index, ITEM_NAME),
              ITEM_getInt(item_index, ITEM_ID)

      );
    }
    return 0;
  }
}

#ifdef _ITEM_PILENUMS // andy_edit 2003/04/01

void CHAR_SendDropItem_Stats(int char_index, int item_index, int itemchar_index,
                             int flg) {
  if (flg == 1) {
    CHAR_setItemIndex(char_index, itemchar_index, -1);
  }
  CHAR_sendItemDataOne(char_index, itemchar_index);

  if (CHAR_complianceParameter(char_index)) {
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
  }

  if (0 <= itemchar_index && itemchar_index < CHAR_STARTITEMARRAY &&
      CHAR_getInt(char_index, CHAR_WHICHTYPE) ==
          CHAR_TYPEPLAYER) { //如果丢弃物为装备
    if (ITEM_CHECKINDEX(item_index))
      CHAR_sendItemDetachEvent(char_index, item_index);

    CHAR_send_P_StatusString(
        char_index, CHAR_P_STRING_HP | CHAR_P_STRING_MAXHP | CHAR_P_STRING_MP |
                        CHAR_P_STRING_MAXMP | CHAR_P_STRING_ATK |
                        CHAR_P_STRING_DEF | CHAR_P_STRING_QUICK |
                        CHAR_P_STRING_CHARM | CHAR_P_STRING_LUCK |
                        CHAR_P_STRING_EARTH | CHAR_P_STRING_WATER |
                        CHAR_P_STRING_FIRE | CHAR_P_STRING_WIND);
  }
}
#ifdef _DROPSTAKENEW
void CHAR_DropStakeByDropItem(int char_index, int itemchar_index,
                              int item_index, int fl, int x, int y) {
  int j, k, casinoflag = 0, dropflag;
  char tmpbuf[256];
  dropflag = CHAR_getWorkInt(char_index, CHAR_WORKSTAKEFLAG);
  if (dropflag >= MAXSTAKENUM) {
    snprintf(
        tmpbuf, sizeof(tmpbuf),
        "你已经下注五次了，无法再下注！");
    CHAR_talkToCli(char_index, -1, tmpbuf, CHAR_COLORYELLOW);
    return;
  }
  for (j = 0; j < arraysizeof(casinomap); j++) {
    if (CHAR_getInt(char_index, CHAR_FLOOR) != casinomap[j].casinofl)
      continue;
    if (x != casinomap[j].casinox || y != casinomap[j].casinoy)
      continue;
    if (casinomap[j].dropflag == 0) {
      CHAR_talkToCli(char_index, -1, "现在无法下注！",
                     CHAR_COLORYELLOW);
      return;
    } else {
      int objindex;
      objindex = CHAR_DropItemAbsolute(item_index, fl, x, y, FALSE);
      if (objindex == -1)
        return;
      ITEM_setInt(item_index, ITEM_PUTTIME, NowTime.tv_sec + 30 * 60);
      CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
      casinoflag = 1;
      snprintf(tmpbuf, sizeof(tmpbuf), "你在 %s 下注了一张彩券",
               casinomap[j].casinoinfo);
      for (k = 0; k < MAXSTAKENUM; k++) { // 下注设定
        if (CHAR_getWorkInt(char_index, CHAR_WORKSTAKETYPE1 + k) != 0)
          continue;
        CHAR_setWorkInt(char_index, CHAR_WORKSTAKETYPE1 + k,
                        casinomap[j].casinotype);
        dropflag++;
        break;
      }
      CHAR_talkToCli(char_index, -1, tmpbuf, CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
      if (dropflag <= MAXSTAKENUM) { // 做扣点的动作
        int nAcc = CHAR_getInt(char_index, CHAR_GAMBLENUM);
        nAcc -= casinomap[j].accumulation;
        CHAR_setInt(char_index, CHAR_GAMBLENUM, nAcc);
      }
#endif
      dropflag = (dropflag >= MAXSTAKENUM) ? MAXSTAKENUM : dropflag;
      CHAR_setWorkInt(char_index, CHAR_WORKSTAKEFLAG, dropflag);
      break;
    }
  }

  if (casinoflag == 0)
    return;
  LogItem(
      CHAR_getChar(char_index, CHAR_NAME), CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
      item_index,
#else
      ITEM_getInt(item_index, ITEM_ID),
#endif
      "StakeDrop(丢出彩券)", CHAR_getInt(char_index, CHAR_FLOOR),
      CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
      ITEM_getChar(item_index, ITEM_UNIQUECODE),
      ITEM_getChar(item_index, ITEM_NAME), ITEM_getInt(item_index, ITEM_ID));
  CHAR_SendDropItem_Stats(char_index, item_index, itemchar_index, 1);
}
#endif

BOOL CHAR_FindAroundUsabilitySpace(int char_index, int item_index, int *fl,
                                   int *x, int *y) {
  int dropx, dropy, i;
  BOOL Find = FALSE;
  int myfl = CHAR_getInt(char_index, CHAR_FLOOR);

  for (i = 0; i < 8; i++) {
    OBJECT object;
    Find = FALSE;
    dropx = CHAR_getInt(char_index, CHAR_X) +
            CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR) + i);
    dropy = CHAR_getInt(char_index, CHAR_Y) +
            CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR) + i);
    if (MAP_walkAbleFromPoint(myfl, dropx, dropy, FALSE) == FALSE)
      continue;
    for (object = MAP_getTopObj(myfl, dropx, dropy); object;
         object = NEXT_OBJECT(object)) {
      int objindex = GET_OBJINDEX(object);
      if (!CHECKOBJECTUSE(objindex))
        continue;
      if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
        typedef BOOL (*ITEMPUTFUNC)(int, int);
        ITEMPUTFUNC ipfunc;
        ipfunc = (ITEMPUTFUNC)CHAR_getFunctionPointer(OBJECT_getIndex(objindex),
                                                      CHAR_ITEMPUTFUNC);
        if (ipfunc && ipfunc(OBJECT_getIndex(objindex), item_index) == TRUE)
          return FALSE;
      } else {
        Find = TRUE;
        break;
      }
    }
    if (Find == FALSE) {
      *fl = myfl;
      *x = dropx;
      *y = dropy;
      return TRUE;
    }
  }
  return FALSE;
}

void CHAR_DropItem(int char_index, int itemchar_index) {
  int item_index, fl, x, y, ret, objindex, beDropOne = 1;

  if (!CHAR_CHECKINDEX(char_index))
    return;

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1,
                   "战斗状态中无法丢道具装备。",
                   CHAR_COLORYELLOW);
    return;
  }
#ifdef _AVID_TRADETRYBUG // 丢道具装备
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    CHAR_talkToCli(
        char_index, -1,
        "交易状态中无法丢道具装备。",
        CHAR_COLORYELLOW);
    return;
  }
#endif

  item_index = CHAR_getItemIndex(char_index, itemchar_index);
  if (!ITEM_CHECKINDEX(item_index))
    return;

#ifdef _LOCK_PET_ITEM
  char *arg = ITEM_getChar(item_index, ITEM_NAME);
  if (arg[0] == '*' || FreeDropItem(char_index, item_index) == 1) {
    char dropmsg[256];
    sprintf(dropmsg,
            "\n【%s】为特殊物品\n\n丢弃会直接消失！\n\n你确定要丢弃么？确定请按[OK]\n",
            ITEM_getChar(item_index, ITEM_NAME));
    CHAR_setWorkInt(char_index, CHAR_WORK_LOCK_PET_ITEM, itemchar_index);
    GmsvServer_WN_send(getfdFromCharaIndex(char_index),
                       WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OKCANCEL,
                       CHAR_WINDOWTYPE_DROPITEM, -1, dropmsg);
    return;
  }
#endif

  if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID &&
      (CHAR_getInt(char_index, CHAR_FLOOR) == 7008 ||
       CHAR_getInt(char_index, CHAR_FLOOR) == 7005 ||
       CHAR_getInt(char_index, CHAR_FLOOR) == 7003)) {

    int dropx, dropy, dropfl;
    dropfl = CHAR_getInt(char_index, CHAR_FLOOR);
    dropx = CHAR_getInt(char_index, CHAR_X) +
            CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR));
    dropy = CHAR_getInt(char_index, CHAR_Y) +
            CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR));
    CHAR_DropStakeByDropItem(char_index, itemchar_index, item_index, dropfl,
                             dropx, dropy);
    return;
  }
  // 找出周围空间
  if (CHAR_FindAroundUsabilitySpace(char_index, item_index, &fl, &x, &y) ==
      FALSE) {
    CHAR_talkToCli(char_index, -1, "周围的地面已经满了。", CHAR_COLORYELLOW);
    return;
  }

#ifdef _ITEM_PILENUMS
  if (ITEM_getInt(item_index, ITEM_CANBEPILE) == 1 &&
      ITEM_getInt(item_index, ITEM_USEPILENUMS) > 1) {
    int ret;
    int nums = ITEM_getInt(item_index, ITEM_USEPILENUMS);
    ret = ITEM_makeItemAndRegist(ITEM_getInt(item_index, ITEM_ID));
    if (!ITEM_CHECKINDEX(ret))
      return;
    ITEM_setInt(item_index, ITEM_USEPILENUMS, nums - 1);
    item_index = ret;
    beDropOne = 0;
  }
#endif
  // 找到空间
  ret = ITEM_eventDrop(item_index, char_index, itemchar_index);
  if (ret == 1) { //-1 物品不存在 1 消失 0 一般物品
    CHAR_SendDropItem_Stats(char_index, item_index, itemchar_index, beDropOne);
    return;
  } else {
    objindex = CHAR_DropItemAbsolute(item_index, fl, x, y, FALSE);
    if (objindex == -1)
      return;
    ITEM_setInt(item_index, ITEM_PUTTIME, NowTime.tv_sec);
#ifdef _PET_AND_ITEM_UP
    ITEM_setChar(item_index, ITEM_CDKEY, CHAR_getChar(char_index, CHAR_CDKEY));
#endif
    {
      LogItem(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
              CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
              item_index,
#else
              ITEM_getInt(item_index, ITEM_ID), /* 失奶  丞  寞 */
#endif
              "Drop(丢出道具)", CHAR_getInt(char_index, CHAR_FLOOR),
              CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
              ITEM_getChar(item_index, ITEM_UNIQUECODE),
              ITEM_getChar(item_index, ITEM_NAME),
              ITEM_getInt(item_index, ITEM_ID));
    }
    CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
  }
  CHAR_SendDropItem_Stats(char_index, item_index, itemchar_index, beDropOne);
}
#else
void CHAR_DropItem(int char_index, int itemchar_index) {
  int dirx[9], diry[9];
  int i, j;
  int floor, objindex = -1;
  int droped = 0;
  int count_item = 0, count_chara = 0;
  int fl, x, y;
  int item_index = CHAR_getItemIndex(char_index, itemchar_index);

  if (!CHAR_CHECKINDEX(char_index))
    return;
  if (!ITEM_CHECKINDEX(item_index))
    return;
#ifdef _AVID_TRADETRYBUG // 丢道具装备
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    CHAR_talkToCli(
        char_index, -1,
        "交易状态中无法丢道具装备。",
        CHAR_COLORYELLOW);
    return;
  }
#endif
#ifdef _LOCK_PET_ITEM
  char *arg = ITEM_getChar(item_index, ITEM_NAME);
  if (arg[0] == '*') {
    CHAR_talkToCli(char_index, -1,
                   "绑定的物品不能丢弃！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif
#ifdef _DROPCHECK2
  fl = CHAR_getInt(char_index, CHAR_FLOOR);
  x = CHAR_getInt(char_index, CHAR_X);
  y = CHAR_getInt(char_index, CHAR_Y);

  for (i = x - CHAR_DEFAULTSEESIZ / 2; i <= x + CHAR_DEFAULTSEESIZ / 2; i++) {
    for (j = y - CHAR_DEFAULTSEESIZ / 2; j <= y + CHAR_DEFAULTSEESIZ / 2; j++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int objindex = GET_OBJINDEX(object);
        if (OBJECT_getType(objindex) == OBJTYPE_NOUSE)
          continue;
        if (OBJECT_getType(objindex) == OBJTYPE_ITEM ||
            OBJECT_getType(objindex) == OBJTYPE_GOLD) {
          count_item++;
        }
        if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
          count_chara++;
        }
      }
    }

#ifdef _DROPSTAKENEW
    if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
      int k = 0;
      int casinodropflag = 0;
      for (k = 0; k < arraysizeof(casinomap); k++) {
        if (fl == casinomap[k].casinofl) {
          if (x >= casinomap[k].casinox - 1 && x <= casinomap[k].casinox + 1) {
            if (y >= casinomap[k].casinoy - 1 &&
                y <= casinomap[k].casinoy + 1) {
              casinodropflag = 1;
              break;
            }
          }
        }
      }
      if (casinodropflag == 0) {
#endif
        if (count_item > 80 || count_chara > 80) {
          CHAR_talkToCli(char_index, -1, "这里的物品已经太多了，不能再丢了。",
                         CHAR_COLORYELLOW);
          return;
        }
#ifdef _DROPSTAKENEW
      }
    }
#endif
  }
#endif

  for (i = 1; i < 8; i++) {
    dirx[i] = CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR) + i);
    diry[i] = CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR) + i);
  }
  dirx[0] = CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR));
  diry[0] = CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR));
  dirx[8] = 0;
  diry[8] = 0;
  floor = CHAR_getInt(char_index, CHAR_FLOOR);
  {
    int ret;
    if (0 <= itemchar_index &&
        itemchar_index < CHAR_STARTITEMARRAY) { //如果丢弃物为装备
      CHAR_sendItemDetachEvent(char_index, item_index);
    }
    ret = ITEM_eventDrop(item_index, char_index, itemchar_index);
    if (ret == 1) { //-1 物品不存在 1 消失 0 一般物品
      item_index = -1;
      goto END;
    } else if (ret == -1) {
      print("%s:%d err\n", __FILE__, __LINE__);
      return;
    } else {
      ;
    }
  }

  droped = 0;
  for (i = 0; i < 9; i++) {
    int x = CHAR_getInt(char_index, CHAR_X) + dirx[i];
    int y = CHAR_getInt(char_index, CHAR_Y) + diry[i];
#ifdef _DROPSTAKENEW
    // 判断物品是否为彩券
    if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
      int j = 0, k = 0;
      int bdropstake = 0;
      int casinoflag = 0;
      for (j = 0; j < arraysizeof(casinomap); j++) {
        if (CHAR_getInt(char_index, CHAR_FLOOR) == casinomap[j].casinofl) {
          if (x == casinomap[j].casinox && y == casinomap[j].casinoy) {
            char tmpbuf[256];
            int dropflag = CHAR_getWorkInt(char_index, CHAR_WORKSTAKEFLAG);
            casinoflag = 1;
            if (casinomap[j].dropflag == 0) {
              CHAR_talkToCli(char_index, -1,
                             "现在无法下注！",
                             CHAR_COLORYELLOW);
              return;
            } else {
              snprintf(tmpbuf, sizeof(tmpbuf), "你在 %s 下注了一张彩券",
                       casinomap[j].casinoinfo);
              if (dropflag >= MAXSTAKENUM) {
                snprintf(tmpbuf, sizeof(tmpbuf), "DROP FLAG 大于最大的数值");
                CHAR_talkToCli(char_index, -1, tmpbuf, CHAR_COLORYELLOW);
                return;
              }
              for (k = 0; k < MAXSTAKENUM; k++) {
                if (CHAR_getWorkInt(char_index, CHAR_WORKSTAKETYPE1 + k) == 0) {
                  CHAR_setWorkInt(char_index, CHAR_WORKSTAKETYPE1 + k,
                                  casinomap[j].casinotype);
                  casinomap[j].stakenum = casinomap[j].stakenum + 1;
                  if (casinomap[j].stakenum >= 100000000)
                    casinomap[j].stakenum = 100000000;
                  dropflag++;
                  if (casinomap[j].stakenum <= 1)
                    bdropstake = 1;
                  break;
                }
              }
            }
            CHAR_talkToCli(char_index, -1, tmpbuf, CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
            // 做扣点的动作
            if (dropflag <= MAXSTAKENUM) {
              int nAcc = CHAR_getInt(char_index, CHAR_GAMBLENUM);
              nAcc -= casinomap[j].accumulation;
              CHAR_setInt(char_index, CHAR_GAMBLENUM, nAcc);
            }
#endif
            if (dropflag >= MAXSTAKENUM) {
              dropflag = 0;
              for (k = 0; k < MAXSTAKENUM; k++) {
                if (CHAR_getWorkInt(char_index, CHAR_WORKSTAKETYPE1 + k) > 0)
                  dropflag++;
              }
              CHAR_setWorkInt(char_index, CHAR_WORKSTAKEFLAG, dropflag);
            } else {
              CHAR_setWorkInt(char_index, CHAR_WORKSTAKEFLAG, dropflag);
            }
            continue;
          }
        }
      }

      // 此地方的彩券数量超过一张以上
      if (bdropstake == 0 && casinoflag != 0) {
        CHAR_setItemIndex(char_index, itemchar_index, -1);
        CHAR_sendItemDataOne(char_index, itemchar_index);
        LogItem(CHAR_getChar(char_index, CHAR_NAME),
                CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
                item_index,
#else
                ITEM_getInt(item_index, ITEM_ID),
#endif
                "StakeDrop(丢出彩券)", CHAR_getInt(char_index, CHAR_FLOOR),
                CHAR_getInt(char_index, CHAR_X),
                CHAR_getInt(char_index, CHAR_Y),
                ITEM_getChar(item_index, ITEM_UNIQUECODE),
                ITEM_getChar(item_index, ITEM_NAME),
                ITEM_getInt(item_index, ITEM_ID)

        );
        return;
      }
    }
#endif
    {
      int ret = CHAR_DropItemFXY(char_index, itemchar_index,
                                 CHAR_getInt(char_index, CHAR_FLOOR), x, y,
                                 &objindex);
      switch (ret) {
      case 0:
        droped = 1;
#ifdef _ITEM_ORNAMENTS // WON FIX
        if (ITEM_getWorkInt(item_index, ITEM_CANPICKUP) > 0) {
          ITEM_setInt(item_index, ITEM_PUTTIME, NowTime.tv_sec + 60 * 5);
        } else {
          ITEM_setInt(item_index, ITEM_PUTTIME, NowTime.tv_sec);
        }
#else
        ITEM_setInt(item_index, ITEM_PUTTIME, NowTime.tv_sec);
#endif
        goto END;
        break;
      case -1:
      case -2:
        break;
      case -3:
      default:
        return;
        break;
      }
#ifdef _DROPSTAKENEW
    }
#endif
  }
#ifdef _DROPCHECK
  if (droped != 1) {
    CHAR_talkToCli(char_index, -1, "不能丢弃物品", CHAR_COLORYELLOW);
    return;
  }
#endif
END:
  CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
  CHAR_setItemIndex(char_index, itemchar_index, -1);
  {
    char category[3];
    snprintf(category, sizeof(category), "J%d", itemchar_index);
    CHAR_sendStatusString(char_index, category);
  }
  if (CHAR_complianceParameter(char_index)) {
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
  }
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_send_P_StatusString(
        char_index, CHAR_P_STRING_HP | CHAR_P_STRING_MAXHP | CHAR_P_STRING_MP |
                        CHAR_P_STRING_MAXMP | CHAR_P_STRING_ATK |
                        CHAR_P_STRING_DEF | CHAR_P_STRING_QUICK |
                        CHAR_P_STRING_CHARM | CHAR_P_STRING_LUCK |
                        CHAR_P_STRING_EARTH | CHAR_P_STRING_WATER |
                        CHAR_P_STRING_FIRE | CHAR_P_STRING_WIND);
    CHAR_sendItemDataOne(char_index, itemchar_index);
  }
  return;
}
#endif

int CHAR_DropItemAbsolute(int item_index, int floor, int x, int y, BOOL net) {
  Object object;
  int objindex;

  if (!ITEM_CHECKINDEX(item_index))
    return FALSE;

  object.type = OBJTYPE_ITEM;
  object.index = item_index;
  object.x = x;
  object.y = y;
  object.floor = floor;
  objindex = initObjectOne(&object);
  ITEM_setWorkInt(item_index, ITEM_WORKOBJINDEX, objindex);
  ITEM_setWorkInt(item_index, ITEM_WORKCHARAINDEX, -1);
  if (net)
    CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);

  return objindex;
}

int CHAR_addItemSpecificItemIndex(int char_index, int item_index) {
  int emptyindex;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return -1;
  if (ITEM_CHECKINDEX(item_index) == FALSE)
    return -1;
  emptyindex = CHAR_findEmptyItemBox(char_index);
  if (emptyindex == -1) {
    return CheckCharMaxItem(char_index);
  } else {
    CHAR_setItemIndex(char_index, emptyindex, item_index);
    ITEM_setWorkInt(item_index, ITEM_WORKOBJINDEX, -1);
    ITEM_setWorkInt(item_index, ITEM_WORKCHARAINDEX, char_index);
    CHAR_sendItemDataOne(char_index, emptyindex);
  }
  return emptyindex;
}

static int CHAR_PickUpItemFXY(int char_index, int fl, int x, int y,
                              int *contents, int *objindex) {
  OBJECT object;

  for (object = MAP_getTopObj(fl, x, y); object; object = NEXT_OBJECT(object)) {
    int index = GET_OBJINDEX(object);
    int objtype = OBJECT_getType(index);
    if (objtype == OBJTYPE_NOUSE)
      continue;
    else if (objtype == OBJTYPE_ITEM) {
      int ret;
      int item_index = OBJECT_getIndex(index);
#ifdef _ITEM_ORNAMENTS
      if (ITEM_getWorkInt(item_index, ITEM_CANPICKUP) > 0) {
        return -1;
      }
#endif
#ifdef _PET_AND_ITEM_UP
      if (strcmp(getItemup(), "否") == 0) {
        if (strcmp(ITEM_getChar(item_index, ITEM_CDKEY),
                   CHAR_getChar(char_index, CHAR_CDKEY)) != 0) {
          if (ITEM_getInt(item_index, ITEM_ID) != 12808 &&
              ITEM_getInt(item_index, ITEM_ID) != 2400 &&
              ITEM_getInt(item_index, ITEM_ID) != 2174) {
            return -6;
          }
        }
      }
#endif

#ifdef _DROPSTAKENEW
      // 判断物品是否为彩券
      if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
        int i = 0;
        int bdropstake = 0, casinoflag = 0, emptyitem_indexinchara = -1;
        for (i = 0; i < arraysizeof(casinomap); i++) {
          if (fl == casinomap[i].casinofl) {
            if (x == casinomap[i].casinox && y == casinomap[i].casinoy) {
              int j = 0, stakeflag = 0;
              int pickupflag = 0;
              casinoflag = 1;
              if (casinomap[i].dropflag == 0) {
                CHAR_talkToCli(
                    char_index, -1,
                    "现在已经无法取回下注彩券了！"
                    "现在已经无法取回下注彩券了！",
                    CHAR_COLORYELLOW);
                return -1;
              }
              stakeflag = CHAR_getWorkInt(char_index, CHAR_WORKSTAKEFLAG);
              if (stakeflag == 0) {
                CHAR_talkToCli(char_index, -1,
                               "您并没有下注，所以无法取回彩券！"
                               "ȯ"
                               "您并没有下注，所以无法取回彩券！"
                               "您并没有下注，所以无法取回彩券！",
                               CHAR_COLORYELLOW);
                return -1;
              }
              for (j = 0; j < MAXSTAKENUM; j++) {
                if (CHAR_getWorkInt(char_index, CHAR_WORKSTAKETYPE1 + j) ==
                    casinomap[i].casinotype) {
                  emptyitem_indexinchara = CHAR_findEmptyItemBox(char_index);
                  if (emptyitem_indexinchara < 0)
                    return -2;
                  CHAR_setWorkInt(char_index, CHAR_WORKSTAKETYPE1 + j, 0);
                  casinomap[i].stakenum = casinomap[i].stakenum - 1;
                  stakeflag--;
                  pickupflag = 1;
#ifdef _FIX_GAMBLENUM
                  // 做加点的动作
                  {
                    int nAcc = CHAR_getInt(char_index, CHAR_GAMBLENUM);
                    nAcc += casinomap[i].accumulation;
                    CHAR_setInt(char_index, CHAR_GAMBLENUM, nAcc);
                  }
#endif
                  if (casinomap[i].stakenum <= 0) {
                    bdropstake = 1;
                    casinomap[i].stakenum = 0;
                  }
                  break;
                }
              }
              if (stakeflag < 0) {
                stakeflag = 0;
                for (j = 0; j < MAXSTAKENUM; j++) {
                  if (CHAR_getWorkInt(char_index, CHAR_WORKSTAKETYPE1 + j) > 0)
                    stakeflag++;
                }
                CHAR_setWorkInt(char_index, CHAR_WORKSTAKEFLAG, stakeflag);
              } else
                CHAR_setWorkInt(char_index, CHAR_WORKSTAKEFLAG, stakeflag);
              if (pickupflag == 0) {
                CHAR_talkToCli(char_index, -1,
                               "您没有在此下注，所以无法取回彩券！"
                               "ȯ"
                               "您没有在此下注，所以无法取回彩券！"
                               "您没有在此下注，所以无法取回彩券！",
                               CHAR_COLORYELLOW);
                return -1;
              }
            }
          }
        }

        if (bdropstake == 0 && casinoflag != 0) { // 此地方的彩券数量超过一张以上
          char tmpbuf[256];
          if (emptyitem_indexinchara != -1) {
            int itemid = -1, ret = -1;
            itemid = ITEM_makeItemAndRegist(CASINOTOKENID);
            if (itemid == -1)
              return -1;
            ret = CHAR_addItemSpecificItemIndex(char_index, itemid);
            if (ret < 0 || ret >= CheckCharMaxItem(char_index)) {
              ITEM_endExistItemsOne(itemid);
              return -1;
            }
          }
          CHAR_sendItemDataOne(char_index, emptyitem_indexinchara);
          snprintf(tmpbuf, sizeof(tmpbuf), "拾回%s，已将下注取消！",
                   ITEM_getChar(item_index, ITEM_NAME));
          CHAR_talkToCli(char_index, -1, tmpbuf, CHAR_COLORWHITE);
          // 记录捡回记录
          LogItem(CHAR_getChar(char_index, CHAR_NAME),
                  CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
                  item_index,
#else
                  ITEM_getInt(item_index, ITEM_ID),
#endif
                  "StakePickUp(捡起彩券)", CHAR_getInt(char_index, CHAR_FLOOR),
                  CHAR_getInt(char_index, CHAR_X),
                  CHAR_getInt(char_index, CHAR_Y),
                  ITEM_getChar(item_index, ITEM_UNIQUECODE),
                  ITEM_getChar(item_index, ITEM_NAME),
                  ITEM_getInt(item_index, ITEM_ID));
          return -1;
        }
      }
#endif
      ret = CHAR_addItemSpecificItemIndex(char_index, item_index);
      if (ret == CheckCharMaxItem(char_index)) {
        return -2;
      } else if (ret != -1) {
        typedef void (*PICKUPFUNC)(int, int);
        PICKUPFUNC pickupfunc = NULL;
        pickupfunc =
            (PICKUPFUNC)ITEM_getFunctionPointer(item_index, ITEM_PICKUPFUNC);
        if (pickupfunc) {
          pickupfunc(char_index, item_index);
        }
        *contents = OBJECT_getIndex(index);
        *objindex = index;
        return 0;
      }
    } else if (objtype == OBJTYPE_GOLD) {
      int MaxGold;
      long dropgoldamount = 0, newamountgold = 0, Minus = 0, newgold = 0;
      long mygold = (long)CHAR_getInt(char_index, CHAR_GOLD);
      MaxGold = CHAR_getMaxHaveGold(char_index);
      if ((mygold < 0) || (mygold > MaxGold))
        return -2;
      dropgoldamount = OBJECT_getIndex(index);
      newamountgold =
          (unsigned int)CHAR_getInt(char_index, CHAR_GOLD) + dropgoldamount;
      Minus = newamountgold - MaxGold;
      if (Minus > 0) {
        newgold = Minus;
        *contents = dropgoldamount - Minus;
      } else {
        newgold = 0;
        *contents = dropgoldamount;
      }
      OBJECT_setIndex(index, newgold);
      CHAR_setInt(char_index, CHAR_GOLD, min(newamountgold, MaxGold));
      *objindex = index;
      return 1;
    } else if (objtype == OBJTYPE_CHARA) {
      int pindex = OBJECT_getIndex(index);
      if (!CHAR_CHECKINDEX(pindex))
        continue;
      if (CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
        if (CHAR_getWorkInt(pindex, CHAR_WORKPETFOLLOWMODE) == 3) {
          return -7;
        }
        int havepetindex = CHAR_getCharPetElement(char_index);
        if (havepetindex == -1) {
          if (CHAR_getWorkInt(pindex, CHAR_WORKPETFOLLOWMODE) !=
              CHAR_PETFOLLOW_NOW)
            return -3;
        }
        if (CHAR_getWorkInt(char_index, CHAR_PickAllPet) != TRUE) {
          if ((CHAR_getInt(pindex, CHAR_LV) - CHAR_getInt(char_index, CHAR_LV) >
               20) &&
              (CHAR_getInt(char_index, CHAR_TRANSMIGRATION) <= 0)) {
            return -4;
          }
        }
#ifdef _PET_AND_ITEM_UP
        if (strcmp(getPetup(), "否") == 0)
#endif
        {
          if (!strcmp(CHAR_getChar(pindex, CHAR_OWNERCDKEY), "SYSTEM_WAYI") &&
              !strcmp(CHAR_getChar(pindex, CHAR_OWNERCHARANAME),
                      "SYSTEM_WAYI")) {
          } else {
            if (strcmp(CHAR_getChar(pindex, CHAR_OWNERCDKEY),
                       CHAR_getChar(char_index, CHAR_CDKEY)) ||
                strcmp(CHAR_getChar(pindex, CHAR_OWNERCHARANAME),
                       CHAR_getChar(char_index, CHAR_NAME))) {
              return -5;
            }
          }
        }
        if (CHAR_getWorkInt(pindex, CHAR_WORKPETFOLLOWMODE) ==
            CHAR_PETFOLLOW_NOW) {
          CHAR_pickupFollowPet(char_index, pindex);
          return -1;
        }
        if (CHAR_getInt(pindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) {
          if (strcmp(CHAR_getChar(pindex, CHAR_OWNERCDKEY),
                     CHAR_getChar(char_index, CHAR_CDKEY)) != 0 ||
              strcmp(CHAR_getChar(pindex, CHAR_OWNERCHARANAME),
                     CHAR_getChar(char_index, CHAR_NAME)) != 0) {
            return -5;
          } else if (CHAR_getInt(pindex, CHAR_MAILMODE) != CHAR_PETMAIL_IDLE5) {
            return -5;
          }
        }
        if (CHAR_getInt(pindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE) {
          Char *ch;
          CHAR_setInt(pindex, CHAR_MAILMODE, CHAR_PETMAIL_NONE);
          CHAR_setInt(pindex, CHAR_PETMAILIDLETIME, 0);
          PETMAIL_delPetMailTotalnums(1); // 手动捡起mail PET
          ch = CHAR_getCharPointer(pindex);
          if (ch != NULL) {
            strncpysafe(ch->charfunctable[CHAR_LOOPFUNC].string,
                       sizeof(ch->charfunctable[CHAR_LOOPFUNC]), "");
            CHAR_constructFunctable(pindex);
          }
        }
        CHAR_setWorkInt(pindex, CHAR_WORKPLAYERINDEX, char_index);
        CHAR_setCharPet(char_index, havepetindex, pindex);
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
        if (CHAR_getInt(pindex, CHAR_PETID) != 718) {
#endif
#endif
          CHAR_setChar(pindex, CHAR_OWNERCDKEY,
                       CHAR_getChar(char_index, CHAR_CDKEY));
          CHAR_setChar(pindex, CHAR_OWNERCHARANAME,
                       CHAR_getChar(char_index, CHAR_NAME));
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
        }
#endif
#endif
#ifdef _PET_ITEM
        CHAR_sendPetItemData(char_index, havepetindex);
#endif
        CHAR_complianceParameter(pindex);
        {
          char category[3];
          snprintf(category, sizeof(category), "K%d", havepetindex);
          CHAR_sendStatusString(char_index, category);
          snprintf(category, sizeof(category), "W%d", havepetindex);
          CHAR_sendStatusString(char_index, category);
        }
        CHAR_setInt(pindex, CHAR_PUTPETTIME, 0);
        *contents = pindex;
        *objindex = index;
        return 2;
      }
    }
  }
  return -1;
}

void CHAR_PickUpItem(int char_index, int dir) {
  int i;
  int dirx[2], diry[2];
  int searchloopcount = 0;

  if (!CHAR_CHECKINDEX(char_index))
    return;
  if (CHAR_getFlg(char_index, CHAR_ISDIE))
    return;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE))
    return;
  if (dir < 0) {
    searchloopcount = 1;
    dirx[0] = 0;
    diry[0] = 0;
  } else {
    if (dir != CHAR_getInt(char_index, CHAR_DIR)) {
      VALIDATEDIR(dir);
      CHAR_setInt(char_index, CHAR_DIR, dir);
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
    }
    dirx[0] = CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR));
    diry[0] = CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR));
    dirx[1] = 0;
    diry[1] = 0;
    searchloopcount = 2;
  }
  for (i = 0; i < searchloopcount; i++) {
    int contents, objindex;
    int fl = CHAR_getInt(char_index, CHAR_FLOOR);

    int y = CHAR_getInt(char_index, CHAR_Y) + diry[i];
    int x = CHAR_getInt(char_index, CHAR_X) + dirx[i];
    int ret = CHAR_PickUpItemFXY(char_index, fl, x, y, &contents, &objindex);
    switch (ret) {
    case 0:
      CHAR_ObjectDelete(objindex);
      {
        char mesg[256];
#ifdef _DROPSTAKENEW
        int j = 0;
        int dropflag = 0;
        for (j = 0; j < arraysizeof(casinomap); j++) {
          if (CHAR_getInt(char_index, CHAR_FLOOR) == casinomap[j].casinofl) {
            if (x == casinomap[j].casinox && y == casinomap[j].casinoy) {
              dropflag = 1;
              break;
            }
          }
        }
        if (ITEM_getInt(contents, ITEM_ID) == CASINOTOKENID && dropflag == 1)
          snprintf(mesg, sizeof(mesg), "拾回%s，已将下注取消！",
                   ITEM_getAppropriateName(contents));
        else
#endif
          snprintf(mesg, sizeof(mesg), "拾获 %s",
                   ITEM_getAppropriateName(contents));
        CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
      }

      {
        LogItem(
            CHAR_getChar(char_index, CHAR_NAME),
            CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
            contents,
#else
            ITEM_getInt(contents, ITEM_ID),
#endif
            "PickUp(捡起道具)", CHAR_getInt(char_index, CHAR_FLOOR),
            CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
            ITEM_getChar(contents, ITEM_UNIQUECODE),
            ITEM_getChar(contents, ITEM_NAME), ITEM_getInt(contents, ITEM_ID));
      }

      return;
      break;
    case 1:
      CHAR_send_P_StatusString(char_index, CHAR_P_STRING_GOLD);

      if (OBJECT_getIndex(objindex) == 0) {
        CHAR_ObjectDelete(objindex);
      } else {
        CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
      }
      {
        char mesg[256];
        if (contents == 0) {
          snprintf(mesg, sizeof(mesg), "无法再拾获Stone了。");
        } else {
          snprintf(mesg, sizeof(mesg), "拾获 %d Stone", contents);
        }
        CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
      }
      if (contents != 0) {
        LogStone(-1, CHAR_getChar(char_index, CHAR_NAME),
                 CHAR_getChar(char_index, CHAR_CDKEY), contents,
                 CHAR_getInt(char_index, CHAR_GOLD), "PickUp(拾获金钱)",
                 CHAR_getInt(char_index, CHAR_FLOOR),
                 CHAR_getInt(char_index, CHAR_X),
                 CHAR_getInt(char_index, CHAR_Y));
      }
      return;
      break;
    case 2: // 宠物
      CHAR_ObjectDelete(objindex);
      CHAR_setWorkInt(contents, CHAR_WORKOBJINDEX, -1);
      {
        char mesg[256];
        char *p = NULL;
        p = CHAR_getChar(contents, CHAR_USERPETNAME);
        if (strlen(p) == 0) {
          p = CHAR_getChar(contents, CHAR_NAME);
        }

        snprintf(mesg, sizeof(mesg), "拾获 %s", p);
        CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
      }
      // 矢永玄毛胶匀凶夫弘
      LogPet(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
             CHAR_getChar(char_index, CHAR_CDKEY),
             CHAR_getChar(contents, CHAR_NAME), CHAR_getInt(contents, CHAR_LV),
             "PickUp(捡宠)", CHAR_getInt(char_index, CHAR_FLOOR),
             CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
             CHAR_getChar(contents, CHAR_UNIQUECODE) // shan 2001/12/14
      );
      return;
      break;
    // Robin 0701 petFollow
    case 3:

      /*  引歹曰卞仇木毛壅六午中丹    */
      CHAR_ObjectDelete(objindex);
      CHAR_setWorkInt(contents, CHAR_WORKOBJINDEX, -1);
      {
        char mesg[256];
        char *p = NULL;
        p = CHAR_getChar(contents, CHAR_USERPETNAME);
        if (strlen(p) == 0) {
          p = CHAR_getChar(contents, CHAR_NAME);
        }

        snprintf(mesg, sizeof(mesg), "拾回 %s", p);
        CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORWHITE);
      }
      // 矢永玄毛胶匀凶夫弘
      LogPet(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
             CHAR_getChar(char_index, CHAR_CDKEY),
             CHAR_getChar(contents, CHAR_NAME), CHAR_getInt(contents, CHAR_LV),
             "FollowPickUp(收回溜宠)", CHAR_getInt(char_index, CHAR_FLOOR),
             CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
             CHAR_getChar(contents, CHAR_UNIQUECODE) // shan 2001/12/14
      );

      return;
      break;
    case -1:
      break;
    case -2:
      CHAR_talkToCli(char_index, -1, "道具栏已满。", CHAR_COLORYELLOW);
      break;
    case -3:
      CHAR_talkToCli(char_index, -1, "宠物数已满。", CHAR_COLORYELLOW);
      break;
    case -4:
      CHAR_talkToCli(char_index, -1, "无法照顾该宠物。",
                     CHAR_COLORYELLOW);
      break;
    case -5:
      CHAR_talkToCli(char_index, -1, "无法拾获该宠物。",
                     CHAR_COLORYELLOW);
      break;
    case -6:
      CHAR_talkToCli(char_index, -1, "无法拾获该道具。",
                     CHAR_COLORYELLOW);
      break;
    default:
      return;
    }
  }
  return;
}

/* 历史注释或停用代码的原始编码已损坏，无法可靠恢复。 */
static BOOL CHAR_DropMoneyFXY(int char_index, int amount, int fl, int x, int y,
                              BOOL force, int *objindex) {
  OBJECT object;
  int MaxGold;
  int mygold = CHAR_getInt(char_index, CHAR_GOLD);
  if (amount <= 0)
    return -6;
  if (amount > mygold)
    return -1;

  MaxGold = CHAR_getMaxHaveGold(char_index);
  if ((mygold < 0) || (mygold > MaxGold))
    return -1;

  if (MAP_walkAbleFromPoint(fl, x, y, FALSE) == FALSE)
    return -2;
  for (object = MAP_getTopObj(fl, x, y); object; object = NEXT_OBJECT(object)) {
    int index = GET_OBJINDEX(object);
    int objtype = OBJECT_getType(index);

    if (objtype == OBJTYPE_NOUSE)
      continue;
    else if (objtype == OBJTYPE_GOLD) {
      long tmpamount = OBJECT_getIndex(index) + amount;
      if (tmpamount > MaxGold)
        return -4;
      OBJECT_setIndex(index, tmpamount);
      CHAR_setInt(char_index, CHAR_GOLD,
                  (unsigned int)CHAR_getInt(char_index, CHAR_GOLD) - amount);
      *objindex = index;
#ifdef _DEL_DROP_GOLD
      OBJECT_setTime(index, NowTime.tv_sec);
#endif
      if (amount >= 100) { /* 云嗯毛  中凶夫弘 */
        LogStone(-1, CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
                 CHAR_getChar(char_index, CHAR_CDKEY),    /* 交□扒□ID */
                 amount,                                  /* 嗯喊 */
                 CHAR_getInt(char_index, CHAR_GOLD), "Drop(放置金钱)",
                 CHAR_getInt(char_index, CHAR_FLOOR),
                 CHAR_getInt(char_index, CHAR_X),
                 CHAR_getInt(char_index, CHAR_Y));
      }
      return 0;
    } else if (objtype == OBJTYPE_ITEM) {
      if (force == TRUE)
        continue;
      else
        /* 允匹卞失奶  丞互丐月及匹分户 */
        return -5;
    }
  }

  /* 仇仇引匹谗琼允月午中丹仪反蕙筋瓒  允月 */
  {
    Object one;
    one.type = OBJTYPE_GOLD;
    one.x = x;
    one.y = y;
    one.floor = fl;

    if (amount > MaxGold)
      return -4;

    one.index = amount;
    *objindex = initObjectOne(&one);
    /*  左皮斥尼弁玄瓒  允月    */
    if (*objindex == -1)
      /*  瓒  撩      */
      return -3;

    /*云嗯毛蛹日允*/
    CHAR_setInt(char_index, CHAR_GOLD,
                (unsigned int)CHAR_getInt(char_index, CHAR_GOLD) - amount);
#ifdef _DEL_DROP_GOLD
    OBJECT_setTime(*objindex, NowTime.tv_sec);
#endif
    if (amount >= 100) { /* 云嗯毛  中凶夫弘 */
      LogStone(-1, CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
               CHAR_getChar(char_index, CHAR_CDKEY),    /* 交□扒□ID */
               amount,                                  /* 嗯喊 */
               CHAR_getInt(char_index, CHAR_GOLD), "Drop(放置金钱)",
               CHAR_getInt(char_index, CHAR_FLOOR),
               CHAR_getInt(char_index, CHAR_X),
               CHAR_getInt(char_index, CHAR_Y));
    }
    return 0;
  }
}

/* 历史注释或停用代码的原始编码已损坏，无法可靠恢复。 */
void CHAR_DropMoney(int char_index, int amount) {
  int dirx[9], diry[9];
  int ret;
  int i;
  int objindex, MaxGold;

  if (!CHAR_CHECKINDEX(char_index))
    return;

#ifdef _AVID_TRADETRYBUG // 丢出石币
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING) {
    CHAR_talkToCli(char_index, -1,
                   "交易状态中无法丢出石币。",
                   CHAR_COLORYELLOW);
    return;
  }
#endif

  MaxGold = CHAR_getMaxHaveGold(char_index);
  if (CHAR_getFlg(char_index, CHAR_ISDIE))
    return;

  // ANDY_ADD	12/28
  {
    int money = 0;
    money = CHAR_getInt(char_index, CHAR_GOLD);
    if (money < 0) {
      CHAR_setInt(char_index, CHAR_GOLD, 0);
      return;
    }
    if ((amount > money) || (money > MaxGold)) {
      if (money > MaxGold) {
        CHAR_setInt(char_index, CHAR_GOLD, 0);
        print("\n ERR: %s(stone%d) dropMoney%d > CHAR_MAXGOLDHAVE !!",
              CHAR_getChar(char_index, CHAR_CDKEY), money, amount);
      } else {
        print("\n ERR: %s dropMoney > CHAR_GOLD !!",
              CHAR_getChar(char_index, CHAR_CDKEY));
      }
      CHAR_send_P_StatusString(char_index, CHAR_P_STRING_GOLD);
      return;
    }
  }
  // ANDY_END
  for (i = 0; i < 7; i++) {
    dirx[i + 2] = CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR) + i + 1);
    diry[i + 2] = CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR) + i + 1);
  }

  dirx[0] = CHAR_getDX(CHAR_getInt(char_index, CHAR_DIR));
  diry[0] = CHAR_getDY(CHAR_getInt(char_index, CHAR_DIR));
  dirx[1] = 0;
  diry[1] = 0;

  for (i = 0; i < 9; i++) {
    int x = CHAR_getInt(char_index, CHAR_X) + dirx[i];
    int y = CHAR_getInt(char_index, CHAR_Y) + diry[i];

// CoolFish 2002/04/06 下注处不可丢置石币
#ifdef _DROPSTAKENEW
    {
      int j = 0;
      for (j = 0; j < arraysizeof(casinomap); j++) {
        if (CHAR_getInt(char_index, CHAR_FLOOR) == casinomap[j].casinofl) {
          if (x == casinomap[j].casinox && y == casinomap[j].casinoy) {
            CHAR_talkToCli(char_index, -1, "这里不能放置石币！", CHAR_COLORYELLOW);
            return;
          }
        }
      }
    }
#endif
    ret = CHAR_DropMoneyFXY(char_index, amount,
                            CHAR_getInt(char_index, CHAR_FLOOR), x, y, FALSE,
                            &objindex);
    switch (ret) {
    case 0:
      goto END;
      break;
    case -1:
      return;
      break;
    case -3:
      return;
      break;
    case -2:
    case -4:
    case -5:
    case -6:
      break;
    default:
      break;
    }
  }

  {
    int x = CHAR_getInt(char_index, CHAR_X) + dirx[1];
    int y = CHAR_getInt(char_index, CHAR_Y) + diry[1];

    ret = CHAR_DropMoneyFXY(char_index, amount,
                            CHAR_getInt(char_index, CHAR_FLOOR), x, y, TRUE,
                            &objindex);
    switch (ret) {
    case 0:
      goto END;
      break;
    case -1:
      break;
    case -3:
      return;
      break;
    case -2:
    case -4:
    case -5:
      break;
    default:
      break;
    }
    return;
  }

END:
  CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_GOLD);
  return;
}

static int CHAR_findEmptyItemBoxNoFromChar(Char *ch) {
  int i, j = 0;
  ;
  int itemMax = CheckCharMaxItemChar(ch);
  for (i = CHAR_STARTITEMARRAY; i < itemMax; i++) {
    int item_index;
    item_index = ch->indexOfExistItems[i];
    if (item_index == -1)
      j++;
  }
  return j;
}

int CHAR_findEmptyItemBoxNo(int index) {
  Char *ch;

  if (!CHAR_CHECKINDEX(index))
    return -1;
  ch = CHAR_getCharPointer(index);
  if (ch != NULL)
    return CHAR_findEmptyItemBoxNoFromChar(ch);
  else
    return -1;
}

// CoolFish: Trade 2001/4/4
int CHAR_findTotalEmptyItem(int index) {
  int i, totaleptitem = 0;
  Char *ch;

  if (!CHAR_CHECKINDEX(index))
    return -1;
  ch = CHAR_getCharPointer(index);
  if (ch != NULL) {
    for (i = CHAR_STARTITEMARRAY; i < CheckCharMaxItem(index); i++) {
      int item_index = 0;
      item_index = ch->indexOfExistItems[i];
      if (item_index == -1)
        totaleptitem++;
    }
    return totaleptitem;
  }

  return -1;
}

int CHAR_pickupFollowPet(int char_index, int pickupindex) {
  int petindex;
  int objindex;
  int havepetindex;
  char category[3];
  if (!CHAR_CHECKINDEX(char_index))
    return FALSE;
  if (pickupindex != -1)
    petindex = pickupindex;
  else {
#ifdef _PETFOLLOW_NEW_
    int petnum;
    petnum = 0;
    for (; petnum < 5; petnum++) {
      petindex = CHAR_getWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum);
      if (!CHAR_CHECKINDEX(petindex)) {
        CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum, -1);
      } else {
#ifdef _PETFOLLOW_NEW_
        int havepetindex = CHAR_getCharPetElementOld(char_index);
#else
        int havepetindex = CHAR_getCharPetElement(char_index);
#endif
        if (havepetindex == -1) {
          CHAR_talkToCli(
              char_index, -1,
              "宠物栏已满！无法拾回！",
              CHAR_COLORYELLOW);
          return FALSE;
        }
        objindex = CHAR_getWorkInt(petindex, CHAR_WORKOBJINDEX);
        if ((pickupindex == -1) ||
            (pickupindex ==
             CHAR_getWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum)))
          CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum, -1);
        if (strcmp(CHAR_getChar(petindex, CHAR_OWNERCDKEY),
                   CHAR_getChar(char_index, CHAR_CDKEY)) != 0 ||
            strcmp(CHAR_getChar(petindex, CHAR_OWNERCHARANAME),
                   CHAR_getChar(char_index, CHAR_NAME)) != 0) {
          return FALSE;
        }
        CHAR_ObjectDelete(objindex);
        CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NONE);
        CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, char_index);
        CHAR_setCharPet(char_index, havepetindex, petindex);
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
        if (CHAR_getInt(petindex, CHAR_PETID) != 718) {
#endif
#endif
          CHAR_setChar(petindex, CHAR_OWNERCDKEY,
                       CHAR_getChar(char_index, CHAR_CDKEY));
          CHAR_setChar(petindex, CHAR_OWNERCHARANAME,
                       CHAR_getChar(char_index, CHAR_NAME));
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
        }
#endif
#endif
        snprintf(category, sizeof(category), "K%d", havepetindex);
        CHAR_sendStatusString(char_index, category);
        snprintf(category, sizeof(category), "W%d", havepetindex);
        CHAR_sendStatusString(char_index, category);
        CHAR_setInt(petindex, CHAR_PUTPETTIME, 0);
        CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
        CHAR_setWorkInt(petindex, CHAR_WORKOBJINDEX, -1);
        {
          char mesg[256];
          char *p = NULL;
          p = CHAR_getChar(petindex, CHAR_USERPETNAME);
          if (strlen(p) == 0) {
            p = CHAR_getChar(petindex, CHAR_NAME);
          }
          snprintf(mesg, sizeof(mesg), "拾回 %s", p);
          CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORYELLOW);
        }
        LogPet(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
               CHAR_getChar(char_index, CHAR_CDKEY),
               CHAR_getChar(petindex, CHAR_NAME),
               CHAR_getInt(petindex, CHAR_LV), "FollowPickUp(收回溜宠)",
               CHAR_getInt(char_index, CHAR_FLOOR),
               CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
               CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
        );
      }
    }
    return TRUE;
#else
    petindex = CHAR_getWorkInt(char_index, CHAR_WORKPETFOLLOW);
#endif
  }
  if (!CHAR_CHECKINDEX(petindex)) {
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW, -1);
#ifdef _PETFOLLOW_NEW_
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + 1, -1);
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + 2, -1);
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + 3, -1);
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + 4, -1);
#endif
    return FALSE;
  }
#ifdef _PETFOLLOW_NEW_
  havepetindex = CHAR_getCharPetElementOld(char_index);
#else
  havepetindex = CHAR_getCharPetElement(char_index);
#endif
  if (havepetindex == -1) {
    CHAR_talkToCli(char_index, -1,
                   "宠物栏已满！无法拾回！",
                   CHAR_COLORYELLOW);
    return FALSE;
  }
  objindex = CHAR_getWorkInt(petindex, CHAR_WORKOBJINDEX);

#ifdef _PETFOLLOW_NEW_
  int petnum;
  petnum = 0;
  for (; petnum < 5; petnum++) {
    if ((pickupindex == -1) ||
        (pickupindex ==
         CHAR_getWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum)))
      CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW + petnum, -1);
  }
#else
  if ((pickupindex == -1) ||
      (pickupindex == CHAR_getWorkInt(char_index, CHAR_WORKPETFOLLOW)))
    CHAR_setWorkInt(char_index, CHAR_WORKPETFOLLOW, -1);
#endif
  /* CDKEY 午平乓仿  互域谯仄卅中午五反胶尹卅中 */
  if (strcmp(CHAR_getChar(petindex, CHAR_OWNERCDKEY),
             CHAR_getChar(char_index, CHAR_CDKEY)) != 0 ||
      strcmp(CHAR_getChar(petindex, CHAR_OWNERCHARANAME),
             CHAR_getChar(char_index, CHAR_NAME)) != 0) {
    return FALSE;
  }
  CHAR_ObjectDelete(objindex);
  CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NONE);
  CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, char_index);
  CHAR_setCharPet(char_index, havepetindex, petindex);
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
  if (CHAR_getInt(petindex, CHAR_PETID) != 718) {
#endif
#endif
    CHAR_setChar(petindex, CHAR_OWNERCDKEY,
                 CHAR_getChar(char_index, CHAR_CDKEY));
    CHAR_setChar(petindex, CHAR_OWNERCHARANAME,
                 CHAR_getChar(char_index, CHAR_NAME));
#ifndef _FIX_COPY_MM_BUG
#ifdef _PET_TALK
  }
#endif
#endif
  snprintf(category, sizeof(category), "K%d", havepetindex);
  CHAR_sendStatusString(char_index, category);
  snprintf(category, sizeof(category), "W%d", havepetindex);
  CHAR_sendStatusString(char_index, category);
  CHAR_setInt(petindex, CHAR_PUTPETTIME, 0);
  CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
  CHAR_setWorkInt(petindex, CHAR_WORKOBJINDEX, -1);

  char mesg[256];
  char *p = NULL;
  p = CHAR_getChar(petindex, CHAR_USERPETNAME);
  if (strlen(p) == 0) {
    p = CHAR_getChar(petindex, CHAR_NAME);
  }
  snprintf(mesg, sizeof(mesg), "拾回 %s", p);
  CHAR_talkToCli(char_index, -1, mesg, CHAR_COLORYELLOW);
  LogPet(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
         CHAR_getChar(char_index, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "FollowPickUp(收回溜宠)", CHAR_getInt(char_index, CHAR_FLOOR),
         CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );
  return TRUE;
}
#ifdef _GAMBLE_ROULETTE
int NPC_MAPCLEANGOLD(int meindex, int floor) {
  OBJECT object;
  int x, y, fl;
  int End_type;
  fl = floor;

  for (End_type = 0; End_type < arraysizeof(casinomap); End_type++) {
    if (floor != casinomap[End_type].casinofl)
      continue;
    x = casinomap[End_type].casinox;
    y = casinomap[End_type].casinoy;

    for (object = MAP_getTopObj(fl, x, y); object;
         object = NEXT_OBJECT(object)) {
      int index = GET_OBJINDEX(object);
      int objtype = OBJECT_getType(index);
      int item_index = OBJECT_getIndex(index);

      if (objtype == OBJTYPE_NOUSE)
        continue;
      if (objtype == OBJTYPE_ITEM) {
        if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
          ITEM_endExistItemsOne(item_index);
          OBJECT_setIndex(index, 0);
          CHAR_ObjectDelete(index); // 清除
          print(" clean( %d )", index);
        }
        break;
      }
    }
    casinomap[End_type].stakenum = 0;
  }
  return 1;
}
#endif

#ifdef _DROPSTAKENEW
// casinotype : flg = 0 不分type，针对整层type做设定；扩充参数
// mapdropflag : TRUE = 可以下注;
int SetCasinoMap(int npcindex, int casinotype, int mapdropflag) {
  int npcfloor = CHAR_getInt(npcindex, CHAR_FLOOR);
  int i = 0;
  if (casinotype < 0 || mapdropflag < 0)
    return -1;
  for (i = 0; i < arraysizeof(casinomap); i++) {
    if (npcfloor == casinomap[i].casinofl) {
      if (casinotype != 0) {
        if (casinomap[i].casinotype == casinotype)
          casinomap[i].dropflag = mapdropflag;
      } else
        casinomap[i].dropflag = mapdropflag;
    }
  }
  return 1;
}

// 计算积分
int CasinoAccumulation(int charindex, int npcindex, int floor,
                       int wincasinotype) {
  int i = 0;
  int accumulation = CHAR_getInt(charindex, CHAR_GAMBLENUM);
  char tmpbuf[256];
  for (i = 0; i < MAXSTAKENUM; i++) {
    if (CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i) == wincasinotype) {
      int j = 0;
      for (j = 0; j < arraysizeof(casinomap); j++) {
        if (casinomap[j].casinotype == wincasinotype) {
#ifdef _FIX_GAMBLENUM
          // 加 原本积分及WIN积分
          accumulation = accumulation + casinomap[j].accumulation * 2;
#else
          accumulation += casinomap[j].accumulation;
#endif
          CHAR_setWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i, 0);
          break;
        }
      }
    }

    else if (CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i) != 0) {
      int j = 0;
      for (j = 0; j < arraysizeof(casinomap); j++) {
        if (casinomap[j].casinotype ==
            CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i)) {
#ifndef _FIX_GAMBLENUM
          accumulation -= casinomap[j].accumulation;
#endif
          CHAR_setWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i, 0);
          break;
        }
      }
    }
  }
  CHAR_setWorkInt(charindex, CHAR_WORKSTAKEFLAG, 0);
  if (accumulation < 0)
    accumulation = 0;
  else if (accumulation > MAXACCUMULATION)
    accumulation = MAXACCUMULATION;
  CHAR_setInt(charindex, CHAR_GAMBLENUM, accumulation);
  snprintf(tmpbuf, sizeof(tmpbuf), "您现在总共有%4d点游乐场积分", accumulation);
  CHAR_talkToCli(charindex, npcindex, tmpbuf, CHAR_COLORWHITE);
  return 1;
}

int CasinoPay(int npcindex, int wincasinotype) {
  int npcfloor = CHAR_getInt(npcindex, CHAR_FLOOR);
  int i = 0;
  int playernum = CHAR_getPlayerMaxNum();
  if (wincasinotype < 0)
    return FALSE;
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) == FALSE)
      continue;
    if (!CHAR_CHECKINDEX(i))
      continue;
    if (CHAR_getInt(i, CHAR_FLOOR) == npcfloor) {
      // 查询玩家是否有下注
      if (CHAR_getWorkInt(i, CHAR_WORKSTAKEFLAG) == 0)
        continue;
      // 计算积分
      CasinoAccumulation(i, npcindex, npcfloor, wincasinotype);
    }
  }
  // 清除此层地面彩券
  for (i = 0; i < arraysizeof(casinomap); i++) {
    OBJECT object;
    if (casinomap[i].casinofl != npcfloor)
      continue;
    for (object = MAP_getTopObj(casinomap[i].casinofl, casinomap[i].casinox,
                                casinomap[i].casinoy);
         object; object = NEXT_OBJECT(object)) {
      int index = GET_OBJINDEX(object);
      int objtype = OBJECT_getType(index);
      int item_index = OBJECT_getIndex(index);
      if (objtype == OBJTYPE_NOUSE)
        continue;
      else if (objtype == OBJTYPE_ITEM) {
        if (ITEM_getInt(item_index, ITEM_ID) == CASINOTOKENID) {
          OBJECT_setIndex(index, 0);
          CHAR_ObjectDelete(index);
        }
      }
    }
    casinomap[i].stakenum = 0;
  }
  return TRUE;
}
#endif

#ifdef _PET_ITEM
void CHAR_sendPetItemData(int char_index, int petid) {
  int i;
  char CHAR_statusSendBuffer[1024];
  int strlength = 2;
  int petindex = CHAR_getCharPet(char_index, petid);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  int fd = getfdFromCharaIndex(char_index);
  snprintf(CHAR_statusSendBuffer, sizeof(CHAR_statusSendBuffer), "B%d", petid);
  for (i = 0; i < CHAR_MAXPETITEMHAVE; i++) {
    char token[512];
    int item_index;
    item_index = CHAR_getItemIndex(petindex, i);
    if (item_index >= 0) {
      snprintf(token, sizeof(token), "%s|",
               ITEM_makeItemStatusString(-1, item_index));
    } else {
      snprintf(token, sizeof(token), "%s|", ITEM_makeItemFalseString());
    }
    strncpysafe(CHAR_statusSendBuffer + strlength,
               sizeof(CHAR_statusSendBuffer) - strlength, token);
    strlength += strlen(token);
    if (strlength >= arraysizeof(CHAR_statusSendBuffer))
      GmsvServer_S_send(fd, CHAR_statusSendBuffer);
  }
  dchop(CHAR_statusSendBuffer, "|");
  GmsvServer_S_send(fd, CHAR_statusSendBuffer);
}

void CHAR_movePetItem(int char_index, int petid, int fromindex, int toindex) {
  if (!CHAR_CHECKINDEX(char_index))
    return;
  if (CHAR_getFlg(char_index, CHAR_ISDIE))
    return;
  int petindex = CHAR_getCharPet(char_index, petid);
  char token[512];
  if (toindex == -1 && fromindex >= CHAR_EQUIPPLACENUM &&
      fromindex < CheckCharMaxItem(char_index)) {
    int item_index = CHAR_getItemIndex(char_index, fromindex);
    toindex = ITEM_getInt(item_index, ITEM_TYPE) - ITEM_PET_HEAD;
  } else if (toindex == -2) {
    if (fromindex < CHAR_PETITEMNUM)
      CHAR_talkToCli(
          char_index, -1,
          "无法丢弃宠物身上的物品。",
          CHAR_COLORYELLOW);
    else
      CHAR_DropItem(char_index, fromindex);
  }

  if ((fromindex >= 0 && fromindex < CHAR_PETITEMNUM &&
       toindex >= CHAR_EQUIPPLACENUM &&
       toindex < CheckCharMaxItem(char_index)) ||
      (toindex >= 0 && toindex < CHAR_PETITEMNUM &&
       fromindex >= CHAR_EQUIPPLACENUM &&
       fromindex < CheckCharMaxItem(char_index))) {
    if (fromindex >= CHAR_EQUIPPLACENUM) {
      int charitem_index = CHAR_getItemIndex(char_index, fromindex);
      int petitem_index = CHAR_getItemIndex(petindex, toindex);
      int type = ITEM_getInt(charitem_index, ITEM_TYPE);
      if (CHAR_getInt(petindex, CHAR_VARIABLEAI) < 100) {
        CHAR_talkToCli(
            char_index, -1,
            "无法训服宠物装备该物品。",
            CHAR_COLORRED);
        return;
      }
      if (type < ITEM_PET_HEAD || type > ITEM_PET_FEET ||
          (type - ITEM_PET_HEAD) != toindex) {
        CHAR_talkToCli(char_index, -1,
                       "无法装备该项物品。",
                       CHAR_COLORWHITE);
        return;
      }
      if (CHAR_getInt(petindex, CHAR_LV) <
          ITEM_getInt(charitem_index, ITEM_LEVEL)) {
        CHAR_talkToCli(char_index, -1, "宠物等级尚未到达装备该项物品！",
                       CHAR_COLORYELLOW);
        return;
      }
      if (ITEM_CHECKINDEX(petitem_index)) {
        sprintf(token, "宠物%s 卸下%s", CHAR_getChar(petindex, CHAR_NAME),
                ITEM_getChar(petitem_index, ITEM_NAME));
        CHAR_talkToCli(char_index, -1, token, CHAR_COLORWHITE);
      }
      sprintf(token, "宠物%s 装备%s", CHAR_getChar(petindex, CHAR_NAME),
              ITEM_getChar(charitem_index, ITEM_NAME));
      CHAR_talkToCli(char_index, -1, token, CHAR_COLORWHITE);
      CHAR_setItemIndex(char_index, fromindex, petitem_index);
      CHAR_setItemIndex(petindex, toindex, charitem_index);
      CHAR_sendItemDataOne(char_index, fromindex);
      CHAR_sendPetItemData(char_index, petid);
    } else {
      int charitem_index = CHAR_getItemIndex(char_index, toindex);
      int petitem_index = CHAR_getItemIndex(petindex, fromindex);
      int type = ITEM_getInt(charitem_index, ITEM_TYPE);
      sprintf(token, "宠物%s 卸下%s", CHAR_getChar(petindex, CHAR_NAME),
              ITEM_getChar(petitem_index, ITEM_NAME));
      CHAR_talkToCli(char_index, -1, token, CHAR_COLORWHITE);
      if (ITEM_CHECKINDEX(charitem_index)) {
        if (CHAR_getInt(petindex, CHAR_VARIABLEAI) < 100) {
          CHAR_talkToCli(
              char_index, -1,
              "无法训服宠物装备该物品。",
              CHAR_COLORRED);
          return;
        }
        /* 历史注释或停用代码的原始编码已损坏，无法可靠恢复。 */
        if (type < ITEM_PET_HEAD || type > ITEM_PET_FEET ||
            (type - ITEM_PET_HEAD) != toindex) {
          CHAR_talkToCli(char_index, -1,
                         "无法装备该项物品。",
                         CHAR_COLORWHITE);
          return;
        }
        if (CHAR_getInt(petindex, CHAR_LV) <
            ITEM_getInt(charitem_index, ITEM_LEVEL)) {
          CHAR_talkToCli(char_index, -1, "宠物等级尚未到达装备该项物品！",
                         CHAR_COLORYELLOW);
          return;
        }
        sprintf(token, "宠物%s 装备%s", CHAR_getChar(petindex, CHAR_NAME),
                ITEM_getChar(charitem_index, ITEM_NAME));
        CHAR_talkToCli(char_index, -1, token, CHAR_COLORWHITE);
      }
      CHAR_setItemIndex(char_index, toindex, petitem_index);
      CHAR_setItemIndex(petindex, fromindex, charitem_index);
      CHAR_sendItemDataOne(char_index, toindex);
      CHAR_sendPetItemData(char_index, petid);
    }
  }
  CHAR_complianceParameter(petindex);
  snprintf(token, sizeof(token), "K%d", petid);
  CHAR_sendStatusString(char_index, token);
}

void CHAR_sendPetItemEmpty(int char_index, int petid) {
  int fd = getfdFromCharaIndex(char_index);
  char CHAR_statusSendBuffer[16];
  sprintf(CHAR_statusSendBuffer, "B%d|", petid);
  GmsvServer_S_send(fd, CHAR_statusSendBuffer);
}

#endif
#ifdef _ROOKIE_ITEM
void CHAR_CheckUserItem(int char_index) {
  int i, maxitem;
  maxitem = CheckCharMaxItem(char_index);
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    for (i = 0; i < maxitem; i++) {
      int item_index = CHAR_getItemIndex(char_index, i);

      if (item_index >= 0) {
        if (CHAR_getInt(char_index, CHAR_TRANSMIGRATION) > 0 ||
            CHAR_getInt(char_index, CHAR_LV) >= 120) {
          if (ITEM_getInt(item_index, ITEM_ID) == getRookieItem(0) ||
              ITEM_getInt(item_index, ITEM_ID) == getRookieItem(1) ||
              ITEM_getInt(item_index, ITEM_ID) == getRookieItem(2) ||
              ITEM_getInt(item_index, ITEM_ID) == getRookieItem(3) ||
              ITEM_getInt(item_index, ITEM_ID) == getRookieItem(4)) {
            char itemname[128];
            sprintf(itemname,
                    "你的道具：%s 已不适用于当前阶段，系统已自动回收。",
                    ITEM_getChar(item_index, ITEM_NAME));
            CHAR_talkToCli(char_index, -1, itemname, CHAR_COLORYELLOW);
            CHAR_setItemIndex(char_index, i, -1);
            CHAR_sendItemDataOne(char_index, i);
            ITEM_endExistItemsOne(item_index);
            continue;
          }
        }
#ifdef _ITEM_USE_TIME
        if (ITEM_getInt(item_index, ITEM_USETIME) > 0) {
          if (ITEM_getInt(item_index, ITEM_USETIME) < (int)time(NULL)) {
            char itemname[128];
            sprintf(itemname, "您的道具:%s，由于使用期限已到，系统自动收回。",
                    ITEM_getChar(item_index, ITEM_NAME));
            CHAR_talkToCli(char_index, -1, itemname, CHAR_COLORYELLOW);
            CHAR_setItemIndex(char_index, i, -1);
            CHAR_sendItemDataOne(char_index, i);
            ITEM_endExistItemsOne(item_index);
          }
        }
#endif
      }
    }
  }
}
#endif
