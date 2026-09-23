#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "magic_base.h"
//
#include "gmsv_server.h"

static CharBase MagicBaseInt[] = {
	{{"ID"}, MAGIC_ID}
	, {{"字段"}, MAGIC_FIELD}
	, {{"目标"}, MAGIC_TARGET}
	, {{"死亡目标"}, MAGIC_TARGET_DEADFLG}
#ifdef _ATTACK_MAGIC
	, {{"攻击精灵"}, MAGIC_IDX}
#endif
};

static CharBase MagicBaseChar[] = {
	{{"名字"}, MAGIC_NAME},
	{{"注释"}, MAGIC_COMMENT},
	{{"函数名"}, MAGIC_FUNCNAME},
	{{"选项"}, MAGIC_OPTION}};

#if _MAGIC_LUA
#define MAGIC_LUA_RETURN                                                     \
  return MAGIC_addLUAListFunction(L, luaFuncName, luaFuncTable, gmLevel,     \
                                  luaFuncName)
#else
#define MAGIC_LUA_RETURN return 1
#endif

static int addLUAListFunction(lua_State *L) {
  size_t l;
  const char *luaFuncName = luaL_checklstring(L, 1, &l);
  const char *luaFuncTable = luaL_checklstring(L, 2, &l);
  const char *luaFuncTablePath = luaL_checklstring(L, 3, &l);
  int gmLevel = luaL_checkint(L, 4);
  const char *useString = luaL_checklstring(L, 5, &l);

  if (strlen(luaFuncTablePath) > 0) {
    SaLua *mylua = &gSaLua;
    while (mylua->next != NULL) {
      if (strcmp(mylua->luapath, luaFuncTablePath) == 0)
        MAGIC_LUA_RETURN;
      mylua = mylua->next;
    }
    return 1;
  }
  MAGIC_LUA_RETURN;
}

static int getChar(lua_State *L)
{
	const int magic_id = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, MagicBaseChar, arraysizeof(MagicBaseChar));
	int array = MAGIC_getMagicArray(magic_id);
	if (array != -1)
		lua_pushstring(L, MAGIC_getChar(array, element));
	else
		lua_pushstring(L, "");
	return 1;
}

static int getInt(lua_State *L)
{
	const int magic_id = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, MagicBaseInt, arraysizeof(MagicBaseInt));
	int array = MAGIC_getMagicArray(magic_id);
	if (array != -1)
		lua_pushinteger(L, MAGIC_getInt(array, element));
	else
		lua_pushinteger(L, -1);
	return 1;
}

// magic全局变量包含以下3个函数
static const luaL_Reg magiclib[] = {
    {"addLUAListFunction", addLUAListFunction},
    {"getInt", getInt},
    {"getChar", getChar},
    {NULL, NULL}};

// 2026.09.22 注册magic全局变量
LUALIB_API int luaopen_Magic(lua_State *L)
{
	luaL_register(L, "magic", magiclib);
	return 1;
}
