#include "version.h"
#include "event.h"
#include "char.h"
#include "char_base.h"
#include "map_warppoint.h"
#include "npc_npcenemy.h"
#include "npc_warp.h"
#include "object.h"
#include "readmap.h"

static int EVENT_onWarpNPC(int char1, int char2, int fl, int x, int y);
static int EVENY_npcEncount(int char1, int char2, int fl, int x, int y);

typedef int (*func_pointer)(int char1, int char2, int fl, int x, int y);
static func_pointer func_tbl[] = {
    NULL,
    NULL,             /* ����NPC */
    EVENY_npcEncount, /* ���óľ޼��������� */
    EVENT_onWarpNPC,  /* warp */
    NULL,             /* normal NPC */
    NULL,             /* normal NPC */
    EVENT_onWarpNPC,  /* warp */
    EVENT_onWarpNPC,  /* warp */
    EVENT_onWarpNPC,  /* warp */
};

INLINE BOOL EVENT_CHECKEVENTINDEX(int event) {
  if (event < 0 || event >= CHAR_EVENTNUM)
    return FALSE;
  return TRUE;
}

int EVENT_main(const int char_index, int event, int x, int y) {
  OBJECT object;
  BOOL found = FALSE;
  int rc = FALSE;
  int floor = CHAR_getInt(char_index, CHAR_FLOOR);
  if (!EVENT_CHECKEVENTINDEX(event))
    return FALSE;
  for (object = MAP_getTopObj(floor, x, y); object; object = NEXT_OBJECT(object)) {
    int o = GET_OBJINDEX(object);
    if (OBJECT_getType(o) == OBJTYPE_CHARA) {
      int etype;
      int echaraindex = OBJECT_getIndex(o);
      if (!CHAR_CHECKINDEX(echaraindex))
        continue;
      if (CHAR_getInt(echaraindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
        continue;
      etype = CHAR_getWorkInt(echaraindex, CHAR_WORKEVENTTYPE);
      if (etype != CHAR_EVENT_NONE) {
        if (etype == event) {
          if (func_tbl[event] != NULL) {
            rc = func_tbl[event](char_index, echaraindex, floor, x, y);
          }
          found = TRUE;
          break;
        }
      }
    }
#ifdef __MAP_WARP_POINT
    else if (OBJECT_getType(o) == OBJTYPE_WARPPOINT) {
      if (!MAPPOINT_CHECKINDEX(OBJECT_getIndex(o)))
        continue;
      if (OBJECT_getchartype(o) != event)
        continue;
      MAPPOINT_MapWarpHandle(char_index, OBJECT_getIndex(o), floor, x, y);
      found = TRUE;
      rc = TRUE;
      break;
    }
#endif
  }
  if (!found) {
    print("not found eventobject : cind [%d] event [%d] floor[%d] x[%d] y[%d]\n",
          char_index, event, floor, x, y);
  }
  return rc;
}

static int EVENT_onWarpNPC(int char1, int char2, int fl, int x, int y) {
  NPC_WarpWarpCharacter(char2, char1);
  if (CHAR_getWorkInt(char1, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    for (i = 1; i < getPartyNum(char1); i++) {
      int index = CHAR_getWorkInt(char1, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        NPC_WarpWarpCharacter(char2, index);
      }
    }
  }

  return TRUE;
}
static int EVENY_npcEncount(int from_id, int to_id, int fl, int x, int y) {
  return NPC_NPCEnemy_Encount(to_id, from_id, 0);
}
