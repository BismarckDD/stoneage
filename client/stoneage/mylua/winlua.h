#pragma once

extern "C" {
  #include "lua/lauxlib.h"
  #include "lua/lua.h"
  #include "lua/lualib.h"
}

#ifdef _WIN_LUAJIT_

int myluaload(char *filename);
int remyluaload(char *filename);
void LoadStoneAgeLUA(const char *path);
int CloseSaLua();
int dofile(lua_State *L, const char *name);
int docall(lua_State *L, int narg, int clear);
int getArrayInt(lua_State *L, int idx);
LUALIB_API void luaAB_openlibs(lua_State *L);
LUALIB_API int luaopen_Win(lua_State *L);
LUALIB_API int luaopen_Lssproto(lua_State *L);
LUALIB_API int luaopen_Sa(lua_State *L);
LUALIB_API int luaopen_ServerList(lua_State *L);
LUALIB_API int luaopen_PetData(lua_State *L);
lua_State *FindLua(char *filename);
lua_State *FindWinIntLua(int wintype, char *data);
ACTION *FreeCreateWinTypeLua(lua_State *lua);
ACTION *FreeServerWinTypeLua(lua_State *lua);
void ReLoadStoneAgeLUA(char *filename);
void LC_LUA_ITEM_DIRECTIONS(int x, int y, char *name, int iamge);

struct SaLua {
  lua_State *lua;
  char *luapath;
  SaLua *next;
};
int FreeGetBattleMap(int floor);
int CheckPlayOnLine(int index, char *account, char *mac);

#endif // 
