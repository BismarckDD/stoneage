#define __SAAC_CLIENT_SEND_C__
#include "gmsv_server.h"
#include "saac_client.h"
//
#include "char.h"
#include "chatroom.h"
#include "config_file.h"
#include "handletime.h"
#include "map_deal.h"
#include "msignal.h"
#include "net.h"
#include "npc_duelranking.h"
#include "npc_eventaction.h"
#include "npc_scheduleman.h"
#include "npc_warpman.h"
#include "object.h"
#include "readmap.h"
// CoolFish: Family 2001/5/24
#include "battle.h"
#include "buf.h"
#include "enemy.h"
#include "family.h"
#include "log.h"
#ifdef _ALLDOMAN
#include "npc_alldoman.h"
#endif

time_t initTime = 0;

extern int i_shutdown_time;

#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

void SaacClient_ACGmsvDownRequest_recv(int saac_fd, int min) {
  char buff[32];
  int i;
  int player_max_num = CHAR_getPlayerMaxNum();

  snprintf(buff, sizeof(buff), "%s即将关闭.", getGameservername());
  for (i = 0; i < player_max_num; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      CHAR_talkToCli(i, -1, buff, CHAR_COLORYELLOW);
    }
  }
  SERVSTATE_setLimittime(min);
  SERVSTATE_setShutdown(NowTime.tv_sec);
  SERVSTATE_setDsptime(0);
}

void SaacClient_ACServerLogin_recv(int saac_fd, char *result, char *data) {
  {
    if (strcmp(result, SUCCESSFUL) != 0) {
      print("Connect SAAC FAILED. result: %s, data: %s\n", result, data);
      shutdownProgram();
      exit(1);
    }
    print("Connect SAAC SUCCEED. result: %s, data: %s\n", result, data);
    time(&initTime);

    print("StoneAge version: Origin.\n");
    print("Visit Us: http://github.com\n");
    print("@Copyright Michael Franklin.\n");
    print("初始化家族数据......");
    FAMILY_Init();
    print("成功.\n");
#ifdef _ANGEL_SUMMON
    print("Start to init AC mission table...");
    SaacClient_ACMissionTable_send(saac_fd, -1, 1, "", "");
    print("succeed.\n");
#endif
  }
}

void SaacClient_ACCharList_recv(int saac_fd, char *result, char *data, int retfd) {
  int client_fd = getfdFromFdid(retfd);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
#ifdef _ALLBLUES_LUA_1_9
#ifdef _NEWCLISETMAC
  if (FreeLoginCheck(client_fd) == 0) {
    return;
  }
#endif
#endif
  GmsvServer_CharList_send(client_fd, result, data);
  CONNECT_setState(client_fd, NOTLOGIN);
}

void SaacClient_ACCharLoad_recv(int saac_fd, char *result, char *data,
                                int client_fdid,
                                int save_index) {
  const int client_fd = getfdFromFdid(client_fdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if ((strcmp(result, SUCCESSFUL) == 0) && (data[0])) {
    // print("Before CHAR_login, client_fd: %d, data: %s, save_index:%d\n",
    //   client_fd, data, save_index);
    NETWATCH_set("ACCharLoad.CHAR_login", client_fd, "ACCharLoad");
    CHAR_login(client_fd, data, save_index);
    NETWATCH_set("SAAC_dispatch", saac_fd, "ACCharLoad.complete");
  } else {
    char cdkey[64];
#ifndef _FIX_MESSAGE
    data = "";
#endif
    CONNECT_getCdkey(client_fd, cdkey, sizeof(cdkey));
    print("[SAAC回复]ACCharLoad: cdkey:%s, data:%s, result:%s\n.", cdkey, data, result);
    GmsvServer_CharLogin_send(client_fd, result, data);
    CONNECT_setState(client_fd, NOTLOGIN);
  }
}

void SaacClient_ACCharSave_recv(int saac_fd, char *result, char *data, int client_fdid) {
  const int client_fd = getfdFromFdid(client_fdid);
  char cdkey[CDKEYLEN], passwd[PASSWDLEN], charname[CHARNAMELEN];
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  CONNECT_getCdkey(client_fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(client_fd, passwd, sizeof(passwd));
  CONNECT_getCharname(client_fd, charname, sizeof(charname));
  const int fdid = CONNECT_getFdid(client_fd);
  switch (CONNECT_getState(client_fd)) {
  case WHILECREATE:
    if (strcmp(result, FAILED) == 0)
      data = "";
    GmsvServer_CreateNewChar_send(client_fd, result, data);
    CONNECT_setState(client_fd, NOTLOGIN);
    break;

  case WHILELOGOUTSAVE:
    if (strcmp(result, SUCCESSFUL) == 0)
      GmsvServer_CharLogout_send(client_fd, result, "success");
    else
      GmsvServer_CharLogout_send(client_fd, result, "Cannot save");
    CONNECT_setState(client_fd, NOTLOGIN);
    CONNECT_setCharaindex(client_fd, -1);
    break;

  case WHILELOSTCHARSAVE:
    SaacClient_ACCharDelete_send(saac_fd, cdkey, passwd, charname, "", fdid);
    CONNECT_setState(client_fd, WHILELOSTCHARDELETE);
    CONNECT_setCharaindex(client_fd, -1);
    // CONNECT_setCloseRequest( client_fd , 1 );
    break;

  case WHILECLOSEALLSOCKETSSAVE:
    CONNECT_setState(client_fd, NOTLOGIN);
    // Arminius debug
    // CONNECT_endOne_debug(client_fd);
    CONNECT_setUse(client_fd, FALSE);

    SERVSTATE_decrementCloseallsocketnum();
    if (SERVSTATE_getCloseallsocketnum() == 0) {
      SERVSTATE_SetAcceptMore(-1);
      print("Server is shutdown.\n");
      print("\nShutDown in Time=%d.\n", i_shutdown_time); // ttom
      if (i_shutdown_time == 1) {
        signalShutdown(0);
      }
    }
  default:
    break;
  }
}

void SaacClient_ACCharDelete_recv(int saac_fd, char *result, char *data, int retfd) {
  int client_fd = getfdFromFdid(retfd);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  switch (CONNECT_getState(client_fd)) {
  case WHILELOSTCHARDELETE:
    CONNECT_setState(client_fd, NOTLOGIN);
    CONNECT_setCharaindex(client_fd, -1);
    // CONNECT_setCloseRequest(client_fd, 1);
    break;
  case WHILECHARDELETE:
    if (strcmp(result, FAILED) == 0)
      data = "";
    GmsvServer_CharDelete_send(client_fd, result, data);
    CONNECT_setState(client_fd, NOTLOGIN);
    CONNECT_setCharaindex(client_fd, -1);
    // CONNECT_setCloseRequest(client_fd, 1);
    break;
  default:
    break;
  }
}

void SaacClient_ACLock_recv(int saac_fd, char *result, char *data, int retfd) {
  int client_fd = getfdFromFdid(retfd);
  char cdkey[CDKEYLEN];
  int cindex = getCharindexFromFdid(retfd);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  // Arminius 7.25 GM unlock test
  if (strncmp(data, "USRLOCKED", 9) == 0) {
    char buf[4096];
    sprintf(buf, "%s", &data[10]);
    CHAR_talkToCli(cindex, -1, buf, CHAR_COLORYELLOW);
    return;
  } else if (strncmp(data, "USRUNLOCKED", 11) == 0) {
    CHAR_talkToCli(cindex, -1, "User not locked.", CHAR_COLORYELLOW);
    return;
  } else if (strncmp(data, "GMUNLOCKSUCCESS", 15) == 0) {
    CHAR_talkToCli(cindex, -1, "User unlock success", CHAR_COLORYELLOW);
    return;
  } else if (strncmp(data, "GMUNLOCKFAIL", 12) == 0) {
    CHAR_talkToCli(cindex, -1, "User unlock failed", CHAR_COLORYELLOW);
    return;
  } else if (strncmp(data, "GMUNLOCKALL", 11) == 0) {
    CHAR_talkToCli(cindex, -1, "Server unlocked", CHAR_COLORYELLOW);
    return;
  }
  CONNECT_getCdkey(client_fd, cdkey, sizeof(cdkey));

  switch (CONNECT_getState(client_fd)) {
  case WHILECANNOTLOGIN:
    if (strcmp(result, SUCCESSFUL) == 0) {
      CONNECT_setState(client_fd, NOTLOGIN);

    } else {
      char mesg[128];
      snprintf(mesg, sizeof(mesg), "%s hasn't been locked", cdkey);
      if (strcmp(data, mesg) == 0) {
        CONNECT_setState(client_fd, NOTLOGIN);
      } else {
        SaacClient_ACLock_send(saac_fd, cdkey, UNLOCK, CONNECT_getFdid(client_fd));
      }
    }
    break;
  default:
    break;
  }
}

void SaacClient_ACUCheck_recv(int saac_fd, char *cd) {
  int conind;
  int flag = 0;
  conind = getfdFromCdkeyWithLogin(cd);
  flag = (conind >= 0) ? 1 : 0;
  SaacClient_ACUCheck_send(saac_fd, cd, flag);
}

void SaacClient_DBUpdateEntryInt_recv(int saac_fd, char *result, char *table,
                                      char *key, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBUpdateEntryInt\n");
    return;
  }
}

void SaacClient_DBUpdateEntryString_recv(int saac_fd, char *result, char *table,
                                         char *key, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBUpdateEntryString\n");
    return;
  }
}

