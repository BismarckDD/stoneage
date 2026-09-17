#ifndef __MYLUA_MYLUA__H__
#define __MYLUA_MYLUA__H__

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

int loadMyLua(const char *filename);
int reLoadMyLua(const char *filename);
int closeMyLua();
int dofile(lua_State *L, const char *name);

#define docall(L, narg, clear) _docall(L, narg, clear, __func__)
int _docall(lua_State *L, int narg, int clear, const char *name);
int getArrayInt(lua_State *L, int idx);
LUALIB_API void luaAB_openlibs(lua_State *L);

typedef struct tagMyLua {
  lua_State *lua;
  char *luapath;
  struct tagMyLua *next;
} MY_Lua;

#ifdef __MYLUA_MYLUA_C__
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MY_Lua gMyLua;

#undef EXTERN

#endif
