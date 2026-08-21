#define __SAAC_SERVER_RECV_C__
//
#include "recv.h"
#include "saac_config.h"
#include "saac_server.h"

//
#include "char.h"
#include "db.h"
#include "lock.h"
#include "mail.h"
#include "main.h"

#ifdef _SASQL
#include "sasql.h"
#endif

// CoolFish: Family 2001/5/9
#include "acfamily.h"

char retdata[CHARDATASIZE];
char result[CHARDATASIZE];

// Spock +1 2000/11/1
#define MAX_PROCESS 16777216
#define SAAC_PASSWORD_MAX 32
#define SAAC_IP_MAX 45
#define SAAC_MAC_MAX 127
#if _ATTESTAION_ID == 1
void SaacServer_ACServerLogin_recv(const int ti, const int id,
                                   const char *servername,
                                   const char *password) {
  print("收到GMSV的连接请求: fd: %d, id: %d, name: %s, pass: %s\n",
    ti, id, servername, password);
  login_game_server(ti, id, servername, password, result, sizeof(result),
                    retdata, sizeof(retdata));
  SaacServer_ACServerLogin_send(ti, result, retdata);
}
#else
void SaacServer_ACServerLogin_recv(const int ti, const char *servername,
                                   const char *password) {
  login_game_server(ti, servername, password, result, sizeof(result), retdata,
                    sizeof(retdata));
  SaacServer_ACServerLogin_send(ti, result, retdata);
}
#endif

void SaacServer_ACServerLogout_recv(int ti) { logout_game_server(ti); }

void SaacServer_ACCharLoad_recv(int ti, char *id, char *pas, char *charname,
                                int lock, char *opt, int mesgid) {
  if (!is_game_server_login(ti))
    return;
  // Spock 2000/10/31
  static int process = 0;
  char buf[16];
  // Spock end
#ifdef _NewSave
  int charindex = -1;
#endif

#ifdef _SASQL
  if (sasql_query(id, pas) != 1) {
#ifdef _NewSave
    SaacServer_ACCharLoad_send(ti, FAILED, "not login", mesgid, charindex);
#else
    SaacServer_ACCharLoad_send(ti, FAILED, "not login", mesgid);
#endif
    return;
  }
#endif

  if (!is_game_server_login(ti)) {
#ifdef _NewSave
    SaacServer_ACCharLoad_send(ti, FAILED, "not login", mesgid, charindex);
#else
    SaacServer_ACCharLoad_send(ti, FAILED, "not login", mesgid);
#endif
    return;
  }
  process++;
  if (process > MAX_PROCESS)
    process = 1;
  snprintf(buf, sizeof(buf), "%d", process);
  charLoadCallback(ti, 0, id, pas, charname, buf, "", lock, mesgid);
}

#ifdef _NewSave
void SaacServer_ACCharSave_recv(int ti, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int mesgid,
                                int charindex)
#else
void SaacServer_ACCharSave_recv(int ti, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int mesgid)
#endif
{
  char process[16];
  int ret;
  if (!is_game_server_login(ti)) {
    SaacServer_ACCharSave_send(ti, FAILED, "not login", mesgid);
    return;
  }
#ifdef _NewSave
  ret = charSave(ti, id, charname, opt, charinfo, unlock, mesgid, charindex);
#else
  ret = charSave(ti, id, charname, opt, charinfo, unlock, mesgid);
#endif
  snprintf(process, sizeof(process), "%d", ret);
  if (unlock) {
    dummyCallback(ti, 0, id, "dummy", charname, process, "", mesgid, 0);
  }
}

void SaacServer_ACCharList_recv(int ti, char *id, char *pas, char *ip,
                                char *mac, int mesgid, int charlistflg) {
  if (!is_game_server_login(ti))
    return;
  static int process = 0;
  char buf[10];

  process++;
  if (process > MAX_PROCESS)
    process = 1;
  snprintf(buf, sizeof(buf), "%d", process);
  if (!is_game_server_login(ti)) {
    SaacServer_ACCharList_send(ti, FAILED, "无法登陆游戏！", mesgid);
    return;
  }

#ifdef _SASQL
  if (strlen(id) == 0 || strlen(pas) == 0) {
    SaacServer_ACCharList_send(ti, FAILED, "账号密码不能为空！", mesgid);
    return;
  }
  if (sasql_check_lock(id)) {
    SaacServer_ACCharList_send(ti, FAILED, "你的游戏账号被锁定，无法登陆！",
                               mesgid);
    return;
  }

  int res = sasql_query(id, pas);
  if (res == 3) {
#ifdef _SQL_REGISTER
    if (!sasql_register(id, pas))
#endif
    {
      SaacServer_ACCharList_send(
          ti, FAILED, "您的账号尚未注册，请登陆服务器官网进行注册！", mesgid);
      return;
    }
  } else if (res != 1) {
#ifdef _ITEM_PET_LOCKED
    int res2 = sasql_ItemPetLocked_Char(id, pas);
    printf("res2=%d\n", res2);
    if (res2 != 1)
#endif
    {
      SaacServer_ACCharList_send(ti, FAILED, "您的账号或密码错误！", mesgid);
      return;
    }
  }

  if (strlen(id) == 0 || strlen(pas) == 0 || strlen(ip) == 0) {
    printf("登陆信息有错误！\n");
    SaacServer_ACCharList_send(ti, FAILED, "账号或密码不能为空！", mesgid);
    return;
  }
  if (sasql_check_lock(id)) {
    printf("该账号%s禁止登陆!\n", id);
    SaacServer_ACCharList_send(ti, FAILED, "你的游戏账号被锁定，无法登陆！",
                               mesgid);
    return;
  }
  if (sasql_check_lock(ip)) {
    printf("该IP%s禁止登陆!\n", ip);
    SaacServer_ACCharList_send(ti, FAILED, "你的游戏IP已被锁定，无法登陆！",
                               mesgid);
    return;
  }
  if (strlen(mac) > 0) {
    if (sasql_check_lock(mac)) {
      printf("该MAC%s禁止登陆!\n", ip);
      SaacServer_ACCharList_send(ti, FAILED, "你的游戏IP已被锁定，无法登陆！",
                                 mesgid);
      return;
    }
  }
#endif
  charListCallback(ti, 0, id, pas, "", buf, "0", mesgid, 0, charlistflg);
  logOut("档案列表: %s\n", id);
}

void SaacServer_ACCharDelete_recv(int ti, char *id, char *pas, char *charname,
                                  char *option, int mesgid) {
  if (!is_game_server_login(ti)) {
    SaacServer_ACCharDelete_send(ti, FAILED, "not delete", mesgid);
    return;
  }

#ifdef _ITEM_PET_LOCKED
  if (sasql_ItemPetLocked_Char(id, pas) == -1) {
    SaacServer_ACCharDelete_send(ti, FAILED, "not delete", mesgid);
    return;
  }
#endif
  // Nuke
  charDeleteCallback(ti, 0, id, pas, charname, "0", "", mesgid, 0);
  logOut("档案删除: 附加作业 %s %s\n", id, pas);
}

