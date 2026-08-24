#include "systeminc/system.h"
#include "systeminc/netmain.h"
#include "winlua.h"

#ifdef _WIN_LUAJIT_

static int SetGroupMaxNum(lua_State *L) {
  // lua脚本中，SetGroupMaxNum应该只有一个int类型参数
  const int num = luaL_checkint(L, 1);
  nGroup = num;
  return 1;
}

static int SetGroupList(lua_State *L) {
  size_t l;
  // lua脚本中，SetGroupList应该有一个List类型参数
  // List的每一个元素应该是四元组：int, int, int, string
  const int id = luaL_checkint(L, 1);
  const int GroupUse = luaL_checkint(L, 2);
  const int ServerMaxNum = luaL_checkint(L, 3);
  const int ServerIndex = luaL_checkint(L, 4);
  const char *GroupName = luaL_checklstring(L, 5, &l);
  if (id < 0 || id >= MAX_GMGROUP)
    return luaL_error(L, "GROUP ID OUT OF RANGE: %d", id);
  gmgroup[id].used = GroupUse;
  gmgroup[id].num = ServerMaxNum;
  gmgroup[id].startindex = ServerIndex;
  strcpy_s(gmgroup[id].name, GroupName);
  return 1;
}

static int SetServerList(lua_State *L) {
  size_t l;
  const int id = luaL_checkint(L, 1);
  const char *ServerIP = luaL_checklstring(L, 2, &l);
  const char *ServerPort = luaL_checklstring(L, 3, &l);
  const char *ServerName = luaL_checklstring(L, 4, &l);
  if (id < 0 || id >= MAX_GMSV)
    return luaL_error(L, "SERVER ID OUT OF RANGE: %d", id);
  strcpy_s(gmsv[id].ipaddr, ServerIP);
  strcpy_s(gmsv[id].port, ServerPort);
  strcpy_s(gmsv[id].name, ServerName);
  gmsv[id].used = '1';
  return 1;
}

static const luaL_Reg ServerListlib[] = {{"SetGroupList", SetGroupList},
                                         {"SetGroupMaxNum", SetGroupMaxNum},
                                         {"SetServerList", SetServerList},
                                         {NULL, NULL}};

LUALIB_API int luaopen_ServerList(lua_State *L) {
  luaL_register(L, "ServerList", ServerListlib);
  return 1;
}

#endif
