#include "version.h"
#include "addressbook.h"
#include "battle.h"
#include "battle_command.h"
#include "char.h"
#include "config_file.h"
#include "event.h"
#include "family.h" // CoolFish: Family 2001/5/24
#include "handletime.h"
#include "item_event.h" // shan: blackmarket
#include "item_gen.h"
#include "log.h" //add this because the second had it
#include "lssproto_serv.h"
#include "magic.h"
#include "map_deal.h" // CoolFish: 2001/4/18
#include "net.h"
#include "object.h"
#include "pet.h"
#include "pet_skill.h"
#include "petmail.h"
#include "readmap.h"
#include "trade.h" // CoolFish: Trade 2001/4/18
#include "util.h"
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "npc_healer.h"
#ifdef _PROFESSION_SKILL
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL
#include "chatroom.h"
#endif
#ifdef _ONLINE_SHOP
#include "longzoro/newshop.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
extern int player_online;
BOOL checkStringErr(char *);

extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];

static int Callfromcli_Util_getTargetCharaindex(int fd, int toindex) {
  int to_charaindex = -1;
  const int char_index = CONNECT_getCharaindex(fd);
  // 如何解释to_index是个什么东东?
  if (toindex == 0) {
    to_charaindex = char_index;
  } else if (toindex > 0 && toindex < 6) {
    to_charaindex = CHAR_getCharPet(char_index, toindex - 1);
    if (!CHAR_CHECKINDEX(to_charaindex)) {
      to_charaindex = -1;
    }
  } else if (toindex > 5 && toindex < 11) {
    to_charaindex = CHAR_getPartyIndex(char_index, toindex - 6);
  }
  return to_charaindex;
}

void lssproto_ClientLogin_recv(int fd, char *cdkey, char *passwd, char *mac,
                               int servid, char *Newip) {
  if (CONNECT_getState(fd) == NULLCONNECT) {
    CONNECT_setState(fd, NOTLOGIN);
  }
  // ttom avoid the restore 2001/01/09
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    print("\n the Client had  Logined fd=%d", fd);
    return;
  }
  if (checkStringErr(cdkey))
    return;
  if (checkStringErr(passwd))
    return;
  CONNECT_setCdkey(fd, cdkey);
  CONNECT_setPasswd(fd, passwd);
  CONNECT_setCtype(fd, CLI);
#ifdef _NEWCLISETSERVID
  CONNECT_setServid(fd, servid);
#endif
#ifdef _NEWCLISETMAC
  if (checkStringErr(mac)) {
    strcpy(mac, "");
  }
  CONNECT_setMAC(fd, mac);
#endif
  unsigned long tmpip;
  int a, b, c, d;
  char ip[32];
  tmpip = CONNECT_get_userip(fd);
  a = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  b = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  c = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  d = (tmpip % 0x100);
  sprintf(ip, "%d.%d.%d.%d", a, b, c, d);
#ifdef _NO_FULLPLAYER_ATT
  {
    int res;

    if (strlen(cdkey) == 0 || strlen(passwd) == 0 || strlen(ip) == 0) {
      print("��½��Ϣ�д���\n");
      lssproto_ClientLogin_send(fd, "no");
      CONNECT_endOne_debug(fd);
      return;
    }

    res = sasql_query(cdkey, passwd);
    if (res == 3) {
      if (getNoCdkeyPlayer() > 0 && fd - player_online >= getNoCdkeyPlayer() &&
          getNoCdkeyMode() != 0) {
        if (strcmp(ip, getNoAttIp(0)) != 0 && strcmp(ip, getNoAttIp(1)) != 0 &&
            strcmp(ip, getNoAttIp(2)) != 0 && strcmp(ip, getNoAttIp(3)) != 0 &&
            strcmp(ip, getNoAttIp(4)) != 0) {
          if (getNoCdkeyType() == 0) {
            // if(sasql_query_online_ip(ip)==0 &&
            // sasql_check_lockip(ip,getFengType())<1){
            if (sasql_query_online_ip(ip) == 0) {
              print("����1��\n");
              CONNECT_endOne_debug(fd);
              return;
            }
          } else {
            print("����2��\n");
            CONNECT_endOne_debug(fd);
            return;
          }
        }
      }
    } else if (res != 1) {
      if (getNoCdkeyPlayer() > 0 && fd - player_online >= getNoCdkeyPlayer() &&
          getNoCdkeyMode() == 2) {
        if (strcmp(ip, getNoAttIp(0)) != 0 && strcmp(ip, getNoAttIp(1)) != 0 &&
            strcmp(ip, getNoAttIp(2)) != 0 && strcmp(ip, getNoAttIp(3)) != 0 &&
            strcmp(ip, getNoAttIp(4)) != 0) {
          if (getNoCdkeyType() == 0) {
            // if(sasql_query_online_ip(ip)==0 &&
            // sasql_check_lockip(ip,getFengType())<1){
            if (sasql_query_online_ip(ip) == 0) {
              // if(sasql_add_lockip(ip,getFengType(),getServernumber(),2)==1){
              print("����3��\n");
              CONNECT_endOne_debug(fd);
              return;
              //}
            }
          } else {
            print("����4��\n");
            CONNECT_endOne_debug(fd);
            return;
          }
        }
      }
      CONNECT_endOne_debug(fd);
      return;
    }
  }
#endif
#ifdef _NEWCLISETMAC
  saacproto_ACCharLogin_send(acfd, fd, cdkey, passwd, ip, mac);
#else
  saacproto_ACCharLogin_send(acfd, fd, cdkey, passwd, ip);
#endif
}

void lssproto_CreateNewChar_recv(int fd, int dataplacenum, char *charname,
                                 int imgno, int faceimgno, int vital, int str,
                                 int tgh, int dex, int earth, int water,
                                 int fire, int wind, int hometown) {
  char cdkey[CDKEYLEN];
  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    // lssproto_CreateNewChar_send(fd, FAILED, "Not NOTLOGIN State\n");
    lssproto_CreateNewChar_send(fd, FAILED, "当前用户没有登录.\n");
    return;
  }
  if (strlen(charname) == 0) {
    // lssproto_CreateNewChar_send(fd, FAILED, "0 length name.\n");
    lssproto_CreateNewChar_send(fd, FAILED, "用户名称长度不能位0.\n");
    return;
  } else if (strlen(charname) >= 32) {
    // lssproto_CreateNewChar_send(fd, FAILED, "Too long charname.\n");
    lssproto_CreateNewChar_send(fd, FAILED, "用户名称长度大于16个字符.\n");
    return;
    // Nuke start 0711: Avoid naming as WAEI
  } else if (strstr(charname, "����") || strstr(charname, "gm") ||
             strstr(charname, "GM") || strstr(charname, "Gm") ||
             strstr(charname, "gM") || strstr(charname, "管理员") ||
             strstr(charname, "�ǣ�") || strstr(charname, "�ǣ�") ||
             strstr(charname, "���")
#ifdef _UNREG_NEMA
             || (strstr(charname, getUnregname(0)) &&
                 strlen(getUnregname(0)) > 0) ||
             (strstr(charname, getUnregname(1)) &&
              strlen(getUnregname(1)) > 0) ||
             (strstr(charname, getUnregname(2)) &&
              strlen(getUnregname(2)) > 0) ||
             (strstr(charname, getUnregname(3)) &&
              strlen(getUnregname(3)) > 0) ||
             (strstr(charname, getUnregname(4)) && strlen(getUnregname(4)) > 0)
#endif
             || strstr(charname, "��������") || strstr(charname, "|") ||
             strstr(charname, "'") || strstr(charname, "=") ||
             strstr(charname, ";")
             // WON END
  ) {
    lssproto_CreateNewChar_send(fd, FAILED, "Ivalid Char Name.\n");
    return;
    /*
    unsigned int ip=CONNECT_get_userip(fd);
    int a, b, c, d, ck;
    a=(ip % 0x100); ip=ip / 0x100;
    b=(ip % 0x100); ip=ip / 0x100;
    c=(ip % 0x100); ip=ip / 0x100;
    d=(ip % 0x100);

    ck= (
        ( (a== 10) && (b==0)   && (c==0) ) ||
        ( (a==211) && (b==76) && (c==176) && (d==21) ) ||  // ̨��wayi
        ( (a==210) && (b==64)  && (c==97)  && ((d>=21)&&(d<=25)) ) ||
        ( (a==61)  && (b==222) && (c==142) && (d==66)) ||
        ( (a==172) && (b==16)  && (c==172)  && (d==29) )
      );

    print(" name_WAEI_IP:%d.%d.%d.%d ck:%d ",a,b,c,d,ck );
    if( !ck ) {
      lssproto_CreateNewChar_send(fd,FAILED, "Invalid charname\n");
      return;
    }
    */
  }
#ifdef _MO_ILLEGAL_NAME
  int j;
  for (j = 0; j < 32; j++) {
    if (strstr(charname, getIllegalName(j)) && strlen(getIllegalName(j)) > 0) {
      // lssproto_CreateNewChar_send(fd,FAILED, "Invalid charname\n");
      lssproto_CreateNewChar_send(fd, FAILED,
                                  "角色名称命中非法角色名称名单.\n");
      return;
    }
  }
#endif
  {
    // Nuke start 0801,0916: Avoid strange name
    int i, ach;
    for (i = 0, ach = 0; i < strlen(charname); i++) {
      if ((unsigned char)charname[i] == 0xff) {
        ach = 1;
        break;
      } // Force no 0xff
      if (((unsigned char)charname[i] >= 0x7f) &&
          ((unsigned char)charname[i] <= 0xa0)) {
        ach = 1;
        break;
      } // Force no 0x7f~0xa0
      if ((unsigned char)charname[i] <= 0x20) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if (ach) {
        if ((((unsigned char)charname[i] >= 0x40) &&
             ((unsigned char)charname[i] <= 0x7e)) ||
            (((unsigned char)charname[i] >= 0xa1) &&
             ((unsigned char)charname[i] <= 0xfe)))
          ach = 0;
      } else {
        if (((unsigned char)charname[i] >= 0xa1) &&
            ((unsigned char)charname[i] <= 0xfe))
          ach = 1;
      }
    }
    if (ach) {
      lssproto_CreateNewChar_send(fd, FAILED, "角色名称中存在奇异字符.\n");
      return;
    }
  }
  // Nuke end

  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CHAR_createNewChar(fd, dataplacenum, charname, imgno, faceimgno, vital, str,
                     tgh, dex, earth, water, fire, wind, hometown, cdkey);
}

