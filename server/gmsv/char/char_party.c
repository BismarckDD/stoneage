#include "version.h"
//
#include "gmsv_server.h"
//
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "family.h"       // shan
#include "npc_airplane.h" // Arminius 7.10 Airplane
#include "npc_bus.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"

#ifdef _ITEM_QUITPARTY
#include "init.h"
#endif
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];
#ifdef _NEW_ITEM_

extern int CheckCharMaxItem(int charindex);
#endif

int CHAR_getEmptyPartyArray(int char_index) {
  int i = -1;
  int rc = FALSE;
  int toindex;
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    toindex = char_index;
  } else {
    toindex = CHAR_getPartyIndex(char_index, 0);
  }
  if (CHAR_CHECKINDEX(toindex)) {
    for (i = 1; i < getPartyNum(toindex); i++) {
      if (CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1) == -1) {
        rc = TRUE;
        break;
      }
    }
  }
  return (rc ? i : -1);
}
/*------------------------------------------------------------
 *   端卞由□  奴卞  月质
 *
 *  char_index		int		愤坌
 *  targetindex		int		  月锹澎及谛
 ------------------------------------------------------------*/
BOOL CHAR_JoinParty_Main(int char_index, int targetindex) {
  int firstflg = FALSE;
  int i;
  char c[3];
  char buf[64];
  int toindex;
  int parray;

  if (CHAR_getWorkInt(targetindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    toindex = targetindex;
  } else {
    toindex = CHAR_getPartyIndex(targetindex, 0);
  }
  if (!CHAR_CHECKINDEX(toindex)) {
    return FALSE;
  }

  parray = CHAR_getEmptyPartyArray(toindex);
  if (parray == -1) {
    return FALSE;
  }
  if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 1);
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, 1);
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1, toindex);
    firstflg = TRUE;
  }
  CHAR_setWorkInt(toindex, parray + CHAR_WORKPARTYINDEX1, char_index);

  CHAR_setWorkChar(char_index, CHAR_WORKWALKARRAY, "");

  CHAR_setWorkInt(char_index, CHAR_WORKPARTYMODE, CHAR_PARTY_CLIENT);

  CHAR_setWorkInt(char_index, CHAR_WORKPARTYINDEX1, toindex);

  if (firstflg) {
    CHAR_sendStatusString(toindex, "N0");
  }

  for (i = 0; i < getPartyNum(toindex); i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      snprintf(c, sizeof(c), "N%d", i);
      CHAR_sendStatusString(char_index, c);
    }
  }

  snprintf(buf, sizeof(buf), "%s 加入团队！",
           CHAR_getChar(char_index, CHAR_NAME));

  for (i = 0; i < getPartyNum(toindex); i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      if (index != char_index) {
        snprintf(c, sizeof(c), "N%d", parray);
        CHAR_sendStatusString(index, c);
        CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
      } else {
        CHAR_talkToCli(index, -1, "加入团队！", CHAR_COLORYELLOW);
      }
      CHAR_complianceParameter(index);
    }
  }
  return TRUE;
}

BOOL CHAR_JoinParty_Main_New(int char_index, int targetindex, int flg) {
  int firstflg = FALSE;
  int i;
  char c[3];
  char buf[64];
  int toindex;
  int parray;

  /* 褪互中凶日娄匀舰曰请允 */
  if (CHAR_getWorkInt(targetindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    toindex = targetindex;
  } else {
    toindex = CHAR_getPartyIndex(targetindex, 0);
  }
  if (!CHAR_CHECKINDEX(toindex)) {
    //	print( " %s:%d err\n", __FILE__, __LINE__);
    return FALSE;
  }

  parray = CHAR_getEmptyPartyArray(toindex);
  if (parray == -1) {
    return FALSE;
  }
  if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 1);
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, 1);
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1, toindex);
    firstflg = TRUE;
  }
  CHAR_setWorkInt(toindex, parray + CHAR_WORKPARTYINDEX1, char_index);

  CHAR_setWorkChar(char_index, CHAR_WORKWALKARRAY, "");

  CHAR_setWorkInt(char_index, CHAR_WORKPARTYMODE, CHAR_PARTY_CLIENT);

  CHAR_setWorkInt(char_index, CHAR_WORKPARTYINDEX1, toindex);

  if (firstflg) {
    CHAR_sendStatusString(toindex, "N0");
  }

  for (i = 0; i < getPartyNum(toindex); i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      snprintf(c, sizeof(c), "N%d", i);
      CHAR_sendStatusString(char_index, c);
    }
  }
  if (flg)
    snprintf(buf, sizeof(buf), "%s 加入团队！",
             CHAR_getChar(char_index, CHAR_NAME));

  for (i = 0; i < getPartyNum(toindex); i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      if (index != char_index) {
        snprintf(c, sizeof(c), "N%d", parray);
        CHAR_sendStatusString(index, c);
        if (flg)
          CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
      } else if (flg)
        CHAR_talkToCli(index, -1, "加入团队！", CHAR_COLORYELLOW);
      CHAR_complianceParameter(index);
    }
  }
  return TRUE;
}

