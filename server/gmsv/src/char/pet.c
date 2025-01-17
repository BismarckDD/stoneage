#include "version.h"
#include "battle.h"
#include "pet.h"
#include "function.h"
#include "handletime.h"
#include "log.h"
#include "lssproto_serv.h"
#include "map_deal.h"
#include "object.h"
#include "petmail.h"
#include "readmap.h"

int PET_DEBUG_initPetOne(int charaindex) {
  Char ch;
  int havepetindex;
  int index;
  havepetindex = CHAR_getCharPetElement(charaindex);

  memset(&ch, 0, sizeof(ch));
  if (!CHAR_getDefaultChar(&ch, 31010))
    return -1;

  ch.data[CHAR_BASEBASEIMAGENUMBER] = ch.data[CHAR_BASEIMAGENUMBER] = 30008;
  ch.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
  ch.workint[CHAR_WORKATTACKPOWER] = 100;
  ch.workint[CHAR_WORKDEFENCEPOWER] = 50;
  ch.data[CHAR_HP] = 100;
  strcpysafe(ch.string[CHAR_NAME].string, 32, "宠物１");
  index = PET_initCharOneArray(&ch);

  if (index < 0)
    return -1;

  CHAR_setWorkInt(index, CHAR_WORKPLAYERINDEX, charaindex);
  CHAR_setWorkInt(index, CHAR_WORKOBJINDEX, -1);
  CHAR_setCharPet(charaindex, havepetindex, index);
  CHAR_setInt(index, CHAR_SLOT, 2);
  return havepetindex;
}

static int _PET_dropPet(int charaindex, int havepetindex, int tofl, int tox,
                        int toy) {
  char szPet[128];
  int dirx[9], diry[9];
  int i, j;
  int objindex = -1;
  int floor, x, y;
  int petindex;
  int count_chara = 0, count_item = 0;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return FALSE;
  petindex = CHAR_getCharPet(charaindex, havepetindex);
  if (!CHAR_CHECKINDEX(petindex))
    return FALSE;
  if (!CHAR_CHECKINDEX(charaindex))
    return FALSE;
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return FALSE;

#ifdef _AVID_TRADETRYBUG // 丢出宠物
  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING) {
    CHAR_talkToCli(charaindex, -1, "交易状态中无法丢出宠物。",
                   CHAR_COLORYELLOW);
    return FALSE;
  }
#endif
  // CoolFish: Family 2001/6/13
  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
    CHAR_talkToCli(charaindex, -1, "家族守护兽无法丢出！", CHAR_COLORYELLOW);
    return FALSE;
  }
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) == havepetindex) {
    CHAR_talkToCli(charaindex, -1, "骑乘中的宠物无法丢出！", CHAR_COLORYELLOW);
    return FALSE;
  }
#ifdef _LOCK_PET_ITEM
  {
    char *name = CHAR_getChar(petindex, CHAR_NAME);
    char *owntile = CHAR_getChar(petindex, CHAR_OWNTITLE);
    if (name[0] == '*' || owntile[0] == '*') {
      CHAR_talkToCli(charaindex, -1, "绑定宠物无法丢出！", CHAR_COLORYELLOW);
      return FALSE;
    }
  }
#endif
#ifdef _DROPCHECK2
  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  x = CHAR_getInt(charaindex, CHAR_X);
  y = CHAR_getInt(charaindex, CHAR_Y);
  for (i = x - CHAR_DEFAULTSEESIZ / 2; i <= x + CHAR_DEFAULTSEESIZ / 2; i++) {
    for (j = y - CHAR_DEFAULTSEESIZ / 2; j <= y + CHAR_DEFAULTSEESIZ / 2; j++) {
      OBJECT object;
      for (object = MAP_getTopObj(floor, i, j); object;
           object = NEXT_OBJECT(object)) {
        int objindex = GET_OBJINDEX(object);
        if (OBJECT_getType(objindex) == OBJTYPE_NOUSE)
          continue;

        if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
          count_chara++;
        }
        if (OBJECT_getType(objindex) == OBJTYPE_ITEM ||
            OBJECT_getType(objindex) == OBJTYPE_GOLD) {
          count_item++;
        }
      }
    }
    if (count_item > 80 || count_chara > 80) {
      CHAR_talkToCli(charaindex, -1, "这里已经太拥挤了，不能再丢了。",
                     CHAR_COLORYELLOW);
      return FALSE;
    }
  }