void lssproto_CharLogin_recv(int fd, char *charname) {
  char cdkey[CDKEYLEN], passwd[PASSWDLEN];

  if (CONNECT_isCLI(fd) == FALSE)
    return;

  print("\n���Ե�½: ��������=%s\n", charname);

  if (charname[0] == '\0') {
    lssproto_CharLogin_send(fd, FAILED, "Can't access char have no name\n");
    return;
  }
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharLogin_send(fd, FAILED, "Already Logged in\n");
    return;
  }
  CONNECT_setCharname(fd, charname);
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(fd, passwd, sizeof(passwd));

  saacproto_ACCharLoad_send(acfd, cdkey, passwd, charname, 1, "",
                            CONNECT_getFdid(fd));
  CONNECT_setState(fd, WHILELOGIN);
}
#ifdef _NEW_ITEM_
extern int CheckCharMaxItem(int charindex);
#endif
#ifdef _ITEM_CHECKDROPATLOGOUT
BOOL CheckDropatLogout(int charaindex) {
  int i;
  for (i = 0; i < CheckCharMaxItem(charaindex); i++) {
    int itemindex;
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_CHECKINDEX(itemindex) == FALSE)
      continue;
    if (ITEM_getInt(itemindex, ITEM_DROPATLOGOUT) == TRUE) {
      return TRUE;
    }
  }
  return FALSE;
}
#endif

void lssproto_CharLogout_recv(int fd, int flg) {
  char cdkey[CDKEYLEN], charname[CHARNAMELEN];
  int charaindex = CONNECT_getCharaindex(fd);
  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isLOGIN(fd) == FALSE) {
    lssproto_CharLogout_send(fd, FAILED, "Not Logged in\n");

    return;
  }
#ifdef _OFFLINE_SYSTEM
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) > 0) {
    CHAR_setWorkInt(charaindex, CHAR_WORKFD, -1);
  } else
#endif
  {

    int fl, x, y;
    // CoolFish: 2001/10/18
    if (!CHAR_CHECKINDEX(charaindex))
      return;

    if (CHAR_getInt(charaindex, CHAR_LASTTALKELDER) >= 0) {
      if (CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
                                &fl, &x, &y) == FALSE) {
        CHAR_talkToCli(charaindex, -1, "����ǰ�ļ�¼����������¼�¼��",
                       CHAR_COLORYELLOW);
        return;
      }
#ifdef _NO_TEAMWARP_SKYLAND
      if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE &&
          fl == 30691) {
        CHAR_talkToCli(charaindex, -1,
                       "�Ŷ����޷������֮����",
                       CHAR_COLORYELLOW);
        return;
      }
#endif

#ifdef _CHAR_NEWLOGOUT
      if (flg == 1) { // �ؼ�¼��
        if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
            BATTLE_CHARMODE_NONE) {
          CHAR_talkToCli(charaindex, -1,
                         "ս�����޷��ؼ�¼�㣡",
                         CHAR_COLORYELLOW);
          return;
        }
#ifdef _ITEM_CHECKWARES
        if (CHAR_CheckInItemForWares(charaindex, 0) == FALSE) {
          CHAR_talkToCli(charaindex, -1,
                         "Я�������޷�ʹ�á�",
                         CHAR_COLORYELLOW);
          return;
        }
#endif
        if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
            CHAR_PARTY_CLIENT) {
          CHAR_talkToCli(charaindex, -1,
                         "�Ŷ����޷��ؼ�¼�㣡",
                         CHAR_COLORYELLOW);
          return;
        }
#ifdef _ITEM_CHECKDROPATLOGOUT
        if (CheckDropatLogout(charaindex)) {
          CHAR_talkToCli(
              charaindex, -1,
              "Я������Ʒʹ���޷��ؼ�¼�㣡",
              CHAR_COLORYELLOW);
          return;
        }
#endif
#ifdef _AUTO_PK
        if (CHAR_getInt(charaindex, CHAR_FLOOR) == 20000 &&
            CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE &&
            CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) != -1) {
          char buf[64];
          int i, num = 0, winindex = -1;
          int playernum = CHAR_getPlayerMaxNum();
          sprintf(buf, "��С�� %s �˳���ǰ������",
                  CHAR_getChar(charaindex, CHAR_NAME));
          AutoPk_PKSystemTalk(buf, buf);
          if (AutoPk_PKTimeGet() <= 0) {
            for (i = 0; i < playernum; i++) {
              if (CHAR_CHECKINDEX(i) == FALSE ||
                  CHAR_getWorkInt(i, CHAR_WORK_AUTOPK) == -1)
                continue;
              if (CHAR_getInt(i, CHAR_FLOOR) == 20000) {
                if (CHAR_getWorkInt(i, CHAR_WORK_AUTOPK) != -1) {
                  if (i != charaindex)
                    winindex = i;
                  num++;
                }
              }
            }
            if (num == 2) {
              int fl = 0, x = 0, y = 0;
              CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER),
                                    &fl, &x, &y);
              CHAR_warpToSpecificPoint(winindex, fl, x, y);
              AutoPk_ChampionShipSet(
                  winindex, CHAR_getWorkInt(winindex, CHAR_WORK_AUTOPK), 1);
              AutoPk_ChampionShipSet(
                  charaindex, CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK), 2);
              AutoPk_GetChampionShip();
#ifdef _FORMULATE_AUTO_PK
              //              saacproto_FormulateAutoPk_send(acfd,
              //              CHAR_getChar(winindex, CHAR_CDKEY),
              //              CHAR_getWorkInt(winindex,CHAR_WORK_AUTOPK));
              CHAR_setInt(winindex, CHAR_AMPOINT,
                          CHAR_getInt(winindex, CHAR_AMPOINT) +
                              CHAR_getWorkInt(winindex, CHAR_WORK_AUTOPK));
              CHAR_setWorkInt(winindex, CHAR_WORK_AUTOPK, 0);
#endif
            }
          }
        }
#endif
        if (CHAR_getInt(charaindex, CHAR_FLOOR) != 117 &&
            CHAR_getInt(charaindex, CHAR_FLOOR) != 887
#ifdef _ADD_DUNGEON // ׷�ӵ���
            && CHAR_getInt(charaindex, CHAR_FLOOR) != 8513
#endif
        ) {

          CHAR_warpToSpecificPoint(charaindex, fl, x, y);

          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_LEADER) {
            int i;
            for (i = 1; i < getPartyNum(charaindex); i++) {
              int pindex =
                  CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(pindex)) {
                CHAR_warpToSpecificPoint(pindex, fl, x, y);
              }
            }
          }
        }
        return;
      } else {
        CONNECT_endOne_debug(fd);
      }
#else
      if (CHAR_getInt(charaindex, CHAR_FLOOR) == 117) {
        CHAR_setInt(charaindex, CHAR_X, 225);
        CHAR_setInt(charaindex, CHAR_Y, 13);
      } else {
        CHAR_setInt(charaindex, CHAR_FLOOR, fl);
        CHAR_setInt(charaindex, CHAR_X, x);
        CHAR_setInt(charaindex, CHAR_Y, y);
      }
#endif
    }
  }
}

void lssproto_CharDelete_recv(int fd, char *charname, char *passwd) {
  char cdkey[CDKEYLEN];
  int fdid;

  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharDelete_send(fd, FAILED, "Already Logged in\n");
    return;
  }
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));

  fdid = CONNECT_getFdid(fd);
  saacproto_ACCharDelete_send(acfd, cdkey, passwd, charname, "", fdid);
  {
    char buff[512];
    char escapebuf[1024];
    snprintf(buff, sizeof(buff), "%s_%s", cdkey, charname);
    makeEscapeString(buff, escapebuf, sizeof(escapebuf));
    saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0);
    saacproto_DBDeleteEntryString_send(acfd, DB_ADDRESSBOOK, escapebuf, fdid,
                                       0);
  }
  saacproto_Broadcast_send(acfd, cdkey, charname, "chardelete", 0);

  CONNECT_setState(fd, WHILECHARDELETE);
}

void lssproto_NewCharDelete_recv(int fd, char *charname, char *passwd) {
  char cdkey[CDKEYLEN];
  int fdid;

  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharDelete_send(fd, FAILED, "Already Logged in\n");
    return;
  }
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  fdid = CONNECT_getFdid(fd);
#ifdef _ALLBLUES_LUA_1_9
  if (FreeCharDelet(fd, cdkey, passwd) == 0) {
    return;
  }
#endif
  saacproto_ACCharDelete_send(acfd, cdkey, passwd, charname, "", fdid);

  {
    char buff[512];
    char escapebuf[1024];
    snprintf(buff, sizeof(buff), "%s_%s", cdkey, charname);
    makeEscapeString(buff, escapebuf, sizeof(escapebuf));
    saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0);
    saacproto_DBDeleteEntryString_send(acfd, DB_ADDRESSBOOK, escapebuf, fdid,
                                       0);
  }
  saacproto_Broadcast_send(acfd, cdkey, charname, "chardelete", 0);

  CONNECT_setState(fd, WHILECHARDELETE);
}

void lssproto_CharList_recv(int fd) {
  char cdkey[CDKEYLEN], passwd[PASSWDLEN];
  int fdid = -1;
  int charlistflg = 0;

  if (CONNECT_isCLI(fd) == FALSE)
    return;

  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharList_send(fd, FAILED, "�벻Ҫ���зǷ���½��Ϸ��");
    return;
  }

  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(fd, passwd, sizeof(passwd));
#ifdef _MO_LOGIN_NO_KICK
  if (getLoginNoKick() == 1) {
    if (!sasql_CheckPasswd(cdkey, passwd)) {
      lssproto_CharList_send(fd, FAILED, "�˺��������");
      return;
    }
  }
#endif
  fdid = CONNECT_getFdid(fd);
  {
    int i;
    int playernum = CHAR_getPlayerMaxNum();
    for (i = 0; i < playernum; i++) {
      if (!CHAR_CHECKINDEX(i))
        continue;
      if (!strcmp(CHAR_getChar(i, CHAR_CDKEY), cdkey)) {
#ifdef _OFFLINE_SYSTEM
        if (CHAR_getWorkInt(i, CHAR_WORK_OFFLINE) != 0) {
          if (!CHAR_logout(i, TRUE)) {
            print("err %s:%d\n", __FILE__, __LINE__);
          }
          lssproto_CharList_send(fd, FAILED, "�����˺��������ߣ������µ�¼��Ϸ��");
          CONNECT_setCloseRequest(getfdFromCharaIndex(i), 1);
          break;
        } else
#endif
        {
          CHAR_talkToCli(i, -1, "���ظ���½������!", CHAR_COLORYELLOW);
          int oldfd = getfdFromCharaIndex(i);
          CONNECT_endOne_debug(oldfd);
          charlistflg = 1;
          break;
        }
      }
    }
  }
#ifdef _NEWCLISETMAC
  char mac[64];
  CONNECT_getMAC(fd, mac, sizeof(mac));
#endif

  unsigned long tmpip;
  int a, b, c, d;
  char ip[32];
  tmpip = CONNECT_get_userip(fd);
  a = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  b = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  c = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  d = (tmpip % 0x100);
  sprintf(ip, "%d.%d.%d.%d", a, b, c, d);

  saacproto_ACCharList_send(acfd, cdkey, passwd, ip, mac, fdid, charlistflg);

  CONNECT_setState(fd, WHILEDOWNLOADCHARLIST);
}

