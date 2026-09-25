#include "version.h"
//
#include "gmsv_server.h"
//
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "char_event.h"
#include "config_file.h"
#include "encount.h"
#include "handletime.h"
#include "map_deal.h"
#include "net.h"
#include "npc_npcenemy.h"
#include "npccreate.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"

static void CHAR_sendCharaAtWalk(int char_index,
  int of, int ox, int oy, int xflg, int yflg);
static void CHAR_sendCDCharaAtWalk(int char_index,
  int of, int ox, int oy, int xflg, int yflg);
typedef void (*POSTOFUNC)(int, int);

// 转向？
static CHAR_WALKRET CHAR_walk_turn(int index, int dir) {
  OBJECT object;
  if (!CHAR_CHECKINDEX(index))
    return CHAR_WALKSYSTEMERROR;
  CHAR_setInt(index, CHAR_DIR, dir);
  int fx = CHAR_getInt(index, CHAR_X);
  int fy = CHAR_getInt(index, CHAR_Y);
  int ff = CHAR_getInt(index, CHAR_FLOOR);
  CHAR_sendWatchEvent(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX), CHAR_ACTTURN,
                      NULL, 0, FALSE);
  CHAR_setWorkInt(index, CHAR_WORKACTION, CHAR_ACTTURN);
  for (object = MAP_getTopObj(ff, fx, fy); object;
       object = NEXT_OBJECT(object)) {
    POSTOFUNC pfunc = NULL;
    int obj_index = GET_OBJINDEX(object);
    switch (OBJECT_getType(obj_index)) {
    case OBJTYPE_CHARA: // 角色的回调函数
      pfunc = (POSTOFUNC)CHAR_getFunctionPointer(
        OBJECT_getIndex(obj_index), CHAR_POSTOVERFUNC);
      break;
    case OBJTYPE_ITEM: // 物品的回调函数
      pfunc = (POSTOFUNC)ITEM_getFunctionPointer(
        OBJECT_getIndex(obj_index), ITEM_POSTOVERFUNC);
      break;
    case OBJTYPE_GOLD:
      break;
    default:
      break;
    }
    if (pfunc)
      pfunc(OBJECT_getIndex(obj_index), index);
  }
  return CHAR_WALKSUCCESSED;
}

static void CHAR_sendMapAtWalk(int index, int fl, int ox, int oy, int fx,
                               int fy) {
  int vx, vy;
  const int seesiz = MAP_CHAR_DEFAULTSEESIZ;

  if (!CHAR_CHECKINDEX(index))
    return;

  vx = fx - ox;
  vy = fy - oy;

  if (ABS(vx) >= seesiz / 2 || /*  差が大きい  (差值过大)  */
      ABS(vy) >= seesiz / 2) { /*  差が大きい  (差值过大)  */
    return;

  } else {
    RECT send, get;
    char *mapdata;
    int oldlux = ox - (int)(seesiz / 2);
    int oldluy = oy - (int)(seesiz / 2);
    int absx = ABS(vx);
    int absy = ABS(vy);
    if (vx != 0) {
      if (vx > 0) {
        send.x = oldlux + seesiz;
        send.width = absx;
      } else if (vx < 0) {
        send.x = oldlux - absx;
        send.width = absx;
      }

      if (vy >= 0) {
        send.y = oldluy;
        send.height = seesiz + absy;
      } else {
        send.y = oldluy - absy;
        send.height = seesiz + absy;
      }
      {
        int tilesum, objsum, eventsum;
        mapdata = MAP_getChecksumFromRECT(fl, &send, &get, &tilesum, &objsum,
                                          &eventsum);
        if (mapdata != NULL && !(get.width == 0 && get.height == 0)) {
          int fd = getfdFromCharaIndex(index);
          if (fd != -1)
            GmsvServer_MC_send(fd, fl, get.x, get.y, get.x + get.width,
                             get.y + get.height, tilesum, objsum, eventsum,
                             mapdata);
        }
      }
    }

    /* == 0 の時は送らなくてよい (为0时无需发送) */
    if (vy != 0) {
      if (vy > 0) {
        send.y = oldluy + seesiz;
        send.height = absy;
      } else {
        send.y = oldluy - absy;
        send.height = absy;
      }
      if (vx >= 0) {
        send.x = oldlux;
        send.width = seesiz + absx;
      } else if (vx < 0) {
        send.x = oldlux - absx;
        send.width = seesiz + absx;
      }
      {
        int tilesum, objsum, eventsum;
        mapdata = MAP_getChecksumFromRECT(fl, &send, &get, &tilesum, &objsum,
                                          &eventsum);
        if (mapdata != NULL && !(get.width == 0 && get.height == 0)) {
          int fd = getfdFromCharaIndex(index);
          if (fd != -1)
            GmsvServer_MC_send(fd, fl, get.x, get.y, get.x + get.width,
                             get.y + get.height, tilesum, objsum, eventsum,
                             mapdata);
        }
      }
    }
  }
}


static const POINT scOffset[5] = {
  {0, -1}, {-1, 0}, {0, 0}, {1, 0}, {0, 1},
};

/*------------------------------------------------------------
 * 移動する  (移动)［
 * 引数  (参数)
 *  index  int  キャラのインデックス  (角色索引)
 *  dir    int  方向  (方向)
 * 戻り値  (返回值)
 *  CHAR_WALKRET
 ------------------------------------------------------------*/
