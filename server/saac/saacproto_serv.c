#define _SAACPROTOSERV_C_
#include "saacproto_serv.h"
#include "main.h"

// Nuke : This 'fd' is not a socket, it's the index of structure connect
int saacproto_ServerDispatchMessage(int fd, char *encoded, char *debugfun) {
  unsigned int msgid;
  char funcname[1024];
  saacproto_splitString(encoded);
  GetMessageInfo(&msgid, funcname, sizeof(funcname), saacproto.token_list);

  if (strcmp(funcname, "ACServerLogin") == 0) {
    char *servername;
    char *serverpas;
#if _ATTESTAION_ID == 1
    int id;
    id = demkstr_int(saacproto.token_list[2]);
    servername = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    serverpas = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    saacproto_ACServerLogin_recv(fd, id, servername, serverpas);
    return 0;
#else
    servername = saacproto_wrapStringAddr(
        saacproto_stringwrapper[1], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[2]));
    serverpas = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    saacproto_ACServerLogin_recv(fd, servername, serverpas);
    return 0;
#endif
  }

  if (strcmp(funcname, "ACServerLogout") == 0) {
    saacproto_ACServerLogout_recv(fd);
    return 0;
  }

  if (strcmp(funcname, "ACCharList") == 0) {
    char *id;
    char *pas;
    char *ip;
    char *mac;
    int mesgid;
    int charlistflg;

    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    pas = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    ip = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[4]));
    mac = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[5]));
    mesgid = demkstr_int(saacproto.token_list[6]);
    charlistflg = demkstr_int(saacproto.token_list[7]);
    saacproto_ACCharList_recv(fd, id, pas, ip, mac, mesgid, charlistflg);
    return 0;
  }

  if (strcmp(funcname, "ACCharLoad") == 0) {
    char *id;
    char *pas;
    char *charname;
    int lock;
    char *opt;
    int mesgid;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    pas = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    lock = demkstr_int(saacproto.token_list[5]);
    opt = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[6]));
    mesgid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACCharLoad_recv(fd, id, pas, charname, lock, opt, mesgid);
    return 0;
  }

  if (strcmp(funcname, "ACCharSave") == 0) {
    char *id;
    char *charname;
    char *opt;
    char *charinfo;
    int unlock;
    int mesgid;
#ifdef _NewSave
    int charindex;
#endif
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    opt = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[4]));
    charinfo = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    unlock = demkstr_int(saacproto.token_list[6]);
    mesgid = demkstr_int(saacproto.token_list[7]);

#ifdef _NewSave
    charindex = demkstr_int(saacproto.token_list[8]);
    saacproto_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid,
                              charindex);
#else
    saacproto_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid);
#endif
    return 0;
  }

  if (strcmp(funcname, "ACCharDelete") == 0) {
    char *id;
    char *passwd;
    char *charname;
    char *option;
    int mesgid;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    passwd = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[3]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    option = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[5]));
    mesgid = demkstr_int(saacproto.token_list[6]);
    saacproto_ACCharDelete_recv(fd, id, passwd, charname, option, mesgid);
    return 0;
  }

  if (strcmp(funcname, "ACLock") == 0) {
    char *id;
    int lock;
    int mesgid;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    lock = demkstr_int(saacproto.token_list[3]);
    mesgid = demkstr_int(saacproto.token_list[4]);
    saacproto_ACLock_recv(fd, id, lock, mesgid);
    return 0;
  }

#ifdef _WAEI_KICK
  if (strcmp(funcname, "ACKick") == 0) {
    char *id;  // 欲踢的帐号
    int flg;   // 事件flg
    int clifd; // GM
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    clifd = demkstr_int(saacproto.token_list[3]);
    flg = demkstr_int(saacproto.token_list[4]);
    saacproto_ACKick_recv(fd, id, flg, clifd);
    return 0;
  }
#endif

  if (strcmp(funcname, "ACUCheck") == 0) {
    char *mem_id;
    int status;
    mem_id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    status = demkstr_int(saacproto.token_list[3]);
    saacproto_ACUCheck_recv(fd, mem_id, status);
    return 0;
  }

  if (strcmp(funcname, "DBUpdateEntryString") == 0) {
    char *table;
    char *key;
    char *value;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    value = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[4]));
    msgid = demkstr_int(saacproto.token_list[5]);
    msgid2 = demkstr_int(saacproto.token_list[6]);
    saacproto_DBUpdateEntryString_recv(fd, table, key, value, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBDeleteEntryString") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    msgid = demkstr_int(saacproto.token_list[4]);
    msgid2 = demkstr_int(saacproto.token_list[5]);
    saacproto_DBDeleteEntryString_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryString") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    msgid = demkstr_int(saacproto.token_list[4]);
    msgid2 = demkstr_int(saacproto.token_list[5]);
    saacproto_DBGetEntryString_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBUpdateEntryInt") == 0) {
    char *table;
    char *key;
    int value;
    char *info;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    value = demkstr_int(saacproto.token_list[4]);
    info = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[5]));
    msgid = demkstr_int(saacproto.token_list[6]);
    msgid2 = demkstr_int(saacproto.token_list[7]);
    saacproto_DBUpdateEntryInt_recv(fd, table, key, value, info, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryRank") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    msgid = demkstr_int(saacproto.token_list[4]);
    msgid2 = demkstr_int(saacproto.token_list[5]);
    saacproto_DBGetEntryRank_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
  if (strcmp(funcname, "UpdataStele") == 0) {
    char *cdkey;
    char *name;
    char *title;
    int level;
    int trns;
    int time;
    int floor;
    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    name = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[3]));
    title = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[4]));
    level = demkstr_int(saacproto.token_list[5]);
    trns = demkstr_int(saacproto.token_list[6]);
    time = demkstr_int(saacproto.token_list[7]);
    floor = demkstr_int(saacproto.token_list[8]);
    saacproto_UpdataStele_recv(fd, cdkey, name, title, level, trns, time,
                               floor);
    return 0;
  }
