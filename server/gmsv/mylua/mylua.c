#define __MYLUA_MYLUA_C__
#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "autil.h"
#include "buf.h"
#include "util.h"
//
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

int getArrayInt(lua_State *L, int idx) {
  int result = 0;
  lua_pushnumber(L, idx + 1);
  lua_gettable(L, -2);
  result = (int)lua_tonumber(L, -1);
  lua_pop(L, 1);
  return result;
}

static int report(lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL)
      msg = "(error object is not a string)";
    fprintf(stderr, "%s: ", "lua");
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
    lua_pop(L, 1);
  }
  return status;
}

static int traceback(lua_State *L) {
  if (!lua_isstring(L, 1)) /* 'message' not a string? */
    return 1;              /* keep it intact */
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);   /* pass error message */
  lua_pushinteger(L, 2); /* skip this function and traceback */
  lua_call(L, 2, 1);     /* call debug.traceback */
  return 1;
}

int _docall(lua_State *L, int narg, int clear, const char *file) {
  int base = lua_gettop(L) - narg; /* function index */
  lua_pushcfunction(L, traceback); /* push traceback function */
  lua_insert(L, base);             /* put it under chunk and args */
  int status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  lua_remove(L, base); /* remove traceback function */
  /* force a complete garbage collection in case of errors */
  if (status != 0) {
    printf("函数错误=%s  错误提示：%s\n", file, (char *)lua_tostring(L, -1));
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
  return status;
}

int dofile(lua_State *L, const char *name) {
  int status = luaL_loadfile(L, name) || docall(L, 0, 1);
  return report(L, status);
}

int loadSaLua(const char *filename) {
  SaLua *mylua = &gSaLua;
  print("[Lua] loading: %s\n", filename);
  while (mylua->next != NULL) {
    if (strcmp(mylua->luapath, filename) == 0) {
      print("[Lua] skipped (already loaded): %s\n", filename);
      return FALSE;
    }
    mylua = mylua->next;
  }
  int filename_len = strlen(filename);
  mylua->luapath = allocateMemory(filename_len + 1);
  memset(mylua->luapath, 0, filename_len + 1);
  strcpy(mylua->luapath, filename);
  mylua->next = allocateMemory(sizeof(SaLua));
  memset(mylua->next, 0, sizeof(SaLua));
  if (mylua->next == NULL)
    return EXIT_FAILURE;
  mylua->lua = lua_open(); /* 2026.09.22 这个函数很关键 如何open? */

  if (mylua->lua == NULL) {
    print("[Lua] failed to create state: %s\n", filename);
    return FALSE;
  }
  
  /* stop collector during initialization open libraries */
  lua_gc(mylua->lua, LUA_GCSTOP, 0);
  luaL_openlibs(mylua->lua); // 2026.09.22 这两个函数的形式完全相同
  luaAB_openlibs(mylua->lua);
  // 区别仅在于lualibs的值, 这样lua就有标准库和自定义库
  lua_gc(mylua->lua, LUA_GCRESTART, 0);

  //
  int status = dofile(mylua->lua, filename);
  if (status != 0) {
    print("[Lua] load failed: %s (status=%d)\n", filename, status);
    return FALSE;
  }

  // 查找当前lua模块中是否有init函数，如果有，执行init()
  lua_getglobal(mylua->lua, "init");
  if (lua_isfunction(mylua->lua, -1)) {
    status = docall(mylua->lua, 0, 1);
    if (status != 0) {
      print("[Lua] init() failed: %s (status=%d)\n", filename, status);
      return FALSE;
    }
  }

  // 查找当前lua模块中是否有main函数，如果有，执行main()
  lua_getglobal(mylua->lua, "main");
  if (lua_isfunction(mylua->lua, -1)) {
    status = docall(mylua->lua, 0, 1);
    if (status != 0) {
      print("[Lua] main() failed: %s (status=%d)\n", filename, status);
      return FALSE;
    }
  }
  // 执行完main就算加载完毕.
  print("[Lua] loaded: %s\n", filename);
  return TRUE;
}

int reLoadSaLua(const char *filename) {
  SaLua *mylua = &gSaLua;

  while (mylua->next != NULL) {
    if (strlen(mylua->luapath) > 0) {
      if (strlen(filename) > 0) {
        if (strstr(mylua->luapath, filename) == 0) {
          mylua = mylua->next;
          continue;
        }
      }
      lua_gc(mylua->lua, LUA_GCSTOP, 0);
      luaL_openlibs(mylua->lua);
      luaAB_openlibs(mylua->lua);
      lua_gc(mylua->lua, LUA_GCRESTART, 0);
      dofile(mylua->lua, mylua->luapath);

      // 这就是为什么多个lua脚本有data()函数, 这里会调用
      lua_getglobal(mylua->lua, "data");
      if (lua_isfunction(mylua->lua, -1)) {
        docall(mylua->lua, 0, 1);
      }
    }
    mylua = mylua->next;
  }
  return EXIT_SUCCESS;
}

int closeSaLua() {
  SaLua *mylua = &gSaLua;
  while (mylua->next != NULL) {
    lua_pop(mylua->lua, 1);
    lua_close(mylua->lua);
    mylua = mylua->next;
  }
  return EXIT_SUCCESS;
}

static int ablua_is_regular_file(const char *fullpath,
                                 const struct dirent *ent) {
#if defined(_WIN32) || defined(_WIN64)
  struct stat st;
  (void)ent;
  return (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode));
#else
  (void)fullpath;
  return (ent->d_type == 8); /* DT_REG */
#endif
}

