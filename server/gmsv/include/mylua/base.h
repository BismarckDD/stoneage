#ifndef __MYLUA_BASE_H__
#define __MYLUA_BASE_H__

#include "version.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

LUALIB_API int luaopen_Char(lua_State *L);
LUALIB_API int luaopen_NPC(lua_State *L);
LUALIB_API int luaopen_Client(lua_State *L);  // LUA直接和Client通信
LUALIB_API int luaopen_Saac(lua_State *L); // LUA直接和SAAC通信
LUALIB_API int luaopen_Battle(lua_State *L);
LUALIB_API int luaopen_Object(lua_State *L);

LUALIB_API int luaopen_Map(lua_State *L);
LUALIB_API int luaopen_Other(lua_State *L);
LUALIB_API int luaopen_Config(lua_State *L);

LUALIB_API int luaopen_Item(lua_State *L);
LUALIB_API int luaopen_Magic(lua_State *L);
LUALIB_API int luaopen_Offline(lua_State *L);

LUALIB_API int luaopen_EnemyTemp(lua_State *L);
LUALIB_API int luaopen_Sasql(lua_State *L);

LUALIB_API int luaopen_Net(lua_State *L);

LUALIB_API int luaopen_Family(lua_State *L);
LUALIB_API int luaopen_PetSkill(lua_State *L);
#ifdef _PROFESSION_SKILL
LUALIB_API int luaopen_ProfessionSkill(lua_State *L);
#endif

#endif
