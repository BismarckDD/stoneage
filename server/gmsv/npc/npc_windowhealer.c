#include "version.h"
//
#include "npc_windowhealer.h"
//
#include "char.h"
#include "char_base.h"
#include "gmsv_server.h"
#include "npcutil.h"
#include "object.h"

#define RATE 1000

enum {
  CHAR_WORK_LEVEL = CHAR_NPCWORKINT1,
  CHAR_WORK_RANGE = CHAR_NPCWORKINT2,
  CHAR_WORK_HP = CHAR_NPCWORKINT3,
  CHAR_WORK_MP = CHAR_NPCWORKINT4,

};

enum WINDOWHEALER_SELECT_NUM {
  WINDOWHEALER_SELECT_START = 0,        /* 开始选择 */
  WINDOWHEALER_SELECT_HP = 1,           /* 耐久力回复 */
  WINDOWHEALER_SELECT_MP = 2,           /* 气力回复 */
  WINDOWHEALER_HEALED_HP = 3,           /* 耐久力已回复 */
  WINDOWHEALER_HEALED_MP = 4,           /* 气力已回复 */
  WINDOWHEALER_SELECT_ALL = 6,          /* 耐久力气力回复 */
  WINDOWHEALER_HEALED_ALL = 7,          /* 耐久力气力已回复 */
  WINDOWHEALER_NOT_ENOUGH_MONEY = 8,    /* 金钱不够 */
  WINDOWHEALER_PET_HEALED = 9,          /* 宠物回复 */
  WINDOWHEALER_PET_NO_NEED = 10,        /* 宠物回复(不必要) */
};

static void NPC_WindowHealer_selectWindow(int meindex, int toindex, int scenario_id);
void NPC_WindowHealerAllHeal(int talker, int mode);
BOOL NPC_WindowHealerLevelCheck(int meindex, int talker);
BOOL NPC_WindowMoneyCheck(int meindex, int talker, int mode);
int NPC_WindowCostCheck(int meindex, int talker);
BOOL NPC_PetHealerCheck(int talker);
int NPC_WindowCostCheckMp(int meindex, int talker);

BOOL NPC_WindowHealerInit(int meindex) {
  char *npcarg;
  char buf2[256];
  int range = 1;
  int rate;
  double drate;
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPEHEALER);
  npcarg = CHAR_getChar(meindex, CHAR_NPCARGUMENT);

  if (getStringFromIndexWithDelim(npcarg, "|", 1, buf2, sizeof(buf2)) !=
      FALSE) {
    CHAR_setWorkInt(meindex, CHAR_WORK_LEVEL, atoi(buf2));
  } else {
    return FALSE;
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 2, buf2, sizeof(buf2)) !=
      FALSE) {
    drate = atof(buf2);
    if (drate == 0) {
      rate = 500;
    } else {
      rate = (int)(drate * RATE);
    }
    CHAR_setWorkInt(meindex, CHAR_WORK_HP, rate);
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 3, buf2, sizeof(buf2)) !=
      FALSE) {
    drate = atof(buf2);
    if (drate == 0) {
      rate = 2000;
    } else {
      rate = (int)(drate * RATE);
    }
    CHAR_setWorkInt(meindex, CHAR_WORK_MP, rate);
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 4, buf2, sizeof(buf2)) !=
      FALSE) {
    range = atoi(buf2);
    if (range == 0) {
      range = 1;
    }
  }
  CHAR_setWorkInt(meindex, CHAR_WORK_RANGE, range);
  return TRUE;
}