static CHAR_WALKRET CHAR_tryWalkMove(int char_index, int dir, int of,
                                      int ox, int oy, int ff, int fx, int fy) {
  int i;
  int objbuf[128];
  int objbufindex = 0;
  int notover = FALSE;
  int retvalue = CHAR_WALKSUCCESSED;
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
    RECT walkr;
    POINT nextp;
    int npccreateindex = CHAR_getInt(char_index, CHAR_NPCCREATEINDEX);
    if (CHAR_isInvincibleArea(ff, fx, fy)) {
      CHAR_setInt(char_index, CHAR_DIR, dir);
      retvalue = CHAR_WALKEXTEND;
      return retvalue;
    }
    if (NPC_isBoundarySet(npccreateindex) &&
        NPC_createGetRECT(npccreateindex, &walkr) == TRUE) {
      nextp.x = fx;
      nextp.y = fy;
      if (PointInRect(&walkr, &nextp) == FALSE) {
        CHAR_setInt(char_index, CHAR_DIR, dir);
        retvalue = CHAR_WALKEXTEND;
        return retvalue;
      }
    }
  }

  if (CHAR_getFlg(char_index, CHAR_ISBIG) == 1) {

    int k;
    for (k = 0; k < 5; k++) {
      if (!MAP_walkAble(char_index, ff, fx + scOffset[k].x, fy + scOffset[k].y)) {
        CHAR_setInt(char_index, CHAR_DIR, dir);
        retvalue = CHAR_WALK1357;
        return retvalue;
      }
    }

  } else {
    if (CHAR_getDX(dir) * CHAR_getDY(dir) == 0) {
      if (!MAP_walkAble(char_index, ff, fx, fy)) {
        CHAR_setInt(char_index, CHAR_DIR, dir);
        retvalue = CHAR_WALK1357;
        return retvalue;
      }
    } else {
      int xflg, yflg;
      if (!MAP_walkAble(char_index, ff, fx, fy)) {
        CHAR_setInt(char_index, CHAR_DIR, dir);
        retvalue = CHAR_WALK1357;
        return retvalue;
      }

      xflg = MAP_walkAble(char_index, of, ox + CHAR_getDX(dir), oy);
      yflg = MAP_walkAble(char_index, of, ox, oy + CHAR_getDY(dir));

      if (!xflg || !yflg) {
        CHAR_setInt(char_index, CHAR_DIR, dir);
        retvalue = CHAR_WALK1357;
        return retvalue;
      }
    }
  }
  objbufindex =
      CHAR_getSameCoordinateObjects(objbuf, arraysizeof(objbuf), ff, fx, fy);
  for (i = 0; i < objbufindex; i++) {
    int obj_index = objbuf[i];
    switch (OBJECT_getType(obj_index)) {
    case OBJTYPE_CHARA:
      if (CHAR_CHECKINDEX(OBJECT_getIndex(obj_index)) == TRUE) {
#ifdef _PLAYER_OVERLAP_PK
        if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
            CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                CHAR_TYPEPLAYER) {
          if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) !=
              CHAR_PARTY_NONE) {
            int i;
            for (i = 0; i < getPartyNum(char_index); i++) {
              int subindex =
                  CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(subindex) == FALSE)
                continue;
              if (OBJECT_getIndex(obj_index) == subindex)
                break;
            }

            if (i == getPartyNum(char_index)) {
              int j;
              for (j = 0; j < 32; j++) {
                if (CHAR_getInt(char_index, CHAR_FLOOR) ==
                    getPlayerOverlapPk(j)) {
                  BATTLE_CreateVsPlayer(char_index, OBJECT_getIndex(obj_index));
                  break;
                }
              }
            }
          }
        }
#endif
      }
      if (!CHAR_getFlg(OBJECT_getIndex(obj_index), CHAR_ISOVERED)) {
        if (!CHAR_CHECKINDEX(OBJECT_getIndex(obj_index))) {
          printf("自动删除一个问题对象！\n");
          endObjectOne(obj_index);
          break;
        }
        notover = TRUE;
      }
      break;
    case OBJTYPE_ITEM:
      if (!ITEM_getInt(OBJECT_getIndex(obj_index), ITEM_ISOVERED))
        notover = TRUE;
      break;
    case OBJTYPE_GOLD:
      break;
    default:
      break;
    }
  }
  if (notover == TRUE) {
    CHAR_setInt(char_index, CHAR_DIR, dir);
    retvalue = CHAR_WALKHITOBJECT;
  } else {
    for (i = 0; i < objbufindex; i++) {
      typedef void (*PREOFUNC)(int, int);
      PREOFUNC pfunc = NULL;
      int obj_index = objbuf[i];
      switch (OBJECT_getType(obj_index)) {
      case OBJTYPE_CHARA:
        pfunc = (PREOFUNC)CHAR_getFunctionPointer(OBJECT_getIndex(obj_index),
                                                  CHAR_PREOVERFUNC);
        break;
      case OBJTYPE_ITEM:
        pfunc = (PREOFUNC)ITEM_getFunctionPointer(OBJECT_getIndex(obj_index),
                                                  ITEM_PREOVERFUNC);
        break;
      case OBJTYPE_GOLD:
        break;
      default:
        break;
      }
      if (pfunc)
        pfunc(OBJECT_getIndex(obj_index), char_index);
    }

    CHAR_setInt(char_index, CHAR_X, fx);
    CHAR_setInt(char_index, CHAR_Y, fy);
    CHAR_setInt(char_index, CHAR_FLOOR, ff);
    CHAR_setInt(char_index, CHAR_DIR, dir);
    {
      int obj_index;
      int ox, oy, of;
      obj_index = CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX);
      of = OBJECT_setFloor(obj_index, CHAR_getInt(char_index, CHAR_FLOOR));
      ox = OBJECT_setX(obj_index, CHAR_getInt(char_index, CHAR_X));
      oy = OBJECT_setY(obj_index, CHAR_getInt(char_index, CHAR_Y));
      if (!MAP_objmove(obj_index, of, ox, oy, ff, fx, fy)) {
        /* 修复：走路必须同步 olink 网格挂点。可见性扫描
           (CHAR_sendArroundCharaData / CHAR_sendCToArroundCharacter) 只按
           网格格子取候选对象，挂点不更新则玩家走出 ±10 格后彻底失联。
           该调用在 a578d7c(2026.08.27 fix tcp) 中被删除。 */
        print("%s:%d:[MAP_OBJMOVE_FAIL] walk obj_index=%d cell(%d,%d,%d)->"
              "(%d,%d,%d) char=%d\n",
              __FILE__, __LINE__, obj_index, of, ox, oy, ff, fx, fy, char_index);
      }
    }

    CHAR_setInt(char_index, CHAR_WALKCOUNT,
                CHAR_getInt(char_index, CHAR_WALKCOUNT) + 1);

    for (i = 0; i < objbufindex; i++) {
      POSTOFUNC pfunc = NULL;
      int obj_index = objbuf[i];
      switch (OBJECT_getType(obj_index)) {
      case OBJTYPE_CHARA:
        pfunc = (POSTOFUNC)CHAR_getFunctionPointer(
          OBJECT_getIndex(obj_index), CHAR_POSTOVERFUNC);
        break;
      case OBJTYPE_ITEM:
        pfunc = (POSTOFUNC)ITEM_getFunctionPointer(
          OBJECT_getIndex(obj_index), ITEM_POSTOVERFUNC);
        break;
      case OBJTYPE_GOLD:
        break;
      default:
        break;
      }
      if (pfunc)
        pfunc(OBJECT_getIndex(obj_index), char_index);
    }
    objbufindex =
        CHAR_getSameCoordinateObjects(objbuf, arraysizeof(objbuf), of, ox, oy);
    for (i = 0; i < objbufindex; i++) {
      typedef void (*OFFFUNC)(int, int);
      OFFFUNC ofunc = NULL;
      int obj_index = objbuf[i];
      switch (OBJECT_getType(obj_index)) {
      case OBJTYPE_CHARA:
        ofunc = (OFFFUNC)CHAR_getFunctionPointer(
          OBJECT_getIndex(obj_index), CHAR_OFFFUNC);
        break;
      case OBJTYPE_ITEM: /* に何もしない (不处理) */
        break;
      case OBJTYPE_GOLD: /* に何もしない (不处理) */
        break;
      default:
        break;
      }
      if (ofunc)
        ofunc(OBJECT_getIndex(obj_index), char_index);
    }
  }
  return retvalue;
}

