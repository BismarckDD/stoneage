#include "systeminc/system.h"
#include "systeminc/netmain.h"
#include "systeminc/text_encoding.h"
#include "winlua.h"

#ifdef _WIN_LUAJIT_

static int SetGroupMaxNum(lua_State *L) {
  const int num = luaL_checkint(L, 1);
  nGroup = num;
  return 1;
}

static int SetGroupList(lua_State *L) {
  size_t l;
  const int id = luaL_checkint(L, 1);
  const int GroupUse = luaL_checkint(L, 2);
  const int ServerMaxNum = luaL_checkint(L, 3);
  const int ServerIndex = luaL_checkint(L, 4);
  const char *GroupName = luaL_checklstring(L, 5, &l);
  if (id < 0 || id >= MAX_GMGROUP)
    return luaL_error(L, "group id out of range: %d", id);
  gmgroup[id].used = GroupUse;
  gmgroup[id].num = ServerMaxNum;
  gmgroup[id].startindex = ServerIndex;
  const std::string groupNameGbk = Utf8ToGbk(GroupName);
  strcpy_s(gmgroup[id].name, groupNameGbk.c_str());
  return 1;
}

static int SetServerList(lua_State *L) {
  size_t l;
  const int id = luaL_checkint(L, 1);
  const char *ServerIP = luaL_checklstring(L, 2, &l);
  const char *ServerPort = luaL_checklstring(L, 3, &l);
  const char *ServerName = luaL_checklstring(L, 4, &l);
  if (id < 0 || id >= MAX_GMSV)
    return luaL_error(L, "server id out of range: %d", id);
  strcpy_s(gmsv[id].ipaddr, ServerIP);
  strcpy_s(gmsv[id].port, ServerPort);
  const std::string serverNameGbk = Utf8ToGbk(ServerName);
  strcpy_s(gmsv[id].name, serverNameGbk.c_str());
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