void SaacClient_DBGetEntryRank_recv(int saac_fd, char *result, int rank, int count,
                                    char *table, char *key, int msgid,
                                    int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBGetEntryRank\n");
    return;
  }
  if (strcmp(table, DB_DUELPOINT) == 0) {
    if (CHAR_getInt(OBJECT_getIndex(msgid2), CHAR_WHICHTYPE) ==
        CHAR_TYPEDUELRANKING) {
      NPC_Duelranking_CallMyRanking(count, msgid, msgid2);
    } else {
      NPC_GetDuelRank(rank, msgid, msgid2);
    }
  }
}

void SaacClient_DBDeleteEntryInt_recv(int saac_fd, char *result, char *table,
                                      char *key, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBDeleteEntryInt\n");
    return;
  }
}

void SaacClient_DBDeleteEntryString_recv(int saac_fd, char *result, char *table,
                                         char *key, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBDeleteEntryString\n");
    return;
  }
}

void SaacClient_DBGetEntryInt_recv(int saac_fd, char *result, int value, char *table,
                                   char *key, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBGetEntryInt\n");
    return;
  }
}

void SaacClient_DBGetEntryString_recv(int saac_fd, char *result, char *value,
                                      char *table, char *key, int msgid,
                                      int msgid2) {
  char cdkey[32];
  char charaname[32];

  if (strcmp(table, DB_ADDRESSBOOK) == 0) {
    int mode = 1;
    if (strcmp(result, NET_STRING_FAILED) == 0) {
      mode = 0;
    }
    makeStringFromEscaped(key);
    if (!getStringFromIndexWithDelim(key, "_", 1, cdkey, sizeof(cdkey))) {
      return;
    }
    if (!getStringFromIndexWithDelim(key, "_", 2, charaname,
                                     sizeof(charaname))) {
      return;
    }
    ADDRESSBOOK_DispatchMessage(cdkey, charaname, value, mode);
  } else {
    if (strcmp(result, NET_STRING_FAILED) == 0) {
      print("failed: DBGetEntryString\n");
      return;
    }
  }
}

void SaacClient_DBGetEntryByRank_recv(int saac_fd, char *result, char *list,
                                      char *table, int msgid, int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBGetEntryByRank\n");
    return;
  }
}

void SaacClient_DBGetEntryByCount_recv(int saac_fd, char *result, char *list,
                                       char *table, int count_start, int msgid,
                                       int msgid2) {
  if (strcmp(result, NET_STRING_FAILED) == 0) {
    print("failed: DBGetEntryByCount\n");
    return;
  }
  if (strcmp(table, DB_DUELPOINT) == 0) {
    NPC_Duelranking_PrintRanking(list, count_start, msgid, msgid2);
  }
}
#ifdef _ALLDOMAN
void SaacClient_UpdataStele_recv(int saac_fd, char *token) {
  NPC_AlldomanWriteStele(token);
}
void SaacClient_S_UpdataStele_recv(int saac_fd, char *ocdkey, char *oname,
                                   char *ncdkey, char *nname, char *title,
                                   int level, int trns, int floor) {
  print("\nSyu log Single=> %s , %s , %s , %s ", ocdkey, oname, ncdkey, nname);
  NPC_Alldoman_S_WriteStele(ocdkey, oname, ncdkey, nname, title, level, trns,
                            floor);
}
#endif

void SaacClient_Broadcast_recv(const int saac_fd, const char *char_id,
                               const char *char_name, const char *message) {

  if (strstr(message, "online") == 0 || strstr(message, "offline") == 0 ||
      strstr(message, "param") == 0 || strstr(message, "chardelete") == 0) {
    char info[512];
    char escape_info[1024];
    snprintf(info, sizeof(info), "%s_%s", char_id, char_name);
    makeEscapeString(info, escape_info, sizeof(escape_info));
    SaacClient_DBGetEntryString_send(saac_fd, DB_ADDRESSBOOK, escape_info, 0, 0);
  }
}

void SaacClient_Message_recv(const int saac_fd, const char *id_from,
                             const char *char_name_from, const char *id_to,
                             const char *char_name_to, const char *message,
                             const int option, const int mesgid) {
  int ret = ADDRESSBOOK_sendMessage_FromOther(id_from, char_name_from, id_to,
                                              char_name_to, message, option);
  if (ret == 1) {
    SaacClient_MessageAck_send(saac_fd, id_to, char_name_to, SUCCESSFUL, mesgid);
  }
}