void NPC_WindowHealerTalked(int meindex, int talkerindex, char *szMes,
                            int color) {
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  if (NPC_Util_CharDistance(talkerindex, meindex) >
      CHAR_getWorkInt(meindex, CHAR_WORK_RANGE)) {
    return;
  }
  if ((CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 0) ||
      (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 2)) {
    NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
  } else {
    int i = 0;
    int otherindex;

    for (i = 0; i < getPartyNum(talkerindex); i++)

    {
      otherindex = CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYINDEX1 + i);
      if (otherindex != -1) {
        NPC_WindowHealer_selectWindow(meindex, otherindex, WINDOWHEALER_SELECT_START);
      }
    }
  }
}
void NPC_WindowHealerLooked(int meindex, int lookedindex) {
  if (CHAR_getInt(lookedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }
  if (NPC_Util_CharDistance(lookedindex, meindex) >
      CHAR_getWorkInt(meindex, CHAR_WORK_RANGE)) {
    return;
  }
  NPC_WindowHealer_selectWindow(meindex, lookedindex, WINDOWHEALER_SELECT_START);
}

static void NPC_WindowHealer_selectWindow(int meindex, int toindex, int scenario_id) {

  char token[1024];
  char escapedname[1024];
  int fd = getfdFromCharaIndex(toindex);
  int buttontype = 0;
  int windowtype = 0;
  int windowno = 0;
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (scenario_id) {
  case WINDOWHEALER_SELECT_START: // 开始选择
    sprintf(token, "        哎呀！你受伤了吗？       \n\n"
                   "        ＜   耐久力回复  ＞      \n"
                   "        ＜   气力回复    ＞      \n"
                   "        ＜耐久力和气力回复＞      \n\n"
                   "        ＜宠物回复（免费）＞      ");
    buttontype = WINDOW_BUTTONTYPE_CANCEL;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG;
    break;
  case WINDOWHEALER_SELECT_HP: // 耐久力回复
    if (CHAR_getInt(toindex, CHAR_HP) ==
        CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "        ＜　耐久力回复　＞"
                       "\n\n\n\n        似乎没有必要回复唷！");
      } else {
        sprintf(token, "        ＜　耐久力回复　＞"
                       "\n\n        似乎没有必要回复唷！　"
                       "\n\n        因为宠物好像也受伤了！"
                       "\n        先帮他回复吧！");
        NPC_WindowHealerAllHeal(toindex, 0);
      }

      buttontype = WINDOW_BUTTONTYPE_OK;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG;
      break;
    } else if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "        ＜　耐久力回复　＞"
              "\n\n\n      是要回复耐久力没错吧！"
              "\n\n        现在的等级可以免费帮你回复唷！");

    } else {
      int gold = NPC_WindowCostCheck(meindex, toindex);
      sprintf(token, "        ＜　耐久力回复　＞"
              "\n\n\n　　　　　　　是要回复耐久力没错吧！　　　 "
              "\n\n        收您%d的STONE 。",
              gold);
    }
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG;
    break;
  case WINDOWHEALER_SELECT_MP:
    if (CHAR_getInt(toindex, CHAR_MP) ==
        CHAR_getWorkInt(toindex, CHAR_WORKMAXMP)) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "　　　　　　  ＜气力回复＞"
                       "\n\n\n\n　　　　似乎没有必要回复唷！　");
      } else {
        sprintf(token, "        ＜气力回复＞"
                       "\n\n        似乎没有必要回复唷！"
                       "\n\n        因为宠物好像也受伤了！"
                       "\n          先帮他回复吧！");
        NPC_WindowHealerAllHeal(toindex, 0);
      }
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
      break;
    }

    if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "        ＜气力回复＞"
              "\n\n        是要回复气力没错吧！"
              "\n\n        现在的等级可以免费帮你回复唷！");
    } else {
      sprintf(token, "        ＜气力回复＞"
              "\n\n        是要回复气力没错吧！"
              "\n\n        收您%d的STONE。", NPC_WindowCostCheckMp(meindex, toindex));
    }
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
    break;
  case WINDOWHEALER_HEALED_HP:
    sprintf(token, "\n        ＜耐久力已回复＞"
                   "\n      ＜宠物也已回复一般状态＞"
                   "\n\n       这样子就没问题了！");
    NPC_WindowHealerAllHeal(toindex, 1);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;
  case WINDOWHEALER_HEALED_MP:
    sprintf(token, "\n         ＜气力已回复＞"
                   "\n   ＜宠物也已回复一般状态＞"
                   "\n\n      这样子就没问题了！");
    NPC_WindowHealerAllHeal(toindex, 2);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG;
    break;
  case WINDOWHEALER_SELECT_ALL:
    int cost = 0;
    if (CHAR_getInt(toindex, CHAR_HP) <
        CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) {
      cost += NPC_WindowCostCheck(meindex, toindex);
    }
    if (CHAR_getInt(toindex, CHAR_MP) <
        CHAR_getWorkInt(toindex, CHAR_WORKMAXMP)) {
      cost = cost + NPC_WindowCostCheckMp(meindex, toindex);
    }
    if (cost == 0) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "        ＜耐久力气力回复＞"
                       "\n\n        似乎没有必要回复唷！");
      } else {
        sprintf(token, "        ＜耐久力气力回复＞"
                       "\n\n        似乎没有必要回复唷！"
                       "\n\n        因为宠物好像也受伤了！"
                       "\n          先帮他回复吧！");
        NPC_WindowHealerAllHeal(toindex, 0);
      }

      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
      break;

    } else {
      sprintf(token,
              "        ＜　耐久力气力回复　＞"
              "\n\n\n        是要回复耐久力气力没错吧！"
              "\n\n        收您%d的STONE 。", cost);
    }
    if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "        ＜　耐久力气力回复　＞"
                     "\n\n\n    是要回复耐久力气力没错吧！"
                     "\n\n　现在的等级可以免费帮你回复唷！");
    }
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG;
    break;

  case WINDOWHEALER_HEALED_ALL: // 一起回复
    sprintf(token, "    ＜耐久力气力已回复＞"
                   "\n  ＜宠物也已回复一般状态＞"
                   "\n\n\n 这样一来就回复健康了！");

    NPC_WindowHealerAllHeal(toindex, 3);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG;
    break;
  case WINDOWHEALER_NOT_ENOUGH_MONEY: // 身上金钱不够 
    sprintf(token, "\n\n\n\n 真可惜似乎所带的金钱不够唷！");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;

  case WINDOWHEALER_PET_HEALED: // 选择宠物回复且已回复
    sprintf(token, "        ＜宠物回复＞"
                   "\n\n\n        已经没问题了啦！"
                   "\n\n        但是太勉强的话也不行唷！");
    NPC_WindowHealerAllHeal(toindex, 0);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;
  case WINDOWHEALER_PET_NO_NEED: // 选择宠物回复但是没必要
    sprintf(token, "        ＜宠物回复＞"
                   "\n\n\n        似乎没有必要回复宠物的样子。"
                   "\n\n        但是太勉强的话也不行唷！");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;
  }
  makeEscapeString(token, escapedname, sizeof(escapedname));
  GmsvServer_WN_send(fd, windowtype, buttontype, windowno,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

void NPC_WindowHealerWindowTalked(int meindex, int talkerindex, int seqno,
                                  int select, char *data) {

  if (NPC_Util_CharDistance(talkerindex, meindex) >
      (CHAR_getWorkInt(meindex, CHAR_WORK_RANGE) + 4))
    return;

  switch (seqno) {
  case CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG:
    if (atoi(data) == 2) /*--蓟  仄凶嫩   1--*/
    {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_HP);

    } else if (atoi(data) == 3) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_MP);

    } else if (atoi(data) == 4) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_ALL);

    } else if (atoi(data) == 6) {
      if (NPC_PetHealerCheck(talkerindex) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_PET_HEALED);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_PET_NO_NEED);
      }
    } else if (select == WINDOW_BUTTONTYPE_CANCEL) {
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      /*--云嗯及民尼永弁--*/
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 1) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_HEALED_HP);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_NOT_ENOUGH_MONEY);
      }
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 2) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_HEALED_MP);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_NOT_ENOUGH_MONEY);
      }
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }

    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }
    break;

  /*--蝈荚汊今六月--*/
  case CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 3) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_HEALED_ALL);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_NOT_ENOUGH_MONEY);
      }

    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }
    break;

  /*--蝈荚汊及瑛绊--*/
  case CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG:
    if (select == WINDOW_BUTTONTYPE_YES) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, WINDOWHEALER_SELECT_START);
    }
    break;
  }

  if (select == WINDOW_BUTTONTYPE_CANCEL &&
      CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 2) {
    CHAR_sendWatchEvent(CHAR_getWorkInt(talkerindex, CHAR_WORKOBJINDEX),
                        CHAR_ACTPLEASURE, NULL, 0, TRUE);
    CHAR_setWorkInt(talkerindex, CHAR_WORKACTION, CHAR_ACTPLEASURE);
  }
}