/*------------------------------------------------------------
 * 由□  奴卞  欠丹午允月［
 ------------------------------------------------------------*/
BOOL CHAR_JoinParty(int char_index) {

  int result = -1;
  int x, y;
  OBJECT object;
  int found = FALSE;
  int fd;
  int cnt;
  int i;

  fd = getfdFromCharaIndex(char_index);
  if (fd == -1) {
    print("%s : %d err\n", __FILE__, __LINE__);
    return FALSE;
  }

  /* 愤坌互由□  奴赚氏匹凶日蛲   */
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    GmsvServer_PR_send(fd, 1, FALSE);
    return FALSE;
  }

  /*   及蟆及甄  毛  月 */
  CHAR_getCoordinationDir(CHAR_getInt(char_index, CHAR_DIR),
                          CHAR_getInt(char_index, CHAR_X),
                          CHAR_getInt(char_index, CHAR_Y), 1, &x, &y);

  /* 赓渝祭允月 */
  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setJoinpartychar_index(fd, i, -1);
  }
  cnt = 0;

  /*愤坌及  及蟆及平乓仿毛潸  允月 */

  for (object = MAP_getTopObj(CHAR_getInt(char_index, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int parray;
    int objindex = GET_OBJINDEX(object);
    int targetindex = -1;

    /* 平乓仿弁正□元扎卅中 */
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);

    // shan begin
    if (CHAR_getInt(char_index, CHAR_FMINDEX) > 0 &&
        CHAR_getInt(toindex, CHAR_FMINDEX) > 0) {
      for (i = 0; i < FAMILY_FMPKFLOOR; i++) {
        if (fmpkflnum[i].fl == CHAR_getInt(char_index, CHAR_FLOOR))
          if (CHAR_getInt(char_index, CHAR_FMINDEX) !=
              CHAR_getInt(toindex, CHAR_FMINDEX)) {
            GmsvServer_PR_send(fd, 1, FALSE);
            return FALSE;
          }
      }
    }
    // shan end

    /* 皿伊奶乩□及凛 */
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        || CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif
    ) {
      found = TRUE;
      /* 锹澎互阂分匀凶日褪毛娄匀舰曰请允 */
      if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
        targetindex = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
        if (!CHAR_CHECKINDEX(targetindex)) {
          print(" %s:%d err\n", __FILE__, __LINE__);
          continue;
        }
        if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
          continue;
        }
      } else {
        targetindex = toindex;
      }

      /*   褪午  ㄠ汹动  卞中月井 */
      if (NPC_Util_CharDistance(char_index, targetindex) > 1) {
        continue;
      }

      /* 爵    反匹卅中仪［*/
      if (CHAR_getWorkInt(targetindex, CHAR_WORKBATTLEMODE) !=
          BATTLE_CHARMODE_NONE) {
        continue;
      }
      /* 醮棉袱第乒□玉井 */
      if (!CHAR_getFlg(targetindex, CHAR_ISPARTY))
        continue;

#ifdef _NO_JOIN_FLOOR
      {
        int i;
        int floor = CHAR_getInt(char_index, CHAR_FLOOR);
        for (i = 0; i < 32; i++) {
          if (floor == getNoJoinFloor(i)) {
            break;
          }
        }
        if (i < 32) {
          continue;
        }
      }
#endif

