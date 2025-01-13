#include "version.h"

#include "autil.h"
#include "buf.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "map_warppoint.h"
#include "object.h"
#include "readmap.h"
#include "util.h"
#include "utils/file.h"

#ifdef __MAP_WARP_POINT
#define MAX_MAP_WARP_POINT 6000

typedef struct tagMapWarpPoint {
  int use;
  int ofloor;
  int ox;
  int oy;
  int floor;
  int x;
  int y;
  int type;
} MapWarpPoint;

MapWarpPoint *map_warp_point;
static int map_warp_point_num = 0;
/* */
char PointType[3][256] = {"NONE", "FREE", "ERROR"};

int GetObjType(const char *segment) {
  int obj_type = CHAR_EVENT_WARP;
  if (!strcmp(segment, "NULL")) {
  } else if (!strcmp(segment, "M")) {
    obj_type = CHAR_EVENT_WARP_MORNING;
  } else if (!strcmp(segment, "N")) {
    obj_type = CHAR_EVENT_WARP_NIGHT;
  } else if (!strcmp(segment, "A")) {
    obj_type = CHAR_EVENT_WARP_NOON;
  }
  return obj_type;
}

int GetPoint(const char *line, int *floor, int *x, int *y) {
  char segment[256];
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ",", 1, segment, sizeof(segment)) == FALSE) {
    return -1;
  }
  *floor = atoi(segment);
  if (getStringFromIndexWithDelim(line, ",", 2, segment, sizeof(segment)) == FALSE) {
    return -1;
  }
  *x = atoi(segment);
  if (getStringFromIndexWithDelim(line, ",", 3, segment, sizeof(segment)) == FALSE) {
    return -1;
  }
  *y = atoi(segment);
  if (*floor == -1 || *x == -1 || *y == -1)
    return -1;
  return 0;
}

int MAPPOINT_InitMapWarpPoint() {
  map_warp_point =
      (MapWarpPoint *)calloc(1, sizeof(MapWarpPoint) * MAX_MAP_WARP_POINT);
  if (map_warp_point == NULL)
    return 0;
  MAPPOINT_resetMapWarpPoint(0);
  return MAX_MAP_WARP_POINT;
}

void MAPPOINT_resetMapWarpPoint(int flg) {
  int i;
  if (map_warp_point == NULL)
    return;
  for (i = 0; i < MAX_MAP_WARP_POINT; i++) {
    map_warp_point[i].use = 0;
    map_warp_point[i].floor = -1;
  }
  if (flg == 1) {
    int objindex;
    int objmaxnum = OBJECT_getNum();
    for (objindex = 0; objindex < objmaxnum; objindex++) {
      if (CHECKOBJECT(objindex) == FALSE)
        continue;
      if (OBJECT_getType(objindex) != OBJTYPE_WARPPOINT)
        continue;
      CHAR_ObjectDelete(objindex);
    }
  }
}

// 这个方法搞了一个Object，但是没用？
int MAPPOINT_creatMapWarpObj(int point_index, char *line, int obj_type) {
  Object obj;
  obj.index = point_index;
  memset(obj.objname, 0, sizeof(obj.objname));
  if (GetPoint(line, &obj.floor, &obj.x, &obj.y) == -1) {
    return -1;
  }
  obj.type = OBJTYPE_WARPPOINT;
  obj.chartype = obj_type;
  obj.dir = 0;
  int obj_index = initObjectOne(&obj);
  // if (obj_index == -1) {
  // print("Failed to init obj index.")
  // }
  return obj_index;
}

BOOL MAPPOINT_CHECKINDEX(int warp_point_idx) {
  if (warp_point_idx < 0 || warp_point_idx >= MAX_MAP_WARP_POINT)
    return FALSE;
  return map_warp_point[warp_point_idx].use;
}

int MAPPOINT_getMPointEVType(int warp_point_idx) {
  if (!MAPPOINT_CHECKINDEX(warp_point_idx))
    return -1;
  return map_warp_point[warp_point_idx].type;
}

int MAPPOINT_setMapWarpFrom(int warp_point_idx, char *line) {
  if (MAPPOINT_CHECKINDEX(warp_point_idx)) {
    print("设置传送点入口 :%s!!\n", line);
    return -1;
  }
  if (GetPoint(line, &map_warp_point[warp_point_idx].ofloor,
               &map_warp_point[warp_point_idx].ox,
               &map_warp_point[warp_point_idx].oy) == -1) {
    return -1;
  }
  return 1;
}

