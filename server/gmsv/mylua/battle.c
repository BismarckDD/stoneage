#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "battle.h"
#include "battle_command.h"
#include "char_base.h"
#include "enemy.h"
//

#ifdef _ALLBLUES_LUA

static int CreateVsEnemy(lua_State *L) {
  const int char_index = luaL_checkint(L, 1);
  const int npcindex = luaL_checkint(L, 2);
  int enemytable[11];
  int i;
  for (i = 0; i < 11; i++) {
    enemytable[i] = -1;
  }
  luaL_checktype(L, 3, LUA_TTABLE);
  int n = luaL_getn(L, 3);
  int id = 0;
  for (i = 0; i < n; i++) {
    int enemyid = getArrayInt(L, i);
    int curEnemy = ENEMY_getEnemyArrayFromId(enemyid);

    if (ENEMY_CHECKINDEX(curEnemy)) {
      enemytable[id++] = curEnemy;
    } else {
      enemytable[i] = -1;
    }
  }
  int ret = BATTLE_CreateVsEnemyNew(char_index, npcindex, enemytable);
  if (ret == 0) {
    if (npcindex > -1) {
      CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_INIT);
      CHAR_sendBattleEffect(npcindex, ON);
    }
    lua_pushinteger(L, CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX));
    return 1;
  } else {
    return 0;
  }
}

static int CreateVsEnemyLv(lua_State *L) {
  size_t l;
  const int char_index = luaL_checkint(L, 1);
  const int npcindex = luaL_checkint(L, 2);
  char *enemytablebuf = luaL_checklstring(L, 3, &l);
  char *enemylvtablebuf = luaL_checklstring(L, 4, &l);
  char tmpenemytablebuf[10];
  char tmpenemylvtablebuf[10];
  int j = 1;
  int k = 1;
  int enemytable[11];
  int enemylvtable[11];
  int i;
  for (i = 0; i < 11; i++) {
    enemytable[i] = -1;
    enemylvtable[i] = -1;
  }
  while (getStringFromIndexWithDelim(enemytablebuf, ",", j, tmpenemytablebuf,
                                     sizeof(tmpenemytablebuf)) != FALSE) {
    int enemyid = atoi(tmpenemytablebuf);
    int curEnemy = ENEMY_getEnemyArrayFromId(enemyid);
    if (ENEMY_CHECKINDEX(curEnemy)) {
      enemytable[j - 1] = curEnemy;
    } else {
      enemytable[j - 1] = -1;
    }
    j++;
  }
  while (getStringFromIndexWithDelim(enemylvtablebuf, ",", k,
                                     tmpenemylvtablebuf,
                                     sizeof(tmpenemylvtablebuf)) != FALSE) {
    enemylvtable[k - 1] = atoi(tmpenemylvtablebuf);
    k++;
  }
  if (j != k) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int ret =
      BATTLE_CreateVsEnemyLvNew(char_index, npcindex, enemytable, enemylvtable);
  if (ret == 0) {
    if (npcindex > -1) {
      CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_INIT);
      CHAR_sendBattleEffect(npcindex, ON);
    }
    lua_pushinteger(L, CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX));
    return 1;
  } else {
    return 0;
  }
}

static int getCharOne(lua_State *L) {
  const int battle = luaL_checkint(L, 1);
  const int num = luaL_checkint(L, 2);
  const int side = luaL_checkint(L, 3);

  lua_pushinteger(L, BattleArray[battle].Side[side].Entry[num].char_index);

  return 1;
}

static int Exit(lua_State *L) {
  const int char_index = luaL_checkint(L, 1);
  const int battleindex = luaL_checkint(L, 2);

  BATTLE_Exit(char_index, battleindex);

  return 1;
}

#ifdef _ALLBLUES_LUA_1_4
static CharBase BattleBaseEvent[] = {{{"结束事件"}, BATTLE_FINISH},
                                     {{"逃跑事件"}, BATTLE_ESCAPE}};

static int CreateRandVsPlayer(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);
  int char_index1 = -1, char_index2 = -1;
  int num = 0;
  int i = 0;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) == FALSE)
      continue;
    if (!CHAR_CHECKINDEX(i))
      continue;
    if (CHAR_getInt(i, CHAR_FLOOR) == floorid) {
      if (CHAR_getWorkInt(i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
        if (char_index1 == -1) {
          char_index1 = i;
        } else {
          char_index2 = i;
        }
        if (CHAR_CHECKINDEX(char_index1) && CHAR_CHECKINDEX(char_index2)) {
          BATTLE_CreateVsPlayer(char_index1, char_index2);
          num++;
        }
      }
    }
  }

  lua_pushinteger(L, num);

  return 1;
}