#ifdef _ANGEL_SUMMON
      if (CHAR_getWorkInt(targetindex, CHAR_WORKANGELMODE) == TRUE) {
        CHAR_talkToCli(char_index, -1, "使者不可以当领队。", CHAR_COLORYELLOW);
        continue;
      }
#endif
    }
    /* 穴件乒旦田旦互中月凛反］谛棉方曰穸燮允月［ */
    else if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      targetindex = toindex;
      cnt = 0;
      if (!NPC_BusCheckJoinParty(toindex, char_index, TRUE)) {
        /* 椭瘀毛  凶今卅井匀凶［醮棉  月及反蔽歹月［谛棉及质  手仄卅中［
         * 支支仇仄中及匹［
         */
        break;
      }
      { // Arminius 7.10 Airplane
        int busimg = CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER);
        if ((busimg != 100355) && (busimg != 100461)) {
#ifdef _SHIP_MATEMO
          if (busimg == 100348) {
            int metamo =
                (CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER) - 100000) /
                20;
            if (metamo < 0)
              metamo = 0;
            else if (metamo > 11)
              metamo = 11;
            CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER, 102062 + metamo);
          } else
#endif
            CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER, busimg);
          CHAR_setInt(char_index, CHAR_RIDEPET, -1);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
          // Robin debug 01/11/21
          if (CHAR_getInt(char_index, CHAR_RIDEPET) != -1) {
            CHAR_setInt(char_index, CHAR_RIDEPET, -1);
            CHAR_send_P_StatusString(char_index, CHAR_P_STRING_RIDEPET);
          }
          /*
              CHAR_sendPMEToArroundCharacterFLXY(char_index,
                  CHAR_getInt( char_index, CHAR_FLOOR),
                  CHAR_getInt( char_index, CHAR_X),
                  CHAR_getInt( char_index, CHAR_Y),
                  0,1,CHAR_getInt( char_index, CHAR_PETMAILEFFECT)
                  );
          */
        }
      }
    }
    /* 皿伊奶乩□坭反穴件乒旦田旦动陆反  骰允月 */
    else {
      continue;
    }
    /* 锹澎由□  奴及谛醒反    井＂ */
    parray = CHAR_getEmptyPartyArray(targetindex);
    if (parray == -1)
      continue;

    /* 仇仇引匹仁木壬     */
    CONNECT_setJoinpartychar_index(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;

    /* 穴件乒旦田旦  苇仄分中］伙□皿毛  仃月［ */
    if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS)
      break;
  }

  if (cnt == 0) {
    if (found == TRUE) {
      CHAR_talkToCli(char_index, -1, "无法加入团队。", CHAR_COLORYELLOW);
    }
    result = FALSE;
  } else if (cnt == 1) {
    {
      CHAR_JoinParty_Main(char_index, CONNECT_getJoinpartychar_index(fd, 0));
      result = TRUE;
    }
  } else {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n和谁组成团队呢？\n");
    strlength = strlen(msgbuf);
    for (i = 0; CONNECT_getJoinpartychar_index(fd, i) != -1 &&
                i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getJoinpartychar_index(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("%s:%d视窗讯息buffer不足。\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_SELECTPARTY, -1,
                       makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  if (result != -1) {
    GmsvServer_PR_send(fd, 1, result);
  }

  return result;
}

static BOOL CHAR_DischargePartySub(int char_index, int msgflg) {
  char buf[64], c[3];
  int toindex, flg, i;
#ifdef _ITEM_QUITPARTY
  int j = 0, k;
#endif

  if (!CHAR_CHECKINDEX(char_index))
    return FALSE;

  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int pindex, airplaneflag = 0;
    // Arminius 7.10 Airplane
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      if ((CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) != 100355) &&
          (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) != 100461)) {
        airplaneflag = 1;
      }
    }
    for (i = 0; i < getPartyNum(char_index); i++) {
      pindex = CHAR_getWorkInt(char_index, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex)) {
        CHAR_setWorkInt(char_index, i + CHAR_WORKPARTYINDEX1, -1);
#ifdef _PLAYER_NPC
        if (CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
            CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPELUANPC) {
          CHAR_CharaDeleteHavePet(pindex);
          CHAR_CharaDelete(pindex);
          continue;
        }
#endif
        CHAR_setWorkInt(pindex, CHAR_WORKPARTYINDEX1, -1);
        CHAR_setWorkInt(pindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
        if (msgflg) {
          CHAR_talkToCli(pindex, -1, "团队已解散！", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
          // won fix
          for (j = 0; j < CheckCharMaxItem(pindex); j++) {
            int del_item_index = CHAR_getItemIndex(pindex, j);
            if (ITEM_CHECKINDEX(del_item_index)) { // 格子内有道具
              for (k = 0; k < itemquitparty_num; k++) {
                if (ITEM_getInt(del_item_index, ITEM_ID) ==
                    atoi(Disappear_Item[k].string)) { // 若等於所设定的道具ID
                  CHAR_setItemIndex(pindex, j, -1); // 格子内道具消失
                  ITEM_endExistItemsOne(del_item_index);
                  CHAR_sendItemDataOne(pindex, j);
                }
              }
            }
          }
#endif
        }
        int fd = getfdFromCharaIndex(pindex);
        if (fd != -1) {
          GmsvServer_PR_send(fd, 0, 1);
        }
        // Arminius 7.10 Airplane
        if (airplaneflag &&
            (CHAR_getInt(pindex, CHAR_WHICHTYPE) != CHAR_TYPEBUS)) {
          int bi, bbi, ii, category;
          bbi = CHAR_getInt(pindex, CHAR_BASEBASEIMAGENUMBER);
          ii = CHAR_getItemIndex(pindex, CHAR_ARM);
          if (!ITEM_CHECKINDEX(ii))
            category = ITEM_FIST;
          else
            category = ITEM_getInt(ii, ITEM_TYPE);
          bi = CHAR_getNewImagenumberFromEquip(pindex, bbi, category);
          if (bi == -1)
            bi = bbi;
          CHAR_setInt(pindex, CHAR_BASEIMAGENUMBER, bi);
          // Robin 0810 debug
          CHAR_complianceParameter(pindex);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX));
        }
      }
    }
    CHAR_sendLeader(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), 0);
  } else if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_CLIENT) {
    int myarray = -1;
    int fd = getfdFromCharaIndex(char_index);
    CHAR_setWorkInt(char_index, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
    toindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    if (!CHAR_CHECKINDEX(toindex))
      return FALSE;
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      NPC_BusCheckAllowItem(toindex, char_index, TRUE);
      // Arminius 7.9 Airplane
      if ((CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100355) &&
          (CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100461)) {
        int bi, bbi, ii, category;

        bbi = CHAR_getInt(char_index, CHAR_BASEBASEIMAGENUMBER);
        ii = CHAR_getItemIndex(char_index, CHAR_ARM);
        if (!ITEM_CHECKINDEX(ii))
          category = ITEM_FIST;
        else
          category = ITEM_getInt(ii, ITEM_TYPE);
        bi = CHAR_getNewImagenumberFromEquip(char_index, bbi, category);
        if (bi == -1)
          bi = bbi;
        CHAR_setInt(char_index, CHAR_BASEIMAGENUMBER, bi);
        CHAR_setInt(char_index, CHAR_RIDEPET, -1);
        // Robin 0810 debug
        CHAR_complianceParameter(char_index);

        CHAR_sendCToArroundCharacter(
            CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
        if (CHAR_getWorkInt(toindex, CHAR_NPCWORKINT5) == 1) {
          if (CHAR_getInt(char_index, CHAR_LASTTALKELDER) >= 0) {
            int fl, x, y;
            CHAR_getElderPosition(CHAR_getInt(char_index, CHAR_LASTTALKELDER),
                                  &fl, &x, &y);
            CHAR_warpToSpecificPoint(char_index, fl, x, y);
          }
        }
      }
    }
    CHAR_setWorkInt(char_index, CHAR_WORKPARTYINDEX1, -1);
    for (i = 0; i < getPartyNum(toindex); i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        if (index == char_index) {
          myarray = i;
          break;
        }
      }
    }
    if (myarray == getPartyNum(toindex)) {
      print("DischargeParty(): 真奇怪！");
      return FALSE;
    }
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1 + myarray, -1);
    snprintf(buf, sizeof(buf), "%s 脱离团队！",
             CHAR_getChar(char_index, CHAR_NAME));
    if (msgflg) {
      CHAR_talkToCli(char_index, -1, "脱离团队！", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
      // won fix
      for (i = 0; i < CheckCharMaxItem(char_index); i++) {
        int del_item_index = CHAR_getItemIndex(char_index, j);
        if (ITEM_CHECKINDEX(del_item_index)) { // 格子内有道具
          for (j = 0; j < itemquitparty_num; j++) {
            if (ITEM_getInt(del_item_index, ITEM_ID) ==
                atoi(Disappear_Item[j].string)) { // 若等於所设定的道具ID
              CHAR_setItemIndex(char_index, i, -1); // 格子内道具消失
              ITEM_endExistItemsOne(del_item_index);
              CHAR_sendItemDataOne(char_index, i);
            }
          }
        }
      }
#endif
    }
    snprintf(c, sizeof(c), "N%d", myarray);
    if (fd != -1) {
      GmsvServer_PR_send(fd, 0, 1);
    }
    for (i = 0; i < getPartyNum(toindex); i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
#ifdef _ITEM_QUITPARTY
        // won fix
        for (j = 0; j < CheckCharMaxItem(index); j++) {
          int del_item_index = CHAR_getItemIndex(index, j);
          if (ITEM_CHECKINDEX(del_item_index)) { // 格子内有道具
            for (k = 0; k < itemquitparty_num; k++) {
              if (ITEM_getInt(del_item_index, ITEM_ID) ==
                  atoi(Disappear_Item[k].string)) { // 若等於所设定的道具ID
                CHAR_setItemIndex(index, j, -1); // 格子内道具消失
                ITEM_endExistItemsOne(del_item_index);
                CHAR_sendItemDataOne(index, j);
              }
            }
          }
        }
#endif
        if (msgflg) {
          CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
        }
        CHAR_sendStatusString(index, c);
        CHAR_complianceParameter(index);
      }
    }
    flg = FALSE;
    for (i = 1; i < getPartyNum(toindex); i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        flg = TRUE;
        break;
      }
    }
    if (!flg) {
      CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
      CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 0);
    } else {

      POINT start, end;
      int previndex = toindex;
      end.x = CHAR_getInt(char_index, CHAR_X);
      end.y = CHAR_getInt(char_index, CHAR_Y);
      for (i = 1; i < getPartyNum(toindex); i++) {
        int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(index)) {
          if (NPC_Util_CharDistance(index, previndex) > 1) {
            int parent_dir;
            start.x = CHAR_getInt(index, CHAR_X);
            start.y = CHAR_getInt(index, CHAR_Y);
            parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
            end = start;
            if (parent_dir != -1) {
              CHAR_walk(index, parent_dir, 0);
            }
          }
          previndex = index;
        }
      }
    }
  }

  return TRUE;
}
BOOL CHAR_DischargeParty(int char_index, int flg) {
  return CHAR_DischargePartySub(char_index, 1);
}