int MAPPOINT_setMapWarpGoal(int warp_point_idx, char *line) {
  if (MAPPOINT_CHECKINDEX(warp_point_idx)) {
    print("设置传送点出口 :%s!!\n", line);
    return -1;
  }
  if (GetPoint(line, &map_warp_point[warp_point_idx].ofloor,
               &map_warp_point[warp_point_idx].ox,
               &map_warp_point[warp_point_idx].oy) == -1) {
    return -1;
  }
  return 1;
}

int MAPPOINT_getMapWarpGoal(int warp_point_idx, int ofl, int ox, int oy,
                            int *fl, int *x, int *y) {
  if (!MAPPOINT_CHECKINDEX(warp_point_idx)) {
    print("获取传送点warp_point_idx:%d 错误!!\n", warp_point_idx);
    return -1;
  }

  if (map_warp_point[warp_point_idx].ofloor != ofl ||
      map_warp_point[warp_point_idx].ox != ox ||
      map_warp_point[warp_point_idx].oy != oy) {
    print("不正常传送点: 旧的有问题传送点 %d %d %d !!\n", ofl, ox, oy);
    return -1;
  }
  // 可加判断条件
  if (MAP_IsValidCoordinate(map_warp_point[warp_point_idx].floor,
                            map_warp_point[warp_point_idx].x,
                            map_warp_point[warp_point_idx].y) == FALSE) {
    print("不正常传送点:有问题 %d %d %d !!\n",
          map_warp_point[warp_point_idx].floor,
          map_warp_point[warp_point_idx].x, map_warp_point[warp_point_idx].y);
    return -1;
  }
  *fl = map_warp_point[warp_point_idx].floor;
  *x = map_warp_point[warp_point_idx].x;
  *y = map_warp_point[warp_point_idx].y;
  return 1;
}