static CHAR_WALKRET CHAR_walk_move(int char_index, int dir) {
  int i;
  int fx, fy, ff;
  int retvalue = CHAR_WALKSUCCESSED;
  if (!CHAR_CHECKINDEX(char_index))
    return CHAR_WALKSYSTEMERROR;
  int ox = CHAR_getInt(char_index, CHAR_X);
  int oy = CHAR_getInt(char_index, CHAR_Y);
  int of = CHAR_getInt(char_index, CHAR_FLOOR);
  CHAR_getCoordinationDir(dir, ox, oy, 1, &fx, &fy);
  ff = of;
  retvalue = CHAR_tryWalkMove(char_index, dir, of, ox, oy, ff, fx, fy);
  if (retvalue == CHAR_WALK1357 || retvalue == CHAR_WALKHITOBJECT) {
    {
      int opt[2] = {ox, oy};
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTWALK, opt, 2, TRUE);
    }
    CHAR_setWorkChar(char_index, CHAR_WORKWALKARRAY, "");
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTWARP, NULL, 0, TRUE);
    }
  } else if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    BOOL flg = FALSE;
    int par;
    int count;
    CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_NONE) {
      CHAR_sendCharaAtWalk(char_index, of, ox, oy, CHAR_getDX(dir),
                           CHAR_getDY(dir));
    }

    {
      int opt[2] = {ox, oy};
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTWALK, opt, 2, TRUE);
    }
#ifdef _MO_MAP_AUTO_UPDATE
    CHAR_sendMapAtWalk(char_index, of, ox, oy, CHAR_getInt(char_index, CHAR_X),
                       CHAR_getInt(char_index, CHAR_Y));
#endif
    count = CHAR_getWorkInt(char_index, CHAR_WORK_TOHELOS_COUNT);
    if (count > 0) {
      CHAR_setWorkInt(char_index, CHAR_WORK_TOHELOS_COUNT, count - 1);
      if (count - 1 == 0) {
        CHAR_talkToCli(char_index, -1, "道具的效力已到。", CHAR_COLORWHITE);
      }
    }
    par = ENCOUNT_getEncountPercentMin(char_index, of, ox, oy);
    if (par != -1) {
      if (CHAR_getWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MIN) != par) {
        flg = TRUE;
        CHAR_setWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MIN, par);
      }
    }
    par = ENCOUNT_getEncountPercentMax(char_index, of, ox, oy);
    if (par != -1) {
      if (CHAR_getWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MAX) != par) {
        flg = TRUE;
        CHAR_setWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MAX, par);
      }
    }
    // 2026.09.21 判断是否遇敌
    // POST_WALK
    {
      int encount_fd = getfdFromCharaIndex(char_index);
      int eqNoEncount = getEqNoEnemy(encount_fd); // Arminius 7.2: Ra's amulet
      int noEncount = getNoEnemy(encount_fd);

      // Arminius 7.31 cursed stone
      if (getStayEncount(encount_fd) > 0) {
        clearStayEncount(encount_fd);
      }
      // print("\n encount_fd=%d,eqNoEncount=%d,noEncount=%d", encount_fd, eqNoEncount, noEncount);
      // Arminius 7.2 Ra's amulet
      if (eqNoEncount >= 200) { // 
        noEncount = 1;
      } else if (eqNoEncount >= 120) {
        if ((ff == 100) || (ff == 200) || (ff == 300) || (ff == 400) ||
            (ff == 500))
          noEncount = 1;
      } else if (eqNoEncount >= 80) {
        if ((ff == 100) || (ff == 200) || (ff == 300) || (ff == 400))
          noEncount = 1;
      } else if (eqNoEncount >= 40) {
        if ((ff == 100) || (ff == 200))
          noEncount = 1;
      }
      // 特殊不遇敌的地图
      if (noEncount > 0) {
        if ((ff == 8252) || (ff == 8253) || (ff == 8254) || (ff == 8256)) {
          noEncount = 0;
        }
      }
      if (noEncount == 0) {
        int maxep =
            CHAR_getWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MAX);
        int minep =
            CHAR_getWorkInt(char_index, CHAR_WORKENCOUNTPROBABILITY_MIN);
        int cep = CONNECT_get_CEP(encount_fd);

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
        int temp = 0;
        int p_cep = CHAR_getWorkInt(char_index, CHAR_ENCOUNT_FIX);
        if (p_cep != 0) {
          if (CHAR_getWorkInt(char_index, CHAR_ENCOUNT_NUM) < (int)time(NULL)) {
            CHAR_talkToCli(char_index, -1, "技能效用结束。", CHAR_COLORYELLOW);
            CHAR_setWorkInt(char_index, CHAR_ENCOUNT_FIX, 0);
            CHAR_setWorkInt(char_index, CHAR_ENCOUNT_NUM, 0);
          }
          temp = cep * (100 + p_cep) / 100;
        } else {
          temp = cep;
        }
#endif

        if (cep < minep)
          cep = minep;
        if (cep > maxep)
          cep = maxep;
        if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_NONE) {
          int entflag = 1;
          {
            int obj_index, index;
            OBJECT obj;
            for (obj = MAP_getTopObj(ff, fx, fy); obj; obj = NEXT_OBJECT(obj)) {
              obj_index = GET_OBJINDEX(obj);
              if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
                int etype;
                index = OBJECT_getIndex(obj_index);
                if (!CHAR_CHECKINDEX(index))
                  continue;
                if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPENPCENEMY &&
                    CHAR_getWorkInt(index, CHAR_WORKEVENTTYPE) ==
                        CHAR_EVENT_ENEMY) {
                  CHAR_setInt(char_index, CHAR_X, ox);
                  CHAR_setInt(char_index, CHAR_Y, oy);
                  /* 修复：撞到敌人被弹回时，CHAR 坐标回滚了，OBJECT 坐标和
                     olink 网格挂点也必须一起回滚，否则角色位置与网格脱节，
                     周围玩家看不到他（与走路不更新网格是同一类 bug）。 */
                  {
                    int rb_obj, rb_of, rb_ox, rb_oy;
                    rb_obj = CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX);
                    rb_of = OBJECT_setFloor(
                        rb_obj, CHAR_getInt(char_index, CHAR_FLOOR));
                    rb_ox =
                        OBJECT_setX(rb_obj, CHAR_getInt(char_index, CHAR_X));
                    rb_oy =
                        OBJECT_setY(rb_obj, CHAR_getInt(char_index, CHAR_Y));
                    if (!MAP_objmove(rb_obj, rb_of, rb_ox, rb_oy,
                                     CHAR_getInt(char_index, CHAR_FLOOR),
                                     CHAR_getInt(char_index, CHAR_X),
                                     CHAR_getInt(char_index, CHAR_Y))) {
                      print("%s:%d:[MAP_OBJMOVE_FAIL] walk-rollback "
                            "obj_index=%d\n",
                            __FILE__, __LINE__, rb_obj);
                    }
                  }
                  GmsvServer_XYD_send(getfdFromCharaIndex(char_index),
                                    CHAR_getInt(char_index, CHAR_X),
                                    CHAR_getInt(char_index, CHAR_Y),
                                    CHAR_getInt(char_index, CHAR_DIR));
                  break;
                }
                etype = CHAR_getWorkInt(index, CHAR_WORKEVENTTYPE);
                if (etype != CHAR_EVENT_NONE) {
                  if (etype == CHAR_EVENT_WARP)
                    entflag = 0;
                }
              }
            }
          }

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#ifdef _ENEMY_ACTION
          if (rand() % (120 * getEnemyAction()) < temp)