BOOL CHAR_DischargeParty_New(int char_index, int flg) {
  return CHAR_DischargePartySub(char_index, flg);
}

BOOL CHAR_DischargePartyNoMsg(int char_index) {
  return CHAR_DischargePartySub(char_index, 0);
}

/*------------------------------------------------------------
 * 愤坌互伉□母□井升丹井毛霜耨允月［
 ------------------------------------------------------------*/
void CHAR_sendLeader(int objindex, int leader) {
  int opt[1];
  opt[0] = leader;
  CHAR_sendWatchEvent(objindex, CHAR_ACTLEADER, opt, 1, TRUE);
}

int CHAR_getPartyIndex(int index, int num) {
  int nindex = -1;
  if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    nindex = CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1 + num);
  }
  else {
    int oyaindex = CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex)) {
      nindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + num);
    }
  }
  return nindex;
}

void CHAR_talkToCliAndParty(int talkedchar_index, int talkchar_index,
                            char *message, CHAR_COLOR color) {
  int i;
  CHAR_talkToCli(talkedchar_index, talkchar_index, message, color);
  for (i = 0; i < getPartyNum(talkedchar_index); i++) {
    int index = CHAR_getPartyIndex(talkedchar_index, i);
    if (CHAR_CHECKINDEX(index) && index != talkedchar_index) {
      CHAR_talkToCli(index, talkchar_index, message, color);
    }
  }
}

int getPartyNum(int char_index) { return CHAR_PARTYMAX; }