#endif
  if (tofl == -1) {
    for (i = 0; i < 7; i++) {
      dirx[i + 2] = CHAR_getDX(CHAR_getInt(charaindex, CHAR_DIR) + i + 1);
      diry[i + 2] = CHAR_getDY(CHAR_getInt(charaindex, CHAR_DIR) + i + 1);
    }
    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex, CHAR_DIR));
    diry[0] = CHAR_getDY(CHAR_getInt(charaindex, CHAR_DIR));
    dirx[1] = 0;
    diry[1] = 0;
    floor = CHAR_getInt(charaindex, CHAR_FLOOR);
    for (i = 0; i < 9; i++) {
      int x = CHAR_getInt(charaindex, CHAR_X) + dirx[i];
      int y = CHAR_getInt(charaindex, CHAR_Y) + diry[i];
      if (PET_isPutPoint(floor, x, y) == TRUE) {
        break;
      }
    }
    if (i == 9)
      i = 1;
    x = CHAR_getInt(charaindex, CHAR_X) + dirx[i];
    y = CHAR_getInt(charaindex, CHAR_Y) + diry[i];
  } else {
    if (MAP_walkAbleFromPoint(tofl, tox, toy, FALSE) == FALSE) {
      print("map walkable err %s:%d\n", __FILE__, __LINE__);
      return FALSE;
    }
    floor = tofl;
    x = tox;
    y = toy;
  }
  objindex = PET_dropPetAbsolute(petindex, floor, x, y, FALSE);
  if (objindex == -1)
    return FALSE;

  CHAR_setWorkInt(petindex, CHAR_WORKOBJINDEX, objindex);
  CHAR_setCharPet(charaindex, havepetindex, -1);
  CHAR_setInt(petindex, CHAR_FLOOR, floor);
  CHAR_setInt(petindex, CHAR_X, x);
  CHAR_setInt(petindex, CHAR_Y, y);
  CHAR_setInt(petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
  if (havepetindex == CHAR_getInt(charaindex, CHAR_DEFAULTPET)) {
    int fd;
    CHAR_setInt(charaindex, CHAR_DEFAULTPET, -1);
    fd = getfdFromCharaIndex(charaindex);
    lssproto_KS_send(fd, havepetindex, 0);
  }
  CHAR_sendCToArroundCharacter(objindex);
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _PET_ITEM
    CHAR_sendPetItemEmpty(charaindex, havepetindex);
#endif
    snprintf(szPet, sizeof(szPet), "K%d", havepetindex);
    CHAR_sendStatusString(charaindex, szPet);
  }
  return TRUE;
}

int PET_dropPetAbsolute(int petindex, int floor, int x, int y, BOOL net) {
  Object object;
  if (!CHAR_CHECKINDEX(petindex))
    return FALSE;
  object.type = OBJTYPE_CHARA;
  object.index = petindex;
  object.x = x;
  object.y = y;
  object.floor = floor;
  int objindex = initObjectOne(&object);
  if (net)
    CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);

  return objindex;
}

BOOL PET_isPutPoint(int fl, int x, int y) {
  OBJECT object;
  if (MAP_walkAbleFromPoint(fl, x, y, FALSE) == FALSE)
    return FALSE;
  for (object = MAP_getTopObj(fl, x, y); object; object = NEXT_OBJECT(object)) {
    int objindex = GET_OBJINDEX(object);
    switch (OBJECT_getType(objindex)) {
    case OBJTYPE_NOUSE:
      break;
    case OBJTYPE_ITEM:
    case OBJTYPE_GOLD:
    case OBJTYPE_CHARA:
      return FALSE;
      break;
    default:
      break;
    }
  }
  return TRUE;
}