void lssproto_Echo_recv(int fd, char *arg0) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  lssproto_Echo_send(fd, arg0);
}

#define CHECKFD                                                                \
  if (CONNECT_isCLI(fd) == FALSE)                                              \
    return;                                                                    \
  if (CONNECT_isLOGIN(fd) == FALSE)                                            \
    return;
#define CHECKFDANDTIME                                                         \
  if (CONNECT_isCLI(fd) == FALSE)                                              \
    return;                                                                    \
  if (CONNECT_isLOGIN(fd) == FALSE)                                            \
    return;

void lssproto_W_recv(int fd, int x, int y, char *direction) {
  // ttom +3
  int char_index, ix, iy;
  char_index = CONNECT_getCharaindex(fd);

  ix = CHAR_getInt(char_index, CHAR_X);
  iy = CHAR_getInt(char_index, CHAR_Y);

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    return;
  }
  // nuke 0407
  if (checkNu(fd) < 0) {
    print(" NU-Err ");
    CHAR_talkToCli(char_index, -1, "Ѷ�Ŵ���", CHAR_COLORYELLOW);

#ifdef _NETLOG_
    char cdkey[16];
    char charname[32];
    CONNECT_getCharname(CHAR_getWorkInt(char_index, CHAR_WORKFD), charname,
                        32);
    CONNECT_getCdkey(CHAR_getWorkInt(char_index, CHAR_WORKFD), cdkey, 16);
    LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, "Ѷ�Ŵ���");
#endif

    CONNECT_setCloseRequest(fd, 1);
    return;
  }
  // ttom debug
  if ((x == 0) && (y == 0)) {
    // CHAR_talkToCli(char_index, -1, "�������������ߡ�", CHAR_COLORYELLOW);
    //  Roibn 03/14
    return;
  }
  // ttom avoid the warp at will 11/6
  {
    int i_diff_x, i_diff_y;
    i_diff_x = abs(ix - x);
    i_diff_y = abs(iy - y);
    // Robin 03/14
    if ((i_diff_x > 1) || (i_diff_y > 1)) {
      // Robin 0518
      // CHAR_talkToCli(char_index, -1, "����·�����������ߡ�",
      // CHAR_COLORYELLOW);

      // return;
      x = ix;
      y = iy;
    }
  }
  if (!(MAP_walkAble(char_index, CHAR_getInt(char_index, CHAR_FLOOR), x,
                     y))) {
    // Robin 03/14
    x = ix;
    y = iy;
  } else {
  }

  CHAR_walk_init(fd, x, y, direction, TRUE);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_W2_recv(int fd, int x, int y, char *direction) {
  // ttom +3
  int char_index, ix, iy, i_fl;
  // Char *chwk;// CoolFish: Rem 2001/4/18
  char_index = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(char_index))
    return;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  if (CHAR_getWorkInt(char_index, CHAR_WORK_X) == x &&
      CHAR_getWorkInt(char_index, CHAR_WORK_Y) == x) {
    if (strlen(direction) == 1) {
      if (direction[0] >= 'a' && direction[0] <= 'h') {
        if (CHAR_getWorkInt(char_index, CHAR_WORK_DIR) !=
            direction[0] - 'a') {
          CHAR_setWorkInt(char_index, CHAR_WORK_DIR, direction[0] - 'a');
          CHAR_setWorkInt(char_index, CHAR_WORK_W, 0);
        } else if (CHAR_getWorkInt(char_index, CHAR_WORK_W) > 30) {
          if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) !=
              CHAR_PARTY_CLIENT) {
            if (CONNECT_getBDTime(fd) < time(NULL)) {
              lssproto_EN_recv(fd, CHAR_getInt(char_index, CHAR_X),
                               CHAR_getInt(char_index, CHAR_Y));
              CONNECT_setBDTime(fd, (int)time(NULL) + 3);
            }
          }
          return;
        } else {
          CHAR_setWorkInt(char_index, CHAR_WORK_W,
                          CHAR_getWorkInt(char_index, CHAR_WORK_W) + 1);
        }
      }
    } else {
      CHAR_setWorkInt(char_index, CHAR_WORK_W, 0);
    }
  } else {
    CHAR_setWorkInt(char_index, CHAR_WORK_W, 0);
  }

  CHAR_setWorkInt(char_index, CHAR_WORK_X, x);
  CHAR_setWorkInt(char_index, CHAR_WORK_Y, x);

#ifdef _FIX_STW_SPEED_ENEMY
  if (strstr(direction, "gcgc") != NULL) {
    if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) !=
        CHAR_PARTY_CLIENT) {
      if (CONNECT_getBDTime(fd) < time(NULL)) {
        lssproto_EN_recv(fd, CHAR_getInt(char_index, CHAR_X),
                         CHAR_getInt(char_index, CHAR_Y));
        CONNECT_setBDTime(fd, (int)time(NULL) + 3);
      }
    }
    return;
  }
#else
#ifdef _NO_STW_ENEMY
  if (strstr(direction, "gcgc") != NULL) {
    switch (getNoSTWNenemy()) {
    case 0:
      break;
    case 1:
      return;
    case 2:
      if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) !=
          CHAR_PARTY_CLIENT) {
        if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
            BATTLE_CHARMODE_NONE) {
          return;
        }
        if (getNoSTWNenemyPoint() > 0) {
          if (CHAR_getInt(char_index, CHAR_GOLD) < getNoSTWNenemyPoint()) {
            char token[256];
            sprintf(token,
                    "������ʯ�Ҳ���%"
                    "d���޷�ԭ������",
                    getNoSTWNenemyPoint());
            CHAR_talkToCli(char_index, -1, token, CHAR_COLORYELLOW);
            return;
          }
        }
        if (BATTLE_CreateVsEnemy(char_index, 0, -1) !=
            BATTLE_ERR_CHARAINDEX) {
          if (getNoSTWNenemyPoint() > 0) {
            CHAR_setInt(char_index, CHAR_GOLD,
                        CHAR_getInt(char_index, CHAR_GOLD) -
                            getNoSTWNenemyPoint());
            CHAR_send_P_StatusString(char_index, CHAR_P_STRING_GOLD);
          }
        }
      }
      return;
    }
  }
#endif
#endif
  ix = CHAR_getInt(char_index, CHAR_X);
  iy = CHAR_getInt(char_index, CHAR_Y);
  i_fl = CHAR_getInt(char_index, CHAR_FLOOR);

  // ttom avoid the warp at will 11/6
  {
    int i_diff_x, i_diff_y;
    // ix=CHAR_getInt(char_index, CHAR_X);
    // iy=CHAR_getInt(char_index, CHAR_Y);
    // i_fl=CHAR_getInt(char_index, CHAR_FLOOR);
    i_diff_x = abs(ix - x);
    i_diff_y = abs(iy - y);
    if ((i_diff_x > 1) || (i_diff_y > 1)) { // 2
      x = ix;
      y = iy;
    }
  } // ttom
  if (!(MAP_walkAble(char_index, CHAR_getInt(char_index, CHAR_FLOOR), x,
                     y))) {
    x = ix;
    y = iy;
  }
  CHAR_walk_init(fd, x, y, direction, FALSE);
}

void lssproto_SKD_recv(int fd, int dir, int index) { CHECKFDANDTIME; }

void lssproto_ID_recv(int fd, int x, int y, int haveitemindex, int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int to_charaindex;
  int char_index;

  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  // ttom avoid the warp at will 12/5
  {
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);
  to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  CHAR_ItemUse(char_index, to_charaindex, haveitemindex);
}

/*------------------------------------------------------------
 * ��įë����
 ------------------------------------------------------------*/
void lssproto_ST_recv(int fd, int titleindex) {
  CHECKFDANDTIME;
  CHAR_selectTitle(CONNECT_getCharaindex(fd), titleindex);
}
/*------------------------------------------------------------
 * ��įë��������
 ------------------------------------------------------------*/
void lssproto_DT_recv(int fd, int titleindex) {
  CHECKFDANDTIME;
  CHAR_deleteTitle(CONNECT_getCharaindex(fd), titleindex);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_FT_recv(int fd, char *data) {
  CHECKFDANDTIME;

  // Robin 04/23 debug
  if (strlen(data) > 12)
    return;

  if (checkStringErr(data))
    return;

  CHAR_inputOwnTitle(CONNECT_getCharaindex(fd), data);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PI_recv(int fd, int x, int y, int dir) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<PI>--Error!!!!");
      // print("\n<PI>origion x=%d,y=%d",ix,iy);
      // print("\n<PI>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  } // ttom end

  CHAR_setMyPosition(char_index, x, y, TRUE);
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_PickUpItem(char_index, dir);
}

void lssproto_DI_recv(int fd, int x, int y, int itemindex) {
  int charaindex;
  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);

  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif

#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(charaindex, CHAR_LOCKED) == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "Ϊ��ȷ�������Ʒ��ȫ����������İ�ȫ������н�����\n");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

  CHAR_setMyPosition(charaindex, CHAR_getInt(charaindex, CHAR_X),
                     CHAR_getInt(charaindex, CHAR_Y), TRUE);

  CHAR_DropItem(charaindex, itemindex);
}

void lssproto_DP_recv(int fd, int x, int y, int petindex) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(char_index, CHAR_LOCKED) == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "Ϊ��ȷ�������Ʒ��ȫ����������İ�ȫ������н�����\n");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  {
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  PET_dropPet(char_index, petindex);
}

