#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "magic_base.h"
//
#include "gmsv_server.h"

#ifdef _ALLBLUES_LUA_1_2

static CharBase MagicBaseInt[] = {
	{{"ID"}, MAGIC_ID},
	{{"字段"}, MAGIC_FIELD},
	{{"目标"}, MAGIC_TARGET},
	{{"死亡目标"}, MAGIC_TARGET_DEADFLG}
#ifdef _ATTACK_MAGIC
	,
	{{"攻击精灵"}, MAGIC_IDX}
#endif
};

static CharBase MagicBaseChar[] = {
	{{"名字"}, MAGIC_NAME},
	{{"注释"}, MAGIC_COMMENT},
	{{"函数名"}, MAGIC_FUNCNAME},
	{{"选项"}, MAGIC_OPTION}};

static int addLUAListFunction(lua_State *L)
{
	size_t l;
	char *luaFuncName = luaL_checklstring(L, 1, &l);
	char *luaFuncTable = luaL_checklstring(L, 2, &l);
	char *luaFuncTablePath = luaL_checklstring(L, 3, &l);
	const int gmLevel = luaL_checkint(L, 4);
	char *usestring = luaL_checklstring(L, 5, &l);

	if (strlen(luaFuncTablePath) > 0)
	{
		MyLua *mylua = &gMyLua;
		while (mylua->next != NULL)
		{
			if (strcmp(mylua->luapath, luaFuncTablePath) == 0)
			{
#ifdef _MAGIC_LUA
				return MAGIC_addLUAListFunction(mylua->lua, luafuncname, luafunctable, gmlevel, usestring);
#else
				return 1;
#endif
			}
			mylua = mylua->next;
		}
	}
	else
	{
#ifdef _MAGIC_LUA
		return MAGIC_addLUAListFunction(L, luafuncname, luafunctable, gmlevel, usestring);
#else
		return 1;
#endif
	}
	return 1;
}

static int getChar(lua_State *L)
{
	const int magicid = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, MagicBaseChar, arraysizeof(MagicBaseChar));
	int marray = MAGIC_getMagicArray(magicid);
	if (marray != -1)
	{
		lua_pushstring(L, MAGIC_getChar(marray, element));
	}
	else
	{
		lua_pushstring(L, "");
	}
	return 1;
}

static int getInt(lua_State *L)
{
	const int magicid = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, MagicBaseInt, arraysizeof(MagicBaseInt));
	int marray = MAGIC_getMagicArray(magicid);
	if (marray != -1)
	{
		lua_pushinteger(L, MAGIC_getInt(marray, element));
	}
	else
	{
		lua_pushinteger(L, -1);
	}
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

#endif