int PET_dropPet(int charaindex, int havepetindex) {
  int petindex;
  petindex = CHAR_getCharPet(charaindex, havepetindex);
  if (!CHAR_CHECKINDEX(petindex))
    return FALSE;

  if (_PET_dropPet(charaindex, havepetindex, -1, -1, -1) == TRUE) {
    LogPet(CHAR_getChar(charaindex, CHAR_NAME), /* 平乓仿   */
           CHAR_getChar(charaindex, CHAR_CDKEY),
           CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
           "Drop(丢宠)", CHAR_getInt(charaindex, CHAR_FLOOR),
           CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
           CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
    );
    return 1;
  }
  return 0;
}

int PET_dropPetFLXY(int charaindex, int havepetindex, int fl, int x, int y) {
  return _PET_dropPet(charaindex, havepetindex, fl, x, y);
}

int PET_initCharOneArray(Char *ch) {
  int i;
  char *tmp[CHAR_FUNCTABLENUM] = {
      "",              /*  CHAR_INITFUNC */
      "",              /*  CHAR_WALKPREFUNC    */
      "",              /*  CHAR_WALKPOSTFUNC   */
      "",              /*  CHAR_PREOVERFUNC    */
      "",              /*  CHAR_PREOVERFUNC    */
      "core_PetWatch", /*  CHAR_WATCHFUNC  */
      "",              /*  CHAR_LOOPFUNC */
      "",              /*  CHAR_DYINGFUNC */
      "core_PetTalk",  /*  CHAR_TALKEDFUNC */
      "",              /*  CHAR_PREATTACKEDFUNC    */
      "",              /*  CHAR_POSTATTACKEDFUNC    */
      "",              /*  CHAR_OFFFUNC    */
      "",              /*  CHAR_LOOKEDFUNC */
      "",              /*  CHAR_ITEMPUTFUNC    */
      "",              /*  CHAR_SPECIALTALKEDFUNC    */
      "",              /*  CHAR_WINDOWTALKEDFUNC    */
#ifdef _USER_CHARLOOPS
      "", // CHAR_LOOPFUNCTEMP1,
      "", // CHAR_LOOPFUNCTEMP2,
      "", // CHAR_BATTLEPROPERTY,
#endif
  };
  for (i = 0; i < CHAR_FUNCTABLENUM; i++) {
    strcpysafe(ch->charfunctable[i].string, sizeof(ch->charfunctable[i]),
               tmp[i]);
  }
  if (ch->data[CHAR_MAILMODE] != CHAR_PETMAIL_NONE) {
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNC].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNC]), "PETMAIL_Loop");
  }

#ifdef _PET_FUSION
  if (ch->data[CHAR_FUSIONBEIT] == 1 && ch->data[CHAR_FUSIONRAISE] > 0) {
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]),
               "PET_CheckIncubateLoop");
    ch->data[CHAR_LOOPINTERVAL] = 60000;

    ch->functable[CHAR_LOOPFUNCTEMP1] =
        getFunctionPointerFromName("PET_CheckIncubateLoop");
    //    CHAR_constructFunctable( petindex);
  }
#endif
  int petindex = CHAR_initCharOneArray(ch);

  return petindex;
}