void lssproto_DG_recv(int fd, int x, int y, int amount) {
  CHECKFDANDTIME;
  const int char_index = CONNECT_getCharaindex(fd);
  // ttom avoid the warp at will 12/15
  {
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  CHAR_DropMoney(char_index, amount);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_MI_recv(int fd, int fromid, int toid) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif

  if (fromid != toid) {
    int fromitemindex = CHAR_getItemIndex(char_index, fromid);
    int toitemindex = CHAR_getItemIndex(char_index, toid);
    if (ITEM_CHECKINDEX(fromitemindex) && ITEM_CHECKINDEX(toitemindex)) {
      CHAR_setWorkInt(char_index, CHAR_WORKITEMINDEX, fromitemindex);
      CHAR_setWorkInt(char_index, CHAR_WORKTOITEMINDEX, toitemindex);

#ifdef _ITEM_COLOER
      if (strcmp(ITEM_getChar(fromid, ITEM_USEFUNC), "ITEM_COLOER") == 0) {
        char *itemarg = ITEM_getChar(fromitemindex, ITEM_ARGUMENT);
        ITEM_setInt(toitemindex, ITEM_COLOER, atoi(itemarg));
        ITEM_setInt(fromitemindex, ITEM_COLOER, atoi(itemarg));
        CHAR_sendItemDataOne(char_index, toid);
        CHAR_DelItem(char_index, fromid);
        return;
      }
#endif
#ifdef _ITEM_UPLEVEL
      if (strcmp(ITEM_getChar(fromitemindex, ITEM_USEFUNC), "ITEM_UPLEVEL") ==
          0) {
        char message[256];
        char buf[256];

        int armtype = ITEM_getInt(toitemindex, ITEM_TYPE);

        if (armtype == ITEM_FIST || armtype == ITEM_AXE ||
            armtype == ITEM_CLUB || armtype == ITEM_SPEAR ||
            armtype == ITEM_BOW || armtype == ITEM_BOOMERANG ||
            armtype == ITEM_BOUNDTHROW || armtype == ITEM_BREAKTHROW ||
            armtype == ITEM_HELM || armtype == ITEM_ARMOUR) {
          sprintf(message, "�Ƿ�Ը���Ʒ�������������Ķ�����ϸ��\n\n"
                           "1��������Ʒ������װ���������˷�����ʯͷ\n"
                           "2��������Ʒ�ɹ��ȼ�+1��ʧ����ȼ�-1\n"
                           "3��װ���ȼ�Խ�ߣ������ɹ�������Խ��\n"
                           "4�����ʹ��ͣ��Ƿ��Ǹ����߼�������ʯ��"
                           "\n");

          lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
                           WINDOW_BUTTONTYPE_YESNO,
                           CHAR_WINDOWTYPE_ITEM_UPLEVEL, -1,
                           makeEscapeString(message, buf, sizeof(buf)));

          return;
        }
      }
#endif

#ifdef _ITEM_OVER_LAP
      if (ItemOverlapFunction(char_index, fromitemindex, toitemindex) == 0) {
        return;
      }
#endif
    }
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_moveEquipItem(char_index, fromid, toid);
}

#ifdef _PET_ITEM
void lssproto_PETITEM_recv(int fd, int x, int y, int petindex, int fromindex,
                           int toindex) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(char_index, CHAR_LOCKED) == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "Ϊ��ȷ�������Ʒ��ȫ����������İ�ȫ������н�����\n");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));
    return;
  }
#endif

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_movePetItem(char_index, petindex, fromindex, toindex);
}
#endif

void lssproto_SKUP_recv(int fd, int skillid) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);

  /* ��Ʈ��Ͻ��� �ʥ饰�Ǥ���˰��ä������ǽ�������*/
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_SkillUp(char_index, skillid);
}

/*------------------------------------------------------------
 * ���ͥ���������˥�å�����������
 ------------------------------------------------------------*/
void lssproto_MSG_recv(int fd, int index, char *message, int color) {
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_sendMessage(char_index, index, message, color);
}

/*------------------------------------------------------------
 * ���ɥ쥹�֥å������Ƥ����������ɤ����׵᤬�褿
 ------------------------------------------------------------*/
void lssproto_AB_recv(int fd) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_sendAddressbookTable(char_index);
}

/*------------------------------------------------------------
 * ���ɥ쥹�֥å��ι��ܤ�������
 ------------------------------------------------------------*/
void lssproto_DAB_recv(int fd, int index) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_deleteEntry(char_index, index);
}

void lssproto_AAB_recv(int fd, int x, int y) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);
  ADDRESSBOOK_addEntry(char_index);
}

void lssproto_L_recv(int fd, int dir) {
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  CHAR_Look(char_index, dir);
}

void lssproto_TK_recv(int fd, int x, int y, char *message, int color,
                      int area) {
  int char_index, ix, iy; // ttom+2
  int fmindex, channel;

  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  fmindex = CHAR_getInt(char_index, CHAR_FMINDEX);
  channel = CHAR_getWorkInt(char_index, CHAR_WORKFMCHANNEL);

  { // Robin 0629 silent
    int silentSec, talkCount;
    silentSec = CHAR_getInt(char_index, CHAR_SILENT);
    if (silentSec > 0) {
      int loginTime;
      char buf[256];
      int leftSec;
      loginTime = CHAR_getWorkInt(char_index, CHAR_WORKLOGINTIME);
      // Robin 20040817
      if ((int)NowTime.tv_sec < loginTime) {
        CHAR_setInt(char_index, CHAR_SILENT, 0);
        return;
      }
      if (((int)NowTime.tv_sec - loginTime) > silentSec) {
        CHAR_setInt(char_index, CHAR_SILENT, 0);
        return;
      }
      silentSec += 10; // ���10��
      leftSec = silentSec - ((int)NowTime.tv_sec - loginTime);
      sprintf(buf, "������!!����%d�룬�ٽ����10���ӡ�", leftSec);
      CHAR_talkToCli(char_index, -1, buf, color);
      CHAR_setInt(char_index, CHAR_SILENT, silentSec);
      return;
    }

    talkCount = CHAR_getWorkInt(char_index, CHAR_WORKTALKCOUNT);
    talkCount++;
    CHAR_setWorkInt(char_index, CHAR_WORKTALKCOUNT, talkCount);
    if (talkCount > 8) {
      int lastTalkTime = CHAR_getWorkInt(char_index, CHAR_WORKTALKTIME);
      if ((int)NowTime.tv_sec - lastTalkTime < 10) {
        CHAR_setInt(char_index, CHAR_SILENT, 60);
        CHAR_setWorkInt(char_index, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
        CHAR_talkToCli(char_index, -1,
                       "��̫�໰��ࡣ�������������Ϣ��һ���Ӱɣ"
                       "�",
                       color);
        CHAR_setWorkInt(char_index, CHAR_WORKTALKCOUNT, 0);
        return;
      } else {
        CHAR_setWorkInt(char_index, CHAR_WORKTALKTIME, (int)NowTime.tv_sec);
        CHAR_setWorkInt(char_index, CHAR_WORKTALKCOUNT, 0);
      }
    }
  }
  ix = CHAR_getInt(char_index, CHAR_X);
  iy = CHAR_getInt(char_index, CHAR_Y);
  x = ix;
  y = iy;
  CHAR_setMyPosition(char_index, x, y, TRUE);

  if (CHAR_getInt(char_index, CHAR_LV) < 5 &&
      message[strlen(message) - 1] != ']') {
    CHAR_talkToCli(char_index, -1, "���ĵȼ�С��5��,Ŀǰֻ��˵hi", CHAR_COLORYELLOW);
    CHAR_Talk(fd, char_index, "P|hi", color, area);
  } else {
    CHAR_Talk(fd, char_index, message, color, area);
  }

  //}
}

void lssproto_M_recv(int fd, int fl, int x1, int y1, int x2, int y2) {
  char *mapdata;
  RECT seek = {x1, y1, x2 - x1, y2 - y1}, ret;
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
#ifdef _MASK_ENCOUNTER
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    if (getStayEncount(fd))
      return;
  } else {
    int oyaindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    if (getStayEncount(getfdFromCharaIndex(oyaindex)))
      return;
  }
#endif
#ifdef _MO_MAP_AUTO_UPDATE
  mapdata = MAP_getdataFromRECT(fl, &seek, &ret);
  if (mapdata != NULL) {
    lssproto_M_send(fd, fl, ret.x, ret.y, ret.x + ret.width, ret.y + ret.height,
                    mapdata);
  }
#endif
}

void lssproto_C_recv(int fd, int index) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int char_index;

  char_index = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(char_index)) {
    return;
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif

  CHAR_sendCSpecifiedObjindex(fd, index);
}
#ifdef _FAMILYBADGE_
extern int getFamilyBadge(int index);
#endif
void lssproto_S_recv(int fd, char *category) {
  char *string;
  int char_index;
  char_index = CONNECT_getCharaindex(fd);
  string = CHAR_makeStatusString(char_index, category);
  if (string != NULL)
    lssproto_S_send(fd, string);
}

void lssproto_EV_recv(int fd, int event, int seqno, int x, int y, int dir) {
  int rc;
  int fx, fy;
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      goto CK1;
    }
    goto OK;
  }
CK1 : {
  OBJECT object;
  int ix, iy, ifloor, i, j;
  int warp_point_x[9];
  int warp_point_y[9];
  int warp_point = 0;
  ix = CHAR_getInt(char_index, CHAR_X);
  iy = CHAR_getInt(char_index, CHAR_Y);
  ifloor = CHAR_getInt(char_index, CHAR_FLOOR);
  for (i = iy - 1; i <= iy + 1; i++) {
    for (j = ix - 1; j <= ix + 1; j++) {
      for (object = MAP_getTopObj(ifloor, j, i); object;
           object = NEXT_OBJECT(object)) {
        int o = GET_OBJINDEX(object);
        if (OBJECT_getType(o) == OBJTYPE_CHARA) {
          int etype;
          int charaindex = OBJECT_getIndex(o);
          if (!CHAR_CHECKINDEX(charaindex))
            continue;
          etype = CHAR_getWorkInt(charaindex, CHAR_WORKEVENTTYPE);
          if (etype != CHAR_EVENT_NONE) {
            if (etype == CHAR_EVENT_WARP) {
              warp_point_x[warp_point] = j;
              warp_point_y[warp_point] = i;
              warp_point++;
            }
          }
        }
#ifdef __MAP_WARP_POINT
        else if (OBJECT_getType(o) == OBJTYPE_WARPPOINT) {
          int etype = OBJECT_getchartype(o);
          if (etype != CHAR_EVENT_NONE) {
            warp_point_x[warp_point] = j;
            warp_point_y[warp_point] = i;
            warp_point++;
            break;
          }
        }
#endif
      }
    }
  }

  for (i = 0; i < warp_point; i++) {
    if ((x == warp_point_x[i]) && (y == warp_point_y[i]))
      goto OK;
  }
  x = CHAR_getInt(char_index, CHAR_X);
  y = CHAR_getInt(char_index, CHAR_Y);
}
OK:
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    for (i = 0; i < getPartyNum(char_index); i++) {
      int subindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1 + i);
      if (CHAR_CHECKINDEX(subindex) == FALSE)
        continue;
    }
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);
  CHAR_setWorkChar(char_index, CHAR_WORKWALKARRAY, "");

  if (dir < 0 || dir > 7) {
    fx = CHAR_getInt(char_index, CHAR_X);
    fy = CHAR_getInt(char_index, CHAR_Y);
  } else {
    CHAR_getCoordinationDir(dir, CHAR_getInt(char_index, CHAR_X),
                            CHAR_getInt(char_index, CHAR_Y), 1, &fx, &fy);
  }
  rc = EVENT_main(char_index, event, fx, fy);
  lssproto_EV_send(fd, seqno, rc);
#ifdef _PLAYER_EFFECT
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT) > 0) {
    char msg[256];
    sprintf(msg, "2|%d", CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef FAMILY_MANOR_
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT_MANOR) > 0) {
    char msg[256];
    sprintf(msg, "3|%d",
            CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT_MANOR));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif

