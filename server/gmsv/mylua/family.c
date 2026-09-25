#include "mylua/base.h"
#include "mylua/mylua.h"
//
#include "family.h"
#include "char_base.h"
//
#include "saac_client.h"

#ifdef _ALLBLUES_LUA

#ifdef _MO_LUA_FMPOINT
extern struct FM_POINTLIST fmpointlist; // 家族据点
#endif

static int ShowFamilyList(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	char subbuf[256];
	if( getDelimitedField( gFamilyList, "|", index, subbuf, sizeof(subbuf) ) == TRUE){
		lua_pushstring(L, subbuf);
	}else{
		lua_pushstring(L, "");
	}
	return 1;
}
#ifdef _MO_LUA_FMPOINT
static int ShowPointListArray(lua_State *L)
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if (index>=FAMILY_MAXHOME || index<0) lua_pushstring(L, fmpointlist.pointlistarray[0]);
	lua_pushstring(L, fmpointlist.pointlistarray[index]);
	return 1;
}
#endif

static int GetFamilyBadge(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	if (CHAR_CHECKINDEX(index)){
		#ifdef _FAMILY_BADGE
		lua_pushinteger(L,getFamilyBadge(index));
		#else
		lua_pushinteger(L,0);
		#endif
	}else lua_pushinteger(L,0);
	return 1;
}
static const luaL_Reg Familylib[] = {
	{"ShowFamilyList", 	ShowFamilyList},
	{"GetFamilyBadge", 	GetFamilyBadge},
#ifdef _MO_LUA_FMPOINT
	{"ShowPointListArray", 	ShowPointListArray},
#endif
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Family (lua_State *L) {
  luaL_register(L, "family", Familylib);
  return 1;
}

#endif

