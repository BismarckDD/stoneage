#include "npc_lua.h"
//
#include "anim_tbl.h"
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "item.h"
#include "map_deal.h"
#include "net.h"
#include "npc_eventaction.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"
#include "util.h"

int NPC_Lua_Map_CheckCoordinates(lua_State *lua) // 检测某点是否在地图范围内。
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int x = (int)lua_tointeger(lua, 2);
  int y = (int)lua_tointeger(lua, 3);
  BOOL RETBOOL = MAP_checkCoordinates(map, x, y);
  LRetBool(lua, RETBOOL);
}

int NPC_Lua_Map_GetExitFloorXY(lua_State *lua) // 检测地图是否支持登出
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int maparray[3];
  int map_type = 0;
  unsigned int point;
  point = MAP_getExFloor_XY(map, &map_type);
  if (point > 0) {
    if (map_type >= 0) {                 // map_type >= 0 优先回纪录点
      if (map == map_type && map >= 0) { // 回纪录点
      } else {
        maparray[0] = (point >> 16) & 0xffffff;
        maparray[1] = (point >> 8) & 0xff;
        maparray[2] = (point >> 0) & 0xff;
      }
    }
  } else if (map == 10032) {
    maparray[0] = 10030;
    maparray[1] = 52;
    maparray[2] = 36;
  }
#ifdef _MAP_TIMEP
  else if (map >= 30017 && map >= 30021) {
    maparray[0] = 30008;
    maparray[1] = 39;
    maparray[2] = 39;
  }
#endif
  else {
    maparray[0] = map;
    maparray[1] = 0;
    maparray[2] = 0;
  }
  LRetArray(lua, maparray, arraysizeof(maparray));
}

int NPC_Lua_Map_GetfloorX(lua_State *lua) // 获取地图X长度
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int x = MAP_getfloorX(map);
  LRetInt(lua, x);
}

int NPC_Lua_Map_GetfloorY(lua_State *lua) // 获取地图Y长度
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int y;
  y = MAP_getfloorX(map);
  LRetInt(lua, y);
}

int NPC_Lua_Map_GetTileAndObjId(lua_State *lua) // 获取地图某点的地板和装饰层
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int x = (int)lua_tointeger(lua, 2);
  int y = (int)lua_tointeger(lua, 3);
  int tile = -1, obj = -1;
  MAP_getTileAndObjData(map, x, y, &tile, &obj);
  int maparray[2];
  maparray[0] = tile;
  maparray[1] = obj;
  LRetArray(lua, maparray, arraysizeof(maparray));
}

int NPC_Lua_Map_SetTileAndObjId(lua_State *lua) // 设置地图某点的地板和装饰层
{
  CheckEx(lua, 5);

  CheckIndexNull(lua, 1);

  int map = (int)lua_tointeger(lua, 1);
  int x = (int)lua_tointeger(lua, 2);
  int y = (int)lua_tointeger(lua, 3);
  int tile = (int)lua_tointeger(lua, 4);
  int obj = (int)lua_tointeger(lua, 5);

  if (MAP_setTileAndObjData(map, x, y, tile, obj)) {
    LRetInt(lua, 0);
  }
  LRetInt(lua, -1);
}

int NPC_Lua_Map_GetWalkAbleFromPoint(lua_State *lua) // 获取某点是否可以行走
{
  CheckEx2(lua, 3, 4);
  CheckIndexNull(lua, 1);
  int TM_Top = lua_gettop(lua);
  int map = (int)lua_tointeger(lua, 1);
  int x = (int)lua_tointeger(lua, 2);
  int y = (int)lua_tointeger(lua, 3);
  BOOL IsFly = FALSE;
  if (TM_Top == 4) {
    IsFly = (BOOL)lua_toboolean(lua, 4);
  }
  IsFly = MAP_walkAbleFromPoint(map, x, y, IsFly);
  LRetBool(lua, IsFly);
}

int NPC_Lua_Map_GetImageData(lua_State *lua) // 获取图片数据
{
  CheckEx(lua, 2);
  CheckIndexNull(lua, 1);
  int MapImageid = (int)lua_tointeger(lua, 1);
  int flg = (int)lua_tointeger(lua, 2);
  if (flg >= 0) {
    if (flg < LUA_DATALINE1) {
      flg -= LUA_DATALINE0;
      int TM_RetInt = MAP_getImageInt(MapImageid, flg);
      LRetInt(lua, TM_RetInt);
    }
  }
  LRetErrNull(lua, "传入的标志是错误的。");
}

