#include "mylua/base.h"
//
#include "char.h"
#include "char_base.h"
#include "config_file.h"

static int set(lua_State *L)
{
	size_t l;
	char *data = luaL_checklstring(L, 1, &l);
	luareadconfigfile(data);
	return 1;
}

static int checkIp(lua_State *L)
{
	int ip = luaL_checkint(L, 1);
#ifdef _CHECK_SEVER_IP
	lua_pushinteger(L, checkServerIp(ip));
#else
	lua_pushinteger(L, 0);
#endif
	return 1;
}

static int getGameID(lua_State *L)
{
	lua_pushstring(L, getGameserverID());
	return 1;
}

static int getGamename(lua_State *L)
{
	lua_pushstring(L, getGameservername());
	return 1;
}

static int getnumber(lua_State *L)
{
	lua_pushinteger(L, getServernumber());
	return 1;
}
#ifdef _GET_BATTLE_EXP
static int getexp(lua_State *L)
{
	lua_pushinteger(L, getBattleexp());
	return 1;
}
#endif

#ifdef _VIP_BATTLE_EXP
int getVipexp(lua_State *L)
{
	lua_pushinteger(L, getVipBattleexp());
	return 1;
}
#endif

int getFdMaxNnum(lua_State *L)
{
	lua_pushinteger(L, getFdnum());
	return 1;
}

int PlayerNum(lua_State *L)
{
	int num = luaL_checkint(L, 1);
#ifdef _PLAYER_NUM
	setPlayerNum(num);
#endif
	return 1;
}
static const luaL_Reg configlib[] = {
	{"set", set},
	{"checkIp", checkIp},
	{"getGameserverID", getGameID},
	{"getGameservername", getGamename},
	{"getServernumber", getnumber},
#ifdef _GET_BATTLE_EXP
	{"getBattleexp", getexp},
#endif
#ifdef _VIP_BATTLE_EXP
	{"getVipBattleexp", getVipexp},
#endif
	{"getFdnum", getFdMaxNnum},
	{"PlayerNum", PlayerNum},
	{NULL, NULL}};

LUALIB_API int luaopen_Config(lua_State *L)
{
	luaL_register(L, "config", configlib);
	return 1;
}