/* 收到从客户端发送创建family的请求 */
void SaacClient_ACAddFM_recv(int saac_fd, char *result, int family_index,
                             int char_fdid, int index) {
  int ret;
  const int client_fd = getfdFromFdid(char_fdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACAddFM(client_fd, ret, family_index, index);
}

/* 收到从客户端发送加入family的请求 */
void SaacClient_ACJoinFM_recv(int saac_fd, char *result, int recv, int charfdid) {
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACJoinFM(client_fd, ret, recv);
}

/* 收到从客户端发送离开family的请求 */
void SaacClient_ACLeaveFM_recv(int saac_fd, char *result, int resultflag,
                               int charfdid) {
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACLeaveFM(client_fd, ret, resultflag);
  print("ACLeaveFM_%d", ret);
}

void SaacClient_ACChangeFM_recv(int saac_fd, char *result, int charfdid) {}

/* Saac客户端收到服务端的发送删除family的请求 */
void SaacClient_ACDelFM_recv(int saac_fd, char *result, int charfdid) {
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACDelFM(client_fd, ret);
  print("ACDelFM_%d", ret);
}

void SaacClient_ACShowFMList_recv(int saac_fd, char *result, int fmnum, char *data) {
  int ret;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACShowFMList(ret, fmnum, data);
}

void SaacClient_ACShowMemberList_recv(int saac_fd, char *result, int index,
                                      int fmmemnum, int fmacceptflag,
                                      int fmjoinnum, char *data
#ifdef _FAMILYBADGE_
                                      ,
                                      int badge
#endif
) {
  int ret;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACShowMemberList(ret, index, fmmemnum, fmacceptflag, fmjoinnum, data
#ifdef _FAMILYBADGE_
                   ,
                   badge
#endif
  );
}

void SaacClient_ACFMDetail_recv(int saac_fd, char *result, char *data,
                                int charfdid) {
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACFMDetail(ret, data, client_fd);
}
void SaacClient_ACMemberJoinFM_recv(int saac_fd, char *result, char *data,
                                    int charfdid) {}
void SaacClient_ACMemberLeaveFM_recv(int saac_fd, char *result, char *data,
                                     int charfdid) {}
#ifdef _PERSONAL_FAME
void SaacClient_ACFMCharLogin_recv(int saac_fd, char *result, int index, int floor,
                                   int fmdp, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfame,
                                   int charfdid
#ifdef _NEW_MANOR_LAW
                                   ,
                                   int momentum
#endif
)
#else
void SaacClient_ACFMCharLogin_recv(int saac_fd, char *result, int index, int floor,
                                   int fmdp, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfdid)
#endif
{
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
#ifdef _PERSONAL_FAME // Arminius:
  ACFMCharLogin(client_fd, ret, index, floor, fmdp, joinflag, fmsetupflag, flag,
                charindex, charfame
#ifdef _NEW_MANOR_LAW
                ,
                momentum
#endif
  );
#else
  ACFMCharLogin(client_fd, ret, index, floor, fmdp, joinflag, fmsetupflag, flag,
                charindex);
#endif
}
void SaacClient_ACFMCharLogout_recv(int saac_fd, char *result, int charfdid) {}
void SaacClient_ACFMReadMemo_recv(int saac_fd, char *result, int index, int num,
                                  int dataindex, char *data) {
  int ret;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACShowFMMemo(ret, index, num, dataindex, data);
}
void SaacClient_ACFMWriteMemo_recv(int saac_fd, char *result, int index) {}
void SaacClient_ACFMPointList_recv(int saac_fd, char *result, char *data) {
  int ret;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;
  ACShowPointList(ret, data);
}

void SaacClient_ACSetFMPoint_recv(int saac_fd, char *result, int r, int charfdid) {
  int ret;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  if (strcmp(result, SUCCESSFUL) == 0) {
    ret = 1;
  } else {
    ret = 0;
  }
  ACSetFMPoint(ret, r, client_fd);
}
void SaacClient_ACFixFMPoint_recv(int saac_fd, char *result, int r) {}
void SaacClient_ACFMAnnounce_recv(int saac_fd, char *result, char *fmname,
                                  int fmindex, int index, int kindflag,
                                  char *data, int color) {
  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  ACFMAnnounce(ret, fmname, fmindex, index, kindflag, data, color);
}

void SaacClient_ACShowTopFMList_recv(int saac_fd, char *result, int kindflag,
                                     int num, char *data) {
  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  ACShowDpTop(ret, num, data, kindflag);
}
#ifdef _NEW_MANOR_LAW
extern struct FMS_DPTOP fmdptop;
#endif

void SaacClient_ACFixFMData_recv(int saac_fd, char *result, int kindflag,
                                 char *data1, char *data2, int charfdid) {
  int intdata;
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  int charaindex = CONNECT_getCharaindex(client_fd);

  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (kindflag == FM_FIX_FMRULE) {
    CHAR_talkToCli(charaindex, -1, "这样OK吗？",
                   CHAR_COLORWHITE);
  } else if (kindflag == FM_FIX_FMGOLD) {
    intdata = atoi(data1);
    if (ret == 0) {
      if (atoi(data1) > 0) {
        CHAR_setInt(charaindex, CHAR_GOLD,
                    CHAR_getInt(charaindex, CHAR_GOLD) + atoi(data1));
      }
      CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
      return;
    } else {
      if (atoi(data1) < 0) {
        CHAR_setInt(charaindex, CHAR_GOLD,
                    CHAR_getInt(charaindex, CHAR_GOLD) - atoi(data1));
        CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
      }
      LogStone(atoi(data2), CHAR_getChar(charaindex, CHAR_NAME),
               CHAR_getChar(charaindex, CHAR_CDKEY), -intdata,
               CHAR_getInt(charaindex, CHAR_GOLD), "FMBank(家族银行)",
               CHAR_getInt(charaindex, CHAR_FLOOR),
               CHAR_getInt(charaindex, CHAR_X),
               CHAR_getInt(charaindex, CHAR_Y));
    }
  } else if (kindflag == FM_FIX_FMLEADERCHANGE) {
    ACFMJob(client_fd, ret, data1, data2);
  }
  // CoolFish: 2001/10/03
  else if (kindflag == FM_FIX_FMADV || kindflag == FM_FIX_FMFEED ||
           kindflag == FM_FIX_FMSYNTHESIZE || kindflag == FM_FIX_FMDEALFOOD) {
    char tmpbuf[1024], flag[128];
#ifdef _NEW_MANOR_LAW
    int i;
#endif
    if (kindflag == FM_FIX_FMADV)
      strcpy(flag, "ADV");
    else if (kindflag == FM_FIX_FMFEED)
      strcpy(flag, "FEED");
    else if (kindflag == FM_FIX_FMSYNTHESIZE)
      strcpy(flag, "SYN");
    else if (kindflag == FM_FIX_FMDEALFOOD)
      strcpy(flag, "FOOD");
    sprintf(tmpbuf, "%s::%d %s %s", flag, CHAR_getInt(charaindex, CHAR_FMINDEX),
            CHAR_getChar(charaindex, CHAR_FMNAME), data1);
    LogFMPOP(tmpbuf);
#ifdef _NEW_MANOR_LAW
    for (i = 0; i < FAMILY_MAXNUM; i++) {
      if (fmdptop.fmtopid[i] == CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI))
        break;
    }
    if (i < FAMILY_MAXNUM) {
      sprintf(tmpbuf, "MOMENTUM::%d %s %d",
              CHAR_getInt(charaindex, CHAR_FMINDEX),
              CHAR_getChar(charaindex, CHAR_FMNAME), fmdptop.fmMomentum[i]);
      LogFMPOP(tmpbuf);
    }
#endif
  }
}
void SaacClient_ACFixFMPK_recv(int saac_fd, char *result, int data, int winindex,
                               int loseindex) {
  int ret;
  if (strcmp(result, SUCCESSFUL) == 0)
    ret = 1;
  else
    ret = 0;

  //        print("FixFMPK_recv result:%s data:%d winindex:%d loseindex:%d\n",
  //        result, data, winindex, loseindex);
  if (ret == 1) {
    char tmpbuf[256];
    ACFixFMPK(winindex, loseindex, data);
    sprintf(tmpbuf, "FMPK(家族PK)::win(优胜家族):%d lose(落败家族):%d point:%d",
            winindex, loseindex, data);
    // print("FixFMPK tmpbuf:%s\n", tmpbuf);
    LogFMPOP(tmpbuf);
  }
}
void SaacClient_ACGMFixFMData_recv(int saac_fd, char *result, char *fmname,
                                   int charfdid) {
  int client_fd = getfdFromFdid(charfdid);
  char buf[256];
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  print("GMFixFMData_recv result:%s\n", result);
  if (ret == 1) {
    sprintf(buf, "修改%s家族数据成功.", fmname);
    CHAR_talkToCli(CONNECT_getCharaindex(client_fd), -1, buf, CHAR_COLORWHITE);
  } else {
    sprintf(buf, "修改%s家族数据失败.", fmname);
    CHAR_talkToCli(CONNECT_getCharaindex(client_fd), -1, buf, CHAR_COLORWHITE);
  }
}

