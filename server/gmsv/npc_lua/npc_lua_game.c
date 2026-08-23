#include "npc_lua.h"
//
#include "anim_tbl.h"
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "family.h"
#include "item.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"
#include "saac_client.h"
#include "util.h"


#ifdef _MO_LNS_CHARSUOXU
int NPC_Lua_Game_FMPOINT_GetData(lua_State *lua) {
  CheckEx(lua, 2);
  CheckIndexNull(lua, 1);
  int GA_Index = (int)lua_tointeger(lua, 1);
  int GA_Flg = (int)lua_tointeger(lua, 2);
  if (GA_Flg >= 0)
  {
    if (GA_Flg < LUA_DATALINE1)
    {
      GA_Flg -= LUA_DATALINE0;
      char *GA_RetInt = FM_getManorData(GA_Index, GA_Flg);
      LRetInt(lua, atoi(GA_RetInt));
    } else if (GA_Flg < LUA_DATALINE2)
    {
      GA_Flg -= LUA_DATALINE1;
      char *GA_RetPoint = FM_getManorData(GA_Index, GA_Flg);
      LRetMsg(lua, GA_RetPoint);
    } else
      LRetErrNull(lua, "传入的标志是错误的。");
  }
  LRetErrNull(lua, "传入的索引是无效的。");
}
#endif

int NPC_Lua_Game_FMPOINT_ACSetFMPoint(lua_State *lua)

{

  CheckEx(lua, 5);
  CheckIndexNull(lua, 1);
  int leaderindex = (int)lua_tointeger(lua, 1);
  int fmpointindex = (int)lua_tointeger(lua, 2);
  int map = (int)lua_tointeger(lua, 3);
  int x = (int)lua_tointeger(lua, 4);
  int y = (int)lua_tointeger(lua, 5);
  if (!CHAR_CHECKINDEX(leaderindex))
    LRetErrInt(lua, -1, "传入的索引是无效的。");
  if (fmpointindex < 0 || fmpointindex > 9)
    LRetErrInt(lua, -1, "传入的庄园编号是无效的。");
  int fd = getfdFromCharaIndex(leaderindex);
  if (fd == -1)
    LRetErrInt(lua, -2, "该玩家可能已经下线！");
  char *tmpbuf = "";

  sprintf(tmpbuf, "%d|%d|%d|%d|%d|", leaderindex, fmpointindex, map, x, y);

  FAMILY_SetPoint(fd, leaderindex, tmpbuf);

  LRetInt(lua, 0);
}

int NPC_Lua_Game_FMPOINT_ACFixFMPoint(lua_State *lua)
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int winindex = (int)lua_tointeger(lua, 1);
  int loseindex = (int)lua_tointeger(lua, 2);
  int id = (int)lua_tointeger(lua, 3);
  if (!CHAR_CHECKINDEX(winindex) || !CHAR_CHECKINDEX(loseindex))
    LRetErrNull(lua, "传入的索引是无效的。");
  if (id < 0 || id > 9)
    LRetErrNull(lua, "传入的庄园编号是无效的。");
  if (CHAR_getInt(winindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) // 不是族长
    LRetErrNull(lua, "传入的索引对象并不是族长");
  if (CHAR_getInt(loseindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) // 不是族长
    LRetErrNull(lua, "传入的索引对象并不是族长");
  SaacClient_ACFixFMPoint_send(acfd, CHAR_getChar(winindex, CHAR_FMNAME),
                               CHAR_getInt(winindex, CHAR_FMINDEX) + 1,
                               CHAR_getInt(winindex, CHAR_FMINDEX),
                               CHAR_getChar(loseindex, CHAR_FMNAME),
                               CHAR_getInt(loseindex, CHAR_FMINDEX) + 1,
                               CHAR_getInt(loseindex, CHAR_FMINDEX), id);
  LRetInt(lua, 0);
}