#endif

  if (strcmp(funcname, "DBDeleteEntryInt") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    msgid = demkstr_int(saacproto.token_list[4]);
    msgid2 = demkstr_int(saacproto.token_list[5]);
    saacproto_DBDeleteEntryInt_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryInt") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    key = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[3]));
    msgid = demkstr_int(saacproto.token_list[4]);
    msgid2 = demkstr_int(saacproto.token_list[5]);
    saacproto_DBGetEntryInt_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByRank") == 0) {
    char *table;
    int rank_start;
    int rank_end;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    rank_start = demkstr_int(saacproto.token_list[3]);
    rank_end = demkstr_int(saacproto.token_list[4]);
    msgid = demkstr_int(saacproto.token_list[5]);
    msgid2 = demkstr_int(saacproto.token_list[6]);
    saacproto_DBGetEntryByRank_recv(fd, table, rank_start, rank_end, msgid,
                                    msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByCount") == 0) {
    char *table;
    int count_start;
    int num;
    int msgid;
    int msgid2;
    table = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    count_start = demkstr_int(saacproto.token_list[3]);
    num = demkstr_int(saacproto.token_list[4]);
    msgid = demkstr_int(saacproto.token_list[5]);
    msgid2 = demkstr_int(saacproto.token_list[6]);
    saacproto_DBGetEntryByCount_recv(fd, table, count_start, num, msgid,
                                     msgid2);
    return 0;
  }

  if (strcmp(funcname, "Broadcast") == 0) {
    char *id;
    char *charname;
    char *message;
    int flag;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    message = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                       saacproto.work_buf_size,
                                       demkstr_string(saacproto.token_list[4]));
    flag = demkstr_int(saacproto.token_list[5]);
    saacproto_Broadcast_recv(fd, id, charname, message, flag);

    return 0;
  }

  if (strcmp(funcname, "Message") == 0) {
    char *id_from;
    char *charname_from;
    char *id_to;
    char *charname_to;
    char *message;
    int option;
    id_from = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                       saacproto.work_buf_size,
                                       demkstr_string(saacproto.token_list[2]));
    charname_from = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    id_to = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[4]));
    charname_to = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    message = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                       saacproto.work_buf_size,
                                       demkstr_string(saacproto.token_list[6]));
    option = demkstr_int(saacproto.token_list[7]);
    saacproto_Message_recv(fd, id_from, charname_from, id_to, charname_to,
                           message, option);
    return 0;
  }

  if (strcmp(funcname, "MessageAck") == 0) {
    char *id;
    char *charname;
    char *result;
    int mesgid;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    result = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[4]));
    mesgid = demkstr_int(saacproto.token_list[5]);
    saacproto_MessageAck_recv(fd, id, charname, result, mesgid);
    return 0;
  }

  if (strcmp(funcname, "MessageFlush") == 0) {
    char *id;
    char *charname;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    saacproto_MessageFlush_recv(fd, id, charname);
    return 0;
  }
  // 成立家族
  if (strcmp(funcname, "ACAddFM") == 0) {
    int fmleaderlv, fmsprite, charfdid, fmleadergrano;
    char *fmname, *fmleadername, *fmleaderid, *petname, *petattr, *fmrule;
#ifdef _PERSONAL_FAME
    int fame;
#endif
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmleadername = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    fmleaderid = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    fmleaderlv = demkstr_int(saacproto.token_list[5]);
    petname = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                       saacproto.work_buf_size,
                                       demkstr_string(saacproto.token_list[6]));
    petattr = saacproto_wrapStringAddr(saacproto_stringwrapper[6],
                                       saacproto.work_buf_size,
                                       demkstr_string(saacproto.token_list[7]));
    fmrule = saacproto_wrapStringAddr(saacproto_stringwrapper[7],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[8]));
    fmsprite = demkstr_int(saacproto.token_list[9]);
    fmleadergrano = demkstr_int(saacproto.token_list[10]);
#ifdef _PERSONAL_FAME
    fame = demkstr_int(saacproto.token_list[11]);
    charfdid = demkstr_int(saacproto.token_list[12]);
#ifdef _FAMILYBADGE_
    int fmbadge = demkstr_int(saacproto.token_list[13]);
#endif
    saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid, fmleaderlv,
                           petname, petattr, fmrule, fmsprite, fmleadergrano,
                           fame
#ifdef _FAMILYBADGE_
                           ,
                           fmbadge
#endif
                           ,
                           charfdid);
#else
    charfdid = demkstr_int(saacproto.token_list[11]);
    saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid, fmleaderlv,
                           petname, petattr, fmrule, fmsprite, fmleadergrano,
                           charfdid);
#endif
    return 0;
  }
  // 加入家族
  if (strcmp(funcname, "ACJoinFM") == 0) {
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
    int fmindex, charlv, charfdid, index, fame;
#else
    int fmindex, charlv, charfdid, index;
#endif
    char *fmname, *charname, *charid;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[5]));
    charlv = demkstr_int(saacproto.token_list[6]);
    index = demkstr_int(saacproto.token_list[7]);
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
    fame = demkstr_int(saacproto.token_list[8]);
    charfdid = demkstr_int(saacproto.token_list[9]);
    saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname, charid, charlv,
                            index, fame, charfdid);
#else
    charfdid = demkstr_int(saacproto.token_list[8]);
    saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname, charid, charlv,
                            index, charfdid);
