#define LUA_LIB

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "mylua/base.h"

static const luaL_Reg lualibs[] = {{"char", luaopen_Char},
                                   {"npc", luaopen_NPC},
                                   {"lssproto", luaopen_Lssproto},
                                   {"battle", luaopen_Battle},
                                   {"obj", luaopen_Object},
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
                                   {"saacproto", luaopen_Saacproto},
#ifdef _ALLBLUES_LUA_1_8
#ifdef _PROFESSION_SKILL
                                   {"Professionskill", luaopen_ProfessionSkill},
#endif
                                   {"family", luaopen_Family},
                                   {"petskill", luaopen_PetSkill},
#endif
                                   {NULL, NULL}};

LUALIB_API void luaAB_openlibs(lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}