int PET_createPetFromCharaIndex(int charaindex, int enemyindex) {
  Char CharNew;
  int newindex;
  int havepetelement;
  char szPet[128];
  int i;

  havepetelement = CHAR_getCharPetElement(charaindex);
  if (havepetelement < 0)
    return -1;
  memset(&CharNew, 0, sizeof(Char));
  if (!CHAR_getDefaultChar(&CharNew, 31010))
    return -1;
  CharNew.data[CHAR_BASEBASEIMAGENUMBER] = CharNew.data[CHAR_BASEIMAGENUMBER] =
      CHAR_getInt(enemyindex, CHAR_BASEIMAGENUMBER);
  CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
  CharNew.data[CHAR_HP] = CHAR_getInt(enemyindex, CHAR_HP);
  CharNew.data[CHAR_MP] = CHAR_getInt(enemyindex, CHAR_MP);
  CharNew.data[CHAR_MAXMP] = CHAR_getInt(enemyindex, CHAR_MAXMP);
  CharNew.data[CHAR_VITAL] = CHAR_getInt(enemyindex, CHAR_VITAL);
  CharNew.data[CHAR_STR] = CHAR_getInt(enemyindex, CHAR_STR);
  CharNew.data[CHAR_TOUGH] = CHAR_getInt(enemyindex, CHAR_TOUGH);
  CharNew.data[CHAR_DEX] = CHAR_getInt(enemyindex, CHAR_DEX);
  CharNew.data[CHAR_LUCK] = CHAR_getInt(enemyindex, CHAR_LUCK);
  CharNew.data[CHAR_FIREAT] = CHAR_getInt(enemyindex, CHAR_FIREAT);
  CharNew.data[CHAR_WATERAT] = CHAR_getInt(enemyindex, CHAR_WATERAT);
  CharNew.data[CHAR_EARTHAT] = CHAR_getInt(enemyindex, CHAR_EARTHAT);
  CharNew.data[CHAR_WINDAT] = CHAR_getInt(enemyindex, CHAR_WINDAT);
  // CharNew.data[CHAR_EXP]      = CHAR_getInt(enemyindex, CHAR_EXP);

  CharNew.data[CHAR_SLOT] = CHAR_getInt(enemyindex, CHAR_SLOT);
  CharNew.data[CHAR_MODAI] = CHAR_getInt(enemyindex, CHAR_MODAI);
  CharNew.data[CHAR_LV] = CHAR_getInt(enemyindex, CHAR_LV);
  CharNew.data[CHAR_POISON] = CHAR_getInt(enemyindex, CHAR_POISON);
  CharNew.data[CHAR_PARALYSIS] = CHAR_getInt(enemyindex, CHAR_PARALYSIS);
  CharNew.data[CHAR_SLEEP] = CHAR_getInt(enemyindex, CHAR_SLEEP);
  CharNew.data[CHAR_STONE] = CHAR_getInt(enemyindex, CHAR_STONE);
  CharNew.data[CHAR_DRUNK] = CHAR_getInt(enemyindex, CHAR_DRUNK);
  CharNew.data[CHAR_CONFUSION] = CHAR_getInt(enemyindex, CHAR_CONFUSION);
  CharNew.data[CHAR_RARE] = CHAR_getInt(enemyindex, CHAR_RARE);
  CharNew.data[CHAR_PETRANK] = CHAR_getInt(enemyindex, CHAR_PETRANK);
  CharNew.data[CHAR_PETID] = CHAR_getInt(enemyindex, CHAR_PETID);
  CharNew.data[CHAR_PETENEMYID] = CHAR_getInt(enemyindex, CHAR_PETENEMYID);
  CharNew.data[CHAR_CRITIAL] = CHAR_getInt(enemyindex, CHAR_CRITIAL);
  CharNew.data[CHAR_COUNTER] = CHAR_getInt(enemyindex, CHAR_COUNTER);
  CharNew.data[CHAR_PETMAILEFFECT] = RAND(0, PETMAIL_EFFECTMAX);

  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    CharNew.unionTable.indexOfPetskill[i] = CHAR_getPetSkill(enemyindex, i);
  }
  CharNew.data[CHAR_ALLOCPOINT] = CHAR_getInt(enemyindex, CHAR_ALLOCPOINT);
  strcpysafe(CharNew.string[CHAR_NAME].string,
             sizeof(CharNew.string[CHAR_NAME].string),
             CHAR_getChar(enemyindex, CHAR_NAME));
  newindex = PET_initCharOneArray(&CharNew);
  if (newindex < 0) {
    return -1;
  }

  CHAR_setMaxExpFromLevel(newindex, CHAR_getInt(enemyindex, CHAR_LV));
  CHAR_complianceParameter(newindex);