#else
          if (rand() % 120 < temp)
#endif
#else
#ifdef _ENEMY_ACTION
          if (rand() % (120 * getEnemyAction()) < cep)
#else
          if (rand() % 120 < cep) // Arminius 6.28 lower encounter prob.
#endif
#endif
          {
            if (entflag) {
#ifdef _Item_MoonAct
              if (getEqRandEnemy(encount_fd) > 0) {
                int Rnum = 0;
                int RandEnemy = getEqRandEnemy(encount_fd);
                Rnum = RAND(0, 100);
                if (Rnum > RandEnemy) {
#endif
                  // encounter!!
                  cep = minep;
                  GmsvServer_EN_recv(encount_fd, CHAR_getInt(char_index, CHAR_X),
                                   CHAR_getInt(char_index, CHAR_Y));
#ifdef _Item_MoonAct
                }
              } else {
                cep = minep;
                GmsvServer_EN_recv(encount_fd, CHAR_getInt(char_index, CHAR_X),
                                 CHAR_getInt(char_index, CHAR_Y));
              }
#endif
            }
          } else {
            if (cep < maxep)
              cep++;
          }
        }
        CONNECT_set_CEP(encount_fd, cep);
      }
    }
    if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
      CHAR_setFlg(char_index, CHAR_ISWARP, 0);
    }
  } else if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET ||
             CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEBUS
#ifdef _GAMBLE_ROULETTE
             || CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_GAMBLEROULETTE
#endif
#ifdef _PETRACE
             || CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_PETRACEPET
#endif
  ) {
    CHAR_setWorkInt(char_index, CHAR_WORKACTION, -1);
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_NONE) {
      CHAR_sendCharaAtWalk(char_index, of, ox, oy, CHAR_getDX(dir),
                           CHAR_getDY(dir));
    }
    {
      int opt[2] = {ox, oy};
      CHAR_sendWatchEvent(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                          CHAR_ACTWALK, opt, 2, TRUE);
    }
  }
  if (retvalue == CHAR_WALKSUCCESSED) {
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_NONE) {
      CHAR_sendCDCharaAtWalk(char_index, of, ox, oy, CHAR_getDX(dir),
                             CHAR_getDY(dir));
    }
  }
  return retvalue;
}

/*------------------------------------------------------------
 * キャラが歩く  (角色行走)［
 * 引数  (参数)
 *  index  int  Charでのindex  (角色索引)
 *  dir    int  方向  (方向)
 *  mode   int  0なら歩く［1なら方向転換  (0为行走/1为转向)［
 * 戻り値  (返回值)
 *  CHAR_WALKRET
 ------------------------------------------------------------*/
CHAR_WALKRET CHAR_walk(int index, int dir, int mode) {
  CHAR_WALKRET ret;
  BOOL (*prewalk)(int, int *, int *);
  void (*postwalk)(int);
  if (!CHAR_CHECKINDEX(index))
    return CHAR_WALKSYSTEMERROR;
  if (CHAR_getFlg(index, CHAR_ISDIE))
    return CHAR_WALKDIE;

  VALIDATEDIR(dir);
  prewalk = (BOOL(*)(int, int *, int *))CHAR_getFunctionPointer(
      index, CHAR_WALKPREFUNC);

  if (prewalk != NULL)
    if (prewalk(index, &dir, &mode) == FALSE) {
      return CHAR_WALKPREWALK;
    }

  if (mode == 0)
    ret = CHAR_walk_move(index, dir);
  else
    ret = CHAR_walk_turn(index, dir);
  postwalk = (void (*)(int))CHAR_getFunctionPointer(index, CHAR_WALKPOSTFUNC);

  if (postwalk != NULL)
    postwalk(index);
  return ret;
}

/*------------------------------------------------------------
 * 歩かせる前準備  (行走前准备)［
 * 引数  (参数)
 *  index  int  キャラインデックス  (角色索引)
 * 戻り値  (返回值)
 *  なし  (无)
 ------------------------------------------------------------*/
void CHAR_walkcall(int index) {
  char tmp[128];
  int dir, mode;
  int ret;
  POINT start, end, end1;

  if (!CHAR_CHECKINDEX(index))
    return;
  strncpysafe(tmp, sizeof(tmp), CHAR_getWorkChar(index, CHAR_WORKWALKARRAY));

  CHAR_ctodirmode(tmp[0], &dir, &mode);
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
      end.x = CHAR_getInt(index, CHAR_X);
      end.y = CHAR_getInt(index, CHAR_Y);
    }
  }

  ret = CHAR_walk(index, dir, mode);

  if (mode == 0 && ret == CHAR_WALKSUCCESSED) {
    /* 自分が親なら仲間を歩かせる  (自己为队长则让队友移动) */
    if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
        int i;
        for (i = 1; i < getPartyNum(index); i++) {
          int toindex = CHAR_getWorkInt(index, i + CHAR_WORKPARTYINDEX1);
          if (CHAR_CHECKINDEX(toindex)) {
            int parent_dir;
            /* メンバーの位置と親の歩き前の位置から 向きを求める  (根据成员位置与队长移动前位置求方向) */
            /* 歩き  (行走) */
            start.x = CHAR_getInt(toindex, CHAR_X);
            start.y = CHAR_getInt(toindex, CHAR_Y);
            parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
            /* グラディウスオプション歩きを実現する為に  (为实现子机跟随走位)
             * 次の子は前の子の座標を追うようにする  (后一个跟着前一个的坐标) */
            end = start;
            if (parent_dir != -1) {
              CHAR_walk(toindex, parent_dir, 0);
#ifdef _PETFOLLOW_NEW_
              end1 = start;
              int flg = FALSE;
              int petnum;
              petnum = 0;
              for (; petnum < 5; petnum++)
                if (CHAR_getWorkInt(toindex, CHAR_WORKPETFOLLOW + petnum) !=
                    -1) {
                  int petindex =
                      CHAR_getWorkInt(toindex, CHAR_WORKPETFOLLOW + petnum);
                  if (CHAR_CHECKINDEX(petindex)) {
                    int parent_dir;
                    start.x = CHAR_getInt(petindex, CHAR_X);
                    start.y = CHAR_getInt(petindex, CHAR_Y);
                    if (!flg) {
                      flg = TRUE;
                      if (end1.x > start.x)
                        end1.x--;
                      else if (end1.x < start.x)
                        end1.x++;
                      if (end1.y > start.y)
                        end1.y--;
                      else if (end1.y < start.y)
                        end1.y++;
                    }
                    parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end1);
                    end1 = start;
                    if (parent_dir != -1) {
                      if (CHAR_walk(petindex, parent_dir, 0) ==
                          CHAR_WALKSUCCESSED) {
                        CHAR_setWorkInt(
                            petindex, CHAR_WORKPETFOLLOWCOUNT,
                            1 + CHAR_getWorkInt(petindex,
                                                CHAR_WORKPETFOLLOWCOUNT));
                        if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) >
                            1000) {
                          CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
                          CHAR_pickupFollowPet(toindex, petindex);
                        }
                      }
                    }
                  } else {
                    CHAR_setWorkInt(toindex, CHAR_WORKPETFOLLOW + petnum, -1);
                  }
                }
#else

              // Robin 0702 petFollow
              if (CHAR_getWorkInt(toindex, CHAR_WORKPETFOLLOW) != -1) {
                int petindex = CHAR_getWorkInt(toindex, CHAR_WORKPETFOLLOW);
                if (CHAR_CHECKINDEX(petindex)) {
                  int parent_dir;
                  /* メンバーの位置と親の歩き前の位置から 向きを求める  (根据成员位置与队长移动前位置求方向) */
                  /* 歩き  (行走) */
                  end.x = CHAR_getInt(toindex, CHAR_X);
                  end.y = CHAR_getInt(toindex, CHAR_Y);
                  start.x = CHAR_getInt(petindex, CHAR_X);
                  start.y = CHAR_getInt(petindex, CHAR_Y);

                  if (end.x > start.x)
                    end.x--;
                  else if (end.x < start.x)
                    end.x++;
                  if (end.y > start.y)
                    end.y--;
                  else if (end.y < start.y)
                    end.y++;

                  parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
                  if (parent_dir != -1) {
                    if (CHAR_walk(petindex, parent_dir, 0) ==
                        CHAR_WALKSUCCESSED) {
                      CHAR_setWorkInt(
                          petindex, CHAR_WORKPETFOLLOWCOUNT,
                          1 + CHAR_getWorkInt(petindex,
                                              CHAR_WORKPETFOLLOWCOUNT));
                      if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) >
                          1000) {
                        CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
                        CHAR_pickupFollowPet(toindex, petindex);
                      }
                    }
                  }
                } else {
                  CHAR_setWorkInt(toindex, CHAR_WORKPETFOLLOW, -1);
                }
              }