extern int familyTax[];
void SaacClient_ACGetFMData_recv(int saac_fd, char *result, int kindflag, int data,
                                 int charfdid) {
  char buf[256];
  int client_fd = getfdFromFdid(charfdid);
  if (CONNECT_checkfd(client_fd) == FALSE)
    return;
  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  if (ret == 0)
    return;
  if (kindflag == 1) {
    int fmindex =
        CHAR_getWorkInt(CONNECT_getCharaindex(client_fd), CHAR_WORKFMINDEXI);
    if (fmindex < 0 || fmindex >= FAMILY_MAXNUM) {
      print(" fmindex: %d 异常!!\n", fmindex);
      return;
    }
    familyTax[fmindex] = data;
    sprintf(buf, "B|T|%d", data);
    GmsvServer_FM_send(client_fd, buf);
  }
}
void SaacClient_ACFMClearPK_recv(int saac_fd, char *result, char *fmname,
                                 int fmindex, int index) {
  int i = 0;

  int ret = (strcmp(result, SUCCESSFUL) == 0) ? 1 : 0;
  for (i = 0; i < MAX_SCHEDULEMAN * MAX_SCHEDULE; i++) {
    if (fmpks[i].flag == -1)
      continue;
    if (fmpks[i].host_index == index || fmpks[i].guest_index == index) {
      fmpks[i].host_index = -1;
      strcpy(fmpks[i].host_name, "");
      fmpks[i].guest_index = -1;
      strcpy(fmpks[i].guest_name, "");
      fmpks[i].prepare_time = -1;
      fmpks[i].max_player = -1;
      fmpks[i].win = -1;
      fmpks[i].flag = -1;
    }
  }
}

#ifdef _ACFMPK_LIST
void SaacClient_ACSendFmPk_recv(int saac_fd, int to_index, int flg) {
  char buf[256];
  if (flg == 0) {
    sprintf(buf, "家族PK.");
    CHAR_talkToCli(to_index, -1, buf, CHAR_COLORRED);
  } else {
    sprintf(buf, "%s 将获得挑战庄园资格",
            CHAR_getChar(to_index, CHAR_FMNAME));
    int i;
    for (i = 0; i < CHAR_getPlayerMaxNum(); i++) {
      if (CHAR_getCharUse(i) != FALSE) {
        CHAR_talkToCli(i, -1, buf, CHAR_COLORBLUE2);
      }
    }
    sprintf(buf, "家族PK.");
    CHAR_talkToCli(to_index, -1, buf, CHAR_COLORYELLOW);
  }
}

void SaacClient_ACLoadFmPk_recv(int saac_fd, char *data) {
  char buf[10], fm_pk_data[128];
  char token[256], skip[256];
  int fm_pk_num, fmpks_pos = -1;
  easyGetTokenFromString(data, 1, buf, sizeof(buf));
  fm_pk_num = atoi(buf);
  sprintf(fm_pk_data, "%s", data + 2);
  if (((fmpks_pos = fm_pk_num * MAX_SCHEDULE) == -1) ||
      (fm_pk_num > FMPOINTNUM)) {
    print("\n err fm_pk_num(%d)", fm_pk_num);
    return;
  }
  //
  if (getStringFromIndexWithDelim(data, "|", 2, token, sizeof(token))) {
    fmpks[fmpks_pos + 1].dueltime = atoi(token);
  }
  // 主队 familyindex
  if (getStringFromIndexWithDelim(data, "|", 3, token, sizeof(token))) {
    fmpks[fmpks_pos + 1].host_index = atoi(token);
  }
  // 主队 家族名
  if (getStringFromIndexWithDelim(data, "|", 4, token, sizeof(token))) {
    strcpy(fmpks[fmpks_pos + 1].host_name, makeStringFromEscaped(token));
  }
  // No.5 family_index
  if (getStringFromIndexWithDelim(data, "|", 5, token, sizeof(token))) {
    fmpks[fmpks_pos + 1].guest_index = atoi(token);
  }
  // 客队 家族名
  if (getStringFromIndexWithDelim(data, "|", 6, token, sizeof(token))) {
    strcpy(fmpks[fmpks_pos + 1].guest_name, makeStringFromEscaped(token));
  }
  // 准备时间
  if (getStringFromIndexWithDelim(data, "|", 7, token, sizeof(token))) {
    fmpks[fmpks_pos + 1].prepare_time = atoi(token);
  }
  // 精灵管理
  if (getStringFromIndexWithDelim(data, "|", 8, token, sizeof(token))) {
    int maxnum = atoi(token);
    if (maxnum < 50)
      maxnum = 50;
    fmpks[fmpks_pos + 1].max_player = maxnum;
  }
  // 管理
  if (getStringFromIndexWithDelim(data, "|", 9, token, sizeof(token))) {
    fmpks[fmpks_pos + 1].flag = atoi(token);
  }
  if (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_OTHERPLANET) {
    // 对战星球
    if (getStringFromIndexWithDelim(data, "|", 10, token, sizeof(token))) {
      strcpy(fmpks[fmpks_pos + 2].host_name, makeStringFromEscaped(token));
      strcpy(skip, makeStringFromEscaped(token));
      if (strcmp(getGameservername(), skip) == 0) {
        fmpks[fmpks_pos + 1].flag = 8;
      } else if (fmpks[fmpks_pos + 1].flag == 8) {
        fmpks[fmpks_pos + 1].flag = 10;
      }
    }
  } else {
    strcpy(fmpks[fmpks_pos + 2].host_name, "");
  }
}
#endif

void SaacClient_ACManorPKAck_recv(int saac_fd, char *data) {
  // 精灵管理 server 与 庄园 pk scheduleman
  int i;
  char token[256], skip[256];
  int char_max = CHAR_getCharNum();

  print("ACManorPKAck_recv: %s\n", data);

  for (i = 0; i < char_max; i++) {
    if (CHAR_getCharUse(i)) {
      if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEMANORSCHEDULEMAN) {
        if (getStringFromIndexWithDelim(data, "|", 1, token, sizeof(token))) {
          if (CHAR_getWorkInt(i, CHAR_NPCWORKINT2) == atoi(token)) { // manor id
            int fmpks_pos = CHAR_getWorkInt(i, CHAR_NPCWORKINT1) * MAX_SCHEDULE;

            // 对战星球
            if (getStringFromIndexWithDelim(data, "|", 10, token,
                                            sizeof(token))) {
              strcpy(skip, makeStringFromEscaped(token));
              print("ACManorPKAck: %d %s\n", fmpks_pos, skip);

              if (strcmp(getGameservername(), skip) != 0) {
                strcpy(fmpks[fmpks_pos + 2].host_name, skip);

                // 时间
                if (getStringFromIndexWithDelim(data, "|", 2, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].dueltime = atoi(token);
                }
                // 主队 familyindex
                if (getStringFromIndexWithDelim(data, "|", 3, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].host_index = atoi(token);
                }
                // 主队 家族名
                if (getStringFromIndexWithDelim(data, "|", 4, token,
                                                sizeof(token))) {
                  strcpy(fmpks[fmpks_pos + 1].host_name,
                         makeStringFromEscaped(token));
                }
                // 客队 familyindex
                if (getStringFromIndexWithDelim(data, "|", 5, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].guest_index = atoi(token);
                }
                // 客队 家族名
                if (getStringFromIndexWithDelim(data, "|", 6, token,
                                                sizeof(token))) {
                  strcpy(fmpks[fmpks_pos + 1].guest_name,
                         makeStringFromEscaped(token));
                }
                // 准备时间
                if (getStringFromIndexWithDelim(data, "|", 7, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].prepare_time = atoi(token);
                }
                // 精灵管理
                if (getStringFromIndexWithDelim(data, "|", 8, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].max_player = atoi(token);
                }
                // 管理
                if (getStringFromIndexWithDelim(data, "|", 9, token,
                                                sizeof(token))) {
                  fmpks[fmpks_pos + 1].flag = atoi(token);
                }
                // 对战星球
                if (getStringFromIndexWithDelim(data, "|", 10, token,
                                                sizeof(token))) {
                  strcpy(fmpks[fmpks_pos + 2].host_name,
                         makeStringFromEscaped(token));
                }
              }
            }
          }
        }
      }
    }
  }
}