BOOL NPC_PetHealerCheck(int talker) {

  int petindex;
  int i;

  /*--矢永玄反云引仃匹荚汊仄化丐仆月--*/
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);

    if (petindex == -1)
      continue;

    if (!CHAR_CHECKINDEX(talker))
      continue;

    if (CHAR_getInt(petindex, CHAR_HP) !=
        CHAR_getWorkInt(petindex, CHAR_WORKMAXHP)) {
      return TRUE;
    }
  }

  return FALSE;
}

void NPC_WindowHealerAllHeal(int talker, int mode) {
  int i;
  int petindex;
  char petsend[64];
  char msgbuf[64];

  if (mode == 1) {
    CHAR_setInt(talker, CHAR_HP, CHAR_getWorkInt(talker, CHAR_WORKMAXHP));
  } else if (mode == 2) {
    CHAR_setInt(talker, CHAR_MP, CHAR_getWorkInt(talker, CHAR_WORKMAXMP));
  } else if (mode == 3) {
    /*--蝈荚汊---*/
    CHAR_setInt(talker, CHAR_HP, CHAR_getWorkInt(talker, CHAR_WORKMAXHP));
    CHAR_setInt(talker, CHAR_MP, CHAR_getWorkInt(talker, CHAR_WORKMAXMP));
  }

  /*--矢永玄反云引仃匹荚汊仄化丐仆月--*/
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);

    if (petindex == -1)
      continue;

    if (!CHAR_CHECKINDEX(talker))
      continue;

    /*--荚汊--*/
    CHAR_setFlg(petindex, CHAR_ISDIE, 0);
    CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
    CHAR_setInt(petindex, CHAR_MP, CHAR_getWorkInt(petindex, CHAR_WORKMAXMP));

    /*--由仿丢□正譬帮--*/
    CHAR_complianceParameter(petindex);
    sprintf(petsend, "K%d", i);
    CHAR_sendStatusString(talker, petsend);
  }

  /*--由仿丢□正霜曰--*/
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_HP);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_MP);

  /*---醮棉互中木壬醮棉卞手霜耨--*/
  if (CHAR_getWorkInt(talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    int topartyarray = -1;
    int oyaindex = CHAR_getWorkInt(talker, CHAR_WORKPARTYINDEX1);

    if (CHAR_CHECKINDEX(oyaindex)) {
      int i;

      for (i = 0; i < getPartyNum(oyaindex); i++) {
        int workindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + i);

        if (CHAR_CHECKINDEX(workindex)) {
          if (workindex == talker) {
            topartyarray = i;
            break;
          }
        }
      }
      for (i = 0; i < getPartyNum(talker); i++) {
        int otherindex = CHAR_getPartyIndex(talker, i);

        /* 醮棉由仿丢□正毛霜月 */
        if (CHAR_CHECKINDEX(otherindex)) {
          snprintf(msgbuf, sizeof(msgbuf), "N%d", topartyarray);
          if (otherindex != talker) {
            CHAR_sendStatusString(otherindex, msgbuf);
          }
        }
      }
    }
  }
}