#endif
            }
          } else {
            /* 初期化しなおす  (重新初始化) */
            CHAR_setWorkInt(index, i + CHAR_WORKPARTYINDEX1, -1);
          }
        }
      }
#ifdef _PETFOLLOW_NEW_
      int petnum, petindex;
      int flg = FALSE;
      end.x = CHAR_getInt(index, CHAR_X);
      end.y = CHAR_getInt(index, CHAR_Y);
      for (petnum = 0; petnum < 5; petnum++) {
        petindex = CHAR_getWorkInt(index, CHAR_WORKPETFOLLOW + petnum);
        if (CHAR_CHECKINDEX(petindex)) {
          int parent_dir;
          start.x = CHAR_getInt(petindex, CHAR_X);
          start.y = CHAR_getInt(petindex, CHAR_Y);
          if (!flg) {
            flg = TRUE;
            if (end.x > start.x)
              end.x--;
            else if (end.x < start.x)
              end.x++;
            if (end.y > start.y)
              end.y--;
            else if (end.y < start.y)
              end.y++;
          }
          parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
          end = start;
          if (parent_dir != -1) {
            if (CHAR_walk(petindex, parent_dir, 0) == CHAR_WALKSUCCESSED) {
              CHAR_setWorkInt(
                  petindex, CHAR_WORKPETFOLLOWCOUNT,
                  1 + CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT));
#ifdef _SYSTEM_SPEAD
              if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) > 200)
#else
              if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) > 1000)
#endif
              {
                CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
                CHAR_pickupFollowPet(index, petindex);
              }
            }
          }
        } else {
          CHAR_setWorkInt(index, CHAR_WORKPETFOLLOW + petnum, -1);
        }
      }
#else
      if (CHAR_getWorkInt(index, CHAR_WORKPETFOLLOW) != -1) {
        int petindex = CHAR_getWorkInt(index, CHAR_WORKPETFOLLOW);
        if (CHAR_CHECKINDEX(petindex)) {
          int parent_dir;
          /* メンバーの位置と親の歩き前の位置から 向きを求める  (根据成员位置与队长移动前位置求方向) */
          /* 歩き  (行走) */
          end.x = CHAR_getInt(index, CHAR_X);
          end.y = CHAR_getInt(index, CHAR_Y);
          start.x = CHAR_getInt(petindex, CHAR_X);
          start.y = CHAR_getInt(petindex, CHAR_Y);
          if (end.x > start.x)
            end.x--;
          else if (end.x < start.x)
            end.x++;
          if (end.y > start.y)
            end.y--;
          else if (end.y < start.y)
            end.y++;
          parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
          if (parent_dir != -1) {
            if (CHAR_walk(petindex, parent_dir, 0) == CHAR_WALKSUCCESSED)
            {
              CHAR_setWorkInt(
                  petindex, CHAR_WORKPETFOLLOWCOUNT,
                  1 + CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT));
#ifdef _SYSTEM_SPEAD
              if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) > 200)
#else
              if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT) > 1000)
#endif
              {
                /*
                CHAR_setInt( petindex, CHAR_VARIABLEAI,
                        200 + CHAR_getInt( petindex, CHAR_VARIABLEAI) );
                */
                CHAR_setWorkInt(petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
                CHAR_pickupFollowPet(index, petindex);
              }
            }
          }
        } else {
          /* 初期化しなおす  (重新初始化) */
          CHAR_setWorkInt(index, CHAR_WORKPETFOLLOW, -1);
        }
      }
#endif
    }
  }

  /* 99/03/09 By Kawata */
  /* walkの結果｝WALKARRAYがクリアされている場所があるので  (walk结果中WALKARRAY可能已被清空)
   * 取り出してコピーする  (取出并复制) */
  strncpysafe(tmp, sizeof(tmp), CHAR_getWorkChar(index, CHAR_WORKWALKARRAY));
  if (strlen(tmp) > 0) {
    CHAR_setWorkChar(index, CHAR_WORKWALKARRAY, &tmp[1]);
  }
  /*  時間を進める  (推进时间)    */
  CHAR_setWorkInt(index, CHAR_WORKWALKSTARTSEC, NowTime.tv_sec);
  CHAR_setWorkInt(index, CHAR_WORKWALKSTARTMSEC, NowTime.tv_usec);
}

/*------------------------------------------------------------
 * 歩き始めを登録する  (登记行走起点)［
 * 引数  (参数)
 *  index int   キャラでのインデックス  (角色索引)
 *  dir   char* 方向  (方向)
 *  id    int   ID
 * 戻り値  (返回值)
 *  なし  (无)
 ------------------------------------------------------------*/