void SaacServer_ACLock_recv(int ti, char *id, int lock, int mesgid) {
  if (!is_game_server_login(ti)) {
    SaacServer_ACLock_send(ti, FAILED, "not delete", mesgid);
    return;
  }
  // Arminius 7.25 test unlock
  logOut("ACLock recv:%d\n", lock);
  if (lock == 2) {
    char buf[4096];
    if (GetMemLockState(getHash(id) & 0xff, id, buf)) {
      sprintf(retdata, "USRLOCKED:%s", buf);
    } else {
      sprintf(retdata, "USRUNLOCKED:%s", buf);
    }
    logOut(retdata);
  } else if (lock == 3) {
    int proc;
    if (DeleteMemLock(getHash(id) & 0xff, id, &proc)) {
      sprintf(retdata, "GMUNLOCKSUCCESS");
    } else {
      sprintf(retdata, "GMUNLOCKFAIL");
    }
    logOut(retdata);
  } else if (lock == 4) {
    DeleteMemLockServer(id);
    sprintf(retdata, "GMUNLOCKALL");
  } else {
#ifdef _LOCK_ADD_NAME
    if (lockUser(gs[ti].name, id, "", "0", lock, result, sizeof(result),
                 retdata, sizeof(retdata), "0", "0") < 0) {
#else
    if (lockUser(gs[ti].name, id, "0", lock, result, sizeof(result), retdata,
                 sizeof(retdata), "0", "0") < 0) {
#endif
      logOut("锁定用户: %s 失败.\n", id);
    } else {
      logOut("锁定用户: %s 成功.\n", id);
    }
  }
  // Spock end
  SaacServer_ACLock_send(ti, result, retdata, mesgid);
}

void SaacServer_ACUCheck_recv(int ti, char *id, int status) {
  if (!is_game_server_login(ti)) {
    logOut("服务器发现账号:%s status:%d\n", id, status);
    return;
  }

  if (status == 0) {
    logOut("用户 %s 在 %s 并未锁定！\n", id, gs[ti].name);
    SaacServer_ACKick_recv(ti, id, 6, -1);
  } else {
    logOut("用户 %s 在 %s 已锁定！\n", id, gs[ti].name);
#ifdef _WAEI_KICK
    SaacServer_ACKick_recv(ti, id, 1, -1);
#endif
  }
}

void SaacServer_DBUpdateEntryString_recv(int fd, char *table, char *key,
                                         char *value, int msgid, int msgid2) {
  const int r = dbUpdateEntryString(table, key, value);
  if (r != 0) {
    logOut("failed: DBUpdateEntryString err !!\n");
  }
}

void SaacServer_DBDeleteEntryString_recv(int fd, char *table, char *key,
                                         int msgid, int msgid2) {
  int r = dbDeleteEntryString(table, key);
  if (r == 0) {
    SaacServer_DBDeleteEntryString_send(fd, SUCCESSFUL, table, key, msgid,
                                        msgid2);
  } else {
    SaacServer_DBDeleteEntryString_send(fd, FAILED, table, key, msgid, msgid2);
  }
}

void SaacServer_DBGetEntryString_recv(int fd, char *table, char *key, int msgid,
                                      int msgid2) {}

void SaacServer_DBUpdateEntryInt_recv(int fd, char *table, char *key, int value,
                                      char *info, int msgid, int msgid2) {
  if (!is_game_server_login(fd)) {
    SaacServer_DBUpdateEntryInt_send(fd, FAILED, "", "", msgid, msgid2);
    return;
  }
  if (dbUpdateEntryInt(table, key, value, info) == 0) {
    SaacServer_DBUpdateEntryInt_send(fd, SUCCESSFUL, table, key, msgid, msgid2);
  } else {
    SaacServer_DBUpdateEntryInt_send(fd, FAILED, table, key, msgid, msgid2);
  }
}

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
void SaacServer_UpdataStele_recv(int fd, char *cdkey, char *name, char *title,
                                 int level, int trns, int time, int floor) {
  int i, min = 140, minnum = -1, nameflag = -1;
  for (i = 0; i < MAX_HERO_LIST; i++) {
    if (atoi(Herolist[i][6]) < min) {
      min = atoi(Herolist[i][6]);
      minnum = i;
    }
    if (strcmp(Herolist[i][0], cdkey) == 0 && strcmp(Herolist[i][1], name) == 0)
      nameflag = i;
  }
  if (strcmp(cdkey, "FirstLoad") == 0 && strcmp(name, "LoadHerolist") == 0 &&
      floor == 999)

    Send_A_herolist(fd);
  else if (nameflag != -1 && floor > atoi(Herolist[nameflag][6])) {
    Send_S_herolist(Herolist[nameflag][0], Herolist[nameflag][1], cdkey, name,
                    title, level, trns, floor);
    sprintf(Herolist[nameflag][2], "%s", title);
    sprintf(Herolist[nameflag][3], "%d", level);
    sprintf(Herolist[nameflag][4], "%d", trns);
    sprintf(Herolist[nameflag][5], "%d", time);
    sprintf(Herolist[nameflag][6], "%d", floor);
    SAVE_herolist(fd);
  } else if (nameflag == -1 && floor > min) {
    Send_S_herolist(Herolist[minnum][0], Herolist[minnum][1], cdkey, name,
                    title, level, trns, floor);
    sprintf(Herolist[minnum][0], "%s", cdkey);
    sprintf(Herolist[minnum][1], "%s", name);
    sprintf(Herolist[minnum][2], "%s", title);
    sprintf(Herolist[minnum][3], "%d", level);
    sprintf(Herolist[minnum][4], "%d", trns);
    sprintf(Herolist[minnum][5], "%d", time);
    sprintf(Herolist[minnum][6], "%d", floor);
    SAVE_herolist(fd);
  }
}
#endif

void SaacServer_DBGetEntryRank_recv(int fd, char *table, char *key, int msgid,
                                    int msgid2) {
  int rk, c;

  if (!is_game_server_login(fd)) {
    SaacServer_DBGetEntryRank_send(fd, FAILED, 0, 0, "", "", msgid, msgid2);
    return;
  }

  dbGetEntryRank(table, key, &rk, &c);
  if (rk < 0) {
    SaacServer_DBGetEntryRank_send(fd, FAILED, -1, -1, table, key, msgid,
                                   msgid2);
  } else {
    SaacServer_DBGetEntryRank_send(fd, SUCCESSFUL, rk, c, table, key, msgid,
                                   msgid2);
  }
}

void SaacServer_DBDeleteEntryInt_recv(int fd, char *table, char *key, int msgid,
                                      int msgid2) {
  if (!is_game_server_login(fd)) {
    SaacServer_DBDeleteEntryInt_send(fd, FAILED, "", "", msgid, msgid2);
    return;
  }

  if (dbDeleteEntryInt(table, key) < 0) {
    SaacServer_DBDeleteEntryInt_send(fd, FAILED, table, key, msgid, msgid2);
  } else {
    SaacServer_DBDeleteEntryInt_send(fd, SUCCESSFUL, table, key, msgid, msgid2);
  }
}

void SaacServer_DBGetEntryInt_recv(int fd, char *table, char *key, int msgid,
                                   int msgid2) {
  int r;
  int output;

  if (!is_game_server_login(fd)) {
    SaacServer_DBGetEntryInt_send(fd, FAILED, -1, "", "", msgid, msgid2);
    return;
  }

  r = dbGetEntryInt(table, key, &output);

  if (r < 0) {
    SaacServer_DBGetEntryInt_send(fd, FAILED, -1, table, key, msgid, msgid2);
  } else {
    SaacServer_DBGetEntryInt_send(fd, SUCCESSFUL, output, table, key, msgid,
                                  msgid2);
  }
}

void SaacServer_DBGetEntryByRank_recv(int fd, char *table, int start, int end,
                                      int msgid, int msgid2) {
  char output[16384];
  int r;

  if (!is_game_server_login(fd)) {
    SaacServer_DBGetEntryByRank_send(fd, FAILED, "", "", msgid, msgid2);
    return;
  }

  r = dbGetEntryRankRange(table, start, end, output, sizeof(output));

  if (r < 0) {
    SaacServer_DBGetEntryByRank_send(fd, FAILED, "", table, msgid, msgid2);
  } else {
    SaacServer_DBGetEntryByRank_send(fd, SUCCESSFUL, output, table, msgid,
                                     msgid2);
  }
}

void SaacServer_DBGetEntryByCount_recv(int fd, char *table, int count_start,
                                       int num, int msgid, int msgid2) {
  char output[16384];
  int r;

  if (!is_game_server_login(fd)) {
    SaacServer_DBGetEntryByCount_send(fd, FAILED, "", table, count_start, msgid,
                                      msgid2);
    return;
  }

  r = dbGetEntryCountRange(table, count_start, num, output, sizeof(output));

  if (r < 0) {
    SaacServer_DBGetEntryByCount_send(fd, FAILED, "", table, count_start, msgid,
                                      msgid2);
  } else {
    SaacServer_DBGetEntryByCount_send(fd, SUCCESSFUL, output, table,
                                      count_start, msgid, msgid2);
  }
}

void SaacServer_Broadcast_recv(int fd, char *id, char *charname, char *message,
                               int flag) {}

void SaacServer_Message_recv(int fd, char *id_from, char *charname_from,
                             char *id_to, char *charname_to, char *message,
                             int option) {
  Mail_receive(id_from, charname_from, id_to, charname_to, message, option, 0,
               0);
}

void SaacServer_MessageAck_recv(int fd, char *id, char *charname, char *result,
                                int mesgid) {
  int a;
  if (strcmp(result, SUCCESSFUL))
    a = 0;
  else
    a = -1;
  Mail_receive_ack(id, charname, a, mesgid);
}

void SaacServer_MessageFlush_recv(int fd, char *id, char *charname) {
  Mail_flush(fd, id, charname);
}

/*******************************************************
 * CoolFish: Family 2001/5/18
 ******************************************************/
#ifdef _PERSONAL_FAME
void SaacServer_ACAddFM_recv(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petattr, char *fmrule, int fmsprite,
                             int fmleadergrano, int fame,
#ifdef _FAMILYBADGE_
                             int fmbadge,
#endif
                             int charfdid)
#else
void SaacServer_ACAddFM_recv(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petattr, char *fmrule, int fmsprite,
                             int fmleadergrano, int charfdid)
#endif
{
#ifdef _FAMILY
  int r = 0, index = 0;
#ifdef _PERSONAL_FAME
  r = ACAddFM(fd, &index, fmname, fmleadername, fmleaderid, fmleaderlv, petname,
              petattr, fmrule, fmsprite, fmleadergrano, fame,
#ifdef _FAMILYBADGE_
              fmbadge,
#endif
              charfdid);
#else
  r = ACAddFM(fd, &index, fmname, fmleadername, fmleaderid, fmleaderlv, petname,
              petattr, fmrule, fmsprite, fmleadergrano, charfdid);
#endif
  if (r < 0) {
    SaacServer_ACAddFM_send(fd, FAILED, r, index, charfdid);
  } else {
    SaacServer_ACAddFM_send(fd, SUCCESSFUL, r, index, charfdid);
  }
#endif
}

void SaacServer_ACJoinFM_recv(int fd, char *fmname, int fmindex,
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
                              char *charname, char *charid, int charlv,
                              int index, int fame, int charfdid)
#else
                              char *charname, char *charid, int charlv,
                              int index, int charfdid)