#endif
    return 0;
  }
  // 退出家族
  if (strcmp(funcname, "ACLeaveFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname, *charname, *charid;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[5]));
    index = demkstr_int(saacproto.token_list[6]);
    charfdid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACLeaveFM_recv(fd, fmname, fmindex, charname, charid, index,
                             charfdid);
    return 0;
  }
  // 解散家族
#ifdef _LEADERFUNCHECK
  if (strcmp(funcname, "ACDelFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname, *charname, *charid;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[6]));
    charfdid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charname, charid,
                           charfdid);
    return 0;
  }
#else
  if (strcmp(funcname, "ACDelFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    charfdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charfdid);
    return 0;
  }
#endif
  // 列出家族列表
  if (strcmp(funcname, "ACShowFMList") == 0) {
    saacproto_ACShowFMList_recv(fd);
    return 0;
  }
#ifdef _FAMILY_TOTEM
  // 列出家族图腾
  if (strcmp(funcname, "ACShowFMTotem") == 0) {
    saacproto_ACShowFMTotem_recv(fd);
    return 0;
  }
#endif
  // 列出成员列表
  if (strcmp(funcname, "ACShowMemberList") == 0) {
    int index;
    char *tmpbuf;
    tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    index = demkstr_int(saacproto.token_list[3]);
    saacproto_ACShowMemberList_recv(fd, index);
    return 0;
  }
  // 列出家族详细资料
  if (strcmp(funcname, "ACFMDetail") == 0) {
    int fmindex, charfdid, index;
    char *fmname;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    charfdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACFMDetail_recv(fd, fmname, fmindex, index, charfdid);
    return 0;
  }
  // 列出家族留言
  if (strcmp(funcname, "ACFMReadMemo") == 0) {
    int index;
    char *tmpbuf;
    tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    index = demkstr_int(saacproto.token_list[3]);
    saacproto_ACFMReadMemo_recv(fd, index);
    return 0;
  }
  // 写入家族留言
  if (strcmp(funcname, "ACFMWriteMemo") == 0) {
    int index, fmindex;
    char *fmname, *data;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[4]));
    index = demkstr_int(saacproto.token_list[5]);
    saacproto_ACFMWriteMemo_recv(fd, fmname, fmindex, data, index);
    return 0;
  }

  // 玩家login
  if (strcmp(funcname, "ACFMCharLogin") == 0) {
    int fmindex, charlv, charfdid;
#ifdef _FM_MODIFY
    int gsnum;
#endif
    char *fmname, *charname, *charid;
#ifdef _FMVER21
    int eventflag;
#endif
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[5]));
    charlv = demkstr_int(saacproto.token_list[6]);

#ifdef _FMVER21
    eventflag = demkstr_int(saacproto.token_list[7]);
    charfdid = demkstr_int(saacproto.token_list[8]);
#ifdef _FM_MODIFY
    gsnum = demkstr_int(saacproto.token_list[9]);
    saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                 eventflag, charfdid, gsnum);
#else
    saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                 eventflag, charfdid);
#endif
#else
    charfdid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                 charfdid);
#endif
    return 0;
  }
  // 玩家logout
  if (strcmp(funcname, "ACFMCharLogout") == 0) {
    int fmindex, charfdid, charlv, index;
    char *fmname, *charname, *charid;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[5]));
    charlv = demkstr_int(saacproto.token_list[6]);
    index = demkstr_int(saacproto.token_list[7]);
    charfdid = demkstr_int(saacproto.token_list[8]);
    saacproto_ACFMCharLogout_recv(fd, fmname, fmindex, charname, charid, charlv,
                                  index, charfdid);
    return 0;
  }
  // 族长审核成员加入家族、修改家族成员职位
  if (strcmp(funcname, "ACMemberJoinFM") == 0) {
    int fmindex, charfdid, index, charindex, result;
    char *fmname, *charname;
#ifdef _FMVER21
    int meindex;
#endif
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charindex = demkstr_int(saacproto.token_list[5]);
    index = demkstr_int(saacproto.token_list[6]);
    result = demkstr_int(saacproto.token_list[7]);
#ifdef _FMVER21
    meindex = demkstr_int(saacproto.token_list[8]);
    charfdid = demkstr_int(saacproto.token_list[9]);
    saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname, charindex,
                                  index, result, meindex, charfdid);
#else
    charfdid = demkstr_int(saacproto.token_list[8]);
    saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname, charindex,
                                  index, result, charfdid);
#endif
    return 0;
  }
  // 族长审核成员离开家族
  if (strcmp(funcname, "ACMemberLeaveFM") == 0) {
    int fmindex, charfdid, index, charindex;
    char *fmname, *charname;
#ifdef _FMVER21
    int meindex;
#endif
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    charname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charindex = demkstr_int(saacproto.token_list[5]);
    index = demkstr_int(saacproto.token_list[6]);
#ifdef _FMVER21
    meindex = demkstr_int(saacproto.token_list[7]);
    charfdid = demkstr_int(saacproto.token_list[8]);
    saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname, charindex,
                                   index, meindex, charfdid);
#else
    charfdid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname, charindex,
                                   index, charfdid);
