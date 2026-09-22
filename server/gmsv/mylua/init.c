#define LUA_LIB

#include "mylua/base.h"

static const luaL_Reg lualibs[] = {{"char", luaopen_Char},
                                   {"npc", luaopen_NPC},
                                   {"battle", luaopen_Battle},
                                   {"obj", luaopen_Object},    // 调用
                                   {"client", luaopen_Client}, // 访问client
                                   {"saac", luaopen_Saac},     // 访问saac
#ifdef _ALLBLUES_LUA_1_1
                                   {"map", luaopen_Map},
                                   {"other", luaopen_Other},
                                   {"config", luaopen_Config},
#endif
#ifdef _ALLBLUES_LUA_1_2
                                   {"item", luaopen_Item},
                                   {"magic", luaopen_Magic},
#endif
#ifdef _OFFLINE_SYSTEM
                                   {"offline", luaopen_Offline},
#endif
#ifdef _ALLBLUES_LUA_1_4
                                   {"enemytemp", luaopen_Enemytemp},
#ifdef _SASQL
                                   {"sasql", luaopen_Sasql},
#endif
#endif
                                   {"net", luaopen_Net},
#ifdef _ALLBLUES_LUA_1_8
#ifdef _PROFESSION_SKILL
                                   {"Professionskill", luaopen_ProfessionSkill},
#endif
                                   {"family", luaopen_Family},
                                   {"petskill", luaopen_PetSkill},
#endif
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