#endif
{
#ifdef _FAMILY
  int r = 0;
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  r = ACJoinFM(fd, index, fmname, fmindex, charname, charid, charlv, fame,
               charfdid);
#else
  r = ACJoinFM(fd, index, fmname, fmindex, charname, charid, charlv, charfdid);
#endif
  if (r < 0) {
    SaacServer_ACJoinFM_send(fd, FAILED, r, charfdid);
  } else {
    SaacServer_ACJoinFM_send(fd, SUCCESSFUL, r, charfdid);
  }
#endif
}

void SaacServer_ACLeaveFM_recv(int fd, char *fmname, int fmindex,
                               char *charname, char *charid, int index,
                               int charfdid) {
#ifdef _FAMILY
  int r = 0;
  r = ACLeaveFM(index, fmname, fmindex, charname, charid);
  if (r < 0) {
    SaacServer_ACLeaveFM_send(fd, FAILED, r, charfdid);
  } else {
    SaacServer_ACLeaveFM_send(fd, SUCCESSFUL, r, charfdid);
  }
#endif
}

#ifdef _LEADERFUNCHECK
void SaacServer_ACDelFM_recv(int fd, char *fmname, int fmindex, int index,
                             char *charname, char *charid, int charfdid) {
  int r = 0, result = 0;
  result = CheckLeaderQ(index, fmname, fmindex, charname, charid);
  if (result < 0) {
    SaacServer_ACDelFM_send(fd, FAILED, charfdid);
    return;
  }
  // shan modify begin
  // original
  /*if (ChangeFMLeader(index, fmname, fmindex) < 0)
          r = ACDelFM(index, fmname, fmindex);
  else
          r = -1;  // shan  1 -> -1
  if (r < 0)
          SaacServer_ACDelFM_send(fd, FAILED, charfdid);
  else
          SaacServer_ACDelFM_send(fd, SUCCESSFUL, charfdid);
  */
  r = ACDelFM(index, fmname, fmindex);
  if (r < 0)
    SaacServer_ACDelFM_send(fd, FAILED, charfdid);
  else
    SaacServer_ACDelFM_send(fd, SUCCESSFUL, charfdid);
  // shan end
}
#else
void SaacServer_ACDelFM_recv(int fd, char *fmname, int fmindex, int index,
                             int charfdid) {
#ifdef _FAMILY
  int r = 0;
#ifdef _FMVER21
  if (ChangeFMLeader(index, fmname, fmindex) >= 0)
    r = ACDelFM(index, fmname, fmindex);
  else
    r = 1;
#else
  r = ACDelFM(index, fmname, fmindex);
#endif
  if (r < 0) {
    SaacServer_ACDelFM_send(fd, FAILED, charfdid);
  }
#ifdef _FMVER21
  else if (r == 1) {
  }
#endif
  else {
    SaacServer_ACDelFM_send(fd, SUCCESSFUL, charfdid);
  }
#endif
}
#endif

void SaacServer_ACShowFMList_recv(int fd) {
#ifdef _FAMILY
  int r = 0;
  char data[150 * MAX_FAMILY];
  r = ACShowFMList(data);
  if (r < 0) {
    SaacServer_ACShowFMList_send(fd, FAILED, r, "Nothing");
  } else {
    SaacServer_ACShowFMList_send(fd, SUCCESSFUL, r, data);
  }
#endif
}
#ifdef _FAMILY_TOTEM
void SaacServer_ACShowFMTotem_recv(int fd) { ACShowFMTotem(fd); }
#endif
void SaacServer_ACShowMemberList_recv(int fd, int index) {
#ifdef _FAMILY
  int r = 0, fmacceptflag = 0, fmjoinnum = 0, badge = 0;
  char data[150 * MAX_MEMBERNUM];
  r = ACShowFMMemberList(index, &fmacceptflag, &fmjoinnum, data, &badge);
  if (r < 0) {
    SaacServer_ACShowMemberList_send(fd, FAILED, index, r, fmacceptflag,
                                     fmjoinnum, "Nothing", badge);
  } else {
    SaacServer_ACShowMemberList_send(fd, SUCCESSFUL, index, r, fmacceptflag,
                                     fmjoinnum, data, badge);
  }
#endif
}

void SaacServer_ACFMDetail_recv(int fd, char *fmname, int fmindex, int index,
                                int charfdid) {
#ifdef _FAMILY
  int r = 0;
  char data[15000];
  r = ACFMDetail(index, fmname, fmindex, data);
  if (r < 0) {
    SaacServer_ACFMDetail_send(fd, FAILED, "Nothing", charfdid);
  } else {
    SaacServer_ACFMDetail_send(fd, SUCCESSFUL, data, charfdid);
  }
#endif
}

#ifdef _FMVER21
void SaacServer_ACMemberJoinFM_recv(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int meindex, int charfdid)
#else
void SaacServer_ACMemberJoinFM_recv(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int charfdid)
#endif
{
  int r = 0;
#ifdef _FMVER21
  r = ACMemberJoinFM(index, fmname, fmindex, charname, charindex, result,
                     meindex);
#else
  r = ACMemberJoinFM(index, fmname, fmindex, charname, charindex);
#endif
  if (r < 0) {
    SaacServer_ACMemberJoinFM_send(fd, FAILED, charfdid);
  } else {
    SaacServer_ACMemberJoinFM_send(fd, SUCCESSFUL, charfdid);
  }
}