#ifdef _WAEI_KICK
void SaacClient_ACKick_recv(int saac_fd, int act, char *data, int retfd) {
  int client_fd = getfdFromFdid(retfd);
  // if( CONNECT_checkfd(client_fd) == FALSE )return;
  // char cdkey[CDKEYLEN];
  int cindex = getCharindexFromFdid(retfd);
  switch (act) {
  case 0: // FAIL
    if (CONNECT_checkfd(client_fd) == FALSE)
      return;
    CHAR_talkToCli(cindex, -1, data, CHAR_COLORYELLOW);
    break;
  case 1: //
  {
    char szName[64];
    int i, MAX_USER = 0;
    BOOL find = FALSE;
    {
      int i;
      int playernum = CHAR_getPlayerMaxNum();
      for (i = 0; i < playernum; i++) {
        if (!CHAR_CHECKINDEX(i))
          continue;
        if (!strcmp(CHAR_getChar(i, CHAR_CDKEY), data)) {
#ifdef _OFFLINE_SYSTEM
          if (CHAR_getWorkInt(i, CHAR_WORK_OFFLINE) != 0) {
            if (!CHAR_logout(i, TRUE)) {
              print("err %s:%d\n", __FILE__, __LINE__);
            }
          } else
#endif
          {
            CHAR_talkToCli(i, -1, "因重复登陆而掉线!", CHAR_COLORYELLOW);

#ifdef _NETLOG_
            char cdkey[16];
            char charname[32];
            CONNECT_getCharname(CHAR_getWorkInt(i, CHAR_WORKFD), charname, 32);
            CONNECT_getCdkey(CHAR_getWorkInt(i, CHAR_WORKFD), cdkey, 16);
            LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__,
                       "封包异常而断线");
#endif
            CONNECT_setCloseRequest(getfdFromCharaIndex(i), 1);
          }
        }
      }
    }
    if (find == TRUE && retfd != -1 && CHAR_CHECKINDEX(cindex)) {
      char buf1[256]; //, buf2[256];
      sprintf(buf1, "系统公告[%s]因重复登陆而断线",
              /*CHAR_getChar( cindex, CHAR_NAME),*/ szName);
      for (i = 0; i < MAX_USER; i++) {
        if (CONNECT_getUse(i)) {
          int fd_charaindex = CONNECT_getCharaindex(i);
          if (!CHAR_CHECKINDEX(fd_charaindex))
            continue;
          CHAR_talkToCli(fd_charaindex, -1, buf1, CHAR_COLORYELLOW);
          //						CHAR_talkToCli(fd_charaindex,
          //-1, buf2, CHAR_COLORYELLOW);
        }
      }
    }
  } break;
  case 2:
    print("\n CASE 2");
    break;
  }
}

#endif

#ifdef _CHAR_POOLITEM
void SaacClient_ACCharSavePoolItem_recv(int saac_fd, char *result, char *data,
                                        int retfd) {
  int charaindex = getCharindexFromFdid(retfd);
  // print("\n ACCharSavePoolItem_recv:%s ", data);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (strstr(result, FAILED) != NULL) {
    CHAR_talkToCli(charaindex, -1, "道具仓库资料存档失败！", CHAR_COLORYELLOW);
    return;
  }
}

void SaacClient_ACCharGetPoolItem_recv(int saac_fd, char *result, char *data,
                                       int retfd, int meindex) {
#ifdef _NPC_DEPOTITEM
  Char *ch = NULL;
  int i, client_fd, charaindex;

  // print("\n ACCharGetPoolItem_recv:%s ", data);

  if (strcmp(result, SUCCESSFUL) != 0)
    return;
  charaindex = getCharindexFromFdid(retfd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  client_fd = getfdFromCharaIndex(charaindex);
  if (CHAR_CheckDepotItem(charaindex))
    return; // 仓库已处理

  if ((ch = CHAR_getCharPointer(charaindex)) == NULL)
    return;
  ch->indexOfExistDepotItems =
      allocateMemory(sizeof(int) * CHAR_MAXDEPOTITEMHAVE);
  if (ch->indexOfExistDepotItems == NULL)
    return;
  for (i = 0; i < CHAR_MAXDEPOTITEMHAVE; i++)
    ch->indexOfExistDepotItems[i] = -1;

  if (CHAR_makeDepotItemStringToChar(charaindex, data) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "道具仓库资料读取失败！", CHAR_COLORYELLOW);
    CHAR_removeDepotItem(charaindex);
    return;
  }
  if (!CHAR_CHECKINDEX(meindex))
    return;
  if (client_fd != -1) {
    char message[1024];
    char buf[1024];
    strcpy(message, "3\n\n"
                    "          使用道具仓库\n\n"
                    "          ＜存放道具＞\n"
                    "          ＜取回道具＞\n");
    GmsvServer_WN_send(client_fd, WINDOW_MESSAGETYPE_SELECT,
                       WINDOW_BUTTONTYPE_CANCEL,
                       311, // CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE,
                       CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
                       makeEscapeString(message, buf, sizeof(buf)));
  }
#endif
}

#endif //

#ifdef _CHAR_POOLPET
void SaacClient_ACCharSavePoolPet_recv(int saac_fd, char *result, char *data,
                                       int retfd) {
  int charaindex = getCharindexFromFdid(retfd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (strstr(result, FAILED) != NULL) {
    CHAR_talkToCli(charaindex, -1, "宠物仓库资料存档失败！", CHAR_COLORYELLOW);
    return;
  }
}

void SaacClient_ACCharGetPoolPet_recv(int saac_fd, char *result, char *data,
                                      int retfd, int meindex) {
#ifdef _NPC_DEPOTPET
  Char *ch = NULL;
  int i, client_fd, charaindex;

  if (strcmp(result, SUCCESSFUL) != 0)
    return;
  charaindex = getCharindexFromFdid(retfd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  client_fd = getfdFromCharaIndex(charaindex);
  if (CHAR_CheckDepotPet(charaindex))
    return; // 仓库已处理

  if ((ch = CHAR_getCharPointer(charaindex)) == NULL)
    return;
  ch->indexOfExistDepotPets =
      allocateMemory(sizeof(int) * CHAR_MAXDEPOTPETHAVE);
  if (ch->indexOfExistDepotPets == NULL)
    return;
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++)
    ch->indexOfExistDepotPets[i] = -1;

  if (CHAR_makeDepotPetStringToChar(charaindex, data) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "宠物仓库资料读取失败！", CHAR_COLORYELLOW);
    CHAR_removeDepotPet(charaindex);
    return;
  }
  if (!CHAR_CHECKINDEX(meindex))
    return;
  if (client_fd != -1) {
    char message[1024];
    char buf[1024];
    strcpy(
        message,
        "3\n"
        "              使用宠物仓库\n"
        "◆特别注意：请勿将贵重宠物存入公共仓库\n"
        "　　　　　　『宠物公共仓库』\n"
        "          ＜＜＜存放宠物＞＞＞\n"
        "          ＜＜＜取回宠物＞＞＞\n");
    GmsvServer_WN_send(client_fd, WINDOW_MESSAGETYPE_SELECT,
                       WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE,
                       CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
                       makeEscapeString(message, buf, sizeof(buf)));
  }
#endif
}