void CHAR_walk_start(int index, int x, int y, char *dir, BOOL mapsendmode) {

  if (!CHAR_CHECKINDEX(index))
    return;

  // Nuke 08/27: Stop wap
  /*
  {
     int ox, oy;
     const int     seesiz = MAP_CHAR_DEFAULTSEESIZ;
     ox = CHAR_getInt(index,CHAR_X);
     oy = CHAR_getInt(index,CHAR_Y);
     if ( ABS(x - ox) > seesiz || ABS(y - oy) > seesiz ) {
        CHAR_talkToCli(index, -1, "因座标错误而断线。", CHAR_COLORYELLOW);
        CONNECT_setCloseRequest_debug(getfdFromCharaIndex(index) , 1);
        return;
     }
  }
  */

  if (CHAR_getFlg(index, CHAR_ISDIE)) return;
  CHAR_setWorkChar(index, CHAR_WORKWALKARRAY, dir);
  CHAR_setWorkInt(index, CHAR_WORKWALKSTARTSEC, NowTime.tv_sec);
  CHAR_setWorkInt(index, CHAR_WORKWALKSTARTMSEC, NowTime.tv_usec);

  CHAR_setMyPosition(index, x, y, FALSE);
  if (mapsendmode) {
    int ox, oy;
    char buf[64];
    int len;
    int i;
    strncpysafe(buf, sizeof(buf), CHAR_getWorkChar(index, CHAR_WORKWALKARRAY));

    len = strlen(buf);
    ox = CHAR_getInt(index, CHAR_X);
    oy = CHAR_getInt(index, CHAR_Y);

    for (i = 0; i < len; i++) {
      int dir, mode;
      int fx, fy;
      CHAR_ctodirmode(buf[i], &dir, &mode);
      if (mode == 0) {
        CHAR_getCoordinationDir(dir, ox, oy, 1, &fx, &fy);
        ox = fx;
        oy = fy;
      }
    }
#ifdef _MO_MAP_AUTO_UPDATE
    CHAR_sendMapAtWalk(index, CHAR_getInt(index, CHAR_FLOOR),
                       CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
                       ox, oy);

#endif
  }

  if (strlen(dir) >= 1)
    CHAR_walkcall(index);
}
/*------------------------------------------------------------
 * 歩きの初期化  (行走初始化)
 * 引数  (参数)
 *  index  int     キャラでのインデックス  (角色索引)
 *  dir    char*   方向  (方向)
 *  id     int     ID
 * 戻り値  (返回值)
 *  なし  (无)
 ------------------------------------------------------------*/
void CHAR_walk_init(int fd, int x, int y, char *direction, BOOL mapsendmode) {
  if (strlen(direction) > 32) {
    print("walkarray length over\n");
    return;
  }
  if (CHAR_getWorkInt(CONNECT_getCharaindex(fd), CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    return;
  }
  if (CHAR_getWorkInt(CONNECT_getCharaindex(fd), CHAR_WORKPARTYMODE) !=
      CHAR_PARTY_CLIENT) {
    CHAR_walk_start(CONNECT_getCharaindex(fd), x, y, direction, mapsendmode);
  } else {
    int dir, mode;
    CHAR_ctodirmode(direction[0], &dir, &mode);
    if (mode == 0) {
      print("玩家并非领队也非单人玩家。\n");
    } else {
      CHAR_walk_start(CONNECT_getCharaindex(fd), x, y, direction, FALSE);
    }
  }
}

void CHAR_sendCharaAtWalk(int char_index, int of, int ox, int oy, int xflg,
                          int yflg) {
  int fd = -1;
  int i, j, fl, x, y;
  char introduction[512];
  char c_msg[1024 * 64];
  int strpos = 0;
  int seesiz = CHAR_DEFAULTSEESIZ / 2;
  char cabuf[128];
  char myintroduction[1024 * 4];
  int whichtype;

  whichtype = CHAR_getInt(char_index, CHAR_WHICHTYPE);
  if (whichtype == CHAR_TYPEPLAYER) {
    fd = getfdFromCharaIndex(char_index);
    if (fd == -1)
      return;
  }
  if (xflg > 1 || xflg < -1) {
    print("%s:%d: arg err[xflg]\n", __FILE__, __LINE__);
    return;
  }
  if (yflg > 1 || yflg < -1) {
    print("%s:%d: arg err[yflg]\n", __FILE__, __LINE__);
    return;
  }
  fl = CHAR_getInt(char_index, CHAR_FLOOR);
  x = CHAR_getInt(char_index, CHAR_X);
  y = CHAR_getInt(char_index, CHAR_Y);

  if (of != fl || ABS(x - ox) > seesiz / 2 || ABS(y - oy) > seesiz / 2)
    return;

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }
  c_msg[0] = '\0';
#ifndef _NPC_EVENT_NOTICE
  if (!CHAR_makeObjectCString(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX),
                              myintroduction, sizeof(myintroduction))) {
    myintroduction[0] = '\0';
  }