#ifdef _FMVER21
void SaacServer_ACMemberLeaveFM_recv(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int meindex, int charfdid)
#else
void SaacServer_ACMemberLeaveFM_recv(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int charfdid)
#endif
{
#ifdef _FAMILY
  int r = 0;
  int flag = 1; // 主动离开家族
#ifdef _FMVER21
  r = ACMemberLeaveFM(index, fmname, fmindex, charname, flag, charindex,
                      meindex);
#else
  r = ACMemberLeaveFM(index, fmname, fmindex, charname, flag, charindex);
#endif
  if (r < 0) {
    SaacServer_ACMemberLeaveFM_send(fd, FAILED, charfdid);
  } else {
    SaacServer_ACMemberLeaveFM_send(fd, SUCCESSFUL, charfdid);
  }
#endif
}

#ifdef _FM_MODIFY
void SaacServer_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int eventflag, int charfdid, int gsnum)
#else
#ifdef _FMVER21
void SaacServer_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int eventflag, int charfdid)
#else
void SaacServer_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int charfdid)
#endif
#endif
{
#ifdef _FAMILY
  int r = 0, floor = 0, fmpopular = 0, joinflag = 0, fmsetupflag = 0;
  int flag = 0, charindex = 0;
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  int charfame = 0;
#endif
#ifdef _NEW_MANOR_LAW
  int momentum = 0;
#endif
#ifdef _FM_MODIFY
  r = ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv, &floor,
                    &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid,
                    &charfame, eventflag, gsnum
#ifdef _NEW_MANOR_LAW
                    ,
                    &momentum
#endif
  );
#else
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  r = ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv, &floor,
                    &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid,
                    &charfame, eventflag);
#else
  r = ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv, &floor,
                    &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid);
#endif
#endif

#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  if (r < 0) {
    SaacServer_ACFMCharLogin_send(fd, FAILED, r, floor, fmpopular, joinflag,
                                  fmsetupflag, flag, charindex, charfame,
                                  charfdid
#ifdef _NEW_MANOR_LAW
                                  ,
                                  momentum
#endif
    );
  } else {
    SaacServer_ACFMCharLogin_send(fd, SUCCESSFUL, r, floor, fmpopular, joinflag,
                                  fmsetupflag, flag, charindex, charfame,
                                  charfdid
#ifdef _NEW_MANOR_LAW
                                  ,
                                  momentum
#endif
    );
  }
#else
  if (r < 0) {
    SaacServer_ACFMCharLogin_send(fd, FAILED, r, floor, fmpopular, joinflag,
                                  fmsetupflag, flag, charindex, charfdid);
  } else {
    SaacServer_ACFMCharLogin_send(fd, SUCCESSFUL, r, floor, fmpopular, joinflag,
                                  fmsetupflag, flag, charindex, charfdid);
  }
#endif

#endif
}

void SaacServer_ACFMCharLogout_recv(int fd, char *fmname, int fmindex,
                                    char *charname, char *charid, int charlv,
                                    int index, int charfdid) {
#ifdef _FAMILY
  int r = 0;
  r = ACFMCharLogout(index, fmname, fmindex, charname, charid, charlv,
                     charfdid);
  if (r < 0) {
    SaacServer_ACFMCharLogout_send(fd, FAILED, charfdid);
  } else {
    SaacServer_ACFMCharLogout_send(fd, SUCCESSFUL, charfdid);
  }
#endif
}

void SaacServer_ACFMReadMemo_recv(int fd, int index) {

#ifdef _FAMILY
  int r = 0, dataindex;
  char data[15000];
  r = ACFMReadMemo(index, &dataindex, data);
  if (r < 0) {
    SaacServer_ACFMReadMemo_send(fd, FAILED, index, r, dataindex, "Nothing");
  } else {
    SaacServer_ACFMReadMemo_send(fd, SUCCESSFUL, index, r, dataindex, data);
  }
#endif
}

void SaacServer_ACFMWriteMemo_recv(int fd, char *fmname, int fmindex,
                                   char *data, int index) {
#ifdef _FAMILY
  int r = 0;
  r = ACFMWriteMemo(index, fmname, fmindex, data);
  if (r < 0) {
    SaacServer_ACFMWriteMemo_send(fd, FAILED, index);
  } else {
    SaacServer_ACFMWriteMemo_send(fd, SUCCESSFUL, index);
  }
#endif
}

void SaacServer_ACFMPointList_recv(int fd) {
#ifdef _FAMILY
  int r = 0;
  char data[15000];
  r = ACFMPointList(data);
  if (r < 0) {
    SaacServer_ACFMPointList_send(fd, FAILED, "nothing");
  } else {
    SaacServer_ACFMPointList_send(fd, SUCCESSFUL, data);
  }
#endif
}

void SaacServer_ACSetFMPoint_recv(int fd, char *fmname, int fmindex, int index,
                                  int fmpointindex, int fl, int x, int y,
                                  int charfdid) {
#ifdef _FAMILY
  int r = 0;
  r = ACSetFMPoint(index, fmname, fmindex, fmpointindex, fl, x, y);
  if (r < 0) {
    SaacServer_ACSetFMPoint_send(fd, FAILED, r, charfdid);
  } else {
    SaacServer_ACSetFMPoint_send(fd, SUCCESSFUL, r, charfdid);
  }
#endif
}

void SaacServer_ACFixFMPoint_recv(int fd, char *winfmname, int winfmindex,
                                  int winindex, char *losefmname,
                                  int losefmindex, int loseindex, int village) {
#ifdef _FAMILY
  int r = 0;
  r = ACFixFMPoint(winindex, winfmname, winfmindex, loseindex, losefmname,
                   losefmindex, village);
  if (r < 0) {
    SaacServer_ACFixFMPoint_send(fd, FAILED, r);
  } else {
    SaacServer_ACFixFMPoint_send(fd, SUCCESSFUL, r);
  }
#endif
}

void SaacServer_ACFMAnnounce_recv(int fd, char *fmname, int fmindex, int index,
                                  char *data, int color) {
#ifdef _FAMILY
  int r = 0, i = 0;
  r = ACFMAnnounce(fmname, fmindex, index, data, color);
  if (r < 0) {
    SaacServer_ACFMAnnounce_send(fd, FAILED, fmname, fmindex, index, 1, data,
                                 color);
  } else {
    for (i = 0; i < MAXCONNECTION; i++) {
      if (gs[i].use && gs[i].name[0]) {
        SaacServer_ACFMAnnounce_send(i, SUCCESSFUL, fmname, fmindex, index, 1,
                                     data, color);
      }
    }
  }
#endif
}

void SaacServer_ACShowTopFMList_recv(int fd, int kindflag) {
#ifdef _FAMILY
  int r = 0;
  char *data = (char *)malloc(150 * MAX_FAMILY);
  if (data == NULL) {
    SaacServer_ACShowTopFMList_send(fd, FAILED, kindflag, r, "Nothing");
    return;
  }
  data[0] = '\0';
  r = ACShowTopFMList(data, 150 * MAX_FAMILY, kindflag);
  if (r < 0) {
    SaacServer_ACShowTopFMList_send(fd, FAILED, kindflag, r, "Nothing");
  } else {
    SaacServer_ACShowTopFMList_send(fd, SUCCESSFUL, kindflag, r, data);
  }
  free(data);
#endif
}

void SaacServer_ACFixFMData_recv(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, char *data1, char *data2,
                                 int charindex, int charfdid) {
#ifdef _FAMILY
  int r = 0;
  r = ACFixFMData(index, fmname, fmindex, kindflag, charindex, data1, data2);
  if (r < 0) {
    SaacServer_ACFixFMData_send(fd, FAILED, kindflag, data1, data2, charfdid);
  } else {
    SaacServer_ACFixFMData_send(fd, SUCCESSFUL, kindflag, data1, data2,
                                charfdid);
  }
#endif
}