#endif

#ifdef _ANGEL_SUMMON
// void SaacClient_ACMissionTable_recv( int fd, int num, int type, char *data,
// int charaindex)
void SaacClient_ACMissionTable_recv(int saac_fd, int num, int type, char *data,
                                    char *angelinfo) {
  char msg[1024];

  if (type == 1) { // get data list
    // if( num == -1) // get all data
    {
      int di, ti;
      char onedata[1024], token[1024];

      if (num == -1) {
        print("精灵任务资料回收完成!!\n");
        memset(missiontable, 0, sizeof(missiontable));
      } else
        print("取得一笔资料!!:%s\n", data);

      for (di = 1; di < MAXMISSIONTABLE; di++) {
        if (!getStringFromIndexWithDelim(data, " ", di, onedata,
                                         sizeof(onedata)))
          break;
        if (onedata[0] == '\0' || onedata[0] == ' ')
          break;
        print("资料%d:%s\n", di, onedata);
        getStringFromIndexWithDelim(onedata, "|", 1, token, sizeof(token));
        ti = atoi(token);
        if (ti < 0 || ti >= MAXMISSIONTABLE)
          continue;
        getStringFromIndexWithDelim(onedata, "|", 2, token, sizeof(token));
        strcpy(missiontable[ti].angelinfo, token);
        getStringFromIndexWithDelim(onedata, "|", 3, token, sizeof(token));
        strcpy(missiontable[ti].heroinfo, token);
        getStringFromIndexWithDelim(onedata, "|", 4, token, sizeof(token));
        missiontable[ti].mission = atoi(token);
        getStringFromIndexWithDelim(onedata, "|", 5, token, sizeof(token));
        missiontable[ti].flag = atoi(token);
        getStringFromIndexWithDelim(onedata, "|", 6, token, sizeof(token));
        missiontable[ti].time = atoi(token);
        getStringFromIndexWithDelim(onedata, "|", 7, token, sizeof(token));
        missiontable[ti].limittime = atoi(token);
      }
    }
  } else if (type == 2) { // add data return
    int angelindex = -1, i, maxChar;
    char nameinfo[128], msgbuf[1024];

    if (num == -1) {
      sprintf(msg, "币提货卡\n");
      print(msg);
      LogAngel(msg);
      return;
    }

    sprintf(msgbuf, "收到AC回应(add data) num=%d nameinfo=%s ", num, angelinfo);
    print(msgbuf);
    LogAngel(msgbuf);

    maxChar = CHAR_getPlayerMaxNum();
    for (i = 0; i < maxChar; i++) {
      if (!CHAR_CHECKINDEX(i))
        continue;
      getMissionNameInfo(i, nameinfo);
      if (strcmp(nameinfo, angelinfo))
        continue;

      angelindex = i;
      break;
    }

    if (angelindex == -1) {
      print("使用者不在\n");
      return;
    }

    GmsvServer_WN_send(
        getfdFromCharaIndex(angelindex), WINDOW_MESSAGETYPE_ANGELMESSAGE,
        WINDOW_BUTTONTYPE_YESNO, CHAR_WINDOWTYPE_ANGEL_ASK, -1,
        "目前魔界之门正遭受攻击，精灵需要你的帮忙，请前往寻找并驱除这些魔物，你是否愿意帮忙？");

    CHAR_talkToCli(angelindex, -1, "精灵正在召唤你。", CHAR_COLORYELLOW);
  } else if (type == 3) {
    if (num < 0 || num >= MAXMISSIONTABLE)
      return;

    sprintf(msg, " AC值 对象:%d %s %s ", num, missiontable[num].angelinfo,
            missiontable[num].heroinfo);
    print(msg);
    LogAngel(msg);

    strcpy(missiontable[num].angelinfo, "");
    strcpy(missiontable[num].heroinfo, "");
    missiontable[num].mission = 0;
    missiontable[num].flag = 0;
    missiontable[num].time = 0;
    missiontable[num].limittime = 0;
  }
}

#endif

#ifdef _TEACHER_SYSTEM
void SaacClient_ACCheckCharacterOnLine_recv(int saac_fd, int charaindex,
                                            int iOnline, char *data, int flag) {
  switch (flag) {
  case R_F_TEACHER_SYSTEM:
    CHAR_Teacher_system_View(charaindex, iOnline, data);
    break;
  default:
    printf("SaacClient_ACCheckCharacterOnLine_recv(): error flag type!!(%d)\n",
           flag);
  }
}
#endif

// saac_fd: saac端的sock fd
// client_fd: client端的sock fd
// client_fdid: 自增，永不重复
// 为什么要使用client_fdid：client可能会断联, 如果使用client_fd, 可能会发错
void SaacClient_ACCharLogin_recv(int saac_fd, int client_fdid, int flag) {
  const int client_fd = getfdFromFdid(client_fdid);
  printf("[GMSV收到验证结果] saac_fd=%d client_fdid=%d client_fd=%d flag=%d\n",
         saac_fd, client_fdid, client_fd, flag);

  if (CONNECT_checkfd(client_fd) == FALSE ||
      CONNECT_getState(client_fd) != WHILEAUTH) {
    printf("[GMSV验证结果] 连接无效或状态错误 client_fd=%d\n", client_fd);
    return;
  }

  if (flag == 0) {
    printf("[登录成功] 发送 'ok' 给客户端 fd=%d\n", client_fd);
    CONNECT_setState(client_fd, NOTLOGIN);
    GmsvServer_ClientLogin_send(client_fd, "ok");
  } else {
    printf("[登录失败] 发送 'no' 给客户端 fd=%d flag=%d\n", client_fd, flag);
    /* Failed authentication must not enter character-select operations. */
    CONNECT_setState(client_fd, NULLCONNECT);
    GmsvServer_ClientLogin_send(client_fd, "no");
  }
}

#ifdef _NEW_VIP_SHOP
// What's query point?
void SaacClient_QueryPoint_recv(int client_fd, int point) {
  const int char_index = CONNECT_getCharaindex(client_fd);
  char token[64];
  sprintf(token, "你目前的个人重回币数为：%d", point);
  CHAR_talkToCli(char_index, -1, token, CHAR_COLORYELLOW);
}