#endif
  if (xflg != 0) {
    i = x + seesiz * xflg;
    for (j = y - seesiz; j <= y + seesiz; j++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int introlen;
        int obj_index = GET_OBJINDEX(object);
        int c_index = OBJECT_getIndex(obj_index);

        if (OBJECT_getType(obj_index) == OBJTYPE_NOUSE)
          continue;
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA &&
            !CHAR_getFlg(OBJECT_getIndex(obj_index), CHAR_ISVISIBLE)) {
          continue;
        }
        if (strlen(myintroduction) != 0) {
          if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              int tofd = getfdFromCharaIndex(c_index);
              if (tofd != -1) {
                CDsend(tofd);
#ifdef _NPC_EVENT_NOTICE
                if (!CHAR_makeObjectCStringNew(
                        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), c_index,
                        myintroduction, sizeof(myintroduction))) {
                  myintroduction[0] = '\0';
                }
#endif
                GmsvServer_C_send(tofd, myintroduction);
              }
            }
          }
        }
        if (whichtype == CHAR_TYPEPLAYER) {
          /* Make C*/
#ifdef _NPC_EVENT_NOTICE
          if (CHAR_makeObjectCStringNew(obj_index, char_index, introduction,
                                        sizeof(introduction))) {
#else
          if (CHAR_makeObjectCString(obj_index, introduction,
                                     sizeof(introduction))) {
#endif
            introlen = strlen(introduction);
            introduction[introlen] = ',';
            introduction[introlen + 1] = '\0';
            strncpysafe(&c_msg[strpos], sizeof(c_msg) - strpos, introduction);
            strpos += strlen(introduction);
          }
          if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_LEADER) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                        CHAR_ACTLEADER, 1)) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
            if ((CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) &&
                CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_LEADER &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) ==
                    BATTLE_CHARMODE_NONE) {
              int tofd = getfdFromCharaIndex(c_index);
              if (tofd != -1) {
                if (CHAR_makeCAOPT1String(
                        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), cabuf,
                        sizeof(cabuf), CHAR_ACTLEADER, 1)) {
                  CONNECT_appendCAbuf(tofd, cabuf, strlen(cabuf));
                }
              }
            }
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) !=
                    BATTLE_CHARMODE_NONE) {
              if (CHAR_getWorkInt(c_index, CHAR_WORKBATTLEWATCH) == TRUE) {
                if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                          CHAR_ACTBATTLEWATCH, 1)) {
                  CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
                }
              } else {
                int battleno = CHAR_getWorkInt(c_index, CHAR_WORKBATTLEINDEX);
                int sideno = CHAR_getWorkInt(c_index, CHAR_WORKBATTLESIDE);
                int helpno;
                if (!BATTLE_CHECKINDEX(battleno)) {
                  //
                } else {
                  helpno = (BattleArray[battleno].Side[sideno].flg &
                            BSIDE_FLG_HELP_OK)
                               ? TRUE
                               : FALSE;
                  /* HelpNo = 1 反移 */
                  if (CHAR_makeCAOPT3String(obj_index, cabuf, sizeof(cabuf),
                                            CHAR_ACTBATTLE, battleno, sideno,
                                            helpno)) {
                    CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
                  }
                }
              }
            }
            if ((CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
                 CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
                CHAR_getWorkInt(c_index, CHAR_WORKACTION) != -1) {
              if (CHAR_makeCADefaultString(
                      obj_index, cabuf, sizeof(cabuf),
                      CHAR_getWorkInt(c_index, CHAR_WORKACTION))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#ifdef _ADD_ACTION
            else if (CHAR_getWorkInt(c_index, CHAR_WORKACTION) > 0) {
              if (CHAR_makeCADefaultString(
                      obj_index, cabuf, sizeof(cabuf),
                      CHAR_getWorkInt(c_index, CHAR_WORKACTION))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _STREET_VENDOR
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKSTREETVENDOR) == 1 &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) ==
                    BATTLE_CHARMODE_NONE) {
              if (CHAR_makeCAOPTString(
                      obj_index, cabuf, sizeof(cabuf), CHAR_STREETVENDOR_OPEN,
                      CHAR_getWorkChar(c_index, CHAR_STREETVENDOR_NAME))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef _ANGEL_SUMMON
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKANGELMODE) == TRUE) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                        CHAR_ACTANGEL, 1)) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _PLAYER_EFFECT
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_PLAYER_EFFECT) > 0) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), CHAR_MIND,
                      CHAR_getWorkInt(c_index, CHAR_PLAYER_EFFECT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef FAMILY_MANOR_
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(OBJECT_getIndex(obj_index),
                                CHAR_PLAYER_EFFECT_MANOR) > 0) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), 44,
                      CHAR_getWorkInt(OBJECT_getIndex(obj_index),
                                      CHAR_PLAYER_EFFECT_MANOR))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _TRUMP_EQUIPMENT
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_TRUMP_EFFECT) > 0) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf), 46,
                                        CHAR_getInt(OBJECT_getIndex(obj_index),
                                                    CHAR_TRUMP_EFFECT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _NEW_UPDATETITLE
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_TITLE_DEFAULT) >
                    0) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf), 45,
                                        CHAR_getInt(OBJECT_getIndex(obj_index),
                                                    CHAR_TITLE_DEFAULT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef _FAMILYBADGE_
            extern int getFamilyBadge(int index);
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                getFamilyBadge(OBJECT_getIndex(obj_index))) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), CHAR_FAMILYBADGE,
                      getFamilyBadge(OBJECT_getIndex(obj_index)))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
          }
        }
      }
    }
  }
  if (yflg != 0) {
    j = y + seesiz * yflg;
    for (i = x - seesiz; i <= x + seesiz; i++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int introlen;
        int obj_index = GET_OBJINDEX(object);
        int c_index = OBJECT_getIndex(obj_index);
        if (OBJECT_getType(obj_index) == OBJTYPE_NOUSE)
          continue;
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA &&
            !CHAR_getFlg(OBJECT_getIndex(obj_index), CHAR_ISVISIBLE)) {
          if (!CHAR_CHECKINDEX(OBJECT_getIndex(obj_index))) {
            printf("自动删除一个问题对象！");
            endObjectOne(obj_index);
          }
          continue;
        }
        if (strlen(myintroduction) != 0) {
          if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              int tofd = getfdFromCharaIndex(c_index);
              if (tofd != -1) {
                CDsend(tofd);
                GmsvServer_C_send(tofd, myintroduction);
              }
            }
          }
        }
        if (whichtype == CHAR_TYPEPLAYER) {
          /* Make C*/
#ifdef _NPC_EVENT_NOTICE
          if (CHAR_makeObjectCStringNew(obj_index, char_index, introduction,
                                        sizeof(introduction))) {
#else
          if (CHAR_makeObjectCString(obj_index, introduction,
                                     sizeof(introduction))) {
#endif
            introlen = strlen(introduction);
            introduction[introlen] = ',';
            introduction[introlen + 1] = '\0';
            strncpysafe(&c_msg[strpos], sizeof(c_msg) - strpos, introduction);
            strpos += strlen(introduction);
          }
          if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_LEADER) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                        CHAR_ACTLEADER, 1)) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
            if ((CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) &&
                CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_LEADER &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) ==
                    BATTLE_CHARMODE_NONE) {
              int tofd = getfdFromCharaIndex(c_index);
              if (tofd != -1) {
                if (CHAR_makeCAOPT1String(
                        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), cabuf,
                        sizeof(cabuf), CHAR_ACTLEADER, 1)) {
                  CONNECT_appendCAbuf(tofd, cabuf, strlen(cabuf));
                }
              }
            }
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) !=
                    BATTLE_CHARMODE_NONE) {
              if (CHAR_getWorkInt(c_index, CHAR_WORKBATTLEWATCH) == TRUE) {
                if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                          CHAR_ACTBATTLEWATCH, 1)) {
                  CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
                }
              } else {
                int battleno = CHAR_getWorkInt(c_index, CHAR_WORKBATTLEINDEX);
                int sideno = CHAR_getWorkInt(c_index, CHAR_WORKBATTLESIDE);
                int helpno =
                    (BattleArray[CHAR_getWorkInt(c_index, CHAR_WORKBATTLEINDEX)]
                         .Side[CHAR_getWorkInt(c_index, CHAR_WORKBATTLESIDE)]
                         .flg &
                     BSIDE_FLG_HELP_OK)
                        ? TRUE
                        : FALSE;

                if (CHAR_makeCAOPT3String(obj_index, cabuf, sizeof(cabuf),
                                          CHAR_ACTBATTLE, battleno, sideno,
                                          helpno)) {
                  CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
                }
              }
            }
            if ((CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
                 CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
                CHAR_getWorkInt(c_index, CHAR_WORKACTION) != -1) {
              if (CHAR_makeCADefaultString(
                      obj_index, cabuf, sizeof(cabuf),
                      CHAR_getWorkInt(c_index, CHAR_WORKACTION))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#ifdef _ADD_ACTION
            else if (CHAR_getWorkInt(c_index, CHAR_WORKACTION) > 0) {
              if (CHAR_makeCADefaultString(
                      obj_index, cabuf, sizeof(cabuf),
                      CHAR_getWorkInt(c_index, CHAR_WORKACTION))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _STREET_VENDOR
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKSTREETVENDOR) == 1 &&
                CHAR_getWorkInt(c_index, CHAR_WORKBATTLEMODE) ==
                    BATTLE_CHARMODE_NONE) {
              if (CHAR_makeCAOPTString(
                      obj_index, cabuf, sizeof(cabuf), CHAR_STREETVENDOR_OPEN,
                      CHAR_getWorkChar(c_index, CHAR_STREETVENDOR_NAME))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef _ANGEL_SUMMON
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_WORKANGELMODE) == TRUE) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf),
                                        CHAR_ACTANGEL, 1)) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef _PLAYER_EFFECT
            if (CHAR_getInt(c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(c_index, CHAR_PLAYER_EFFECT) > 0) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), CHAR_MIND,
                      CHAR_getWorkInt(c_index, CHAR_PLAYER_EFFECT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef FAMILY_MANOR_
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getWorkInt(OBJECT_getIndex(obj_index),
                                CHAR_PLAYER_EFFECT_MANOR) > 0) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), 44,
                      CHAR_getWorkInt(OBJECT_getIndex(obj_index),
                                      CHAR_PLAYER_EFFECT_MANOR))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _TRUMP_EQUIPMENT
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_TRUMP_EFFECT) > 0) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf), 46,
                                        CHAR_getInt(OBJECT_getIndex(obj_index),
                                                    CHAR_TRUMP_EFFECT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
#ifdef _NEW_UPDATETITLE
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_TITLE_DEFAULT) >
                    0) {
              if (CHAR_makeCAOPT1String(obj_index, cabuf, sizeof(cabuf), 45,
                                        CHAR_getInt(OBJECT_getIndex(obj_index),
                                                    CHAR_TITLE_DEFAULT))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif

#ifdef _FAMILYBADGE_
            extern int getFamilyBadge(int index);
            if (CHAR_getInt(OBJECT_getIndex(obj_index), CHAR_WHICHTYPE) ==
                    CHAR_TYPEPLAYER &&
                getFamilyBadge(OBJECT_getIndex(obj_index))) {
              if (CHAR_makeCAOPT1String(
                      obj_index, cabuf, sizeof(cabuf), CHAR_FAMILYBADGE,
                      getFamilyBadge(OBJECT_getIndex(obj_index)))) {
                CONNECT_appendCAbuf(fd, cabuf, strlen(cabuf));
              }
            }
#endif
          }
        }
      }
    }
  }
  dchop(c_msg, ",");
  if (strlen(c_msg) != 0) {
    CDflush(char_index);
    GmsvServer_C_send(fd, c_msg);
  }
}

