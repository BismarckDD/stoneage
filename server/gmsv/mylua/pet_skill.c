#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "pet_skill.h"
#include "char.h"
#include "item.h"
//
#include "gmsv_server.h"

static CharBase PetSkillBaseInt[] = {
	{{"序号"}, PETSKILL_ID}
	, {{"字段"}, PETSKILL_FIELD}
	, {{"目标"}, PETSKILL_TARGET}
#ifdef _PETSKILL2_TXT
	, {{"类型"}, PETSKILL_USETYPE}
#endif
	, {{"价值"}, PETSKILL_COST}
	, {{"非法"}, PETSKILL_ILLEGAL}};

static CharBase PetSkillBaseChar[] = {
	{{"名称"}, PETSKILL_NAME}
	, {{"注释"}, PETSKILL_COMMENT}
	, {{"函数"}, PETSKILL_FUNCNAME}
	, {{"选项"}, PETSKILL_OPTION}
#ifdef _CFREE_petskill
	, {{"自由"}, PETSKILL_FREE}
	, {{"种类"}, PETSKILL_KINDCODE}
#endif
};

static int getInt(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseInt, arraysizeof(PetSkillBaseInt));
	lua_pushinteger(L, PETSKILL_getInt(index, element));
	return 1;
}

static int setInt(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseInt, arraysizeof(PetSkillBaseInt));
	const int data = luaL_checkint(L, 3);
	return PETSKILL_setInt(index, element, data) == -1 ? 0 : 1;
}

static int getChar(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseChar, arraysizeof(PetSkillBaseChar));
	lua_pushstring(L, PETSKILL_getChar(index, element));
	return 1;
}

static int setChar(lua_State *L)
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseChar, arraysizeof(PetSkillBaseChar));
	char *data = luaL_checklstring(L, 3, &l);
	return PETSKILL_setChar(index, element, data) == -1 ? 0 : 1;
}

static int check(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	lua_pushinteger(L, PETSKILL_CHECKINDEX(index));
	return 1;
}

static int getPetskillArray(lua_State *L)
{
	const int index = luaL_checkint(L, 1);

	lua_pushinteger(L, PETSKILL_getPetskillArray(index));
	return 1;
}

static const luaL_Reg petSkillLib[] = {
	{"getInt", getInt},
	{"setInt", setInt},
	{"getChar", getChar},
	{"setChar", setChar},
	{"check", check},
	{"getPetskillArray", getPetskillArray},
	{NULL, NULL}};

//
LUALIB_API int luaopen_PetSkill(lua_State *L)
{
	luaL_register(L, "petskill", petSkillLib);
	return 1;
}