#ifdef _PETCOM_
  CHAR_setInt(newindex, CHAR_YHP, CHAR_getWorkInt(newindex, CHAR_WORKMAXHP));
  CHAR_setInt(newindex, CHAR_YATK, CHAR_getWorkInt(newindex, CHAR_WORKFIXSTR));
  CHAR_setInt(newindex, CHAR_YDEF,
              CHAR_getWorkInt(newindex, CHAR_WORKFIXTOUGH));
  CHAR_setInt(newindex, CHAR_YQUICK,
              CHAR_getWorkInt(newindex, CHAR_WORKFIXDEX));
  CHAR_setInt(newindex, CHAR_YLV, CHAR_getInt(newindex, CHAR_LV));
#endif
  CHAR_setWorkInt(newindex, CHAR_WORKPLAYERINDEX, charaindex);
  CHAR_setCharPet(charaindex, havepetelement, newindex);
  CHAR_setChar(newindex, CHAR_OWNERCDKEY, CHAR_getChar(charaindex, CHAR_CDKEY));
  CHAR_setChar(newindex, CHAR_OWNERCHARANAME,
               CHAR_getChar(charaindex, CHAR_NAME));
  snprintf(szPet, sizeof(szPet), "K%d", havepetelement);
  CHAR_sendStatusString(charaindex, szPet);
  snprintf(szPet, sizeof(szPet), "W%d", havepetelement);
  CHAR_sendStatusString(charaindex, szPet);

  return newindex;
}
BOOL PET_SelectBattleEntryPet(int charaindex, int petarray) {
  int pindex;
  if (!CHAR_CHECKINDEX(charaindex))
    return FALSE;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return FALSE;

  if (petarray == -1) {
    CHAR_setInt(charaindex, CHAR_DEFAULTPET, -1);
    return TRUE;
  }
  if (!CHAR_CHECKPETINDEX(petarray))
    return FALSE;
  pindex = CHAR_getCharPet(charaindex, petarray);
  if (!CHAR_CHECKINDEX(pindex))
    return FALSE;

  if (CHAR_getFlg(pindex, CHAR_ISDIE))
    return FALSE;

#ifdef _PET_VALIDITY
  if (CHAR_getInt(pindex, CHAR_PETVALIDITY) > 0 &&
      CHAR_getInt(pindex, CHAR_PETVALIDITY) < time(NULL)) {
    CHAR_talkToCli(charaindex, -1, "该宠物已经失效了！", CHAR_COLORYELLOW);
    int s_pet = CHAR_getWorkInt(charaindex, CHAR_WORKSTANDBYPET);
    s_pet ^= (1 << petarray);
    CHAR_setWorkInt(charaindex, CHAR_WORKSTANDBYPET, s_pet);

    lssproto_SPET_send(getfdFromCharaIndex(charaindex), s_pet, TRUE);
    return FALSE;
  }
#endif
#ifdef _MO_LUA_KS_CALLBACK
  if (FreeSelectBattlePet(charaindex, petarray) == FALSE) {
    return FALSE;
  }
#endif
  CHAR_setInt(charaindex, CHAR_DEFAULTPET, petarray);

  return TRUE;
}