#ifdef _NEW_UPDATETITLE
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(char_index, CHAR_TITLE_DEFAULT) > 0) {
    char msg[256];
    sprintf(msg, "4|%d", CHAR_getInt(char_index, CHAR_TITLE_DEFAULT));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef _TRUMP_EQUIPMENT
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(char_index, CHAR_TRUMP_EFFECT) > 0) {
    char msg[256];
    sprintf(msg, "5|%d", CHAR_getInt(char_index, CHAR_TRUMP_EFFECT));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef _FAMILYBADGE_
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      getFamilyBadge(char_index)) {
    char msg[256];
    sprintf(msg, "1|%d", getFamilyBadge(char_index));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
}

void lssproto_EN_recv(int fd, int x, int y) {
  int ret = FALSE, err = 0;
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);

  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    CHAR_setMyPosition(char_index, x, y, TRUE);
    CHAR_setWorkChar(char_index, CHAR_WORKWALKARRAY, "");
    err = BATTLE_CreateVsEnemy(char_index, 0, -1);
    if (err != 0) {
      ret = FALSE;
    } else {
      ret = TRUE;
    }
  }
}
/*------------------------------------------------------------
 * �ץ쥤�䡼Ʊ�Τǥ��󥫥���ȡʷ�Ʈ��ȯ��
 ------------------------------------------------------------*/
void lssproto_DU_recv(int fd, int x, int y) {
  OBJECT object;
  int char_index;
  int ret = FALSE, charaindex = -1, enemyindex;
  int frontx, fronty;
  int cnt = 0;
  BOOL found = FALSE;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    int i;
    // ��ʬ�Υ���ǥå���
    charaindex = char_index;
    CHAR_setMyPosition(charaindex, x, y, TRUE);
    /* WALKARRAY�򥯥ꥢ���� */
    CHAR_setWorkChar(charaindex, CHAR_WORKWALKARRAY, "");

    /* ��������� */
    for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
      CONNECT_setDuelcharaindex(fd, i, -1);
    }
    CHAR_getCoordinationDir(
        CHAR_getInt(charaindex, CHAR_DIR), CHAR_getInt(charaindex, CHAR_X),
        CHAR_getInt(charaindex, CHAR_Y), 1, &frontx, &fronty);
    for (object =
             MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), frontx, fronty);
         object; object = NEXT_OBJECT(object)) {
      int toindex;
      int objindex = GET_OBJINDEX(object);
      if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
        continue;
      toindex = OBJECT_getIndex(objindex);
      /* �ץ쥤�䡼����ʤ� */
      if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
          && CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
      )
        continue;
      found = TRUE;
      if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
        continue;

#ifdef _ALLBLUES_LUA_1_5
      int flg = FreeVsPlayer(charaindex, toindex);
      if (flg == 1) {
        lssproto_EN_send(fd, FALSE, 0);
        return;
      } else if (flg == 2) {
        return;
      } else
#endif
          if (!CHAR_getFlg(toindex, CHAR_ISDUEL)) {
        int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
#ifdef _AUTO_PK
        if (floor == 20000) {
          if (AutoPk_PKTimeGet() > 0) {
            CHAR_talkToCli(charaindex, -1, "��ȱ�����ʼ�ٽ���PK��",
                           CHAR_COLORYELLOW);
            continue;
          } else {
            if (!CHAR_getFlg(toindex, CHAR_ISDUEL)) {
              if (CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) == -1)
                continue;
              if (CHAR_getWorkInt(toindex, CHAR_WORK_AUTOPK) == -1)
                continue;
            }
          }
        } else
#endif
#ifdef _BATTLE_FLOOR
        {
          if (strcmp(getBattleFloorCF(), "��") != 0) {
            int i;
            for (i = 0; i < 32; i++) {
              if (floor == getBattleFloor(i)) {
                break;
              }
            }
            if (i == 32)
              continue;
          }
        }
#else
        continue;
#endif
      }
#ifdef _BATTLE_PK_TYPE
      else {
        int oyaindex1;
        int oyaindex2;
        int workindex;
        int type = CHAR_getWorkInt(charaindex, CHAR_WORK_BATTLEPKTYPE);
        switch (type) {
        case 0: // ����PK
          break;
        case 1: // ��ϵ�P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) !=
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
            CHAR_talkToCli(charaindex, toindex, "��ϵ�P����", CHAR_COLORYELLOW);
            continue;
          }
          break;
        case 2: // �����P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
            CHAR_talkToCli(charaindex, toindex, "�����P����", CHAR_COLORYELLOW);
            continue;
          }
          oyaindex1 = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
          oyaindex2 = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
          if (CHAR_CHECKINDEX(oyaindex1) && CHAR_CHECKINDEX(oyaindex2)) {
            int i;
            for (i = 0; i < getPartyNum(oyaindex1); i++) {
              workindex = CHAR_getWorkInt(oyaindex1, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_NONE) {
                  CHAR_talkToCli(charaindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
              workindex = CHAR_getWorkInt(oyaindex2, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                    CHAR_PARTY_NONE) {
                  CHAR_talkToCli(charaindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
            }
          }

          break;
#ifdef _CHAR_PROFESSION
        case 3: // ��ְҵ��P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) !=
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ||
              CHAR_getInt(charaindex, PROFESSION_CLASS) == 0) {
            CHAR_talkToCli(charaindex, toindex, "��ְҵ��P����", CHAR_COLORYELLOW);
            continue;
          }
          break;
        case 4: // ��ְҵ��P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ||
              CHAR_getInt(charaindex, PROFESSION_CLASS) == 0) {
            CHAR_talkToCli(charaindex, toindex, "��ְҵ��P����", CHAR_COLORYELLOW);
            continue;
          }
          oyaindex1 = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
          oyaindex2 = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
          if (CHAR_CHECKINDEX(oyaindex1) && CHAR_CHECKINDEX(oyaindex2)) {
            int i;
            for (i = 0; i < getPartyNum(oyaindex1); i++) {
              workindex = CHAR_getWorkInt(oyaindex1, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                        CHAR_PARTY_NONE ||
                    CHAR_getInt(charaindex, PROFESSION_CLASS) == 0) {
                  CHAR_talkToCli(workindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
              workindex = CHAR_getWorkInt(oyaindex2, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                        CHAR_PARTY_NONE ||
                    CHAR_getInt(workindex, PROFESSION_CLASS) == 0) {
                  CHAR_talkToCli(charaindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
            }
          }
          break;
        case 5: // ��ְҵ��P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) !=
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ||
              CHAR_getInt(charaindex, PROFESSION_CLASS) > 0) {
            CHAR_talkToCli(charaindex, toindex, "��ְҵ��P����", CHAR_COLORYELLOW);
            continue;
          }
          break;
        case 6: // ��ְҵ��P
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
                  CHAR_PARTY_NONE ||
              CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ||
              CHAR_getInt(charaindex, PROFESSION_CLASS) > 0) {
            CHAR_talkToCli(charaindex, toindex, "��ְҵ��P����", CHAR_COLORYELLOW);
            continue;
          }
          oyaindex1 = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
          oyaindex2 = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
          if (CHAR_CHECKINDEX(oyaindex1) && CHAR_CHECKINDEX(oyaindex2)) {
            int i;
            for (i = 0; i < getPartyNum(oyaindex1); i++) {
              workindex = CHAR_getWorkInt(oyaindex1, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                        CHAR_PARTY_NONE ||
                    CHAR_getInt(workindex, PROFESSION_CLASS) > 0) {
                  CHAR_talkToCli(charaindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
              workindex = CHAR_getWorkInt(oyaindex2, CHAR_WORKPARTYINDEX1 + i);
              if (CHAR_CHECKINDEX(workindex)) {
                if (CHAR_getWorkInt(workindex, CHAR_WORKPARTYMODE) ==
                        CHAR_PARTY_NONE ||
                    CHAR_getInt(workindex, PROFESSION_CLASS) > 0) {
                  CHAR_talkToCli(charaindex, workindex, "�����P����",
                                 CHAR_COLORYELLOW);
                  continue;
                }
              }
            }
          }
          break;
#endif
        }
      }
#endif
      {
        int i;
        for (i = 0; i < FAMILY_FMPKFLOOR; i++) {
          if (fmpkflnum[i].fl == CHAR_getInt(charaindex, CHAR_FLOOR)) {
            if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLAG) == -1) {
              lssproto_EN_send(fd, FALSE, 0);
              return;
            }
            if (CHAR_getInt(charaindex, CHAR_FMINDEX) ==
                CHAR_getInt(toindex, CHAR_FMINDEX)) {
              lssproto_EN_send(fd, FALSE, 0);
              return;
            }
          }
        }
      }
      if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
        int tmpindex = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(tmpindex)) {
          if (CHAR_getWorkInt(tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
              && CHAR_getInt(tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
          ) {
            continue;
          }
        }
      }

      CONNECT_setDuelcharaindex(fd, cnt, toindex);
      cnt++;
      if (cnt == CONNECT_WINDOWBUFSIZE)
        break;
    }
    if (cnt == 0) {
      goto lssproto_DU_recv_Err;
    } else if (cnt == 1) {
      enemyindex = CONNECT_getDuelcharaindex(fd, 0);
      if (CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYMODE) ==
          CHAR_PARTY_CLIENT) {
        enemyindex = CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYINDEX1);
        if (enemyindex < 0)
          goto lssproto_DU_recv_Err;
      }

#ifdef _VIP_POINT_PK
      {
        char token[256];
        if ((CHAR_getInt(charaindex, CHAR_FLOOR) == 11111 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 11111) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 22222 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 22222) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 33333 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 33333) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 44444 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 44444) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 20000 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 20000)) {
          int menum = 0, tonum = 0;
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_NONE) {
            menum = 1;
          } else {
            for (i = 0; i < getPartyNum(charaindex); i++) {
              int index = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(index)) {
                menum++;
              }
            }
          }

          if (CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_NONE) {
            tonum = 1;
          } else {
            for (i = 0; i < getPartyNum(enemyindex); i++) {
              int index = CHAR_getWorkInt(enemyindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(index)) {
                tonum++;
              }
            }
          }

          if (menum != tonum) {
            sprintf(token,
                    "�ҷ�ս������%d���Է�ս������%"
                    "d�������������Գƣ��޷���ս�"
                    "�",
                    menum, tonum);
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            lssproto_EN_send(fd, FALSE, 0);
            return;
          }
        }

        if (CHAR_getInt(charaindex, CHAR_FLOOR) == 44444 &&
            CHAR_getInt(enemyindex, CHAR_FLOOR) == 44444) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(3)) {
            sprintf(
                token,
                "��Ļ��ֵ㲻��%d���޷�����PK��",
                getVipPointPK(3));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(3)) {
            sprintf(token,
                    "�Է����ֵ㲻��%"
                    "d���޷�����PK��",
                    getVipPointPK(3));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 33333 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 33333) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(2)) {
            sprintf(
                token,
                "��Ļ��ֵ㲻��%d���޷�����PK��",
                getVipPointPK(2));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(2)) {
            sprintf(token,
                    "�Է����ֵ㲻��%"
                    "d���޷�����PK��",
                    getVipPointPK(2));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 22222 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 22222) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(1)) {
            sprintf(
                token,
                "��Ļ��ֵ㲻��%d���޷�����PK��",
                getVipPointPK(1));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(1)) {
            sprintf(token,
                    "�Է����ֵ㲻��%"
                    "d���޷�����PK��",
                    getVipPointPK(1));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 11111 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 11111) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(0)) {
            sprintf(
                token,
                "��Ļ��ֵ㲻��%d���޷�����PK��",
                getVipPointPK(0));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(0)) {
            sprintf(token,
                    "�Է����ֵ㲻��%"
                    "d���޷�����PK��",
                    getVipPointPK(0));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        }
      }