void SaacClient_NewVipShop_recv(int client_fd, int point, char *buf, int flag) {
  int charaindex = CONNECT_getCharaindex(client_fd);
  if (point == -1) {
    CHAR_talkToCli(charaindex, -1, "你的重回币不足！", CHAR_COLORYELLOW);
    return;
  }
  if (strcmp(buf, "") == 0) {
    CHAR_talkToCli(charaindex, -1, "没有该项物品数据！", CHAR_COLORYELLOW);
    return;
  }
  char token[256];
  if (flag == 0) {
    int itemID = -1, itemindex = -1;
    char buff[256];
    int ret;

    if (getStringFromIndexWithDelim(buf, ",", 1, buff, sizeof(buff)) != FALSE)
      itemID = atoi(buf);

    itemindex = CHAR_findEmptyItemBox(charaindex);
    if (itemindex < 0) {
      CHAR_talkToCli(charaindex, -1, "物品栏空间不足！！", CHAR_COLORYELLOW);
      return;
    }

    itemindex = ITEM_makeItemAndRegist(itemID);
    if (itemindex != -1) {
      ret = CHAR_addItemSpecificItemIndex(charaindex, itemindex);
      if (ret < 0 || ret >= CheckCharMaxItem(charaindex)) {
        ITEM_endExistItemsOne(itemindex);
        print("\n ret error!!");
        return;
      }

      sprintf(token, "拿到%s", ITEM_getChar(itemindex, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

      CHAR_sendItemDataOne(charaindex, ret);
    }

#ifdef _SQL_VIPPOINT_LOG
    LogSqlVipPoint(CHAR_getChar(charaindex, CHAR_NAME),
                   CHAR_getChar(charaindex, CHAR_CDKEY), "(购宠剩点)", point,
                   CHAR_getInt(charaindex, CHAR_FLOOR),
                   CHAR_getInt(charaindex, CHAR_X),
                   CHAR_getInt(charaindex, CHAR_Y));
#endif

#ifdef _SAMETHING_SAVEPOINT
    if (CHAR_charSaveFromConnect(charaindex, FALSE)) {
      CHAR_talkToCli(charaindex, -1, "系统自动为您存档!", CHAR_COLORRED);
    }
#endif

  } else {
    int ret;
    char buf1[8];
    int enemynum;
    int i, j;
    int petindex, petindex2, petid;

    if (getStringFromIndexWithDelim(buf, ",", 1, buf1, sizeof(buf1)) == FALSE)
      return;
#ifdef _PET_MM
    if (strstr(buf1, "MM1") || strstr(buf1, "MM3"))
      petid = 1479;
    else if (strstr(buf1, "MM2") || strstr(buf1, "MM4"))
      petid = 2547;
    else
      petid = atoi(buf1);
#else
    petid = atoi(buf1);
#endif

#ifdef _NPC_GET_PETABI
    {
      char buf1[32];
      easyGetTokenFromString(buf, 1, buf1, sizeof(buf1));
      if (strstr(buf1, "PETABI")) {
        easyGetTokenFromString(buf, 2, buf1, sizeof(buf1));
        petid = atoi(buf1);
      }
    }
#endif

    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      petindex = CHAR_getCharPet(charaindex, i);
      if (petindex == -1)
        break;
    }

    if (i == CHAR_MAXPETHAVE) {
      snprintf(token, sizeof(token), "宠物栏已满！");
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
      return;
    }

    enemynum = ENEMY_getEnemyNum();
    for (i = 0; i < enemynum; i++) {
      if (ENEMY_getInt(i, ENEMY_ID) == petid) {
        break;
      }
    }

    if (i == enemynum)
      return;

    ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      if (CHAR_getCharPet(charaindex, i) == ret)
        break;
    }

    if (i == CHAR_MAXPETHAVE)
      CHAR_talkToCli(charaindex, -1, "宠物数量已经达到最大值",
                     CHAR_COLORYELLOW);

    if (CHAR_CHECKINDEX(ret) == TRUE) {
#ifdef _PET_MM
      if (strstr(buf1, "MM1") || strstr(buf1, "MM2")) {
        int LevelUpPoint;
        LevelUpPoint = (50 << 24) + (50 << 16) + (50 << 8) + (50 << 0);
        CHAR_setInt(ret, CHAR_ALLOCPOINT, LevelUpPoint);
      } else if (strstr(buf1, "MM3") || strstr(buf1, "MM4")) {
        while (CHAR_getInt(ret, CHAR_LV) < 79) {
          int LevelUpPoint;
          LevelUpPoint = (50 << 24) + (50 << 16) + (50 << 8) + (50 << 0);
          CHAR_setInt(ret, CHAR_ALLOCPOINT, LevelUpPoint);
          CHAR_PetLevelUp(ret);
          CHAR_PetAddVariableAi(ret, AI_FIX_PETLEVELUP);
          CHAR_setInt(ret, CHAR_LV, CHAR_getInt(ret, CHAR_LV) + 1);
        }
      }
#endif

#ifdef _NPC_GET_PETABI
      {
        char buf1[32];
        easyGetTokenFromString(buf, 1, buf1, sizeof(buf1));
        if (strstr(buf, "PETABI")) {
          int lv, trans, vital, str, tough, dex;
          easyGetTokenFromString(buf, 3, buf1, sizeof(buf1));
          lv = atoi(buf1);
          easyGetTokenFromString(buf, 4, buf1, sizeof(buf1));
          trans = atoi(buf1);
          easyGetTokenFromString(buf, 5, buf1, sizeof(buf1));
          vital = atoi(buf1);
          easyGetTokenFromString(buf, 6, buf1, sizeof(buf1));
          str = atoi(buf1);
          easyGetTokenFromString(buf, 7, buf1, sizeof(buf1));
          tough = atoi(buf1);
          easyGetTokenFromString(buf, 8, buf1, sizeof(buf1));
          dex = atoi(buf1);
          CHAR_setInt(ret, CHAR_LV, lv);
          CHAR_setInt(ret, CHAR_TRANSMIGRATION, trans);
          CHAR_setInt(ret, CHAR_VITAL, vital * 20);
          CHAR_setInt(ret, CHAR_STR, str * 80);
          CHAR_setInt(ret, CHAR_TOUGH, tough * 80);
          CHAR_setInt(ret, CHAR_DEX, dex * 100);
          CHAR_setMaxExpFromLevel(ret, CHAR_getInt(ret, CHAR_LV));
          CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
        }
      }
#endif

      CHAR_setInt(ret, CHAR_HP, CHAR_getWorkInt(ret, CHAR_WORKMAXHP));
      CHAR_setMaxExpFromLevel(ret, CHAR_getInt(ret, CHAR_LV));
      CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
    }

    petindex2 = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(petindex2))
      return;
    snprintf(token, sizeof(token), "拿到%s。",
             CHAR_getChar(petindex2, CHAR_NAME));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    for (j = 0; j < CHAR_MAXPETHAVE; j++) {
      petindex = CHAR_getCharPet(charaindex, j);
      if (!CHAR_CHECKINDEX(petindex))
        continue;
      CHAR_complianceParameter(petindex);
      snprintf(token, sizeof(token), "K%d", j);
      CHAR_sendStatusString(charaindex, token);
      snprintf(token, sizeof(token), "W%d", j);
      CHAR_sendStatusString(charaindex, token);
    }

#ifdef _SQL_VIPPOINT_LOG
    LogSqlVipPoint(CHAR_getChar(charaindex, CHAR_NAME),
                   CHAR_getChar(charaindex, CHAR_CDKEY), "(购宠剩点)", point,
                   CHAR_getInt(charaindex, CHAR_FLOOR),
                   CHAR_getInt(charaindex, CHAR_X),
                   CHAR_getInt(charaindex, CHAR_Y));
#endif
#ifdef _SAMETHING_SAVEPOINT
    if (CHAR_charSaveFromConnect(charaindex, FALSE)) {
      CHAR_talkToCli(charaindex, -1, "系统自动为您存档!", CHAR_COLORRED);
    }
#endif
  }

  snprintf(token, sizeof(token), "您当前重回币剩余 %d", point);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
}

#endif

#ifdef _ITEM_PET_LOCKED
void SaacClient_ItemPetLocked_recv(int client_fd, int flag, char *data) {
  int charaindex = CONNECT_getCharaindex(client_fd);
  if (flag == 1) {
    CHAR_setInt(charaindex, CHAR_LOCKED, 0);
    CHAR_talkToCli(charaindex, -1, data, CHAR_COLORYELLOW);
  } else if (flag == 0) {
    char buf[256];
    GmsvServer_WN_send(client_fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                       WINDOW_BUTTONTYPE_OKCANCEL,
                       CHAR_WINDOWTYPE_ITEM_PET_LOCKED_PASSWD, -1,
                       makeEscapeString(data, buf, sizeof(buf)));
  } else if (flag == -1) {
    CHAR_talkToCli(charaindex, -1, data, CHAR_COLORRED);
  }
}

