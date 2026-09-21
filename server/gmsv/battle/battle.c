#define __BATTLE_C__
#include "version.h"
//
#include "common.h"
#include "gmsv_server.h"
//
#include "anim_tbl.h"
#include "battle.h"
#include "battle_ai.h"
#include "battle_command.h"
#include "battle_event.h"
#include "battle_magic.h"
#include "buf.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "config_file.h"
#include "encount.h"
#include "enemy.h"
#include "handletime.h"
#include "log.h"
#include "magic.h"
#include "net.h"
#include "npc_npcenemy.h"
#include "npcutil.h"
#include "object.h"
#include "pet_skill.h"
#include "pet_skillinfo.h"
#include "readmap.h"
#ifdef _Item_ReLifeAct
#include "item.h"
#endif
#ifdef _TRADE_PK
#include "trade.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "npc_healer.h"
#ifdef _PROFESSION_SKILL
#include "profession_skill.h"
#include "skill.h"
#endif

static int Total_BattleNum = -1;
static int BATTLE_searchCnt = -1;

#ifdef _PET_LIMITLEVEL // ANDY_ADD
void Pet_Check_Die(int petindex);
#endif
static int BATTLE_SearchTask(void);
static int BATTLE_Battling(int battle_index);

#ifdef _Item_ReLifeAct
BOOL CHECK_ITEM_RELIFE(int battle_index, int toindex);
#endif
#ifdef _LOSE_FINCH_
BOOL CHECK_PET_RELIFE(int battle_index, int petindex);
#endif
#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MagicStatusSeq(int char_index);
#endif

// Terry 2001/11/28
static char szBattleString[1024];
// 使用回旋镖时的攻击顺序
int BoomerangVsTbl[4][5] = {
    {4 + 5 * 0, 2 + 5 * 0, 0 + 5 * 0, 1 + 5 * 0, 3 + 5 * 0},
    {4 + 5 * 1, 2 + 5 * 1, 0 + 5 * 1, 1 + 5 * 1, 3 + 5 * 1},
    {4 + 5 * 2, 2 + 5 * 2, 0 + 5 * 2, 1 + 5 * 2, 3 + 5 * 2},
    {4 + 5 * 3, 2 + 5 * 3, 0 + 5 * 3, 1 + 5 * 3, 3 + 5 * 3},
};

BOOL BATTLE_CHECKINDEX(int battle_index) {
  if (battle_index >= BATTLE_battlenum || battle_index < 0)
    return FALSE;

  return BattleArray[battle_index].use;
}

void BATTLE_BadStatusAllClr(int char_index) {
  int i;
  for (i = 1; i < BATTLE_ST_END; i++) {
    CHAR_setWorkInt(char_index, StatusTbl[i], 0);
  }
#ifdef _OTHER_MAGICSTAUTS
  for (i = 1; i < MAXSTATUSTYPE; i++) {
    CHAR_setWorkInt(char_index, MagicTbl[i], 0);
    CHAR_setWorkInt(char_index, CHAR_OTHERSTATUSNUMS, 0);
  }
#endif
#ifdef _IMPRECATE_ITEM
  for (i = 0; i < 3; i++) {
    CHAR_setWorkInt(char_index, CHAR_WORKIMPRECATENUM1 + i, 0);
  }
#endif

#ifdef _PETSKILL_SETDUCK
  CHAR_setWorkInt(char_index, CHAR_MYSKILLDUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLDUCKPOWER, 0);
#ifdef _MAGICPET_SKILL
  CHAR_setWorkInt(char_index, CHAR_MYSKILLSTR, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLSTRPOWER, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLTGH, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLTGHPOWER, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLDEX, 0);
  CHAR_setWorkInt(char_index, CHAR_MYSKILLDEXPOWER, 0);
  CHAR_setWorkInt(char_index, CHAR_MAGICPETMP, 0);
#endif
#endif
#ifdef _STRENGTH_PETSKILL
  CHAR_setWorkInt(char_index, CHAR_WORK_STRENGTH, 0);
#endif
#ifdef _LOSTLOST_PETSKILL
  CHAR_setWorkInt(char_index, CHAR_WORK_LOSTLOST, 0);
#endif
#ifdef _PETSKILL_NEW_PASSIVE
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_DUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_ACURATE, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_CRITICAL, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_COUNTER, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_MULTIPLE, 0);
#endif
  CHAR_setFlg(char_index, CHAR_ISDIE, 0);
#ifdef _BATTLE_PROPERTY
  {
    Char *ch = CHAR_getCharPointer(char_index);
    if (ch == NULL)
      return;
    strncpysafe(ch->charfunctable[CHAR_BATTLEPROPERTY].string,
               sizeof(ch->charfunctable[CHAR_BATTLEPROPERTY]), ""); // 战斗
    CHAR_constructFunctable(char_index);
  }
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  CHAR_setWorkInt(char_index, CHAR_MYSKILLHIT, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_P_DUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKMOD_P_DUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_WEAPON, 0); // 武器专精

  // 火冰电抗性
  for (i = 0; i < 3; i++)
    CHAR_setWorkInt(char_index, CHAR_WORK_F_RESIST + i,
                    0 /*CHAR_getInt( char_index, PROFESSION_FIRE_R+i )*/);

#endif

#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int pindex = CHAR_getCharPet(char_index, i);
      if (!CHAR_CHECKINDEX(pindex))
        continue;
      CHAR_setInt(pindex, CHAR_BECOMEPIG, -1);
    }
  }
#endif
}

// 2026.09.19 删除逻辑存在缺陷且无引用的代码：BATTLE_getTopBattle()
//
static int BATTLE_getBattleFieldNo(int floor, int x, int y) {
  int tile[2], map[3], iRet;
  if (!MAP_getTileAndObjData(floor, x, y, &tile[0], &tile[1]))
    return FALSE;
  map[0] = MAP_getImageInt(tile[0], MAP_BATTLEMAP);
  map[1] = MAP_getImageInt(tile[0], MAP_BATTLEMAP2);
  map[2] = MAP_getImageInt(tile[0], MAP_BATTLEMAP3);
  iRet = map[RAND(0, 2)];
  return iRet;
}

void BATTLE_BadStatusString(int defNo, int status) {
  char szWork[256];
  if (status < 1 || status >= BATTLE_ST_END) {
    status = 0;
  }
  sprintf(szWork, "BM|%X|%X|", defNo, status);
  strncat(szBadStatusString, szWork, sizeof(szBadStatusString));
}

#ifdef _ATTACK_MAGIC

static int CharTableIdx[20][2] = {
    {3, 2}, {3, 1}, {3, 3}, {3, 0}, {3, 4}, {2, 2}, {2, 1},
    {2, 3}, {2, 0}, {2, 4}, {0, 2}, {0, 1}, {0, 3}, {0, 0},
    {0, 4}, {1, 2}, {1, 1}, {1, 3}, {1, 0}, {1, 4}
};

static int SortLoc(const void *pEle1, const void *pEle2) {
  int ele1 = *(int*)pEle1;
  int ele2 = *(int*)pEle2;
  int ele1basex = CharTableIdx[ele1][1];
  int ele1basey = CharTableIdx[ele1][0];
  int ele2basex = CharTableIdx[ele2][1];
  int ele2basey = CharTableIdx[ele2][0];
  if (ele1 >= 10) {
    if (ele1basey != ele2basey)
      return (ele1basey - ele2basey);
    return (ele1basex - ele2basex);
  } else { // 右下方
    if (ele1basey != ele2basey)
      return (ele2basey - ele1basey);
    return (ele2basex - ele1basey);
  }
  return 0;
}
#endif

int BATTLE_MultiList(int battle_index, int toNo, int ToList[]) {
  int j, i, cnt = 0, nLife = 0, nLifeArea[10];
#ifdef _ATTACK_MAGIC
  // 单人攻击
  if (0 <= toNo && toNo <= 19) {
    memset(nLifeArea, -1, sizeof(nLifeArea));
    // 如果被攻击者是右下方的人
    if (toNo >= 0 && toNo <= 9) {
      for (i = 0; i < 10; i++) {
        // 确定活着的人数,并记录活着的人的号码
        if (BATTLE_TargetCheck(battle_index, i) == TRUE)
          nLifeArea[nLife++] = i;
      }
    }
    // 如果被攻击者是左上方的人
    if (toNo >= 10 && toNo <= 19) {
      for (i = 10; i < 20; i++) {
        // 确定活着的人数,并记录活着的人的号码
        if (BATTLE_TargetCheck(battle_index, i) == TRUE)
          nLifeArea[nLife++] = i;
      }
    }
    // 全死(虽然不太可能,但..)
    if (nLife == 0) {
      print("\nAll die!!");
      return -1;
    } else {
      // 被攻击的对象已经死亡或不在战场上
      if (BATTLE_TargetCheck(battle_index, toNo) == FALSE)
        // 随机找一只来打
        while ((toNo = nLifeArea[rand() % 10]) == -1);
    }
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }
  // 右下後一列攻击
  else if (TARGET_SIDE_0_B_ROW == toNo) {
    for (j = 0, i = 0; i < SIDE_OFFSET / 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换前一排
      for (j = 0, i = SIDE_OFFSET / 2; i < SIDE_OFFSET; i++) {
        if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果全死
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_0_F_ROW;
    }
  }
  // 右下前一列攻击
  else if (TARGET_SIDE_0_F_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET / 2; i < SIDE_OFFSET; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换後一排
      for (j = 0, i = 0; i < SIDE_OFFSET / 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果这一排都没有人可以打
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_0_B_ROW;
    }
  }
  // 换後一排
  else if (TARGET_SIDE_1_B_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET + SIDE_OFFSET / 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果全死
    if (j == 0) {
      // 左下後一列攻击
      for (j = 0, i = SIDE_OFFSET + SIDE_OFFSET / 2; i < SIDE_OFFSET * 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果这一排都没有人可以打
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_1_F_ROW;
    }
  }
  // 换前一排
  else if (TARGET_SIDE_1_F_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET + SIDE_OFFSET / 2; i < SIDE_OFFSET * 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    if (j == 0) {
      for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET + SIDE_OFFSET / 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_1_B_ROW;
    }
  } else if (TARGET_SIDE_0 == toNo) {
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }

    ToList[j] = -1;
    cnt = j;
  }
  // 左上方所有攻击
  else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }

    ToList[j] = -1;
    cnt = j;
  }
  // 所有人攻击
  else if (toNo == TARGET_ALL) {
#ifdef _FIX_ARRAYBUG
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++) {
#else
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
#endif
      if (TRUE == BATTLE_TargetCheck(battle_index, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[i] = -1;
    cnt = j;
    // won add	前後排攻击
  } else if (toNo == TARGER_THROUGH) {
    int toNo2 = -1, count = 0;

    if (BATTLE_TargetCheck(battle_index, toNo) != FALSE) {
      ToList[count] = toNo;
      count++;
      cnt = count;
    }

    if (toNo < (SIDE_OFFSET / 2))
      toNo2 = toNo + (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET))
      toNo2 = toNo - (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET * 1.5))
      toNo2 = toNo + (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET * 2))
      toNo2 = toNo - (SIDE_OFFSET / 2);

    if (BATTLE_TargetCheck(battle_index, toNo2) != FALSE) {
      ToList[count] = toNo2;
      count++;
      cnt = count;
    }
  } else {
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }

  if (cnt > 1)
    qsort(ToList, cnt, sizeof(ToList[0]), SortLoc);
  return toNo;

#else
  if (0 <= toNo && toNo <= 19) {
    if (BATTLE_TargetCheck(battle_index, toNo) == TRUE) {
      ToList[0] = toNo;
      ToList[1] = -1;
      cnt = 1;
    } else {
      ToList[0] = -1;
      ToList[1] = -1;
      cnt = 0;
    }
  } else if (toNo == TARGET_SIDE_0) {
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (BATTLE_TargetCheck(battle_index, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
  } else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (BATTLE_TargetCheck(battle_index, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
  } else if (toNo == TARGET_ALL) {
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
      if (BATTLE_TargetCheck(battle_index, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[i] = -1;
    cnt = j;
  } else {
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }

  return 1;
#endif
}

void BATTLE_MultiListDead(int battle_index, int toNo, int ToList[]) {
  int j, i;
  if (0 <= toNo && toNo <= 19) {
    if (BATTLE_TargetCheckDead(battle_index, toNo) == TRUE) {
      ToList[0] = toNo;
      ToList[1] = -1;
    } else {
      ToList[0] = -1;
      ToList[1] = -1;
    }
  } else if (toNo == TARGET_SIDE_0) {
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (BATTLE_TargetCheckDead(battle_index, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
  } else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (BATTLE_TargetCheckDead(battle_index, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
  } else {
    if (toNo == TARGET_ALL) {
#ifdef _FIX_ARRAYBUG
      for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++) {
#else
      for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
#endif
        if (BATTLE_TargetCheckDead(battle_index, i) == TRUE) {
          ToList[j] = i;
          j++;
        }
      }
#ifdef _FIX_ITEMRELIFE // WON ADD 修正替身娃娃问题
      ToList[j] = -1;
#else
      ToList[i] = -1;
#endif
    } else {
      ToList[0] = toNo;
      ToList[1] = -1;
    }
  }
}

BOOL BATTLE_IsThrowWepon(int item_index) {
  int itemtype;
  if (ITEM_CHECKINDEX(item_index) == FALSE) {
    return FALSE;
  }
  itemtype = ITEM_getInt(item_index, ITEM_TYPE);
  if (itemtype == ITEM_BOOMERANG || itemtype == ITEM_BOW ||
      itemtype == ITEM_BREAKTHROW || itemtype == ITEM_BOUNDTHROW) {
    return TRUE;
  } else {
    return FALSE;
  }
}

int BATTLE_ClearGetExp(int char_index) {
  int i, pindex;
  if (CHAR_CHECKINDEX(char_index) == FALSE) {
    return BATTLE_ERR_CHARAINDEX;
  }
  CHAR_setWorkInt(char_index, CHAR_WORKGETEXP, 0);
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    pindex = CHAR_getCharPet(char_index, i);
    if (CHAR_CHECKINDEX(pindex) == FALSE)
      continue;
    CHAR_setWorkInt(pindex, CHAR_WORKGETEXP, 0);
  }
  return 0;
}

INLINE void _BATTLE_ExitAll(char *file, int line, int battle_index) {
  int j, i, char_index;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = BattleArray[battle_index].Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
      BATTLE_Exit(char_index, battle_index);
    }
  }
}

void BATTLE_AllCharaFinishSet(int battle_index) {
  int j, i, char_index;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = BattleArray[battle_index].Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
    }
  }
}

BOOL BATTLE_IsCharge(int char_index) {
  int com;
  com = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);
  if (com == BATTLE_COM_S_CHARGE || com == BATTLE_COM_S_EARTHROUND1 ||
      com == BATTLE_COM_S_EARTHROUND0
#ifdef _PROFESSION_ADDSKILL
      || CHAR_getWorkInt(char_index, CHAR_DOOMTIME) > 0
#endif
  )
    return TRUE;
  else
    return FALSE;
}

void BATTLE_AllCharaCWaitSet(int battle_index) {
  int j, i, char_index;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = BattleArray[battle_index].Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (BATTLE_IsCharge(char_index) == FALSE) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_WAIT);
    }
  }
}

void BATTLE_AllCharaWatchWaitSet(int battle_index) {
  int i, char_index;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = BattleArray[battle_index].Side[0].Entry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
        BATTLE_CHARMODE_WATCHINIT) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_WAIT);
    }
  }
}

void BATTLE_SurpriseSet(int battle_index, int side) {
  int i, char_index;
  BATTLE_ENTRY *pEntry = BattleArray[battle_index].Side[side].Entry;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    }
  }
}

BOOL BATTLE_initBattleArray(int battlenum) {

  int i;
  BATTLE_battlenum = battlenum;
  BattleArray = allocateMemory(sizeof(BATTLE) * BATTLE_battlenum);
  if (BattleArray == NULL)
    return FALSE;
  for (i = 0; i < BATTLE_battlenum; i++) {
    memset(&BattleArray[i], 0, sizeof(BATTLE));
  }
  print("分配 %4.2f MB 空间......",
        ((float)sizeof(Char) * BATTLE_battlenum) / 1024.0 / 1024.0);
  return TRUE;
}

static void EntryInit(BATTLE_ENTRY *pEntry) {
  int i;
  pEntry->char_index = -1;
  pEntry->bid = -1;
  pEntry->escape = 0;
  for (i = 0; i < GETITEM_MAX; i++) {
    pEntry->getitem[i] = -1;
  }
}

int BATTLE_CreateBattle(void) {
  int i, j;
  int battle_index = BATTLE_SearchTask();
  // NUKE 0701
  if (battle_index < 0)
    return -1;
  memset(&BattleArray[battle_index], 0, sizeof(BATTLE));
  BATTLE *pBattle = &BattleArray[battle_index];
  // NUKE 0701
  if (pBattle == NULL)
    return -1;
  pBattle->use = TRUE;
  pBattle->mode = BATTLE_MODE_INIT;
  pBattle->turn = 0;
  pBattle->dpbattle = 0;
  pBattle->norisk = 0;
  pBattle->flg = 0;
  pBattle->field_att = BATTLE_ATTR_NONE;
  pBattle->att_count = 0;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      EntryInit(&pBattle->Side[j].Entry[i]);
    }
  }
  for (i = 0; i < BATTLE_ENTRY_MAX * 2; i++) {
    pBattle->iEntryBack[i] = -1;
    pBattle->iEntryBack2[i] = -1;
  }
  pBattle->WinFunc = NULL;
  pBattle->pNext = NULL;
  pBattle->pBefore = NULL;
  pBattle->battle_index = battle_index;
  Total_BattleNum++;
#ifdef _BATTLECOMMAND_TIME
  pBattle->PartTime = 0;
#endif
#ifdef _PROFESSION_ADDSKILL
  // 冰爆术存放
/*	pBattle->ice_count = 0;
        for( i = 0; i < 20; i ++ ){
                pBattle->ice_bout[i]=0;
                pBattle->ice_use[i]=FALSE;
                pBattle->ice_toNo[i]=0;
                pBattle->ice_level[i]=0;
                pBattle->ice_array[i]=0;
                pBattle->ice_attackNo[i]=0;
                pBattle->ice_char_index[i]=-1;
        }*/
#endif
#ifdef _BATTLE_BOUT_TIME
  pBattle->tv_sec = NowTime.tv_sec;
  pBattle->tv_usec = NowTime.tv_usec;
#endif
  return battle_index;
}

static int BATTLE_SearchTask(void) {
  int j, i = BATTLE_searchCnt;
  for (j = 0; j < BATTLE_battlenum; j++) {
    if (i >= BATTLE_battlenum)
      i = 0;
    if (BattleArray[i].use == FALSE) {
      BATTLE_searchCnt = i + 1;
      return i;
    }
    i++;
  }
  return -1;
}

void BATTLE_DeleteItem(int battle_index) {
  int i, j, k;
  BATTLE_ENTRY *pEntry;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry = &BattleArray[battle_index].Side[j].Entry[i];
      for (k = 0; k < GETITEM_MAX; k++) {
        if (pEntry->getitem[k] >= 0) {
          ITEM_endExistItemsOne(pEntry->getitem[k]);
        }
      }
    }
  }
}

int BATTLE_DeleteBattle(int battle_index) {
  int i, j;
  BATTLE *pBattle;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    printEx("err:battle index error\n");
    return BATTLE_ERR_BATTLEINDEX;
  }
  if (BATTLE_WatchUnLink(battle_index) == FALSE) {
    printEx("err:battle link 不脱离\n");
  }
  pBattle = &BattleArray[battle_index];
  pBattle->use = FALSE;
  pBattle->mode = BATTLE_MODE_NONE;
  BATTLE_DeleteItem(battle_index);
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      EntryInit(&pBattle->Side[j].Entry[i]);
    }
  }
  Total_BattleNum--;
  return 0;
}

int BATTLE_No2Index(int battle_index, int bid) {
  BATTLE_ENTRY *pEntry;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -1;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return -1;

  if (bid >= 10) {
    pEntry = BattleArray[battle_index].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battle_index].Side[0].Entry;
  }
  if (CHAR_CHECKINDEX(pEntry[bid].char_index) == FALSE)
    return -1;
  return pEntry[bid].char_index;
}
#ifdef _Item_ReLifeAct
int BATTLE_getBattleDieIndex(int battle_index, int bid) {
  BATTLE_ENTRY *pEntry;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -1;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return -1;

  if (bid >= 10) {
    pEntry = BattleArray[battle_index].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battle_index].Side[0].Entry;
  }
  if (CHAR_CHECKINDEX(pEntry[bid].char_index) == FALSE)
    return -1;
  if (pEntry[bid].flg & BENT_FLG_ULTIMATE)
    return -1;

  return pEntry[bid].char_index;
}
#endif

#ifdef _PROFESSION_ADDSKILL
BOOL BATTLE_BattleUltimate(int battle_index,
                           int bid) // 检查此位置上是否被打飞
{
  BATTLE_ENTRY *pEntry;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return FALSE;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return FALSE;
  if (bid >= 10) {
    pEntry = BattleArray[battle_index].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battle_index].Side[0].Entry;
  }
  if (pEntry[bid].flg & BENT_FLG_ULTIMATE)
    return FALSE;
  else
    return TRUE;
}
#endif

int BATTLE_Index2No(int battle_index, int char_index) {
  BATTLE_ENTRY *pEntry;
  int i, j;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -1;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return -1;

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (pEntry[i].char_index == char_index) {
        return i + j * SIDE_OFFSET;
      }
    }
  }

  return -1;
}

char *BATTLE_CharTitle(int char_index) {
  char *pName;
  static char Len0Buff[2] = "";

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    pName = Len0Buff;
  } else {
    pName = CHAR_getChar(char_index, CHAR_OWNTITLE);
    if (pName[0] == 0) {
      pName = Len0Buff;
    }
  }
  return pName;
}

int BATTLE_CharaBackUp(int battle_index) {
  int i, j, k, char_index;
  BATTLE *pBattle;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return BATTLE_ERR_BATTLEINDEX;
  }

  pBattle = &BattleArray[battle_index];
  k = 0;

  for (j = 0; j < 2; j++) {
    k = j * SIDE_OFFSET;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      char_index = pBattle->Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == TRUE &&
          CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        pBattle->iEntryBack[k] = pBattle->Side[j].Entry[i].char_index;
        pBattle->iEntryBack2[k] = pBattle->Side[j].Entry[i].char_index;
      } else {
        pBattle->iEntryBack[k] = -1;
        pBattle->iEntryBack2[k] = -1;
      }
      k++;
    }
  }

  return 0;
}

int BATTLE_NewEntry(int char_index, int battle_index, int side) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int i, iEntryMax, iEntryFirst;

  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  pBattle = &BattleArray[battle_index];

  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  if (side < 0 || side > 1) {
    return BATTLE_ERR_PARAM;
  }

  switch (CHAR_getInt(char_index, CHAR_WHICHTYPE)) {
    int work;

  case CHAR_TYPEPLAYER:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_PLAYER_MAX;
    break;
  case CHAR_TYPEENEMY:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_ENEMY) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_ENTRY_MAX;
    break;
  case CHAR_TYPEPET:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }

    work = CHAR_getWorkInt(char_index, CHAR_WORKPLAYERINDEX);
    pEntry = pBattle->Side[side].Entry;

    work = BATTLE_Index2No(battle_index, work);
    work -= side * SIDE_OFFSET;
    work += 5;
    iEntryFirst = work;
    iEntryMax = work + 1;
    break;
#ifdef _PLAYER_NPC
  case CHAR_TYPEPLAYERNPC:
    iEntryFirst = 0;
    iEntryMax = BATTLE_ENTRY_MAX;
    break;

  case CHAR_TYPEPLAYERPETNPC:
    work = CHAR_getWorkInt(char_index, CHAR_WORKPLAYERINDEX);
    pEntry = pBattle->Side[side].Entry;

    work = BATTLE_Index2No(battle_index, work);
    work -= side * SIDE_OFFSET;
    work += 5;
    iEntryFirst = work;
    iEntryMax = work + 1;
    break;
#endif
  default:
    print("不明(%s)(%d)\n", CHAR_getUseName(char_index),
          CHAR_getInt(char_index, CHAR_WHICHTYPE));
    return BATTLE_ERR_TYPE;
  }

  pEntry = pBattle->Side[side].Entry;

  for (i = iEntryFirst; i < iEntryMax; i++) {
    if (pEntry[i].char_index == -1)
      break;
  }

  if (i == iEntryMax) {
    return BATTLE_ERR_ENTRYMAX;
  }

  EntryInit(&pEntry[i]);

  pEntry[i].char_index = char_index;

  pEntry[i].bid = i + side * SIDE_OFFSET;

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, battle_index);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_INIT);

  CHAR_setWorkInt(char_index, CHAR_WORKDBATTLEESCAPE, 0);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLESIDE, side);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, 0);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, -1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, -1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM3, -1);

  CHAR_setWorkInt(char_index, CHAR_WORKMODATTACK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKMODDEFENCE, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKMODQUICK, 0);

  CHAR_setWorkInt(char_index, CHAR_WORKDAMAGEABSROB, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKDAMAGEREFLEC, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKDAMAGEVANISH, 0);

  CHAR_setWorkInt(char_index, CHAR_WORKMODCAPTURE, 0);

  CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);

  BATTLE_BadStatusAllClr(char_index);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEWATCH, 0);

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  CHAR_setWorkInt(char_index, CHAR_WORKTRAP, 0); //陷阱
  BATTLE_ProfessionStatus_init(battle_index, char_index);
#endif

#ifdef _PETSKILL_ACUPUNCTURE
  CHAR_setWorkInt(char_index, CHAR_WORKACUPUNCTURE, 0); //针刺外皮
#endif

#ifdef _PETSKILL_RETRACE
  CHAR_setWorkInt(char_index, CHAR_WORKRETRACE, 0); //追迹攻击
#endif

#ifdef _PETSKILL_BECOMEFOX // 将媚惑术中招时间清除为-1
  CHAR_setWorkInt(char_index, CHAR_WORKFOXROUND, -1);
#endif
#ifdef _PROFESSION_ADDSKILL
  // 结界清除
  CHAR_setWorkInt(char_index, CHAR_WORKFIXEARTHAT_BOUNDARY, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXWATERAT_BOUNDARY, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXFIREAT_BOUNDARY, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXWINDAT_BOUNDARY, 0);
  CHAR_setWorkInt(char_index, CHAR_DOOMTIME, 0); //世界末日集气
  CHAR_setWorkInt(char_index, CHAR_WORK_com1, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_toNo, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_mode, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_skill_level, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_array, 0);
#endif
  return 0;
}

INLINE int _BATTLE_Exit(char *file, int line, int char_index, int battle_index) {
  BATTLE_ENTRY *pEntry;
  int i, j, k;
  BATTLE *pBattle;
  char szPet[32];
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return BATTLE_ERR_BATTLEINDEX;
  }

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _BATTLE_OVER_STAR_TIME
    int floorid = CHAR_getInt(char_index, CHAR_FLOOR);
    if (floorid == 1042 || floorid == 2032 || floorid == 3032 ||
        floorid == 4032) {
      CHAR_setWorkInt(char_index, CHAR_WORKDBATTLETIME, time(NULL));
    }
#endif
  }
  if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E) {
    BATTLE_ENTRY *pWinEntry;
    char token[256];
    pWinEntry = BattleArray[battle_index].Side[0].Entry;

    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      int winindex = pWinEntry[i].char_index;
      // if (CHAR_CHECKINDEX(winindex) == TRUE) {
      //   if (CHAR_getInt(winindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      //     BattleFinishPvEFunction(battle_index, char_index);
      //   }
      // }
    }
  }
#ifdef _ROOKIE_ITEM
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_CheckUserItem(char_index);
  }
#endif

#ifdef _PROFESSION_ADDSKILL
  CHAR_setWorkInt(char_index, CHAR_DOOMTIME, 0); //世界末日集气
#endif
#ifdef _PETSKILL_BECOMEFOX // 离开战斗时将媚惑术造成的变身效果清除
  // print("\n检查图号:%s,%d", CHAR_getChar( char_index, CHAR_NAME), CHAR_getInt(
  // char_index, CHAR_BASEIMAGENUMBER));

  if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101749 ||
      CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1) { //变成小狐狸
    CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER));
    CHAR_setWorkInt(char_index, CHAR_WORKFOXROUND, -1);
    // print("\n变回去:%d",CHAR_getInt( char_index, CHAR_BASEIMAGENUMBER));
  }
#endif

#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1 &&
      CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                CHAR_getInt(char_index, CHAR_BECOMEPIG_BBI));
    CHAR_complianceParameter(char_index);
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
    CHAR_send_P_StatusString(char_index, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  }
#endif

#ifdef _MAGIC_NOCAST // 精灵:沉默
  // WON ADD
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    GmsvServer_NC_send(getfdFromchar_index(char_index), 0); //复原
    CHAR_setWorkInt(char_index, CHAR_WORKNOCAST, 0);
  }
#endif
  pBattle = &BattleArray[battle_index];
  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  for (j = 0; j < 2; j++) {
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (pEntry[i].char_index != char_index)
        continue;
      pEntry[i].char_index = -1;
      pEntry[i].escape = 0;
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, -1);
#ifdef _LOSE_FINCH_
      CHAR_setWorkInt(char_index, CHAR_WORKSPETRELIFE, 0);
#endif
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
        CHAR_endCharOneArray(char_index);
#ifdef _PLAYER_NPC
      } else if (CHAR_getInt(char_index, CHAR_WHICHTYPE) ==
                     CHAR_TYPEPLAYERNPC ||
                 CHAR_getInt(char_index, CHAR_WHICHTYPE) ==
                     CHAR_TYPEPLAYERPETNPC) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, -1);
        {
          CHAR_setInt(char_index, CHAR_HP,
                      CHAR_getWorkInt(char_index, CHAR_WORKMAXHP));
          CHAR_setInt(char_index, CHAR_MP,
                      CHAR_getWorkInt(char_index, CHAR_WORKMAXMP));
          for (i = 0; i < CHAR_MAXPETHAVE; i++) {
            int petindex = CHAR_getCharPet(char_index, i);
            if (CHAR_CHECKINDEX(petindex) == FALSE)
              continue;
            CHAR_setFlg(petindex, CHAR_ISDIE, 0);
            CHAR_setInt(petindex, CHAR_HP,
                        CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
            CHAR_setInt(petindex, CHAR_MP,
                        CHAR_getWorkInt(petindex, CHAR_WORKMAXMP));
          }
        }
        if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
          CHAR_setFlg(char_index, CHAR_ISDIE, FALSE);
        }
        BATTLE_BadStatusAllClr(char_index);
        CHAR_complianceParameter(char_index);
        CHAR_sendBattleEffect(char_index, OFF);
#endif
      } else if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _OFFLINE_SYSTEM
        if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) != 0) {
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE,
                          BATTLE_CHARMODE_NONE);
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, -1);
          CHAR_sendBattleEffect(char_index, OFF);
        }
#endif

#ifdef _PET_ITEM
        if (CHAR_CHECKINDEX(CHAR_getCharPet(char_index, 0))) {
          CHAR_sendPetItemData(char_index, 0);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(char_index, 1))) {
          CHAR_sendPetItemData(char_index, 1);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(char_index, 2))) {
          CHAR_sendPetItemData(char_index, 2);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(char_index, 3))) {
          CHAR_sendPetItemData(char_index, 3);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(char_index, 4))) {
          CHAR_sendPetItemData(char_index, 4);
        }
#endif

        int petindex = pEntry[i + 5].char_index;
        if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {

#ifdef _FIX_SPEED_UPLEVEL // WON ADD 修正加速
                          // Robin cancel
                          // if( CHAR_getFlg( char_index, CHAR_ISDIE ) == TRUE
                          // ){ }else
          {
#ifdef _OFFLINE_SYSTEM
            if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
            {
              unsigned int Dtimes;
              unsigned int battletime;
              int fd = getfdFromchar_index(char_index);
              if (CONNECT_checkfd(fd) == TRUE) {
                Dtimes = BattleArray[battle_index].CreateTime;
                battletime =
                    (unsigned int)(BattleArray[battle_index].flgTime / 100);
                CheckDefBTime(char_index, fd, Dtimes, battletime,
                              10); // 2026.09.20 人物死亡后增加10s延时
              }
            }
          }
#endif
          CHAR_setFlg(char_index, CHAR_ISDIE, 0);
          CHAR_setInt(char_index, CHAR_HP, 1);
          // CoolFish: Faily PK 2001/8/3
          CHAR_setWorkInt(char_index, CHAR_WORKFMPKFLAG, -1);

        } else {
#ifdef _BATTLE_TIMESPEED
          // Robin cancel
#ifdef _OFFLINE_SYSTEM
          if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
          {
            int fd = getfdFromchar_index(char_index);
            if (CONNECT_checkfd(fd) == TRUE) {
              unsigned int create_time = BattleArray[battle_index].CreateTime;
              unsigned int duration_time =
                  (unsigned int)(BattleArray[battle_index].flgTime / 100);
              CheckDefBTime(char_index, fd, create_time, duration_time, 0);
            }
          }
#endif
        }
        if (CHAR_CHECKINDEX(petindex) == TRUE) {
          pEntry[i + 5].char_index = -1;
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEINDEX, -1);
        }
        BATTLE_BadStatusAllClr(char_index);
        CHAR_complianceParameter(char_index);
        CHAR_Skillupsend(char_index);
        // Robin 0730
        if (CHAR_getWorkInt(char_index, CHAR_WORKPETFALL)) {
          CHAR_setWorkInt(char_index, CHAR_WORKPETFALL, 0);
          CHAR_setInt(char_index, CHAR_RIDEPET, -2);
        }
        CHAR_send_P_StatusString(
            char_index, CHAR_P_STRING_HP | CHAR_P_STRING_EXP |
                        CHAR_P_STRING_MP | CHAR_P_STRING_DUELPOINT |
                        CHAR_P_STRING_CHARM | CHAR_P_STRING_EARTH |
                        CHAR_P_STRING_WATER | CHAR_P_STRING_FIRE |
                        CHAR_P_STRING_WIND | CHAR_P_STRING_RIDEPET);
        // Robin 0730
        if (CHAR_getInt(char_index, CHAR_RIDEPET) == -2) {
          CHAR_setInt(char_index, CHAR_RIDEPET, -1);
        }
        for (k = 0; k < CHAR_MAXPETHAVE; k++) {
          petindex = CHAR_getCharPet(char_index, k);
          if (CHAR_CHECKINDEX(petindex) == FALSE)
            continue;
          if (CHAR_getInt(petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE)
            continue;
          if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE ||
              CHAR_getInt(petindex, CHAR_HP) <= 0) {
            CHAR_setFlg(petindex, CHAR_ISDIE, 0);
            CHAR_setInt(petindex, CHAR_HP, 1);
          }
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
#ifdef _LOSE_FINCH_
          CHAR_setWorkInt(char_index, CHAR_WORKSPETRELIFE, 0);
#endif
#ifdef _VARY_WOLF
          if (CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER) !=
              CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER)) // Robin fix
          {
            CHAR_setInt(petindex, CHAR_BASEIMAGENUMBER,
                        CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER));
            sprintf(szPet, "K%d", k);
            CHAR_sendStatusString(char_index, szPet);
          }
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
          // 遗忘
          if (CHAR_getWorkInt(petindex, CHAR_WORKOBLIVION) > 0) // Robin fix
          {
            int pet_no = CHAR_getInt(char_index, CHAR_DEFAULTPET);
            CHAR_setWorkInt(petindex, CHAR_WORKOBLIVION, 0);
            // 宠物编号
            sprintf(szPet, "W%d", pet_no);
            CHAR_sendStatusString(char_index, szPet);
          }
#endif
          BATTLE_BadStatusAllClr(petindex);
          CHAR_complianceParameter(petindex);
          CHAR_send_K_StatusString(
              char_index, k,
              CHAR_K_STRING_HP | CHAR_K_STRING_EXP | CHAR_K_STRING_AI |
                  CHAR_K_STRING_MP | CHAR_K_STRING_ATK | CHAR_K_STRING_DEF |
                  CHAR_K_STRING_QUICK | CHAR_K_STRING_EARTH |
                  CHAR_K_STRING_WATER | CHAR_K_STRING_FIRE |
                  CHAR_K_STRING_WIND);
        }
        if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
          int flg = 0;
          CHAR_setFlg(char_index, CHAR_ISDUEL, 0);
          if (CHAR_getFlg(char_index, CHAR_ISPARTY))
            flg |= CHAR_FS_PARTY;
          if (CHAR_getFlg(char_index, CHAR_ISDUEL))
            flg |= CHAR_FS_DUEL;
          if (CHAR_getFlg(char_index, CHAR_ISPARTYCHAT))
            flg |= CHAR_FS_PARTYCHAT;
          if (CHAR_getFlg(char_index, CHAR_ISTRADECARD))
            flg |= CHAR_FS_TRADECARD;
#ifdef _CHANNEL_MODIFY
          if (CHAR_getFlg(char_index, CHAR_ISTELL))
            flg |= CHAR_FS_TELL;
          if (CHAR_getFlg(char_index, CHAR_ISFM))
            flg |= CHAR_FS_FM;
#ifdef _VERSION_NEW
          if (CHAR_getFlg(char_index, CHAR_ISOCC))
            flg |= CHAR_FS_OCC;
#endif
          if (CHAR_getFlg(char_index, CHAR_ISCHAT))
            flg |= CHAR_FS_CHAT;
          if (CHAR_getFlg(char_index, CHAR_ISSAVE))
            flg |= CHAR_FS_SAVE;
#ifdef _CHATROOMPROTOCOL
#ifdef _THE_WORLD_SEND
          if (CHAR_getFlg(char_index, CHAR_ISWORLD))
            flg |= CHAR_FS_WORLD;
#endif
#ifdef _ONLINE_TALK_IP
          if (CHAR_getFlg(char_index, CHAR_ISTALKIP))
            flg |= CHAR_FS_TALKIP;
#endif
#endif
#endif
#ifdef _CHANNEL_MODIFY
          if (CHAR_getFlg(char_index, CHAR_AI_MOD))
            flg |= CHAR_FS_AI;
#endif

          GmsvServer_FS_send(getfdFromchar_index(char_index), flg);
        }

        GmsvServer_XYD_send(
            getfdFromchar_index(char_index), CHAR_getInt(char_index, CHAR_X),
            CHAR_getInt(char_index, CHAR_Y), CHAR_getInt(char_index, CHAR_DIR));
      }
      break;
    }
  }
  CHAR_PartyUpdate(char_index, CHAR_N_STRING_HP);

#ifdef _TIME_TICKET
  if (CHAR_CHECKINDEX(char_index) == TRUE) {
    int nowtime = time(NULL);
    int tickettime = CHAR_getWorkInt(char_index, CHAR_WORKTICKETTIME);
    int totaltime;
    char msg[1024];
    if (tickettime > 0 && tickettime < nowtime) {
      // int floor, x, y;
      CHAR_talkToCli(char_index, -1, "谢谢光顾。", CHAR_COLORYELLOW);
      if (CHAR_getWorkInt(char_index, CHAR_WORKTICKETTIMESTART) > 0) {
        totaltime =
            nowtime - CHAR_getWorkInt(char_index, CHAR_WORKTICKETTIMESTART);
        sprintf(msg, "总入场时间%d分%d秒。", totaltime / 60, totaltime % 60);
        CHAR_talkToCli(char_index, -1, msg, CHAR_COLORYELLOW);
      }
      CHAR_setWorkInt(char_index, CHAR_WORKTICKETTIME, 0);
      CHAR_setWorkInt(char_index, CHAR_WORKTICKETTIMESTART, 0);
      /*if( CHAR_getElderPosition( CHAR_getInt( char_index, CHAR_LASTTALKELDER),
      &floor, &x, &y ) != TRUE ) { CHAR_warpToSpecificPoint( char_index, 1006,
      15, 15);
      }
      else {
              CHAR_warpToSpecificPoint( char_index, floor, x, y);
      }*/
      CHAR_warpToSpecificPoint(char_index, 7001, 41, 6);
    }
  }
#endif
  if (CHAR_CHECKINDEX(char_index) == TRUE) {
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      if (BattleArray[battle_index].type != BATTLE_TYPE_WATCH) {
#ifdef _MAP_HEALERALLHEAL
        int floor = CHAR_getInt(char_index, CHAR_FLOOR);
        int i;
        for (i = 0; i < 32; i++) {
          if (floor == getMapHeal(i)) {
            break;
          }
        }
        if (i < 32
#ifdef _TRADE_PK
            || floor == 50000
#endif
#ifdef _AUTO_PK
            || floor == 20000
#endif
#ifdef _VIP_POINT_PK
            || floor == 11111 || floor == 22222 || floor == 33333 ||
            floor == 44444
#endif
#ifdef _VIP_ONLINE
            || CHAR_getInt(char_index, CHAR_VIPRIDE) > 0
#endif
#ifdef _FMPOINT_WELFARE
            || (CHAR_getWorkInt(char_index, CHAR_WORKFMFLOOR) == 1041 &&
                CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_NONE &&
                CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) ||
            (CHAR_getWorkInt(char_index, CHAR_WORKFMFLOOR) == 2031 &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_NONE &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) ||
            (CHAR_getWorkInt(char_index, CHAR_WORKFMFLOOR) == 3031 &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_NONE &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) ||
            (CHAR_getWorkInt(char_index, CHAR_WORKFMFLOOR) == 4031 &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_NONE &&
             CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY)
#endif
        ) {
          char petsend[3];
          CHAR_setInt(char_index, CHAR_HP,
                      CHAR_getWorkInt(char_index, CHAR_WORKMAXHP));
          CHAR_setInt(char_index, CHAR_MP,
                      CHAR_getWorkInt(char_index, CHAR_WORKMAXMP));
          CHAR_setFlg(char_index, CHAR_ISDIE, 0);
          for (i = 0; i < CHAR_MAXPETHAVE; i++) {
            int petindex = CHAR_getCharPet(char_index, i);

            if (petindex == -1)
              continue;

            /* キャラのチェック (角色的检查) */
            if (!CHAR_CHECKINDEX(char_index))
              continue;

            /* ペットのindexチェックをする (检查宠物的index) */
            if (CHAR_CHECKINDEX(petindex) == FALSE)
              continue;

            CHAR_setInt(petindex, CHAR_HP,
                        CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
            CHAR_setInt(petindex, CHAR_MP,
                        CHAR_getWorkInt(petindex, CHAR_WORKMAXMP));

            /*--パラメータ調整--*/
            CHAR_complianceParameter(char_index);
            sprintf(petsend, "K%d", i);
            CHAR_sendStatusString(char_index, petsend);
            CHAR_send_P_StatusString(char_index, CHAR_P_STRING_HP);
          }
        }
#endif
      }
    }
  }
  return BATTLE_ERR_NONE;
}

int BATTLE_PetDefaultExit(int char_index, int battle_index) {
  int pno, pindex, iRet;

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return 0;
  pno = CHAR_getInt(char_index, CHAR_DEFAULTPET);
  if (pno < 0)
    return 0;
  pindex = CHAR_getCharPet(char_index, pno);

  iRet = BATTLE_Exit(pindex, battle_index);

  if (iRet) {
    iRet *= -1;
  } else {
    return 1;
  }

  return iRet;
}

int BATTLE_PetDefaultEntry(int char_index, int battle_index, int side) {
  int pindex;

#if 1
  int pno;

  int ret = 0;

  pno = CHAR_getInt(char_index, CHAR_DEFAULTPET);
  if (pno == -1)
    return 0;
  pindex = CHAR_getCharPet(char_index, pno);
  if (CHAR_CHECKINDEX(pindex) && !CHAR_getFlg(pindex, CHAR_ISDIE) &&
      CHAR_getInt(pindex, CHAR_HP) > 0) {
    if (BATTLE_NewEntry(pindex, battle_index, side)) {
    } else {
      ret = 0;
    }
  } else {
#ifdef _PLAYER_NPC
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC)
#endif
      CHAR_setInt(char_index, CHAR_DEFAULTPET, -1);
  }

  return ret;
#else

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    pindex = CHAR_getCharPet(char_index, i);

    if (CHAR_CHECKINDEX(pindex) == FALSE)
      continue;

    if (CHAR_getFlg(pindex, CHAR_ISDIE) == TRUE)
      continue;
    if (CHAR_getInt(pindex, CHAR_HP) <= 0)
      continue;

    if (BATTLE_NewEntry(pindex, battle_index, side)) {
    } else {
      CHAR_setInt(char_index, CHAR_DEFAULTPET, i);
      break;
    }
  }

  if (i == CHAR_MAXPETHAVE) {
    CHAR_setInt(char_index, CHAR_DEFAULTPET, -1);
  }
  return 0;
#endif
}

int BATTLE_RescueEntry(int char_index, int toindex) {
  int iRet = 0, battle_index, fd, pindex;
  char szBuffer[256] = "";

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromchar_index(char_index);
  if (CONNECT_checkfd(fd) == TRUE) {
    if (CONNECT_getBDTime(fd) > time(NULL)) {
      return 1;
    }
  }
#endif

  battle_index = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX);
  if (!BATTLE_CHECKINDEX(battle_index))
    return 1;
  if ((fd = getfdFromchar_index(char_index)) < 0)
    return 1;

  if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX) == -1) {
    iRet = 1;
  }
#ifndef _FIX_P_VS_P_PARENT
  if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
    iRet = 1;
  } else
#endif
  {
    iRet = BATTLE_NewEntry(char_index, battle_index,
                           CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE));
  }

  if (iRet == 0) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    iRet = BATTLE_PetDefaultEntry(
        char_index, battle_index, CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE));
  }

  if (iRet == 0) {
    int flg;
    if (CHAR_getInt(char_index, CHAR_DEFAULTPET) >= 0) {
      pindex =
          CHAR_getCharPet(char_index, CHAR_getInt(char_index, CHAR_DEFAULTPET));
      CHAR_setWorkInt(pindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    }
    // snprintf( szBuffer, sizeof( szBuffer ), "(%s)加入作战。",
    //	CHAR_getUseName( char_index ) );
    // BATTLE_BroadCast( battle_index, szBuffer, CHAR_COLORYELLOW );
    if ((fd = getfdFromchar_index(char_index)) != -1) {
      GmsvServer_EN_send(fd, BattleArray[battle_index].type,
                         BattleArray[battle_index].field_no);
    }
    flg = (BattleArray[battle_index]
               .Side[CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE)]
               .flg &
           BSIDE_FLG_HELP_OK)
              ? TRUE
              : FALSE;
    GmsvServer_HL_send(fd, flg);
    sprintf(szBuffer, "BP|%X|%X|%X", BATTLE_Index2No(battle_index, char_index),
            BP_FLG_JOIN, CHAR_getInt(char_index, CHAR_MP));
    BATTLE_CommandSend(char_index, szBuffer);
  } else {
    snprintf(szBuffer, sizeof(szBuffer), "无法参战。");
    CHAR_talkToCli(char_index, -1, szBuffer, CHAR_COLORYELLOW);
    if ((fd = getfdFromchar_index(char_index)) != -1) {
      GmsvServer_EN_send(fd, FALSE, BattleArray[battle_index].field_no);
    }
  }
  BATTLE_ClearGetExp(char_index);
  return iRet;
}

BOOL BATTLE_RescueTry(int char_index) {
  int x, y;
  OBJECT object;
  int pfound = FALSE;
  int result = FALSE;
  int fd;
  int i;
  int cnt;

  fd = getfdFromchar_index(char_index);
  if (CONNECT_checkfd(fd) == FALSE)
    return FALSE;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1, " 重复加入战斗! ", CHAR_COLORRED);
    print("角色重复加入战斗!:%s.\n", CHAR_getChar(char_index, CHAR_CDKEY));
    return FALSE;
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    GmsvServer_EN_send(fd, FALSE, 0);
    return FALSE;
  }
#ifdef _BATTLE_TIMESPEED
  if (CONNECT_getBDTime(fd) > time(NULL)) {
    return FALSE;
  }
#endif
  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setBattlechar_index(fd, i, -1);
  }
  CHAR_getCoordinationDir(CHAR_getInt(char_index, CHAR_DIR),
                          CHAR_getInt(char_index, CHAR_X),
                          CHAR_getInt(char_index, CHAR_Y), 1, &x, &y);

  cnt = 0;
  for (object = MAP_getTopObj(CHAR_getInt(char_index, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int objindex = GET_OBJINDEX(object);
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        && CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
    )
      continue;
    pfound = TRUE;
    if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
      continue;
    }
    if (BattleArray[CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX)].type !=
        BATTLE_TYPE_P_vs_E) {
      continue;
    }
    if (!(BattleArray[CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX)]
              .Side[CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE)]
              .flg &
          BSIDE_FLG_HELP_OK)) {
      continue;
    }
    CONNECT_setBattlechar_index(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;
  }
  if (cnt == 0) {
    if (pfound == FALSE) {
      CHAR_talkToCli(char_index, -1, "那里没有任何人。", CHAR_COLORYELLOW);
    }
    GmsvServer_EN_send(fd, FALSE, 0);
  } else if (cnt == 1) {
    if (BATTLE_RescueEntry(char_index, CONNECT_getBattlechar_index(fd, 0))) {
      CHAR_talkToCli(char_index, -1, "无法参战。",
                     CHAR_COLORYELLOW);
    } else {
      result = TRUE;
    }
  } else if (cnt > 1) {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n加入谁的战斗呢？\n");
    strlength = strlen(msgbuf);
    for (i = 0;
         CONNECT_getBattlechar_index(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getBattlechar_index(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("\nerr:%s:%d 视窗的讯息缓冲不足\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_SELECTBATTLE, -1,
                       makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  return result;
}

BOOL BATTLE_RescueParentTry(int char_index, int pindex) {
  if (CHAR_CHECKINDEX(char_index) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(pindex) == FALSE)
    return FALSE;
  int result = FALSE;
  if (CHAR_getFlg(char_index, CHAR_ISDIE))
    return FALSE;
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1) != pindex)
    return FALSE;
  if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
    return FALSE;
  }
  if (BATTLE_RescueEntry(char_index, pindex)) {
    result = FALSE;
  } else {
    result = TRUE;
  }
  if (result == FALSE) {
    CHAR_talkToCli(char_index, -1, "无法参战。", CHAR_COLORYELLOW);
  }
  return result;
}

int BATTLE_PartyNewEntry(int char_index, int battle_index, int side) {
  int iRet = 0, i, work;
#if 1

  iRet = BATTLE_NewEntry(char_index, battle_index, side);
  if (iRet)
    return iRet;
  CAflush(char_index);
  CDflush(char_index);
  iRet = BATTLE_PetDefaultEntry(char_index, battle_index, side);
  if (iRet)
    return iRet;
  BATTLE_ClearGetExp(char_index);
#endif

#ifdef _BATTLE_GETITEM_RATE
  if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
    if (CHAR_getInt(char_index, CHAR_FLOOR) == getBattleGetItemRateMap()) {
      CHAR_setWorkInt(char_index, CHAR_WORK_BATTLEPK, TRUE);
    }
  }
#endif

  for (i = 1; i < getPartyNum(char_index); i++) {
    work = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(work) == FALSE)
      continue;
    if (CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE &&
        CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_FINAL) {
      continue;
    }

#ifdef _BATTLE_GETITEM_RATE
    if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
      if (CHAR_getInt(work, CHAR_FLOOR) == getBattleGetItemRateMap()) {
        CHAR_setWorkInt(work, CHAR_WORK_BATTLEPK, TRUE);
      }
    }
#endif

    iRet = BATTLE_NewEntry(work, battle_index, side);

    if (iRet)
      break;
    CAflush(work);
    CDflush(work);
    iRet = BATTLE_PetDefaultEntry(work, battle_index, side);

    if (iRet)
      return iRet;
    BATTLE_ClearGetExp(work);
  }

  return iRet;
}

#if 1

int BATTLE_WatchNewEntry(int char_index, int battle_index, int side) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int iEntryFirst = 0, iEntryMax = BATTLE_ENTRY_MAX, i;

  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  pBattle = &BattleArray[battle_index];

  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  if (side < 0 || side > 1) {
    return BATTLE_ERR_PARAM;
  }

  switch (CHAR_getInt(char_index, CHAR_WHICHTYPE)) {

  case CHAR_TYPEPLAYER:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_PLAYER_MAX;
    break;
  default:
    print("不明(%s)(%d)\n", CHAR_getUseName(char_index),
          CHAR_getInt(char_index, CHAR_WHICHTYPE));
    return BATTLE_ERR_TYPE;
  }

  pEntry = pBattle->Side[side].Entry;

  for (i = iEntryFirst; i < iEntryMax; i++) {
    if (pEntry[i].char_index == -1)
      break;
  }

  if (i == iEntryMax) {
    return BATTLE_ERR_ENTRYMAX;
  }

  EntryInit(&pEntry[i]);

  pEntry[i].char_index = char_index;

  pEntry[i].bid = i + side * SIDE_OFFSET;

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, battle_index);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_WATCHINIT);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLESIDE, side);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, 0);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, -1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, -1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM3, -1);

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEWATCH, 1);

  return 0;
}

int BATTLE_WatchPartyNewEntry(int char_index, int battle_index, int side) {
  int iRet = 0, i, work;
#if 1

  iRet = BATTLE_WatchNewEntry(char_index, battle_index, side);
  if (iRet)
    return iRet;

  CAflush(char_index);

  CDflush(char_index);

#endif

  for (i = 1; i < getPartyNum(char_index); i++) {

    work = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(work) == FALSE)
      continue;

    if (CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != 0) {
      continue;
    }

    iRet = BATTLE_WatchNewEntry(work, battle_index, side);

    if (iRet)
      break;

    CAflush(work);
    CDflush(work);
  }
  return iRet;
}

int BATTLE_CreateForWatcher(int char_index, int topbattleindex);

int BATTLE_WatchEntry(int char_index, int toindex) {
  int iRet = 0, battle_index, fd;
  char szBuffer[256] = "";

  battle_index = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX);

  if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX) == -1) {
    iRet = 1;
  }

  if (iRet == 0) {
    iRet = BATTLE_CreateForWatcher(char_index, battle_index);
  }
  if (iRet == 0) {
    // snprintf( szBuffer, sizeof( szBuffer ), "(%s)开始观战。",
    //	CHAR_getUseName( char_index ) );

    // BATTLE_BroadCast( battle_index, szBuffer, CHAR_COLORYELLOW );
    if ((fd = getfdFromchar_index(char_index)) != -1) {
      GmsvServer_EN_send(fd, BattleArray[battle_index].type,
                         BattleArray[battle_index].field_no);
    }

    sprintf(szBuffer, "BP|%X|%X|%X", 20, BP_FLG_JOIN,
            CHAR_getInt(char_index, CHAR_MP));
    BATTLE_CommandSend(char_index, szBuffer);
    CHAR_setWorkInt(char_index, CHAR_WATCHBATTLETYPE, 1);
  } else {
    snprintf(szBuffer, sizeof(szBuffer), "无法观战。");
    CHAR_talkToCli(char_index, -1, szBuffer, CHAR_COLORYELLOW);

    if ((fd = getfdFromchar_index(char_index)) != -1) {
      GmsvServer_EN_send(fd, FALSE, 0);
    }
  }

  return iRet;
}

BOOL BATTLE_WatchTry(int char_index) {
  int x, y;
  OBJECT object;
  int pfound = FALSE;
  int result = FALSE;
  int fd;
  int i;
  int cnt;

  fd = getfdFromchar_index(char_index);
  if (CONNECT_checkfd(fd) == FALSE)
    return FALSE;

  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setBattlechar_index(fd, i, -1);
  }
  CHAR_getCoordinationDir(CHAR_getInt(char_index, CHAR_DIR),
                          CHAR_getInt(char_index, CHAR_X),
                          CHAR_getInt(char_index, CHAR_Y), 1, &x, &y);

  cnt = 0;
  /*戦闘の 前のキャラクターを取り 出す (取出战斗前方的角色) */
  for (object = MAP_getTopObj(CHAR_getInt(char_index, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int objindex = GET_OBJINDEX(object);
    /* キャラクタじゃない (不是角色) */
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    /* プレイヤーじゃない (不是玩家) */
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        && CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
    )
      continue;
    pfound = TRUE;
    /* 戦闘中でないなら次へ (未在战斗则跳过) */
    if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
      continue;
    }

    CONNECT_setBattlechar_index(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;
  }
  if (cnt == 0) {
    if (pfound == FALSE) {
      CHAR_talkToCli(char_index, -1, "那里没有任何人。", CHAR_COLORYELLOW);
    }
    // 誰もいなくて観戦 終了 (没有人，结束观战)
    GmsvServer_EN_send(fd, FALSE, 0);
  }
  /* 一人しかいない場合はエントリさせる (只有一人时直接观战) */
  else if (cnt == 1) {
    if (BATTLE_WatchEntry(char_index, CONNECT_getBattlechar_index(fd, 0))) {
      result = FALSE;
    } else {
      result = TRUE;
    }
  }
  /*   複数いたらウィンドウを 出して 選択させる (多人时弹出窗口选择) */
  else if (cnt > 1) {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n观看谁的战斗呢？\n");
    strlength = strlen(msgbuf);
    /* ウィンドウのメッセージを 生成 (生成窗口消息)
     * 戦闘のキャラの一覧 (战斗角色列表)
     */
    for (i = 0;
         CONNECT_getBattlechar_index(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getBattlechar_index(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("\nerr:%s:%d 视窗的讯息缓冲不足\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_SELECTBATTLEWATCH, -1,
                       makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  return result;
}

#endif

#define MAX_DOUJYOUENEMY 64
static int DoujyouEnemyTbl[10]; // とりあえず もいらんけど (暂时用不到但保留)
/***************************************************************
 *   モード用にキャラクターを取り 出す (模式用，取出角色)
 ***************************************************************/
int *Doujyou_GetEnemy(int meindex, int char_index) {
  int i;
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  char buf[1024];
  int ret;
  int insert;
  int WorkEnemyTbl[MAX_DOUJYOUENEMY];

  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));

  /* 初期化 (初始化) */
  for (i = 0; i < arraysizeof(WorkEnemyTbl); i++) {
    WorkEnemyTbl[i] = -1;
  }

  if (NPC_Util_GetStrFromStrWithDelim(argstr, "enemyno", buf, sizeof(buf)) ==
      NULL) {
    return NULL;
  }

  insert = 0;
  /* 引数に書いてある敵をエントリさせる (将参数中的敌人加入列表) */
  for (i = 0; i < MAX_DOUJYOUENEMY; i++) {
    int curEnemy;
    char data[128];

    ret = getStringFromIndexWithDelim(buf, ",", i + 1, data, sizeof(data));
    if (ret == FALSE) {
      break;
    }

    curEnemy = ENEMY_getEnemyArrayFromId(atoi(data));
    if (!ENEMY_CHECKINDEX(curEnemy))
      continue;
    WorkEnemyTbl[insert] = curEnemy;
    insert++;
  }
  // 誰もいなかったらエラー (一个都没有则报错)
  if (insert <= 0)
    return NULL;

  // ランダムに保存 (随机保存)
  DoujyouEnemyTbl[0] = WorkEnemyTbl[RAND(0, insert - 1)];

  if (NPC_Util_GetStrFromStrWithDelim(argstr, "enemypetno", buf, sizeof(buf)) ==
      NULL) {
    // いなかったら何も書かない (不存在则不写入)
    DoujyouEnemyTbl[1] = -1;
  } else {
    insert = 0;
    /* 引数に書いてある敵をエントリさせる (将参数中的敌人加入列表) */
    for (i = 0; i < MAX_DOUJYOUENEMY; i++) {
      int curEnemy;
      char data[128];

      ret = getStringFromIndexWithDelim(buf, ",", i + 1, data, sizeof(data));
      if (ret == FALSE) {
        break;
      }

      curEnemy = ENEMY_getEnemyArrayFromId(atoi(data));
      if (!ENEMY_CHECKINDEX(curEnemy))
        continue;
      WorkEnemyTbl[insert] = curEnemy;
      insert++;
    }
    // ランダムに保存 (随机保存)
    DoujyouEnemyTbl[1] = WorkEnemyTbl[RAND(0, insert - 1)];
  }

  // しない (不设置)
  DoujyouEnemyTbl[2] = -1;

  return DoujyouEnemyTbl;
}

static int DoujyouWeponTbl[] = {
    -1,       ITEM_FIST,      ITEM_AXE,        ITEM_CLUB,       ITEM_SPEAR,
    ITEM_BOW, ITEM_BOOMERANG, ITEM_BOUNDTHROW, ITEM_BREAKTHROW,
};
int DoujyouRandomWeponSet(int char_index) {
  int wepon = -1, item_index;
  int work = RAND(0, arraysizeof(DoujyouWeponTbl) - 1);
  switch (DoujyouWeponTbl[work]) {
  case ITEM_AXE:
    wepon = 0;
    break;
  case ITEM_CLUB:
    wepon = 100;
    break;
  case ITEM_SPEAR:
    wepon = 200;
    break;
  case ITEM_BOW:
    wepon = 2498;
    break;
  case ITEM_BOOMERANG:
    wepon = 500;
    break;
  case ITEM_BOUNDTHROW:
    wepon = 600;
    break;
  case ITEM_BREAKTHROW:
    wepon = 700;
    break;
  }
  item_index = CHAR_getItemIndex(char_index, CHAR_ARM);
  if (ITEM_CHECKINDEX(item_index) == TRUE) {
    ITEM_endExistItemsOne(item_index);
  }

  if (wepon >= 0) {
    item_index = ITEM_makeItemAndRegist(wepon);
    CHAR_setItemIndex(char_index, CHAR_ARM, item_index);
    ITEM_setWorkInt(item_index, ITEM_WORKCHARAINDEX, char_index);
    ITEM_setWorkInt(item_index, ITEM_WORKOBJINDEX, -1);
  }
  switch (DoujyouWeponTbl[work]) {
  case -1:
  case ITEM_AXE:
  case ITEM_CLUB:
  case ITEM_SPEAR:
    return 0;
  }
  return 1;
}

static int BattleERSkill1[] = {
    3,   10,  11,  12,  30,  31,  40,  41,  50,  51,  52,  60,  61,  80,
    90,  110, 120, 150, 210, 303, 309, 315, 321, 503, 504, 506, 507, 541,
    542, 543, 544, 545, 546, 547, 575, 579, 580, 606, 613, 615};

static int BattleERSkill2[] = {
    12,  13,  20,  41,  52,  152, 210, 306, 312, 318, 324, 325, 500, 501, 502,
    505, 508, 541, 542, 543, 544, 545, 546, 547, 576, 580, 594, 606, 613, 616};

static int BattleERSkill3[] = {13,  31,  41,  52,  60,  61,  80,  90,  100,
                               110, 120, 152, 210, 325, 500, 501, 504, 503,
                               504, 505, 506, 507, 508, 516, 610};

static int BattleERSkill4[] = {609, 610, 611};

void BATTLE_EnemyRandowSetSkill(int enemy_index, int skillType) {
  int i;
  for (i = 0; i < 7; i++) {
    int skillarray, skillID, Ri;
    switch (skillType) {
    case 1:
      Ri = RAND(0, arraysizeof(BattleERSkill1) - 1);
      skillID = BattleERSkill1[Ri];
      break;
    case 2:
      Ri = RAND(0, arraysizeof(BattleERSkill2) - 1);
      skillID = BattleERSkill2[Ri];
      break;
    case 3:
      Ri = RAND(0, arraysizeof(BattleERSkill3) - 1);
      skillID = BattleERSkill3[Ri];
      break;
    case 4:
      Ri = RAND(0, arraysizeof(BattleERSkill4) - 1);
      skillID = BattleERSkill4[Ri];
      break;
    case 5:
      skillID = RAND(301, 325);
      break;
    default:
      return;
    }
    skillarray = PETSKILL_getPetskillArray(skillID);
    if (PETSKILL_CHECKINDEX(skillarray) == FALSE) {
      CHAR_setPetSkill(enemy_index, i, 1);
      // andy_log
      print("ANDY EnemyRandowSetSkill( ) skillID:%d array:%d err \n", skillID,
            skillarray);
      continue;
    }
    CHAR_setPetSkill(enemy_index, i, skillID);
  }
}

int BATTLE_CreateVsEnemyNew(int char_index, int npcindex, int *table) {
  int battle_index, iRet = 0, enemy_index, i, pindex;
  int fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

  int *enemytable = table;

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

#ifdef _PLAYER_NPC
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
#endif
  {
#ifdef _STREET_VENDOR
    // 摆摊中不可进入战斗
    if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) > -1)
      return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
    if (CHAR_getWorkInt(char_index, CHAR_WORKANGELMODE) == TRUE)
      return BATTLE_ERR_END;
#endif
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;

  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(char_index, CHAR_FLOOR),
                                     CHAR_getInt(char_index, CHAR_X),
                                     CHAR_getInt(char_index, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battle_index].leaderindex = char_index;
  BattleArray[battle_index].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battle_index].createindex = npcindex;
  BattleArray[battle_index].field_no = field_no;
  BattleArray[battle_index].BattleFloor = CHAR_getInt(char_index, CHAR_FLOOR);

  BattleArray[battle_index].Side[0].flg &= ~BSIDE_FLG_HELP_OK;

  if (CHAR_CHECKINDEX(npcindex)) {
    if (CHAR_getInt(npcindex, CHAR_LV) > 1) {
      baselevel = CHAR_getInt(npcindex, CHAR_LV);
    }
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battle_index);
  } else if (npcindex == -1) {
    enemytable = ENEMY_getEnemy(char_index, CHAR_getInt(char_index, CHAR_X),
                                CHAR_getInt(char_index, CHAR_Y));
  }

  BattleArray[battle_index].norisk = 0;
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromchar_index(char_index);
  BattleArray[battle_index].CreateTime = time(NULL);
  BattleArray[battle_index].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemy_index = ENEMY_createEnemy(EnemyList[i], baselevel);

    if (!CHAR_CHECKINDEX(enemy_index)) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }
    CHAR_setInt(enemy_index, CHAR_WHICHTYPE, CHAR_TYPEENEMY);
    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemy_index, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemy_index, skillType);
    }

    if ((iRet = BATTLE_NewEntry(enemy_index, battle_index, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemy_index, CHAR_DUELPOINT) > 0) {
      BattleArray[battle_index].dpbattle = 1;
    }
    work = CHAR_getInt(enemy_index, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemy_index, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print("(%s)无生物\n", CHAR_getUseName(enemy_index));
    }
  }

  if ((iRet = BATTLE_PartyNewEntry(char_index, battle_index, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battle_index].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromchar_index(char_index);
  if (iRet) {
    BATTLE_ExitAll(battle_index);
    BATTLE_DeleteBattle(battle_index);
    if (fd != -1)
      GmsvServer_EN_send(fd, FALSE, field_no);
  } else {
    //		if(npcindex > -1){
    if (fd != -1) {
      if (BattleArray[battle_index].dpbattle) {
        GmsvServer_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battle_index);
        BATTLE_DeleteBattle(battle_index);
        return iRet;
      }
    }
    //		}

    for (i = 1; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromchar_index(pindex);

      if (fd != -1) {
        GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    }
    if (CHAR_getWorkInt(char_index, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(char_index, ON);
    for (i = 1; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }

      CHAR_sendBattleEffect(pindex, ON);
    }
  }

  return iRet;
}

int BATTLE_CreateVsEnemyLvNew(int char_index, int npcindex, int *table,
                              int *lvtable) {
  int battle_index, iRet = 0, enemy_index, i, pindex;
  int fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

  int *enemytable = table;

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

#ifdef _PLAYER_NPC
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
#endif
  {
#ifdef _STREET_VENDOR
    // 摆摊中不可进入战斗
    if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) > -1)
      return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
    if (CHAR_getWorkInt(char_index, CHAR_WORKANGELMODE) == TRUE)
      return BATTLE_ERR_END;
#endif
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;
#ifdef _WAN_FIX
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEPROTYPE, 1);
#endif
  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(char_index, CHAR_FLOOR),
                                     CHAR_getInt(char_index, CHAR_X),
                                     CHAR_getInt(char_index, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battle_index].leaderindex = char_index;
  BattleArray[battle_index].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battle_index].createindex = npcindex;
  BattleArray[battle_index].field_no = field_no;
  BattleArray[battle_index].BattleFloor = CHAR_getInt(char_index, CHAR_FLOOR);

  BattleArray[battle_index].Side[0].flg &= ~BSIDE_FLG_HELP_OK;

  if (CHAR_CHECKINDEX(npcindex)) {
    if (CHAR_getInt(npcindex, CHAR_LV) > 1) {
      baselevel = CHAR_getInt(npcindex, CHAR_LV);
    }
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battle_index);
  } else if (npcindex == -1) {
    enemytable = ENEMY_getEnemy(char_index, CHAR_getInt(char_index, CHAR_X),
                                CHAR_getInt(char_index, CHAR_Y));
  }

  BattleArray[battle_index].norisk = 0;
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromchar_index(char_index);
  BattleArray[battle_index].CreateTime = time(NULL);
  BattleArray[battle_index].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemy_index = ENEMY_createEnemy(EnemyList[i], lvtable[i]);

    if (!CHAR_CHECKINDEX(enemy_index)) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }
    CHAR_setInt(enemy_index, CHAR_WHICHTYPE, CHAR_TYPEENEMY);
    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemy_index, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemy_index, skillType);
    }

    if ((iRet = BATTLE_NewEntry(enemy_index, battle_index, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemy_index, CHAR_DUELPOINT) > 0) {
      BattleArray[battle_index].dpbattle = 1;
    }
    work = CHAR_getInt(enemy_index, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemy_index, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print("(%s)无生物\n", CHAR_getUseName(enemy_index));
    }
  }

  if ((iRet = BATTLE_PartyNewEntry(char_index, battle_index, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battle_index].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromchar_index(char_index);
  if (iRet) {
    BATTLE_ExitAll(battle_index);
    BATTLE_DeleteBattle(battle_index);
    if (fd != -1)
      GmsvServer_EN_send(fd, FALSE, field_no);
  } else {
    //		if(npcindex > -1){
    if (fd != -1) {
      if (BattleArray[battle_index].dpbattle) {
        GmsvServer_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battle_index);
        BATTLE_DeleteBattle(battle_index);
        return iRet;
      }
    }
    //		}

    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromchar_index(pindex);

      if (fd != -1) {
        GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    }
    if (CHAR_getWorkInt(char_index, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(char_index, ON);
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }

      CHAR_sendBattleEffect(pindex, ON);
    }
  }

  return iRet;
}

int BATTLE_CreateVsEnemy(int char_index, int mode, int npcindex) {
  int battle_index, iRet = 0, enemy_index, i, pindex;
  int *enemytable = NULL, fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT)
    return BATTLE_ERR_END;

#ifdef _STREET_VENDOR
  // 摆摊中不可进入战斗
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) > -1)
    return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
  if (CHAR_getWorkInt(char_index, CHAR_WORKANGELMODE) == TRUE)
    return BATTLE_ERR_END;
#endif

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;

  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battle_index);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(char_index, CHAR_FLOOR),
                                     CHAR_getInt(char_index, CHAR_X),
                                     CHAR_getInt(char_index, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battle_index].leaderindex = char_index;
  BattleArray[battle_index].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battle_index].createindex = npcindex;
  BattleArray[battle_index].field_no = field_no;
  BattleArray[battle_index].BattleFloor = CHAR_getInt(char_index, CHAR_FLOOR);
  // andy_edit 2002/10/23
  /*
  if( (iRet = BATTLE_PartyNewEntry( char_index, battle_index, 0 ) ) ){
          goto BATTLE_CreateVsEnemy_End;
  }
  */
  BattleArray[battle_index].Side[0].flg &= ~BSIDE_FLG_HELP_OK;
  if (mode == 0) {
    enemytable = ENEMY_getEnemy(char_index, CHAR_getInt(char_index, CHAR_X),
                                CHAR_getInt(char_index, CHAR_Y));
  } else if (mode == 1) {
    enemytable = NPC_Util_getEnemy(npcindex, char_index);
  } else if (mode == 2) {
    enemytable = Doujyou_GetEnemy(npcindex, char_index);
    baselevel = CHAR_getInt(npcindex, CHAR_LV);
    BattleArray[battle_index].norisk = 1;
  }
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  BattleArray[battle_index].CreateTime = time(NULL);
  BattleArray[battle_index].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemy_index = ENEMY_createEnemy(EnemyList[i], baselevel);

    if (enemy_index < 0) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }

    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemy_index, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemy_index, skillType);
    }

    if (mode == 2) {
      if (i == 0) {
        CHAR_setInt(enemy_index, CHAR_BASEBASEIMAGENUMBER,
                    CHAR_getInt(npcindex, CHAR_BASEBASEIMAGENUMBER));
        CHAR_setInt(enemy_index, CHAR_BASEIMAGENUMBER,
                    CHAR_getInt(npcindex, CHAR_BASEBASEIMAGENUMBER));
        CHAR_setChar(enemy_index, CHAR_NAME, CHAR_getChar(npcindex, CHAR_NAME));
        CHAR_complianceParameter(enemy_index);
      }
    }
    if ((iRet = BATTLE_NewEntry(enemy_index, battle_index, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemy_index, CHAR_DUELPOINT) > 0) {
      BattleArray[battle_index].dpbattle = 1;
    }
    work = CHAR_getInt(enemy_index, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemy_index, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print("(%s)无生物\n", CHAR_getUseName(enemy_index));
    }
  }
  // andy_edit 2002/10/23
  if ((iRet = BATTLE_PartyNewEntry(char_index, battle_index, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battle_index].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromchar_index(char_index);

  if (iRet) {
#ifdef _OFFLINE_SYSTEM
    if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
    {
      BATTLE_ExitAll(battle_index);
      BATTLE_DeleteBattle(battle_index);
      if (fd != -1)
        GmsvServer_EN_send(fd, FALSE, field_no);
    }
  } else {
    if (fd != -1) {
      if (BattleArray[battle_index].dpbattle) {
        GmsvServer_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        if (mode == 1) {
          GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
        } else {
          GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
        }
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battle_index);
        BATTLE_DeleteBattle(battle_index);
        return iRet;
      }
    }

    for (i = 1; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromchar_index(pindex);
      if (fd != -1) {
        if (mode == 1) {
          GmsvServer_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
        } else {
          GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
        }
      }
    }
    if (CHAR_getWorkInt(char_index, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(char_index, ON);
    for (i = 1; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(pindex, ON);
    }
  }
  return iRet;
}

int BATTLE_CreateVsPlayer(int char_index0, int char_index1) {
  int battle_index, pindex, field_no, i, j, char_index[2], parent[2], fd,
      iRet = 0;

  if (CHAR_CHECKINDEX(char_index0) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_CHECKINDEX(char_index1) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(char_index0, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(char_index1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(char_index1, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(char_index1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }

#ifdef _BATTLE_OVER_STAR_TIME
  if (CHAR_getWorkInt(char_index0, CHAR_WORKDBATTLETIME) + 120 > time(NULL)) {
    char token[256];
    sprintf(token, "你当前需冷却%d后才能进行PK",
            CHAR_getWorkInt(char_index0, CHAR_WORKDBATTLETIME) + 120 -
                time(NULL));
    CHAR_talkToCli(char_index0, -1, token, CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(char_index1, CHAR_WORKDBATTLETIME) + 120 > time(NULL)) {
    char token[256];
    sprintf(token, "对方需冷却%d后才能进行PK",
            CHAR_getWorkInt(char_index1, CHAR_WORKDBATTLETIME) + 120 -
                time(NULL));
    CHAR_talkToCli(char_index0, -1, token, CHAR_COLORYELLOW);
    sprintf(token, "你当前需冷却%d后才能进行PK",
            CHAR_getWorkInt(char_index1, CHAR_WORKDBATTLETIME) + 120 -
                time(NULL));
    CHAR_talkToCli(char_index1, -1, token, CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
#endif

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(char_index0, CHAR_FLOOR),
                                     CHAR_getInt(char_index0, CHAR_X),
                                     CHAR_getInt(char_index0, CHAR_Y));

  char_index[0] = char_index0;
  char_index[1] = char_index1;
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(char_index[j], CHAR_WORKBATTLEMODE) != 0) {
      return BATTLE_ERR_ALREADYBATTLE;
    }
  }
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYMODE) ==
        CHAR_PARTY_LEADER) {
      parent[j] = char_index[j];
    } else if (CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYMODE) ==
               CHAR_PARTY_CLIENT) {
      parent[j] = CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYINDEX1);
    } else {
      parent[j] = -1;
    }
  }
  if (parent[0] != -1 && parent[0] == parent[1]) {
    return BATTLE_ERR_SAMEPARTY;
  }
  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;

  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].leaderindex = char_index0;

#ifdef _AUTO_PK
  strcpy(BattleArray[battle_index].leadercdkey,
         CHAR_getChar(char_index0, CHAR_CDKEY));
  strcpy(BattleArray[battle_index].leadername,
         CHAR_getChar(char_index0, CHAR_NAME));
  BattleArray[battle_index].rivalindex = char_index1;
  strcpy(BattleArray[battle_index].rivalcdkey,
         CHAR_getChar(char_index1, CHAR_CDKEY));
  strcpy(BattleArray[battle_index].rivalname,
         CHAR_getChar(char_index1, CHAR_NAME));
#endif
  BattleArray[battle_index].type = BATTLE_TYPE_P_vs_P;
  BattleArray[battle_index].dpbattle = 1;
  BattleArray[battle_index].field_no = field_no;
  BattleArray[battle_index].BattleFloor = CHAR_getInt(char_index0, CHAR_FLOOR);
#ifdef _BATTLE_TIMESPEED
  BattleArray[battle_index].CreateTime = time(NULL);
#endif
  for (j = 0; j < 2; j++) {
    iRet = BATTLE_PartyNewEntry(char_index[j], battle_index, j);
    if (iRet) {
      goto BATTLE_CreateVsPlayer_End;
    }
    BattleArray[battle_index].Side[j].flg &= ~BSIDE_FLG_HELP_OK;
  }
BATTLE_CreateVsPlayer_End:;
  if (iRet) {
    BATTLE_ExitAll(battle_index);
    BATTLE_DeleteBattle(battle_index);
    fd = getfdFromchar_index(char_index[0]);
    if (fd != -1)
      GmsvServer_EN_send(fd, FALSE, field_no);
  } else {
    for (j = 0; j < 2; j++) {
      fd = getfdFromchar_index(char_index[j]);
      if (fd != -1)
        GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
      if (CHAR_getWorkInt(char_index[j], CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(char_index[j], CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(char_index[j], CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(char_index[j], ON);
      for (i = 1; i < getPartyNum(char_index[j]); i++) {
        pindex = CHAR_getWorkInt(char_index[j], i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(pindex) == FALSE)
          continue;
        if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_FINAL)
          continue;

        fd = getfdFromchar_index(pindex);
        if (fd != -1)
          GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
        if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
          CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                              CHAR_ACTSTAND, NULL, 0, FALSE);
          CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
        }
        CHAR_sendBattleEffect(pindex, ON);
      }
    }
  }
  return iRet;
}

#ifdef _TRADE_PK
int BATTLE_CreateVsPlayerForTrade(STradeList TradeList1,
                                  STradeList TradeList2) {
  int battle_index, pindex, field_no, i, j, char_index[2], parent[2], fd,
      iRet = 0;
  char_index[0] = TradeList1.char_index;
  char_index[1] = TradeList2.char_index;
  if (CHAR_CHECKINDEX(char_index[0]) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_CHECKINDEX(char_index[1]) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(char_index[0], CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index[0], -1, "自己已经在一场战斗中", CHAR_COLORYELLOW);
    CHAR_talkToCli(char_index[1], -1, "对方已经在一场战斗中", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(char_index[1], CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index[0], -1, "对方已经在一场战斗中", CHAR_COLORYELLOW);
    CHAR_talkToCli(char_index[1], -1, "自己已经在一场战斗中", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(char_index[0], CHAR_FLOOR),
                                     CHAR_getInt(char_index[0], CHAR_X),
                                     CHAR_getInt(char_index[0], CHAR_Y));
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(char_index[j], CHAR_WORKBATTLEMODE) != 0) {
      return BATTLE_ERR_ALREADYBATTLE;
    }
  }
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYMODE) ==
        CHAR_PARTY_LEADER) {
      parent[j] = char_index[j];
    } else if (CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYMODE) ==
               CHAR_PARTY_CLIENT) {
      parent[j] = CHAR_getWorkInt(char_index[j], CHAR_WORKPARTYINDEX1);
    } else {
      parent[j] = -1;
    }
  }
  if (parent[0] != -1 && parent[0] == parent[1]) {
    return BATTLE_ERR_SAMEPARTY;
  }
  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;

  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].leaderindex = char_index[0];
  BattleArray[battle_index].rivalindex = char_index[1];

#ifdef _TRADE_PK
  memcpy(&BattleArray[battle_index].TradeList[0], &TradeList1,
         sizeof(STradeList));
  memcpy(&BattleArray[battle_index].TradeList[1], &TradeList2,
         sizeof(STradeList));
#endif

  BattleArray[battle_index].type = BATTLE_TYPE_P_vs_P;
  BattleArray[battle_index].dpbattle = 1;
  BattleArray[battle_index].field_no = field_no;
  BattleArray[battle_index].BattleFloor = CHAR_getInt(char_index[0], CHAR_FLOOR);
#ifdef _BATTLE_TIMESPEED
  BattleArray[battle_index].CreateTime = time(NULL);
#endif
  for (j = 0; j < 2; j++) {
    iRet = BATTLE_PartyNewEntry(char_index[j], battle_index, j);
    if (iRet) {
      goto BATTLE_CreateVsPlayer_End;
    }
    BattleArray[battle_index].Side[j].flg &= ~BSIDE_FLG_HELP_OK;
  }
BATTLE_CreateVsPlayer_End:;
  if (iRet) {
    BATTLE_ExitAll(battle_index);
    BATTLE_DeleteBattle(battle_index);
    fd = getfdFromchar_index(char_index[0]);
    if (fd != -1)
      GmsvServer_EN_send(fd, FALSE, field_no);
  } else {
    for (j = 0; j < 2; j++) {
      fd = getfdFromchar_index(char_index[j]);
      if (fd != -1)
        GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
      if (CHAR_getWorkInt(char_index[j], CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(char_index[j], CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(char_index[j], CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(char_index[j], ON);
      for (i = 1; i < getPartyNum(char_index[j]); i++) {
        pindex = CHAR_getWorkInt(char_index[j], i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(pindex) == FALSE)
          continue;
        if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_FINAL)
          continue;

        fd = getfdFromchar_index(pindex);
        if (fd != -1)
          GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
        if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
          CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                              CHAR_ACTSTAND, NULL, 0, FALSE);
          CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
        }
        CHAR_sendBattleEffect(pindex, ON);
      }
    }
  }
  return iRet;
}

#endif

//*********************************************************
//
// 観戦用バトルタスクを追加、トップの後ろに保存する (追加观战战斗，存到top之后)
//
int BATTLE_WatchLink(int topbattleindex, int battle_index)
//
//
//*********************************************************
{
  BATTLE *pWork, *pTop;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    printEx("err:battle index 奇怪(%d)\n", battle_index);
    return FALSE;
  }
  if (BATTLE_CHECKINDEX(topbattleindex) == FALSE) {
    printEx("err:battle index 奇怪(%d)\n", topbattleindex);
    return FALSE;
  }

  pTop = &BattleArray[topbattleindex];

  if (BATTLE_CHECKADDRESS(pTop) == FALSE) {
    printEx("err:battle address 奇怪(%p)\n", pTop);
    return FALSE;
  }

  pWork = pTop->pNext; // バックアップ (备份)

  if (pWork) {
    if (BATTLE_CHECKADDRESS(pWork) == FALSE) {
      printEx("err:battle address 奇怪(%p)\n", pWork);
      return FALSE;
    }
  }

  // 戦闘を前の後ろにする (把战斗放到前一个之后)
  pTop->pNext = &BattleArray[battle_index];
  // 戦闘の前バトルを保存 (保存战斗的前一个)
  BattleArray[battle_index].pBefore = pTop;
  // 戦闘の後バトルを保存 (保存战斗的后一个)
  BattleArray[battle_index].pNext = pWork;
  // 後の前は戦闘 (后一个的前一个是此战斗)
  if (pWork) {
    pWork->pBefore = &BattleArray[battle_index];
  }
  return TRUE;
}

//*********************************************************
//
// 観戦用バトルタスクを取り 外す (解除观战战斗链接)
//
int BATTLE_WatchUnLink(int battle_index)
//
//
//*********************************************************
{
  BATTLE *pTop;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    printEx("err:battle index 奇怪(%d)\n", battle_index);
    return FALSE;
  }

  // 戦闘の前のバトル (战斗的前一个)
  pTop = BattleArray[battle_index].pBefore;

  if (pTop) {
    if (BATTLE_CHECKADDRESS(pTop) == FALSE) {
      printEx("err:battle address 奇怪(%p)\n", pTop);
    } else {
      // 繋ぎ直し (重新连接)
      pTop->pNext = BattleArray[battle_index].pNext;
    }
  }
  if (BattleArray[battle_index].pNext) {
    if (BATTLE_CHECKADDRESS(BattleArray[battle_index].pNext) == FALSE) {
      printEx("err:battle address 奇怪(%p)\n", BattleArray[battle_index].pNext);
    } else {
      BattleArray[battle_index].pNext->pBefore = pTop;
    }
  }
  // 戦闘の前バトルを保存 (保存战斗的前一个)
  BattleArray[battle_index].pBefore = NULL;
  // 戦闘の後バトルは 無し (战斗的后一个设为无)
  BattleArray[battle_index].pNext = NULL;

  return TRUE;
}

int BATTLE_CreateForWatcher(int char_index, int topbattleindex) {
  int battle_index, field_no, pindex, i, fd, iRet = 0;

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(char_index, -1, "已经在一场战斗中，无法观战",
                   CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) != 0) {
    return BATTLE_ERR_ALREADYBATTLE;
  }

  battle_index = BATTLE_CreateBattle();
  if (battle_index < 0)
    return BATTLE_ERR_NOTASK;
  BattleArray[battle_index].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].Side[1].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battle_index].leaderindex = char_index;
  BattleArray[battle_index].type = BATTLE_TYPE_WATCH;
  BattleArray[battle_index].mode = BATTLE_MODE_WATCHBC;
  field_no = BattleArray[battle_index].field_no =
      BattleArray[topbattleindex].field_no;
  BattleArray[battle_index].turn = BattleArray[topbattleindex].turn;

  if (BATTLE_WatchLink(topbattleindex, battle_index) == FALSE) {
    printEx("err:battle link error(%d),(%d)\n", topbattleindex, battle_index);
    goto BATTLE_CreateForWatcher_End;
  }

  iRet = BATTLE_WatchPartyNewEntry(char_index, battle_index, 0);
  if (iRet) {
    goto BATTLE_CreateForWatcher_End;
  }

BATTLE_CreateForWatcher_End:;

  if (iRet) { // エラーがあったらタスクを 放棄 (出错则放弃任务)
    // 破棄する (销毁)
    BATTLE_ExitAll(battle_index);
    // 戦闘タスク削除 (删除战斗任务)
    BATTLE_DeleteBattle(battle_index);
    fd = getfdFromchar_index(char_index);
    if (fd != -1)
      GmsvServer_EN_send(fd, FALSE, field_no);
  } else {
    fd = getfdFromchar_index(char_index);
    /* エンカウントを 送る (发送遭遇) */
    if (fd != -1)
      GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
    /*   送る (发送) */
    if (CHAR_getWorkInt(char_index, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    }
    /* 戦闘アイコン表示送信 (发送战斗图标显示) */
    CHAR_sendBattleWatch(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), ON);

    // 仲間にも送る (也发给队友)
    for (i = 1; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      // ちゃんとエンカウントできていないなら送らない (未正常进入战斗则不发送)
      // FINAL だったら前の戦闘を している (若为FINAL状态则跳过)
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;

      fd = getfdFromchar_index(pindex);
      /* エンカウントを 送る (发送遭遇)   */
      if (fd != -1)
        GmsvServer_EN_send(fd, BattleArray[battle_index].type, field_no);
      /*   送る (发送) */
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }
      /* 戦闘アイコン表示送信 (发送战斗图标显示) */
      CHAR_sendBattleWatch(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), ON);
    }
    /*
                    print( "以(%s)为领队与敌军交手。",
                            CHAR_getChar( char_index, CHAR_NAME ) );
    */
  }

  return iRet;
}

/*------------------------------------------------------------
 * 戦闘を 止める (停止战斗)
 ------------------------------------------------------------*/
void BATTLE_WatchStop(int char_index) {
  int battle_index;

  battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return;
  // キャラクタがいなかったら 除外させる (角色不在则退出)
  BATTLE_Exit(char_index, battle_index);
  // パーティから 除外する (从队伍中移除)
  CHAR_DischargePartyNoMsg(char_index);

  CHAR_talkToCli(char_index, -1, "战斗中止。", CHAR_COLORYELLOW);
  CHAR_setWorkInt(char_index, CHAR_WATCHBATTLETYPE, 0);
  GmsvServer_B_send(getfdFromchar_index(char_index), "BU");
}

//*************************************************************
//
//    ターン毎にパラメータを 補正していく (每回合修正参数)
//
void BATTLE_TurnParam(int char_index, int fixkind, int mod, int last)
//
//
//*************************************************************
{
  int modparam, fixparam, lastparam;

  if (fixkind == -1) {
    fixparam = 0; // 元が い場合は0 (原值为-1则取0)
  } else {
    fixparam = CHAR_getWorkInt(char_index, fixkind);
  }
  if (last == -1) {
    lastparam = 0;
  } else {
    lastparam = CHAR_getWorkInt(char_index, last);
  }
  modparam = CHAR_getWorkInt(char_index, mod);
  // ここで 元の 値を 抑制して (此处对原值做衰减)
  //       の値を抑制していく (对该值进行衰减)

  // 今は MODPARAM を減少していく (此处将MODPARAM衰减)
  modparam *= 0.8;
  CHAR_setWorkInt(char_index, mod, modparam);

  //         に加える (累加)
  if (last != -1) {
    CHAR_setWorkInt(char_index, last, lastparam + modparam * 0.01);
  }
}

void BATTLE_AttReverse(int char_index) {
  int earth, water, fire, wind;
  // ttom start  because the second had this
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
      CHAR_BATTLEFLG_REVERSE) {
    //   してるなら次へ (若已反转则继续)
  } else {
    //   してないのではリターン (未反转则返回)
    return;
  }
  // ttom end
  earth = CHAR_getWorkInt(char_index, CHAR_WORKFIXEARTHAT);
  water = CHAR_getWorkInt(char_index, CHAR_WORKFIXWATERAT);
  fire = CHAR_getWorkInt(char_index, CHAR_WORKFIXFIREAT);
  wind = CHAR_getWorkInt(char_index, CHAR_WORKFIXWINDAT);

  CHAR_setWorkInt(char_index, CHAR_WORKFIXEARTHAT, fire);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXWATERAT, wind);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXFIREAT, earth);
  CHAR_setWorkInt(char_index, CHAR_WORKFIXWINDAT, water);
}

void BATTLE_PreCommandSeq(int battle_index) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int i, j, char_index;
  BATTLE_CharSendAll(battle_index);
  BATTLE_CharaBackUp(battle_index);
  BattleArray[battle_index].timer = NowTime.tv_sec;
  BATTLE_AllCharaCWaitSet(battle_index);
  BATTLE_ActSettingSend(battle_index);
  BattleArray[battle_index].flg |= BATTLE_FLG_FREEDP;
  pBattle = &BattleArray[battle_index];
  for (j = 0; j < 2; j++) {
    int flg;
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry[i].guardian = -1;
      char_index = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      flg = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG);
      // ttom the second only one //flg &= ~CHAR_BATTLEFLG_GUARDIAN;
      flg &= ~CHAR_BATTLEFLG_GUARDIAN;
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, flg);
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_EARTHROUND0)
        continue;

      CHAR_complianceParameter(char_index);
      BATTLE_TurnParam(char_index,
                       CHAR_WORKFIXSTR, //   猾
                       CHAR_WORKMODATTACK, CHAR_WORKATTACKPOWER);
      BATTLE_TurnParam(char_index,
                       CHAR_WORKFIXTOUGH, // 潮
                       CHAR_WORKMODDEFENCE, CHAR_WORKDEFENCEPOWER);
      BATTLE_TurnParam(char_index,
                       CHAR_WORKFIXDEX, // 素早さ (敏捷)
                       CHAR_WORKMODQUICK, CHAR_WORKQUICK);
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        BATTLE_TurnParam(char_index, -1,
                         CHAR_WORKMODCHARM, //
                         CHAR_WORKFIXCHARM);
      }
      BATTLE_TurnParam(char_index,
                       -1, //   凯
                       CHAR_WORKMODCHARM, -1);
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
          CHAR_BATTLEFLG_REVERSE) {
        BATTLE_AttReverse(char_index);
      }
    }
  }
}

//**************************************************
//
// 観戦の場合のコマンド処理、 時の一連の 流れ (观战时指令处理流程)
//
void BATTLE_PreWatchWaitSeq(int battle_index)
//
//
//**************************************************
{
  // この時の時間で 保存 (保存当前时间)
  BattleArray[battle_index].timer = NowTime.tv_sec;
  // 全員を待機モードにする (全员进入待机模式)
  BATTLE_AllCharaWatchWaitSet(battle_index);
}
static int BATTLE_Init(int battle_index) {
  BATTLE *pBattle;
  int iRet = 0;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battle_index];
  pBattle->mode = BATTLE_MODE_BATTLE;
  iRet = BATTLE_SurpriseCheck(battle_index);
  if (iRet == 0) {
  } else if (iRet == 1) {
    BattleArray[battle_index].Side[1].flg |= BSIDE_FLG_SURPRISE;
  } else if (iRet == 2) {
    BattleArray[battle_index].Side[0].flg |= BSIDE_FLG_SURPRISE;
  }
  BATTLE_PreCommandSeq(battle_index);
#ifdef _MO_LUA_BATTLE_START
  BattleStartFunction(battle_index);
#endif
  return 0;
}
int BATTLE_CountEntry(int battle_index, int side) {
  int i;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // パラメータチェック (参数检查)
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // エントリ (入场)
  pEntry = BattleArray[battle_index].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    if (pEntry[i].char_index != -1) { // キャラがいたら数える (有角色则计数)
      cnt++;
    }
  }
  return cnt;
}

static BOOL BATTLE_CommandWait(int battle_index, int side) {
  int i, char_index, BeOk = 0;
  BATTLE_ENTRY *pEntry;
  BOOL iRet = TRUE;
  BOOL TimeOut = FALSE;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return TRUE; //检查值是否在合法  围
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return TRUE;
  if (BattleArray[battle_index].Side[side].type == BATTLE_S_TYPE_ENEMY)
    return TRUE;
  pEntry = BattleArray[battle_index].Side[side].Entry;
  int playerindex;
#ifdef _BATTLECOMMAND_TIME
  // print("\nPartTime=%d",BattleArray[battle_index].PartTime);
  if (BattleArray[battle_index].PartTime > 1 &&
      BattleArray[battle_index].PartTime < time(NULL)) {
    TimeOut = TRUE;
  }
#endif

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;

    if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
      continue;
    }

#ifdef _OFFLINE_SYSTEM
    if (CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 1) {
      OffLineCommand(battle_index, char_index, side);
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
          || CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif
      ) {
        int petnum = CHAR_getInt(char_index, CHAR_DEFAULTPET);
        int petindex = CHAR_getCharPet(char_index, petnum);
        if (CHAR_CHECKINDEX(petindex)) {
          OffLineCommand(battle_index, petindex, side);
        }
      }
    }
#endif
    switch (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE)) {
    case BATTLE_CHARMODE_C_OK:
#ifdef _BATTLE_BOUT_TIME
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        if (CHAR_getWorkInt(char_index, CHAR_BATTLE_BOUT_TIME) >
            (int)time(NULL) - 1) {
          if (CHAR_getInt(char_index, CHAR_MP) > 0) {
            CHAR_talkToCli(char_index, -1,
                           "系统检测到你正在控制战斗，已自动将 MP 设为 0。",
                           CHAR_COLORYELLOW);
            CHAR_setInt(char_index, CHAR_MP, 0);
          }
        }
      }
#endif
      BeOk++;
    case BATTLE_CHARMODE_INIT:
    case BATTLE_CHARMODE_RESCUE:
    case BATTLE_CHARMODE_WATCHINIT:
      break;
    case BATTLE_CHARMODE_C_WAIT: {
#ifdef _BATTLECOMMAND_TIME
      if (TimeOut == TRUE) {
// andy_log
#ifdef _OFFLINE_SYSTEM
#endif
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
#ifdef _OFFLINE_SYSTEM
#endif
      }
#endif
      iRet = FALSE;
    } break;
    default:
      BATTLE_Exit(char_index, battle_index);
      break;
    }
  }

  if (BeOk > 0) {
#ifdef _BATTLECOMMAND_TIME
    if (BattleArray[battle_index].PartTime <= 0) {
      // 如果战役中有任一人执行指令, 则延迟时间设为120秒
      BattleArray[battle_index].PartTime = (int)time(NULL) + 120;
    }
#endif
  } else {
    if (BattleArray[battle_index].PartTime <= 0) {
      BattleArray[battle_index].PartTime = (int)time(NULL) + 99;
    }
  }
  return iRet;
}

#ifdef _ITEM_ADDEXP // vincent 经验提升
#ifdef _ITEM_ADDEQUIPEXP
int BATTLE_GetExp(int char_index, int midx)
#else
int BATTLE_GetExp(int char_index)
#endif
{
  // 2026.09.07 修改，totalAddExp没有初始化，导致经验计算错误
  int totalAddExp = 0, i; // 本次增加的总EXP
  int extraExp = 0; // extraExp: 额外获得的EXP
  int baseExp = 0; // baseExp: 基础获得的EXP
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return 0;

  // CHAR是PET
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int ownerindex = CHAR_getWorkInt(char_index, CHAR_WORKPLAYERINDEX);
    if (CHAR_CHECKINDEX(ownerindex)) {
      if (CHAR_getWorkInt(ownerindex, CHAR_WORKITEM_ADDEXP) > 0) {
        extraExp += CHAR_getWorkInt(ownerindex, CHAR_WORKITEM_ADDEXP);
      }
#ifdef _PET_ADD_EXP
      if (CHAR_getWorkInt(char_index, CHAR_WORKITEM_ADDEXP) > 0) {
        extraExp += CHAR_getWorkInt(char_index, CHAR_WORKITEM_ADDEXP);
      }
#endif
    }
  } else if (CHAR_getWorkInt(char_index, CHAR_WORKITEM_ADDEXP) > 0) {
    extraExp = CHAR_getWorkInt(char_index, CHAR_WORKITEM_ADDEXP);
  }

  baseExp = CHAR_getWorkInt(char_index, CHAR_WORKGETEXP);
#ifdef _NEWOPEN_MAXEXP
  if (baseExp > 1e9)
    baseExp = 1e9;
#endif
#ifdef _BATTLE_GOLD
  int gold = CHAR_getInt(char_index, CHAR_GOLD);
  if ((gold + getBattleGold()) > CHAR_getMaxHaveGold(char_index))
    gold = CHAR_getMaxHaveGold(char_index);
  else
    gold += getBattleGold();
  CHAR_setInt(char_index, CHAR_GOLD, gold);
  CHAR_complianceParameter(char_index);
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_GOLD);
#endif
  // 如果baseExp < 0, 或者下一级的经验为-1(已经升满级了)
  if (baseExp < 0 ||
      CHAR_GetLevelExp(char_index, CHAR_getInt(char_index, CHAR_LV) + 1) ==
          -1) {
    baseExp = 0;
  }
  // 2026.09.07 增加经验
  else if (baseExp < 1e6) {
    baseExp = 1e6;
  }
  totalAddExp += baseExp; /* 基础经验作为加成基数 */
#ifdef _ITEM_ADDEQUIPEXP
  if (CHAR_CHECKINDEX(midx)) {
    for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
      int id = CHAR_getItemIndex(midx, i); // 道具id
      if (ITEM_CHECKINDEX(id)) {
        char *arg, *P;
        arg = ITEM_getChar(id, ITEM_ARGUMENT); // 道具参数
        if ((P = strstr(arg, "EXPUP"))) {
          if ((P = strstr(arg, "人"))) { // 只对人有效
            if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
              totalAddExp += baseExp * atoi(P += 2) * 0.01 * getBattleexp();
          } else if ((P = strstr(arg, "宠"))) { //只对宠有效
            if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
                (CHAR_getInt(midx, CHAR_RIDEPET) != char_index))
              totalAddExp += baseExp * atoi(P += 2) * 0.01 * getBattleexp();
          } else if ((P = strstr(arg, "骑"))) { //只对骑宠有效
            if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
                (CHAR_getInt(midx, CHAR_RIDEPET) == char_index))
              totalAddExp += baseExp * atoi(P += 2) * 0.01 * getBattleexp();
          } else {
            P = strstr(arg, "EXPUP");
            totalAddExp += baseExp * atoi(P += 5) * 0.01 * getBattleexp();
          }
        }
      }
    }
  }
#endif
#ifdef _ITEM_ADDPETEXP
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET)
    if (CHAR_getInt(char_index, CHAR_PETID) == 1163)
      totalAddExp = 0;
#endif
  totalAddExp = (totalAddExp < 0) ? 0 : totalAddExp;
  if (CHAR_getInt(char_index, CHAR_LV) >= CHAR_MAXUPLEVEL)
    totalAddExp = 0;
  CHAR_setWorkInt(char_index, CHAR_WORKGETEXP, totalAddExp); //回存CHAR_WORKGETEXP
  // print("EXP: %d %d %d\n", baseExp, extraExp, totalAddExp);
  CHAR_AddMaxExp(char_index, totalAddExp);
  return totalAddExp;
}
#else

#ifdef _ITEM_ADDEQUIPEXP
int BATTLE_GetExp(int char_index, int midx)
#else
int BATTLE_GetExp(int char_index)
#endif
{
  int addexp, nowexp;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return 0;
  nowexp = CHAR_getInt(char_index, CHAR_EXP);
  addexp = CHAR_getWorkInt(char_index, CHAR_WORKGETEXP);
  // shan 11/27 10^9 1224160000
  CHAR_AddMaxExp(char_index, addexp);
  return addexp;
}
#endif

int BATTLE_DpCalc(int battle_index) {
  BATTLE_ENTRY *pLooseEntry, *pWinEntry;
  int winside, looseside, i, char_index, dpadd, dpall, num = 0;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  winside = BattleArray[battle_index].winside;
  looseside = 1 - winside;
  if (winside != -1 && winside != 1)
    return BATTLE_ERR_PARAM;

  dpall = 0;

  pLooseEntry = BattleArray[battle_index].Side[looseside].Entry;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pLooseEntry[i].char_index;
    // プレイヤーでなければ除外 (非玩家则跳过)
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    // その分の もらえる (取其相应数值)
    dpadd = CHAR_getInt(char_index, CHAR_DUELPOINT) * DUELPOINT_RATE;
    //   敗けた には をマイナス保存 (败方扣减经验)
    CHAR_setWorkInt(char_index, CHAR_WORKGETEXP,
                    CHAR_getWorkInt(char_index, CHAR_WORKGETEXP) - dpadd);
    // 決闘計に加算 (决斗点累加)
    dpall += dpadd;
  }
  // さらに 勝利用の を加算 (再加上胜方公共决斗点)
  dpall += BattleArray[battle_index].Side[winside].common_dp;

  pWinEntry = BattleArray[battle_index].Side[winside].Entry;
  for (num = 0, i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pWinEntry[i].char_index;
    // プレイヤーでなければ除外 (非玩家则跳过)
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    num++;
  }
  // 勝った方に 誰もいない (胜方无人则出错)
  if (num <= 0)
    return BATTLE_ERR_BATTLEINDEX;
  dpadd = dpall / num;
  if (dpadd <= 0)
    dpadd = 1; //   0でも 与える (0也给)

  for (num = 0, i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pWinEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    CHAR_setWorkInt(char_index, CHAR_WORKGETEXP,
                    CHAR_getWorkInt(char_index, CHAR_WORKGETEXP) + dpadd);
  }
  return 0;
}

// #define RS_LIST_MAX	4	//   時に何 戦闘結果を送るか (何时发送战斗结果)
#define RS_LIST_MAX 5
typedef struct {
  int num;
  int exp;
  int levelup;
} RS_LIST;

int BATTLE_GetDuelPoint(int battle_index, // 戦闘インデックス (战斗索引)
                        int side,        // サイド 0 1 (阵营 0 1)
                        int num // 戦闘の 何番 のキャラか (战斗中的第几个角色)
) {
  char szBuffer[1024] = "";
  int char_index;
  int dpnow, dpadd;
  int fd;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  char_index = BattleArray[battle_index].Side[side].Entry[num].char_index;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_PARAM;
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    return 0;
  }
  if (BattleArray[battle_index].Side[side].type != BATTLE_S_TYPE_PLAYER) {
    return 0;
  }
  dpadd = CHAR_getWorkInt(char_index, CHAR_WORKGETEXP);

#ifdef _VIP_POINT_PK
  int floor = CHAR_getInt(char_index, CHAR_FLOOR);
  if (floor == 11111 || floor == 22222 || floor == 33333 || floor == 44444) {
    int point = 0;
    int oldpoint = CHAR_getInt(char_index, CHAR_AMPOINT);
    if (floor == 44444) {
      point = getVipPointPK(3);
    } else if (floor == 33333) {
      point = getVipPointPK(2);
    } else if (floor == 22222) {
      point = getVipPointPK(1);
    } else if (floor == 11111) {
      point = getVipPointPK(0);
    }

    if (dpadd < 0) {
      CHAR_setInt(char_index, CHAR_AMPOINT, oldpoint - point);
#ifdef _AMPOINT_LOG
      LogAmPoint(CHAR_getChar(char_index, CHAR_NAME),
                 CHAR_getChar(char_index, CHAR_CDKEY), -point,
                 CHAR_getInt(char_index, CHAR_AMPOINT), "(乱舞格斗)",
                 CHAR_getInt(char_index, CHAR_FLOOR),
                 CHAR_getInt(char_index, CHAR_X),
                 CHAR_getInt(char_index, CHAR_Y));
#endif
      sprintf(szBuffer, "由于你战败失去积分点:%d\n", point);
      CHAR_talkToCli(char_index, -1, szBuffer, CHAR_COLORCYAN);

      if (CHAR_getInt(char_index, CHAR_AMPOINT) < point) {
        sprintf(szBuffer, "你的积分点不足%d，自动脱离团队！", point);
        CHAR_talkToCli(char_index, -1, szBuffer, CHAR_COLORYELLOW);
        CHAR_DischargePartyNoMsg(char_index);
      }

      int i;
      int num = 0;
      BATTLE_ENTRY *pWinEntry = BattleArray[battle_index].Side[1 - side].Entry;
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        int char_index = pWinEntry[i].char_index;

        if (CHAR_CHECKINDEX(char_index) == FALSE)
          continue;
        if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
          continue;
        num++;
      }

      int addpoint = point / num;
      addpoint -= addpoint * getVipPointPKCost();
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        int char_index = pWinEntry[i].char_index;

        if (CHAR_CHECKINDEX(char_index) == FALSE)
          continue;
        if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
          continue;

        int oldpoint = CHAR_getInt(char_index, CHAR_AMPOINT);
        CHAR_setInt(char_index, CHAR_AMPOINT, oldpoint + addpoint);
#ifdef _AMPOINT_LOG
        LogAmPoint(CHAR_getChar(char_index, CHAR_NAME),
                   CHAR_getChar(char_index, CHAR_CDKEY), addpoint,
                   CHAR_getInt(char_index, CHAR_AMPOINT), "(乱舞格斗)",
                   CHAR_getInt(char_index, CHAR_FLOOR),
                   CHAR_getInt(char_index, CHAR_X),
                   CHAR_getInt(char_index, CHAR_Y));
#endif
        sprintf(szBuffer, "恭喜你赢得积分点:%d\n", addpoint);
        CHAR_talkToCli(char_index, -1, szBuffer, CHAR_COLORCYAN);
      }
    }
  } else
#endif
#ifdef _NEW_AUTO_PK
      if (floor == 20000 && AutoPk_PKTimeGet() <= 0) {
    if (dpadd == 0) {
      CHAR_talkToCli(char_index, -1, "战绩不理想，未加减竟赛积分！", CHAR_COLORRED);
    } else if (dpadd < 0) {
      int fl = 0, x = 0, y = 0;
      if (CHAR_CHECKINDEX(char_index) != FALSE) {
        CHAR_getElderPosition(CHAR_getInt(char_index, CHAR_LASTTALKELDER), &fl,
                              &x, &y);
        CHAR_warpToSpecificPoint(char_index, fl, x, y);
#ifdef _FORMULATE_AUTO_PK
        //					  SaacClient_FormulateAutoPk_send(acfd,
        // CHAR_getChar(char_index, CHAR_CDKEY),
        // CHAR_getWorkInt(char_index,CHAR_WORK_AUTOPK));
        CHAR_setInt(char_index, CHAR_AMPOINT,
                    CHAR_getInt(char_index, CHAR_AMPOINT) +
                        CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK));
#ifdef _AMPOINT_LOG
        LogAmPoint(CHAR_getChar(char_index, CHAR_NAME),
                   CHAR_getChar(char_index, CHAR_CDKEY),
                   CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK),
                   CHAR_getInt(char_index, CHAR_AMPOINT), "(自定义PK)",
                   CHAR_getInt(char_index, CHAR_FLOOR),
                   CHAR_getInt(char_index, CHAR_X),
                   CHAR_getInt(char_index, CHAR_Y));
#endif
        CHAR_setInt(char_index, CHAR_WORK_AUTOPK, 0);
#endif
        CHAR_DischargeParty(char_index, 0);
        CHAR_talkToCli(char_index, -1, "你已战败，系统自动把你送回记录点",
                       CHAR_COLORYELLOW);

        int playernum = CHAR_getPlayerMaxNum();
        int num = 0;
        int lastindex = 0;
        int i;
        for (i = 0; i < playernum; i++) {
          if (CHAR_CHECKINDEX(i) == FALSE)
            continue;
          if (CHAR_getInt(i, CHAR_FLOOR) == 20000)
            if (CHAR_getWorkInt(i, CHAR_WORK_AUTOPK) != -1) {
              lastindex = i;
              num++;
            }
        }
        if (num == 1) {
          int fl = 0, x = 0, y = 0;
          CHAR_getElderPosition(CHAR_getInt(lastindex, CHAR_LASTTALKELDER), &fl,
                                &x, &y);
          CHAR_warpToSpecificPoint(lastindex, fl, x, y);
          AutoPk_ChampionShipSet(
              lastindex, CHAR_getWorkInt(lastindex, CHAR_WORK_AUTOPK), 1);
          AutoPk_ChampionShipSet(
              char_index, CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK), 2);
          AutoPk_GetChampionShip();
#ifdef _FORMULATE_AUTO_PK
          //					  SaacClient_FormulateAutoPk_send(acfd,
          // CHAR_getChar(char_index, CHAR_CDKEY),
          // CHAR_getWorkInt(char_index,CHAR_WORK_AUTOPK));
          CHAR_setInt(char_index, CHAR_AMPOINT,
                      CHAR_getInt(char_index, CHAR_AMPOINT) +
                          CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK));
#ifdef _AMPOINT_LOG
          LogAmPoint(CHAR_getChar(char_index, CHAR_NAME),
                     CHAR_getChar(char_index, CHAR_CDKEY),
                     CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK),
                     CHAR_getInt(char_index, CHAR_AMPOINT), "(自定义PK)",
                     CHAR_getInt(char_index, CHAR_FLOOR),
                     CHAR_getInt(char_index, CHAR_X),
                     CHAR_getInt(char_index, CHAR_Y));
#endif
          CHAR_setWorkInt(char_index, CHAR_WORK_AUTOPK, 0);
#endif
        }
      }
    } else {
#ifdef _FORMULATE_AUTO_PK
      CHAR_setWorkInt(char_index, CHAR_WORK_AUTOPK,
                      CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK) +
                          getKillPoint());
#else
      CHAR_setWorkInt(char_index, CHAR_WORK_AUTOPK,
                      CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK) + 1);
#endif
      AutoPk_ChampionShipSet(char_index,
                             CHAR_getWorkInt(char_index, CHAR_WORK_AUTOPK), 0);
    }
  } else
#endif
  {
    if (dpadd == 0) {
      dpadd = 0;
    } else if (dpadd < 0) {
      dpadd = min(-1, dpadd);
    } else {
      dpadd = max(1, dpadd);
    }
    dpnow = CHAR_getInt(char_index, CHAR_DUELPOINT) + dpadd;
    dpnow = max(dpnow, 0);

    CHAR_setInt(char_index, CHAR_DUELPOINT, min(dpnow, CHAR_MAXDUELPOINT));

    szBuffer[0] = 0;
    {
      char szAdd[64], szNow[64];
      cnv10to62(dpadd, szAdd, sizeof(szAdd));
      cnv10to62(dpnow, szNow, sizeof(szNow));
      snprintf(szBuffer, sizeof(szBuffer), "%s|%s|", szAdd, szNow);
    }

    fd = getfdFromchar_index(char_index);
    GmsvServer_RD_send(fd, szBuffer);

    CHAR_send_DpDBUpdate(char_index);
#ifndef _NET_REDUCESEND
    CHAR_send_DpDBUpdate_AddressBook(char_index, TRUE);
#endif
  }
  return 0;
}
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif
int BATTLE_GetExpGold(int battle_index,
                      int side,
                      int num
) {
  BATTLE_ENTRY *pEntryChara;
  char szBuffer[1024] = "", szItemString[512], szEscItemString[256];
  int char_index, UpLevel, petindex, i, j, item_index = -1;
  int rsCnt = 0;
  RS_LIST aRsList[RS_LIST_MAX];
  int itemgroup[CHAR_MAXITEMHAVE - CHAR_STARTITEMARRAY];
  int itemnum = 0;
  memset(aRsList, 0, sizeof(aRsList));
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  char_index = BattleArray[battle_index].Side[side].Entry[num].char_index;
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return BATTLE_ERR_PARAM;
  if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
    return 0;
  }
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
#ifdef _PLAYER_NPC
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
        CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC) {
      if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE) {
#ifdef _ITEM_ADDEQUIPEXP
        BATTLE_GetExp(char_index, char_index);
#else
        BATTLE_GetExp(char_index);
#endif
        int UpLevel = CHAR_LevelUpCheck(char_index, -1);
        int i;
        for (i = 0; i < UpLevel; i++) {
          CHAR_PetLevelUp(char_index);
          CHAR_PetAddVariableAi(char_index, AI_FIX_PETLEVELUP);
        }
        for (i = 0; i < CHAR_MAXPETHAVE; i++) {
          petindex = CHAR_getCharPet(char_index, i);
          if (CHAR_CHECKINDEX(petindex) == FALSE)
            continue;
          if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE)
            continue;
          if (CHAR_getWorkInt(petindex, CHAR_WORKGETEXP) <= 0) {
            CHAR_complianceParameter(petindex);
            sprintf(szBuffer, "K%d", i);
            CHAR_sendStatusString(char_index, szBuffer);
            continue;
          }
#ifdef _ITEM_ADDEQUIPEXP
          BATTLE_GetExp(petindex, char_index);
#else
          BATTLE_GetExp(petindex);
#endif
          UpLevel = CHAR_LevelUpCheck(petindex, char_index);
          if (UpLevel > 0) {
            for (j = 0; j < UpLevel; j++) {
              CHAR_PetLevelUp(petindex);
              CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
            }
            aRsList[rsCnt].levelup = 1;
          } else {
            aRsList[rsCnt].levelup = 0;
          }
          aRsList[rsCnt].num = i;
          aRsList[rsCnt].exp = CHAR_getWorkInt(petindex, CHAR_WORKGETEXP);
          rsCnt++;
          CHAR_complianceParameter(petindex);
          if (UpLevel > 0) {
            sprintf(szBuffer, "K%d", i);
            CHAR_sendStatusString(char_index, szBuffer);
          }
        }
      }
    }
#endif
    return 0;
  }
  pEntryChara = &BattleArray[battle_index].Side[side].Entry[num];
  if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE) {
    // 2026.09.07 将获得的写入 CHAR_WORKGETEXP
#ifdef _ITEM_ADDEQUIPEXP
    BATTLE_GetExp(char_index, char_index);
#else
    BATTLE_GetExp(char_index);
#endif
    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 得到 EXP %d",
               CHAR_getUseName(char_index),
               CHAR_getWorkInt(char_index, CHAR_WORKGETEXP));
      BATTLE_talkToCli(char_index, szBuffer, CHAR_COLORYELLOW);
    }
  }
  UpLevel = CHAR_LevelUpCheck(char_index, -1);
  if (UpLevel > 0) {
    aRsList[rsCnt].levelup = 1;
  } else {
    aRsList[rsCnt].levelup = 0;
  }
  aRsList[rsCnt].num = -2;
  aRsList[rsCnt].exp = CHAR_getWorkInt(char_index, CHAR_WORKGETEXP);
  rsCnt++;
  if (UpLevel > 0) {
    int work;

    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 升级至 %d",
               CHAR_getUseName(char_index), CHAR_getInt(char_index, CHAR_LV));
      BATTLE_talkToCli(char_index, szBuffer, CHAR_COLORYELLOW);
    }
#ifdef _SKILLUPPOINT_CF
    CHAR_setInt(char_index, CHAR_SKILLUPPOINT,
                CHAR_getInt(char_index, CHAR_SKILLUPPOINT) +
                    UpLevel * getSkup());
#else
    CHAR_setInt(char_index, CHAR_SKILLUPPOINT,
                CHAR_getInt(char_index, CHAR_SKILLUPPOINT) + UpLevel * 3);
#endif
    work = CHAR_getInt(char_index, CHAR_CHARM);
    work += CH_FIX_PLAYERLEVELUP;
    CHAR_setInt(char_index, CHAR_CHARM, min(100, work));
    CHAR_complianceParameter(char_index);
    CHAR_send_P_StatusString(char_index, CHAR_P_STRING_LV |
                                             CHAR_P_STRING_NEXTEXP |
                                             CHAR_P_STRING_DUELPOINT);
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
    CHAR_PartyUpdate(char_index, CHAR_N_STRING_LV);
#ifndef _NET_REDUCESEND
    CHAR_send_DpDBUpdate_AddressBook(char_index, TRUE);
#endif
  }
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(char_index, i);
    if (CHAR_CHECKINDEX(petindex) == FALSE)
      continue;
    if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE)
      continue;
    if (CHAR_getWorkInt(petindex, CHAR_WORKGETEXP) <= 0) {
      CHAR_complianceParameter(petindex);
      sprintf(szBuffer, "K%d", i);
      CHAR_sendStatusString(char_index, szBuffer);
      continue;
    }
#ifdef _ITEM_ADDEQUIPEXP
    BATTLE_GetExp(petindex, char_index);
#else
    BATTLE_GetExp(petindex);
#endif
    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 得到 EXP %d",
               CHAR_getUseName(petindex),
               CHAR_getWorkInt(petindex, CHAR_WORKGETEXP));
      BATTLE_talkToCli(char_index, szBuffer, CHAR_COLORYELLOW);
    }
    UpLevel = CHAR_LevelUpCheck(petindex, char_index);
    if (UpLevel > 0) {
      if (getBattleDebugMsg() != 0) {
        snprintf(szBuffer, sizeof(szBuffer), "(%s) 升级至 %d",
                 CHAR_getUseName(petindex), CHAR_getInt(petindex, CHAR_LV));
        BATTLE_talkToCli(char_index, szBuffer, CHAR_COLORYELLOW);
      }
      for (j = 0; j < UpLevel; j++) {
        CHAR_PetLevelUp(petindex);
        CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
      }
      aRsList[rsCnt].levelup = 1;
    } else {
      aRsList[rsCnt].levelup = 0;
    }
    aRsList[rsCnt].num = i;
    aRsList[rsCnt].exp = CHAR_getWorkInt(petindex, CHAR_WORKGETEXP);
    rsCnt++;
    CHAR_complianceParameter(petindex);
    if (UpLevel > 0) {
      sprintf(szBuffer, "K%d", i);
      CHAR_sendStatusString(char_index, szBuffer);
    }
  }
  szItemString[0] = 0;
  if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE
#ifdef _OFFLINE_SYSTEM
      && CHAR_getWorkInt(char_index, CHAR_WORK_OFFLINE) == 0
#endif
  ) {
    for (i = 0; i < GETITEM_MAX; i++) {
      int ItemGet = FALSE;
      szEscItemString[0] = 0;
      item_index = pEntryChara->getitem[i];

#ifdef _BATTLE_GETITEM_RATE
      if (item_index < 0) {
        int floor = CHAR_getInt(char_index, CHAR_FLOOR);
        if (floor == getBattleGetItemRateMap()) {
          int rate = rand() % 10000;
          if (rate < 2) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(0));
          } else if (rate < 5) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(1));
          } else if (rate < 10) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(2));
          } else if (rate < 18) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(3));
          } else if (rate < 28) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(4));
          } else if (rate < 40) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(5));
          } else if (rate < 65) {
            item_index = ITEM_makeItemAndRegist(getBattleGetItemRate(6));
          }
        }
        int i;
        int playernum = CHAR_getPlayerMaxNum();
        int num = 0;
        for (i = 0; i < playernum; i++) {
          if (CHAR_CHECKINDEX(i) == FALSE)
            continue;
          if (CHAR_getInt(i, CHAR_FLOOR) == getBattleGetItemRateMap()) {
            num++;
          }
        }
        int rate = rand() % playernum < num * 5;
        if (rate) {
          item_index = -1;
        }
      }
#endif
      if (item_index < 0) {
        item_index = -1;
      } else if (CHAR_findEmptyItemBox(char_index) >= 0) {
        int ret = CHAR_addItemSpecificItemIndex(char_index, item_index);
        if (0 <= ret && ret < CHAR_EQUIPPLACENUM) {
          print("itemget err: 在这里不能取得item(%s)(%d)(%s)\n",
                CHAR_getUseName(char_index), ret,
                ITEM_getAppropriateName(item_index));
        }
        if (!CHAR_CHECKITEMINDEX(char_index, ret)) {
          ItemGet = FALSE;
        } else {
          ItemGet = TRUE;
          itemgroup[itemnum] = ret;
          itemnum++;
        }
      }
      if (ItemGet == TRUE) {
        if (getBattleDebugMsg() != 0) {
          snprintf(szBuffer, sizeof(szBuffer), "拾获(%s)",
                   ITEM_getAppropriateName(item_index));
          BATTLE_talkToCli(char_index, szBuffer, CHAR_COLORYELLOW);
        }

        LogItem(CHAR_getChar(char_index, CHAR_NAME),
                CHAR_getChar(char_index, CHAR_CDKEY),
#ifdef _add_item_log_name // WON ADD 在item的log中增加item名称
                item_index,
#else
                ITEM_getInt(item_index, ITEM_ID), /* アイテム番号 (物品编号) */
#endif
                "BattleGet(战斗後所得的道具)", CHAR_getInt(char_index, CHAR_FLOOR),
                CHAR_getInt(char_index, CHAR_X),
                CHAR_getInt(char_index, CHAR_Y),
                ITEM_getChar(item_index, ITEM_UNIQUECODE), // shan 2001/12/14
                ITEM_getChar(item_index, ITEM_NAME),
                ITEM_getInt(item_index, ITEM_ID));
        makeEscapeString(ITEM_getAppropriateName(item_index), szEscItemString,
                         sizeof(szEscItemString));
        strncat(szItemString, szEscItemString, sizeof(szItemString));
        pEntryChara->getitem[i] = -1;
      } else {
        if (item_index >= 0) {
          ITEM_endExistItemsOne(item_index);
        } else {
        }
        pEntryChara->getitem[i] = -1;
      }
      strncat(szItemString, "|", sizeof(szItemString));
    }
  }

  szBuffer[0] = 0;
  for (i = 0; i < RS_LIST_MAX; i++) {
    char szWork[256], sz62[64];
    szWork[0] = 0;
    if (aRsList[i].exp > 0 || i == 0) {
      cnv10to62(aRsList[i].exp, sz62, sizeof(sz62));
      snprintf(szWork, sizeof(szWork), "%d|%d|%s", aRsList[i].num,
               aRsList[i].levelup, sz62);
    }
    strncat(szBuffer, szWork, sizeof(szBuffer));
    strncat(szBuffer, ",", sizeof(szBuffer));
  }
  strncat(szBuffer, szItemString, sizeof(szBuffer));
  {
    int fd;
    fd = getfdFromchar_index(char_index);
    GmsvServer_RS_send(fd, szBuffer);
  }
  if (itemnum > 0) {
    int inum, ie;
    for (inum = 0; inum < itemnum; inum++) {
      ie = CHAR_getItemIndex(char_index, itemgroup[inum]);
    }
    CHAR_sendItemData(char_index, itemgroup, itemnum);
  }
  return 0;
}

int BATTLE_GetProfit(int battle_index, int side, int num) {
  if (BattleArray[battle_index].dpbattle == 1) {
    return BATTLE_GetDuelPoint(battle_index, side, num);
  } else {
    return BATTLE_GetExpGold(battle_index, side, num); // 包括金钱和经验
  }
}

int BATTLE_FinishSet(int battle_index) {

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  BattleArray[battle_index].mode = BATTLE_MODE_FINISH;
  return 0;
}


// 2026.09.07 Battle Finish 各种结算
static int BATTLE_Finish(int battle_index) {
  BATTLE *pBattle;
  BATTLE_ENTRY *pEntry;
  int i, char_index, j;
  char watch_detail[32];
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  if (BattleArray[battle_index].winside == -1 &&
      BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E &&
      BattleArray[battle_index].WinFunc != NULL) {
    NETWATCH_set("BATTLE_Finish.WinFunc", battle_index, NULL);
    BattleArray[battle_index].WinFunc(battle_index,
                                     BattleArray[battle_index].createindex);
  }
#ifdef _BATTLE_PK
  if (CHAR_CHECKINDEX(BattleArray[battle_index].rivalindex) &&
      CHAR_CHECKINDEX(BattleArray[battle_index].leaderindex)) {
    if (CHAR_getWorkInt(BattleArray[battle_index].rivalindex,
                        CHAR_WORK_BATTLEPK) == TRUE ||
        CHAR_getWorkInt(BattleArray[battle_index].leaderindex,
                        CHAR_WORK_BATTLEPK) == TRUE) {
      int winindex, lostindex;
      if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
        if (BattleArray[battle_index].winside == -1) {
          winindex = BattleArray[battle_index].leaderindex;
          lostindex = BattleArray[battle_index].rivalindex;
        } else {
          winindex = BattleArray[battle_index].rivalindex;
          lostindex = BattleArray[battle_index].leaderindex;
        }

        int fl = 0, x = 0, y = 0;
        CHAR_getElderPosition(CHAR_getInt(lostindex, CHAR_LASTTALKELDER), &fl,
                              &x, &y);
        NETWATCH_set("BATTLE_Finish.PK_warp", lostindex, NULL);
        CHAR_warpToSpecificPoint(lostindex, fl, x, y);
        NETWATCH_set("BATTLE_Finish.PK_party", lostindex, NULL);
        CHAR_DischargeParty(lostindex, 0);
        CHAR_setWorkInt(winindex, CHAR_WORK_BATTLEPK, FALSE);
        CHAR_setWorkInt(lostindex, CHAR_WORK_BATTLEPK, FALSE);
      }
    }
  }
#endif

#ifdef _TRADE_PK
  if (CHAR_CHECKINDEX(BattleArray[battle_index].TradeList[0].char_index) &&
      CHAR_CHECKINDEX(BattleArray[battle_index].TradeList[1].char_index)) {
    if (CHAR_getInt(BattleArray[battle_index].TradeList[0].char_index,
                    CHAR_FLOOR) == 50000 &&
        CHAR_getInt(BattleArray[battle_index].TradeList[1].char_index,
                    CHAR_FLOOR) == 50000) {
      if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
        if (BattleArray[battle_index].winside == -1) {
          NETWATCH_set("BATTLE_Finish.TradePK", battle_index, NULL);
          TRADE_HandleTradeForPK(&BattleArray[battle_index].TradeList[0],
                                 &BattleArray[battle_index].TradeList[1]);
        } else if (BattleArray[battle_index].winside == 1) {
          NETWATCH_set("BATTLE_Finish.TradePK", battle_index, NULL);
          TRADE_HandleTradeForPK(&BattleArray[battle_index].TradeList[1],
                                 &BattleArray[battle_index].TradeList[0]);
        }
      }
    }
  }
#endif

  if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
    BATTLE_ENTRY *pWinEntry, *pLostEntry;
    char token[256];
    if (BattleArray[battle_index].winside == -1) {
      pWinEntry = BattleArray[battle_index].Side[0].Entry;
      pLostEntry = BattleArray[battle_index].Side[1].Entry;
    } else {
      pWinEntry = BattleArray[battle_index].Side[1].Entry;
      pLostEntry = BattleArray[battle_index].Side[0].Entry;
    }

    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      int winindex = pWinEntry[i].char_index;
      if (CHAR_CHECKINDEX(winindex) == TRUE) {

        CHAR_setInt(winindex, CHAR_DUELSTWINCOUNT,
                    CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT) + 1);
        CHAR_setInt(winindex, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(winindex, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(winindex, CHAR_DUELWINCOUNT,
                    CHAR_getInt(winindex, CHAR_DUELWINCOUNT) + 1);

        CHAR_setInt(winindex, CHAR_DUELSTLOSECOUNT, 0);
        if (CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT) >
            CHAR_getInt(winindex, CHAR_DUELMAXSTWINCOUNT)) {
          CHAR_setInt(winindex, CHAR_DUELMAXSTWINCOUNT,
                      CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT));
        }
      }

      int lostindex = pLostEntry[i].char_index;
      if (CHAR_CHECKINDEX(lostindex) == TRUE) {

        CHAR_setInt(lostindex, CHAR_DUELSTLOSECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELSTLOSECOUNT) + 1);

        CHAR_setInt(lostindex, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(lostindex, CHAR_DUELLOSECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELLOSECOUNT) + 1);

        CHAR_setInt(lostindex, CHAR_DUELSTWINCOUNT, 0);
      }
    }
  }
  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battle_index].Side[j].Entry;
#ifdef _PET_TALK
    for (i = 0; i < BATTLE_ENTRY_MAX; i++)
#else
    for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--)
#endif
    {
      char_index = pEntry[i].char_index;

      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;

      snprintf(watch_detail, sizeof(watch_detail), "battle=%d,side=%d,slot=%d",
               battle_index, j, i);
      NETWATCH_set("BATTLE_Finish.GetProfit", char_index, watch_detail);
      BATTLE_GetProfit(battle_index, j, i); // 包括取得经验值、金钱、物品
      NETWATCH_set("BATTLE_Finish.Exit", char_index, watch_detail);
      BATTLE_Exit(char_index, battle_index);
    }
  }
  if (BattleArray[battle_index].type == BATTLE_TYPE_WATCH) {
  } else {
    pBattle = BattleArray[battle_index].pNext;
    for (; pBattle; pBattle = pBattle->pNext) {
      NETWATCH_set("BATTLE_Finish.WatchExit", pBattle->battle_index, NULL);
      if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
        printEx("err:battle address 奇怪(%p)\n", pBattle);
        break;
      }
      for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--) {
        char_index = pBattle->Side[0].Entry[i].char_index;
        if (CHAR_CHECKINDEX(char_index) == FALSE)
          continue;
        snprintf(watch_detail, sizeof(watch_detail),
                 "battle=%d,watch=%d,slot=%d", battle_index,
                 pBattle->battle_index, i);
        NETWATCH_set("BATTLE_Finish.WatcherExit", char_index, watch_detail);
        BATTLE_Exit(char_index, pBattle->battle_index);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
      }
    }
    pBattle = BattleArray[battle_index].pNext;
    for (; pBattle; pBattle = pBattle->pNext) {
      NETWATCH_set("BATTLE_Finish.DeleteWatch", pBattle->battle_index, NULL);
      if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
        printEx("err:battle address 奇怪(%p)\n", pBattle);
        break;
      }
      BATTLE_DeleteBattle(pBattle->battle_index);
    }
  }
  NETWATCH_set("BATTLE_Finish.DeleteMain", battle_index, NULL);
  BATTLE_DeleteBattle(battle_index);
  return 0;
}

int BATTLE_StopSet(int battle_index) {

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  BattleArray[battle_index].mode = BATTLE_MODE_STOP;

  return 0;
}

static int BATTLE_Stop(int battle_index) {
  BATTLE_ENTRY *pEntry;
  int i, char_index, j;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battle_index].Side[j].Entry;
    for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--) {
      char_index = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      BATTLE_GetProfit(battle_index, j, i);
      BATTLE_Exit(char_index, battle_index);
    }
  }

  BATTLE_DeleteBattle(battle_index);
  return 0;
}

int BATTLE_DefaultAttacker(int battle_index, int side) {
  int i, rnd, cnt;
  int CharaTbl[BATTLE_ENTRY_MAX];

  BATTLE_ENTRY *pEntry;

  pEntry = BattleArray[battle_index].Side[side].Entry;
  cnt = 0;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    CharaTbl[i] = -1;
    if (CHAR_CHECKINDEX(pEntry[i].char_index) == FALSE) {
      continue;
    }

    if (CHAR_getWorkInt(pEntry[i].char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_RESCUE) {
      continue;
    }

    if (BATTLE_TargetCheck(battle_index, i + side * SIDE_OFFSET) == FALSE)
      continue;

    CharaTbl[cnt] = i + side * SIDE_OFFSET;
    cnt++;
  }

  if (cnt == 0) {
    return -1;
  }

  rnd = RAND(0, cnt - 1);
  return CharaTbl[rnd];
}

//*********************************************************
//
// 敵キャラに戦闘コマンドを 与える (给敌方角色战斗指令)
//
int BATTLE_AddDpAlive(int battle_index, // バトルインデックス (战斗索引)
                      int side, // サイド( 0 or 1 ) (阵营 0 或 1)
                      int dp)
//
//   戻り値 生存 しているプレイヤーの数 (返回值：存活玩家数)
// 戦闘 が 終わっていない 場合の値 (未结束时返回的值)
//
//*********************************************************
{
  int i, char_index;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // パラメータチェック (参数检查)
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // エントリ (入场)
  pEntry = BattleArray[battle_index].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    // ペットはこの 対象外 (宠物不计入)
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 生きている 分 加算 (存活者累加)
    if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE) {
      CHAR_setWorkInt(char_index, CHAR_WORKGETEXP,
                      CHAR_getWorkInt(char_index, CHAR_WORKGETEXP) + dp);
    }
  }

  return cnt;
}

#if 1
//*********************************************************
//
// 生存プレイヤーがいるか 調べる (统计存活玩家)
//
int BATTLE_CountAlive(int battle_index, // バトルインデックス (战斗索引)
                      int side         // サイド( 0 or 1 ) (阵营 0 或 1)
                      )
//
//   戻り値 生存 しているプレイヤーの数 (返回值：存活玩家数)
// 戦闘 が 終わっていない 場合の値 (未结束时返回的值)
//
//*********************************************************
{
  int i, char_index;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // パラメータチェック (参数检查)
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // エントリ (入场)
  pEntry = BattleArray[battle_index].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    // ペットはこの 対象外 (宠物不计入)
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 生きている (存活)
    if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE) {
      cnt++;
    }
  }

  return cnt;
}
#endif

//*********************************************************
// 生存プレイヤーが救援のみか をチェック (检查是否只剩救援状态)
// 戻り値 生存 しているプレイヤーの数 (返回值：存活玩家数)
// 戦闘 が 終わっていない 場合の値 (未结束时返回的值)
//*********************************************************
int BATTLE_OnlyRescue(int battle_index, // バトルインデックス (战斗索引)
                      int side, // サイド( 0 or 1 ) (阵营 0 或 1)
                      int *pOnlyFlg)
{
  int i, char_index;
  BATTLE_ENTRY *pEntry;
  int cnt = 0, OnlyRescue = 1;
  (*pOnlyFlg) = 0;
  // パラメータチェック (参数检查)
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // エントリ (入场)
  pEntry = BattleArray[battle_index].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pEntry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE) {
      continue;
    }
    // ペットはこの 対象外 (宠物不计入)
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 生きている (存活)
    if (CHAR_getFlg(char_index, CHAR_ISDIE) == FALSE) {
      cnt++;
      // 生きている 者は救援か (存活者是否处于救援状态)
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
      } else {
        OnlyRescue = 0;
      }
    }
#ifdef _PETSKILL_LER
    if (CHAR_getWorkInt(char_index, CHAR_WORK_RELIFE) > 0)
      cnt++;
#endif
  }

  // 生きている 者が 全員救援 だったら (存活者全部在救援状态时)
  if (cnt > 0 && OnlyRescue) {
    // フラグを 立てる (置标志)
    (*pOnlyFlg) = 1;
  } else {
    // 違ったら0にする (否则置0)
    (*pOnlyFlg) = 0;
  }

  return cnt;
}

static BOOL BATTLE_TimeOutCheck(int battle_index) {
  int i, j, char_index;
  BATTLE *pBattle;
  BATTLE_ENTRY *pEntry;
  pBattle = &BattleArray[battle_index];

  if (NowTime.tv_sec > pBattle->timer + BATTLE_TIME_LIMIT) {
  } else {
    return FALSE;
  }
  // BATTLE_BroadCast( battle_index, "server时间已到。", CHAR_COLORYELLOW );
  for (j = 0; j < 2; j++) {
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
        continue;
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_C_WAIT) {
        BATTLE_Exit(char_index, battle_index);
        CHAR_DischargePartyNoMsg(char_index);
        CHAR_talkToCli(char_index, -1, "时间到，结束战斗。", CHAR_COLORYELLOW);
        BATTLE_CommandSend(char_index, "BU");
      }
    }
  }

  return TRUE;
}

int BATTLE_WatchWait(int battle_index) {
  BATTLE *pBattle;
  BOOL commandflg = TRUE;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battle_index];
  if (BATTLE_TimeOutCheck(battle_index) == TRUE) {
    commandflg = TRUE;
  }
  if (commandflg == FALSE) {
    return 0;
  }
  pBattle->mode = BATTLE_MODE_WATCHMOVIE;
  pBattle->turn++;
  return 0;
}

int BATTLE_WatchMovie(int battle_index) { return 0; }

int BATTLE_WatchAfter(int battle_index) {
  BattleArray[battle_index].mode = BATTLE_MODE_WATCHPRE;
  return 0;
}

int BATTLE_WatchBC(int battle_index) { return 0; }

int BATTLE_WatchPre(int battle_index) {
  // コマンド処理の時の一連の 流れ (指令处理流程)
  BATTLE_PreWatchWaitSeq(battle_index);
  BattleArray[battle_index].mode = BATTLE_MODE_WATCHWAIT;
  return 0;
}

static int BATTLE_Command(int battle_index) {
  BATTLE *pBattle, *pWatchBattle;
  BOOL commandflg = TRUE, iFinish = FALSE;
  int OnlyRescue[2], i, j, char_index;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battle_index];
  if (BATTLE_CommandWait(battle_index, 0) == FALSE) {
    commandflg = FALSE;
  }
  if (BATTLE_CommandWait(battle_index, 1) == FALSE) {
    commandflg = FALSE;
  }

  pWatchBattle = pBattle->pNext;
  for (; pWatchBattle; pWatchBattle = pWatchBattle->pNext) {
    if (BATTLE_CHECKADDRESS(pWatchBattle) == FALSE) {
      printEx("err:观战battle address错误(%p)\n", pWatchBattle);
      break;
    }

    if (pWatchBattle->mode == BATTLE_MODE_WATCHBC) {
      BATTLE_MakeCharaString(battle_index, szAllBattleString,
                             sizeof(szAllBattleString));
      BATTLE_BpSendToWatch(pWatchBattle, szAllBattleString);
      pWatchBattle->mode = BATTLE_MODE_WATCHPRE;
      commandflg = FALSE;
    } else if (pWatchBattle->mode != BATTLE_MODE_WATCHMOVIE) {
      commandflg = FALSE;
    }
  }
  if (BATTLE_TimeOutCheck(battle_index) == TRUE) {
    commandflg = TRUE;
  }
  if (commandflg == FALSE) {
    return 0;
  }

  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = pBattle->Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == TRUE) {
        if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {

#ifdef _BATTLE_BOUT_TIME
          if (CHAR_getInt(char_index, CHAR_FLOOR) == 40001 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40002 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40003 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40004 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40005 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40006 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40007 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40008 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40009 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 40010 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 50001 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 50002 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 50003 ||
              CHAR_getInt(char_index, CHAR_FLOOR) == 50004) {
            CHAR_setWorkInt(char_index, CHAR_BATTLE_BOUT_TIME, (int)time(NULL));
          } else {
            CHAR_setWorkInt(char_index, CHAR_BATTLE_BOUT_TIME, 0);
          }
#endif
        }
      }
    }
  }

#ifdef _BATTLECOMMAND_TIME
  BattleArray[battle_index].PartTime = 0;
#endif
  pBattle->turn++; // 自己回合数增加
  BATTLE_ai_all(battle_index, 0, 0);
  BATTLE_ai_all(battle_index, 1, 0);
  BATTLE_Battling(battle_index); // 处理战斗
  pBattle->Side[0].flg &= ~BSIDE_FLG_SURPRISE;
  pBattle->Side[1].flg &= ~BSIDE_FLG_SURPRISE;
  if (BATTLE_OnlyRescue(battle_index, 0, &OnlyRescue[0]) == 0) {
    pBattle->winside = 1;
    iFinish = TRUE;
  } else if (BATTLE_OnlyRescue(battle_index, 1, &OnlyRescue[1]) == 0) {
    pBattle->winside = -1;
    iFinish = TRUE;
  }
  for (j = 0; j < 2; j++) {
    if (OnlyRescue[j] != 1)
      continue;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      char_index = pBattle->Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;

      if (CHAR_getInt(char_index, CHAR_HP) <= 0) {
        BATTLE_Exit(char_index, battle_index);
      }
    }
  }
  BATTLE_PreCommandSeq(battle_index);
  if (iFinish == TRUE) {
    BATTLE_FinishSet(battle_index);
  }
  return 0;
}

extern void BATTLE_changeRideImage(int index);

int BATTLE_Loop(void) {
  int i, cnt = 0;
  rand();
  for (i = 0; i < BATTLE_battlenum; i++) { // 最多战斗场数
    // if( BattleArray[i].use == FALSE )continue;
    if (!BATTLE_CHECKINDEX(i))
      continue;
#ifdef _BATTLE_TIME
    struct timeval old;
    old.tv_sec = BattleArray[i].tv_sec;
    old.tv_usec = BattleArray[i].tv_usec;
    if (getBattleTime() > 0 &&
        time_diff_us(NowTime, old) < getBattleTime() * 1000.0)
      continue;
    BattleArray[i].tv_sec = NowTime.tv_sec;
    BattleArray[i].tv_usec = NowTime.tv_usec;
#endif
    if (BattleArray[i].type == BATTLE_TYPE_WATCH) { // 若是观战模式
      NETWATCH_set("BATTLE_CountAlive", i, NULL);
      if (BATTLE_CountAlive(i, 0) == 0) {
        NETWATCH_set("BATTLE_FinishSet", i, NULL);
        BATTLE_FinishSet(i);
      }
    }

    switch (BattleArray[i].mode) {
    case BATTLE_MODE_NONE: // 未战斗
      break;
    case BATTLE_MODE_INIT: // 战斗初始化
      NETWATCH_set("BATTLE_Init", i, NULL);
      BATTLE_Init(i);
      break;
    case BATTLE_MODE_BATTLE: // Server内部战斗中
      NETWATCH_set("BATTLE_Command", i, NULL);
      BATTLE_Command(i);
      break;
    case BATTLE_MODE_FINISH: // 战斗结束
      NETWATCH_set("BATTLE_Finish", i, NULL);
      BATTLE_Finish(i);
      break;
    case BATTLE_MODE_STOP: // 战斗中断
      NETWATCH_set("BATTLE_Stop", i, NULL);
      BATTLE_Stop(i);
      break;
    case BATTLE_MODE_WATCHBC: // 观战中...
      NETWATCH_set("BATTLE_WatchBC", i, NULL);
      BATTLE_WatchBC(i);
      break;
    case BATTLE_MODE_WATCHPRE:
      NETWATCH_set("BATTLE_WatchPre", i, NULL);
      BATTLE_WatchPre(i);
      break;
    case BATTLE_MODE_WATCHWAIT:
      NETWATCH_set("BATTLE_WatchWait", i, NULL);
      BATTLE_WatchWait(i);
      break;
    case BATTLE_MODE_WATCHMOVIE:
      NETWATCH_set("BATTLE_WatchMovie", i, NULL);
      BATTLE_WatchMovie(i);
      break;
    case BATTLE_MODE_WATCHAFTER:
      NETWATCH_set("BATTLE_WatchAfter", i, NULL);
      BATTLE_WatchAfter(i);
      break;
    }
    cnt++;
  }

  return cnt;
}

typedef struct {
  int char_index; // キャラクタインデックス (角色索引)
  int side;       // サイド (阵营)
  int dex;        // 素早さ (敏捷, 每轮行动的敏捷并不相同)
  int num;        // エントリ番号 (战场编号, 即战场上的位置)
  int combo;      // どの技をするか (合击编号：如果参与合击，则参与哪个合击)
#ifdef _EQUIT_SEQUENCE
  int sequence;   // 装备和人物属性上提供的顺序加成
#endif
} RoundActionListEntry;

//************************************************************
// 戦闘の素早さで 比較 (按敏捷比较)
// 素早さは値が大きい順である (按敏捷从高到低)
// ***********************************************************
static int EsCmp(const void *pc1, const void *pc2) {
#ifdef _EQUIT_SEQUENCE
  RoundActionListEntry *pC1 = pc1, *pC2 = pc2;
  return ((pC2->dex + pC2->sequence) - (pC1->dex + pC1->sequence));
#else
  return (pC2->dex - pC1->dex);
#endif
}
static void RoundActionSort(RoundActionListEntry *sRoundActionList, int listSize) {
  qsort(sRoundActionList, listSize, sizeof(RoundActionListEntry), EsCmp);
}

#ifdef _PETSKILL_NEW_PASSIVE
void BATTLE_PassiveSkill(int char_index) {
  int i, pskill_array, skill_type;
  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {

    if (CHAR_getCharHaveSkill(char_index, i) == NULL) {
      continue;
    }
    pskill_array = PETSKILL_GetArray(char_index, i);
    skill_type = PETSKILL_getInt(pskill_array, PETSKILL_FIELD);
    if (skill_type == PETSKILL_FIELD_MAP) {
      PETSKILL_Use(char_index, i, NULL, NULL);
    } else {
      continue;
    }
  }
}
#endif
//************************************************************
// 素早さを計算する (战斗中的敏捷值不但和角色基础敏捷有关，还会增加浮动)
// 戻り値 素早さ (返回值：战斗中的敏捷值)
//************************************************************
static int BATTLE_DexCalc(int char_index)
{
  int work, dex = 0;
  int pet_index = BATTLE_getRidePet(char_index);
#ifdef _PETSKILL_NEW_PASSIVE
  // 清除上轮被动加成
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_DUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_ACURATE, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_CRITICAL, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_COUNTER, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKPASSIVE_MULTIPLE, 0);
  // 重新计算被动加成
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    BATTLE_PassiveSkill(char_index);
  }
#endif
  // コマンド取得 (获取指令)
  int command = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);

#ifdef _PETSKILL_BECOMEFOX // 攻击顺序中的敏捷降下20%
  if (CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1) {
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work * 0.8; // 敏降20%
  }
#endif
#ifdef _PROFESSION_ADDSKILL
  if (CHAR_getWorkInt(char_index, CHAR_WORKFEAR) > 0) {
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work * 0.8; // 敏降20%
  }
#endif
  // 指令の 場合 (按战斗指令，敏捷值加成会有不同)
  switch (command) {
#ifdef _PROFESSION_SKILL         // WON ADD 人物职业技能
  case BATTLE_COM_S_BLOOD:       // 嗜血成性
  case BATTLE_COM_S_BLOOD_WORMS: // 嗜血蛊
  case BATTLE_COM_S_SIGN:        // 一针见血
#endif

#ifdef _BATTLE_NEWPOWER
  case BATTLE_COM_JYUJYUTU:
  case BATTLE_COM_S_ATTACK_MAGIC:
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, 15);
    break;
#endif

#ifdef _PROFESSION_SKILL             // WON ADD 人物职业技能
  case BATTLE_COM_S_FIRE_ENCLOSE:    // 火附体
  case BATTLE_COM_S_ICE_ENCLOSE:     // 冰附体
  case BATTLE_COM_S_THUNDER_ENCLOSE: // 雷附体
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(work * 0.3, work * 0.5);
    break;
  case BATTLE_COM_S_VOLCANO_SPRINGS: // 火山泉
  case BATTLE_COM_S_SUMMON_THUNDER:  // 召雷术
  case BATTLE_COM_S_ICE_ARROW:       // 冰箭术
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, work * 0.2);
    break;
  case BATTLE_COM_S_CURRENT:   // 电流术
  case BATTLE_COM_S_FIRE_BALL: // 火星球
  case BATTLE_COM_S_ICE_CRACK: // 冰爆术
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, work * 0.5);
    break;
  case BATTLE_COM_S_FIRE_SPEAR: // 火龙枪
  case BATTLE_COM_S_STORM:      // 暴风雨
  case BATTLE_COM_S_ICE_MIRROR: // 冰镜术
  case BATTLE_COM_S_ENCLOSE:    // 附身术
  case BATTLE_COM_S_TRANSPOSE:  // 移形换位
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(work * 0.2, work * 0.5);
    break;

  case BATTLE_COM_S_DOOM: // 世界末日
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0.3, work * 0.6);
    break;
#endif

#ifdef _SKILL_SPEEDY_ATT // vincent  宠技:疾速攻击
  case BATTLE_COM_S_SPEEDYATTACK:
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work + work * 0.3; // 敏加30%
    break;
#endif
#ifdef _PETSKILL_DAMAGETOHP // 宠技:暗月狂狼(嗜血技的变体)
  case BATTLE_COM_S_DAMAGETOHP2:
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work + work * 0.2; // 敏加20%
    break;
#endif
  case BATTLE_COM_ITEM: // アイテムを使う場合 (使用道具时)
    work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, work * 0.1) + work * 0.15;
    break;
  default: // その他 戦闘など) (其他战斗指令)
    // Robin 0727 ride pet
    if (pet_index == -1)
      work = CHAR_getWorkInt(char_index, CHAR_WORKQUICK) + 20;
    else
      work = BATTLE_adjustRidePet3A(char_index, pet_index, CHAR_WORKQUICK,
                                    ATTACKSIDE) +
             20;
    dex = work - RAND(0, work * 0.1);
    break;
  }
  // if( dex <= 1 )dex = 1;
  return dex;
}
  
inline BOOL combo_condition(int command,
                            int arm_type,
                            int can_move) {
  return (command == BATTLE_COM_ATTACK
           && arm_type != 1 // 投掷武器
           && can_move == 1) ? 1 : 0;
}
//*************************************************************
// コンビネーションがあるかどうかチェック (预设值本轮行动可能的合击)
//*************************************************************
static void ComboSet(RoundActionListEntry *pRoundActionList, int actionNum)
{

  int i, char_index;
  int prev_side = -3;  // デフにありえない値 (不可能的初始值)
  int prev_target = -3; // デフにありえない値 (不可能的初始值)
  int start = -1;
  int combo_id = 1; // コンボID (合击编号)
  int combo_rate; // 野怪和玩家角色的合击率不一致
  int arm_type; // 武器的类型，弓/石头/回旋镖 不能参与合击
  int can_move; // 角色是否可以行动（如位于混乱、石化、睡眠状态）
  int command, target, side;
  for (i = 0; i < actionNum; i++) {
    char_index = pRoundActionList[i].char_index;
    command = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);
    target = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
    side = CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE);
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
      combo_rate = 20; // 敵は20 (野怪20%)
    } else {
      combo_rate = 80; // 戦闘は50 (玩家或者玩家宠物50%)
    } // 2026.09.09 合击概率由 50% -> 80%
    // HPが0か 動けなかったら (HP为0或不能移动)
    can_move = (CHAR_getInt(char_index, CHAR_HP) <= 0 ||
        BATTLE_CanMoveCheck(char_index) == FALSE) ? 0 : 1;
    arm_type = BATTLE_IsThrowWepon(CHAR_getItemIndex(char_index, CHAR_ARM));

    pRoundActionList[i].combo = 0; // 初期化 (初始化)

    if (start != -1) { // 連続技の 途中 (合击组合中已有一名角色)
      if (!combo_condition(command, arm_type, can_move)
        || target != prev_target || side != prev_side) {
        start = -1;     // リセット (合击组合统计中断)
        prev_side = side; // サイド記える (记录阵营)
      } else {
        // コンボID保存 (设置当前角色的状态合击)
        CHAR_setWorkInt(pRoundActionList[i].char_index, CHAR_WORKBATTLECOM1,
                        BATTLE_COM_COMBO);
        pRoundActionList[i].combo = combo_id;
        // 前の 値も 保存 (再设置一次合击发起人的状态)
        CHAR_setWorkInt(pRoundActionList[start].char_index, CHAR_WORKBATTLECOM1,
                        BATTLE_COM_COMBO);
        pRoundActionList[start].combo = combo_id;
      }
    } else { // 連続技を していない場合 (合击组合尚无一名角色)
      if (combo_condition(command, arm_type, can_move)
          && RAND(1, 100) <= combo_rate) { // 連続技 である (合击概率判定)
        // 只有合击发起人会判断这个
        start = i; // 当前角色满足合击判断
        prev_target = target; // 攻撃してる敵を記える (记录攻击目标)
        prev_side = side;   // サイド記える (记录阵营)
        ++combo_id;
      }
    }
  }
}

//*************************************************************
// コンボが 成立できるかチェック (检查合击在发生时是否还有效(合击不能反击、不能躲避))
// 後の 値がコンボID で ない なら FALSE
// コンボIDで きるなら TRUE (合击判定成立则TRUE)
// できない  FALSE (否则FALSE)
// ************************************************************
static BOOL ComboCheck(
    RoundActionListEntry *pRoundActionList, // エントリスト (入场列表)
    int currAction,                  // 現在の位置 (当前Action的序号)
    int actionNum                    // エントリ 数 (本轮Action的总数)
    )
{

  int i;
  // コンボID 保存 (保存连击ID)
  int combo_id = pRoundActionList[currAction].combo;
  int char_index = pRoundActionList[currAction].char_index;
  // 2026.09.19 AI发现的逻辑错误, 这里多了一个取反的操作
  if (CHAR_CHECKINDEX(char_index)) {
    // 忠誠が 低いなら 失敗 (忠诚不足则失败)
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
        CHAR_BATTLEFLG_AIBAD) {
      return FALSE;
    }
  }
  // 後の 値がコンボIDに 一致するか (后续是否同连击)
  for (i = currAction + 1; i < actionNum; i++) {
    char_index = pRoundActionList[i].char_index;
    if (!CHAR_CHECKINDEX(char_index))
      continue;
    // コンボID が 違うと 終了 (合击ID不同, 即不参与本轮合击)
    if (combo_id != pRoundActionList[i].combo)
      break;
    // 戦闘に 参加していないなら次へ
    int battle_mode = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE);
    if (battle_mode == 0 // 未参与战斗(Bug?)
        || battle_mode == BATTLE_CHARMODE_FINAL // 战斗已结束(击飞或逃跑)
        || CHAR_getInt(char_index, CHAR_HP) <= 0 // 战斗中死亡
        || BATTLE_CanMoveCheck(char_index) == FALSE) { // 石化、睡眠、混乱
      continue;
    } else {
      return TRUE;
    }
  }
  return FALSE;
}

// 击飞的判断
void BATTLE_UltimateExtra(int battle_index, int char_index, int enemy_index) {
  char szBuffer[256] = "";
  int pindex, pno;
  int floor = 0, x = 0, y = 0;
  szBuffer[0] = 0;
  if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
      || CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif
  ) {
    BATTLE_PetDefaultExit(enemy_index, battle_index);
    if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_P) {
      if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        CHAR_setInt(enemy_index, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(enemy_index, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(enemy_index, CHAR_DUELLOSECOUNT,
                    CHAR_getInt(enemy_index, CHAR_DUELLOSECOUNT) + 1);
        CHAR_setInt(enemy_index, CHAR_DUELSTWINCOUNT, 0);
      }
    } else if (BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E) {
      int levelflg = 1;
#ifdef _ULTIMATE_ANNOUNCE
      if ((CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
           CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
          (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
           CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPET ||
           CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
           CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC)) {
        int badindex;
        if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          badindex = char_index;
        } else {
          badindex = CHAR_getWorkInt(char_index, CHAR_WORKPLAYERINDEX);
        }
        if (CHAR_getWorkInt(char_index, CHAR_WORKCONFUSION) <= 0) {
          if (enemy_index != badindex) {
            //			CHAR_warpToSpecificPoint( badindex, 117, 289,
            // 168
            //); 			CHAR_DischargePartyNoMsg( badindex );
            //BATTLE_Exit( badindex, battle_index );
            /*
            if (CHAR_getInt(badindex,CHAR_FAME) <10000){
                    CHAR_setInt(badindex,CHAR_FAME,0);
            }else{
                    CHAR_setInt(badindex,CHAR_FAME,CHAR_getInt(badindex,CHAR_FAME)-10000);
            }
            */
            char message[256];
            sprintf(message,
                    "%s 被 %s 击飞至远方， %s 因飞人（宠），二人恩怨值火爆上升！！",
                    CHAR_getUseName(enemy_index), CHAR_getUseName(badindex),
                    CHAR_getUseName(badindex));
            int i;
            int playernum = CHAR_getPlayerMaxNum();
            for (i = 0; i < playernum; i++) {
              if (CHAR_getCharUse(i) != FALSE) {
                CHAR_talkToCli(i, -1, message, CHAR_COLORRED);
              }
            }
          }
        }
      }
#endif
      // snprintf( szBuffer, sizeof(szBuffer),
      //	"(%s)被击飞至远方。",
      //	CHAR_getUseName( enemy_index ) );
      if (CHAR_getInt(enemy_index, CHAR_LV) <= 10) {
        levelflg = 2;
      }

      if (BattleArray[battle_index].norisk == 0) {
        CHAR_AddCharm(enemy_index, CH_FIX_PLAYEULTIMATE / levelflg);
        pno = CHAR_getInt(enemy_index, CHAR_DEFAULTPET);
        if (0 <= pno && pno < CHAR_MAXPETHAVE) {
          pindex = CHAR_getCharPet(enemy_index, pno);
          if (CHAR_CHECKINDEX(pindex) == TRUE) {
            CHAR_PetAddVariableAi(pindex, AI_FIX_PLAYERULTIMATE / levelflg);
          }
        }
      }
      if (CHAR_getElderPosition(CHAR_getInt(enemy_index, CHAR_LASTTALKELDER),
                                &floor, &x, &y) != TRUE) {
      } else {
        CHAR_warpToSpecificPoint(enemy_index, floor, x, y);
      }
    }
    BATTLE_Exit(enemy_index, battle_index);
    CHAR_DischargePartyNoMsg(enemy_index);
    if (getBattleDebugMsg() != 0) {
      BATTLE_talkToCli(enemy_index, szBuffer, CHAR_COLORYELLOW);
    }

  } else if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    // ペットなら (若是宠物)
    int levelflg = 1;
    int playerindex = CHAR_getWorkInt(enemy_index, CHAR_WORKPLAYERINDEX);
    // レベルが低い場合は 減る (等级低则减半)
    if (CHAR_getInt(playerindex, CHAR_LV) <= 10) {
      levelflg = 2;
    }
    // 主人のデフォルトペットから 外す (解除主人默认宠物)
    CHAR_setInt(playerindex, CHAR_DEFAULTPET, -1);
    // PvPでなければ 懲罰を 与える (非PvP则给予惩罚)
    if (BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P) {
      // リスクのある バトルなら 懲罰を 与える (有风险战斗则给予惩罚)
      if (BattleArray[battle_index].norisk == 0) {
        CHAR_PetAddVariableAi(enemy_index, AI_FIX_PETULTIMATE / levelflg);
      }
      // 主人のペット 死亡数をカウントする (统计主人宠物死亡数)
      CHAR_setInt(playerindex, CHAR_DEADPETCOUNT,
                  CHAR_getInt(playerindex, CHAR_DEADPETCOUNT) + 1);
    }
    // 戦闘から 除外する (从战斗中移除)
    BATTLE_Exit(enemy_index, battle_index);
  } else {
    int flg;
    // これでなければ アルティメット (否则为终极技)
    // 敵キャラはサイドに しておく (敌方角色保留在阵营中)
    flg = CHAR_getWorkInt(enemy_index, CHAR_WORKBATTLEFLG);
    flg |= CHAR_BATTLEFLG_ULTIMATE; // アルティメットを かけた (附加终极技状态)
    CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLEFLG, flg);
    BATTLE_Exit(enemy_index, battle_index);
  }
}

//*************************************************************
//  死亡 した場合の 追加処理 (死亡时的附加处理)
//*************************************************************
void BATTLE_NormalDeadExtra(int battle_index, // 戦闘インデックス (战斗索引)
                            int char_index, // 倒したキャラのインデックス (被击倒角色的索引)
                            int enemy_index // 倒されたキャラのインデックス (被击倒角色的索引)
                            )
{
  int pindex, pno;
  char szBuffer[256] = "";

  szBuffer[0] = 0;
  // プレイヤーで (是玩家)
  // 敵との戦闘だったら (若是与敌对战)
  // リスクのあるバトルなら (若为有风险战斗)
  if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E &&
      BattleArray[battle_index].norisk == 0) {
#ifdef _ULTIMATE_ANNOUNCE
    if ((CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
         CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
        (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
         CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPET ||
         CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
         CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC)) {
      int badindex;
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        badindex = char_index;
      } else {
        badindex = CHAR_getWorkInt(char_index, CHAR_WORKPLAYERINDEX);
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKCONFUSION) <= 0) {
        if (enemy_index != badindex) {
          //			CHAR_warpToSpecificPoint( badindex, 117, 289,
          // 168
          //); 			CHAR_DischargePartyNoMsg( badindex );
          // BATTLE_Exit( badindex, battle_index );
          /*
          if (CHAR_getInt(badindex,CHAR_FAME) <10000){
                  CHAR_setInt(badindex,CHAR_FAME,0);
          }else{
                  CHAR_setInt(badindex,CHAR_FAME,CHAR_getInt(badindex,CHAR_FAME)-10000);
          }
          */
          char message[256];
          sprintf(message, "%s 被 %s 击飞至远方， %s 因飞人，二人恩怨值火爆上升！！",
                  CHAR_getUseName(enemy_index), CHAR_getUseName(badindex),
                  CHAR_getUseName(badindex));
          int i;
          int playernum = CHAR_getPlayerMaxNum();
          for (i = 0; i < playernum; i++) {
            if (CHAR_getCharUse(i) != FALSE) {
              CHAR_talkToCli(i, -1, message, CHAR_COLORRED);
            }
          }
        }
      }
    }
#endif

    int levelflg = 1;
    // 戦闘の 魅力を 与える (给予战斗魅力)
    // レベルが 低い場合は 減らす (等级低则减半)
    if (CHAR_getInt(enemy_index, CHAR_LV) <= 10) {
      levelflg = 2;
    }
    CHAR_AddCharm(enemy_index, CH_FIX_PLAYERDEAD / levelflg);
    // 戦闘に 参加していたペットの を 与える (给参战宠物魅力)
    pno = CHAR_getInt(enemy_index, CHAR_DEFAULTPET);
    if (0 <= pno && pno < CHAR_MAXPETHAVE) {
      // ペットのインデックス (宠物索引)
      pindex = CHAR_getCharPet(enemy_index, pno);
      if (CHAR_CHECKINDEX(pindex) == TRUE) {
        CHAR_PetAddVariableAi(pindex, AI_FIX_PLAYERDEAD / levelflg);
      }
    }
    // 倒れたキャラのコマンドは 無効にする (死亡角色指令置无效)
    CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  } else if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
    // ペットなら (若是宠物)
        BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E &&
        BattleArray[battle_index].norisk == 0) {
      int levelflg = 1;
      int playerindex = CHAR_getWorkInt(enemy_index, CHAR_WORKPLAYERINDEX);

      // 主人のレベルが 低い場合は 減らす (主人等级低则减半)
      if (CHAR_getInt(playerindex, CHAR_LV) <= 10) {
        levelflg = 2;
      }
      // 戦闘の懲罰を 与える (给予战斗惩罚)
      CHAR_PetAddVariableAi(enemy_index, AI_FIX_PETDEAD / levelflg);
      // 主人のペット 死亡数をカウントする (统计主人宠物死亡数)
      CHAR_setInt(playerindex, CHAR_DEADPETCOUNT,
                  CHAR_getInt(playerindex, CHAR_DEADPETCOUNT) + 1);

      // 倒れたキャラのコマンドは 無効にする (死亡角色指令置无效)
      CHAR_setWorkInt(enemy_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  }
}

void BATTLE_EscapeDpSend(int battle_index, int char_index) {
  int enemyside, dpadd;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return;
  if (BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P)
    return;
#ifndef DANTAI
  if (BattleArray[battle_index].flg & BATTLE_FLG_FREEDP) {
    CHAR_setWorkInt(char_index, CHAR_WORKGETEXP, 0);
    return;
  }
  BattleArray[battle_index].flg |= BATTLE_FLG_CHARALOST;
#endif
  enemyside = 1 - CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE);
  dpadd = CHAR_getInt(char_index, CHAR_DUELPOINT) * DUELPOINT_RATE;
  if (dpadd < 1)
    dpadd = 1;
  CHAR_setWorkInt(char_index, CHAR_WORKGETEXP, -dpadd * 2);

#ifndef DANTAI
  int cnt = BATTLE_CountAlive(battle_index, enemyside);
  if (cnt == 0)
    return;
  dpadd /= cnt;
  if (dpadd < 1)
    dpadd = 1;
  BATTLE_AddDpAlive(battle_index, enemyside, dpadd);
#else
  BattleArray[battle_index].Side[enemyside].common_dp += dpadd;
#endif

#ifndef DANTAI
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_setInt(char_index, CHAR_DUELBATTLECOUNT,
                CHAR_getInt(char_index, CHAR_DUELBATTLECOUNT) + 1);
    CHAR_setInt(char_index, CHAR_DUELLOSECOUNT,
                CHAR_getInt(char_index, CHAR_DUELLOSECOUNT) + 1);
    CHAR_setInt(char_index, CHAR_DUELSTWINCOUNT, 0);
  }
#endif
}

int BATTLE_AddDuelPoint(int battle_index, int *pBidList) {
  int i, j, k, side, num, allnum = 0;
  int char_index[BATTLE_ENTRY_MAX + 1];

  if (pBidList == NULL)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  int bid = pBidList[0];
  side = bid >= SIDE_OFFSET ? 1 : 0;
  num = bid - side * SIDE_OFFSET;
  if (BATTLE_CHECKSIDE(side) == FALSE || num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;

  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++)
    char_index[i] = -1;

  // 将攻击宠物换算为主人，并去除重复的参与者。
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int work = BATTLE_No2Index(battle_index, pBidList[i]);
    if (work < 0)
      return BATTLE_ERR_PARAM;
    if (CHAR_getInt(work, CHAR_WHICHTYPE) == CHAR_TYPEENEMY)
      continue;
    if (CHAR_getInt(work, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      work = CHAR_getWorkInt(work, CHAR_WORKPLAYERINDEX);
    if (!CHAR_CHECKINDEX(work))
      return BATTLE_ERR_PARAM;
    for (k = 0; k < allnum && char_index[k] != work; k++)
      ;
    if (k < allnum)
      continue;
    char_index[allnum++] = work;
  }
  char_index[allnum] = -1;

  for (j = 0; j < 2; j++) {
    BATTLE_ENTRY *pEntryEnemy = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      int enemy_index = pEntryEnemy[i].char_index;
      if (!CHAR_CHECKINDEX(enemy_index) ||
          CHAR_getInt(enemy_index, CHAR_HP) > 0 ||
          CHAR_getFlg(enemy_index, CHAR_ISDIE) == TRUE)
        continue;

      int enemy_type = CHAR_getInt(enemy_index, CHAR_WHICHTYPE);
      BOOL ultimate = (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) != 0;
#ifndef DANTAI
      if (enemy_type == CHAR_TYPEPLAYER)
        BattleArray[battle_index].flg |= BATTLE_FLG_CHARALOST;
#endif

#ifdef DANTAI
      if (ultimate && enemy_type == CHAR_TYPEPLAYER) {
        int dpadd = CHAR_getInt(enemy_index, CHAR_DUELPOINT) * DUELPOINT_RATE;
        CHAR_setWorkInt(enemy_index, CHAR_WORKGETEXP,
                        CHAR_getWorkInt(enemy_index, CHAR_WORKGETEXP) - dpadd);
        BattleArray[battle_index].Side[1 - j].common_dp += dpadd;
        BATTLE_GetProfit(battle_index, j, i);
      }
#else
      if (enemy_type == CHAR_TYPEPLAYER || enemy_type == CHAR_TYPEENEMY) {
        int dpadd = CHAR_getInt(enemy_index, CHAR_DUELPOINT) * DUELPOINT_RATE;
        CHAR_setWorkInt(enemy_index, CHAR_WORKGETEXP,
                        CHAR_getWorkInt(enemy_index, CHAR_WORKGETEXP) - dpadd);
        if (side != j) {
          int divisor = allnum > 0 ? allnum : 1;
          dpadd /= divisor;
          if (dpadd <= 0)
            dpadd = 1;
          for (k = 0; k < allnum; k++)
            CHAR_setWorkInt(char_index[k], CHAR_WORKGETEXP,
                            CHAR_getWorkInt(char_index[k], CHAR_WORKGETEXP) + dpadd);
        } else {
          int alive = BATTLE_CountAlive(battle_index, 1 - j);
          int dpdiv = dpadd / (alive > 0 ? alive : 1);
          BATTLE_AddDpAlive(battle_index, 1 - j, dpdiv > 0 ? dpdiv : 1);
        }
      }
      if (ultimate)
        BATTLE_GetProfit(battle_index, j, i);
#endif

      CHAR_setFlg(enemy_index, CHAR_ISDIE, 1);
      CHAR_setInt(enemy_index, CHAR_DEADCOUNT,
                  CHAR_getInt(enemy_index, CHAR_DEADCOUNT) + 1);
      if (ultimate) {
        BATTLE_UltimateExtra(battle_index, char_index[0], enemy_index);
      } else {
        BATTLE_NormalDeadExtra(battle_index, char_index[0], enemy_index);
      }
    }
  }
  return 0;
}

int BATTLE_ItemDelCheck(int item_index) {
  int icnt, jcnt, playernum;
  playernum = getFdnum();
  for (icnt = 0; icnt < playernum; icnt++) {
    if (CHAR_CHECKINDEX(icnt)) {
#ifdef _NEW_ITEM_
      int itemMax = CheckCharMaxItem(icnt);
      for (jcnt = 0; jcnt < itemMax; jcnt++) {
#else
      for (jcnt = 0; jcnt < CHAR_MAXITEMHAVE; jcnt++) {
#endif
        if (CHAR_getItemIndex(icnt, jcnt) == item_index) {
          print("err:战利品奇怪的item处理(%s)(%s)index(%d)\n", CHAR_getUseName(icnt),
                ITEM_getAppropriateName(item_index), item_index);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

#ifdef _COMBO_EXP
int BATTLE_AddComboExp(int battle_index, int *pBidList) {
  int enemy_index, i, side, num, proflg = 1, j, k;
  int char_index[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy;
  int item = 0, item_index, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    char_index[i] = -1;
  }

  if (BattleArray[battle_index].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battle_index].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    char_index[i] = BATTLE_No2Index(battle_index, pBidList[i]);
    if (char_index[i] < 0)
      return BATTLE_ERR_PARAM;
  }

  char_index[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemy_index = pEntryEnemy[i].char_index;
      if (CHAR_CHECKINDEX(enemy_index) == FALSE)
        continue;

      if (CHAR_getInt(enemy_index, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemy_index, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {
        int fix_exp;
        int exp = CHAR_getInt(enemy_index, CHAR_EXP);
        int enemy_level = CHAR_getInt(enemy_index, CHAR_LV);
        for (k = 0; char_index[k] != -1; k++) {
          int char_level = CHAR_getInt(char_index[k], CHAR_LV);
          fix_exp = BATTLE_calcExp(char_level, enemy_level, exp);
          CHAR_setWorkInt(char_index[k], CHAR_WORKGETEXP,
                          CHAR_getWorkInt(char_index[k], CHAR_WORKGETEXP) +
                              fix_exp);
          int ridepet = BATTLE_getRidePet(char_index[k]);
          // andy_edit
          if (CHAR_CHECKINDEX(ridepet)) {
            int ridepet_level = CHAR_getInt(ridepet, CHAR_LV);
            fix_exp = BATTLE_calcExp(ridepet_level, enemy_level, exp);
            fix_exp *= 0.6;
            CHAR_setWorkInt(ridepet, CHAR_WORKGETEXP,
                            CHAR_getWorkInt(ridepet, CHAR_WORKGETEXP) + fix_exp);
          }
        }
      }
    }
  }
  return 0;
}
#endif
#ifdef _SHARE_EXP
int BATTLE_AddExp(int battle_index, int *pBidList) {
  int enemy_index, i, side, num, proflg = 1, j, exp, k, enemylevel,
                                char_index[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy, *pEntryChara[BATTLE_ENTRY_MAX + 1],
      *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, item_index, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    char_index[i] = -1;
    pEntryPlayer[i] = NULL;
  }

  if (BattleArray[battle_index].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battle_index].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    char_index[i] = BATTLE_No2Index(battle_index, pBidList[i]);
    if (char_index[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battle_index].Side[side].Entry[subnum];
    if (CHAR_getInt(char_index[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battle_index].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }

  char_index[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemy_index = pEntryEnemy[i].char_index;
      if (CHAR_CHECKINDEX(enemy_index) == FALSE)
        continue;

      if (CHAR_getInt(enemy_index, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemy_index, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {

        int base_exp = CHAR_getInt(enemy_index, CHAR_EXP);
        int enemy_level = CHAR_getInt(enemy_index, CHAR_LV);
        for (k = 0; char_index[k] != -1; k++) {
          int fix_exp;
          int char_level = CHAR_getInt(char_index[k], CHAR_LV);
          fix_exp = BATTLE_calcExp(char_level, enemy_level, base_exp);
          CHAR_setWorkInt(char_index[k], CHAR_WORKGETEXP,
                          CHAR_getWorkInt(char_index[k], CHAR_WORKGETEXP) +
                              fix_exp);
          CHAR_setInt(char_index[k], CHAR_KILLPETCOUNT,
                      CHAR_getInt(char_index[k], CHAR_KILLPETCOUNT) + 1);
          int ridepet = BATTLE_getRidePet(char_index[k]);
          // andy_edit
          if (CHAR_CHECKINDEX(ridepet)) {
            int ridepet_level = CHAR_getInt(ridepet, CHAR_LV);
            fix_exp = BATTLE_calcExp(char_level, enemy_level, base_exp);
            fix_exp *= 0.6;
            CHAR_setWorkInt(ridepet, CHAR_WORKGETEXP,
                            CHAR_getWorkInt(ridepet, CHAR_WORKGETEXP) + fix_exp);
            CHAR_setInt(ridepet, CHAR_KILLPETCOUNT,
                        CHAR_getInt(ridepet, CHAR_KILLPETCOUNT) + 1);
          }
          if (BattleArray[battle_index].norisk == 0 &&
              CHAR_getInt(char_index[k], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            if (CHAR_getInt(enemy_index, CHAR_LV) >
                CHAR_getInt(char_index[k], CHAR_LV)) {
              CHAR_PetAddVariableAi(char_index[k], AI_FIX_PETGOLDWIN);
            } else {
              CHAR_PetAddVariableAi(char_index[k], AI_FIX_PETWIN);
            }
          }
        }
        CHAR_setMaxExp(enemy_index, 0);
      }
      CHAR_setFlg(enemy_index, CHAR_ISDIE, 1);
#ifdef _PET_LIMITLEVEL
      Pet_Check_Die(enemy_index);
#endif
      CHAR_setInt(enemy_index, CHAR_DEADCOUNT,
                  CHAR_getInt(enemy_index, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        BATTLE_UltimateExtra(battle_index, char_index[0], enemy_index);
      } else {
        BATTLE_NormalDeadExtra(battle_index, char_index[0], enemy_index);
      }
    }
  }
  return 0;
}

int BATTLE_AddItem(int battle_index, int *pBidList) {
  int enemy_index, i, side, num, proflg = 1, j, exp, k, enemylevel,
                                char_index[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy, *pEntryChara[BATTLE_ENTRY_MAX + 1],
      *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, item_index, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    char_index[i] = -1;
    pEntryPlayer[i] = NULL;
  }

  if (BattleArray[battle_index].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battle_index].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    char_index[i] = BATTLE_No2Index(battle_index, pBidList[i]);
    if (char_index[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battle_index].Side[side].Entry[subnum];
    if (CHAR_getInt(char_index[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battle_index].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }

  char_index[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemy_index = pEntryEnemy[i].char_index;
      if (CHAR_CHECKINDEX(enemy_index) == FALSE)
        continue;

      if (CHAR_getInt(enemy_index, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemy_index, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {
        if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) != CHAR_TYPEENEMY) {
          print("\nerr: 夺取敌人以外的战利品(%s:%d)\n", __FILE__, __LINE__);
        }
        for (item = CHAR_STARTITEMARRAY; item < CHAR_STARTITEMARRAY + 10;
             item++) {
          item_index = CHAR_getItemIndex(enemy_index, item);
          if (ITEM_CHECKINDEX(item_index) == FALSE) {
            continue;
          }
          if (ITEM_getWorkInt(item_index, ITEM_WORKCHARAINDEX) <= getFdnum()) {
            print("\nerr: 将玩家的item当做战利品(%s:%d)\n", __FILE__, __LINE__);
            continue;
          }
          CHAR_setItemIndex(enemy_index, item, -1);
          k = RAND(0, (allnum - 1));
          for (itemloop = 0; itemloop < 3; itemloop++) {
            if (pEntryPlayer[k]->getitem[itemloop] == -1) {
              pEntryPlayer[k]->getitem[itemloop] = item_index;
              break;
            }
          }
          if (itemloop >= 3) {
            if (RAND(0, 1)) {
              int olditem;
              int randwork = RAND(0, 2);
              olditem = pEntryPlayer[k]->getitem[randwork];
              pEntryPlayer[k]->getitem[randwork] = item_index;
              if (BATTLE_ItemDelCheck(olditem) == TRUE) {
                ITEM_endExistItemsOne(olditem);
              }
            } else {
              ITEM_endExistItemsOne(item_index);
            }
          }
        }
      }
    }
  }
  return 0;
}

#endif


#define EXPGET_MAXLEVEL 5 // 相差5级以内, 
#define EXPGET_DIV 15     // 相差15级以上，直接为1
// 2026.09.18 经验值计算
inline int BATTLE_calcExp(int me_level,    // 我方LEVEL
                          int enemy_level, // 敌方LEVEL
                          int base_exp     // 敌人基础经验
) {
  int fix_exp;
  int level_diff = me_level - enemy_level;
  if (level_diff <= EXPGET_MAXLEVEL) {
    fix_exp = base_exp;
  } else {
    level_diff = EXPGET_MAXLEVEL + EXPGET_DIV - level_diff;
    // level_diff = level_diff > EXPGET_DIV ? EXPGET_DIV : level_diff;
    if (level_diff <= 0) {
      fix_exp = 1;
    } else {
      fix_exp = base_exp * level_diff / EXPGET_DIV;
    }
  }
  fix_exp = fix_exp < 1 ? 1 : fix_exp;
  return fix_exp;
}
int BATTLE_AddExpItem(int battle_index, int *pBidList) {
  int enemy_index, i, side, num, proflg = 1, j, k;
  int char_index[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY *pEntryEnemy;
  BATTLE_ENTRY *pEntryChara[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, item_index, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    char_index[i] = -1;
    pEntryPlayer[i] = NULL;
  }

  if (BattleArray[battle_index].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battle_index].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    char_index[i] = BATTLE_No2Index(battle_index, pBidList[i]);
    if (char_index[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battle_index].Side[side].Entry[subnum];
    if (CHAR_getInt(char_index[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battle_index].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }

  char_index[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battle_index].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemy_index = pEntryEnemy[i].char_index;
      if (CHAR_CHECKINDEX(enemy_index) == FALSE)
        continue;

      if (CHAR_getInt(enemy_index, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemy_index, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {
        if (CHAR_getInt(enemy_index, CHAR_WHICHTYPE) != CHAR_TYPEENEMY) {
          print("\nerr: 夺取敌人以外的战利品(%s:%d)\n", __FILE__, __LINE__);
        }
        for (item = CHAR_STARTITEMARRAY; item < CHAR_STARTITEMARRAY + 10;
             item++) {
          item_index = CHAR_getItemIndex(enemy_index, item);
          if (ITEM_CHECKINDEX(item_index) == FALSE) {
            continue;
          }
          if (ITEM_getWorkInt(item_index, ITEM_WORKCHARAINDEX) <= getFdnum()) {
            print("\nerr: 将玩家的item当做战利品(%s:%d)\n", __FILE__, __LINE__);
            continue;
          }
          CHAR_setItemIndex(enemy_index, item, -1);
          k = RAND(0, (allnum - 1));
          for (itemloop = 0; itemloop < 3; itemloop++) {
            if (pEntryPlayer[k]->getitem[itemloop] == -1) {
              pEntryPlayer[k]->getitem[itemloop] = item_index;
              break;
            }
          }
          if (itemloop >= 3) {
            if (RAND(0, 1)) {
              int olditem;
              int randwork = RAND(0, 2);
              olditem = pEntryPlayer[k]->getitem[randwork];
              pEntryPlayer[k]->getitem[randwork] = item_index;
              if (BATTLE_ItemDelCheck(olditem) == TRUE) {
                ITEM_endExistItemsOne(olditem);
              }
            } else {
              ITEM_endExistItemsOne(item_index);
            }
          }
        }

        // 2026.09.18 计算经验
        int fix_exp;
        int base_exp = CHAR_getInt(enemy_index, CHAR_EXP);
        int enemy_level = CHAR_getInt(enemy_index, CHAR_LV);
        for (k = 0; char_index[k] != -1; k++) {
          // 计算第K个人的经验
          int char_level = CHAR_getInt(char_index[k], CHAR_LV);
          fix_exp = BATTLE_calcExp(char_level, enemy_level, base_exp);
          CHAR_setWorkInt(char_index[k], CHAR_WORKGETEXP,
                          CHAR_getWorkInt(char_index[k], CHAR_WORKGETEXP) +
                          fix_exp);
          CHAR_setInt(char_index[k], CHAR_KILLPETCOUNT,
                      CHAR_getInt(char_index[k], CHAR_KILLPETCOUNT) + 1);
          // 计算第K个人的骑宠
          int ridepet = BATTLE_getRidePet(char_index[k]);
          // andy_edit
          if (CHAR_CHECKINDEX(ridepet)) {
            int ridepet_level = CHAR_getInt(ridepet, CHAR_LV);
            fix_exp = BATTLE_calcExp(ridepet_level, enemy_level, base_exp);
            fix_exp *= 0.6;
            CHAR_setWorkInt(ridepet, CHAR_WORKGETEXP,
                            CHAR_getWorkInt(ridepet, CHAR_WORKGETEXP) + fix_exp);
            CHAR_setInt(ridepet, CHAR_KILLPETCOUNT,
                        CHAR_getInt(ridepet, CHAR_KILLPETCOUNT) + 1);
          }
          if (BattleArray[battle_index].norisk == 0 &&
              CHAR_getInt(char_index[k], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            if (CHAR_getInt(enemy_index, CHAR_LV) >
                CHAR_getInt(char_index[k], CHAR_LV)) {
              CHAR_PetAddVariableAi(char_index[k], AI_FIX_PETGOLDWIN);
            } else {
              CHAR_PetAddVariableAi(char_index[k], AI_FIX_PETWIN);
            }
          }
        }
        CHAR_setMaxExp(enemy_index, 0);
      }
      CHAR_setFlg(enemy_index, CHAR_ISDIE, 1);
#ifdef _PET_LIMITLEVEL
      Pet_Check_Die(enemy_index);
#endif
      CHAR_setInt(enemy_index, CHAR_DEADCOUNT,
                  CHAR_getInt(enemy_index, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        BATTLE_UltimateExtra(battle_index, char_index[0], enemy_index);
      } else {
        BATTLE_NormalDeadExtra(battle_index, char_index[0], enemy_index);
      }
    }
  }
  return 0;
}

#ifdef _PET_LIMITLEVEL
void Pet_Check_Die(int petindex) {

  int LevelUpPoint;
  int vital, str, tgh, dex;
  float modai;

  // 宠物玛蕾菲雅死亡扣基本属性及忠诚
  if (CHAR_getInt(petindex, CHAR_PETID) == 718) {
    LevelUpPoint = CHAR_getInt(petindex, CHAR_ALLOCPOINT);
    vital = ((LevelUpPoint >> 24) & 0xff);
    str = ((LevelUpPoint >> 16) & 0xff);
    tgh = ((LevelUpPoint >> 8) & 0xff);
    dex = ((LevelUpPoint >> 0) & 0xff);
    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);
    vital = vital - RAND(1, 8);
    str = str - RAND(1, 4);
    tgh = tgh - RAND(1, 4);
    dex = dex - RAND(1, 4);
    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);
    vital = vital < 0 ? 0 : vital > 50 ? 50 : vital;
    str = str < 0 ? 0 : str > 50 ? 50 : str;
    tgh = tgh < 0 ? 0 : tgh > 50 ? 50 : tgh;
    dex = dex < 0 ? 0 : dex > 50 ? 50 : dex;
    LevelUpPoint = (vital << 24) + (str << 16) + (tgh << 8) + (dex << 0);
    CHAR_setInt(petindex, CHAR_ALLOCPOINT, LevelUpPoint);
    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);
    modai = CHAR_getInt(petindex, CHAR_MODAI);
    modai = modai - ((modai * 5) / 100);
    CHAR_setInt(petindex, CHAR_MODAI, modai);

    LogPetPointChange(CHAR_getChar(petindex, CHAR_OWNERCHARANAME),
                      CHAR_getChar(petindex, CHAR_OWNERCDKEY),
                      CHAR_getChar(petindex, CHAR_NAME), petindex, 2,
                      CHAR_getInt(petindex, CHAR_LV), "battle_die(战斗死亡)",
                      CHAR_getInt(petindex, CHAR_FLOOR), 0, 0);
  }
  return;
}
#endif

int BATTLE_AddProfit(int battle_index, int *pBidList) {
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  if (BattleArray[battle_index].dpbattle == 1) {
    return BATTLE_AddDuelPoint(battle_index, pBidList);
  } else {
#ifdef _SHARE_EXP
    if (getExpShare() == 1 &&
        BattleArray[battle_index].type == BATTLE_TYPE_P_vs_E) {

      if (pBidList[0] >= SIDE_OFFSET || pBidList[0] < 0)
        return BATTLE_AddExpItem(battle_index, pBidList);

#ifdef _COMBO_EXP
      if (pBidList[0] > -1 && pBidList[1] > -1) {
        BATTLE_AddComboExp(battle_index, pBidList);
      }
#endif
      BATTLE_AddItem(battle_index, pBidList);
      int i = 0;
      int k = 0;
      int aAttackList[BATTLE_ENTRY_MAX + 1];
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        if (CHAR_CHECKINDEX(
                BattleArray[battle_index].Side[0].Entry[i].char_index) == FALSE)
          continue;
        if (BATTLE_CanMoveCheck(
                BattleArray[battle_index].Side[0].Entry[i].char_index) ==
            FALSE) {
          continue;
        }
        if (CHAR_getInt(BattleArray[battle_index].Side[0].Entry[i].char_index,
                        CHAR_HP) <= 0) {
          continue;
        }
        aAttackList[k++] = BattleArray[battle_index].Side[0].Entry[i].bid;
      }
      aAttackList[k] = -1;
      return BATTLE_AddExp(battle_index, aAttackList);
    } else
#else
    return BATTLE_AddExpItem(battle_index, pBidList);
#endif
  }
}

int BATTLE_TargetCheck(int battle_index, int defNo) {
  int defindex = BATTLE_No2Index(battle_index, defNo);

  if (CHAR_CHECKINDEX(defindex) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) == 0 ||
      CHAR_getFlg(defindex, CHAR_ISDIE) == TRUE ||
      CHAR_getInt(defindex, CHAR_HP) <= 0 ||
      CHAR_getFlg(defindex, CHAR_ISATTACKED) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
    return FALSE;
  }
  return TRUE;
}

int BATTLE_TargetCheckDead(int battle_index, int defNo) {
  int defindex = BATTLE_No2Index(battle_index, defNo);
  if (CHAR_CHECKINDEX(defindex) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) == 0 ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE ||
      CHAR_getFlg(defindex, CHAR_ISATTACKED) == FALSE ||
      CHAR_getFlg(defindex, CHAR_ISDIE) == FALSE) {
    return FALSE;
  }
  return TRUE;
}

int BATTLE_TargetAdjust(int battle_index, int char_index, int myside) {
  int defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
  if (BATTLE_TargetCheck(battle_index, defNo) == FALSE) {
    defNo = BATTLE_DefaultAttacker(battle_index, 1 - myside);
  }
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, defNo);
  return defNo;
}

void Compute_Down(int char_index, int rideindex, int *down1, int *down2,
                  int flg) {
  int hp = 0, pethp = 0;
  int downs = 0;

  *down1 = 0;
  *down2 = 0;
  // 人物
  downs =
      CHAR_getInt(char_index, CHAR_VITAL) + CHAR_getInt(char_index, CHAR_STR) +
      CHAR_getInt(char_index, CHAR_DEX) + CHAR_getInt(char_index, CHAR_TOUGH);
  if ((downs = (((downs / 100) - 20) / 4)) < 1)
    downs = 1;
  hp = CHAR_getInt(char_index, CHAR_HP);
  if (hp <= downs)
    downs = hp - 1;
  if (downs >= 0) {
    hp = ((hp - downs) < 1) ? 1 : (hp - downs);
    CHAR_setInt(char_index, CHAR_HP, hp);
    *down1 = downs;
  }
  if (flg == -1) {
  } else { // 骑乘宠物
    if (!CHAR_CHECKINDEX(rideindex))
      return;
    downs =
        CHAR_getInt(rideindex, CHAR_VITAL) + CHAR_getInt(rideindex, CHAR_STR) +
        CHAR_getInt(rideindex, CHAR_DEX) + CHAR_getInt(rideindex, CHAR_TOUGH);
    if ((downs = (((downs / 100) - 20) / 4)) < 1)
      downs = 1;
    pethp = CHAR_getInt(rideindex, CHAR_HP);
    if (pethp <= downs)
      downs = pethp - 1;
    if (downs >= 0) {
      pethp = ((pethp - downs) < 1) ? 1 : (pethp - downs);
      CHAR_setInt(rideindex, CHAR_HP, pethp);
      *down2 = downs;
    }
  }
}

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
void Compute_Down_SARS(int char_index, int rideindex, int *down1, int *down2,
                       int flg) {
  int hp = 0, pethp = 0;
  int downs = 0;
  *down1 = 0;
  *down2 = 0;
  // 人物
  hp = CHAR_getInt(char_index, CHAR_HP);
  downs = hp * 10 / 100;

  if (hp <= downs)
    downs = hp - 1;
  if (downs >= 0) {
    hp = ((hp - downs) < 1) ? 1 : (hp - downs);
    CHAR_setInt(char_index, CHAR_HP, hp);
    *down1 = downs;
  }
  if (flg == -1) {
  } else { // 骑乘宠物
    if (!CHAR_CHECKINDEX(rideindex))
      return;
    downs =
        CHAR_getInt(rideindex, CHAR_VITAL) + CHAR_getInt(rideindex, CHAR_STR) +
        CHAR_getInt(rideindex, CHAR_DEX) + CHAR_getInt(rideindex, CHAR_TOUGH);
    if ((downs = (((downs / 100) - 20) / 4)) < 1)
      downs = 1;
    pethp = CHAR_getInt(rideindex, CHAR_HP);
    if (pethp <= downs)
      downs = pethp - 1;
    if (downs >= 0) {
      pethp = ((pethp - downs) < 1) ? 1 : (pethp - downs);

      CHAR_setInt(rideindex, CHAR_HP, pethp);
      *down2 = downs;
    }
  }
}
#endif

#ifdef _PROFESSION_ADDSKILL
void WorkIceCrackPlay(int char_index, int cnt, int workicecracknum) {
  char szBuffer[256] = "";
  int i, value = 0;
  int battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);
  int defNo = CHAR_getWorkInt(
      char_index, CHAR_WORKBATTLECOM2); // defNo = BATTLE_Index2No( battle_index,
                                        // char_index );
  int ridepet = BATTLE_getRidePet(char_index);
  int bid = BATTLE_Index2No(battle_index, char_index);
  value = CHAR_getWorkInt(char_index, workicecracknum);

  if (cnt <= 0 && value > 0) {
    int damage = 0, petdamage = 0;

    int ToList[SIDE_OFFSET * 2 + 1];
    memset(ToList, -1, sizeof(ToList));
    BATTLE_MultiList(battle_index, TARGET_SIDE_1, ToList);
    // 将魔法参数代入动画
    analysis_profession_parameter(2, 4, defNo, char_index);
    PROFESSION_MAGIC_ATTAIC_Effect(battle_index, bid, ToList, 2);

    if (defNo == 20 || defNo == 25 || defNo == 26) // 右
      bid = 0;
    else
      bid = 10;

    for (i = bid; i < bid + 10; i++) {
      int defType, charaidx, petidx;
      charaidx = BATTLE_No2Index(battle_index, i);
      if (CHAR_CHECKINDEX(charaidx)) {
        if (CHAR_getInt(charaidx, CHAR_HP) > 0) {
          int hp, pethp = 0;
          petidx = BATTLE_getRidePet(charaidx);
          defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
          CHAR_setWorkInt(charaidx, workicecracknum, 0);

          damage = value;

          hp = CHAR_getInt(charaidx, CHAR_HP);
          if (petidx == -1) {
            hp -= damage;
            if (hp < 0)
              hp = 0;
          } else {
            int defquick = 0, petquick = 0, playerdamage = 0;
            pethp = CHAR_getInt(petidx, CHAR_HP);
            defquick = CHAR_getWorkInt(charaidx, CHAR_WORKDEFENCEPOWER);
            petquick = CHAR_getWorkInt(petidx, CHAR_WORKDEFENCEPOWER);
            if (pethp > 0) {
              playerdamage = (damage * petquick) / (defquick + petquick) + 1;
              petdamage = damage - playerdamage + 1;
              hp -= playerdamage;
              pethp -= petdamage;
            } else {
              hp -= damage;
            }
            if (hp < 0)
              hp = 0;
            if (pethp < 0)
              pethp = 0;
            CHAR_setInt(petidx, CHAR_HP, pethp);
          }

          CHAR_setInt(charaidx, CHAR_HP, hp);

          if (hp <= 0) {
            CHAR_setFlg(charaidx, CHAR_ISDIE, 1);
            CHAR_setWorkInt(charaidx, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }
          if (pethp <= 0) {
            CHAR_setFlg(petidx, CHAR_ISDIE, 1);
            CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }

          if (damage >= 0) {
            sprintf(szBuffer, "%X|%X|%X|%X|", i, i, (int)(damage),
                    (int)(petdamage));
            BATTLESTR_ADD(szBuffer);
          }
          if (petidx != -1 && petdamage >= 0) {
          }
        }
      }
    }
    sprintf(szBuffer, "%X|", 0x5711438);
    BATTLESTR_ADD(szBuffer);
  }
}
#endif

static int BATTLE_StatusSeq(int char_index) {
  int cnt, i, bid, battle_index, down;
  char szBuffer[256] = "";
#ifdef _MAGIC_DEEPPOISON
  int defNo, defindex = 0;
  char szCommand[1024];
#endif
  int ridepet = BATTLE_getRidePet(char_index);
  int hp = 0, pethp = 0;

  battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);

#ifdef _MAGIC_DEEPPOISON
  defNo = BATTLE_Index2No(battle_index, char_index);
  defindex = BATTLE_No2Index(battle_index, defNo);
#endif
  bid = BATTLE_Index2No(battle_index, char_index);
  if (BATTLE_CanMoveCheck(char_index) == FALSE) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  }

  for (i = 1; i < BATTLE_ST_END; i++) {
    if ((cnt = CHAR_getWorkInt(char_index, StatusTbl[i])) <= 0)
      continue;
    CHAR_setWorkInt(char_index, StatusTbl[i], --cnt);
#ifdef _MAGIC_WEAKEN //   虚弱
    if (CHAR_getWorkInt(char_index, CHAR_WORKWEAKEN) > 0)
      CHAR_setWorkInt(char_index, StatusTbl[i], cnt + 1);
#endif
#ifdef _MAGIC_BARRIER // vincent  精灵:魔障
    if (CHAR_getWorkInt(char_index, CHAR_WORKBARRIER) > 0)
      CHAR_setWorkInt(char_index, StatusTbl[i], cnt + 1);
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    if (cnt <= 0 && (StatusTbl[i] != CHAR_WORKICECRACK)
#ifdef _PROFESSION_ADDSKILL
        && (StatusTbl[i] != CHAR_WORKICECRACK2) &&
        (StatusTbl[i] != CHAR_WORKICECRACK3) &&
        (StatusTbl[i] != CHAR_WORKICECRACK4) &&
        (StatusTbl[i] != CHAR_WORKICECRACK5) &&
        (StatusTbl[i] != CHAR_WORKICECRACK6) &&
        (StatusTbl[i] != CHAR_WORKICECRACK7) &&
        (StatusTbl[i] != CHAR_WORKICECRACK8) &&
        (StatusTbl[i] != CHAR_WORKICECRACK9) &&
        (StatusTbl[i] != CHAR_WORKICECRACK10)
#endif
    ) { // 冰爆术
#else
    if (cnt <= 0) {
#endif
      BATTLE_BadStatusString(bid, 0);
      // snprintf( szBuffer, sizeof( szBuffer ), "(%s)将(%s)修理完毕。",
      //	CHAR_getUseName( char_index ), aszStatusFull[i] );
      //	BATTLE_BroadCast( battle_index, szBuffer,
      //		(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
// Terry del
//				sprintf( szBuffer, "BM|%X|%X|", bid, 0 );
//				BATTLESTR_ADD( szBuffer );
#ifdef _MAGIC_NOCAST
      if (StatusTbl[i] == CHAR_WORKNOCAST) {
        GmsvServer_NC_send(getfdFromchar_index(char_index), 0); //复原
      }
#endif
      if (StatusTbl[i] == CHAR_WORKDRUNK) {
        if (ridepet != -1) {
          CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                          CHAR_getWorkInt(char_index, CHAR_WORKQUICK) +
                              CHAR_getWorkInt(ridepet, CHAR_WORKQUICK));
        } else {
          CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                          CHAR_getWorkInt(char_index, CHAR_WORKQUICK) * 2);
        }
      }
      continue;
    }

    switch (StatusTbl[i]) {

    case CHAR_WORKPOISON: {
      int petdown = -1;
      Compute_Down(char_index, ridepet, &down, &petdown, ridepet);
      if (down >= 0) {
        // snprintf( szBuffer, sizeof( szBuffer ),
        //	"(%s)因(%s)受到(%d)的损伤。",
        //	CHAR_getUseName( char_index ),
        //	aszStatusFull[i],
        //	down );
        // BATTLE_BroadCast( battle_index, szBuffer,
        //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

#ifdef _NOTRIDE_
        sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
        BATTLESTR_ADD(szBuffer);
#else
        sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                (int)(petdown));
        BATTLESTR_ADD(szBuffer);
#endif
      }
      if (ridepet != -1 && petdown >= 0) {
        // snprintf( szBuffer, sizeof( szBuffer ),
        //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
        //	CHAR_getUseName( char_index ),
        //	aszStatusFull[i], petdown );
        // BATTLE_BroadCast( battle_index, szBuffer,
        //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
      }
    } break;
#ifdef _MAGIC_DEEPPOISON
    case CHAR_WORKDEEPPOISON:
      if (CHAR_getInt(defindex, CHAR_HP) <= 1) {
        snprintf(szCommand, sizeof(szCommand), "Bd|a%X|r%X|f%X|d%X|p%X|FF|",
                 defNo, defNo, BCF_NORMAL, 1, 0);
        BATTLESTR_ADD(szCommand);
        CHAR_setInt(defindex, CHAR_HP, 0);
        CHAR_setWorkInt(defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        CHAR_setFlg(defindex, CHAR_ISDIE, 1);
        break;
      }

      if (CHAR_getWorkInt(defindex, CHAR_WORKDEEPPOISON) <= 1) {
        snprintf(szCommand, sizeof(szCommand), "Bd|a%X|r%X|f%X|d%X|p%X|FF|",
                 defNo, defNo, BCF_NORMAL, CHAR_getInt(char_index, CHAR_HP), 0);
        BATTLESTR_ADD(szCommand);
        CHAR_setInt(defindex, CHAR_HP, 0);
        CHAR_setFlg(defindex, CHAR_ISDIE, 1);
        CHAR_setWorkInt(defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        sprintf(szBuffer, "%s身中剧毒未解而阵亡了!!",
                CHAR_getChar(defindex, CHAR_NAME));
        if (CHAR_getInt(defindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          CHAR_talkToCli(CHAR_getWorkInt(defindex, CHAR_WORKPLAYERINDEX), -1,
                         szBuffer, CHAR_COLORYELLOW);
        } else {
          CHAR_talkToCli(defindex, -1, szBuffer, CHAR_COLORYELLOW);
        }
        break;
      }
      if (ridepet == -1) { // Robin 0728 ride Pet
        down = CHAR_getInt(char_index, CHAR_VITAL) +
               CHAR_getInt(char_index, CHAR_STR) +
               CHAR_getInt(char_index, CHAR_DEX) +
               CHAR_getInt(char_index, CHAR_TOUGH);
        down = (down / 100 - 20) / 4;
        if (down < 1)
          down = 1;
        hp = CHAR_getInt(char_index, CHAR_HP);
        if (hp <= down)
          down = hp - 1;
        if (down >= 0) {
          CHAR_setInt(char_index, CHAR_HP, hp - down);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( char_index ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battle_index, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
#ifdef _NOTRIDE_
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
          BATTLESTR_ADD(szBuffer);
#else
          sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down), 0);
          BATTLESTR_ADD(szBuffer);
#endif
        }
        break;
      } else {
        int petdown;
        down = CHAR_getInt(char_index, CHAR_VITAL) +
               CHAR_getInt(char_index, CHAR_STR) +
               CHAR_getInt(char_index, CHAR_DEX) +
               CHAR_getInt(char_index, CHAR_TOUGH);
        petdown =
            CHAR_getInt(ridepet, CHAR_VITAL) + CHAR_getInt(ridepet, CHAR_STR) +
            CHAR_getInt(ridepet, CHAR_DEX) + CHAR_getInt(ridepet, CHAR_TOUGH);
        down = (down / 100 - 20) / 4;
        petdown = (petdown / 100 - 20) / 4;
        if (down < 1)
          down = 1;
        if (petdown < 1)
          petdown = 1;
        hp = CHAR_getInt(char_index, CHAR_HP);
        pethp = CHAR_getInt(ridepet, CHAR_HP);
        if (hp <= down)
          down = hp - 1;
        if (pethp <= petdown)
          petdown = pethp - 1;
        if (down >= 0) {
          CHAR_setInt(char_index, CHAR_HP, hp - down);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( char_index ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battle_index, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
#ifdef _NOTRIDE_
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
          BATTLESTR_ADD(szBuffer);
#else
          sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                  (int)(petdown));
          BATTLESTR_ADD(szBuffer);
#endif
        }
        if (petdown >= 0) {
          CHAR_setInt(ridepet, CHAR_HP, pethp - petdown);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( char_index ),
          //	aszStatusFull[i],
          //	petdown );
          // BATTLE_BroadCast( battle_index, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
        }
        break;
      }
#endif
#ifdef _MAGIC_NOCAST // 精灵:沉默
    case CHAR_WORKNOCAST:
      GmsvServer_NC_send(getfdFromchar_index(char_index), 1); //沉默
      break;
#endif
    case CHAR_WORKCONFUSION: {
      int side, pos, defNo, lop;
      if (RAND(1, 100) > 80) {
        break;
      }
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = RAND(0, 1);
      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battle_index, defNo) == TRUE) {
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, -1);
      }
    } break;
#ifdef _PROFESSION_ADDSKILL
    case CHAR_WORKANNEX: // 附身术
    {
      int side, pos, defNo, lop;

      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = RAND(0, 1);
      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battle_index, defNo) == TRUE) {
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, -1);
      }

    } break;
#endif
    case CHAR_WORKDRUNK:
      break;

#ifdef _PROFESSION_SKILL    // WON ADD 人物职业技能
    case CHAR_WORKOBLIVION: // 遗忘
    {
      // 恢复技能
      if (cnt <= 1) {
        int defNo = BATTLE_Index2No(battle_index, char_index);
        int toNo = defNo - 5;
        int toNoindex = -1, pet_no = -1;
        char msg[20];

        memset(msg, -1, sizeof(msg));

        // 主人index
        toNoindex = BATTLE_No2Index(battle_index, toNo);
        CHAR_setWorkInt(char_index, StatusTbl[BATTLE_ST_OBLIVION], 0);

        // 宠物编号
        pet_no = CHAR_getInt(toNoindex, CHAR_DEFAULTPET);
        sprintf(msg, "W%d", pet_no);

        CHAR_sendStatusString(toNoindex, msg);

        BATTLE_BadStatusString(bid, 0);
        sprintf(szBuffer, "BM|%X|%X|", bid, 0);
        BATTLESTR_ADD(szBuffer);
      }
      break;
    }
    case CHAR_WORK_F_ENCLOSE: // 火附体
    {
      int ToList[SIDE_OFFSET * 2 + 1];
      int defNo = BATTLE_Index2No(battle_index, char_index);
      int old_hp = CHAR_getInt(char_index, CHAR_HP);
      int dec_hp = 0, hp = 0;
      int DAMAGE = 50;

      memset(ToList, -1, sizeof(ToList));
      BATTLE_MultiList(battle_index, defNo, ToList);
      BATTLE_MagicEffect(battle_index, defNo, ToList, 101699, 101699);

      // 取魔法伤害值
      DAMAGE = DAMAGE * cnt;
#ifdef _PROFESSION_ADDSKILL
      dec_hp =
          PROFESSION_MAGIC_GET_DAMAGE(char_index, char_index, 1, DAMAGE, 0);
#else
      dec_hp = PROFESSION_MAGIC_GET_DAMAGE(char_index, char_index, 1, DAMAGE);
#endif
      hp = old_hp - dec_hp;

      if (hp <= 0) {
        CHAR_setInt(char_index, CHAR_HP, 0);
        CHAR_setFlg(char_index, CHAR_ISDIE, 1);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      } else {
        CHAR_setInt(char_index, CHAR_HP, hp);
      }

      sprintf(szCommand, "BD|r%X|0|0|%X|", defNo, dec_hp);
      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORK_I_ENCLOSE: // 冰附体
    {
      int old_dex = CHAR_getInt(char_index, CHAR_DEX);
      int dex = 0;
      int ToList[SIDE_OFFSET * 2 + 1];
      int defNo = BATTLE_Index2No(battle_index, char_index);

      memset(ToList, -1, sizeof(ToList));
      BATTLE_MultiList(battle_index, defNo, ToList);
      BATTLE_MagicEffect(battle_index, defNo, ToList, 27692, 101700);

      dex = old_dex * 0.9;
      CHAR_setWorkInt(char_index, CHAR_WORKFIXDEX, dex);
      sprintf(szCommand, "BD|r%X|0|6|%X|", defNo,
              (-1) * (int)((old_dex * 0.1) / 100));
      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORKRESIST_F: // 火抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(char_index, CHAR_WORK_F_RESIST);
        add_value = CHAR_getWorkInt(char_index, CHAR_WORKMODRESIST_F);

        CHAR_setWorkInt(char_index, CHAR_WORK_F_RESIST, old_value - add_value);
        CHAR_talkToCli(char_index, -1, "火抗性恢复", CHAR_COLORYELLOW);
      }
      break;
    }
    case CHAR_WORKRESIST_I: // 冰抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(char_index, CHAR_WORK_I_RESIST);
        add_value = CHAR_getWorkInt(char_index, CHAR_WORKMODRESIST_I);

        CHAR_setWorkInt(char_index, CHAR_WORK_I_RESIST, old_value - add_value);
        CHAR_talkToCli(char_index, -1, "冰抗性恢复", CHAR_COLORYELLOW);
      }

      break;
    }
    case CHAR_WORKRESIST_T: // 雷抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(char_index, CHAR_WORK_T_RESIST);
        add_value = CHAR_getWorkInt(char_index, CHAR_WORKMODRESIST_T);

        CHAR_setWorkInt(char_index, CHAR_WORK_T_RESIST, old_value - add_value);
        CHAR_talkToCli(char_index, -1, "雷抗性恢复", CHAR_COLORYELLOW);
      }

      break;
    }
#ifdef _PROFESSION_ADDSKILL
    case CHAR_WORKFEAR:
      if (cnt <= 0)
        CHAR_talkToCli(char_index, -1, "攻、防、敏恢复", CHAR_COLORYELLOW);
      break;
#endif
    case CHAR_WORKINSTIGATE: // 挑拨
    {
      int side, pos, defNo, lop, rate = 0;
      if (RAND(1, 100) > 80) {
        break;
      }
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE);

      rate = CHAR_getWorkInt(char_index, CHAR_WORKMODINSTIGATE);

      CHAR_setWorkInt(
          char_index, CHAR_WORKFIXSTR,
          (CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR) * (100 - rate) / 100));
      CHAR_setWorkInt(char_index, CHAR_WORKFIXTOUGH,
                      (CHAR_getWorkInt(char_index, CHAR_WORKFIXTOUGH) *
                       (100 - rate) / 100));
      CHAR_setWorkInt(
          char_index, CHAR_WORKFIXDEX,
          (CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX) * (100 - rate) / 100));

      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battle_index, defNo) == TRUE) {
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, -1);
      }
      break;
    }
    case CHAR_WORKSIGN: // 一针见血
    {
      // 吸血
      int skill_level = CHAR_getWorkInt(char_index, CHAR_WORKMODSIGN);
      int old_hp = CHAR_getInt(char_index, CHAR_HP);
      int old_mp = CHAR_getInt(char_index, CHAR_MP);
      int to_index = CHAR_getWorkInt(char_index, CHAR_WORKSIGNID);
      int to_old_hp = CHAR_getInt(to_index, CHAR_HP);
      int to_old_mp = CHAR_getInt(to_index, CHAR_MP);
      int dec_hp = 0, dec_mp = 0, rate = 0, hp = 0, mp = 0, add_hp = 0,
          add_mp = 0;

      if (old_hp <= 0)
        break;

#ifdef _PROFESSION_ADDSKILL
      if (skill_level >= 10) {
        dec_hp = 200;
        dec_mp = 30;
      } else if (skill_level > 6) {
        dec_hp = 150;
        dec_mp = 20;
      } else if (skill_level > 3) {
        dec_hp = 100;
        dec_mp = 15;
      } else {
        dec_hp = 50;
        dec_mp = 10;
      }
      // 增加hp
      if (skill_level > 8)
        rate = 100;
      else if (skill_level > 7)
        rate = 50;
      else
        rate = 0;
      add_hp = dec_hp * rate / 100;

      // 增加mp
      if (skill_level > 8)
        rate = 100;
      else
        rate = 0;
      add_mp = dec_mp * rate / 100;

#else
      if (skill_level >= 10) {
        dec_hp = 300;
        dec_mp = 70;
      } else if (skill_level >= 9) {
        dec_hp = 200;
        dec_mp = 50;
      } else if (skill_level >= 7) {
        dec_hp = 150;
        dec_mp = 30;
      } else if (skill_level >= 4) {
        dec_hp = 100;
        dec_mp = 20;
      } else {
        dec_hp = 50;
        dec_mp = 10;
      }
      // 增加hp
      if (skill_level >= 9)
        rate = 80;
      else if (skill_level >= 7)
        rate = (skill_level - 1) * 10;
      else if (skill_level >= 4)
        rate = (skill_level - 2) * 10;
      else
        rate = skill_level * 10;
      add_hp = dec_hp * rate / 100;

      // 增加mp
      if (skill_level >= 9)
        rate = 100;
      else if (skill_level >= 7)
        rate = (skill_level - 1) * 10;
      else if (skill_level >= 4)
        rate = (skill_level - 2) * 10;
      else
        rate = skill_level * 10;
      add_mp = dec_mp * rate / 100;
#endif

      // 中嗜血蛊的人减 HP
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        mp = old_mp - dec_mp;
        if (mp < 0) {
          mp = 0;
          add_mp = 0;
        }
        CHAR_setInt(char_index, CHAR_MP, mp);
      }

      // 中嗜血蛊的人减 HP
      if (ridepet == -1) {
        hp = old_hp - dec_hp;
        if (hp < 0)
          hp = 0;
        CHAR_setInt(char_index, CHAR_HP, hp);
        sprintf(szCommand, "BD|r%X|0|0|%X|m%X|", bid, dec_hp, dec_mp);
      } else {
        int pet_old_hp = CHAR_getInt(ridepet, CHAR_HP);
        int pet_hp = 0;

        dec_hp /= 2;
        hp = old_hp - dec_hp;
        pet_hp = pet_old_hp - dec_hp;

        if (hp < 0)
          hp = 0;
        if (pet_hp < 0)
          pet_hp = 0;

        CHAR_setInt(char_index, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pet_hp);
        if (pet_hp <= 0) {
          CHAR_setFlg(ridepet, CHAR_ISDIE, 1);
          CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
        sprintf(szCommand, "BD|r%X|0|0|%X|m%X|p%X|", bid, dec_hp, dec_mp,
                dec_hp);
      }

      if (hp <= 0) {
        CHAR_setFlg(char_index, CHAR_ISDIE, 1);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }

      BATTLESTR_ADD(szCommand);

      // 使用嗜血蛊的人加 HP、MP
      if ((add_hp > 0) && (to_old_hp > 0)) {
        int to_bid = 0, to_hp = 0, to_mp = 0;

        to_bid = BATTLE_Index2No(battle_index, to_index);
        sprintf(szCommand, "BD|r%X|0|1|%X|m%X|", to_bid, add_hp, add_mp);
        BATTLESTR_ADD(szCommand);

        to_hp = to_old_hp + add_hp;
        if (to_hp > CHAR_getWorkInt(to_index, CHAR_WORKMAXHP))
          to_hp = CHAR_getWorkInt(to_index, CHAR_WORKMAXHP);

        to_mp = to_old_mp + add_mp;
        if (to_mp > CHAR_getWorkInt(to_index, CHAR_WORKMAXMP))
          to_mp = CHAR_getWorkInt(to_index, CHAR_WORKMAXMP);

        CHAR_setInt(to_index, CHAR_HP, to_hp);
        CHAR_setInt(to_index, CHAR_MP, to_mp);
      }

      break;
    }
    case CHAR_WORKBLOODWORMS: // 嗜血蛊
    {
      // 吸血
      int skill_level = CHAR_getWorkInt(char_index, CHAR_WORKMODBLOODWORMS);
      int old_hp = CHAR_getInt(char_index, CHAR_HP);
      int to_index = CHAR_getWorkInt(char_index, CHAR_WORKBLOODWORMSID);
      int to_old_hp = CHAR_getInt(to_index, CHAR_HP);
      int dec_hp = 0, rate = 0, hp = 0, add_hp = 0;

      skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

      if (old_hp <= 0)
        break;

      dec_hp = skill_level * 10 + 30;
      if (skill_level >= 10)
        rate = 20;
      else if (skill_level >= 7)
        rate = 15;
      else if (skill_level >= 5)
        rate = 10;
      else
        rate = 5;

      add_hp = dec_hp * rate / 100;

      // 中嗜血蛊的人减 HP
      if (ridepet == -1) {
        hp = old_hp - dec_hp;
        if (hp < 0)
          hp = 0;

        CHAR_setInt(char_index, CHAR_HP, hp);
        sprintf(szCommand, "BD|r%X|0|0|%X|", bid, dec_hp);
      } else {
        int pet_old_hp = CHAR_getInt(ridepet, CHAR_HP);
        int pet_hp = 0;

        dec_hp /= 2;
        hp = old_hp - dec_hp;
        pet_hp = pet_old_hp - dec_hp;

        if (hp < 0)
          hp = 0;
        if (pet_hp < 0)
          pet_hp = 0;

        CHAR_setInt(char_index, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pet_hp);
        if (pet_hp == 0) {
          CHAR_setFlg(ridepet, CHAR_ISDIE, 1);
          CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
        sprintf(szCommand, "BD|r%X|0|0|%X|p%X|", bid, dec_hp, dec_hp);
      }

      if (hp <= 0) {
        CHAR_setFlg(char_index, CHAR_ISDIE, 1);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }

      BATTLESTR_ADD(szCommand);

      // 使用嗜血蛊的人加 HP
      if ((add_hp > 0) && (to_old_hp > 0)) {
        int to_bid = 0, to_hp = 0;

        to_bid = BATTLE_Index2No(battle_index, to_index);
        sprintf(szCommand, "BD|r%X|0|1|%X|", to_bid, add_hp);
        BATTLESTR_ADD(szCommand);

        to_hp = to_old_hp + add_hp;
        if (to_hp > CHAR_getWorkInt(to_index, CHAR_WORKMAXHP))
          to_hp = CHAR_getWorkInt(to_index, CHAR_WORKMAXHP);

        CHAR_setInt(to_index, CHAR_HP, to_hp);
      }

      break;
    }
    case CHAR_WORKICEARROW: // 冰箭
    {
      // 减敏
      int dec_dex = CHAR_getWorkInt(char_index, CHAR_WORKMODICEARROW);
      int old_dex = CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX);
      int dex = 0;

      dex = old_dex * (100 - dec_dex) / 100;
      CHAR_setWorkInt(char_index, CHAR_WORKFIXDEX, dex);
      sprintf(szCommand, "BD|r%X|0|6|%X|", bid, (-1) * dec_dex);

      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORKICECRACK: // 冰爆术
    {
      int value = 0;
      int defNo = CHAR_getWorkInt(
          char_index, CHAR_WORKBATTLECOM2); // defNo = BATTLE_Index2No(
                                            // battle_index, char_index );

      value = CHAR_getWorkInt(char_index, CHAR_WORKMODICECRACK);

      if (cnt <= 0 && value > 0) {
        int damage = 0, petdamage = 0;

        int ToList[SIDE_OFFSET * 2 + 1];
        memset(ToList, -1, sizeof(ToList));
        BATTLE_MultiList(battle_index, TARGET_SIDE_1, ToList);
        // 将魔法参数代入动画
        analysis_profession_parameter(2, 4, defNo, char_index);
        PROFESSION_MAGIC_ATTAIC_Effect(battle_index, bid, ToList, 2);

        if (defNo == 20 || defNo == 25 || defNo == 26) // 右
          bid = 0;
        else
          bid = 10;

        for (i = bid; i < bid + 10; i++) {
          int defType, charaidx, petidx;
          charaidx = BATTLE_No2Index(battle_index, i);
          if (CHAR_CHECKINDEX(charaidx)) {
            if (CHAR_getInt(charaidx, CHAR_HP) > 0) {
              petidx = BATTLE_getRidePet(charaidx);
              defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
              CHAR_setWorkInt(charaidx, CHAR_WORKICECRACK, 0);
              // BATTLE_BadStatusString( bid, 0 );
              // snprintf( szBuffer, sizeof( szBuffer ), "(%s)将(%s)修理完毕。",
              //	CHAR_getUseName( char_index ), aszStatusFull[i] );
              //	BATTLE_BroadCast( battle_index, szBuffer,
              //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
              // sprintf( szBuffer, "BM|%X|%X|", bid, 0 );
              // BATTLESTR_ADD( szBuffer );

              damage = value;

              hp = CHAR_getInt(charaidx, CHAR_HP);
              if (petidx == -1) {
                hp -= damage;
                if (hp < 0)
                  hp = 0;
              } else {
                int defquick = 0, petquick = 0, playerdamage = 0;
                pethp = CHAR_getInt(petidx, CHAR_HP);
                defquick = CHAR_getWorkInt(charaidx, CHAR_WORKDEFENCEPOWER);
                petquick = CHAR_getWorkInt(petidx, CHAR_WORKDEFENCEPOWER);
                if (pethp > 0) {
                  playerdamage =
                      (damage * petquick) / (defquick + petquick) + 1;
                  petdamage = damage - playerdamage + 1;
                  hp -= playerdamage;
                  pethp -= petdamage;
                } else {
                  hp -= damage;
                }
                if (hp < 0)
                  hp = 0;
                if (pethp < 0)
                  pethp = 0;
                CHAR_setInt(petidx, CHAR_HP, pethp);
              }

              CHAR_setInt(charaidx, CHAR_HP, hp);

              if (hp <= 0) {
                CHAR_setFlg(charaidx, CHAR_ISDIE, 1);
                CHAR_setWorkInt(charaidx, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
              }
              if (pethp <= 0) {
                CHAR_setFlg(petidx, CHAR_ISDIE, 1);
                CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
              }

              if (damage >= 0) {
                // snprintf( szBuffer, sizeof( szBuffer ),
                //	"(%s)因(%s)受到(%d)的损伤。",
                //	CHAR_getUseName( char_index ),
                //	aszStatusFull[i],
                //	damage );
                // BATTLE_BroadCast( battle_index, szBuffer,
                //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

                sprintf(szBuffer, "%X|%X|%X|%X|", i, i, (int)(damage),
                        (int)(petdamage));

                // sprintf( szBuffer, "BD|r%X|0|0|%X|p%X|", i, (int)(damage),
                // (int)(petdamage) );
                BATTLESTR_ADD(szBuffer);
              }
              if (petidx != -1 && petdamage >= 0) {
                // snprintf( szBuffer, sizeof( szBuffer ),
                //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
                //	CHAR_getUseName( char_index ),
                //	aszStatusFull[i], petdamage );
                // BATTLE_BroadCast( battle_index, szBuffer,
                //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
              }
            }
          }
        }
        sprintf(szBuffer, "%X|", 0x5711438);
        BATTLESTR_ADD(szBuffer);
      }
    }
      print("\n冰爆术");
      break;
#ifdef _PROFESSION_ADDSKILL
    case CHAR_WORKICECRACK2: // 冰爆术2
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK2);
      print("\n冰爆术2");
      break;
    case CHAR_WORKICECRACK3: // 冰爆术3
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK3);
      print("\n冰爆术3");
      break;
    case CHAR_WORKICECRACK4: // 冰爆术4
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK4);
      print("\n冰爆术4");
      break;
    case CHAR_WORKICECRACK5: // 冰爆术5
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK5);
      print("\n冰爆术5");
      break;
    case CHAR_WORKICECRACK6: // 冰爆术6
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK6);
      print("\n冰爆术6");
      break;
    case CHAR_WORKICECRACK7: // 冰爆术7
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK7);
      print("\n冰爆术7");
      break;
    case CHAR_WORKICECRACK8: // 冰爆术8
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK8);
      print("\n冰爆术8");
      break;
    case CHAR_WORKICECRACK9: // 冰爆术9
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK9);
      print("\n冰爆术9");
      break;
    case CHAR_WORKICECRACK10: // 冰爆术10
      WorkIceCrackPlay(char_index, cnt, CHAR_WORKICECRACK10);
      print("\n冰爆术10");
      break;
#endif //_PROFESSION_ADDSKILL

#endif

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case CHAR_WORKSARS: {
      int j;
      int rate = 60; // 传染 SARS 机率

      // 受传染的人受伤
      {
        int petdown = -1;
        int dec_mp = 0;

        Compute_Down_SARS(char_index, ridepet, &down, &petdown, ridepet);
        if (down >= 0) {
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( char_index ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battle_index, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

          // 如果是玩家扣 MP
          if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
            int old_mp = CHAR_getInt(char_index, CHAR_MP);
            dec_mp = old_mp / 10;
            if (dec_mp < 0)
              dec_mp = 0;
            CHAR_setInt(char_index, CHAR_MP, old_mp - dec_mp);
            sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|m%X|", bid, (int)(down),
                    (int)(petdown), dec_mp);
          } else
            sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                    (int)(petdown));

          BATTLESTR_ADD(szBuffer);
        }
        if (ridepet != -1 && petdown >= 0) {
        }
      }

      if (CHAR_getWorkInt(char_index, CHAR_WORKMODSARS) > 0) {
        int buf2[5], ti = 0;
        int tibuf[10] = {3, 1, 0, 2, 4, 8, 6, 5, 7, 9};

        memset(buf2, -1, sizeof(buf2));

        if (defNo > 9) {
          for (j = 0; j < 10; j++) {
            tibuf[j] += 10;
          }
        }

        for (j = 0; j < 10; j++) {
          if (tibuf[j] != defNo)
            continue;
          if (j > 4) {
            if ((j + 1) < 10)
              buf2[ti++] = tibuf[j + 1];
            if ((j - 1) > 4)
              buf2[ti++] = tibuf[j - 1];
            // 不等边
            if ((j - 5 + 1) < 5)
              buf2[ti++] = tibuf[j - 5 + 1];
            if ((j - 5 - 1) >= 0)
              buf2[ti++] = tibuf[j - 5 - 1];
            if ((j - 5) >= 0)
              buf2[ti++] = tibuf[j - 5];
          } else {
            if ((j + 1) < 5)
              buf2[ti++] = tibuf[j + 1];
            if ((j - 1) >= 0)
              buf2[ti++] = tibuf[j - 1];
            // 不等边
            if ((j + 5 + 1) < 10)
              buf2[ti++] = tibuf[j + 5 + 1];
            if ((j + 5 - 1) > 4)
              buf2[ti++] = tibuf[j + 5 - 1];
            if ((j + 5) < 10)
              buf2[ti++] = tibuf[j + 5];
          }
          break;
        }

        print("buf2[%d,%d,%d,%d,%d]\n", buf2[0], buf2[1], buf2[2], buf2[3],
              buf2[4]);

        for (j = 0; j < 5; j++) {

          int toindex = 0;
          if (buf2[j] == -1)
            continue;

          toindex = BATTLE_No2Index(battle_index, buf2[j]);
          if (!CHAR_CHECKINDEX(toindex))
            continue;
          // 得到sars的离开
          if (CHAR_getWorkInt(toindex, CHAR_WORKSARS) > 0)
            continue;

          if (RAND(1, 100) <= rate) { // 中 SARS
            if (toindex == char_index)
              continue;
            if (CHAR_getInt(toindex, CHAR_HP) <= 0)
              continue;

            CHAR_setWorkInt(toindex, CHAR_WORKSARS, 3);
          }
        }
      }
    } break;
#endif
    default:
      break;
    }
  }

#ifdef _SUIT_ITEM
  {
    int defCode;
    int AddHp = 0, AddMp = 0;
#ifdef _IMPRECATE_ITEM
    int nums1, nums2, nums3, delMp = 0;
#endif
    if ((defCode = CHAR_getWorkInt(char_index, CHAR_WORKSUITITEM)) > 0) {
      AddHp = CHAR_getWorkInt(char_index, CHAR_WORKROUNDHP);
      AddMp = CHAR_getWorkInt(char_index, CHAR_WORKROUNDMP);
    }

#ifdef _IMPRECATE_ITEM
    if ((nums1 = CHAR_getWorkInt(char_index, CHAR_WORKIMPRECATENUM1)) > 0) {
      delMp = CHAR_getWorkInt(char_index, CHAR_WORKHURTMP);
      CHAR_setWorkInt(char_index, CHAR_WORKIMPRECATENUM1, nums1 - 1);
    }
    if ((nums2 = CHAR_getWorkInt(char_index, CHAR_WORKIMPRECATENUM2)) > 0) {
      AddHp += CHAR_getWorkInt(char_index, CHAR_WORKWISHESHP);
      CHAR_setWorkInt(char_index, CHAR_WORKIMPRECATENUM2, nums2 - 1);
    }
    if ((nums3 = CHAR_getWorkInt(char_index, CHAR_WORKIMPRECATENUM3)) > 0) {
      AddMp += CHAR_getWorkInt(char_index, CHAR_WORKWISHESMP);
      CHAR_setWorkInt(char_index, CHAR_WORKIMPRECATENUM3, nums3 - 1);
    }
    AddMp = AddMp - delMp;
#endif
    if (AddHp != 0 || AddMp != 0) {
      int AddPHp = 0;

#ifdef _TYPE_TOXICATION
      if (AddHp != 0 && CHAR_CanCureFlg(char_index, "FALSE") != FALSE) {
#else
      if (AddHp != 0) {
#endif
        int MyHp = CHAR_getInt(char_index, CHAR_HP);
        int MaxHp = CHAR_getWorkInt(char_index, CHAR_WORKMAXHP);
        MyHp = ((MyHp + AddHp) >= MaxHp) ? MaxHp : (MyHp + AddHp);
        MyHp = (MyHp <= 1) ? 1 : MyHp;
        CHAR_setInt(char_index, CHAR_HP, MyHp);
        if (ridepet != -1) {
          int PetMaxHp = CHAR_getWorkInt(ridepet, CHAR_WORKMAXHP);
          int PetHp = CHAR_getInt(ridepet, CHAR_HP);
          AddPHp = AddHp;
          PetHp = ((PetHp + AddPHp) >= PetMaxHp) ? PetMaxHp : (PetHp + AddPHp);
          PetHp = (PetHp <= 1) ? 1 : PetHp;
          CHAR_setInt(ridepet, CHAR_HP, PetHp);
        }
      }
      if (AddMp != 0) {
        int MyMp = CHAR_getInt(char_index, CHAR_MP);
        int MaxMp = CHAR_getWorkInt(char_index, CHAR_WORKMAXMP);
        MyMp = ((MyMp + AddMp) > MaxMp) ? MaxMp : (MyMp + AddMp);
        MyMp = (MyMp < 0) ? 0 : MyMp;
        CHAR_setInt(char_index, CHAR_MP, MyMp);
        sprintf(szBuffer, "BD|r%X|0|2|%X|p%X|m%X|", bid, AddHp, AddPHp, AddMp);
      } else {
        sprintf(szBuffer, "BD|r%X|0|2|%X|p%X|", bid, AddHp, AddPHp);
      }
      BATTLESTR_ADD(szBuffer);
    }
  }
#endif
#ifdef _PETSKILL_SETDUCK
  {
    int turns = 0, nums = 0;
    if ((turns = CHAR_getWorkInt(char_index, CHAR_MYSKILLDUCK)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(char_index, CHAR_MYSKILLDUCKPOWER);
        sprintf(szBuffer, "BD|r%X|0|3|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(char_index, CHAR_MYSKILLDUCK, turns);
    }
#ifdef _MAGICPET_SKILL
    if ((turns = CHAR_getWorkInt(char_index, CHAR_MYSKILLSTR)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(char_index, CHAR_MYSKILLSTRPOWER);
        sprintf(szBuffer, "BD|r%X|0|4|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(char_index, CHAR_MYSKILLSTR, turns);
    }
    if ((turns = CHAR_getWorkInt(char_index, CHAR_MYSKILLTGH)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(char_index, CHAR_MYSKILLTGHPOWER);
        sprintf(szBuffer, "BD|r%X|0|5|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(char_index, CHAR_MYSKILLTGH, turns);
    }
    if ((turns = CHAR_getWorkInt(char_index, CHAR_MYSKILLDEX)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(char_index, CHAR_MYSKILLDEXPOWER);
        sprintf(szBuffer, "BD|r%X|0|6|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(char_index, CHAR_MYSKILLDEX, turns);
    }
#ifdef _PROFESSION_SKILL // WON ADD
    if ((turns = CHAR_getWorkInt(char_index, CHAR_MYSKILLHIT)) > 0) {
      turns--;
      if (turns == 0) {
        int dec_num = CHAR_getWorkInt(char_index, CHAR_MYSKILLHIT_NUM);
        nums = CHAR_getWorkInt(char_index, CHAR_WORKHITRIGHT);
        nums -= dec_num;
        CHAR_setWorkInt(char_index, CHAR_WORKHITRIGHT, nums);

        CHAR_talkToCli(char_index, -1, "恢复命中率", CHAR_COLORYELLOW);
      }
      CHAR_setWorkInt(char_index, CHAR_MYSKILLHIT, turns);
    }
#endif

#endif
  }
#endif
  return 0;
}

BOOL BATTLE_CanMoveCheck(int char_index) {

  //
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARALYSIS) > 0) {
    return FALSE;
  }

  // 檗祭
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTONE) > 0) {
    return FALSE;
  }
  // 眠り  (睡眠)
  if (CHAR_getWorkInt(char_index, CHAR_WORKSLEEP) > 0) {
    return FALSE;
  }
#ifdef _MAGIC_BARRIER
  if (CHAR_getWorkInt(char_index, CHAR_WORKBARRIER) > 0) {
    return FALSE;
  }
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  // 晕眩
  if (CHAR_getWorkInt(char_index, CHAR_WORKDIZZY) > 0) {
    return FALSE;
  }
  // 天罗地网
  if (CHAR_getWorkInt(char_index, CHAR_WORKDRAGNET) > 0) {
    return FALSE;
  }
  /*
          // 冰箭术
          if( CHAR_getWorkInt( char_index, CHAR_WORKICEARROW ) > 0 ){
                  return FALSE;
          }
  */
  /*
          // 专注战斗
          if( CHAR_getWorkInt( char_index, CHAR_WORK_FOCUS ) > 0 ){
                  return FALSE;
          }
  */
  // 雷附体
  if (CHAR_getWorkInt(char_index, CHAR_WORK_T_ENCLOSE) > 0) {
    return FALSE;
  }
#endif
#ifdef _PROFESSION_ADDSKILL
  if (CHAR_getWorkInt(char_index, CHAR_DOOMTIME) > 0) {
    return FALSE;
  }
#endif
  return TRUE;
}

#ifdef _ITEM_EQUITSPACE
int BATTLE_GetEqShield(int char_index) {
  int iRet = ITEM_FIST, item_index;

  item_index = CHAR_getItemIndex(char_index, CHAR_EQSHIELD);
  if (ITEM_CHECKINDEX(item_index) == FALSE) {
    iRet = ITEM_FIST;
  } else {
    iRet = ITEM_getInt(item_index, ITEM_TYPE);
  }
  return iRet;
}
#endif

int BATTLE_GetWepon(int char_index) //取得角色手持武器
{
  int iRet = ITEM_FIST, item_index;

  item_index = CHAR_getItemIndex(char_index, CHAR_ARM);
  if (ITEM_CHECKINDEX(item_index) == FALSE) {
    iRet = ITEM_FIST;
  } else {
    iRet = ITEM_getInt(item_index, ITEM_TYPE);
  }

  return iRet;
}

static int aBowW[50] = {
    0, 2, 1, 4, 3, 0, 1, 2, 3, 4, 1, 0, 3, 2, 4, 1, 3, 0, 2, 4, 2, 4, 0, 1, 3,
    2, 0, 4, 1, 3, 3, 1, 0, 2, 4, 3, 1, 0, 2, 4, 4, 2, 0, 1, 3, 4, 2, 0, 1, 3,
};

void BATTLE_TargetListSet(int char_index, int attackNo, int *pList) {
  int i, j = 0, defNo, battle_index = -1;
  defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
  battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);

#ifdef _PREVENT_TEAMATTACK
#ifdef _SKILLLIMIT // (可开放) Syu ADD 不得攻击我方限制
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_FALLRIDE || // 落马
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_WILDVIOLENTATTACK || //狂暴攻击
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_TIMID || // 怯战
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_MPDAMAGE || // MP攻击
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_WEAKEN || // 虚弱
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_DAMAGETOHP || // 嗜血
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_STEALMONEY) { // 捐献
    if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      //			BATTLE_NoAction( battle_index, attackNo );
      return;
    }
  }
#endif
#endif

  for (i = 0; i < BATTLE_ENTRY_MAX * 2; i++) {
    pList[i] = defNo;
  }
#ifdef _BATTLE_ATTCRAZED // ANDY 疯狂暴走
#ifdef _SHOOTCHESTNUT    // Syu ADD 宠技：丢栗子
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_ATTCRAZED ||
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_ATTSHOOT) {
#else
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
      BATTLE_COM_S_ATTCRAZED) {
#endif
    int deftop, defsub;
    int plive[10];
    int n = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);

#ifdef _PREVENT_TEAMATTACK
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      BATTLE_NoAction(battle_index, attackNo);
      return;
    }
#endif
#endif
    if (0 <= defNo && defNo <= 9) {
      defsub = 0;
      deftop = 9;
    } else if (10 <= defNo && defNo <= 19) {
      defsub = 10;
      deftop = 19;
    } else {
      pList[1] = -1;
      return;
    }
    for (i = defsub; i < deftop; i++) {
      if (BATTLE_TargetCheck(battle_index, i) == FALSE)
        continue;
      plive[j++] = i;
    }
    if (j == 0)
      return;
    for (i = 0; i < n; i++) {
      pList[i] = plive[RAND(0, j - 1)];
    }
    pList[i] = -1;
    return;
  }
#endif
  if (BATTLE_GetWepon(char_index) != ITEM_BOW) {
    return;
  }

  if (0 <= defNo && defNo <= 19) {
    int defsub = defNo % 5;
    int deftop = defNo - defsub;
    int Rnd = RAND(0, 1);
    for (j = 0, i = 0; j < 5; j++) {
      pList[i] = aBowW[defsub * 10 + Rnd * 5 + j] + deftop;
      if (deftop == 0 || deftop == 10) {
        pList[i + 1] = pList[i] + 5;
      } else {
        pList[i + 1] = pList[i] - 5;
      }
      if (pList[i] == attackNo) {
        pList[i] = -1;
      }
      if (pList[i + 1] == attackNo) {
        pList[i + 1] = -1;
      }
      i += 2;
    }
    pList[i] = -1;
  } else {
    pList[1] = -1;
  }

  return;
}

int BATTLE_GetAttackCount(int char_index) {
  int iRet = 1, item_index;
  item_index = CHAR_getItemIndex(char_index, CHAR_ARM);
  if (ITEM_CHECKINDEX(item_index) == FALSE) {
    return 0;
  } else {
    iRet = RAND(ITEM_getInt(item_index, ITEM_ATTACKNUM_MIN),
                ITEM_getInt(item_index, ITEM_ATTACKNUM_MAX));
  }
  if (iRet <= 0)
    iRet = 1;

  return iRet;
}

int BATTLE_PetRandomSkill(int battle_index, int char_index) {
#define PETSKILLSERCHTIME 50
  int i, j, iNum, toNo, k, myNo, side, pskill_array;
  int skill_type;
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
  int pid = CHAR_getInt(char_index, CHAR_PETID);
  int skNum;
#endif

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);

  myNo = BATTLE_Index2No(battle_index, char_index);
  side = CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE);

#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
  iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
  skNum = CHAR_getPetSkill(char_index, iNum);
  if (((pid == 981) || (pid == 982) || (pid == 983) || (pid == 984)) &&
      (skNum == 600)) {
    do {
      iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
      skNum = CHAR_getPetSkill(char_index, iNum);
    } while (skNum == 600);
  }
#else
  iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
#endif

  for (k = 0; k < 3; k++) {
    toNo = BATTLE_DefaultAttacker(battle_index, 1 - side);
    if (toNo == myNo) {

    } else {
      break;
    }
  }

  if (myNo == toNo) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    return 0;
  }

  for (j = 0, i = 0, k = 0; k < PETSKILLSERCHTIME; i++, k++) {
    if (i >= CHAR_MAXPETSKILLHAVE)
      i = 0;

    if (CHAR_getCharHaveSkill(char_index, i) == NULL) {
      continue;
    }

    pskill_array = PETSKILL_GetArray(char_index, i);
    skill_type = PETSKILL_getInt(pskill_array, PETSKILL_FIELD);

    if (skill_type == PETSKILL_FIELD_BATTLE ||
        skill_type == PETSKILL_FIELD_ALL) {
    } else {
      continue;
    }

    if (j < iNum) {
      j++;
      continue;
    }

    if (strcmp(PETSKILL_getChar(pskill_array, PETSKILL_FUNCNAME),
               "PETSKILL_AttackMagic") == 0) {
      continue;
    }

    if (strcmp(PETSKILL_getChar(pskill_array, PETSKILL_FUNCNAME),
               "PETSKILL_Combined") == 0) {
      continue;
    }

    if (PETSKILL_Use(char_index, iNum, toNo, NULL) == TRUE) {

    } else {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
    break;
  }

  if (k >= PETSKILLSERCHTIME) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  }
  return 0;
}

enum {
  PETAI_MODE_NORMAL,
  PETAI_MODE_TARGETRANDOM,
  PETAI_MODE_RANDOMACT,
  PETAI_MODE_OWNERATTACK,
  PETAI_MODE_ENEMYATTACK,
  PETAI_MODE_ESCAPE,
  PETAI_MODE_NOACT,
  PETAI_MODE_END
};

static int BATTLE_PetLoyalCheck(int battle_index, int bid, int char_index) {
  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return FALSE;

  int ai, toNo, mode = 0, type, Rand, toSide = 1, flg, i;
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    return 0;
  }
  if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE)
    return 0;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) == BATTLE_COM_WAIT) {
    return 0;
  }
  flg = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG);
  flg &= ~CHAR_BATTLEFLG_AIBAD;
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, flg);
  if (BATTLE_CanMoveCheck(char_index) == FALSE) {
    return 0;
  }
  toNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
  toSide = (0 <= toNo && toNo < BATTLE_ENTRY_MAX) ? (0) : (1);
  ai = CHAR_getWorkInt(char_index, CHAR_WORKFIXAI);
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) == BATTLE_COM_GUARD ||
      bid == toNo) {
    type = 1;
  } else {
    type = 0;
  }

  Rand = RAND(1, 100);

  mode = 0;                   // 未設定 (初始未设)
  if (ai >= 80) {             // 忠誠80以上 (忠诚80以上)
    mode = PETAI_MODE_NORMAL; // 通常行動 (正常行动)
  } else if (ai >= 70) {      // 忠誠70以上 (忠诚70以上)
    if (Rand < 10)
      mode = PETAI_MODE_TARGETRANDOM; // ターゲット (目标)
  } else if (ai >= 60) {
    if (Rand < 20)
      mode = PETAI_MODE_TARGETRANDOM; // ターゲット (目标)
  } else if (ai >= 50) {
    if (Rand < 35)
      mode = PETAI_MODE_TARGETRANDOM; // ターゲット (目标)
  } else if (ai >= 40) {
    if (Rand < 50)
      mode = PETAI_MODE_TARGETRANDOM; // ターゲット (目标)
  } else if (ai >= 30) {
    if (Rand < 70)
      mode = PETAI_MODE_RANDOMACT; // ランダム (随机)
  } else if (ai >= 20) {
    if (Rand < 70)
      mode = PETAI_MODE_RANDOMACT; // ランダム (随机)
  } else if (ai >= 10) {
    if (Rand < 80) {
      mode = PETAI_MODE_OWNERATTACK; // 主人 攻撃 (攻击主人)
    } else {
      mode = PETAI_MODE_ENEMYATTACK; // 敵 攻撃 (攻击敌人)
    }
  } else {
    if (Rand < 60) {
      mode = PETAI_MODE_OWNERATTACK; // 主人 攻撃 (攻击主人)
    } else {
      mode = PETAI_MODE_ESCAPE; //   逃げ (逃跑)
    }
  }

  if (mode == PETAI_MODE_NORMAL)
    return 0;

  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    if (CHAR_getCharHaveSkill(char_index, i)) {
      break;
    }
  }
  if (i >= CHAR_MAXPETSKILLHAVE) {
    mode = PETAI_MODE_NOACT;
  }
  flg = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG);
  flg |= CHAR_BATTLEFLG_AIBAD;
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, flg);

  switch (mode) {
  case PETAI_MODE_NOACT:
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    break;
  case PETAI_MODE_TARGETRANDOM:
    if (type == 1) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    } else {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2,
                      BATTLE_DefaultAttacker(battle_index, toSide));
    }
    break;
  case PETAI_MODE_RANDOMACT: // ランダム (随机行动)
    // 回避技 の 前は 通常に戻す (回避技前恢复普通)
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
        BATTLE_COM_S_EARTHROUND0) {
      return 0;
    }

    // ガーディアンフラグを はずす (清除守护标志)
    flg = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG);
    flg &= ~CHAR_BATTLEFLG_GUARDIAN;
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEFLG, flg);

    if (toNo == bid) { // 戦闘を 回避したら (若自身为目标则回避)
      // 何も できないようにする (设为无法行动)
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    } else {
      // ランダムに 技をセット (随机选择技能)
      BATTLE_PetRandomSkill(battle_index, char_index);
    }
    break;

  case PETAI_MODE_OWNERATTACK: // 主人 攻撃 (攻击主人)
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, bid - 5);
    break;

  case PETAI_MODE_ENEMYATTACK: // 敵 攻撃 (攻击敌人)
  {
    int myside = 0;
    if (bid >= BATTLE_ENTRY_MAX)
      myside = 1;
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    // 敵サイドに (对敌方阵营)
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2,
                    BATTLE_DefaultAttacker(battle_index, 1 - myside));
  } break;

  case PETAI_MODE_ESCAPE: //   逃げ の セット (设置逃跑)
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_S_LOSTESCAPE);
    break;
  }

  return 1;
}

#ifdef _ATTACK_MAGIC

#define MAX_MAGIC_NUMBER 25

// 每一种魔法的攻击  围 (id,area)
int TargetIndex[MAX_MAGIC_NUMBER][2] = {
    {301, -1}, {302, -1}, {303, 26}, {304, -1}, {305, 20}, {306, 20}, {307, -1},
    {308, -1}, {309, -1}, {310, -1}, {311, 26}, {312, 20}, {313, -1}, {314, -1},
    {315, -1}, {316, -1}, {317, 26}, {318, 20}, {319, -1}, {320, -1}, {321, 26},
    {322, -1}, {323, 26}, {324, 20}, {325, 20}};

int magic, toindex, magic_count;

#endif


// 
const static char *aszFieldAttr[] = {"无", "地", "水", "火", "风"};
RoundActionListEntry sRoundActionList[40];

static int BATTLE_Battling(int battle_index) {
  BATTLE *pBattle, *pWatchBattle;
  BATTLE_ENTRY *pEntry[2];
  char szBuffer[256] = "", szWork[256];
  int i, j, k, len;
  int char_index, item_index; // 人物，物品
  int array, skill; // 后面技能专用的变量
  int canCountineCounter; // 攻击行为是否继续
  int attackNo, defNo;
  int aAttackList[BATTLE_ENTRY_MAX * 2 + 1];
  int aDefList[BATTLE_ENTRY_MAX * 2 + 1];
  int AllSize = 0;
  if (BATTLE_CHECKINDEX(battle_index) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battle_index];
  pEntry[0] = pBattle->Side[0].Entry;
  pEntry[1] = pBattle->Side[1].Entry;

  for (i = 0; i < arraysizeof(sRoundActionList); i++) {
    sRoundActionList[i].char_index = -1;
    sRoundActionList[i].combo = 0;
  }
  sprintf(szBuffer, "战役 %d 转换 %d 属性 %s", battle_index, pBattle->turn,
          aszFieldAttr[pBattle->field_att]);

  int actionNum = 0;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry[j][i].flg &= ~BENT_FLG_ULTIMATE;
      if (!CHAR_CHECKINDEX(pEntry[j][i].char_index))
        continue;
      sRoundActionList[actionNum].char_index = pEntry[j][i].char_index;
      sRoundActionList[actionNum].side = j;
      sRoundActionList[actionNum].num = pEntry[j][i].bid;
      sRoundActionList[actionNum].dex = BATTLE_DexCalc(pEntry[j][i].char_index);
#ifdef _EQUIT_SEQUENCE
      sRoundActionList[actionNum].sequence =
          CHAR_getWorkInt(pEntry[j][i].char_index, CHAR_WORKSEQUENCEPOWER);
#endif
      BATTLE_talkToCli(pEntry[j][i].char_index, szBuffer, CHAR_COLORYELLOW);
      actionNum++;
    }
  }
  szAllBattleString[0] = 0;
  k = 0;
  for (j = 0; j < 2; j++) {
    k = j * SIDE_OFFSET;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      szBattleString[0] = '\0';
      if (pBattle->iEntryBack[k] >= 0 &&
          pBattle->Side[j].Entry[i].char_index == -1) {
        if (pBattle->iEntryBack2[k] >= 0) {
          sprintf(szBattleString, "BE|et%X|f1|", k);
        } else {
          print("应是输入ID不正确出现的error。\n");
        }
      }
      k++;
      strcat(szAllBattleString, szBattleString);
    }
  }
  len = strlen(szAllBattleString);
  AllSize += len;
  RoundActionSort(sRoundActionList, actionNum);
  ComboSet(sRoundActionList, actionNum);
  for (i = 0; i < actionNum; i++) {
    int COM, myside, otherside, bi, attack_flg = 1, attack_max = 1,
                                    attack_count = 0;
    char_index = sRoundActionList[i].char_index;
    myside = sRoundActionList[i].side;
    otherside = 1 - myside;
    attackNo = sRoundActionList[i].num;
    bi = attackNo - myside * SIDE_OFFSET;
    aAttackList[0] = attackNo;
    aAttackList[1] = -1;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE)
      continue;
    szBadStatusString[0] = 0;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
        BATTLE_CHARMODE_C_OK) {
      // sprintf( szBuffer, "指令输入不完全。(%s)", CHAR_getUseName( char_index ) );
      // BATTLE_BroadCast( battle_index, szBuffer, CHAR_COLORYELLOW );
      continue;
    }
    if (CHAR_getInt(char_index, CHAR_HP) <= 0)
      continue;

    szBattleString[0] = 0;
    pszBattleTop = szBattleString;
    pszBattleLast = szBattleString + arraysizeof(szBattleString);
    // WON ADD
    if (CHAR_getFlg(char_index, CHAR_ISDIE) != TRUE)
      BATTLE_StatusSeq(char_index);
#ifdef _OTHER_MAGICSTAUTS
    BATTLE_MagicStatusSeq(char_index);
#endif
#ifdef _MAGIC_DEEPPOISON //   剧毒
    if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    BATTLE_ProfessionStatusSeq(battle_index, char_index);
#endif

    if (BATTLE_CanMoveCheck(char_index) == FALSE) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
    gBattleDamageModyfy = 1.0;
    gBattleDuckModyfy = 0;
    gBattleStausChange = -1;
    gBattleStausTurn = 0;
    gWeponType = ITEM_FIST;
    gWeponType = BATTLE_GetWepon(char_index);
#ifdef _PETSKILL_BECOMEFOX
    if (CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1 //若是变成小狐狸
        || CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101749) {
      int COM = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);
      gWeponType = ITEM_FIST;

      if (!(COM == BATTLE_COM_ATTACK || COM == BATTLE_COM_GUARD ||
            COM == BATTLE_COM_NONE)) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        CHAR_talkToCli(char_index, -1, "变成小狐狸後只能攻击、防御和待机！",
                       CHAR_COLORYELLOW);
      }
    }
#endif
#ifdef _PETSKILL_BECOMEPIG
    if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1 //变成乌力了
        && (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)) {
      int COM = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);
      gWeponType = ITEM_FIST;
      // CHAR_setWorkInt( char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
      if (!(COM == BATTLE_COM_ATTACK || COM == BATTLE_COM_GUARD ||
            COM == BATTLE_COM_NONE || COM == BATTLE_COM_ITEM ||
            COM == BATTLE_COM_ESCAPE || COM == BATTLE_COM_CAPTURE ||
            COM == BATTLE_COM_WAIT || COM == BATTLE_COM_PETIN ||
            COM == BATTLE_COM_PETOUT)) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);
        CHAR_talkToCli(char_index, -1,
                       "变成乌力後不能使用咒术和职业技能！"
                       "ܣ",
                       CHAR_COLORYELLOW);
      }
    }
#endif
    gDamageDiv = 1.0;
    attack_max = BATTLE_GetAttackCount(char_index);
    if (attack_max <= 0) {
      int work, randwork;
      work = CHAR_getInt(char_index, CHAR_LV);
      if (work < 10 ||
          CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
        attack_max = 1;
      } else {
        int luckwork = CHAR_getInt(char_index, CHAR_LUCK) * 5;
        if (luckwork > 5 * 5) {
          print("\nerr:运气奇怪(%s)(%d)\n", CHAR_getUseName(char_index),
                CHAR_getInt(char_index, CHAR_LUCK));
          luckwork = 5 * 5;
        }
        randwork = RAND(1, 1000);
        if (randwork <= 10 + luckwork) {
          attack_max = RAND(5, 10);
        } else if (randwork <= 30 + luckwork) {
          attack_max = 3;
        } else if (randwork <= 70 + luckwork) {
          attack_max = 2;
        } else {
          attack_max = 1;
        }
      }
    } else {
      if (gWeponType == ITEM_FIST)
        gDamageDiv = attack_max;
    }
    if (gWeponType == ITEM_BREAKTHROW) {
      gBattleStausChange = BATTLE_ST_PARALYSIS;
      gBattleStausTurn = 1 - 1;
    }

    if (BattleArray[battle_index].Side[myside].flg & BSIDE_FLG_SURPRISE) {
    } else {
      if (BATTLE_PetLoyalCheck(battle_index, attackNo, char_index)) {
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
        if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101428
#ifdef _EXPANSION_VARY_WOLF
            || CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 104109
#endif
        ) {
          if (CHAR_getWorkInt(char_index, CHAR_WORKTURN) == 0) {
            CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                        CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER));
            CHAR_setWorkInt(char_index, CHAR_WORKATTACKPOWER,
                            CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR));
            CHAR_setWorkInt(char_index, CHAR_WORKDEFENCEPOWER,
                            CHAR_getWorkInt(char_index, CHAR_WORKFIXTOUGH));
            CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                            CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX));
          }
        }
#endif
        sprintf(szWork, "BX|%X|", attackNo);
        BATTLESTR_ADD(szWork);
      }
    }
#ifdef _PROFESSION_ADDSKILL
    if (CHAR_getWorkInt(char_index, CHAR_DOOMTIME) > 0) {
      CHAR_setWorkInt(char_index, CHAR_DOOMTIME,
                      CHAR_getWorkInt(char_index, CHAR_DOOMTIME) - 1);
    }
    if (CHAR_getWorkInt(char_index, CHAR_DOOMTIME) == 0 &&
        (CHAR_getWorkInt(char_index, CHAR_WORK_com1) ==
             BATTLE_COM_S_FIRE_SPEAR ||
         CHAR_getWorkInt(char_index, CHAR_WORK_com1) == BATTLE_COM_S_DOOM)) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1,
                      CHAR_getWorkInt(char_index, CHAR_WORK_com1));
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2,
                      CHAR_getWorkInt(char_index, CHAR_WORK_toNo));
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE,
                      CHAR_getWorkInt(char_index, CHAR_WORK_mode));
      CHAR_SETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3,
                           CHAR_getWorkInt(char_index, CHAR_WORK_skill_level));
      CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3,
                          CHAR_getWorkInt(char_index, CHAR_WORK_array));
      CHAR_setWorkInt(char_index, CHAR_WORK_com1, 0);
      CHAR_setWorkInt(char_index, CHAR_WORK_toNo, 0);
      CHAR_setWorkInt(char_index, CHAR_WORK_mode, 0);
      CHAR_setWorkInt(char_index, CHAR_WORK_skill_level, 0);
      CHAR_setWorkInt(char_index, CHAR_WORK_array, 0);
    }

#endif
    COM = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);
    // printf("对像名=%s   COM=%d\n",CHAR_getChar(char_index,CHAR_NAME),COM);

    switch (COM) {
    case BATTLE_COM_ATTACK:
      if (gWeponType == ITEM_BOOMERANG) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG);

#ifdef _PETSKILL_BECOMEFOX
        if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101749 ||
            CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸不可以使用回力标
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
#endif
#ifdef _PETSKILL_BECOMEPIG
        if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1) { //变成乌力了
          gWeponType = ITEM_FIST;
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
        }
#endif
      }
      break;
    case BATTLE_COM_COMBO:
      if (ComboCheck(sRoundActionList, i, actionNum) == FALSE) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      }
      break;

    case BATTLE_COM_S_STATUSCHANGE:
      gBattleStausChange = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
      break;

#ifdef _PETSKILL_HECTOR
    case BATTLE_COM_S_HECTOR:
      gBattleStausChange = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
      break;
#endif

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case BATTLE_COM_S_SARS:
      gBattleStausChange = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
      break;
#endif

    case BATTLE_COM_S_CHARGE:
      BATTLE_Charge(battle_index, attackNo);
      break;
    case BATTLE_COM_S_RENZOKU:
      attack_max = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      gDamageDiv = attack_max;
      break;
#ifdef _VARY_WOLF
    case BATTLE_COM_S_VARY:
      // 先用不到
      break;
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
      // 狂暴攻击 vincent add 2002/05/16
    case BATTLE_COM_S_WILDVIOLENTATTACK:
      attack_max = RAND(3, 10); // 攻击次数
      gDamageDiv = attack_max;  // 每次攻击造成伤害
      gBattleDuckModyfy =
          CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3); //命中率
      break;
#endif
    case BATTLE_COM_S_EARTHROUND0:
      gBattleDamageModyfy =
          1.0 + 0.01 * CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM3);
      break;
#ifdef _BATTLE_ATTCRAZED
    case BATTLE_COM_S_ATTCRAZED:
      attack_max = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
      break;
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    case BATTLE_COM_S_ATTSHOOT:
      attack_max = CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
      gDamageDiv = attack_max; // 每次攻击造成伤害
      break;
#endif
    case BATTLE_COM_S_MIGHTY:
      gBattleDamageModyfy =
          CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3) * 0.01;
      gBattleDuckModyfy = CHAR_GETWORKINT_HIGH(
          char_index, CHAR_WORKBATTLECOM3); //   逃げられない共通へ  (无法闪避，统一)
      break;
    }

    BATTLE_TargetListSet(char_index, attackNo, aDefList);
    COM = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1);

    canCountineCounter = FALSE;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
            CHAR_BATTLEFLG_AIBAD &&
        COM == BATTLE_COM_COMBO) {
      print("由於忠诚度不足使用必杀技失败(%s)\n", CHAR_getUseName(char_index));
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    }
#ifdef _BATTLE_TIMESPEED // 动画播放速度
    {
      int flgtime = 0;
      switch (COM) {
#ifdef _BATTLE_ATTCRAZED
      case BATTLE_COM_S_ATTCRAZED:
        flgtime = 200 * 3;
        break;
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
      case BATTLE_COM_S_ATTSHOOT:
        flgtime = 80 * attack_max;
        break;
#endif
      case BATTLE_COM_JYUJYUTU:
      case BATTLE_COM_S_ATTACK_MAGIC:
        flgtime = 260; // 1/100sec
        break;
      case BATTLE_COM_COMBO:
        flgtime = 200; // 1/100sec
        break;
      case BATTLE_COM_GUARD:
        break;
      case BATTLE_COM_WAIT:
      default:
        flgtime = 150; // 1/100sec
        break;
      }
      BattleArray[battle_index].flgTime += flgtime;
    }
#endif
#ifdef _PETSKILL_BECOMEFOX // 中了媚惑术後攻防敏能力各降 20%
    if (CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1) { //变成小狐狸
      CHAR_setWorkInt(char_index, CHAR_WORKATTACKPOWER,
                      CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR) *
                          0.8); //攻降20%
      CHAR_setWorkInt(char_index, CHAR_WORKDEFENCEPOWER,
                      CHAR_getWorkInt(char_index, CHAR_WORKFIXTOUGH) *
                          0.8); //防降20%
      CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                      CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX) *
                          0.8); //敏降20%
    }
#endif
    switch (COM) { // 处理攻击

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    //==== 直接攻击系 ===============================================================
    //===============================================================
    case BATTLE_COM_S_BRUST:       // 爆击
    case BATTLE_COM_S_CHAIN_ATK:   // 连环攻击
    case BATTLE_COM_S_CHAIN_ATK_2: // 双重攻击
    case BATTLE_COM_S_CAVALRY:     // 座骑攻击
    case BATTLE_COM_S_DEAD_ATTACK: // 濒死攻击
    case BATTLE_COM_S_ATTACK_WEAK: // 弱点攻击
    case BATTLE_COM_S_PLUNDER:     // 体掠夺
    case BATTLE_COM_S_CHAOS:       // 混乱攻击
#ifdef _PREVENT_TEAMATTACK
    {
      // 防止同队互打
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
    }
#endif
    case BATTLE_COM_S_ENRAGE_PET: // 激怒宠物
    {
      CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      battle_profession_attack_fun(battle_index, attackNo, defNo, char_index);
      // change fix 修正使用职业得不到道具
      // BATTLE_AddProfit( battle_index, aAttackList);
    } break;
    //==== 魔法攻击系 ================================================================
    //================================================================
    case BATTLE_COM_S_VOLCANO_SPRINGS: // 火山泉
    case BATTLE_COM_S_FIRE_BALL:       // 火星球
    case BATTLE_COM_S_FIRE_SPEAR:      // 火龙枪
    case BATTLE_COM_S_SUMMON_THUNDER:  // 召雷术
    case BATTLE_COM_S_CURRENT:         // 电流术
    case BATTLE_COM_S_STORM:           // 暴风雨
    case BATTLE_COM_S_ICE_ARROW:       // 冰箭
    case BATTLE_COM_S_ICE_CRACK:       // 冰爆术
    case BATTLE_COM_S_ICE_MIRROR:      // 冰镜术
    case BATTLE_COM_S_DOOM:            // 世界末日
    case BATTLE_COM_S_BLOOD_WORMS:     // 嗜血蛊
    case BATTLE_COM_S_SIGN:            // 一针见血
    case BATTLE_COM_S_ENCLOSE:         // 附身术
    case BATTLE_COM_S_THROUGH_ATTACK:  // 贯穿攻击
    case BATTLE_COM_S_CONVOLUTE:       // 回旋攻击
    {
#ifdef _PROFESSION_ADDSKILL
      /*			if( COM == BATTLE_COM_S_DOOM //世界末日集气
                                      || COM == BATTLE_COM_S_FIRE_SPEAR){
                                      if( CHAR_getWorkInt( char_index,
         CHAR_DOOMTIME ) > 0 ){ BATTLE_NoAction( battle_index, attackNo );
                                              sprintf( szWork, "bt|%x|",
         attackNo ); BATTLESTR_ADD( szWork ); break;
                                      }
                              }*/
#endif
#ifdef _PREVENT_TEAMATTACK
      // 防止同队互打
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
#endif
    }
    case BATTLE_COM_S_BLOOD: // 嗜血成性
    {
      CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
#if 1 // Robin fix 修正嗜血吸别人的血
      if (defNo != attackNo && COM == BATTLE_COM_S_BLOOD) {
        print("\n 改封包!嗜血吸别人的血!!:%s ",
              CHAR_getChar(char_index, CHAR_CDKEY));
        defNo = attackNo;

        CHAR_talkToCli(char_index, -1, "因封包异常而断线。", CHAR_COLORYELLOW);
        CONNECT_setCloseRequest(CHAR_getWorkInt(char_index, CHAR_WORKFD), 1);
        break;
      }
#endif
#ifdef _PROFESSION_ADDSKILL
      if (COM == BATTLE_COM_S_ICE_CRACK) {
        if (pBattle->ice_count >= 20)
          pBattle->ice_count = 0;
        pBattle->ice_use[pBattle->ice_count] = TRUE;
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        pBattle->ice_level[pBattle->ice_count] =
            CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3);
        pBattle->ice_bout[pBattle->ice_count] = 2;
        pBattle->ice_toNo[pBattle->ice_count] =
            CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
        pBattle->ice_array[pBattle->ice_count] =
            CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
        pBattle->ice_char_index[pBattle->ice_count] = char_index;
        pBattle->ice_attackNo[pBattle->ice_count] = attackNo;
        ++pBattle->ice_count;
        BATTLE_NoAction(battle_index, attackNo);
        print("\n进入冰爆");
        break;
      }
#endif
      battle_profession_attack_magic_fun(battle_index, attackNo, defNo,
                                         char_index);
      // change fix 修正使用职业得不到道具
      // BATTLE_AddProfit( battle_index, aAttackList);
      break;
    }
    //==== 提升自已能力系 ====================================================================
    //====================================================================
    case BATTLE_COM_S_FOCUS:     // 专注战斗
    case BATTLE_COM_S_AVOID:     // 回避
    case BATTLE_COM_S_RECOVERY:  // 补血
    case BATTLE_COM_S_SCAPEGOAT: // 舍已为友
    case BATTLE_COM_S_ENRAGE:    // 激化攻击
    case BATTLE_COM_S_COLLECT:   // 能量聚集
    case BATTLE_COM_S_TRAP:      // 陷阱
    case BATTLE_COM_S_TRANSPOSE: // 移形换位
    case BATTLE_COM_S_DOCILE:    // 驯伏宠物
#ifdef _PROFESSION_ADDSKILL
    case BATTLE_COM_S_CALL_NATURE: // 号召自然
#endif
    {
      CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      battle_profession_assist_fun(battle_index, attackNo, defNo, char_index);
      break;
    }
    //==== 击中改变状态系 ====================================================================
    //====================================================================
    case BATTLE_COM_S_SHIELD_ATTACK: // 盾击
    case BATTLE_COM_S_ENTWINE:       // 树根缠绕
    case BATTLE_COM_S_DRAGNET:       // 天罗地网
    case BATTLE_COM_S_TOXIN_WEAPON:  // 毒素武器
    case BATTLE_COM_S_INSTIGATE:     // 挑拨
    case BATTLE_COM_S_OBLIVION:      // 遗忘
    {
#ifdef _PREVENT_TEAMATTACK
      // 防止同队互打
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
        BATTLE_NoAction(battle_index, attackNo);
        if (COM == BATTLE_COM_S_TOXIN_WEAPON)
          printf("\nbug-同队互打");
        break;
      }
#endif
    }
    case BATTLE_COM_S_RESIST_FIRE:    // 火抗性提升
    case BATTLE_COM_S_RESIST_ICE:     // 冰抗性提升
    case BATTLE_COM_S_RESIST_THUNDER: // 雷抗性提升
#ifdef _PROFESSION_ADDSKILL
    case BATTLE_COM_S_RESIST_F_I_T: // 自然威能
    case BATTLE_COM_S_BOUNDARY:     // 四属性结界
#endif
    case BATTLE_COM_S_FIRE_ENCLOSE:    // 火附体
    case BATTLE_COM_S_ICE_ENCLOSE:     // 冰附体
    case BATTLE_COM_S_THUNDER_ENCLOSE: // 雷附体
    {

      CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      battle_profession_status_chang_fun(battle_index, attackNo, defNo,
                                         char_index);

      break;
    }
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    case BATTLE_COM_S_ACUPUNCTURE: // 针刺外皮
      CHAR_setWorkInt(char_index, CHAR_WORKACUPUNCTURE, 1);
#endif
    case BATTLE_COM_S_CHARGE_OK:
    case BATTLE_COM_S_POWERBALANCE:
    case BATTLE_COM_S_STATUSCHANGE:
    case BATTLE_COM_S_EARTHROUND0:
    case BATTLE_COM_S_MIGHTY:
    case BATTLE_COM_S_GUARDIAN_ATTACK:
    case BATTLE_COM_S_RENZOKU:
#ifdef _BATTLE_ATTCRAZED // ANDY 疯狂暴走
    case BATTLE_COM_S_ATTCRAZED:
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    case BATTLE_COM_S_ATTSHOOT:
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
    case BATTLE_COM_S_WILDVIOLENTATTACK: // 狂暴攻击 vincent add 2002/05/16
                                         // 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
    case BATTLE_COM_S_SPEEDYATTACK: // 疾速攻击 vincent add 2002/05/20
#endif
    case BATTLE_COM_ATTACK:
#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case BATTLE_COM_S_SARS:
#endif

#ifdef _PETSKILL_BECOMEFOX
    case BATTLE_COM_S_BECOMEFOX:
#endif

#ifdef _PETSKILL_BECOMEPIG
    case BATTLE_COM_S_BECOMEPIG:
#endif

#ifdef _PETSKILL_SHOWMERCY
    case BATTLE_COM_S_SHOWMERCY:
#endif
#ifdef _MASSAGE_PETSKILL
    case BATTLE_COM_S_MASSAGE:
#endif
#ifdef _INVERSION_PETSKILL
    case BATTLE_COM_S_INVERSION:
#endif
#ifdef _PETOUT_PETSKILL
    case BATTLE_COM_S_PETOUT:
      if (COM == BATTLE_COM_S_PETOUT) {
        int defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
        int defindex = BATTLE_No2Index(battle_index, defNo);
        if (CHAR_getInt(defindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          int array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
          char *skillarg = PETSKILL_getChar(array, PETSKILL_OPTION);
          char key[32];
          char jl[32];
          getStringFromIndexWithDelim(skillarg, "|", 1, key, sizeof(key));
          getStringFromIndexWithDelim(skillarg, "|", 2, jl, sizeof(jl));

          char token[256];
          if ((rand() % 100) > atoi(jl)) {
            sprintf(token, "BK|%s 使用 %s 不成功|", CHAR_getUseName(defindex),
                    PETSKILL_getChar(array, PETSKILL_OPTION));
            strcat(szBadStatusString, token);
            break;
          }

          int petid;
          for (petid = 0; petid < CHAR_MAXPETHAVE; petid++) {
            if (!CHAR_CHECKINDEX(CHAR_getCharPet(defindex, petid)))
              continue;

            if ((strstr(skillarg, "休")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_NONE) {
                continue;
              }
            } else if ((strstr(skillarg, "邮")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_MAIL) {
                continue;
              }
            } else if ((strstr(skillarg, "待")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_SELECT) {
                continue;
              }
            } else if ((strstr(skillarg, "骑")) != NULL) {
              if (CHAR_getInt(defindex, CHAR_RIDEPET) != petid)
                continue;
            }

            int pet_no = CHAR_getInt(defindex, CHAR_DEFAULTPET);
            if (pet_no != -1) {
              BATTLE_PetIn(battle_index, defNo);
              GmsvServer_SPET_send(getfdFromchar_index(defindex), defNo, TRUE);
            }
            if ((strstr(skillarg, "收")) != NULL)
              break;
            // shan End
            if (BATTLE_S_PetOut(battle_index, defNo, petid) == FALSE) {
              if (pet_no != -1) {
                BATTLE_S_PetOut(battle_index, defNo, pet_no);
              }
            }
            GmsvServer_KS_send(getfdFromchar_index(defindex),
                               CHAR_getInt(defindex, CHAR_DEFAULTPET), TRUE);
            break;
          }
        } else {
          CHAR_talkToCli(char_index, -1, "该技能只能对玩家人物使用!",
                         CHAR_COLORYELLOW);
        }
        break;
      }
#endif
#ifdef _GRAPPLING_PETSKILL
    case BATTLE_COM_S_GRAPPLING:
#endif
#ifdef _PETSKILL_GYRATE
    case BATTLE_COM_S_GYRATE:
      if (COM == BATTLE_COM_S_GYRATE) {
        int i, j = 0, f_num, temp[5] = {0};
        char szWork[256];
        defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
        if (defNo < 5)
          f_num = 0;
        else if (defNo >= 5 && defNo < 10)
          f_num = 5;
        else if (defNo >= 10 && defNo < 15)
          f_num = 10;
        else
          f_num = 15;

        // 取出战场上存活的人
        for (i = f_num; i < f_num + 5; i++) {
          if (BATTLE_TargetCheck(battle_index, i) != FALSE) {
            temp[j++] = i;
          }
        }
        if (j > 0) {
          sprintf(szWork, "BH|a%X|", attackNo); // 下攻击指令
          BATTLESTR_ADD(szWork);
        }
        for (i = 0; i < j; i++) {
          // int atk_index = BATTLE_No2Index( battle_index, attackNo );
          // int def_index = BATTLE_No2Index( battle_index, temp[i] );

          BATTLE_Attack(battle_index, attackNo, temp[i]);
        }

        BATTLESTR_ADD("FF|");
        break;
      }
#endif

#ifdef _PETSKILL_FIREKILL
    case BATTLE_COM_S_FIREKILL:
      if (COM == BATTLE_COM_S_FIREKILL) {
        int i;
        defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);

        // 目标死亡,或不存在,或地球一周
        if (BATTLE_TargetCheck(battle_index, defNo) == FALSE ||
            CHAR_getWorkInt(BATTLE_No2Index(battle_index, defNo),
                            CHAR_WORKBATTLECOM1) == BATTLE_COM_S_EARTHROUND0) {
          // 随意找一只打
          if (defNo < 10) {
            for (i = 0; i < 10; i++)
              if (BATTLE_TargetCheck(battle_index, i) &&
                  CHAR_getWorkInt(BATTLE_No2Index(battle_index, i),
                                  CHAR_WORKBATTLECOM1) !=
                      BATTLE_COM_S_EARTHROUND0) {
                defNo = i;
                break;
              }
          } else {
            for (i = 10; i < 20; i++)
              if (BATTLE_TargetCheck(battle_index, i) &&
                  CHAR_getWorkInt(BATTLE_No2Index(battle_index, i),
                                  CHAR_WORKBATTLECOM1) !=
                      BATTLE_COM_S_EARTHROUND0) {
                defNo = i;
                break;
              }
          }
          if (BATTLE_TargetCheck(battle_index, defNo) == FALSE ||
              CHAR_getWorkInt(BATTLE_No2Index(battle_index, defNo),
                              CHAR_WORKBATTLECOM1) ==
                  BATTLE_COM_S_EARTHROUND0) {
            return 0;
          }
        }
#ifdef _PREVENT_TEAMATTACK
        // 防止同队互打
        if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
          BATTLE_NoAction(battle_index, attackNo);
          CHAR_talkToCli(char_index, -1, "不能对同队使用此技能!", CHAR_COLORYELLOW);
          break;
        }
#endif
        // 物理攻击减少为80%
        CHAR_setWorkInt(
            char_index, CHAR_WORKATTACKPOWER,
            ((float)CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR) * 0.8));

        sprintf(szWork, "Bf|a%X|", attackNo);
        BATTLESTR_ADD(szWork);

        BATTLE_Attack_FIREKILL(battle_index, attackNo, defNo);

        BATTLE_MultiAttMagic_Fire(battle_index, attackNo, defNo, 2,
                                  200); //2与200代表火属性魔法200伤害
        BATTLESTR_ADD("FF|");

        break;
      }
#endif
#ifdef _PETSKILL_HECTOR
    case BATTLE_COM_S_HECTOR:
      if (COM == BATTLE_COM_S_HECTOR) {
        int def_index = -1;

        defNo =
            CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2); //取得被攻击者的位置
        def_index = BATTLE_No2Index(battle_index, defNo); // 取得被攻击者的idx

#ifdef _PREVENT_TEAMATTACK
        // 防止同队互打
        if (BATTLE_CheckSameSide(char_index, defNo) == 1) {
          BATTLE_NoAction(battle_index, attackNo);
          break;
        }
#endif
        if (!CHAR_CHECKINDEX(def_index))
          break;
        char *pszP;
        int skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
        char *pszOption = PETSKILL_getChar(skill, PETSKILL_OPTION);
        int fPer = 60;
        if ((pszP = strstr(pszOption, "命%")) != NULL) {
          sscanf(pszP + 3, "%d", &fPer);
        }

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
        if (PROFESSION_BATTLE_StatusAttackCheck(char_index, def_index, 2,
                                                fPer)) {
#ifdef _FIX_PETSKILL_HECTOR
          char szTurn[] = "turn";
          int status = -1, i = 0, turn = 3;
          pszP = pszOption;
          for (; status == -1 && pszP[0] != 0; pszP++) {
            for (i = 1; i < BATTLE_ST_END; i++) {
              if (strncmp(pszP, aszStatus[i], 2) == 0) {
                status = i;
                pszP += 2;
                break;
              }
            }
          }

          if ((pszP = strstr(pszP, szTurn)) != NULL) {
            pszP += sizeof(szTurn);
            sscanf(pszP, "%d", &turn);
          }

          CHAR_setWorkInt(def_index, StatusTbl[status], turn); // 设定麻痹
#else

          CHAR_setWorkInt(def_index, StatusTbl[2], 1); // 设定麻痹

#endif
        }
#endif
      }
#endif
#ifdef _PETSKILL_ANTINTER
    case BATTLE_COM_S_ANTINTER:
      if (COM == BATTLE_COM_S_ANTINTER) {
        int skill;
        defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
        if (CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_WHICHTYPE) ==
                CHAR_TYPEPET &&
            CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_HP) <= 0) {
          skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
          canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                          BATTLE_COM_S_ANTINTER, skill);
          break;
        }
      }
#endif
#ifdef _RESURRECTION_PETSKILL
    case BATTLE_COM_S_RESURRECTION:
      if (COM == BATTLE_COM_S_RESURRECTION) {
        defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);

        if (CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_HP) <= 0) {
          char szcommand[256];
          int skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
          char *pszP = PETSKILL_getChar(skill, PETSKILL_OPTION);
          int attackindex = BATTLE_No2Index(battle_index, attackNo);
          int hp = CHAR_getInt(attackindex, CHAR_HP) - atoi(pszP);
          CHAR_setInt(attackindex, CHAR_HP, atoi(pszP));

          if (atoi(pszP) == 0) {
            CHAR_setFlg(attackindex, CHAR_ISDIE, TRUE);
            CHAR_setWorkInt(attackindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }

          BATTLE_MultiRessurect(battle_index, attackNo, defNo, hp, 0,
                                MAGIC_EFFECT_USER, SPR_fukkatu3);
          if (CHAR_getInt(attackindex, CHAR_RIDEPET) != -1) {
            sprintf(szcommand, "BD|r%X|0|0|%X|p%X", attackNo, hp * 60, hp * 40);
            BATTLESTR_ADD(szcommand);
          } else {
            sprintf(szcommand, "BD|r%X|0|0|%X", attackNo, hp);
            BATTLESTR_ADD(szcommand);
          }
          break;
        }
      }
#endif

#ifdef _LOSTLOST_PETSKILL
    case BATTLE_COM_S_LOSTLOST:
      if (COM == BATTLE_COM_S_LOSTLOST) {
        char szCommand[256];
        defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);

        int defHp = CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_HP);
        int attackHp =
            CHAR_getInt(BATTLE_No2Index(battle_index, attackNo), CHAR_HP);

        CHAR_setInt(BATTLE_No2Index(battle_index, attackNo), CHAR_HP, 1);
        CHAR_setInt(BATTLE_No2Index(battle_index, defNo), CHAR_HP, 1);

        snprintf(szCommand, sizeof(szCommand), "BH|a%X|r%X|0|d%X|FF|", attackNo,
                 defNo, defHp - 1);
        BATTLESTR_ADD(szCommand);

        sprintf(szCommand, "BD|r%X|0|0|%X|", attackNo, attackHp - 1);
        BATTLESTR_ADD(szCommand);
        break;
      }
#endif

#ifdef _PETSKILL_RETRACE
    case BATTLE_COM_S_RETRACE:
      if (COM == BATTLE_COM_S_RETRACE)
        CHAR_setWorkInt(char_index, CHAR_WORKRETRACE, 1);
#endif
      CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
      if (gWeponType == ITEM_BOW) {
        for (attack_count = 0; attack_count < 10; attack_count++) {
          defNo = aDefList[attack_count];
          if (attackNo == defNo)
            continue;
          if (BATTLE_TargetCheck(battle_index, defNo) == TRUE)
            break;
        }
        if (attack_count >= 10) {
          BATTLE_NoAction(battle_index, attackNo);
          break;
        }
        defNo = aDefList[0];
      } else if ((defNo = BATTLE_TargetAdjust(battle_index, char_index,
                                              myside)) < 0 ||
                 defNo == attackNo) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      if (COM == BATTLE_COM_S_EARTHROUND0) {
        sprintf(szWork, "BI|a%X|", attackNo);
      } else {
#ifdef _FIXBUG_ATTACKBOW
        if (CHAR_getWorkInt(char_index, CHAR_WORKITEMMETAMO) > 0 ||
            CHAR_getWorkInt(char_index, CHAR_WORKNPCMETAMO) > 0 ||
            CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸

          if (gWeponType == ITEM_BOW || gWeponType == ITEM_BOUNDTHROW ||
              gWeponType == ITEM_BREAKTHROW || gWeponType == ITEM_BOOMERANG) {
            CHAR_talkToCli(
                char_index, -1,
                "变身状态下无法使用远程武器！",
                CHAR_COLORYELLOW);
            continue;
          }
        }
#endif
        if (gWeponType == ITEM_BOW)
          sprintf(szWork, "BB|a%X|w0|", attackNo);
        else if (gWeponType == ITEM_BOUNDTHROW) // 若是丢斧头
          sprintf(szWork, "BB|a%X|w1|", attackNo);
        else if (gWeponType == ITEM_BREAKTHROW)
          sprintf(szWork, "BB|a%X|w2|", attackNo);
        else
          sprintf(szWork, "BH|a%X|", attackNo);
      }
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
      if (COM == BATTLE_COM_S_ATTSHOOT)
        sprintf(szWork, "BB|a%X|w0|", attackNo);
#endif
      BATTLESTR_ADD(szWork);
      if (COM == BATTLE_COM_S_CHARGE_OK || COM == BATTLE_COM_S_EARTHROUND0) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      } else {
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
        if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) !=
            BATTLE_COM_S_ATTSHOOT)
#endif
#ifdef _PETSKILL_SHOWMERCY
          if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) !=
              BATTLE_COM_S_SHOWMERCY)
#endif
#ifdef _MASSAGE_PETSKILL
            if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) !=
                BATTLE_COM_S_MASSAGE)
#endif
#ifdef _GRAPPLING_PETSKILL
              if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) !=
                  BATTLE_COM_S_GRAPPLING)
#endif
#ifdef _INVERSION_PETSKILL
                if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) !=
                    BATTLE_COM_S_INVERSION)
#endif

                  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1,
                                  BATTLE_COM_ATTACK);
      }

      for (attack_count = 0, k = 0;;) {
        if (gWeponType == ITEM_BOW) {
          if (BATTLE_TargetCheck(battle_index, defNo) == FALSE) {
            attack_flg = 0;
          } else {
            attack_flg = 1;
          }
        }

        if (attack_flg) {
          canCountineCounter = BATTLE_Attack(battle_index, attackNo, defNo);
#ifdef _PETSKILL_RETRACE
          if (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData ==
                  BATTLE_RET_DODGE &&
              COM == BATTLE_COM_S_RETRACE) {
            if (RAND(1, 100) < 80) { // 80%机率发动2次攻击
              // 攻+20%
              CHAR_setWorkInt(
                  BATTLE_No2Index(battle_index, attackNo), CHAR_WORKATTACKPOWER,
                  (CHAR_getWorkInt(BATTLE_No2Index(battle_index, attackNo),
                                   CHAR_WORKFIXSTR) +
                   (CHAR_getWorkInt(BATTLE_No2Index(battle_index, attackNo),
                                    CHAR_WORKFIXSTR) *
                    0.2)));

              BATTLE_Attack(battle_index, attackNo, defNo);
            }
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = 0;
          }
#endif
          BATTLE_AddProfit(battle_index, aAttackList);
          if (++attack_count >= attack_max)
            break;
          if (CHAR_getInt(char_index, CHAR_HP) <= 0) {
            canCountineCounter = FALSE;
            break;
          }
        }
        defNo = aDefList[++k];
        if (defNo < 0)
          break;
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, defNo);
        if (gWeponType == ITEM_BOW) {
        } else if ((defNo = BATTLE_TargetAdjust(battle_index, char_index,
                                                myside)) < 0) {
          break;
        }
        if (BATTLE_CountAlive(battle_index, 0) == 0 ||
            BATTLE_CountAlive(battle_index, 1) == 0) {
          break;
        }
      }
      gBattleDamageModyfy = 1.0;
      gBattleDuckModyfy = 0;
      for (k = 0; k < 5 && canCountineCounter == TRUE; k++) {
        int round = k & 1;
        int attackNoSub = round ? attackNo : defNo;
        int defNoSub = round ? defNo : attackNo;
        // 反击判定
        canCountineCounter = BATTLE_Counter(battle_index, attackNoSub, defNoSub);
        aAttackList[0] = attackNoSub;
        aAttackList[1] = -1;
        BATTLE_AddProfit(battle_index, aAttackList);
      }
      aAttackList[0] = -1;
      BATTLESTR_ADD("FF|");

#ifdef _INVERSION_PETSKILL
      if (COM == BATTLE_COM_S_INVERSION) {
        if ((Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
             BATTLE_RET_MISS) //没有miss
            && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
                BATTLE_RET_DODGE) &&
            (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
             BATTLE_RET_ALLGUARD) //没有闪避过
            && (BATTLE_TargetCheck(battle_index, defNo)) // 还活着
        ) {
          int flg, OnOff;
          int toindex = BATTLE_No2Index(battle_index, defNo);
          flg = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEFLG);
          flg ^= CHAR_BATTLEFLG_REVERSE;
          CHAR_setWorkInt(toindex, CHAR_WORKBATTLEFLG, flg);
          OnOff = (flg & CHAR_BATTLEFLG_REVERSE) ? (1) : (0);
          BATTLE_AttReverse(toindex);
          sprintf(szWork, "FF|BR|%X|%X|FF|", defNo, OnOff);
          BATTLESTR_ADD(szWork);
        }
      }
#endif

#ifdef _PETSKILL_BECOMEFOX // 向对方使用媚惑术
      if ((COM == BATTLE_COM_S_BECOMEFOX) &&
          (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
           BATTLE_RET_MISS) //没有miss
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_DODGE) //没有闪避过
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ALLGUARD)
#ifdef _EQUIT_ARRANGE
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ARRANGE)
#endif
          && (BATTLE_TargetCheck(battle_index, defNo)) // 还活着
          && (rand() % 100 < 31) &&
          (CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_WHICHTYPE) !=
           CHAR_TYPEPLAYER) //只有宠物(CHAR_TYPEPET)才可以被媚惑
          && (CHAR_getWorkInt(BATTLE_No2Index(battle_index, defNo),
                              CHAR_WORK_PETFLG) != 0)
#ifdef _PETSKILL_BECOMEPIG
          && (CHAR_getInt(char_index, CHAR_BECOMEPIG) == -1) //没有处於乌力化
#endif
      ) {
        int ToList[SIDE_OFFSET * 2 + 1];
        int defindex = BATTLE_No2Index(battle_index, defNo);

        BATTLE_MultiList(battle_index, defNo, ToList);
        BATTLE_MagicEffect(battle_index, defNo, ToList, 101120, 101750);
        CHAR_setWorkInt(defindex, CHAR_WORKFOXROUND, pBattle->turn);

        if (CHAR_getInt(defindex, CHAR_RIDEPET) != -1) { // 骑宠就要让他落马
          CHAR_setInt(defindex, CHAR_RIDEPET, -1);
          BATTLE_changeRideImage(defindex);
          CHAR_setWorkInt(defindex, CHAR_WORKPETFALL, 1);
        }

        CHAR_setInt(defindex, CHAR_BASEIMAGENUMBER, 101749);
      }
#endif
#ifdef _PETSKILL_BECOMEPIG // 向对方使用乌力化
      if ((COM == BATTLE_COM_S_BECOMEPIG) &&
          (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
           BATTLE_RET_MISS) //没有miss
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_DODGE) //没有闪避过
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ALLGUARD)
#ifdef _EQUIT_ARRANGE
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ARRANGE)
#endif
          && (BATTLE_TargetCheck(battle_index, defNo)) // 还活着
          &&
          (CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_WHICHTYPE) ==
           CHAR_TYPEPLAYER) //只有玩家才可以被乌力化
#ifdef _PREVENT_TEAMATTACK
          && (BATTLE_CheckSameSide(char_index, defNo) != 1) // 防止同队互打
#endif
          && CHAR_getInt(BATTLE_No2Index(battle_index, defNo), CHAR_BECOMEPIG) <
                 2000000000) //防止破表
      {
        char temp[64];
        int ToList[SIDE_OFFSET * 2 + 1], compute, petrate, pettime,
            pigbbi = 100250;
        char *pszOption = NULL;
        int defindex = BATTLE_No2Index(battle_index, defNo);
        pszOption = PETSKILL_getChar(
            CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3),
            PETSKILL_OPTION);
        if (pszOption == "\0")
          sscanf(pszOption, "%d %d %d", &petrate, &pettime, &pigbbi);
        else
          petrate = 30, pettime = 60, pigbbi = 100250;

        if (rand() % 100 < petrate) {
          CHAR_setWorkInt(defindex, CHAR_WORKITEMMETAMO, 0); // 道具变身取消
          CHAR_setWorkInt(defindex, CHAR_WORKNPCMETAMO, 0); // NPC变身取消
          CHAR_setWorkInt(defindex, CHAR_WORKFOXROUND, -1); // 媚惑术变身取消

          BATTLE_MultiList(battle_index, defNo, ToList);
          BATTLE_MagicEffect(battle_index, defNo, ToList, 101120, 101750);

          if (CHAR_getInt(defindex, CHAR_RIDEPET) != -1) { // 骑宠就要让他落马
            CHAR_setInt(defindex, CHAR_RIDEPET, -1);
            BATTLE_changeRideImage(defindex);
            CHAR_setWorkInt(defindex, CHAR_WORKPETFALL, 1);
          }

          CHAR_setInt(defindex, CHAR_BECOMEPIG_BBI, pigbbi);

          compute = CHAR_getInt(defindex, CHAR_BECOMEPIG);
          CHAR_setInt(defindex, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(defindex, CHAR_BECOMEPIG_BBI));
          if (compute == -1) // 第一次中
            CHAR_setInt(defindex, CHAR_BECOMEPIG,
                        pettime + 1 + compute); //乌力化时间(单位:秒)
          else
            CHAR_setInt(defindex, CHAR_BECOMEPIG, pettime + compute);
          sprintf(temp, "乌力时间:%d秒", CHAR_getInt(defindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(defindex, -1, temp, CHAR_COLORYELLOW);

          print("\n乌力化开始0:%s,round:%d", CHAR_getChar(defindex, CHAR_NAME),
                CHAR_getWorkInt(defindex, CHAR_WORKFOXROUND));
        }
      }
#endif
      break;
    case BATTLE_COM_BOOMERANG:
      // BOOMRANG似乎是回旋镖？
      gBattleDamageModyfy = 0.3;
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      if (defNo < 0) {
        defNo = BATTLE_DefaultAttacker(battle_index, 1 - myside);
      }
      if (0 <= defNo && defNo <= 19) {
        defNo /= 5; //
      } else {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      if ((int)(attackNo / 5) == defNo) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      {
        int *pBoomerangTbl = BoomerangVsTbl[defNo];
        for (k = 0; k < 5; k++) {
          if (BATTLE_TargetCheck(battle_index, pBoomerangTbl[k]) == FALSE) {
            continue;
          } else {
            break;
          }
        }
        if (k == 5) {
          defNo = BATTLE_DefaultAttacker(battle_index, 1 - myside);
          if (0 <= defNo && defNo <= 19) {
            defNo /= 5;
          } else {
            BATTLE_NoAction(battle_index, attackNo);
            break;
          }
        }
      }
      sprintf(szWork, "BO|a%X|", attackNo);
      BATTLESTR_ADD(szWork);
      {
        int *pBoomerangTbl = BoomerangVsTbl[defNo];
        if (myside == 1) {
          k = 4;
          j = -1;
        } else {
          k = 0;
          j = 1;
        }
        for (;; k += j) {
          if (j > 0 && k > 4)
            break;
          if (j < 0 && k < 0)
            break;
          defNo = pBoomerangTbl[k];
          if (BATTLE_TargetCheck(battle_index, defNo) == FALSE) {
            continue;
          }
          BATTLE_Attack(battle_index, attackNo, defNo);
        }
      }
      gBattleDamageModyfy = 1.0;
      gBattleDuckModyfy = 0;
      BATTLESTR_ADD("FF|");
      break;

    //---------------------------------------------
    case BATTLE_COM_CAPTURE:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      BATTLE_Capture(battle_index, attackNo, defNo);
      break;

      //---------------------------------------------
#ifdef _VARY_WOLF
    case BATTLE_COM_S_VARY: {
      int ToList[SIDE_OFFSET * 2 + 1];
      int bk_amn = 0;

      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
#ifdef _EXPANSION_VARY_WOLF
      if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101428) {
        bk_amn = (CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER) + 5) +
                 (myside * 4);
      } else {
        bk_amn = (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER));
      }

      BATTLE_MultiList(battle_index, defNo, ToList);
      BATTLE_MagicEffect(battle_index, attackNo, ToList, 101120, bk_amn);
#endif
    } break;
#endif
    case BATTLE_COM_GUARD:
      BATTLE_Guard(battle_index, attackNo);

      break;
    case BATTLE_COM_ESCAPE:
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
        if (BATTLE_Escape(battle_index, attackNo, 0) == TRUE) {
          if (CHAR_CHECKINDEX(char_index)) {
            if (CHAR_getWorkInt(char_index, CHAR_WORKFMPKFLAG) == 1) {
              CHAR_setWorkInt(char_index, CHAR_WORKFMPKFLAG, -1);
            }
          }
        }
      }
      break;
    case BATTLE_COM_COMPELESCAPE: // 强制离开
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
        BATTLE_Escape(battle_index, attackNo, 1);
        if (CHAR_getWorkInt(char_index, CHAR_WORKFMPKFLAG) == 1)
          CHAR_setWorkInt(char_index, CHAR_WORKFMPKFLAG, -1);
      }
      break;

    case BATTLE_COM_PETIN:
      if (CHAR_getInt(char_index, CHAR_DEFAULTPET) == -1) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      {
        char szWork[256];
        snprintf(szWork, sizeof(szWork), "K%d",
                 CHAR_getInt(char_index, CHAR_DEFAULTPET));
        CHAR_sendStatusString(char_index, szWork);
      }
      BATTLE_PetIn(battle_index, attackNo);
      GmsvServer_KS_send(getfdFromchar_index(char_index),
                         CHAR_getInt(char_index, CHAR_DEFAULTPET), 1);
      break;
    case BATTLE_COM_PETOUT:
      if (CHAR_getInt(char_index, CHAR_DEFAULTPET) != -1) {
        GmsvServer_KS_send(getfdFromchar_index(char_index),
                           CHAR_getInt(char_index, CHAR_DEFAULTPET), 1);
        {
          char szWork[256];
          snprintf(szWork, sizeof(szWork), "K%d",
                   CHAR_getInt(char_index, CHAR_DEFAULTPET));
          CHAR_sendStatusString(char_index, szWork);
        }

        BATTLE_PetIn(battle_index, attackNo);
      }
      BATTLE_PetOut(battle_index, attackNo);
      GmsvServer_KS_send(getfdFromchar_index(char_index),
                         CHAR_getInt(char_index, CHAR_DEFAULTPET), 1);
      break;
// Terry add 2001/11/05
#ifdef _ATTACK_MAGIC
    case BATTLE_COM_S_ATTACK_MAGIC: // 宠物魔法

      magic = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      toindex = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      //			print("magic->%d,toindex->%d\n",magic,toindex);
      // 判断攻    围
      for (magic_count = 0; magic_count < MAX_MAGIC_NUMBER; magic_count++) {
        // 比对魔法的id值来找出魔法的攻击  围
        if (magic == TargetIndex[magic_count][0]) {
          // 如果是打全体的魔法
          if (TargetIndex[magic_count][1] == 20) {
#ifdef _OPEN_E_PETSKILL
            if (toindex >= 10) {
              toindex = 21;
            } else {
              toindex = 20;
            }
#else
            toindex = TargetIndex[magic_count][1];
#endif
          }
          // 如果不是打单人的魔法
          else if (TargetIndex[magic_count][1] != -1) {
            // 如果是打右下的後面那一排
            if (toindex >= 0 && toindex <= 4)
              toindex = TargetIndex[magic_count][1];
            // 打右下的前面那一排
            else
              toindex = TargetIndex[magic_count][1] - 1;
          }
          break;
        }
      }
      MAGIC_DirectUse(char_index, magic, toindex,
                      CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3));
      BATTLE_AddProfit(battle_index, aAttackList);
      break;
#endif
    case BATTLE_COM_JYUJYUTU:
      MAGIC_DirectUse(char_index,
                      CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3),
                      CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2),
                      CHAR_GETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3));
      break;

    case BATTLE_COM_ITEM:
      item_index = CHAR_getItemIndex(
          char_index, CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM3));
      if (ITEM_CHECKINDEX(item_index) == FALSE) {
        print("\nerr:要使用的item位置奇怪(%s)(%d)\n", CHAR_getUseName(char_index),
              CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM3));
        break;
      }
#ifdef _FIXBUG_ATTACKBOW
      {
        if (CHAR_getWorkInt(char_index, CHAR_WORKITEMMETAMO) > 0 ||
            CHAR_getWorkInt(char_index, CHAR_WORKNPCMETAMO) > 0 ||
            CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸

          int armtype;
          int item_index = CHAR_getItemIndex(
              char_index, CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM3));
          if (!ITEM_CHECKINDEX(item_index))
            break;
          armtype = ITEM_getInt(item_index, ITEM_TYPE);

          if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
              armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
            CHAR_talkToCli(
                char_index, -1,
                "变身无法使用远距离武器！",
                CHAR_COLORYELLOW);
            break;
          }
        }
      }
#endif
      CHAR_ItemUse(char_index, CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2),
                   CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM3));

      int itemtype = ITEM_getEquipPlace(char_index, item_index);
      if (itemtype == CHAR_ARM || itemtype == CHAR_HEAD ||
          itemtype == CHAR_BODY || itemtype == CHAR_DECORATION1 ||
          itemtype == CHAR_DECORATION2) {
        sprintf(szWork, "BN|a%X|%X|", attackNo,
                CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER));
        BATTLESTR_ADD(szWork);

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
        // 武器专精
        BATTLE_ProfessionStatus_init(battle_index, char_index);
#endif
      }

      break;

#ifdef _PSKILL_FALLGROUND
    case BATTLE_COM_S_FALLRIDE: // 落马术
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      canCountineCounter = BATTLE_S_FallGround(battle_index, attackNo, defNo,
                                    BATTLE_COM_S_FALLRIDE);
      break;

#endif
#ifdef _PETSKILL_EXPLODE
    case BATTLE_COM_S_EXPLODE:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      canCountineCounter =
          BATTLE_S_Explode(battle_index, attackNo, defNo, BATTLE_COM_S_EXPLODE);
      break;
#endif

#ifdef _PETSKILL_TIMID
    case BATTLE_COM_S_TIMID: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_TIMID, skill);
    } break;
#endif
#ifdef _PETSKILL_2TIMID
    case BATTLE_COM_S_2TIMID: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_2TIMID, skill);
    } break;
#endif

#ifdef _PETSKILL_PROPERTY
    case BATTLE_COM_S_PROPERTYSKILL: {
      int skill;
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      BATTLE_S_PetSkillProperty(battle_index, attackNo, COM, skill);
      BATTLE_NoAction(battle_index, attackNo);
    } break;
#endif

#ifdef _BATTLE_LIGHTTAKE
    case BATTLE_COM_S_LIGHTTAKE: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_LIGHTTAKE, skill);
    } break;
#endif

#ifdef _SKILL_DAMAGETOHP
    case BATTLE_COM_S_DAMAGETOHP: // 嗜血技
    {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_DAMAGETOHP, skill);
    } break;
#endif

#ifdef _PETSKILL_DAMAGETOHP
    case BATTLE_COM_S_DAMAGETOHP2: // 暗月狂狼(嗜血技的变体)
    {
      int skill, attackidxtemp;
      attackidxtemp = BATTLE_No2Index(battle_index, attackNo);

      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_DAMAGETOHP2, skill);
    } break;
#endif

#ifdef _Skill_MPDAMAGE
    case BATTLE_COM_S_MPDAMAGE: // MP伤害
    {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_MPDAMAGE, skill);
    } break;
#endif
#ifdef _SKILL_TOOTH
    case BATTLE_COM_S_TOOTHCRUSHE: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_TOOTHCRUSHE, skill);
    } break;
#endif
#ifdef _PSKILL_MODIFY
    case BATTLE_COM_S_MODIFYATT: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_MODIFYATT, skill);
    } break;
#endif
#ifdef _PSKILL_MDFYATTACK
    case BATTLE_COM_S_MDFYATTACK: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_MDFYATTACK, skill);
    } break;
#endif

#ifdef _PETSKILL_TEAR
    case BATTLE_COM_S_PETSKILLTEAR: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_PETSKILLTEAR, skill);
    } break;
#endif

#ifdef _SONIC_ATTACK // WON ADD 音波攻击
    case BATTLE_COM_S_SONIC: {
      int skill, defNo2 = -1, index2 = -1;

      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);

      if (defNo >= 15 && defNo < 20)
        defNo2 = defNo - 5;
      else if (defNo >= 5 && defNo < 10)
        defNo2 = defNo - 5;
      else
        defNo2 = -1;

      battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);
      index2 = BATTLE_No2Index(battle_index, defNo2);

      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_SONIC, skill);

      if (index2 >= 0) {
        if (defNo2 >= 0) {
          canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo2,
                                          BATTLE_COM_S_SONIC2, skill);
        }
      }

    } break;
#endif

#ifdef _PETSKILL_REGRET
    case BATTLE_COM_S_REGRET: {
      int defNo2 = -1, index2 = -1;
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      if (defNo >= 15 && defNo < 20)
        defNo2 = defNo - 5;
      else if (defNo >= 5 && defNo < 10)
        defNo2 = defNo - 5;
      else
        defNo2 = -1;
      battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);
      index2 = BATTLE_No2Index(battle_index, defNo2);
      canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo,
                                      BATTLE_COM_S_REGRET, skill);
      if (index2 >= 0 && defNo2 >= 0) {
          canCountineCounter = BATTLE_S_AttackDamage(battle_index, attackNo, defNo2,
                                          BATTLE_COM_S_REGRET2, skill);
      }

    } break;
#endif
#ifdef _MAGIC_SUPERWALL
    case BATTLE_COM_S_SUPERWALL: {
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = PETSKILL_MagicStatusChange_Battle(battle_index, attackNo, defNo,
                                                  skill);
    } break;
#endif
#ifdef _PETSKILL_SETDUCK
    case BATTLE_COM_S_SETDUCK: {
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter =
          PETSKILL_SetDuckChange_Battle(battle_index, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _MAGICPET_SKILL
    case BATTLE_COM_S_SETMAGICPET: {
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter =
          PETSKILL_SetMagicPet_Battle(battle_index, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _STRENGTH_PETSKILL
    case BATTLE_COM_S_STRENGTH: {
      int skill;
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter =
          PETSKILL_SetStrength_Battle(battle_index, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _OFFLINE_SYSTEM
    case BATTLE_COM_S_OFFLINE_RECOVERY: {
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      int HP = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = OFFLINE_MultiRessurect(battle_index, attackNo, defNo, HP, 0);
      break;
    }
#endif
#ifdef _PRO_BATTLEENEMYSKILL
    case BATTLE_COM_S_ENEMYRELIFE:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }

      canCountineCounter = BATTLE_E_ENEMYREFILE(battle_index, attackNo, defNo,
                                     BATTLE_COM_S_ENEMYRELIFE);
      if (canCountineCounter == FALSE) {
        CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
        canCountineCounter = BATTLE_Attack(battle_index, attackNo, defNo);
        BATTLE_AddProfit(battle_index, aAttackList);
      }
      BATTLE_AddProfit(battle_index, aAttackList);
      break;

    case BATTLE_COM_S_ENEMYREHP:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      canCountineCounter = BATTLE_E_ENEMYREHP(battle_index, attackNo, defNo,
                                   BATTLE_COM_S_ENEMYREHP);
      if (canCountineCounter == FALSE) {
        CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
        canCountineCounter = BATTLE_Attack(battle_index, attackNo, defNo);
        BATTLE_AddProfit(battle_index, aAttackList);
      }
      BATTLE_AddProfit(battle_index, aAttackList);
      break;

    case BATTLE_COM_S_ENEMYHELP:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      canCountineCounter = BATTLE_E_ENEMYHELP(battle_index, attackNo, defNo,
                                   BATTLE_COM_S_ENEMYREHP);
      if (canCountineCounter == FALSE) {
        CHAR_setFlg(char_index, CHAR_ISATTACKED, 1);
        canCountineCounter = BATTLE_Attack(battle_index, attackNo, defNo);
        BATTLE_AddProfit(battle_index, aAttackList);
      }
      BATTLE_AddProfit(battle_index, aAttackList);
      break;
#endif
    case BATTLE_COM_S_GBREAK: // 破除防御
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      canCountineCounter = BATTLE_S_GBreak(battle_index, attackNo, defNo);
      break;
#ifdef _SKILL_GUARDBREAK2 // 破除防御2 vincent add 2002/05/20
    case BATTLE_COM_S_GBREAK2:
      // 确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      canCountineCounter = BATTLE_S_GBreak2(battle_index, attackNo, defNo);
      break;
#endif

#ifdef _SKILL_SACRIFICE
    case BATTLE_COM_S_SACRIFICE: // 救援 vincent add 2002/05/30
                                 // 确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      canCountineCounter = BATTLE_S_Sacrifice(battle_index, attackNo, defNo);

      break;
#endif

#ifdef _SKILL_REFRESH // 状态回复 vincent add 2002/08/08
    case BATTLE_COM_S_REFRESH: {
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      canCountineCounter = BATTLE_S_Refresh(battle_index, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_WEAKEN // vincent宠技:虚弱
    case BATTLE_COM_S_WEAKEN: {
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      canCountineCounter = BATTLE_S_Weaken(battle_index, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_DEEPPOISON // vincent宠技:剧毒
    case BATTLE_COM_S_DEEPPOISON: {
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      canCountineCounter = BATTLE_S_Deeppoison(battle_index, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_BARRIER // vincent宠技:魔障
    case BATTLE_COM_S_BARRIER: {
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      canCountineCounter = BATTLE_S_Barrier(battle_index, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_NOCAST // vincent宠技:沉默
    case BATTLE_COM_S_NOCAST: {
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM2);
      canCountineCounter = BATTLE_S_Nocast(battle_index, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_ROAR // vincent宠技:大吼
    case BATTLE_COM_S_ROAR: {
      // 确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      canCountineCounter = BATTLE_S_Roar(battle_index, attackNo, defNo, array);
    } break;
#endif
    case BATTLE_COM_S_ABDUCT: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      array = CHAR_GETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3);
      BATTLE_Abduct(battle_index, attackNo, defNo, array);
    } break;
    case BATTLE_COM_COMBO:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      gDamageDiv = 1.0;
      int combo_id = sRoundActionList[i].combo;
      aAttackList[0] = sRoundActionList[i].num; // リストに加える (加入列表)
      i++;                               // 後の値から (从下一个开始)
      k = 1;
      for (; i < actionNum && sRoundActionList[i].combo == combo_id; ++i) {
        int charmode;
        // 存在 するかチェック (检查是否存在)
        if (CHAR_CHECKINDEX(sRoundActionList[i].char_index) == FALSE)
          continue;

        // 戦闘に 参加していない場合は次へ (未参战则跳过)
        charmode =
            CHAR_getWorkInt(sRoundActionList[i].char_index, CHAR_WORKBATTLEMODE);
        if (charmode == 0 || charmode == BATTLE_CHARMODE_FINAL) {
          continue;
        }
        // ステータス 処理 (状态处理)
        BATTLE_StatusSeq(sRoundActionList[i].char_index);
#ifdef _OTHER_MAGICSTAUTS
        BATTLE_MagicStatusSeq(sRoundActionList[i].char_index);
#endif
        // 動けない場合 (不能移动时)
        if (BATTLE_CanMoveCheck(sRoundActionList[i].char_index) == FALSE) {
          continue;
        }
        if (CHAR_getInt(sRoundActionList[i].char_index, CHAR_HP) <= 0) {
          continue;
        }
        aAttackList[k++] = sRoundActionList[i].num;
      }
      aAttackList[k] = -1;
      i--;
      if (k == 1) {
        print("出现一人合击的BUG.\n");
      }
      sprintf(szWork, "BY|r%X|", defNo);
      BATTLESTR_ADD(szWork);
      BATTLE_Combo(battle_index, aAttackList, defNo);

#ifdef _Item_ReLifeAct
      BATTLE_AddProfit(battle_index, aAttackList);
#endif
      // 終了 は FF を くっつける (结束时附加FF)
      BATTLESTR_ADD("FF|");
      break;

    //---------------------------------------------
    case BATTLE_COM_WAIT:
    case BATTLE_COM_NONE:
      BATTLE_NoAction(battle_index, attackNo);
      break;

    //---------------------------------------------
    case BATTLE_COM_S_EARTHROUND1:
      BATTLE_EarthRoundHide(battle_index, attackNo);
      break;

    //---------------------------------------------
    case BATTLE_COM_S_LOSTESCAPE:
      BATTLE_LostEscape(battle_index, attackNo);
      break;

    case BATTLE_COM_S_STEAL:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      BATTLE_Steal(battle_index, attackNo, defNo);
      break;

#ifdef _BATTLESTEAL_FIX
    case BATTLE_COM_S_STEALMONEY:
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }
      BATTLE_StealMoney(battle_index, attackNo, defNo);
      break;
#endif
    case BATTLE_COM_S_NOGUARD:
      BATTLE_NoAction(battle_index, attackNo);
      break;
    case BATTLE_COM_S_CHARGE:
      sprintf(szWork, "bt|%x|", attackNo);
      BATTLESTR_ADD(szWork);
      break;
#ifdef _PETSKILL_LER
    case BATTLE_COM_S_BAT_FLY: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }

      BATTLE_BatFly(battle_index, attackNo, myside);
    } break;
    case BATTLE_COM_S_DIVIDE_ATTACK: {
      if ((defNo = BATTLE_TargetAdjust(battle_index, char_index, myside)) < 0) {
        BATTLE_NoAction(battle_index, attackNo);
        break;
      }

      BATTLE_DivideAttack(battle_index, attackNo, myside);
    } break;
#endif
#ifdef _PETSKILL_BATTLE_MODEL
    case BATTLE_COM_S_BATTLE_MODEL:
      BATTLE_BattleModel(battle_index, attackNo, myside);
      break;
#endif
    //---------------------------------------------
    default:
      printf("\nerr:奇怪的命令(%d:%d:%s)\n", char_index, COM,
             CHAR_getUseName(char_index));
      sprintf(szWork, "bg|%x|", attackNo);
      BATTLESTR_ADD(szWork);
      break;
    }
    if (CHAR_CHECKINDEX(char_index)) {
#ifdef _VARY_WOLF
      if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101428
#ifdef _EXPANSION_VARY_WOLF
          || CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 104109
#endif
      ) {
        if (CHAR_getWorkInt(char_index, CHAR_WORKTURN) == 0)
          CHAR_setWorkInt(char_index, CHAR_WORKTURN, 1);
        else
          CHAR_setWorkInt(char_index, CHAR_WORKTURN,
                          CHAR_getWorkInt(char_index, CHAR_WORKTURN) + 1);

        if (CHAR_getWorkInt(char_index, CHAR_WORKTURN) > 5) {
          CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER));
          CHAR_setWorkInt(char_index, CHAR_WORKATTACKPOWER,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR));
          CHAR_setWorkInt(char_index, CHAR_WORKDEFENCEPOWER,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXTOUGH));
          CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX));
          CHAR_setWorkInt(char_index, CHAR_WORKTURN, 0);
        }
      }
#endif
#ifdef _PETSKILL_BECOMEFOX // 判断中了媚惑术後是否已到恢复的回合数
      // if(CHAR_getInt( char_index, CHAR_BASEIMAGENUMBER)==101749)
      // 用图号判断的话,若是人变身时,会被自动改回去
      if (CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1) //若是变成小狐狸
      {
        if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) != 101749)
          CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER, 101749);

        if (pBattle->turn - CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) >
            2) {
          int defNo = BATTLE_Index2No(battle_index, char_index);
          int toNo = defNo - 5; // 主人的编号
          // print("\n变身人物回复:%s,round:%d", CHAR_getChar( char_index,
          // CHAR_NAME), CHAR_getWorkInt( char_index, CHAR_WORKFOXROUND));

          CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER));

          CHAR_setWorkInt(char_index, CHAR_WORKATTACKPOWER,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXSTR));
          CHAR_setWorkInt(char_index, CHAR_WORKDEFENCEPOWER,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXTOUGH));
          CHAR_setWorkInt(char_index, CHAR_WORKQUICK,
                          CHAR_getWorkInt(char_index, CHAR_WORKFIXDEX));
          CHAR_setWorkInt(char_index, CHAR_WORKFOXROUND, -1);

          // Change fix 回复宠物忘掉的技能
          sprintf(
              szWork, "W%d",
              CHAR_getInt(BATTLE_No2Index(battle_index, toNo), CHAR_DEFAULTPET));
          CHAR_sendStatusString(BATTLE_No2Index(battle_index, toNo), szWork);
        }
      }
#endif
#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1 &&
          (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)) {
        char pigbuffer[128];
        CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER,
                    CHAR_getInt(char_index, CHAR_BECOMEPIG_BBI));
        if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > 0) {
          sprintf(pigbuffer, "乌力时间:%d秒",
                  CHAR_getInt(char_index, CHAR_BECOMEPIG));
          CHAR_talkToCli(char_index, -1, pigbuffer, CHAR_COLORWHITE);
        } else {
          sprintf(pigbuffer, " 乌力时间结束，离开战斗後可解除乌力化:%d ",
                  CHAR_getInt(char_index, CHAR_BECOMEPIG));
          CHAR_talkToCli(char_index, -1, pigbuffer, CHAR_COLORWHITE);
        }
      }
#endif
#ifdef _PROFESSION_ADDSKILL
      {
        int earth_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(char_index, CHAR_WORKFIXEARTHAT_BOUNDARY));
        int water_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(char_index, CHAR_WORKFIXWATERAT_BOUNDARY));
        int fire_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(char_index, CHAR_WORKFIXFIREAT_BOUNDARY));
        int wind_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(char_index, CHAR_WORKFIXWINDAT_BOUNDARY));
        int boundary_turn = 0;
        BOOL boundaryclear = FALSE;
        if (earth_boundary > 0) { // 地结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              char_index, CHAR_WORKFIXEARTHAT_BOUNDARY)) -
                          1;
          //				print("\n地:%d ",boundary_turn );
          if (boundary_turn <= -1)
            CHAR_setWorkInt(char_index, CHAR_WORKFIXEARTHAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(char_index, CHAR_WORKFIXEARTHAT_BOUNDARY,
                            MAKE2VALUE(earth_boundary, boundary_turn));
        }
        if (water_boundary > 0) { // 水结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              char_index, CHAR_WORKFIXWATERAT_BOUNDARY)) -
                          1;
          //				print("\n水:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(char_index, CHAR_WORKFIXWATERAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(char_index, CHAR_WORKFIXWATERAT_BOUNDARY,
                            MAKE2VALUE(water_boundary, boundary_turn));
        }
        if (fire_boundary > 0) { // 火结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              char_index, CHAR_WORKFIXFIREAT_BOUNDARY)) -
                          1;
          //				print("\n火:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(char_index, CHAR_WORKFIXFIREAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(char_index, CHAR_WORKFIXFIREAT_BOUNDARY,
                            MAKE2VALUE(fire_boundary, boundary_turn));
        }
        if (wind_boundary > 0) { // 风结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              char_index, CHAR_WORKFIXWINDAT_BOUNDARY)) -
                          1;
          //				print("\n风:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(char_index, CHAR_WORKFIXWINDAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(char_index, CHAR_WORKFIXWINDAT_BOUNDARY,
                            MAKE2VALUE(wind_boundary, boundary_turn));
        }
        if (GETHIGHVALUE(CHAR_getWorkInt(char_index,
                                         CHAR_WORKFIXEARTHAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(char_index,
                                         CHAR_WORKFIXWATERAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(char_index,
                                         CHAR_WORKFIXFIREAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(char_index,
                                         CHAR_WORKFIXWINDAT_BOUNDARY)) == 0 &&
            boundaryclear) {
          int toNo = BATTLE_Index2No(battle_index, char_index);
          if (toNo < 10) // 右方
            snprintf(szWork, sizeof(szWork), "Ba|%X|%X|", 0, 1);
          else
            snprintf(szWork, sizeof(szWork), "Ba|%X|%X|", 0, 0);
          BATTLESTR_ADD(szWork);
          BATTLESTR_ADD("FF|");
        }
      }
#endif
    }
#ifdef _Item_ReLifeAct
    {
      // pEntry
      int k, userindex = -1;
      // if( BattleArray[ battle_index].type != BATTLE_TYPE_P_vs_P){
      for (k = 0; k < 20; k++) {
        userindex = BATTLE_getBattleDieIndex(battle_index, k);
        if (!CHAR_CHECKINDEX(userindex))
          continue;
        // Change fix CHECK_ITEM_RELIFE里面有检查了,这里不用
        // if( CHAR_getFlg( userindex, CHAR_ISDIE ) == FALSE )
        //	continue;
        if (CHAR_getInt(userindex, CHAR_HP) > 0)
          continue;
        if (CHAR_getInt(userindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          CHECK_ITEM_RELIFE(battle_index, userindex);
        }
#ifdef _LOSE_FINCH_
        else if (CHAR_getInt(userindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          CHECK_PET_RELIFE(battle_index, userindex);
        }
#endif
      }
    }
#endif
    BATTLESTR_ADD(szBadStatusString);
    BATTLE_AddProfit(battle_index, aAttackList); // 取得经验值
    len = pszBattleTop - szBattleString;
    if (AllSize + len >= sizeof(szAllBattleString) - 1) {
    // 満 で いたから 何もしない (缓冲区满则跳过)
    } else {
      memcpy(szAllBattleString + AllSize, szBattleString, len);
      szAllBattleString[AllSize + len] = 0; // NULL をセット (置NULL)
      AllSize += len;
    }
    if (BATTLE_CountAlive(battle_index, 0) == 0 ||
        BATTLE_CountAlive(battle_index, 1) == 0) {
      break;
    }
  }
#ifdef _PETSKILL_LER
  {
    register int i, dieindex;
    for (i = 0; i < 20; i++) {
      dieindex = BATTLE_getBattleDieIndex(battle_index, i);
      if (!CHAR_CHECKINDEX(dieindex))
        continue;
      if (CHAR_getFlg(dieindex, CHAR_ISDIE) == FALSE)
        continue;
      if (CHAR_getInt(dieindex, CHAR_HP) > 0)
        continue;
      // 雷尔死亡,变身
      if (CHAR_getInt(dieindex, CHAR_BASEBASEIMAGENUMBER) == 101813 ||
          CHAR_getInt(dieindex, CHAR_BASEBASEIMAGENUMBER) == 101814)
        BATTLE_LerChange(battle_index, dieindex, i);
    }
  }
#endif
  szBattleString[0] = 0;
  pszBattleTop = szBattleString;
  pszBattleLast = szBattleString + arraysizeof(szBattleString);
  // printf("前 szBattleString=%s\n",szBattleString);
  if (pBattle->field_att != BATTLE_ATTR_NONE) {
    pBattle->att_count--;
    if (pBattle->att_count <= 0) {
      pBattle->field_att = BATTLE_ATTR_NONE;
      // snprintf( szBuffer, sizeof(szBuffer),
      //	"field回复成无属性。"
      //);
      // BATTLE_BroadCast( battle_index, szBuffer, CHAR_COLORYELLOW );
      BATTLESTR_ADD("BV|15|0|");
    }
  }
  // printf("后 szBattleString=%s\n",szBattleString);
  strncatsafe(szAllBattleString, szBattleString, sizeof(szAllBattleString));
  for (i = 0; i < actionNum; i++) {
    char_index = sRoundActionList[i].char_index;
    if (!CHAR_CHECKINDEX(char_index))
      continue;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_RESCUE)
      continue;

    if (BATTLE_CommandSend(char_index, szAllBattleString) == TRUE) {
    }
  }
  pWatchBattle = pBattle->pNext;
  for (; pWatchBattle; pWatchBattle = pWatchBattle->pNext) {
    if (BATTLE_CHECKADDRESS(pWatchBattle) == FALSE) {
      print("err:观战battle address错误(%p)\n", pWatchBattle);
      break;
    }
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = pWatchBattle->Side[0].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      BATTLE_CommandSend(char_index, szAllBattleString);
    }
    pWatchBattle->mode = BATTLE_MODE_WATCHAFTER;
  }
  return 0;
}

#ifdef _Item_ReLifeAct
BOOL CHECK_ITEM_RELIFE(int battle_index, int toindex) {
  int i, item_index;
  CHAR_EquipPlace ep;
  typedef void (*DIERELIFEFUNC)(int, int, int);
  DIERELIFEFUNC Drf;

  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return FALSE;
  }

  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return FALSE;

  if (CHAR_getInt(toindex, CHAR_HP) > 0)
    return FALSE;

#ifdef _DUMMYDIE
  if (CHAR_getFlg(toindex, CHAR_ISDIE) == FALSE &&
      CHAR_getFlg(toindex, CHAR_ISDUMMYDIE) == FALSE)
#else
  if (CHAR_getFlg(toindex, CHAR_ISDIE) == FALSE)
#endif
    return FALSE;

  for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
    item_index = CHAR_getItemIndex(toindex, i);
    if (!ITEM_CHECKINDEX(item_index))
      continue;
    ep = ITEM_getEquipPlace(toindex, item_index);
    if (ep == -1)
      continue;
    Drf = (DIERELIFEFUNC)ITEM_getFunctionPointer(item_index, ITEM_DIERELIFEFUNC);

    if (Drf) {
      Drf(toindex, item_index, i);
      return TRUE;
    }
  }

  return FALSE;
}
#endif

#ifdef _LOSE_FINCH_
BOOL CHECK_PET_RELIFE(int battle_index, int petindex) {
  int ReceveEffect = -1;
  int toNo;
  int attackNo = -1;
  int WORK_HP = 1;
  int i;
  struct _RePet {
    int petid;
    int Effect;
    int Addhp;
    // Syu Mod
    //	}rePet[]={{1,SPR_fukkatu3,100},{-1,0,0}};
  } rePet[] = {{-1, 0, 0}};

  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return FALSE;

  if (CHAR_getFlg(petindex, CHAR_ISDIE) == FALSE) {
    print("\n PET_RELIFE:[ %d, CHAR_ISDIE FALSE] return !", petindex);
    return FALSE;
  }
  if (CHAR_getWorkInt(petindex, CHAR_WORKSPETRELIFE) != 0)
    return FALSE;
  for (i = 0; i < arraysizeof(rePet); i++) {
    if (CHAR_getInt(petindex, CHAR_PETID) == rePet[i].petid) // 朱雀
      break;
  }
  if (i == arraysizeof(rePet))
    return FALSE;
  // ReceveEffect = SPR_fukkatu3;
  ReceveEffect = rePet[i].Effect; // 朱雀动画 未定

  WORK_HP = CHAR_getWorkInt(petindex, CHAR_WORKMAXHP) * (rePet[i].Addhp / 100);
  toNo = BATTLE_Index2No(battle_index, petindex);
  attackNo = -1;
  BATTLE_MultiReLife(battle_index, attackNo, toNo, WORK_HP, ReceveEffect);
  CHAR_setWorkInt(petindex, CHAR_WORKSPETRELIFE, 1);
  return TRUE;
}
#endif

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MagicStatusSeq(int char_index) {
  int cnt, i;
  int battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);
  int bid = BATTLE_Index2No(battle_index, char_index);
  for (i = 1; i < MAXSTATUSTYPE; i++) {
    if ((cnt = CHAR_getWorkInt(char_index, MagicTbl[i])) <= 0)
      continue;
    CHAR_setWorkInt(char_index, MagicTbl[i], --cnt);
    if (cnt <= 0) {
      CHAR_setWorkInt(char_index, MagicTbl[i], 0);
      continue;
    }
  }
  return;
}
#endif

int Battle_getTotalBattleNum() { return Total_BattleNum; }

#ifdef _TYPE_TOXICATION
void CHAR_ComToxicationHp(int char_index) {
  int hp, dhp, nums;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  dhp = (CHAR_getWorkInt(char_index, CHAR_WORKMAXHP) / 100) + 1;
  if (dhp < 1)
    dhp = 1;
  hp = CHAR_getInt(char_index, CHAR_HP);
  hp -= dhp;
  nums = CHAR_getWorkInt(char_index, CHAR_WORKTOXICATION) + 1;
  CHAR_setWorkInt(char_index, CHAR_WORKTOXICATION, nums);
  if (hp <= 0) {
    int floor, x, y;
    char buf[256];
    int fd = getfdFromchar_index(char_index);
    sprintf(buf, "因中毒严重，昏迷回纪录点。");
    CHAR_talkToCli(char_index, -1, buf, CHAR_COLORYELLOW);
    CHAR_DischargePartyNoMsg(char_index); //解散团队
    if (CHAR_getElderPosition(CHAR_getInt(char_index, CHAR_LASTTALKELDER),
                              &floor, &x, &y) != TRUE) {
      CHAR_warpToSpecificPoint(char_index, 1006, 15, 15);
    } else {
      CHAR_warpToSpecificPoint(char_index, floor, x, y);
    }

    setToxication(fd, 0);
    hp = 1;
  } else if (nums % 10 == 0 || hp <= (dhp + 1) * 5) {
    char buf[256];
    sprintf(buf, "因中毒，HP剩余%d。", hp);
    CHAR_talkToCli(char_index, -1, buf, CHAR_COLORYELLOW);
  }
  CHAR_setInt(char_index, CHAR_HP, hp);
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_HP);
}
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能

void BATTLE_ProfessionStatus_init(int battle_index, int char_index) {
  int profession_class = CHAR_getInt(char_index, PROFESSION_CLASS);

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;
  if (profession_class <= PROFESSION_CLASS_NONE)
    return;

  // 初始化
  CHAR_setWorkInt(char_index, CHAR_WORK_WEAPON, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKMOD_WEAPON, 0);
  CHAR_setWorkInt(char_index, CHAR_WORK_P_DUCK, 0);
  CHAR_setWorkInt(char_index, CHAR_WORKMOD_P_DUCK, 0);

  {
    int i = 0, Pskillid = -1, skillid = -1;
    int value = 0, old_value = 0, WeaponType = -1;
    int rate = 0;
    char *skill_name, *pszOption, *pszP;
    CHAR_HaveSkill *hskill;

    for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {

      // 技能ID
      skillid = CHAR_getCharSkill(char_index, i);
      Pskillid = PROFESSION_SKILL_getskillArray(skillid);
      if (Pskillid <= 0)
        continue;

      // 技能名称
      skill_name =
          PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
      if (skill_name == "\0")
        continue;

      // 火熟练度
      if ((strcmp(skill_name, "PROFESSION_FIRE_PRACTICE")) == 0) {
        int old_practice = CHAR_getInt(char_index, PROFESSION_FIRE_P);
        int add_practice = 0;
        int skill_level = 0;
        // 技能等级
        hskill = CHAR_getCharHaveSkill(char_index, i);
        skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
        skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

        if (skill_level >= 6)
          add_practice = (skill_level - 5) * 3 + 10;
        else
          add_practice = skill_level * 2;
        if (add_practice > 25)
          add_practice = 25;

        add_practice += old_practice;
        // 因为old_practice=0所以add_practice为2~25

        CHAR_setWorkInt(char_index, CHAR_WORK_F_PROFICIENCY, add_practice);

      } else
        // 冰熟练度
        if ((strcmp(skill_name, "PROFESSION_ICE_PRACTICE")) == 0) {
          int old_practice = CHAR_getInt(char_index, PROFESSION_ICE_P);
          int add_practice = 0;
          int skill_level = 0;

          // 技能等级
          hskill = CHAR_getCharHaveSkill(char_index, i);
          skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
          skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

          if (skill_level >= 6)
            add_practice = (skill_level - 5) * 3 + 10;
          else
            add_practice = skill_level * 2;
          if (add_practice > 25)
            add_practice = 25;

          add_practice += old_practice;

          CHAR_setWorkInt(char_index, CHAR_WORK_I_PROFICIENCY, add_practice);

        } else
          // 雷熟练度
          if ((strcmp(skill_name, "PROFESSION_THUNDER_PRACTICE")) == 0) {
            int old_practice = CHAR_getInt(char_index, PROFESSION_THUNDER_P);
            int add_practice = 0;
            int skill_level = 0;

            // 技能等级
            hskill = CHAR_getCharHaveSkill(char_index, i);
            skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
            skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

            if (skill_level >= 6)
              add_practice = (skill_level - 5) * 3 + 10;
            else
              add_practice = skill_level * 2;
            if (add_practice > 25)
              add_practice = 25;

            add_practice += old_practice;

            CHAR_setWorkInt(char_index, CHAR_WORK_T_PROFICIENCY, add_practice);

          } else
            // 回避
            if ((strcmp(skill_name, "PROFESSION_AVOID")) == 0) {
              int skill_level = 0;

              if (profession_class !=
                  PROFESSION_SKILL_getInt(Pskillid,
                                          PROFESSION_SKILL_PROFESSION_CLASS))
                return;

              // 技能等级
              hskill = CHAR_getCharHaveSkill(char_index, i);
              skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
              skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

              if (skill_level <= 5)
                value = skill_level * 2;
              else
                value = (skill_level - 5) * 3;
              if (value > 25)
                value = 25;

              CHAR_setWorkInt(char_index, CHAR_WORK_P_DUCK, 1);
              CHAR_setWorkInt(char_index, CHAR_WORKMOD_P_DUCK, value);

            } else
              // 格档
              if ((strcmp(skill_name, "PROFESSION_DEFLECT")) == 0) {
                int skill_level = 0;
                int old_value = 0, add_value = 0;

                if (profession_class !=
                    PROFESSION_SKILL_getInt(Pskillid,
                                            PROFESSION_SKILL_PROFESSION_CLASS))
                  return;

                // 技能等级
                hskill = CHAR_getCharHaveSkill(char_index, i);
                skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

                old_value = CHAR_getWorkInt(char_index, CHAR_WORKFIXARRANGE);
                add_value = old_value + skill_level + 10;

                CHAR_setWorkInt(char_index, CHAR_WORKFIXARRANGE, add_value);
                CHAR_complianceParameter(char_index);

              } else
                // 武器专精
                if ((strcmp(skill_name, "PROFESSION_WEAPON_FOCUS")) == 0) {
                  int skill_level = 0;

                  rate = 0;
                  if (profession_class !=
                      PROFESSION_SKILL_getInt(
                          Pskillid, PROFESSION_SKILL_PROFESSION_CLASS))
                    continue;

                  // 技能等级
                  hskill = CHAR_getCharHaveSkill(char_index, i);
                  skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                  skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

                  // 技能参数
                  pszOption = PROFESSION_SKILL_getChar(Pskillid,
                                                       PROFESSION_SKILL_OPTION);
                  if (pszOption == "\0")
                    continue;

                  // 装备武器
                  WeaponType = BATTLE_GetWepon(char_index);

                  switch (WeaponType) {
                  case ITEM_AXE:
                    if ((pszP = strstr(pszOption, "斧")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_CLUB:
                    if ((pszP = strstr(pszOption, "棍")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_SPEAR:
                    if ((pszP = strstr(pszOption, "ǹ")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOW:
                    if ((pszP = strstr(pszOption, "弓")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOOMERANG: // 回旋标
                    if ((pszP = strstr(pszOption, "镖")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOUNDTHROW: // 投掷斧头
                    if ((pszP = strstr(pszOption, "Ͷ")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BREAKTHROW: // 投掷石
                    if ((pszP = strstr(pszOption, "ʯ")) != NULL)
                      rate = 1;
                    break;
                  default:
                    rate = 0;
                    break;
                  }

                  if (rate == 1) {
                    old_value =
                        CHAR_getWorkInt(char_index, CHAR_MYSKILLSTRPOWER);

                    if (skill_level <= 5)
                      value = skill_level * 2 + old_value;
                    else
                      value = (skill_level - 5) * 3 + 10 + old_value;

                    if (value > 25)
                      value = 25;

                    CHAR_setWorkInt(char_index, CHAR_WORK_WEAPON, 1);
                    CHAR_setWorkInt(char_index, CHAR_WORKMOD_WEAPON, value);
                  }
                }
    }
  }
  return;
}

void BATTLE_ProfessionStatusSeq(int battle_index, int char_index) {
  int profession_class = 0, i = 0, j = 0, Pskillid = -1, skillid = -1;
  int skill_level = 0, value = 0, old_value = 0;
  int bid = -1, hp = 0, flag = 0;
  char *skill_name, szcommand[256];
  CHAR_HaveSkill *hskill;

  int status_table[9] = {
      2,  //麻痹
      3,  // 睡眠
      4,  // 石化
      12, // 晕眩
      13, // 缠绕
      14, // 天罗
      15, // 冰暴
      17, // 冰箭
      23,
  }; //雷附体

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;

  profession_class = CHAR_getInt(char_index, PROFESSION_CLASS);
  if (profession_class <= PROFESSION_CLASS_NONE)
    return;

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
    skillid = CHAR_getCharSkill(char_index, i);
    Pskillid = PROFESSION_SKILL_getskillArray(skillid);
    if (Pskillid <= 0)
      return;
    skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
    if (skill_name == "\0")
      return;
    if ((strcmp(skill_name, "PROFESSION_REBACK")) == 0) {
      if (profession_class !=
          PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS))
        return;
      for (j = 0; j < 9; j++) {
        if ((CHAR_getWorkInt(char_index, StatusTbl[status_table[j]])) > 0) {
          flag = 1;
          break;
        }
      }

      if (flag > 0) {
        // 技能等级
        hskill = CHAR_getCharHaveSkill(char_index, i);
        skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
        skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);
        value = skill_level * 2;
        if (value > 20)
          value = 20;
        old_value = CHAR_getWorkInt(char_index, CHAR_WORKMAXHP);
        value = old_value * value / 100;
        hp = CHAR_getInt(char_index, CHAR_HP);
        if (value + hp > old_value)
          value = old_value - hp;

        if (value < 0)
          value = 0;

        CHAR_setInt(char_index, CHAR_HP, value + hp);

        bid = BATTLE_Index2No(battle_index, char_index);
        sprintf(szcommand, "BD|r%X|0|1|%X|", bid, value);
        BATTLESTR_ADD(szcommand);

        PROFESSION_SKILL_LVEVEL_UP(char_index, "PROFESSION_REBACK");
      }
    }

    if ((strcmp(skill_name, "PROFESSION_TRAP")) == 0) {
      int count = 0;
      count = CHAR_getWorkInt(char_index, CHAR_WORKTRAP);
      if (count > 0) {
        count--;
        CHAR_setWorkInt(char_index, CHAR_WORKTRAP, count);
      } else if (count == 0) {
        CHAR_setWorkInt(char_index, CHAR_WORKTRAP, 0);
        CHAR_setWorkInt(char_index, CHAR_WORKMODTRAP, 0);
      }
    }
  }

  return;
}

#endif

#ifdef _ALLBLUES_LUA_1_4
INLINE BOOL BATTLE_setLUAFunction(int battle_index, int functype, lua_State *L,
                                  const char *luafunctable) {
  if (!BATTLE_CHECKINDEX(battle_index) || functype < 0 ||
      functype >= BATTLE_FUNCTABLENUM || L == NULL || luafunctable == NULL)
    return FALSE;

  size_t name_length = strlen(luafunctable) + 1;
  char *name = allocateMemory(name_length);
  if (name == NULL)
    return FALSE;
  memcpy(name, luafunctable, name_length);

  if (BattleArray[battle_index].luafunctable[functype] != NULL)
    freeMemory(BattleArray[battle_index].luafunctable[functype]);
  BattleArray[battle_index].lua[functype] = L;
  BattleArray[battle_index].luafunctable[functype] = name;
  return TRUE;
}

INLINE lua_State *BATTLE_getLUAFunction(int battle_index, int functype) {
  if (!BATTLE_CHECKINDEX(battle_index) || functype < 0 ||
      functype >= BATTLE_FUNCTABLENUM ||
      BattleArray[battle_index].lua[functype] == NULL ||
      BattleArray[battle_index].luafunctable[functype] == NULL)
    return NULL;

  lua_getglobal(BattleArray[battle_index].lua[functype],
                BattleArray[battle_index].luafunctable[functype]);
  return BattleArray[battle_index].lua[functype];
}
#endif

int BATTLE_getType(int battle_index) {
  if (battle_index >= BATTLE_battlenum || battle_index < 0)
    return -1;

  return BattleArray[battle_index].type;
}
#ifdef _BATTLE_TIMESPEED
int BATTLE_getCreateTime(int battle_index) {
  if (battle_index >= BATTLE_battlenum || battle_index < 0)
    return -1;

  return BattleArray[battle_index].CreateTime;
}
#endif

int BATTLE_getBattleFloor(int battle_index) {
  if (battle_index >= BATTLE_battlenum || battle_index < 0)
    return -1;

  return BattleArray[battle_index].BattleFloor;
}
#ifdef _JZ_NEWSCRIPT_LUA
int SearchFmWarRandIndex(int char_index, int fmwarfloor) {
  int i = 1, j, fmwarindex;
  int bindex[100];
  bindex[0] = -1;
  for (j = 0; j < BATTLE_battlenum; j++) {
    if (BattleArray[j].use == FALSE) {
      continue;
    } else {
      if (BattleArray[j].floor != fmwarfloor) {
        continue;
      } else {
        bindex[i - 1] = j;
        i++;
        continue;
      }
    }
  }
  if (bindex[0] > -1) {
    fmwarindex = bindex[RAND(1, i) - 1];
    if (BATTLE_CHECKINDEX(fmwarindex) == FALSE)
      return -1;
    else {
      if (BattleArray[fmwarindex].floor != fmwarfloor)
        return -1;
      if (BATTLE_CreateForWatcher(char_index, fmwarindex) == 0) {
        int fd;
        if ((fd = getfdFromchar_index(char_index)) != -1) {
          GmsvServer_EN_send(fd, BattleArray[fmwarindex].type,
                             BattleArray[fmwarindex].field_no);
        }
        char szBuffer[256];
        sprintf(szBuffer, "BP|%X|%X|%X", 20, BP_FLG_JOIN,
                CHAR_getInt(char_index, CHAR_MP));
        BATTLE_CommandSend(char_index, szBuffer);
        return 0;
      }
    }
  }
  return -1;
}
#endif