// Robin 0707 petFollow <charaindex>
#if 1
// 人物索引,宠物位置,地图,X,Y
int PET_dropPetFollow(int charaindex, int havepetindex, int tofl, int tox,
                      int toy) {
  char szPet[128];
  int dirx[9], diry[9];
  int i;
  int objindex = -1;
  int floor, x, y;
  int petindex;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return FALSE;
  petindex = CHAR_getCharPet(charaindex, havepetindex);
  if (petindex == -1)
    return FALSE;
  if (!CHAR_CHECKINDEX(charaindex))
    return FALSE;
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return FALSE;

  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
    CHAR_talkToCli(charaindex, -1, "家族守护兽无法丢出！", CHAR_COLORYELLOW);
    return FALSE;
  }
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) == havepetindex) {
    CHAR_talkToCli(charaindex, -1, "骑乘中的宠物无法跟随！", CHAR_COLORYELLOW);
    return FALSE;
  }
  if (tofl == -1) {
    for (i = 0; i < 7; i++) {
      dirx[i + 2] = CHAR_getDX(CHAR_getInt(charaindex, CHAR_DIR) + i + 1);
      diry[i + 2] = CHAR_getDY(CHAR_getInt(charaindex, CHAR_DIR) + i + 1);
    }
    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex, CHAR_DIR));
    diry[0] = CHAR_getDY(CHAR_getInt(charaindex, CHAR_DIR));
    dirx[1] = 0;
    diry[1] = 0;

    floor = CHAR_getInt(charaindex, CHAR_FLOOR);
    for (i = 0; i < 9; i++) {
      int x = CHAR_getInt(charaindex, CHAR_X) + dirx[i];
      int y = CHAR_getInt(charaindex, CHAR_Y) + diry[i];
      if (PET_isPutPoint(floor, x, y) == TRUE) {
        break;
      }
    }
    if (i == 9)
      i = 1;

    x = CHAR_getInt(charaindex, CHAR_X) + dirx[i];
    y = CHAR_getInt(charaindex, CHAR_Y) + diry[i];
  } else {
    if (MAP_walkAbleFromPoint(tofl, tox, toy, FALSE) == FALSE) {
      print("map walkable err %s:%d\n", __FILE__, __LINE__);
      return FALSE;
    }
    floor = tofl;
    x = tox;
    y = toy;
  }

  objindex = PET_dropPetAbsolute(petindex, floor, x, y, FALSE);
  if (objindex == -1)
    return FALSE;

  CHAR_setWorkInt(petindex, CHAR_WORKOBJINDEX, objindex);
  CHAR_setCharPet(charaindex, havepetindex, -1);
  CHAR_setInt(petindex, CHAR_FLOOR, floor);
  CHAR_setInt(petindex, CHAR_X, x);
  CHAR_setInt(petindex, CHAR_Y, y);
  CHAR_setInt(petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
  if (havepetindex == CHAR_getInt(charaindex, CHAR_DEFAULTPET)) {
    int fd;
    CHAR_setInt(charaindex, CHAR_DEFAULTPET, -1);
    fd = getfdFromCharaIndex(charaindex);
    lssproto_KS_send(fd, havepetindex, 0);
  }
  CHAR_sendCToArroundCharacter(objindex);
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _PET_ITEM
    CHAR_sendPetItemData(charaindex, havepetindex);
#endif
    snprintf(szPet, sizeof(szPet), "K%d", havepetindex);
    CHAR_sendStatusString(charaindex, szPet);
  }

#ifdef _PETFOLLOW_NEW_

  int petnum;
  petnum = 0;
  for (; petnum < 5; petnum++) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKPETFOLLOW + petnum) == -1) {
      CHAR_setWorkInt(charaindex, CHAR_WORKPETFOLLOW + petnum, petindex);
      break;
    }
  }
#else
  CHAR_setWorkInt(charaindex, CHAR_WORKPETFOLLOW, petindex);