#endif
    return 0;
  }
  // 列出家族据点
  if (strcmp(funcname, "ACFMPointList") == 0) {
    saacproto_ACFMPointList_recv(fd);
    return 0;
  }

  // 申请家族据点
  if (strcmp(funcname, "ACSetFMPoint") == 0) {
    int fl, x, y, fmindex, index, fmpointindex, charfdid;
    char *fmname;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    fmpointindex = demkstr_int(saacproto.token_list[5]);
    fl = demkstr_int(saacproto.token_list[6]);
    x = demkstr_int(saacproto.token_list[7]);
    y = demkstr_int(saacproto.token_list[8]);
    charfdid = demkstr_int(saacproto.token_list[9]);
    saacproto_ACSetFMPoint_recv(fd, fmname, fmindex, index, fmpointindex, fl, x,
                                y, charfdid);
    return 0;
  }
  // 设定家族据点
  if (strcmp(funcname, "ACFixFMPoint") == 0) {
    int winindex, winfmindex, loseindex, losefmindex, village;
    char *winfmname, *losefmname;
    winfmname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[1], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[2]));
    winfmindex = demkstr_int(saacproto.token_list[3]);
    winindex = demkstr_int(saacproto.token_list[4]);
    losefmname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    losefmindex = demkstr_int(saacproto.token_list[6]);
    loseindex = demkstr_int(saacproto.token_list[7]);
    village = demkstr_int(saacproto.token_list[8]);
    saacproto_ACFixFMPoint_recv(fd, winfmname, winfmindex, winindex, losefmname,
                                losefmindex, loseindex, village);
    return 0;
  }
  // 族长对全员广播
  if (strcmp(funcname, "ACFMAnnounce") == 0) {
    int index, fmindex, color;
    char *fmname, *data;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[5]));
    color = demkstr_int(saacproto.token_list[6]);
    saacproto_ACFMAnnounce_recv(fd, fmname, fmindex, index, data, color);
    return 0;
  }
  // 列出家族排行榜
  if (strcmp(funcname, "ACShowTopFMList") == 0) {
    int kindflag;
    char *tmpbuf;
    tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    kindflag = demkstr_int(saacproto.token_list[3]);
    saacproto_ACShowTopFMList_recv(fd, kindflag);
    return 0;
  }
  // 修正家族资料
  if (strcmp(funcname, "ACFixFMData") == 0) {
    int fmindex, charfdid, index, kindflag, charindex;
    char *fmname, *data1, *data2;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    kindflag = demkstr_int(saacproto.token_list[5]);
    data1 = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[6]));
    data2 = saacproto_wrapStringAddr(saacproto_stringwrapper[6],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[7]));
    charindex = demkstr_int(saacproto.token_list[8]);
    charfdid = demkstr_int(saacproto.token_list[9]);
    saacproto_ACFixFMData_recv(fd, fmname, fmindex, index, kindflag, data1,
                               data2, charindex, charfdid);
    return 0;
  }

  // 家族ＰＫ後修正声望
  if (strcmp(funcname, "ACFixFMPK") == 0) {
    int winindex, winfmindex, loseindex, losefmindex;
    char *winfmname, *losefmname;
    winfmname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[1], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[2]));
    winfmindex = demkstr_int(saacproto.token_list[3]);
    winindex = demkstr_int(saacproto.token_list[4]);
    losefmname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    losefmindex = demkstr_int(saacproto.token_list[6]);
    loseindex = demkstr_int(saacproto.token_list[7]);
    saacproto_ACFixFMPK_recv(fd, winfmname, winfmindex, winindex, losefmname,
                             losefmindex, loseindex);
    return 0;
  }
  // GM修正家族资料
  if (strcmp(funcname, "ACGMFixFMData") == 0) {
    int index, charfdid;
    char *cmd, *data, *tmpbuf, *charid;
    tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    index = demkstr_int(saacproto.token_list[3]);
    charid = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[4]));
    cmd = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[5]));
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[6]));
    charfdid = demkstr_int(saacproto.token_list[7]);
    saacproto_ACGMFixFMData_recv(fd, index, charid, cmd, data, charfdid);
    return 0;
  }
  // 取得家族资料
  if (strcmp(funcname, "ACGetFMData") == 0) {
    int fmindex, charfdid, index, kindflag;
    char *fmname;
    fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                      saacproto.work_buf_size,
                                      demkstr_string(saacproto.token_list[2]));
    fmindex = demkstr_int(saacproto.token_list[3]);
    index = demkstr_int(saacproto.token_list[4]);
    kindflag = demkstr_int(saacproto.token_list[5]);
    charfdid = demkstr_int(saacproto.token_list[6]);
    saacproto_ACGetFMData_recv(fd, fmname, fmindex, index, kindflag, charfdid);
    return 0;
  }
  // 广播踢馆人的资料
  if (strcmp(funcname, "ACManorPKAck") == 0) {
    char *data;
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[2]));
    saacproto_ACManorPKAck_recv(fd, data);
    return 0;
  }

  if (strcmp(funcname, "ACreLoadFmData") == 0) {
    int type, data;
    type = demkstr_int(saacproto.token_list[2]);
    data = demkstr_int(saacproto.token_list[3]);
    saacproto_ACreLoadFmData_recv(fd, type, data);
    return 0;
  }

#ifdef _AC_SEND_FM_PK
  if (strcmp(funcname, "ACLoadFmPk") == 0) {
    int fmpks_pos;
    fmpks_pos = demkstr_int(saacproto.token_list[2]);
    saacproto_ACLoadFmPk_recv(fd, fmpks_pos);
    return 0;
  }
#ifdef _ACFMPK_LIST
  if (strcmp(funcname, "ACSendFmPk") == 0) {
    int fmpks_pos, userindex, flg;
    char *data;
    fmpks_pos = demkstr_int(saacproto.token_list[2]);
    userindex = demkstr_int(saacproto.token_list[3]);
    flg = demkstr_int(saacproto.token_list[4]);
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[5]));
    saacproto_ACSendFmPk_recv(fd, fmpks_pos, userindex, flg, data);
    return 0;
  }
#else
  if (strcmp(funcname, "ACSendFmPk") == 0) {
    int fmpks_pos;
    char *data;
    fmpks_pos = demkstr_int(saacproto.token_list[2]);
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[3]));
    saacproto_ACSendFmPk_recv(fd, fmpks_pos, data);
    return 0;
  }
#endif

#endif

#ifdef _CHAR_POOLITEM
  if (strcmp(funcname, "ACInsertPoolItem") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    Pooldataarg = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    userindex = demkstr_int(saacproto.token_list[4]);
    clifdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACCharInsertPoolItem_recv(fd, cdkey, userindex, clifdid,
                                        Pooldataarg);
    return 0;
  }
  if (strcmp(funcname, "ACSavePoolItem") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    Pooldataarg = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    userindex = demkstr_int(saacproto.token_list[4]);
    clifdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACCharSavePoolItem_recv(fd, cdkey, userindex, clifdid,
                                      Pooldataarg);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolItem") == 0) {
    char *cdkey;
    int clifdid, userindex, npcid;
    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    userindex = demkstr_int(saacproto.token_list[3]);
    clifdid = demkstr_int(saacproto.token_list[4]);
    npcid = demkstr_int(saacproto.token_list[5]);

    saacproto_ACCharGetPoolItem_recv(fd, cdkey, userindex, clifdid, npcid);
    return 0;
  }
#endif

#ifdef _CHAR_POOLPET
  if (strcmp(funcname, "ACInsertPoolPet") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    Pooldataarg = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    userindex = demkstr_int(saacproto.token_list[4]);
    clifdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACCharInsertPoolPet_recv(fd, cdkey, userindex, clifdid,
                                       Pooldataarg);
    return 0;
  }
  if (strcmp(funcname, "ACSavePoolPet") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    Pooldataarg = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    userindex = demkstr_int(saacproto.token_list[4]);
    clifdid = demkstr_int(saacproto.token_list[5]);
    saacproto_ACCharSavePoolPet_recv(fd, cdkey, userindex, clifdid,
                                     Pooldataarg);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolPet") == 0) {
    char *cdkey;
    int clifdid, userindex, npcid;
    cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                     saacproto.work_buf_size,
                                     demkstr_string(saacproto.token_list[2]));
    userindex = demkstr_int(saacproto.token_list[3]);
    clifdid = demkstr_int(saacproto.token_list[4]);
    npcid = demkstr_int(saacproto.token_list[5]);

    saacproto_ACCharGetPoolPet_recv(fd, cdkey, userindex, clifdid, npcid);
    return 0;
  }
