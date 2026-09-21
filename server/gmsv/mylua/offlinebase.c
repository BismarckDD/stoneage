#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "enemy.h"
#include "mylua/mylua.h"
#include "object.h"

#ifdef _OFFLINE_SYSTEM

int Attack(lua_State *L) {
  const int battleindex = luaL_checkint(L, 1);
  const int char_index = luaL_checkint(L, 2);
  const int side = 1 - luaL_checkint(L, 3);

  BATTLE_ENTRY *pEntry = BattleArray[battleindex].Side[side].Entry;
  int i, attackNoList[10] = {9, 7, 5, 6, 8, 4, 2, 0, 1, 3};
  for (i = 0; i < 10; i++) {
    if (pEntry[attackNoList[i]].char_index != -1) {
      break;
    }
  }

  // 设置攻击命令(WORKBATTLECOM1)和攻击目标(WORKBATTLECOM2)
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2,
                  attackNoList[i] + side * BATTLE_ENTRY_MAX);
  int weapon_id = CHAR_getItemIndex(char_index, CHAR_ARM);
  if (ITEM_CHECKINDEX(weapon_id) == TRUE) {
    if (ITEM_getInt(weapon_id, ITEM_TYPE) == ITEM_BOOMERANG) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG);
#ifdef _PETSKILL_BECOMEFOX
      if (CHAR_getInt(char_index, CHAR_IMAGENUMBER) == 101749 ||
          CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) !=
              -1) { // 若是变成小狐狸不可以使用回力标
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }
#endif
#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1) { // 变成乌力了
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      }
#endif
    }
  }
  // WORKBATTLECOM3疑似是攻击命令下的攻击次数
  CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3, 1);
  // 设置为OK状态
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
  return 1;
}

int Recovery(lua_State *L) {
  const int battle_index = luaL_checkint(L, 1);
  const int char_index = luaL_checkint(L, 2);
  const int HP = luaL_checkint(L, 3);
  // HP由其他逻辑计算，这里纯粹是控制逻辑
  int target_no = BATTLE_Index2No(battle_index, char_index);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1,
                  BATTLE_COM_S_OFFLINE_RECOVERY);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, target_no);
  CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3, HP);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
  return 1;
}

static const luaL_Reg offlinelib[] = {
    {"Attack", Attack},
	{"Recovery", Recovery}, {NULL, NULL}};

LUALIB_API int luaopen_Offline(lua_State *L) {
  luaL_register(L, "offline", offlinelib);
  return 1;
}

#endif