static void CHAR_sendCDCharaAtWalk(int char_index, int of, int ox, int oy,
                                   int xflg, int yflg) {
  int i, j, fl, x, y;
  int seesiz = CHAR_DEFAULTSEESIZ / 2;
  int fd, tofd;
  if (xflg > 1 || xflg < -1) {
    print("%s:%d: arg err[xflg]\n", __FILE__, __LINE__);
    return;
  }
  if (yflg > 1 || yflg < -1) {
    print("%s:%d: arg err[yflg]\n", __FILE__, __LINE__);
    return;
  }
  fl = CHAR_getInt(char_index, CHAR_FLOOR);
  x = CHAR_getInt(char_index, CHAR_X);
  y = CHAR_getInt(char_index, CHAR_Y);

  if (of != fl ||                 /*  フロアが違う  (楼层不同)    */
      ABS(x - ox) > seesiz / 2 || /*  差が大きい  (差值过大)  */
      ABS(y - oy) > seesiz / 2)   /*  差が大きい  (差值过大)  */
    return;

  fd = getfdFromCharaIndex(char_index);
  if (xflg != 0) {
    i = x + (seesiz * xflg + xflg) * -1;
    for (j = y - seesiz + (yflg * -1); j <= y + seesiz + (yflg * -1); j++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int obj_index = GET_OBJINDEX(object);
        char buf[64];
        if (OBJECT_getType(obj_index) == OBJTYPE_NOUSE)
          continue;
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA &&
            !CHAR_getFlg(OBJECT_getIndex(obj_index), CHAR_ISVISIBLE)) {
          if (!CHAR_CHECKINDEX(OBJECT_getIndex(obj_index))) {
            printf("自动删除一个问题对象！");
            endObjectOne(obj_index);
          }
          continue;
        }
        if (fd != -1) {
          cnv10to62(obj_index, buf, sizeof(buf));
          CONNECT_appendCDbuf(fd, buf, strlen(buf));
        }
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
          int tochar_index = OBJECT_getIndex(obj_index);
          if (CHAR_getInt(tochar_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
            if (CHAR_getWorkInt(tochar_index, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) {
              tofd = getfdFromCharaIndex(tochar_index);
              if (tofd != -1) {
                cnv10to62(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), buf,
                          sizeof(buf));
                CONNECT_appendCDbuf(tofd, buf, strlen(buf));
                CAsend(tofd);
                CDsend(tofd);
              }
            }
          }
        }
      }
    }
  }
  if (yflg != 0) {
    j = y + (seesiz * yflg + yflg) * -1;
    for (i = x - seesiz + (xflg * -1); i <= x + seesiz + (xflg * -1); i++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int obj_index = GET_OBJINDEX(object);
        char buf[64];
        if (OBJECT_getType(obj_index) == OBJTYPE_NOUSE)
          continue;
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA &&
            !CHAR_getFlg(OBJECT_getIndex(obj_index), CHAR_ISVISIBLE)) {
          continue;
        }
        if (fd != -1) {
          cnv10to62(obj_index, buf, sizeof(buf));
          CONNECT_appendCDbuf(fd, buf, strlen(buf));
        }
        if (OBJECT_getType(obj_index) == OBJTYPE_CHARA) {
          int tochar_index = OBJECT_getIndex(obj_index);
          if (CHAR_getInt(tochar_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
            if (CHAR_getWorkInt(tochar_index, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) {
              tofd = getfdFromCharaIndex(tochar_index);
              if (tofd != -1) {
                cnv10to62(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), buf,
                          sizeof(buf));
                CONNECT_appendCDbuf(tofd, buf, strlen(buf));
                CAsend(tofd);
                CDsend(tofd);
              }
            }
          }
        }
      }
    }
  }
  if (fd != -1) {
    CDsend(fd);
  }
}

void CHAR_ctodirmode(char moji, int *dir, int *mode) {
  *mode = 1;
  if ('a' <= moji && moji <= 'h')
    *mode = 0;

  *dir = tolower(moji) - 'a';
}