static void LoadAllbluesLUARecursive(const char *path, int *loaded, int *failed) {
  struct dirent *ent = NULL;
  char filename[256];
  DIR *pDir = opendir(path);
  if (pDir == NULL) {
    print("[Lua] cannot open script directory: %s\n", path);
    (*failed)++;
    return;
  }

  while (NULL != (ent = readdir(pDir))) {
    if (ent->d_name[0] == '.')
      continue;
    memset(filename, 0, 256);
    sprintf(filename, "%s/%s", path, ent->d_name);
    if (ablua_is_regular_file(filename, ent)) {
      if (strcmptail(ent->d_name, ".lua") == 0) {
        if (loadSaLua(filename))
          (*loaded)++;
        else
          (*failed)++;
      }
    } else {
      sprintf(filename, "%s/%s", path, ent->d_name);
      LoadAllbluesLUARecursive(filename, loaded, failed);
    }
  }
  closedir(pDir);
}

void LoadLua(const char *path) {
  int loaded = 0;
  int failed = 0;
  print("[Lua] scanning script directory: %s\n", path);
  LoadAllbluesLUARecursive(path, &loaded, &failed);
  print("[Lua] scan complete: loaded=%d, failed/skipped=%d, path=%s\n", loaded,
        failed, path);
}

void ReLoadLua(const char *filename) { reLoadSaLua(filename); }

const int getCharBaseValue(lua_State *L,
                           int narg,
                           CharBase *charbase,
                           int num) {
  if (!lua_isnumber(L, narg)) {
    size_t l;
    const char *data = luaL_checklstring(L, narg, &l);
    if (data == NULL || data[0] == '\0') {
      return -1;
    }
    char field[64];
    int line = 1;
    int i;
    int value = 0;
    while (getDelimitedField(data, "|", line, field, sizeof(field)) ==
           TRUE) {
      for (i = 0; i < num; i++) {
        if (strcmp(charbase[i].field, field) == 0) {
          value |= charbase[i].element;
          break;
        }
      }
      if (i == num) {
        print("\ncharbase缺失字段[%s]\n", field);
        return -1;
      }
      line++;
    }
    return value;
  } else {
    return luaL_checkint(L, narg);
  }
}