/*-------------------------------------
伊矛伙民尼永弁
        涩烂伊矛伙动晓卅日	FALSE
---------------------------------------*/
BOOL NPC_WindowHealerLevelCheck(int meindex, int talker) {
  int level;

  level = CHAR_getWorkInt(meindex, CHAR_WORK_LEVEL);

  if (level > CHAR_getInt(talker, CHAR_LV)) {
    return TRUE;
  }

  return FALSE;
}

/*-------------------------------------------
娄醒
talker’’’’平乓仿弁正□及奶件犯永弁旦
---------------------------------------------*/
BOOL NPC_WindowMoneyCheck(int meindex, int talker, int mode) {
  int cost = 0;
  int level;

  level = CHAR_getWorkInt(meindex, CHAR_WORK_LEVEL);

  if (mode == 1) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {
      /*---云嗯毛喃曰请允---*/
      cost = NPC_WindowCostCheck(meindex, talker);
      /*--蜇箕反移涩烂---*/
      /*---云嗯互箫曰月井升丹井及民尼永弁---*/
      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }
      CHAR_DelGold(talker, cost);
    }
  }
  if (mode == 2) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {
      /*---云嗯毛喃曰请允---*/
      cost = NPC_WindowCostCheckMp(meindex, talker);

      /*--蜇箕反移涩烂---*/
      /*---云嗯互箫曰月井升丹井及民尼永弁---*/
      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }
      CHAR_DelGold(talker, cost);
    }
  }

  if (mode == 3) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {

      if (CHAR_getInt(talker, CHAR_HP) <
          CHAR_getWorkInt(talker, CHAR_WORKMAXHP)) {
        cost = NPC_WindowCostCheck(meindex, talker);
      }

      if (CHAR_getInt(talker, CHAR_MP) <
          CHAR_getWorkInt(talker, CHAR_WORKMAXMP)) {
        cost = cost + NPC_WindowCostCheckMp(meindex, talker);
      }

      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }

      CHAR_DelGold(talker, cost);
    }
  }

  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  return TRUE;
}

/*-----------------------------------------
戊旦玄及煌遥
------------------------------------------*/
int NPC_WindowCostCheck(int meindex, int talker) {
  int cost;
  double drate;
  int rate;

  rate = CHAR_getWorkInt(meindex, CHAR_WORK_HP);
  drate = (double)rate / 1000;
  cost = CHAR_getInt(talker, CHAR_LV);
  cost = (int)cost * drate;
  if (cost < 1)
    cost = 1;
  return cost;
}

/*-----------------------------------------
------------------------------------------*/
int NPC_WindowCostCheckMp(int meindex, int talker) {
  int cost;
  double drate;
  int rate;

  rate = CHAR_getWorkInt(meindex, CHAR_WORK_MP);
  drate = (double)rate / 1000;
  cost = CHAR_getInt(talker, CHAR_LV);
  cost = (int)cost * drate;
  if (cost == 0)
    cost = 1;
  return cost;
}
