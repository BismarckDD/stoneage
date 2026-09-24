#include "version.h"
//
#include "gmsv_server.h"
//
#include "anim_tbl.h"
#include "char.h"
#include "map_util.h"
#include "net.h"
#include "readmap.h"

BOOL MAP_walkAbleFromPoint(int ff, int fx, int fy, BOOL isfly)
{
  int map[2];
  if (!MAP_getTileAndObjData(ff, fx, fy, &map[0], &map[1]))
    return FALSE;
  if (isfly)
  {
    int i;
    for (i = 0; i < 2; i++)
      if (MAP_getImageInt(map[i], MAP_HAVEHEIGHT) == TRUE)
        return FALSE;
    return TRUE;
  }
  switch (MAP_getImageInt(map[1], MAP_WALKABLE))
  {
  case 0:
    return FALSE;
  case 1:
    return MAP_getImageInt(map[0], MAP_WALKABLE);
  case 2:
    return TRUE;
  default:
    return FALSE;
  }
  return FALSE;
}

BOOL MAP_walkAble(int char_index, int ff, int fx, int fy)
{
  if (CHAR_getFlg(char_index, CHAR_ISTRANSPARENT))
    return TRUE;
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEBUS)
    return TRUE;
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT)
  {
    int oyaindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex))
    {
      if (CHAR_getInt(oyaindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS)
        return TRUE;
    }
  }
  return MAP_walkAbleFromPoint(ff, fx, fy,
                               CHAR_getFlg(char_index, CHAR_ISFLYING) |
                                   CHAR_getWorkInt(char_index, CHAR_WORKSKYWALKER));
}

BOOL MAP_haveHeight(int fl, int x, int y)
{
  int map[2];
  if (!MAP_getTileAndObjData(fl, x, y, &map[0], &map[1]))
    return FALSE;
  return MAP_getImageInt(map[1], MAP_HAVEHEIGHT);
}

// mode: 0 -> pre_event.
// mode: 1 -> post_event.
static void MAP_dealprepostevent(int index, BOOL mode)
{
  int map[2];
  int i;
  int damaged = FALSE;

  if (CHAR_getFlg(index, CHAR_ISFLYING))
    return;

  if (!MAP_getMapDataFromCharIndex(index, map))
    return;

  for (i = 0; i < 2; i++)
  {
    int damage;
    damage = MAP_getImageInt(map[i], mode ? MAP_INTODAMAGE : MAP_OUTOFDAMAGE);
    if (damage != 0)
    {
      int opt[2] = {CG_HIT_MARK_10, -damage};
      damaged = TRUE;
      CHAR_setInt(index, CHAR_HP, CHAR_getInt(index, CHAR_HP) + damage);
      CHAR_complianceParameter(index);
      CHAR_sendWatchEvent(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX),
                          CHAR_ACTDAMAGE, opt, 2, TRUE);
    }
  }
  if (damaged)
    CHAR_sendStatusString(index, "M");
}

void MAP_preovered(int index) { MAP_dealprepostevent(index, FALSE); }

void MAP_postovered(int index) { MAP_dealprepostevent(index, TRUE); }

BOOL MAP_sendArroundCharNeedFD(int fd, int char_index)
{
  char *stringdata;
  int x = CHAR_getInt(char_index, CHAR_X);
  int y = CHAR_getInt(char_index, CHAR_Y);
  int fl = CHAR_getInt(char_index, CHAR_FLOOR);
  int size = MAP_CHAR_DEFAULTSEESIZ;
  RECT seekr, retr;
  seekr.x = x - (int)(size / 2);
  seekr.y = y - (int)(size / 2);
  seekr.width = size;
  seekr.height = size;
#ifdef _MO_MAP_AUTO_UPDATE
  {
    int tilesum, objsum, eventsum;
    stringdata = MAP_getChecksumFromRECT(fl, &seekr, &retr, &tilesum, &objsum,
                                         &eventsum);
    if (stringdata == NULL)
      return FALSE;

    GmsvServer_MC_send(fd, fl, retr.x, retr.y, retr.x + retr.width,
                       retr.y + retr.height, tilesum, objsum, eventsum,
                       stringdata);
  }
#else
  stringdata = MAP_getdataFromRECT(fl, &seekr, &retr);
  if (stringdata == NULL)
    return FALSE;

  GmsvServer_M_send(fd, fl, retr.x, retr.y, retr.x + retr.width,
                    retr.y + retr.height, stringdata);
#endif
  return TRUE;
}

BOOL MAP_sendArroundChar(int char_index)
{
  const int fd = getfdFromCharaIndex(char_index);
  if (fd == -1)
    return FALSE;
  return MAP_sendArroundCharNeedFD(fd, char_index);
}