void SaacServer_ACFixFMPK_recv(int fd, char *winfmname, int winfmindex,
                               int winindex, char *losefmname, int losefmindex,
                               int loseindex) {
#ifdef _FAMILY
  int r = 0;
  r = ACFixFMPK(winindex, winfmname, winfmindex, loseindex, losefmname,
                losefmindex);
  if (r < 0) {
    SaacServer_ACFixFMPK_send(fd, FAILED, r, winindex, loseindex);
  } else {
    SaacServer_ACFixFMPK_send(fd, SUCCESSFUL, r, winindex, loseindex);
  }
#endif
}

void SaacServer_ACGMFixFMData_recv(int fd, int index, char *charid, char *cmd,
                                   char *data, int charfdid) {
#ifdef _FAMILY
  int r = 0;
  char fmname[256];
  r = ACGMFixFMData(index, fmname, charid, cmd, data);
  if (r < 0) {
    SaacServer_ACGMFixFMData_send(fd, FAILED, fmname, charfdid);
  } else {
    SaacServer_ACGMFixFMData_send(fd, SUCCESSFUL, fmname, charfdid);
  }
#endif
}

void SaacServer_ACGetFMData_recv(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, int charfdid) {
#ifdef _FAMILY
  int r = 0, data = 0;
  r = ACGetFMData(index, fmname, fmindex, kindflag, &data);
  if (r < 0) {
    SaacServer_ACGetFMData_send(fd, FAILED, kindflag, data, charfdid);
  } else {
    SaacServer_ACGetFMData_send(fd, SUCCESSFUL, kindflag, data, charfdid);
  }
#endif
}

void SaacServer_ACreLoadFmData_recv(int fd, int type, int data) {
  int i = 0;
  switch (type) {
  case 1:
    logOut("reload FM_DATA:%d !\n", data);
    readOneFamilyFromTi(data);
    break;
  case 2: {
    char data[15000];
    readFMPoint(g_saac_config.fmpointdir);
    if (ACFMPointList(data) >= 0) {
      logOut("reload FM_POINT !\n");
      for (i = 0; i < MAXCONNECTION; i++) {
        if (gs[i].use && gs[i].name[0])
          SaacServer_ACFMPointList_send(i, SUCCESSFUL, data);
      }
    }
  } break;
  case 3: // fmpk_list
  {
    int j = 0;
    logOut("reload FMPK_LIST !\n");
    FMPK_InitList();
    FMPK_LoadList();
    for (j = 0; j < FMPKLIST_MAXNUM; j++) {
      for (i = 0; i < MAXCONNECTION; i++) {
        if (gs[i].use && gs[i].name[0])
          SaacServer_ACLoadFmPk_send(i, FMPK_GetData(j));
      }
    }
  } break;
  }
}

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
#ifdef _ACFMPK_LIST
void SaacServer_ACLoadFmPk_recv(int fd, int fmpks_pos) {
  if (fmpks_pos < 0 || fmpks_pos > MAX_FMPOINT)
    return;
  SaacServer_ACLoadFmPk_send(fd, FMPK_GetData(fmpks_pos - 1));
}

void SaacServer_ACSendFmPk_recv(int fd, int fmpks_pos, int userindex, int flg,
                                char *data) {
  char buf[1024];
  int i = 0;
  sprintf(buf, "%d|%s", fmpks_pos, data);

  if ((userindex != -1 && FMPK_GetTypeFlg(fmpks_pos - 1) == 1) ||
      FMPK_SetData(fmpks_pos - 1, flg, buf, strlen(buf)) != 1) {
    if (userindex != -1)
      SaacServer_ACSendFmPk_send(fd, userindex, 0);
    logOut("err ACSendFmPk_send(%d, %d)\n", userindex, 0);
    return;
  }
  FMPK_BackUpList();
  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && gs[i].name[0]) {
      SaacServer_ACLoadFmPk_send(i, FMPK_GetData(fmpks_pos - 1));
    }
  }
  // 送确认讯息
  if (userindex != -1)
    SaacServer_ACSendFmPk_send(fd, userindex, 1);
}
#else
void SaacServer_ACLoadFmPk_recv(int fd, int fmpks_pos) {
  if (fmpks_pos > MAX_FMPOINT) {
    logOut("\n fmpks_pos(%d) too big", fmpks_pos);
  }
  SaacServer_ACLoadFmPk_send(fd, fm_pk_list[fmpks_pos - 1]);
}
void SaacServer_ACSendFmPk_recv(int fd, int fmpks_pos, char *data) {
  int i = 0;

  if ((fmpks_pos > MAX_FMPOINT) || (fmpks_pos < 1)) {
    logOut("\n fmpks_pos(%d) err !!", fmpks_pos);
  }
  sprintf(fm_pk_list[fmpks_pos - 1], "%d|%s", fmpks_pos, data);

  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && gs[i].name[0] && i != fd) {
      SaacServer_ACLoadFmPk_send(i, fm_pk_list[fmpks_pos - 1]);
    }
  }

  save_fm_pk_list();
}
#endif

#endif

void SaacServer_ACManorPKAck_recv(int fd, char *data) {
#ifdef _FAMILY

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
                      // 不处理
#else
  int i;

  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && gs[i].name[0]) {
      SaacServer_ACManorPKAck_send(i, data);
    }
  }
#endif

#endif
}

#ifdef _WAEI_KICK
void SaacServer_ACKick_recv(int ti, char *id, int lock, int mesgid) {
  if (ti != -1) {
    if (!is_game_server_login(ti)) { // MAXCONNECTION
      SaacServer_ACKick_send(ti, 0, "not login", mesgid);
      return;
    }
  }
  sprintf(retdata, "ACTION !!");
  //"LSLOCK","KICK","DEUNLOCK","UNLOCKALL","LOCK","TYPE", "UNLOCK"
  switch (lock) {
  case 0: // LSLOCK
    break;
  case 1: // 跨星球踢人 KICK
  {
    int i;
    for (i = 0; i < MAXCONNECTION; i++) {
      if (is_game_server_login(i)) {
        SaacServer_ACKick_send(i, 1, id, mesgid);
      }
    }
  } break;
  case 2: // DEUNLOCK
    break;
  case 3: // UNLOCKALL
  {
    DeleteMemLockServer(gs[ti].name);
    sprintf(retdata, "GM UNLOCK ALL");
  } break;
  case 4: // LOCK
#ifdef _LOCK_ADD_NAME
    if (lockUser(gs[ti].name, id, "", "0", lock, result, sizeof(result),
                 retdata, sizeof(retdata), "0", "0") < 0) {
#else
    if (lockUser(gs[ti].name, id, "0", lock, result, sizeof(result), retdata,
                 sizeof(retdata), "0", "0") < 0) {
#endif
      sprintf(retdata, "LOCK USER !!");
    }
    break;
  case 5: // 查询玩家状态 TYPE
  {
    char buf[4096];
    if (GetMemLockState(getHash(id) & 0xff, id, buf)) {
      sprintf(retdata, "USR LOCKED: %s", buf);
    } else {
      sprintf(retdata, "USR UNLOCKED: %s", buf);
    }
  } break;
  case 6: // MEMORY UNLOCK; UNLOCK
  {
    char result[100], retdata[100], process[16];
    int ret = 0;
#ifdef _LOCK_ADD_NAME
    ret = lockUser(gs[ti].name, id, "", "0", 0, result, sizeof(result), retdata,
                   sizeof(retdata), "0", "0");
#else
    ret = lockUser(gs[ti].name, id, "0", 0, result, sizeof(result), retdata,
                   sizeof(retdata), "0", "0");
#endif

    if (strcmp(result, SUCCESSFUL) == 0) {
      logOut("解锁账号 %s 成功！\n", id);
    } else {
      logOut("解锁账号 %s 失败！\n", id);
      return;
    }

    snprintf(process, sizeof(process), "%d", ret);
    dummyCallback(ti, 0, id, "dummy", charname, process, "", mesgid, 0);
  } break;
  case 10: // 跨星球踢人不含load该星球
  {
    int i;
    for (i = 0; i < MAXCONNECTION; i++) {
      if (is_game_server_login(i) && i != ti) {
        SaacServer_ACKick_send(i, 1, id, mesgid);
      }
    }
  } break;
  }
  SaacServer_ACKick_send(ti, 0, retdata, mesgid);
}

#endif

#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
void SendEffect(char *effect) {
  int i;
  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && gs[i].name[0]) {
      SaacServer_SendEffect_send(i, effect);
    }
  }
}
#endif