#endif

      ret = BATTLE_CreateVsPlayer(charaindex, enemyindex);
      if (ret != 0) {
        ret = FALSE;
      } else {
        ret = TRUE;
      }
    }
    /* ���Ͱʾ夤����ϥ�����ɥ���Ф����䤤��碌�� */
    else if (cnt > 1) {
      int strlength;
      char msgbuf[1024];
      char escapebuf[2048];
      strcpy(msgbuf, "1\nҪ��˭ս����\n");
      strlength = strlen(msgbuf);
      /* ������ɥ��Υ�å�����������
       * ��Ʈ��Υ����ΰ���
       */
      for (i = 0;
           CONNECT_getDuelcharaindex(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
           i++) {
        char *a = CHAR_getChar(CONNECT_getDuelcharaindex(fd, i), CHAR_NAME);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s [%s]\n", a,
                 CHAR_getWorkInt(CONNECT_getDuelcharaindex(fd, i),
                                 CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE
                     ? "����"
                     : "����");
        if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
          print("%s:%d�Ӵ�ѶϢbuffer���㡣\n", __FILE__, __LINE__);
          break;
        }
        strcpy(&msgbuf[strlength], buf);
        strlength += strlen(buf);
      }

      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_SELECTDUEL, -1,
                       makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
      ret = TRUE;
    }
  }

lssproto_DU_recv_Err:;
  if (ret == FALSE) {
    /* ����˪�� */
    lssproto_EN_send(fd, FALSE, 0);
    if (cnt > 0)
      return;
    else if (found)
      CHAR_talkToCli(charaindex, -1, "���˿��Զ�ս��", CHAR_COLORYELLOW);
    else
      CHAR_talkToCli(charaindex, -1, "����û���κ��ˡ�", CHAR_COLORYELLOW);
  }
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_EO_recv(int fd, int dummy) {
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  BattleEncountOut(char_index);
#ifdef _PLAYER_EFFECT
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT) > 0) {
    char msg[256];
    sprintf(msg, "2|%d", CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT));
#ifdef _FAMILYBADGE_
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(char_index), msg);
#endif
  }
#endif
#ifdef FAMILY_MANOR_
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT_MANOR) > 0) {
    char msg[256];
    sprintf(msg, "3|%d",
            CHAR_getWorkInt(char_index, CHAR_PLAYER_EFFECT_MANOR));
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(char_index), msg);
  }
#endif
#ifdef _TRUMP_EQUIPMENT
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(char_index, CHAR_TRUMP_EFFECT) > 0) {
    char msg[256];
    sprintf(msg, "5|%d", CHAR_getInt(char_index, CHAR_TRUMP_EFFECT));
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(char_index), msg);
  }
#endif
#ifdef _NEW_UPDATETITLE
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(char_index, CHAR_TITLE_DEFAULT) > 0) {
    char msg[256];
    sprintf(msg, "4|%d", CHAR_getInt(char_index, CHAR_TITLE_DEFAULT));
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(char_index), msg);
  }
#endif
#ifdef _FAMILYBADGE_
  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      getFamilyBadge(char_index)) {
    char msg[256];
    sprintf(msg, "1|%d", getFamilyBadge(char_index));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_BU_recv(int fd, int dummy) {
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  // shan 2001/12/25
  // BATTLE_WatchStop( char_index );
}

void lssproto_B_recv(int fd, char *command) {
  int char_index;
  int battle_index; // ttom++
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  BattleCommandDispach(fd, command);
  // Nuke +1 0827: Battle acceleration
  battle_index = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);
  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return;
  }
}

void lssproto_FS_recv(int fd, int flg) {
  int char_index;
  CHECKFDANDTIME;

  char_index = CONNECT_getCharaindex(fd);
  /* �������������ڱ幫�������׷º��ޥ
   */
  CHAR_setFlg(char_index, CHAR_ISPARTY,
              (flg & CHAR_FS_PARTY) ? TRUE : FALSE);
  //  CHAR_setFlg( char_index, CHAR_ISBATTLE,
  //        (flg & CHAR_FS_BATTLE )? TRUE:FALSE);

#ifdef _BATTLE_PK_TYPE
  if ((CHAR_getFlg(char_index, CHAR_ISDUEL) == FALSE) &&
      (flg & CHAR_FS_DUEL)) {
    CHAR_setWorkInt(char_index, CHAR_WORK_BATTLEPKTYPE, 0);
    char message[256];
    char buf[256];
    sprintf(message, "��ѡ������ϣ����PK����\n"
                     "����PK\n"
                     "��ϵ�P\n"
                     "�����P\n"
#ifdef _CHAR_PROFESSION
                     "��ְҵ��P\n"
                     "��ְҵ��P\n"
                     "��ְҵ��P\n"
                     "��ְҵ��P\n"
#endif
    );

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_BATTLEPKTYPE, -1,
                     makeEscapeString(message, buf, sizeof(buf)));
  }
#endif
  CHAR_setFlg(char_index, CHAR_ISDUEL, (flg & CHAR_FS_DUEL) ? TRUE : FALSE);
  CHAR_setFlg(char_index, CHAR_ISPARTYCHAT,
              (flg & CHAR_FS_PARTYCHAT) ? TRUE : FALSE);
  CHAR_setFlg(char_index, CHAR_ISTRADECARD,
              (flg & CHAR_FS_TRADECARD) ? TRUE : FALSE);
#ifdef _CHANNEL_MODIFY
  // ����Ƶ������
  CHAR_setFlg(char_index, CHAR_ISTELL, (flg & CHAR_FS_TELL) ? TRUE : FALSE);
  // ����Ƶ������
  CHAR_setFlg(char_index, CHAR_ISFM, (flg & CHAR_FS_FM) ? TRUE : FALSE);
  // ְҵƵ������
  CHAR_setFlg(char_index, CHAR_ISOCC, (flg & CHAR_FS_OCC) ? TRUE : FALSE);
  // ������
  CHAR_setFlg(char_index, CHAR_ISCHAT, (flg & CHAR_FS_CHAT) ? TRUE : FALSE);
  // ����Ի�����
  CHAR_setFlg(char_index, CHAR_ISSAVE, (flg & CHAR_FS_SAVE) ? TRUE : FALSE);

#ifdef _THE_WORLD_SEND
  // ����Ƶ������
  CHAR_setFlg(char_index, CHAR_ISWORLD,
              (flg & CHAR_FS_WORLD) ? TRUE : FALSE);
#endif
#endif
  CHAR_setFlg(char_index, CHAR_AI_MOD, (flg & CHAR_FS_AI) ? TRUE : FALSE);

  CHAR_setFlg(char_index, CHAR_ISTRADE,
              (flg & CHAR_FS_TRADE) ? TRUE : FALSE);
  lssproto_FS_send(fd, flg);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PR_recv(int fd, int x, int y, int request) {
  int result = FALSE;
  int char_index;
  CHECKFDANDTIME;

  char_index = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(char_index))
    return;

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#if 1 // ��ֹ�������
  if (request == 1) {
    int nowFloor;
    nowFloor = CHAR_getInt(char_index, CHAR_FLOOR);
    if (nowFloor == 31706 || nowFloor == 10204 ||
        (10601 <= nowFloor && nowFloor <= 10605) || nowFloor == 10919 ||
        nowFloor == 10920 || nowFloor == 20711 || nowFloor == 20712 ||
        nowFloor == 1008 || nowFloor == 1021 || nowFloor == 3008 ||
        nowFloor == 3021 || (nowFloor <= 8213 && nowFloor >= 8200) ||
        (nowFloor >= 30017 && nowFloor <= 30021)
#ifdef _TIME_TICKET
        || check_TimeTicketMap(nowFloor)
#endif
#ifdef _NPC_MAGICCARD
        || nowFloor == 17003 || nowFloor == 17005
#endif
    ) {
      //    print("\n �ķ��!��ֹ�������!!:%s ", CHAR_getChar( char_index,
      //    CHAR_CDKEY) );
      return;
    }
  }
#endif

  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<PR>--Error!!!!");
      // print("\n<PR>origion x=%d,y=%d",ix,iy);
      // print("\n<PR>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(char_index, x, y, TRUE);

  if (request == 0) {
    /* �������� */
    result = CHAR_DischargeParty(char_index, 0);
  } else if (request == 1) {
    result = CHAR_JoinParty(char_index);
  }
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_KS_recv(int fd, int petarray) {
  int ret, char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(char_index))
    return;
  if (petarray != -1 && CHAR_getInt(char_index, CHAR_RIDEPET) == petarray) {
    lssproto_KS_send(fd, petarray, FALSE);
    return;
  }
  ret = PET_SelectBattleEntryPet(char_index, petarray);
  lssproto_KS_send(fd, petarray, ret);
}

void lssproto_SPET_recv(int fd, int standbypet) {
  int char_index;
  int i, s_pet = 0, cnt = 0;

  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(char_index))
    return;
  // if( CHAR_getWorkInt( char_index, CHAR_WORKBATTLEMODE) !=
  // BATTLE_CHARMODE_NONE
  //   && standbypet >= CHAR_getWorkInt( char_index, CHAR_WORKSTANDBYPET) ) {
  //   print("\n �ķ��!??ս������SPET���Ӵ�����!!:%s ", CHAR_getChar(
  //   char_index, CHAR_CDKEY) ); return;
  // }

  // if( CHAR_getInt( char_index, CHAR_RIDEPET) == petarray ) {
  //   lssproto_SPET_send( fd, petarray, FALSE);
  //}

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (standbypet & (1 << i)) {

      if (CHAR_getInt(char_index, CHAR_RIDEPET) == i)
        continue;

      cnt++;
      // if( cnt > 4 ) {
      //   print("\n �ķ��!�����賬������!!:%s ", CHAR_getChar( char_index,
      //   CHAR_CDKEY) );
      // lssproto_SPET_send( fd, s_pet, FALSE);
      //  break;
      //}

      s_pet |= (1 << i);
    }
  }
  CHAR_setWorkInt(char_index, CHAR_WORKSTANDBYPET, s_pet);

  lssproto_SPET_send(fd, s_pet, TRUE);
}

