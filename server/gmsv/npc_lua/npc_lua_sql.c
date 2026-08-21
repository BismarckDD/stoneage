#include "npc_lua.h"
#include "util.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "anim_tbl.h"
#include "object.h"
#include "battle.h"
#include "npcutil.h"
#include "item.h"
#include "readmap.h"

#ifdef _JZ_NEWSCRIPT_LUA


int NPC_Lua_SQL_Push(lua_State *_NLL)
{
	CheckEx(_NLL, 1);

	char*  _SQLSTR = NULL;

	_SQLSTR = lua_tostring(_NLL, 1);
	sasql_LUASQL(_SQLSTR,-1,"NULL","NULL",-1,-1,"NULL");
}


int NPC_Lua_SQL_PushPop(lua_State *_NLL)
{
	CheckEx(_NLL,7);
	int npcindex,char_index,row;
	char* sqlstr,*function,*filepath,*msg;

	sqlstr = lua_tostring(_NLL, 1);
	row = lua_tointeger(_NLL, 2);
	filepath = lua_tostring(_NLL, 3);
	function = lua_tostring(_NLL, 4);
	npcindex = lua_tointeger(_NLL, 5);
	char_index = lua_tointeger(_NLL, 6);
	msg = lua_tostring(_NLL, 7);
//	print("sqlstr=%s\n row=%d\n filepath=%s\n function=%s\n npcindex=%d\n char_index=%d\n msg=%s\n",sqlstr,row,filepath,function,npcindex,char_index,msg);
	sasql_LUASQL(sqlstr,row,filepath,function,npcindex,char_index,msg);
}

int NPC_Lua_SQL_PushPopAdv(lua_State *_NLL)
{
	CheckEx(_NLL,6);
	int npcindex,char_index;
	char* sqlstr,*function,*filepath,*msg;

	sqlstr = lua_tostring(_NLL, 1);
	if(lua_isnil((_NLL), (3))){
		filepath = "";
	}else{
		filepath = lua_tostring(_NLL, 3);
	}
	function = lua_tostring(_NLL, 3);
	npcindex = lua_tointeger(_NLL, 4);
	char_index = lua_tointeger(_NLL, 5);
	msg = lua_tostring(_NLL, 6);
	int flg = 1;
	sasql_LUASQLAdv(sqlstr,filepath,function,npcindex,char_index,msg,flg);
}

int NPC_Lua_SQL_QueryFirstRow(lua_State *_NLL)
{
	CheckEx(_NLL,6);
	int npcindex,char_index;
	char* sqlstr,*function,*filepath,*msg;

	sqlstr = lua_tostring(_NLL, 1);
	if(lua_isnil((_NLL), (3))){
		filepath = "";
	}else{
		filepath = lua_tostring(_NLL, 3);
	}
	function = lua_tostring(_NLL, 3);
	npcindex = lua_tointeger(_NLL, 4);
	char_index = lua_tointeger(_NLL, 5);
	msg = lua_tostring(_NLL, 6);
	int flg = 2;
	sasql_LUASQLAdv(sqlstr,filepath,function,npcindex,char_index,msg,flg);
}

int NPC_Lua_SQL_FetchRow(lua_State *_NLL)
{
	CheckEx(_NLL,5);
	int npcindex,char_index;
	char* sqlstr,*function,*filepath,*msg;


	if(lua_isnil((_NLL), (3))){
		filepath = "";
	}else{
		filepath = lua_tostring(_NLL, 3);
	}
	function = lua_tostring(_NLL, 2);
	npcindex = lua_tointeger(_NLL, 3);
	char_index = lua_tointeger(_NLL, 4);
	msg = lua_tostring(_NLL, 5);
	int flg = 3;
	sasql_LUASQLAdv("",filepath,function,npcindex,char_index,msg,flg);
}

int NPC_Lua_SQL_FreeResult(lua_State *_NLL)
{
	CheckEx(_NLL,5);
	int npcindex,char_index;
	char* sqlstr,*function,*filepath,*msg;


	if(lua_isnil((_NLL), (3))){
		filepath = "";
	}else{
		filepath = lua_tostring(_NLL, 3);
	}
	function = lua_tostring(_NLL, 2);
	npcindex = lua_tointeger(_NLL, 3);
	char_index = lua_tointeger(_NLL, 4);
	msg = lua_tostring(_NLL, 5);
	int flg = 4;
	sasql_LUASQLAdv("",filepath,function,npcindex,char_index,msg,flg);
}
#endif