#ifdef _CHAR_POOLITEM
void SaacServer_ACCharInsertPoolItem_recv(int fd, char *cdkey, int userindex,
                                          int clifdid, char *Pooldataarg) {
  if (!is_game_server_login(fd)) {
    return;
  }
  if (InsertCharPoolItem(cdkey, Pooldataarg, strlen(Pooldataarg)) <= 0) {
    logOut("\n InsertPoolItem( %s) err!!\n", cdkey);
    SaacServer_ACCharSavePoolItem_send(fd, FAILED, Pooldataarg, clifdid);
  }
}

void SaacServer_ACCharSavePoolItem_recv(int fd, char *cdkey, int userindex,
                                        int clifdid, char *Pooldataarg) {
  if (!is_game_server_login(fd)) {
    return;
  }
  if (saveCharPoolItem(cdkey, Pooldataarg, strlen(Pooldataarg)) < 0) {
    SaacServer_ACCharSavePoolItem_send(fd, FAILED, Pooldataarg, clifdid);
  }
}

void SaacServer_ACCharGetPoolItem_recv(int fd, char *cdkey, int userindex,
                                       int clifdid, int npcid) {
  char loadbuf[CHARDATASIZE];
  int ret = -1;
  if (!is_game_server_login(fd)) {
    return;
  }

  ret = loadCharPoolItemOne(cdkey, loadbuf, sizeof(loadbuf));

  if (ret == -1) { // 找不到档案
    SaacServer_ACCharGetPoolItem_send(fd, FAILED, loadbuf, clifdid, npcid);
    return;
  }
  SaacServer_ACCharGetPoolItem_send(fd, SUCCESSFUL, loadbuf, clifdid, npcid);
}
#endif

#ifdef _CHAR_POOLPET
void SaacServer_ACCharInsertPoolPet_recv(int fd, char *cdkey, int userindex,
                                         int clifdid, char *Pooldataarg) {
  if (!is_game_server_login(fd)) {
    return;
  }
  if (InsertCharPoolPet(cdkey, Pooldataarg, strlen(Pooldataarg)) <= 0) {
    logOut("\n InsertPoolPet( %s) err!!\n", cdkey);
    SaacServer_ACCharSavePoolPet_send(fd, FAILED, Pooldataarg, clifdid);
  }
}

void SaacServer_ACCharSavePoolPet_recv(int fd, char *cdkey, int userindex,
                                       int clifdid, char *Pooldataarg) {
  if (!is_game_server_login(fd)) {
    return;
  }
  if (saveCharPoolPet(cdkey, Pooldataarg, strlen(Pooldataarg)) < 0) {
    SaacServer_ACCharSavePoolPet_send(fd, FAILED, Pooldataarg, clifdid);
  }
}

void SaacServer_ACCharGetPoolPet_recv(int fd, char *cdkey, int userindex,
                                      int clifdid, int npcid) {
  char loadbuf[CHARDATASIZE];
  int ret = -1;
  if (!is_game_server_login(fd)) {
    return;
  }

  ret = loadCharPoolPetOne(cdkey, loadbuf, sizeof(loadbuf));

  if (ret == -1) { // 找不到档案
    SaacServer_ACCharGetPoolPet_send(fd, FAILED, loadbuf, clifdid, npcid);
    return;
  }
  SaacServer_ACCharGetPoolPet_send(fd, SUCCESSFUL, loadbuf, clifdid, npcid);
}
#endif

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void LOAD_herolist() {
  FILE *fdb;
  int i, linenum = 0;
  char filename[256], token[512];
  snprintf(filename, sizeof(filename), "%s/%s", "db/herolist",
           "db_herolist.txt");
  if (!(fdb = fopen(filename, "r+"))) {
    logOut("\nSyu log open db_herolist.txt error!!");
    return;
  } else {
    char *addr;
    fread(&token, sizeof(token), 1, fdb);
    fclose(fdb);
    do {
      for (i = 0; i < MAX_HERO_COLUMN; i++) {
        easyGetTokenFromBuf(token, "|", i + 1, Herolist[linenum][i],
                            sizeof(Herolist[linenum][i]));
      }
      linenum++;
      if ((addr = strstr(token, "\n")))
        strcpy(token, addr + 1);
      else
        break;
    } while (1);
  }
}

void SAVE_herolist(int fd) {
  int i;
  FILE *fdb;
  char filename[256];
  snprintf(filename, sizeof(filename), "%s/%s", "db/herolist",
           "db_herolist.txt");
  if (!(fdb = fopen(filename, "w"))) {
    logOut("\nSyu log open db_herolist.txt error!!");
    return;
  } else {
    char tmp[256];
    for (i = 0; i < MAX_HERO_LIST; i++) {
      sprintf(tmp, "%s|%s|%s|%d|%d|%d|%d\n", Herolist[i][0], Herolist[i][1],
              Herolist[i][2], atoi(Herolist[i][3]), atoi(Herolist[i][4]),
              atoi(Herolist[i][5]), atoi(Herolist[i][6]));
      fwrite(tmp, strlen(tmp), 1, fdb);
    }
    fclose(fdb);
  }
}

void Send_A_herolist(int fd) {
  FILE *fdb;
  int lens = 0;
  char filename[256], buf[256];

  snprintf(filename, sizeof(filename), "%s/%s", "db/herolist",
           "db_herolist.txt");
  if (!(fdb = fopen(filename, "r+"))) {
    logOut("\n Syu log open db_herolist.txt error!!");
    return;
  } else {
    char token[100 * 100];
    memset(token, 0, sizeof(token));
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf), fdb)) {
      buf[strlen(buf)] = 0;
      if (lens + strlen(buf) >= 65535) {
        SaacServer_UpdataStele_send(fd, token);
        memset(token, 0, sizeof(token));
        lens = 0;
      }
      strcat(token, buf);
      lens += strlen(buf);
    }
    fclose(fdb);
    if (strlen(token) > 0)
      SaacServer_UpdataStele_send(fd, token);
  }
}

void Send_S_herolist(char *ocdkey, char *oname, char *ncdkey, char *nname,
                     char *title, int level, int trns, int floor) {
  int i;
  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && gs[i].name[0]) {
      SaacServer_S_UpdataStele_send(i, ocdkey, oname, ncdkey, nname, title,
                                    level, trns, floor);
      logOut("\nSyu log AC Send Single to Gmsv ");
    }
  }
}
#endif

#define MAXUNlockM 50
#define delaytime (60 * 3)
UNLockMenus UNlockM[MAXUNlockM];

int UNlockM_Init(void) {
  int i;
  for (i = 0; i < MAXUNlockM; i++) {
    memset(UNlockM[i].PlayerId, 0, sizeof(UNlockM[i].PlayerId));
    UNlockM[i].use = 0;
    UNlockM[i].time = 0;
  }

  return 1;
}
void reset_UNlockMPlayer(int ti) {
  memset(UNlockM[ti].PlayerId, 0, sizeof(UNlockM[ti].PlayerId));
  UNlockM[ti].use = 0;
  UNlockM[ti].time = 0;
}

int UNlockM_isBe(char *id) {
  int i;
  for (i = 0; i < MAXUNlockM; i++) {
    if (UNlockM[i].use == 0)
      continue;
    if (!strcmp(UNlockM[i].PlayerId, id))
      return 1;
  }
  return 0;
}