int NPC_Lua_Map_SetImageData(lua_State *lua) // 设置图片数据
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int MapImageid = (int)lua_tointeger(lua, 1);
  int flg = (int)lua_tointeger(lua, 2);
  int val = (int)lua_tointeger(lua, 3);
  if (flg >= 0) {
    if (flg < LUA_DATALINE1) {
      flg -= LUA_DATALINE0;
      BOOL TM_RetBool = MAP_setImageInt(MapImageid, flg, val);
      LRetBool(lua, TM_RetBool);
    }
  }
  LRetErrNull(lua, "传入的标志是错误的。");
}
static OBJECT TM_Object = NULL;
int NPC_Lua_Map_GetTopObj(lua_State *lua) // 用来获取地图某点上对象
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int x = (int)lua_tointeger(lua, 2);
  int y = (int)lua_tointeger(lua, 3);
  TM_Object = MAP_getTopObj(map, x, y);
  LRetBool(lua, TRUE);
}

int NPC_Lua_Map_GetNextObj(lua_State *lua) // 获取下一个对象索引
{
  CheckEx(lua, 0);
  int TM_ObjIndex = GET_OBJINDEX(TM_Object);
  TM_Object = NEXT_OBJECT(TM_Object);
  if (!CHECKOBJECTUSE(TM_ObjIndex)) {
    LRetInt(lua, -1);
    TM_Object = NULL;
  }
  LRetInt(lua, TM_ObjIndex);
}

int NPC_Lua_Map_CheckImageIndex(lua_State *lua) // 检测某地图号图片是否存在
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  BOOL TM_Ret = IsValidImagenumber(map);
  LRetBool(lua, TM_Ret);
}

int NPC_Lua_Map_CheckIndex(lua_State *lua) // 检测某地图号地图是否存在
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  BOOL tm_ret = CHECKFLOORID(map);
  LRetBool(lua, tm_ret);
}

#ifdef _MO_LNS_MAPSUOXU

int NPC_Lua_Map_MakeNewMap(lua_State *lua) // 制造一个副本地图，并返回新地图号
{
  CheckEx2(lua, 1, 2);
  CheckIndexNull(lua, 1);
  int TM_Top = lua_gettop(lua);
  int map = (int)lua_tointeger(lua, 1);
  char *name = "";
  if (TM_Top == 2) {
    CheckIndexNull(lua, 2);
    name = lua_tostring(lua, 2);
  }
  int TM_Ret = MAP_makenew(map, name);
  LRetInt(lua, TM_Ret);
}

int NPC_Lua_Map_DelNewMap(lua_State *lua) // 删除一个副本地图
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  BOOL TM_Ret = MAP_DelMap(map);
  LRetBool(lua, TM_Ret);
}

int NPC_Lua_Map_SetExWarp(lua_State *lua) // 设置一个地图的退出传送点
{
  CheckEx(lua, 5);
  CheckIndexNull(lua, 1);
  int map = (int)lua_tointeger(lua, 1);
  int exfl = (int)lua_tointeger(lua, 2);
  int exx = (int)lua_tointeger(lua, 3);
  int exy = (int)lua_tointeger(lua, 4);
  int type = (int)lua_tointeger(lua, 4); // 地图类型
  BOOL TM_Ret = MAP_SetExWarp(map, exfl, exx, exy, type);
  LRetBool(lua, TM_Ret);
}

int NPC_Lua_Map_SetMapPoint(
    lua_State *lua) // 设置一个地图传送点 返回一个传送点索引
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  char *mappoint = lua_tostring(lua, 1);
  int TM_Ret = SetMapPoint(mappoint);
  LRetInt(lua, TM_Ret);
}

int NPC_Lua_Map_DelMapPoint(lua_State *lua) // 删除一个传送点
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int ps = (int)lua_tointeger(lua, 1);
  BOOL TM_Ret = DelMapPoint(ps);
  LRetBool(lua, TM_Ret);
}

#endif //

int NPC_Lua_Map_getFloorName(lua_State *lua) // 返回一个地图名
{
  CheckEx(lua, 1);
  CheckIndexNull(lua, 1);
  int map_id = (int)lua_tointeger(lua, 1);
  char *map_name = MAP_getFloorName(map_id);
  LRetMsg(lua, map_name);
}

/*
int NPC_Lua_Map_Upmap(lua_State *lua) //更新地图
{
  CheckEx(lua, 3);
  CheckIndexNull(lua, 1);
  int TM_Index = (int)lua_tointeger(lua, 1);
  int formatmap = (int)lua_tointeger(lua, 2);
  int tomap = (int)lua_tointeger(lua, 3);
  GmsvServer_Upmap_send(TM_Index,formatmap,tomap);
  LRetBool(lua, TRUE);
}
*/
