#define LUA_LIB

#include "mylua/base.h"

static const luaL_Reg lualibs[] = {{"char", luaopen_Char},
                                   {"npc", luaopen_NPC},
                                   {"battle", luaopen_Battle},
                                   {"obj", luaopen_Object},    // 调用
                                   {"client", luaopen_Client}, // 访问client
                                   {"saac", luaopen_Saac},     // 访问saac
                                   {"map", luaopen_Map},
                                   {"other", luaopen_Other},
                                   {"config", luaopen_Config},
                                   {"item", luaopen_Item},
                                   {"magic", luaopen_Magic},
#ifdef _OFFLINE_SYSTEM
                                   {"offline", luaopen_Offline},
#endif
                                   {"enemytemp", luaopen_EnemyTemp},
#ifdef _SASQL
                                   {"sasql", luaopen_Sasql},
#endif
                                   {"net", luaopen_Net},
#ifdef _PROFESSION_SKILL
                                   {"Professionskill", luaopen_ProfessionSkill},
#endif
                                   {"family", luaopen_Family},
                                   {"petskill", luaopen_PetSkill},
                                   {NULL, NULL}};

// 2026.09.22 把C++实现的lua适配器函数注册到lua
LUALIB_API void luaAB_openlibs(lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}
