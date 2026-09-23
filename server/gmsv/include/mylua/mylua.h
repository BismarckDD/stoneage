#ifndef __MYLUA_MYLUA_H__
#define __MYLUA_MYLUA_H__

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

typedef struct tagCharBase {
  char field[128];
  int element;
} CharBase;

const int getCharBaseValue(lua_State *L, int narg, CharBase *charbase, int num);

void LoadLua(const char *filename);
void ReLoadLua(const char *filename);
int loadSaLua(const char *filename);
int reLoadSaLua(const char *filename);
int closeSaLua();
int dofile(lua_State *L, const char *name);

#define docall(L, narg, clear) _docall(L, narg, clear, __func__)
int _docall(lua_State *L, int narg, int clear, const char *name);
int getArrayInt(lua_State *L, int idx);

LUALIB_API void luaAB_openlibs(lua_State *L);

typedef struct tagMyLua {
  lua_State *lua;
  char *luapath;
  struct tagMyLua *next;
} SaLua;

#ifdef __MYLUA_MYLUA_C__
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN SaLua gSaLua;

#undef EXTERN

#endif // __MYLUA_MYLUA_C__