void callbackReadMapWarpPoint(int *warp_point_idx, const char *line) {
  char segment[256];
  int i;
  if (getStringFromIndexWithDelim(line, ":", 1, segment, sizeof(segment)) ==
      FALSE) {
    print("1.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  for (i = 0; i < arraysizeof(PointType); i++) {
    if (!strcmp(segment, PointType[i]))
      break;
  }
  if (i >= arraysizeof(PointType)) {
    print("1-1.map 传送点错误 %s \n", line);
    return;
  }
  map_warp_point[*warp_point_idx].type = i;
  if (getStringFromIndexWithDelim(line, ":", 2, segment, sizeof(segment)) ==
      FALSE) {
    print("2.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  int objtype = GetObjType(segment);
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ":", 3, segment, sizeof(segment)) ==
      FALSE) {
    print("3.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  if (MAPPOINT_setMapWarpFrom(*warp_point_idx, segment) == -1) {
    print("3-1.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  if (MAPPOINT_creatMapWarpObj(&warp_point_idx, segment, objtype) == -1) {
    print("3-2.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ":", 4, segment, sizeof(segment)) ==
      FALSE) {
    print("4.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  if (MAPPOINT_setMapWarpGoal(*warp_point_idx, segment) == -1) {
    print("4-1.map 传送点错误 %s \n", line);
    return;
  }
  memset(segment, 0, sizeof(segment));
  // 这一段好像没啥用？
  if (getStringFromIndexWithDelim(line, ":", 5, segment, sizeof(segment)) ==
      FALSE) {
    print("5.map 传送点错误 %s [%s] \n", line, segment);
    return;
  }
  map_warp_point[*warp_point_idx].use = 1;
  ++warp_point_idx;
  // TODO: 修改后这里是有问题的，并不起作用.
  // 后面开发时应该通过返回值判断循环是否继续。
  if (warp_point_idx >= MAX_MAP_WARP_POINT) {
    return;
  }
}

int MAPPOINT_loadMapWarpPoint() {
  char filename[256];
  sprintf(filename, "%s/mapwarp.txt", getMapdir());
  get_file_lines(filename, &map_warp_point_num, callbackReadMapWarpPoint);
  print("初始化 %d 地图传送点...", map_warp_point_num);
}

void MAPPOINT_MapWarpHandle(int char_index, int warp_point_idx, int ofl, int ox,
                            int oy) {
  int floor, x, y;
  if (MAPPOINT_getMapWarpGoal(warp_point_idx, ofl, ox, oy, &floor, &x, &y) ==
      -1) {
    print("获取传送点( %d, %d,%d,%d)错误!!\n", warp_point_idx, ofl, ox, oy);
    return;
  }
  if (floor == 777)
    return;
  CHAR_warpToSpecificPoint(char_index, floor, x, y);
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    for (i = 1; i < getPartyNum(char_index); i++) {
      int index = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        CHAR_warpToSpecificPoint(index, floor, x, y);
      }
    }
  }
}

#ifdef _MO_LNS_MAPSUOXU
int DelMapPoint(int warp_point_idx) // 卸载地图传送点
{
  if (map_warp_point[warp_point_idx].use != 1)
    return FALSE;
  map_warp_point[warp_point_idx].type = 0; // 设置地图的传送点类型
  endObjectOne(get_mappointindex(map_warp_point[warp_point_idx].ofloor,
                                 map_warp_point[warp_point_idx].ox,
                                 map_warp_point[warp_point_idx].oy)); // 清掉obj
  map_warp_point[warp_point_idx].ofloor = -1; // 设置地图的传送点类型
  map_warp_point[warp_point_idx].ox = -1;     // 设置地图的传送点类型
  map_warp_point[warp_point_idx].oy = -1;     // 设置地图的传送点类型
  map_warp_point[warp_point_idx].floor = -1;  // 设置地图的传送点类型
  map_warp_point[warp_point_idx].x = -1;      // 设置地图的传送点类型
  map_warp_point[warp_point_idx].y = -1;      // 设置地图的传送点类型
  map_warp_point[warp_point_idx].use = 0;
  map_warp_point[warp_point_idx].type = 0;
  return TRUE; // 注意设置传送点时先检查是否原来有传送点
}

int SetMapPoint(char *line) // 加载地图传送点
{                           // "传送点类型:NULL:2000,50,50:2006,20,20:？"
  if (map_warp_point_num >= MAX_MAP_WARP_POINT) {
    print("传送点已达上限！\n");
    return -1;
  }
  char segment[256];
  char line2[256];
  int objtype, i;
  if (getStringFromIndexWithDelim(line, ":", 1, segment, sizeof(segment)) ==
      FALSE) // 找出传送点类型
    return -2;
  for (i = 0; i < arraysizeof(PointType); i++) {
    if (!strcmp(segment, PointType[i]))
      break;
  }
  if (i >= arraysizeof(PointType)) // 传送点类型错误
    return -3;
  map_warp_point[map_warp_point_num].type = i; // 设置地图的传送点类型
  if (getStringFromIndexWithDelim(line, ":", 2, segment, sizeof(segment)) ==
      FALSE) // 找出传送点有效时间
    return -4;
  objtype = GetObjType(segment); // 设置对象为传送点
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ":", 3, segment, sizeof(segment)) ==
      FALSE) // 找出原点
    return -5;
  int fl = -1, x = -1, y = -1;
	GetPoint(segment, &fl, &x, &y);
  if (fl == -1 || x == -1 || y == -1)
    return -5;
  if (get_mappointindex(fl, x, y) != -1)
    return -5;                             // 设置过传送点！
  for (i = 0; i < map_warp_point_num; i++) // 找出一个空闲的传送点
    if (map_warp_point[i].use != 1)
      break;                                       // 找到空闲传送点
  if (MAPPOINT_setMapWarpFrom(i, segment) == -1) { // 放置原点
    return -6;
  }
  if (MAPPOINT_creatMapWarpObj(i, segment, objtype) == -1) { // 放置原点对象
    return -7;
  }
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ":", 4, segment, sizeof(segment)) ==
      FALSE) {
    return -8;
  }
  if (MAPPOINT_setMapWarpGoal(i, segment) == -1) {
    return -9;
  }
  memset(segment, 0, sizeof(segment));
  if (getStringFromIndexWithDelim(line, ":", 5, segment, sizeof(segment)) ==
      FALSE) {
    return -10;
  }
  map_warp_point[i].use = 1;
  if (i >= map_warp_point_num)
    map_warp_point_num++; // 如果当前传送点已经到达最后，预设下一个传送点。
  return i;
}
#endif

#endif