// andy add 2002/06/20
int UNlockM_addPlayer(char *id) {
  int i, post = -1;

  for (i = 0; i < MAXUNlockM; i++) {
    if (UNlockM[i].use == 0) {
      post = i;
      continue;
    }
    if (!strcmp(UNlockM[i].PlayerId, id)) {
      reset_UNlockMPlayer(i);
      return -1;
    }
  }
  if (post == -1)
    return -1;

  memset(UNlockM[post].PlayerId, 0, sizeof(UNlockM[post].PlayerId));
  sprintf(UNlockM[post].PlayerId, "%s", id);
  UNlockM[post].use = 1;
  UNlockM[post].time = time(NULL) + delaytime;

  return post;
}

// andy add 2002/06/20
int UNlockM_UnlockPlayer(void) {
  int i, nums = 0;
  static int UnlockClock = 0;

  if ((++UnlockClock) < 60 != 0)
    return 0;

  // logOut(" UNlockM_UnlockPlayer ");

  UnlockClock = 0;
  for (i = 0; i < MAXUNlockM; i++) {
    if (!UNlockM[i].use)
      continue;
    if (UNlockM[i].time >= time(NULL))
      continue;
    if (isLocked(UNlockM[i].PlayerId)) {
      logOut("等待解锁玩家: %s 还需锁定!!\n", UNlockM[i].PlayerId);
    } else {
      logOut("等待解锁玩家: %s 已经解锁!!\n", UNlockM[i].PlayerId);
    }
    reset_UNlockMPlayer(i);
    nums++;
  }
  // logOut( "等待解锁玩家: 总计 %d 个用户解锁 !!\n", nums);
  return nums;
}

#ifdef _ANGEL_SUMMON
void SaacServer_ACMissionTable_recv(int fd, int num, int type, char *data,
                                    char *angelinfo) {
  int i;
  char buf[1024];
  if (type == 1) {   // ask data list
    if (num == -1) { // ask all data
      char alldata[MISSTION_TABLE_SIZE * 100];
      alldata[0] = '\0';
      for (i = 0; i < MISSTION_TABLE_SIZE; i++) {
        if (g_mission_table[i].flag == MISSION_NONE)
          continue;
        sprintf(buf, "%d|%s|%s|%d|%d|%d|%d ", i, g_mission_table[i].angelinfo,
                g_mission_table[i].heroinfo, g_mission_table[i].mission,
                g_mission_table[i].flag, g_mission_table[i].time,
                g_mission_table[i].limittime);
        strcat(alldata, buf);
      }
      SaacServer_ACMissionTable_send(fd, -1, 1, alldata, "");
      return;
    }
  } else if (type == 2) { // add data
    int empty = -1;

    logOut("\n增加精灵召唤任务:%s \n", data);
    for (i = 0; i < MISSTION_TABLE_SIZE; i++) {
      if (g_mission_table[i].angelinfo[0] == '\0') {
        empty = i;
        break;
      }
    }
    if (empty == -1) {
      SaacServer_ACMissionTable_send(fd, -1, 2, "", angelinfo);
      return;
    }

    easyGetTokenFromBuf(data, "|", 1, buf, sizeof(buf));
    strcpy(g_mission_table[empty].angelinfo, buf);
    easyGetTokenFromBuf(data, "|", 2, buf, sizeof(buf));
    strcpy(g_mission_table[empty].heroinfo, buf);
    easyGetTokenFromBuf(data, "|", 3, buf, sizeof(buf));
    g_mission_table[empty].mission = atoi(buf);
    easyGetTokenFromBuf(data, "|", 4, buf, sizeof(buf));
    g_mission_table[empty].flag = atoi(buf);
    g_mission_table[empty].time = (int)time(NULL);
    g_mission_table[empty].limittime = 1;
    mission_table_save();
    sprintf(buf, "%d|%s|%s|%d|%d|%d|%d ", empty,
            g_mission_table[empty].angelinfo, g_mission_table[empty].heroinfo,
            g_mission_table[empty].mission, g_mission_table[empty].flag,
            g_mission_table[empty].time, g_mission_table[empty].limittime);
    for (i = 0; i < MAXCONNECTION; i++) {
      if (gs[i].use && gs[i].name[0]) {
        SaacServer_ACMissionTable_send(i, 1, 1, buf, "");
      }
    }
    SaacServer_ACMissionTable_send(fd, 0, 2, "", angelinfo);
    return;
  } else if (type == 3) { // del data
    mission_table_delete(num);
    mission_table_save();
    return;
  } else if (type == 4) { // change data flag
    char angelinfo[64];
    int limittime = 0;
    logOut("\n增加精灵召唤任务:%s:%d \n", data, num);

    if (num == MISSION_DOING) {
      easyGetTokenFromBuf(data, "|", 1, buf, sizeof(buf));
      strcpy(angelinfo, buf);
      easyGetTokenFromBuf(data, "|", 2, buf, sizeof(buf));
      limittime = atoi(buf);
    } else {
      strcpy(angelinfo, data);
    }

    for (i = 0; i < MISSTION_TABLE_SIZE; i++) {
      if (g_mission_table[i].angelinfo[0] == '\0')
        continue;
      if (strcmp(g_mission_table[i].angelinfo, angelinfo) &&
          strcmp(g_mission_table[i].heroinfo, angelinfo))
        continue;

      g_mission_table[i].flag = num;

      g_mission_table[i].time = time(NULL);
      if (num == MISSION_DOING) {
        g_mission_table[i].limittime = limittime;
      }

      mission_table_save();

      sprintf(buf, "%d|%s|%s|%d|%d|%d|%d ", i, g_mission_table[i].angelinfo,
              g_mission_table[i].heroinfo, g_mission_table[i].mission,
              g_mission_table[i].flag, g_mission_table[i].time,
              g_mission_table[i].limittime);
      for (i = 0; i < MAXCONNECTION; i++) {
        if (gs[i].use && gs[i].name[0]) {
          SaacServer_ACMissionTable_send(i, 1, 1, buf, "");
        }
      }
      return;
    }
  }
}
#endif

#ifdef _TEACHER_SYSTEM
void SaacServer_ACCheckCharacterOnLine_recv(int fd, int charaindex, char *id,
                                            char *name, int flag) {
  LockNode *ln = g_lock[getHash(id) & 0xff];
  // 须要确认名字
  if (name != NULL) {
    while (ln != NULL) {
      if (ln->use != 0) {
        if (strcmp(ln->cdkey, id) == 0 && strcmp(ln->name, name) == 0) {
          SaacServer_ACCheckCharacterOnLine_send(fd, charaindex, 1, ln->server,
                                                 flag);
          return;
        }
      }
      ln = ln->next;
    }
    // 玩家不在线上
    SaacServer_ACCheckCharacterOnLine_send(fd, charaindex, 0, "", flag);
  }
  // 不须要确认名字
  else {
    while (ln != NULL) {
      if (ln->use != 0) {
        if (strcmp(ln->cdkey, id) == 0) {
          SaacServer_ACCheckCharacterOnLine_send(fd, charaindex, 1, ln->server,
                                                 flag);
          return;
        }
      }
      ln = ln->next;
    }
    // 玩家不在线上
    SaacServer_ACCheckCharacterOnLine_send(fd, charaindex, 0, "", flag);
  }
}
#endif