static int CreateVsPlayer(lua_State *L) {
  const int char_index1 = luaL_checkint(L, 1);
  const int char_index2 = luaL_checkint(L, 2);

  if (CHAR_CHECKINDEX(char_index1) && CHAR_CHECKINDEX(char_index2)) {
    if (CHAR_getWorkInt(char_index1, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_NONE &&
        CHAR_getWorkInt(char_index2, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_NONE) {
      if (BATTLE_CreateVsPlayer(char_index1, char_index2) == 0) {
        lua_pushinteger(L, CHAR_getWorkInt(char_index1, CHAR_WORKBATTLEINDEX));
        return 1;
      }
    }
  }
  lua_pushinteger(L, -1);
  return 1;
}

static int setLUAFunctionPointer(lua_State *L) {
  size_t l;
  const int battleindex = luaL_checkint(L, 1);
  const int functype =
      getCharBaseValue(L, 2, BattleBaseEvent, arraysizeof(BattleBaseEvent));
  char *luafunctable = luaL_checklstring(L, 3, &l);

  BATTLE_setLUAFunction(battleindex, functype, L, luafunctable);

  return 1;
}

static int WatchEntry(lua_State *L) {
  const int from_index = luaL_checkint(L, 1);
  const int to_index = luaL_checkint(L, 2);
  BATTLE_WatchEntry(from_index, to_index);
  return 1;
}

static int NewEntry(lua_State *L) {
  const int char_index = luaL_checkint(L, 1);
  const int battleindex = luaL_checkint(L, 2);
  const int side = luaL_checkint(L, 3);
  int fd = getfdFromCharaIndex(char_index);
  int iRet = BATTLE_NewEntry(char_index, battleindex, side);
  if (iRet == 0) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    iRet = BATTLE_PetDefaultEntry(char_index, battleindex, side);
  }
  if (iRet == 0) {
    int flg;
    if (CHAR_getInt(char_index, CHAR_DEFAULTPET) >= 0) {
      int pindex =
          CHAR_getCharPet(char_index, CHAR_getInt(char_index, CHAR_DEFAULTPET));
      CHAR_setWorkInt(pindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    }
    if (fd != -1) {
      GmsvServer_EN_send(fd, BattleArray[battleindex].type,
                         BattleArray[battleindex].field_no);
    }
    flg = (BattleArray[battleindex].Side[side].flg & BSIDE_FLG_HELP_OK) ? TRUE
                                                                        : FALSE;
    GmsvServer_HL_send(fd, flg);
    char szBuffer[256];
    sprintf(szBuffer, "BP|%X|%X|%X", BATTLE_Index2No(battleindex, char_index),
            BP_FLG_JOIN, CHAR_getInt(char_index, CHAR_MP));
    BATTLE_CommandSend(char_index, szBuffer);
    lua_pushinteger(L, 1);
  } else {
    if (fd != -1) {
      GmsvServer_EN_send(fd, FALSE, BattleArray[battleindex].field_no);
    }
    lua_pushinteger(L, 0);
  }

  return 1;
}
#endif

#ifdef _ALLBLUES_LUA_1_9
static int CreateForWatcher(lua_State *L) {
  const int char_index = luaL_checkint(L, 1);
  const int battle_index = luaL_checkint(L, 2);
  BATTLE_CreateForWatcher(char_index, battle_index);
  return 1;
}

static int CheckIndex(lua_State *L) {
  const int battle_index = luaL_checkint(L, 1);
  lua_pushinteger(L, BATTLE_CHECKINDEX(battle_index));
  return 1;
}

static int GetType(lua_State *L) {
  const int battle_index = luaL_checkint(L, 1);
  lua_pushinteger(L, BATTLE_getType(battle_index));
  return 1;
}

#ifdef _BATTLE_TIMESPEED
static int getCreateTime(lua_State *L) {
  const int batleindex = luaL_checkint(L, 1);
  lua_pushinteger(L, BATTLE_getCreateTime(batleindex));
  return 1;
}
#endif

static int getBattleFloor(lua_State *L) {
  const int battleindex = luaL_checkint(L, 1);

  lua_pushinteger(L, BATTLE_getBattleFloor(battleindex));

  return 1;
}
#endif
static const luaL_Reg battlelib[] = {
    {"CreateVsEnemy", CreateVsEnemy},
    {"CreateVsEnemyLv", CreateVsEnemyLv},
    {"getCharOne", getCharOne},
#ifdef _ALLBLUES_LUA_1_4
    {"CreateRandVsPlayer", CreateRandVsPlayer},
    {"CreateVsPlayer", CreateVsPlayer},
    {"setLUAFunctionPointer", setLUAFunctionPointer},
    {"WatchEntry", WatchEntry},
    {"NewEntry", NewEntry},
#endif
#ifdef _ALLBLUES_LUA_1_9
    {"CreateForWatcher", CreateForWatcher},
    {"CheckIndex", CheckIndex},
    {"GetType", GetType},
#ifdef _BATTLE_TIMESPEED
    {"getCreateTime", getCreateTime},
#endif
    {"getBattleFloor", getBattleFloor},
#endif
    {"Exit", Exit},
    {NULL, NULL}};

LUALIB_API int luaopen_Battle(lua_State *L) {
  luaL_register(L, "battle", battlelib);
  return 1;
}

#endif