#endif

  CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NOW);
  CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
  CHAR_setInt(petindex, CHAR_PUTPETTIME, (int)(NowTime.tv_sec));
  CHAR_setInt(petindex, CHAR_WORKPLAYERINDEX, charaindex);
  LogPet(CHAR_getChar(charaindex, CHAR_NAME),
         CHAR_getChar(charaindex, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "Follow(溜宠)", CHAR_getInt(charaindex, CHAR_FLOOR),
         CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE));
  return 1;
}
#endif

BOOL PET_getBaseForAllocpoint(int toindex, int *work) {
  int LevelUpPoint = 0;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
    return FALSE;
  LevelUpPoint = CHAR_getInt(toindex, CHAR_ALLOCPOINT);
  work[3] = ((LevelUpPoint >> 24) & 0xFF);
  work[0] = ((LevelUpPoint >> 16) & 0xFF);
  work[1] = ((LevelUpPoint >> 8) & 0xFF);
  work[2] = ((LevelUpPoint >> 0) & 0xFF);
  return TRUE;
}

void PET_showEditBaseMsg(int charaindex, int toindex, int itemindex,
                         int *work) {
  int i, maxnums = 6000;
  char buf1[256];
  char buf2[][32] = {"腕力成长率", "耐久力成长率", "速度成长率", "体力成长率",
                     "能力"};
  char buf3[][32] = {"大幅提高", "略为提高", "略为减少"};

  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
    return;
  memset(buf1, 0, sizeof(buf1));

  for (i = 0; i < 4; i++) {
    int type = ITEM_getInt(itemindex, (ITEM_MODIFYATTACK + i));
    //    print(" 喂[%d]%d+%d ", i, work[i], type);
    work[i] += type;
    strcpy(buf1, "\0");
    if (work[i] > maxnums) {
      sprintf(buf1, "%s 已经达到最高了。", buf2[i]);
      work[i] = maxnums;
    } else if (work[i] < 0) {
      sprintf(buf1, "%s 已经为零了。", buf2[i]);
      work[i] = 0;
    } else {
      if (type > 0) {
        if (type > 2) {
          sprintf(buf1, "%s %s %s", buf2[i], buf3[0], "。");
        } else {
          sprintf(buf1, "%s %s %s", buf2[i], buf3[1], "。");
        }
      } else if (type < 0) {
        sprintf(buf1, "%s %s %s", buf2[i], buf3[2], "。");
      }
    }
    if (strcmp(buf1, "\0")) {
      CHAR_talkToCli(charaindex, toindex, buf1, CHAR_COLORYELLOW);
    }
  }
}

#ifdef _PET_EVOLUTION
BOOL PET_getBaseAndSkill(int charaindex, int baseindex, int *base, int *skill,
                         int flg) {
  int i;
  if (!CHAR_CHECKINDEX(baseindex))
    return FALSE;
  if (base != NULL) {
    int levelup = CHAR_getInt(baseindex, CHAR_ALLOCPOINT);
    base[0] = ((levelup >> 24) & 0xFF);
    base[1] = ((levelup >> 16) & 0xFF);
    base[2] = ((levelup >> 8) & 0xFF);
    base[3] = ((levelup >> 0) & 0xFF);
  }
  if (skill != NULL) {
    for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
      skill[i] = CHAR_getPetSkill(baseindex, i);
    }
  }
  return TRUE;
}

#endif
BOOL CHAR_DelPetForIndex(int charaindex, int petindex) {
  int i;
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    int pindex = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(pindex))
      continue;
    if (pindex == petindex)
      break;
  }

  if (i >= CHAR_MAXPETHAVE) {
    return FALSE;
  } else {
    char szPet[256];
    char msgbuf[256];
    CHAR_setCharPet(charaindex, i, -1);
    snprintf(szPet, sizeof(szPet), "K%d", i);
    CHAR_sendStatusString(charaindex, szPet);

    snprintf(msgbuf, sizeof(msgbuf), "交出%s。",
             CHAR_getChar(petindex, CHAR_NAME));
    CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORYELLOW);
    CHAR_endCharOneArray(petindex);
  }
  return TRUE;
}
