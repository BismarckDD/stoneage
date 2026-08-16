#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "common.h"

typedef enum {
  OBJTYPE_NOUSE,         /* 空, 还没使用 */
  OBJTYPE_CHARA,         /* 描述一个玩家 */
  OBJTYPE_ITEM,          /* 描述一个物品 */
  OBJTYPE_GOLD,          /* 描述一个金币 */
#ifdef __MAP_WARP_POINT
  OBJTYPE_WARPPOINT,     /* 描述一个传送点 */
#endif
  OBJTYPE_NUM
} OBJTYPE;

typedef struct tagObject {
  OBJTYPE type;
  char objname[256];
  int npcsindex;
  int dir;
  int imagenum;
  int chartype;
  int index;
  int x;
  int y;
  int floor;
#ifdef _DEL_DROP_GOLD
  int time;
#endif
} Object;

BOOL CHECKOBJECT(int index);
BOOL CHECKOBJECTUSE(int index);
BOOL initObjectArray(int num);
BOOL endObjectArray(void);

INLINE int _initObjectOne(char *file, int line, Object *ob);
#define initObjectOne(ob) _initObjectOne(__FILE__, __LINE__, ob)

void endObjectOne(int index);
int initObjectFromObjectMember(OBJTYPE type, int index, int x, int y,
                               int floor);

INLINE int OBJECT_getType(int index);
INLINE int OBJECT_setType(int index, int newvalue);
INLINE int OBJECT_getFloor(int index);
INLINE int OBJECT_setFloor(int index, int newvalue);
INLINE int OBJECT_getX(int index);
INLINE int OBJECT_setX(int index, int newvalue);
INLINE int OBJECT_getY(int index);
INLINE int OBJECT_setY(int index, int newvalue);
INLINE int OBJECT_getIndex(int index);
INLINE int OBJECT_setIndex(int index, int newvalue);
INLINE int OBJECT_getNum(void);

INLINE int OBJECT_getchartype(int index);
INLINE void OBJECT_setchartype(int index, int flg);

#ifdef _PET_ITEM
BOOL storeObjects(char *dirname);
BOOL restoreObjects(char *dirname);
#endif

#ifdef _DEL_DROP_GOLD
INLINE int OBJECT_setTime(int index, int newvalue);
INLINE int OBJECT_getTime(int index);
#endif

int searchObjectFromCharaIndex(int index);
#ifdef _MO_LNS_MAPSUOXU
INLINE int get_mappointindex(int fl, int x, int y);
#endif
#endif

/*__OBJECT_H__*/