#endif

#ifdef _ANGEL_SUMMON
  if (strcmp(funcname, "ACMissionTable") == 0) {
    int num;
    int type;
    char *data;
    // int charaindex;
    char *angelinfo;

    num = demkstr_int(saacproto.token_list[2]);
    type = demkstr_int(saacproto.token_list[3]);
    data = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[4]));
    // charaindex = demkstr_int( saacproto.token_list[5] );
    angelinfo = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));

    saacproto_ACMissionTable_recv(fd, num, type, data, angelinfo);
    return 0;
  }
#endif

#ifdef _TEACHER_SYSTEM
  if (strcmp(funcname, "ACCheckCharacterOnLine") == 0) {
    int charaindex, flag;
    char *id;
    char *name;

    charaindex = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    name = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                    saacproto.work_buf_size,
                                    demkstr_string(saacproto.token_list[4]));
    flag = demkstr_int(saacproto.token_list[5]);

    saacproto_ACCheckCharacterOnLine_recv(fd, charaindex, id, name, flag);
    return 0;
  }
#endif

  if (strcmp(funcname, "ACCharLogin") == 0) {
    int clifd;
    char *id, *pas, *ip;

    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    pas = saacproto_wrapStringAddr(saacproto_stringwrapper[3],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[4]));
    ip = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[5]));
#ifdef _NEWCLISETMAC
    char *mac;
    mac = saacproto_wrapStringAddr(saacproto_stringwrapper[5],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[6]));
    saacproto_ACCharLogin_recv(fd, clifd, id, pas, ip, mac);
#else
    saacproto_ACCharLogin_recv(fd, clifd, id, pas, ip);
#endif
    return 0;
  }

  if (strcmp(funcname, "LockLogin") == 0) {
    int flag;
    char *id, *ip;
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[1],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[2]));
    ip = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    flag = demkstr_int(saacproto.token_list[4]);
    saacproto_LockLogin_recv(fd, id, ip, flag);
    return 0;
  }

#ifdef _NEW_VIP_SHOP
  if (strcmp(funcname, "QueryPoint") == 0) {
    int clifd;
    char *id;

    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    saacproto_QueryPoint_recv(fd, clifd, id);
    return 0;
  }

  if (strcmp(funcname, "NewVipShop") == 0) {
    int clifd;
    int point;
    char *id;
    char *buf;
    int flag;
    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    point = demkstr_int(saacproto.token_list[4]);
    buf = saacproto_wrapStringAddr(saacproto_stringwrapper[4],
                                   saacproto.work_buf_size,
                                   demkstr_string(saacproto.token_list[5]));
    flag = demkstr_int(saacproto.token_list[6]);
    saacproto_NewVipShop_recv(fd, clifd, id, point, buf, flag);
    return 0;
  }

#ifdef _COST_ITEM
  if (strcmp(funcname, "CostItem") == 0) {
    int clifd;
    char *id;
    int point;
    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    point = demkstr_int(saacproto.token_list[4]);
    saacproto_CostItem_recv(fd, clifd, id, point);
    return 0;
  }
#endif
#endif

#ifdef _ITEM_PET_LOCKED
  if (strcmp(funcname, "ItemPetLocked") == 0) {
    int clifd;
    char *id;
    char *safepasswd;

    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    safepasswd = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    saacproto_ItemPetLocked_recv(fd, clifd, id, safepasswd);
    return 0;
  }

  if (strcmp(funcname, "ItemPetLockedPasswd") == 0) {
    int clifd;
    char *id;
    char *safepasswd;

    clifd = demkstr_int(saacproto.token_list[2]);
    id = saacproto_wrapStringAddr(saacproto_stringwrapper[2],
                                  saacproto.work_buf_size,
                                  demkstr_string(saacproto.token_list[3]));
    safepasswd = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    saacproto_ItemPetLockedPasswd_recv(fd, clifd, id, safepasswd);
    return 0;
  }
#endif

#ifdef _ONLINE_COST
  if (strcmp(funcname, "OnlineCost") == 0) {
    int clifd = demkstr_int(saacproto.token_list[2]);
    char *id = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    char *costpasswd = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    int fmindex = demkstr_int(saacproto.token_list[5]);
    char *fmname = saacproto_wrapStringAddr(
        saacproto_stringwrapper[5], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[6]));
    saacproto_OnlineCost_recv(fd, clifd, id, costpasswd, fmindex, fmname);
    return 0;
  }
#endif

#ifdef _SQL_BUY_FUNC
  if (strcmp(funcname, "OnlineBuy") == 0) {
    int clifd = demkstr_int(saacproto.token_list[2]);
    char *id = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    char *costpasswd = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    saacproto_OnlineBuy_recv(fd, clifd, id, costpasswd);
    return 0;
  }
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
  if (strcmp(funcname, "OldToNew") == 0) {
    int clifd = demkstr_int(saacproto.token_list[2]);
    char *id = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    int point = demkstr_int(saacproto.token_list[4]);
    saacproto_OldToNew_recv(fd, clifd, id, point);
    return 0;
  }
#endif

#ifdef _FORMULATE_AUTO_PK
  if (strcmp(funcname, "FormulateAutoPk") == 0) {
    int clifd = demkstr_int(saacproto.token_list[2]);
    char *id = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    int point = demkstr_int(saacproto.token_list[4]);
    saacproto_FormulateAutoPk_recv(fd, clifd, id, point);
    return 0;
  }
#endif

#ifdef _LOTTERY_SYSTEM
  if (strcmp(funcname, "LotterySystem") == 0) {
    saacproto_LotterySystem_recv(fd);
    return 0;
  }