void SaacClient_ItemPetLockedPasswd_recv(int client_fd, char *data) {
  char buf[256];
  GmsvServer_WN_send(client_fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK, -1,
                     -1, makeEscapeString(data, buf, sizeof(buf)));
}
#endif

#ifdef _ONLINE_COST
void SaacClient_OnlineCost_recv(int client_fd, char *data) {
  int charaindex = CONNECT_getCharaindex(client_fd);

#ifdef _SQL_VIPPOINT_LOG
  LogSqlVipPoint(
      CHAR_getChar(charaindex, CHAR_NAME), CHAR_getChar(charaindex, CHAR_CDKEY),
      data, 0, CHAR_getInt(charaindex, CHAR_FLOOR),
      CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y));
#endif

  CHAR_talkToCli(charaindex, -1, data, CHAR_COLORGREEN);
}
#endif

#ifdef _SQL_BUY_FUNC
void SaacClient_OnlineBuy_recv(int client_fd, char *data) {
  char buf[64];
  int charaindex = CONNECT_getCharaindex(client_fd);
  char token[256];
  if (data[0] == '0') {
    int ret;
    int enemynum;
    int enemyid;
    int i;
    int vital, str, tough, dex;
    getStringFromIndexWithDelim(data, "|", 2, buf, sizeof(buf));
    enemyid = atoi(buf);
    getStringFromIndexWithDelim(data, "|", 3, buf, sizeof(buf));
    vital = atoi(buf);
    getStringFromIndexWithDelim(data, "|", 4, buf, sizeof(buf));
    str = atoi(buf);
    getStringFromIndexWithDelim(data, "|", 5, buf, sizeof(buf));
    tough = atoi(buf);
    getStringFromIndexWithDelim(data, "|", 6, buf, sizeof(buf));
    dex = atoi(buf);

    enemynum = ENEMY_getEnemyNum();
    for (i = 0; i < enemynum; i++) {
      if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
        break;
      }
    }
    if (i == enemynum) {
      CHAR_talkToCli(charaindex, -1, "此宠物不存在，请与本服管理员联系！",
                     CHAR_COLORGREEN);
      return;
    }

    ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
    if (!CHAR_CHECKINDEX(ret)) {
      CHAR_talkToCli(charaindex, -1, "领取宠物失败", CHAR_COLORGREEN);
      return;
    }
    snprintf(token, sizeof(token), "领取宠物 %s ！",
             ENEMY_getChar(i, ENEMY_NAME));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      if (CHAR_getCharPet(charaindex, i) == ret)
        break;
    }
    if (i == CHAR_MAXPETHAVE) {
      CHAR_talkToCli(charaindex, -1, "宠物数量已达到最大值", CHAR_COLORGREEN);
      return;
    }

    if (CHAR_CHECKINDEX(ret) == TRUE) {
      if (vital > 0)
        CHAR_setInt(ret, CHAR_VITAL, vital);
      if (str > 0)
        CHAR_setInt(ret, CHAR_STR, str * 80);
      if (tough > 0)
        CHAR_setInt(ret, CHAR_TOUGH, tough * 80);
      if (dex > 0)
        CHAR_setInt(ret, CHAR_DEX, dex * 100);

      CHAR_setMaxExpFromLevel(ret, CHAR_getInt(ret, CHAR_LV));
      CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
    }

    CHAR_setInt(ret, CHAR_HP, CHAR_getWorkInt(ret, CHAR_WORKMAXHP));
    CHAR_complianceParameter(ret);
    snprintf(token, sizeof(token), "K%d", i);
    CHAR_sendStatusString(charaindex, token);
    snprintf(token, sizeof(token), "W%d", i);
    CHAR_sendStatusString(charaindex, token);

  } else if (data[0] == '1') {
    int emptyitemindexinchara, itemindex;
    int itemid;
    getStringFromIndexWithDelim(data, "|", 2, buf, sizeof(buf));
    itemid = atoi(buf);
    emptyitemindexinchara = CHAR_findEmptyItemBox(charaindex);

    if (emptyitemindexinchara < 0) {
      CHAR_talkToCli(charaindex, -1, "物品栏已满", CHAR_COLORYELLOW);
      return;
    }

    itemindex = ITEM_makeItemAndRegist(itemid);

    if (itemindex != -1) {
      CHAR_setItemIndex(charaindex, emptyitemindexinchara, itemindex);
      ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
      ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
      CHAR_sendItemDataOne(charaindex, emptyitemindexinchara);
      snprintf(token, sizeof(token), "得到物品 %s",
               ITEM_getChar(itemindex, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
    } else {
      CHAR_talkToCli(charaindex, -1, "", CHAR_COLORYELLOW);
    }
  } else if (data[0] == '2') {
    getStringFromIndexWithDelim(data, "|", 2, buf, sizeof(buf));
    int gold = atoi(buf);
    CHAR_setInt(charaindex, CHAR_GOLD,
                CHAR_getInt(charaindex, CHAR_GOLD) + gold);
    CHAR_complianceParameter(charaindex);
    CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
    sprintf(token, "领取 %d 石币！", gold);
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
  } else {
    CHAR_talkToCli(charaindex, -1, data, CHAR_COLORGREEN);
  }
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void SaacClient_OldToNew_recv(int client_fd, char *data) {
  int charaindex = CONNECT_getCharaindex(client_fd);
#ifdef _AMPOINT_LOG
  LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
             CHAR_getChar(charaindex, CHAR_CDKEY),
             -CHAR_getInt(charaindex, CHAR_AMPOINT), 0, "(积分转换)",
             CHAR_getInt(charaindex, CHAR_FLOOR),
             CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y));
#endif
  CHAR_setInt(charaindex, CHAR_AMPOINT, 0);

  CHAR_talkToCli(charaindex, -1, data, CHAR_COLORGREEN);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void SaacClient_FormulateAutoPk_recv(int client_fd, char *data) {
  int charaindex = CONNECT_getCharaindex(client_fd);
  CHAR_setWorkInt(charaindex, CHAR_WORK_AUTOPK, 0);
  CHAR_talkToCli(charaindex, -1, data, CHAR_COLORGREEN);
}
#endif

#ifdef _LOTTERY_SYSTEM
extern int todayaward[7];
void SaacClient_LotterySystem_recv(char *data) {
  int i;
  char token[256];
  for (i = 0; i < 7; i++) {
    if (getStringFromIndexWithDelim(data, ",", i + 1, token, sizeof(token)) ==
        TRUE) {
      todayaward[i] = atoi(token);
    } else {
      todayaward[i] = -1;
    }
  }
  struct tm now;
  time_t timep;
  time(&timep);
  memcpy(&now, localtime(&timep), sizeof(now));

  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      char token[256];
      sprintf(token, "%04d%02d%02d6连抽%0d,%0d,%0d,%0d,%0d,%0d,%0d",
              now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, todayaward[0],
              todayaward[1], todayaward[2], todayaward[3], todayaward[4],
              todayaward[5], todayaward[6]);

      CHAR_talkToCli(i, -1, token, CHAR_COLORGREEN);
      CHAR_talkToCli(i, -1, "抽奖系统", CHAR_COLORGREEN);
    }
  }
}
#endif

#ifdef _ALL_SERV_SEND
void SaacClient_AllServSend_recv(char *data) {
  int i;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      CHAR_talkToCli(i, -1, data, rand() % CHAR_COLORNUM);
    }
  }
}
#endif