void SaacServer_ACCharLogin_recv(int fd, int clifd, char *id, char *pas,
                                 char *ip
#ifdef _NEWCLISETMAC
                                 ,
                                 char *mac
#endif
) {
  int res;
#ifdef _SASQL
  printf("[SAAC验证] clifd=%d id='%s' pas='%s' ip='%s'\n",
         clifd, id ? id : "NULL", pas ? pas : "NULL", ip ? ip : "NULL");
  if (id == NULL || pas == NULL || ip == NULL || strlen(id) == 0 ||
      strlen(id) >= USERID_MAX || strlen(pas) == 0 ||
      strlen(pas) > SAAC_PASSWORD_MAX || strlen(ip) == 0 ||
      strlen(ip) > SAAC_IP_MAX
#ifdef _NEWCLISETMAC
      || mac == NULL || strlen(mac) > SAAC_MAC_MAX
#endif
  ) {
    printf("[SAAC验证失败] 登陆信息有错误 id='%s' pas='%s' ip='%s'\n",
           id ? id : "NULL", pas ? pas : "NULL", ip ? ip : "NULL");
    SaacServer_ACCharLogin_send(fd, clifd, 1);
    return;
  }
  printf("[SAAC检查] 账号锁定检查 id='%s'\n", id);
  if (sasql_check_lock(id)) {
    printf("[SAAC验证失败] 该账号%s禁止登陆!\n", id);
    SaacServer_ACCharLogin_send(fd, clifd, 2);
    return;
  }
  printf("[SAAC检查] IP锁定检查 ip='%s'\n", ip);
  if (sasql_check_lock(ip)) {
    printf("[SAAC验证失败] 该IP%s禁止登陆!\n", ip);
    SaacServer_ACCharLogin_send(fd, clifd, 3);
    return;
  }
  if (strlen(mac) > 0) {
    printf("[SAAC检查] MAC锁定检查 mac='%s'\n", mac);
    if (sasql_check_lock(mac)) {
      printf("[SAAC验证失败] 该MAC%s禁止登陆!\n", mac);
      SaacServer_ACCharLogin_send(fd, clifd, 3);
      return;
    }
  }
  printf("[SAAC验证] 调用sasql_query id='%s'\n", id);
  res = sasql_query(id, pas);
  printf("[SAAC验证] sasql_query结果 res=%d (0=失败,1=成功,2=密码错,3=未注册)\n", res);
  if (res == 3) {
#ifdef _SQL_REGISTER
    printf("[SAAC验证] 账号未注册, 尝试自动注册 id='%s'\n", id);
    if (!sasql_register(id, pas))
#endif
    {
      printf("[SAAC验证失败] 账号未注册且自动注册失败 id='%s'\n", id);
      SaacServer_ACCharLogin_send(fd, clifd, 5);
      return;
    }
  } else if (res != 1) {
#ifdef _ITEM_PET_LOCKED
    if (sasql_ItemPetLocked_Char(id, pas) != 1)
#endif
    {
      printf("[SAAC验证失败] 密码错误 id='%s'\n", id);
      SaacServer_ACCharLogin_send(fd, clifd, 6);
      return;
    }
  }
  printf("[SAAC验证成功] id='%s' 记录在线信息\n", id);
  sasql_online(id, NULL, ip, mac, 1);
#endif
  printf("[SAAC回复] clifd=%d flag=0 (登录成功)\n", clifd);
  SaacServer_ACCharLogin_send(fd, clifd, 0);
}

#ifdef _SASQL
void SaacServer_LockLogin_recv(int fd, char *id, char *ip, int flag) {
  switch (flag) {
  case 0:
    if (strlen(id) > 0) {
      if (sasql_del_lock(id)) {
        logOut("成功解除锁定账号:%s\n", id);
      } else {
        logOut("失败！解除锁定账号:%s\n", id);
      }
    }
    if (strlen(ip) > 0) {
      if (sasql_del_lock(ip)) {
        logOut("成功解除锁定IP:%s\n", ip);
      } else {
        logOut("失败！解除锁定IP:%s\n", ip);
      }
    }
    break;
  case 1:
    if (strlen(id) > 0) {
      if (!sasql_check_lock(id)) {
        if (sasql_add_lock(id)) {
          logOut("成功锁定账号:%s\n", id);
        } else {
          logOut("失败！锁定账号:%s\n", id);
        }
      } else {
        logOut("账号%s已被锁定过！\n", id);
      }
    }
    if (strlen(ip) > 0) {
      if (!sasql_check_lock(ip)) {
        if (sasql_add_lock(ip)) {
          logOut("成功锁定IP:%s\n", ip);
        } else {
          logOut("失败！锁定IP:%s\n", ip);
        }
      } else {
        logOut("IP%s已被锁定过！\n", id);
      }
    }
    break;
  default:
    break;
  }
}
#ifdef _NEW_VIP_SHOP
void SaacServer_QueryPoint_recv(int fd, int clifd, char *id) {
  int point = sasql_query_point(id);

  SaacServer_QueryPoint_send(fd, clifd, point);
}

void SaacServer_NewVipShop_recv(int fd, int clifd, char *id, int point,
                                char *buf, int flag) {
  int vippoint = sasql_add_vippoint(id, point);
  if (vippoint == -1) {
    SaacServer_NewVipShop_send(fd, clifd, vippoint, "", flag);
  } else {
    SaacServer_NewVipShop_send(fd, clifd, vippoint, buf, flag);
  }
}

#ifdef _COST_ITEM
void SaacServer_CostItem_recv(int fd, int clifd, char *id, int point) {
  int vippoint = sasql_add_vippoint(id, point);

  SaacServer_QueryPoint_send(fd, clifd, vippoint);
}
#endif
#endif

#ifdef _ITEM_PET_LOCKED
void SaacServer_ItemPetLocked_recv(int fd, int clifd, char *id,
                                   char *safepasswd) {
  char *data = sasql_ItemPetLocked(id, safepasswd);
  if (strcmp(data, "安全锁已经成功解锁！") == 0) {
    SaacServer_ItemPetLocked_send(fd, clifd, 1, data);
  } else if (strcmp(data, "您还未设置安全锁解锁密码，为了确保安全，请输入一次六"
                          "位以上密码做为安全锁密码并牢牢记住！") == 0) {
    SaacServer_ItemPetLocked_send(fd, clifd, 0, data);
  } else {
    SaacServer_ItemPetLocked_send(fd, clifd, -1, data);
  }
}
void SaacServer_ItemPetLockedPasswd_recv(int fd, int clifd, char *id,
                                         char *safepasswd) {
  char *data = sasql_ItemPetLocked_Passwd(id, safepasswd);
  SaacServer_ItemPetLockedPasswd_send(fd, clifd, data);
}
#endif

#ifdef _ONLINE_COST
void SaacServer_OnlineCost_recv(int fd, int clifd, char *id, char *costpasswd,
                                int fmindex, char *fmname) {
  char *data = sasql_OnlineCost(id, costpasswd, fmindex, fmname);
  SaacServer_OnlineCost_send(fd, clifd, data);
}
#endif

#ifdef _SQL_BUY_FUNC
void SaacServer_OnlineBuy_recv(int fd, int clifd, char *id, char *costpasswd) {
  char *data = sasql_OnlineBuy(id, costpasswd);
  SaacServer_OnlineBuy_send(fd, clifd, data);
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void SaacServer_OldToNew_recv(int fd, int clifd, char *id, int point) {
  int vippoint = sasql_add_vippoint(id, point);
  char data[256];
  sprintf(data, "旧会员点%d已转换至SQL里, 你当前会员点共%d", point, vippoint);
  SaacServer_OldToNew_send(fd, clifd, data);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void SaacServer_FormulateAutoPk_recv(int fd, int clifd, char *id, int point) {
  int pkpoint = sasql_add_FormulateAutoPk(id, point);
  char data[256];
  if (pkpoint == -1) {
    sprintf(data, "您的乱舞积分%d无法转换进SQL里, 请与管理员联系！", point);
  } else {
    sprintf(data, "已将您的乱舞积分%d已转换至SQL里, 你当前乱舞积分共%d", point,
            pkpoint);
  }
  SaacServer_FormulateAutoPk_send(fd, clifd, data);
}
#endif

#ifdef _LOTTERY_SYSTEM
/*彩票*/
void SaacServer_LotterySystem_recv(int fd) {
  char data[256] = {"-1,-1,-1,-1,-1,-1,-1"};
  FILE *f1 = fopen("todayaward.txt", "r+");
  if (f1) {
    if (fgets(data, sizeof(data), f1)) {
      SaacServer_LotterySystem_send(fd, data);
    }
  }
}
#endif

#ifdef _ALL_SERV_SEND
void SaacServer_AllServSend_recv(char *data) {
  int i;
  for (i = 0; i < MAXCONNECTION; i++)
    if (gs[i].use && gs[i].name[0])
      SaacServer_AllServSend_send(i, data);
}
#endif

#endif