#endif

#ifdef _ALL_SERV_SEND
  if (strcmp(funcname, "AllServSend") == 0) {
    char *data = saacproto_wrapStringAddr(
        saacproto_stringwrapper[1], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[2]));
    saacproto_AllServSend_recv(data);
    return 0;
  }
#endif

#ifdef _CHARADATA_SAVE_SQL
  if (strcmp(funcname, "CharadataSaveSQL") == 0) {
    Charadata charadata;
    int clifd = demkstr_int(saacproto.token_list[2]);
    char *id = saacproto_wrapStringAddr(
        saacproto_stringwrapper[2], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[3]));
    charadata.CHAR_list_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[3], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[4]));
    charadata.CHAR_list_info1_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[4], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[5]));
    charadata.CHAR_list_info2_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[5], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[6]));
    charadata.CHAR_list_count_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[6], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[7]));
    charadata.CHAR_list_info3_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[7], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[8]));
    charadata.CHAR_list_event_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[8], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[9]));
    charadata.CHAR_list_info4_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[9], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[10]));
    charadata.CHAR_list_attackmagic_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[10], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[11]));
    charadata.CHAR_list_info5_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[11], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[12]));
    charadata.CHAR_list_profession_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[12], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[13]));
    charadata.CHAR_list_info6_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[13], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[14]));
    charadata.CHAR_list_info_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[14], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[15]));
    charadata.CHAR_list_flg_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[15], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[16]));
    charadata.CHAR_list_skill_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[16], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[17]));
    charadata.CHAR_list_item_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[17], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[18]));
    charadata.CHAR_list_title_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[18], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[19]));
    charadata.CHAR_list_mail_String = saacproto_wrapStringAddr(
        saacproto_stringwrapper[19], saacproto.work_buf_size,
        demkstr_string(saacproto.token_list[20]));

    int saveindex = demkstr_int(saacproto.token_list[21]);
    int flag = demkstr_int(saacproto.token_list[22]);
    saacproto_CharadataSaveSQL_recv(fd, clifd, id, charadata, saveindex, flag);
    return 0;
  }
#endif

  sprintf(debugfun, "%s", funcname);

  return -1;
}