void lssproto_AC_recv(int fd, int x, int y, int actionno) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);

  { // ttom avoid the warp at will
    Char *ch;
    ch = CHAR_getCharPointer(char_index);
    // CoolFish: +1 2001/11/05
    if (!ch)
      return;
    if ((ch->data[CHAR_X] != x) || (ch->data[CHAR_Y] != y)) {
      return;
    }
  }
#ifdef _ALLBLUES_LUA_1_9
  FreeAction(char_index, x, y, actionno);
#endif
  CHAR_setMyPosition(char_index, x, y, TRUE);
  CHAR_sendAction(char_index, actionno, FALSE);
  return;
}

void lssproto_LOOK_recv(int fd, int x, int y) {
  int charaindex, floor;
  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(charaindex, CHAR_X);
    iy = CHAR_getInt(charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(charaindex, x, y, TRUE);
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  BATTLE_WatchTry(charaindex);
}

void lssproto_MU_recv(int fd, int x, int y, int array, int toindex) {
  int to_charaindex = -1, char_index;
  CHECKFDANDTIME;
  char_index = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(char_index, CHAR_X);
    iy = CHAR_getInt(char_index, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<MU>--Error!!!!");
      // print("\n<MU>origion x=%d,y=%d",ix,iy);
      // print("\n<MU>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  }

  CHAR_setMyPosition(char_index, x, y, TRUE);
  to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  MAGIC_Use(char_index, array, to_charaindex);
}

void lssproto_JB_recv(int fd, int x, int y) {
  int charaindex, floor;

  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(charaindex, CHAR_X);
    iy = CHAR_getInt(charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }

  CHAR_setMyPosition(charaindex, x, y, TRUE);
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
#ifdef _BATTLE_LOOK_
  BATTLE_RescueTry(charaindex);
#else
  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  if (floor == 1007 || floor == 2007 || floor == 3007 || floor == 4007 ||
      floor == 130
#ifdef _AUTO_PK
      || (floor == 20000 && CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) == -1)
#endif
#ifdef _WATCH_FLOOR
      || floor == getWatchFloor(1) || floor == getWatchFloor(2) ||
      floor == getWatchFloor(3) || floor == getWatchFloor(4) ||
      floor == getWatchFloor(5) || !strcmp(getWatchFloorCF(), "��")
#endif
  ) {
    BATTLE_WatchTry(charaindex);
  } else {
    if (BATTLE_RescueTry(charaindex) == FALSE) {
      BATTLE_WatchTry(charaindex);
    }
  }
#endif
}

void lssproto_KN_recv(int fd, int havepetindex, char *data) {
  int char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);

  // Robin 04/26 debug
  if (strlen(data) > 16)
    return;

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  if (checkStringErr(data))
    return;
#ifdef _CHANGE_PETNAME_FIX
  if (strstr(data, "|") != NULL && strstr(data, "'") != NULL &&
      strstr(data, "/") != NULL && strstr(data, "\\") != NULL) {
    CHAR_talkToCli(char_index, -1, "���������벻Ҫ���зǷ��ַ���", CHAR_COLORRED);
    return;
  }
#endif
  CHAR_inputUserPetName(char_index, havepetindex, data);
}

void lssproto_WN_recv(int fd, int x, int y, int seqno, int objindex, int select,
                      char *data) {
  CHECKFDANDTIME;
  if (checkStringErr(data))
    return;
  const int char_index = CONNECT_getCharaindex(fd);
  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    return;
  }
  // Robin
  if (checkStringErr(data))
    return;

#ifdef _ANGEL_SUMMON
  if (seqno == CHAR_WINDOWTYPE_ANGEL_ASK) {
    print("\n CHAR_WINDOWTYPE_ANGEL_ASK objindex:%d select:%d data:%s ",
          objindex, select, data);

    if (select == WINDOW_BUTTONTYPE_YES) {
      if (AngelCreate(char_index) == FALSE) {
        sendAngelCleanToCli(fd);
      }
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      int mindex;
      char name_info[64];
      mindex = checkIfAngel(char_index);
      print("====Check Angel Summon Mission Info====");
      getMissionNameInfo(char_index, name_info);
      saacproto_ACMissionTable_send(acfd, mindex, 3, name_info, "");
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK, -1,
                       -1, "�����ź���\n������İ���������ħ������Σ����½�������ˡ�");
      sendAngelCleanToCli(fd);
    }
  }
#endif

  { // ttom avoid the warp at will
    Char *ch;
    ch = CHAR_getCharPointer(char_index);
    // CoolFish: +1 2001/11/05
    if (!ch)
      return;
    if ((ch->data[CHAR_X] != x) || (ch->data[CHAR_Y] != y)) {
      // Robin 04/20 test
      return;
      x = ch->data[CHAR_X];
      y = ch->data[CHAR_Y];
    }
    // ttom avoid WN at will
    if (seqno == CHAR_WINDOWTYPE_NPCENEMY_START) {
      //      Char *ch;
      OBJECT object;
      int ix, iy, ifloor, i, j;
      int whichtype = -1;
      int enemy = 0;
      int enemy_index;
      if (!CHECKOBJECT(objindex)) {
        goto FIRST;
      }
      enemy_index = OBJECT_getIndex(objindex); // ttom 11/15/2000
      ix = ch->data[CHAR_X];
      iy = ch->data[CHAR_Y];
      ifloor = ch->data[CHAR_FLOOR];
      for (i = iy - 1; i <= iy + 1; i++) {
        for (j = ix - 1; j <= ix + 1; j++) {
          for (object = MAP_getTopObj(ifloor, j, i); object;
               object = NEXT_OBJECT(object)) {
            int objindex = GET_OBJINDEX(object);
            switch (OBJECT_getType(objindex)) {
            case OBJTYPE_CHARA:
              whichtype =
                  CHAR_getInt(OBJECT_getIndex(objindex), CHAR_WHICHTYPE);
              if (whichtype == CHAR_TYPENPCENEMY) {
                int i_ene_temp;
                i_ene_temp = OBJECT_getIndex(objindex);
                // print("\n<WN>--enetemp=%d",i_ene_temp);
                if (i_ene_temp == enemy_index) {
                  goto START_WN;
                } else {
                  enemy = 0;
                }
              }
              break;
            case OBJTYPE_ITEM:
              break;
            case OBJTYPE_GOLD:
              break;
            default:
              break;
            }
          }
        }
      }
      if (enemy == 0) {
      FIRST:

        lssproto_EN_send(fd, FALSE, 0);
        CHAR_talkToCli(char_index, -1, "�¼�����", CHAR_COLORYELLOW);
        goto END_WN;
      }
    }
  }
  // ttom end
START_WN:
  CHAR_setMyPosition(char_index, x, y, TRUE);
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
      BATTLE_CHARMODE_NONE) {
    if (seqno == CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN) {
      if (!CONNECT_get_first_warp(fd)) {
        select = 1;
      }
    }
    CHAR_processWindow(char_index, seqno, select, objindex,
                       makeStringFromEscaped(data));
  }
// ttom+1
END_WN:
  return;
}

/* 收到战斗中发起的HELP请求 */
void lssproto_HL_recv(int fd, int flg) {
  char msgbuf[128];
  int i, char_index;
  CHECKFD;
  char_index = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
      BATTLE_CHARMODE_NONE) {
    return;
  }
#ifdef _LOCKHELP_OK // (���ɿ�) Syu ADD �������ɼ���ս��
  if ((CHAR_getInt(char_index, CHAR_FLOOR) >= 8200 &&
       CHAR_getInt(char_index, CHAR_FLOOR) <= 8213) ||
      (CHAR_getInt(char_index, CHAR_FLOOR) >= 30017 &&
       CHAR_getInt(char_index, CHAR_FLOOR) <= 30021) ||
      CHAR_getInt(char_index, CHAR_FLOOR) == 20000) {
    return;
  }
#endif
#ifdef _NO_HELP_MAP
  {
    int floor = CHAR_getInt(char_index, CHAR_FLOOR);
    if (floor >= 33001 && floor <= 33101) {
      return;
    }
    int i;
    for (i = 0; i < 32; i++) {
      if (floor == getNoHelpMap(i)) {
        return;
      }
    }
  }
#endif
  if (flg == TRUE) {
    BattleArray[CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX)]
        .Side[CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE)]
        .flg |= BSIDE_FLG_HELP_OK;

    snprintf(msgbuf, sizeof(msgbuf), "%s ����ȣ�",
             CHAR_getChar(char_index, CHAR_NAME));
  } else {
    BattleArray[CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX)]
        .Side[CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE)]
        .flg &= ~BSIDE_FLG_HELP_OK;

    snprintf(msgbuf, sizeof(msgbuf), "%s �����ܾ�������",
             CHAR_getChar(char_index, CHAR_NAME));
  }

  for (i = 0; i < 5; i++) {
    int toindex =
        BattleArray[CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX)]
            .Side[CHAR_getWorkInt(char_index, CHAR_WORKBATTLESIDE)]
            .Entry[i]
            .charaindex;
    if (CHAR_CHECKINDEX(toindex)) {
      int tofd = getfdFromCharaIndex(toindex);
      if (tofd != -1) {
        lssproto_HL_send(tofd, flg);
      }
      CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORYELLOW);
      CHAR_sendBattleEffect(toindex, ON);
    }
  }
}

void lssproto_ProcGet_recv(int fd) { outputNetProcLog(fd, 1); }

void lssproto_PlayerNumGet_recv(int fd) {
  /*
    int    i;
    int    clicnt  =0;
    int    playercnt = 0;
      for( i = 0; i < ConnectLen; i ++ ) {
          if( CONNECT_getUse_debug(i,1017) ){
              if( CONNECT_getCtype(i) == CLI) {
                clicnt ++;
                if( CONNECT_getCharaindex(i) >= 0 ) playercnt++;
              }
      }
    }
    lssproto_PlayerNumGet_send( fd, clicnt * 0.6, playercnt * 0.6);
  */
}

void lssproto_LB_recv(int fd, int x, int y) {
  CHECKFDANDTIME;
  const int from_id = CONNECT_getCharaindex(fd);
  {
    const ix = CHAR_getInt(from_id, CHAR_X);
    const iy = CHAR_getInt(from_id, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(from_id, x, y, TRUE);
  BATTLE_WatchTry(from_id);
}

void lssproto_Shutdown_recv(int fd, char *passwd, int min) {
  char buff[32];
  // ??? why need this password in mingwen?
  if (strcmp(passwd, "hogehoge") == 0) {
    int i;
    const int player_max_num = CHAR_getPlayerMaxNum();
    snprintf(buff, sizeof(buff), "%s服务器即将关闭.", getGameservername());
    for (i = 0; i < player_max_num; i++) {
      if (CHAR_getCharUse(i) != FALSE) {
        CHAR_talkToCli(i, -1, buff, CHAR_COLORYELLOW);
      }
    }
    SERVSTATE_setLimittime(min);
    SERVSTATE_setShutdown(NowTime.tv_sec);
    SERVSTATE_setDsptime(0);
  }
}
void lssproto_PMSG_recv(int fd, int index, int petindex, int itemindex,
                        char *message, int color) {

  // CoolFish: Prevent Trade Cheat 2001/4/18
  int char_index;
  char_index = CONNECT_getCharaindex(fd);

  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(char_index, CHAR_LOCKED) == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "Ϊ��ȷ�������Ʒ��ȫ����������İ�ȫ������н�����\n");
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  if (CHAR_getWorkInt(char_index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  PETMAIL_sendPetMail(CONNECT_getCharaindex(fd), index, petindex, itemindex,
                      message, color);
}

void lssproto_PS_recv(int fd, int havepetindex, int havepetskill, int toindex,
                      char *data) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  int to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  int petindex;
  BOOL ret;
  petindex = CHAR_getCharPet(charaindex, havepetindex);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  ret = PETSKILL_Use(petindex, havepetskill, to_charaindex, data);
  lssproto_PS_send(fd, ret, havepetindex, havepetskill, toindex);
}

void lssproto_SP_recv(int fd, int x, int y, int dir) {
  const int char_index = CONNECT_getCharaindex(fd);
  { // ttom avoid the warp at will
    int i_x, i_y;
    i_x = CHAR_getInt(char_index, CHAR_X);
    i_y = CHAR_getInt(char_index, CHAR_Y);
    if ((i_x != x) || (i_y != y)) {
      x = i_x;
      y = i_y;
    }
  } // ttom
  CHAR_setMyPosition_main(char_index, x, y, dir, TRUE);
}

void lssproto_TD_recv(int fd, char *message) {
  CHECKFDANDTIME;
  const int char_index = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(char_index, CHAR_WORKSTREETVENDOR) != -1) {
    CHAR_talkToCli(char_index, -1, "不在摆摊状态.", CHAR_COLORYELLOW);
    return;
  }
  CHAR_Trade(fd, char_index, message);
}

void lssproto_FM_recv(int fd, char *message) {
  const int char_index = CONNECT_getCharaindex(fd);
  if (checkStringErr(message))
    return;
  CHAR_Family(fd, char_index, message);
}

// shan 2002/01/10
void lssproto_PETST_recv(int fd, int nPet, int sPet) {
  int charaindex;
  int i, nums = 0;
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

  for (i = 0; i < 5; i++) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORK_PET0_STAT + i) == TRUE)
      nums++;
  }

  if (nPet < 0)
    nPet = 0;
  if (nPet > 4)
    nPet = 4;

  if (nums <= 3) {
    CHAR_setWorkInt(charaindex, CHAR_WORK_PET0_STAT + nPet, sPet);
  }

  if (nPet != CHAR_getInt(charaindex, CHAR_DEFAULTPET)) {
    lssproto_PETS_send(fd, nPet, sPet);
  }
}

#ifdef _MIND_ICON
void lssproto_MA_recv(int fd, int x, int y, int nMind) {
  int charaindex;
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  {
    int i_x, i_y;
    i_x = CHAR_getInt(charaindex, CHAR_X);
    i_y = CHAR_getInt(charaindex, CHAR_Y);
    if ((i_x != x) || (i_y != y)) {
      x = i_x;
      y = i_y;
    }
  }

  CHAR_setMyPosition(charaindex, x, y, TRUE);
  CHAR_setWorkInt(charaindex, CHAR_MIND_NUM, nMind);
  CHAR_sendMindEffect(charaindex, CHAR_getWorkInt(charaindex, CHAR_MIND_NUM));
  if (CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101290 &&
      CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101294 &&
      CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101288)
    CHAR_setWorkInt(charaindex, CHAR_MIND_NUM, 0);
  return;
}
#endif
BOOL checkStringErr(char *checkstring) {
  int i, ach;
  for (i = 0, ach = 0; i < strlen(checkstring); i++) {
    if ((unsigned char)checkstring[i] == 0xff) {
      ach = 1;
      break;
    } // Force no 0xff
    if ((unsigned char)checkstring[i] == 0x80) {
      ach = 1;
      break;
    } // Force no 0x80
    if ((unsigned char)checkstring[i] == 0x7f) {
      ach = 1;
      break;
    } // Force no 0x7f
    if ((unsigned char)checkstring[i] <= 0x20) {
      ach = 1;
      break;
    } // Force greater than 0x20
    if ((unsigned char)checkstring[i] == 0x27) {
      ach = 1;
      break;
    } // Force greater than 0x27
    if ((unsigned char)checkstring[i] == 0x3b) {
      ach = 1;
      break;
    } // Force greater than 0x3b
    if (ach) {
      if ((((unsigned char)checkstring[i] >= 0x40) &&
           ((unsigned char)checkstring[i] <= 0x7e)) ||
          (((unsigned char)checkstring[i] >= 0xa1) &&
           ((unsigned char)checkstring[i] <= 0xfe)))
        ach = 0;
    } else {
      if (((unsigned char)checkstring[i] >= 0xa1) &&
          ((unsigned char)checkstring[i] <= 0xfe))
        ach = 1;
    }
  }
  if (ach) {
    print("StringDog!");
    return TRUE;
  }
  return FALSE;
}

#ifdef _TEAM_KICKPARTY
void lssproto_KTEAM_recv(int fd, int si) {
  // 移出队伍中的第几个成员.
  if (si < 0 || si > 5)
    return;
  // from 是从 fd 得到, 因此应该是队长操作.
  const int from_id = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(from_id))
    return;
  // from_id 必须是队长.
  if (CHAR_getWorkInt(from_id, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
    return;
  const int to_id = CHAR_getWorkInt(from_id, si + CHAR_WORKPARTYINDEX1);
  if (!CHAR_CHECKINDEX(to_id))
    return;
  // to_id 必须不是队长.
  if (CHAR_getWorkInt(to_id, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER)
    return;
  if (CHAR_DischargeParty(to_id, 0) == FALSE) {
    CHAR_talkToCli(from_id, -1, "移出队伍失败.", CHAR_COLORYELLOW);
  } else {
    char buf1[256];
    sprintf(buf1, "角色[%s]将你移出队伍.", CHAR_getUseName(from_id));
    CHAR_talkToCli(to_id, -1, buf1, CHAR_COLORYELLOW);
    sprintf(buf1, "角色[%s]被你移出队伍.", CHAR_getUseName(to_id));
    CHAR_talkToCli(from_id, -1, buf1, CHAR_COLORYELLOW);
  }
}
#endif

#ifdef _CHATROOMPROTOCOL
void lssproto_CHATROOM_recv(int fd, char *data) { ChatRoom_recvall(fd, data); }
#endif

#ifdef _NEWREQUESTPROTOCOL // (���ɿ�) Syu ADD ����ProtocolҪ��ϸ��
void lssproto_RESIST_recv(int fd) {
  int charindex = -1;

  char token[256];
  charindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charindex))
    return;
  sprintf(token, "%d|%d|%d|%d|%d|%d|%d|%d",
          CHAR_getInt(charindex, CHAR_EARTH_RESIST),
          CHAR_getInt(charindex, CHAR_WATER_RESIST),
          CHAR_getInt(charindex, CHAR_FIRE_RESIST),
          CHAR_getInt(charindex, CHAR_WIND_RESIST),
          CHAR_getInt(charindex, CHAR_EARTH_EXP),
          CHAR_getInt(charindex, CHAR_WATER_EXP),
          CHAR_getInt(charindex, CHAR_FIRE_EXP),
          CHAR_getInt(charindex, CHAR_WIND_EXP));
  lssproto_RESIST_send(fd, token);
}
#endif

#ifdef _OUTOFBATTLESKILL // Syu ADD
void lssproto_BATTLESKILL_recv(int fd, int iNum) {
#ifdef _CHAR_PROFESSION // WON ADD
  int charaindex = CONNECT_getCharaindex(fd);
#ifndef _PROSKILL_OPTIMUM
  int skillindex = -1, char_pskill = -1, profession_skill = -1;
#endif

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

#ifndef _PROSKILL_OPTIMUM // Robin fix cancel �˴��Թ�ְҵ���, ����
                          // PROFESSION_SKILL_Use �м��
  char_pskill = CHAR_getInt(charaindex, PROFESSION_CLASS);
  skillindex = PROFESSION_SKILL_GetArray(charaindex, iNum);
  int Pskillid = skillindex;
  profession_skill =
      PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

  if ((char_pskill > 0) && (char_pskill == profession_skill)) {
#else
  if (1) {
#endif
    if (PROFESSION_SKILL_Use(charaindex, iNum, 0, NULL) != 1) {
      if (CHAR_getWorkInt(charaindex, CHAR_ENCOUNT_NUM) - (int)time(NULL) > 0) {
#ifdef _PROSKILL_ERR_KICK
        CONNECT_setCloseRequest(fd, 1);
#endif
      }
    }
  }
#endif
}

#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_recv(int fd, char *message) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(charaindex, CHAR_LOCKED) == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "Ϊ��ȷ�������Ʒ��ȫ����������İ�ȫ������н�����\n");
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    CHAR_talkToCli(charaindex, -1, "当前角色不属于任何帮派", CHAR_COLORYELLOW);
    return;
  }

  CHAR_sendStreetVendor(charaindex, message);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_recv(int fd, int type, char *data) {
  print("\n RCLICK_recv( type=%d data=%s) ", type, data);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_recv(int fd, char *data) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  CHAR_JobDaily(charaindex, data);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_recv(int fd, char *data) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  CHAR_Teacher_system(charaindex, data);
}
#endif

#ifdef _ASSESS_ABILITY
void lssproto_ASSESS_ABILITY_recv(int fd) {
  const int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  char data[512];
  char tmp[512];
  memset(data, 0, sizeof(data));
  int i;
  for (i = 0; i < 25; i++) {
    sprintf(tmp, "%d|", CHAR_getInt(charaindex, CHAR_MATERIAL01 + i));
    strcat(data, tmp);
  }

  lssproto_ASSESS_ABILITY_send(fd, data);
}
#endif

#ifdef _ONLINE_SHOP
void lssproto_VIP_SHOP_recv(int fd, int type, int page) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  OnlineShop_ShowList(fd, charaindex, type, page);
}

void lssproto_VIP_SHOP_buy_recv(int fd, int type, int page, int id, int num) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  OnlineShop_Buy(fd, charaindex, type, page, id, num);
}
#endif

void lssproto_SaMenu_recv(int fd, int index) {
  CHECKFDANDTIME;
  const int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  FreeSaMenu(charaindex, index);
}

#ifdef _FAMILYBADGE_
void lssproto_FamilyBadge_recv(int fd) {
  const int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  LoadFamilyBadge(fd);
}
#endif