void saacproto_ACServerLogin_send(const int fd, const char *result,
                                  const char *data) {
  saacproto_CreateHeader(saacproto.work, "ACServerLogin");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharList_send(int fd, char *result, char *output, int id) {
  saacproto_CreateHeader(saacproto.work, "ACCharList");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(output), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
static int CharDataLens = 0;
#ifdef _NewSave
void saacproto_ACCharLoad_send(int fd, char *result, char *data, int id,
                               int charindex)
#else
void saacproto_ACCharLoad_send(int fd, char *result, char *data, int id)
#endif
{
  if (strstr(result, "successful") != NULL && strlen(data) > CharDataLens) {
    CharDataLens = strlen(data);
    // andy_log
    log("最大数据长度:%d\n", CharDataLens);
  }

  saacproto_CreateHeader(saacproto.work, "ACCharLoad");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
#ifdef _NewSave
  strcatsafe(saacproto.work, mkstr_int(charindex), saacproto.work_buf_size);
#endif
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharSave_send(int fd, char *result, char *data, int id) {
  saacproto_CreateHeader(saacproto.work, "ACCharSave");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharDelete_send(int fd, char *result, char *data, int id) {
  saacproto_CreateHeader(saacproto.work, "ACCharDelete");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACLock_send(int fd, char *result, char *data, int id) {
  saacproto_CreateHeader(saacproto.work, "ACLock");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACUCheck_send(int fd, char *mem_id) {
  saacproto_CreateHeader(saacproto.work, "ACUCheck");
  strcatsafe(saacproto.work, mkstr_string(mem_id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBUpdateEntryString_send(int fd, char *result, char *table,
                                        char *key, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBUpdateEntryString");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBDeleteEntryString_send(int fd, char *result, char *table,
                                        char *key, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBDeleteEntryString");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBGetEntryString_send(int fd, char *result, char *value,
                                     char *table, char *key, int msgid,
                                     int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBGetEntryString");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(value), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void saacproto_UpdataStele_send(int fd, char *data) {
  saacproto_CreateHeader(saacproto.work, "UpdataStele");
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_S_UpdataStele_send(int fd, char *ocdkey, char *oname,
                                  char *ncdkey, char *nname, char *title,
                                  int level, int trns, int floor) {
  saacproto_CreateHeader(saacproto.work, "S_UpdataStele");
  strcatsafe(saacproto.work, mkstr_string(ocdkey), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(oname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(ncdkey), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(nname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(title), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(level), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(trns), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(floor), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif
void saacproto_DBUpdateEntryInt_send(int fd, char *result, char *table,
                                     char *key, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBUpdateEntryInt");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBGetEntryRank_send(int fd, char *result, int rank, int count,
                                   char *table, char *key, int msgid,
                                   int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBGetEntryRank");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(rank), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(count), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBDeleteEntryInt_send(int fd, char *result, char *table,
                                     char *key, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBDeleteEntryInt");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBGetEntryInt_send(int fd, char *result, int value, char *table,
                                  char *key, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBGetEntryInt");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(value), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(key), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBGetEntryByRank_send(int fd, char *result, char *list,
                                     char *table, int msgid, int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBGetEntryByRank");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(list), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_DBGetEntryByCount_send(int fd, char *result, char *list,
                                      char *table, int count_start, int msgid,
                                      int msgid2) {
  saacproto_CreateHeader(saacproto.work, "DBGetEntryByCount");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(list), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(table), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(count_start), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(msgid2), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_Broadcast_send(int fd, char *id, char *charname, char *message) {
  saacproto_CreateHeader(saacproto.work, "Broadcast");
  strcatsafe(saacproto.work, mkstr_string(id), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(charname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(message), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_Message_send(int fd, char *id_from, char *charname_from,
                            char *id_to, char *charname_to, char *message,
                            int option, int mesgid) {
  saacproto_CreateHeader(saacproto.work, "Message");
  strcatsafe(saacproto.work, mkstr_string(id_from), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(charname_from),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(id_to), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(charname_to),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(message), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(option), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(mesgid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_SetServerLogFiles(char *r, char *w) {
  strcpysafe(saacproto_writelogfilename, w, sizeof(saacproto_writelogfilename));
  strcpysafe(saacproto_readlogfilename, r, sizeof(saacproto_readlogfilename));
}
int saacproto_InitServer(int (*writefunc)(int, char *, int), int work_size) {
  int i;
  if ((void *)writefunc == NULL) {
    saacproto.write_func = saacproto_default_write_wrap;
  } else {
    saacproto.write_func = writefunc;
  }
  InitProto(&saacproto, work_size);
  saacproto_stringwrapper = (char **)calloc(1, sizeof(char *) * MAXLSRPCARGS);
  if (saacproto_stringwrapper == NULL)
    return -1;
  memset(saacproto_stringwrapper, 0, sizeof(char *) * MAXLSRPCARGS);
  for (i = 0; i < MAXLSRPCARGS; i++) {
    saacproto_stringwrapper[i] = (char *)calloc(1, work_size);
    if (saacproto_stringwrapper[i] == NULL) {
      for (i = 0; i < MAXLSRPCARGS; i++) {
        free(saacproto_stringwrapper[i]);
        return -1;
      }
    }
  }
  saacproto.ret_work = (char *)calloc(1, sizeof(work_size));
  if (saacproto.ret_work == NULL) {
    return -1;
  }
  return 0;
}
void saacproto_CleanupServer(void) {
  int i;
  free(saacproto.work);
  free(saacproto.array_work);
  free(saacproto.escape_work);
  free(saacproto.val_str);
  free(saacproto.token_list);
  for (i = 0; i < MAXLSRPCARGS; i++) {
    free(saacproto_stringwrapper[i]);
  }
  free(saacproto_stringwrapper);
  free(saacproto.ret_work);
}

// 成立家族
void saacproto_ACAddFM_send(int fd, char *result, int fmindex, int index,
                            int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACAddFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 加入家族
void saacproto_ACJoinFM_send(int fd, char *result, int recv, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACJoinFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(recv), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 退出家族
void saacproto_ACLeaveFM_send(int fd, char *result, int resultflag,
                              int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACLeaveFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(resultflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 解散家族
void saacproto_ACDelFM_send(int fd, char *result, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACDelFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 列出家族列表
void saacproto_ACShowFMList_send(int fd, char *result, int num, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACShowFMList");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#ifdef _FAMILY_TOTEM
// 列出家族图腾
void saacproto_ACShowFMTotem_send(int fd, char *result, int num, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACShowFMTotem");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif
// 列出成员列表
void saacproto_ACShowMemberList_send(int fd, char *result, int index, int num,
                                     int acceptflag, int fmjoinnum, char *data
#ifdef _FAMILYBADGE_
                                     ,
                                     int badge
#endif
) {
  saacproto_CreateHeader(saacproto.work, "ACShowMemberList");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(acceptflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmjoinnum), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
#ifdef _FAMILYBADGE_
  strcatsafe(saacproto.work, mkstr_int(badge), saacproto.work_buf_size);
#endif
  saacproto_Send(fd, saacproto.work);
}
// 列出家族详细资料
void saacproto_ACFMDetail_send(int fd, char *result, char *data, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACFMDetail");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 族长审核成员加入家族
void saacproto_ACMemberJoinFM_send(int fd, char *result, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACMemberJoinFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 族长审核成员离开家族
void saacproto_ACMemberLeaveFM_send(int fd, char *result, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACMemberLeaveFM");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 加入家族之玩家login
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
                                  int fmpopular, int joinflag, int fmsetupflag,
                                  int flag, int charindex, int charfame,
                                  int charfdid
#ifdef _NEW_MANOR_LAW
                                  ,
                                  int momentum
#endif
)
#else
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
                                  int fmpopular, int joinflag, int fmsetupflag,
                                  int flag, int charindex, int charfdid)
#endif
{
  saacproto_CreateHeader(saacproto.work, "ACFMCharLogin");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(floor), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmpopular), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(joinflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmsetupflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charindex), saacproto.work_buf_size);
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  strcatsafe(saacproto.work, mkstr_int(charfame), saacproto.work_buf_size);
#endif
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
#ifdef _NEW_MANOR_LAW
  strcatsafe(saacproto.work, mkstr_int(momentum), saacproto.work_buf_size);
#endif
  saacproto_Send(fd, saacproto.work);
}
// 加入家族之玩家logout
void saacproto_ACFMCharLogout_send(int fd, char *result, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACFMCharLogout");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 列出家族留言板
void saacproto_ACFMReadMemo_send(int fd, char *result, int index, int num,
                                 int dataindex, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACFMReadMemo");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(dataindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 写入家族留言板
void saacproto_ACFMWriteMemo_send(int fd, char *result, int index) {
  saacproto_CreateHeader(saacproto.work, "ACFMWriteMemo");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 列出家族据点
void saacproto_ACFMPointList_send(int fd, char *result, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACFMPointList");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

// 申请家族据点
void saacproto_ACSetFMPoint_send(int fd, char *result, int r, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACSetFMPoint");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(r), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 设定家族据点
void saacproto_ACFixFMPoint_send(int fd, char *result, int r) {
  saacproto_CreateHeader(saacproto.work, "ACFixFMPoint");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(r), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 族长对星系家族成员广播
void saacproto_ACFMAnnounce_send(int fd, char *result, char *fmname,
                                 int fmindex, int index, int kindflag,
                                 char *data, int color) {
  saacproto_CreateHeader(saacproto.work, "ACFMAnnounce");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(fmname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(kindflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(color), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 列出家族排行榜
void saacproto_ACShowTopFMList_send(int fd, char *result, int kindflag, int num,
                                    char *data) {
  saacproto_CreateHeader(saacproto.work, "ACShowTopFMList");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(kindflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACFixFMData_send(int fd, char *result, int kindflag, char *data1,
                                char *data2, int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACFixFMData");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(kindflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data1), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data2), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 设定家族据点
void saacproto_ACFixFMPK_send(int fd, char *result, int award, int winindex,
                              int loseindex) {
  saacproto_CreateHeader(saacproto.work, "ACFixFMPK");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(award), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(winindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(loseindex), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// GM修正家族资料
void saacproto_ACGMFixFMData_send(int fd, char *result, char *fmname,
                                  int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACGMFixFMData");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(fmname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 取得家族资料
void saacproto_ACGetFMData_send(int fd, char *result, int kindflag, int data,
                                int charfdid) {
  saacproto_CreateHeader(saacproto.work, "ACGetFMData");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(kindflag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(charfdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
//
void saacproto_ACFMClearPK_send(int fd, char *result, char *fmname, int fmindex,
                                int index) {
  saacproto_CreateHeader(saacproto.work, "ACFMClearPK");
  strcatsafe(saacproto.work, mkstr_string(result), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(fmname), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(fmindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(index), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
// 通知其他星球的踢馆人
void saacproto_ACManorPKAck_send(int fd, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACManorPKAck");
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
void saacproto_ACLoadFmPk_send(int fd, char *data) {
  saacproto_CreateHeader(saacproto.work, "ACLoadFmPk");
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#ifdef _ACFMPK_LIST
void saacproto_ACSendFmPk_send(int fd, int userindex, int flg) {
  saacproto_CreateHeader(saacproto.work, "ACSendFmPk");
  strcatsafe(saacproto.work, mkstr_int(userindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flg), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#endif

#ifdef _WAEI_KICK
void saacproto_ACKick_send(int fd, int act, char *data, int id) {
  saacproto_CreateHeader(saacproto.work, "ACKick");
  strcatsafe(saacproto.work, mkstr_int(act), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(id), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
void saacproto_SendEffect_send(int fd, char *effect) {
  saacproto_CreateHeader(saacproto.work, "EFFECT");
  strcatsafe(saacproto.work, mkstr_string(effect), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _CHAR_POOLITEM
void saacproto_ACCharSavePoolItem_send(int fd, char *Acces, char *Pooldataarg,
                                       int clifdid) {
  saacproto_CreateHeader(saacproto.work, "ACSavePoolItem");
  strcatsafe(saacproto.work, mkstr_string(Acces), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(Pooldataarg),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(clifdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharGetPoolItem_send(int fd, char *Acces, char *Pooldataarg,
                                      int clifdid, int npcid) {
  saacproto_CreateHeader(saacproto.work, "ACGetPoolItem");
  strcatsafe(saacproto.work, mkstr_string(Acces), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(Pooldataarg),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(clifdid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(npcid), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _CHAR_POOLPET
void saacproto_ACCharSavePoolPet_send(int fd, char *Acces, char *Pooldataarg,
                                      int clifdid) {
  saacproto_CreateHeader(saacproto.work, "ACSavePoolPet");
  strcatsafe(saacproto.work, mkstr_string(Acces), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(Pooldataarg),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(clifdid), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharGetPoolPet_send(int fd, char *Acces, char *Pooldataarg,
                                     int clifdid, int npcid) {
  saacproto_CreateHeader(saacproto.work, "ACGetPoolPet");
  strcatsafe(saacproto.work, mkstr_string(Acces), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(Pooldataarg),
             saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(clifdid), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(npcid), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ANGEL_SUMMON
void saacproto_ACMissionTable_send(int fd, int num, int type, char *data,
                                   char *angelinfo) {
  saacproto_CreateHeader(saacproto.work, "ACMissionTable");
  strcatsafe(saacproto.work, mkstr_int(num), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(type), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(angelinfo), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _TEACHER_SYSTEM
void saacproto_ACCheckCharacterOnLine_send(int fd, int charaindex, int iOnline,
                                           char *data, int flag) {
  saacproto_CreateHeader(saacproto.work, "ACCheckCharacterOnLine");
  strcatsafe(saacproto.work, mkstr_int(charaindex), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(iOnline), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flag), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

void saacproto_ACCharLogin_send(int fd, int clifd, int flag) {
  saacproto_CreateHeader(saacproto.work, "ACCharLogin");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flag), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}

#ifdef _NEW_VIP_SHOP
void saacproto_QueryPoint_send(int fd, int clifd, int point) {
  saacproto_CreateHeader(saacproto.work, "QueryPoint");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(point), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_NewVipShop_send(int fd, int clifd, int point, char *buf,
                               int flag) {
  saacproto_CreateHeader(saacproto.work, "NewVipShop");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(point), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(buf), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flag), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ITEM_PET_LOCKED
void saacproto_ItemPetLocked_send(int fd, int clifd, int flag, char *data) {
  saacproto_CreateHeader(saacproto.work, "ItemPetLocked");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_int(flag), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}

void saacproto_ItemPetLockedPasswd_send(int fd, int clifd, char *data) {
  saacproto_CreateHeader(saacproto.work, "ItemPetLockedPasswd");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);
  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ONLINE_COST
void saacproto_OnlineCost_send(int fd, int clifd, char *data) {
  saacproto_CreateHeader(saacproto.work, "OnlineCost");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _SQL_BUY_FUNC
void saacproto_OnlineBuy_send(int fd, int clifd, char *data) {
  saacproto_CreateHeader(saacproto.work, "OnlineBuy");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void saacproto_OldToNew_send(int fd, int clifd, char *data) {
  saacproto_CreateHeader(saacproto.work, "OldToNew");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void saacproto_FormulateAutoPk_send(int fd, int clifd, char *data) {
  saacproto_CreateHeader(saacproto.work, "FormulateAutoPk");
  strcatsafe(saacproto.work, mkstr_int(clifd), saacproto.work_buf_size);
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _LOTTERY_SYSTEM
void saacproto_LotterySystem_send(int fd, char *award) {
  saacproto_CreateHeader(saacproto.work, "LotterySystem");
  strcatsafe(saacproto.work, mkstr_string(award), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ALL_SERV_SEND
void saacproto_AllServSend_send(int fd, char *data) {
  saacproto_CreateHeader(saacproto.work, "AllServSend");
  strcatsafe(saacproto.work, mkstr_string(data), saacproto.work_buf_size);

  saacproto_Send(fd, saacproto.work);
}
#endif
