#define __SAAC_SERVER_SEND_C__
//
#include "saac_server.h"
//
#include "main.h"

extern WorkSpace gSaacWorkSpace;
WorkSpace *ws = &gSaacWorkSpace;

// Warning: fd is not a socket, but an index of thie connect structure.
int SaacServer_ServerDispatchMessage(int fd, char *encoded, char *debugfun) {
  unsigned int msgid;
  int token_count;
  char funcname[1024];
  if (debugfun != NULL)
    debugfun[0] = '\0';
  token_count = SplitString(encoded, ws);
  if (token_count < 2) {
    if (debugfun != NULL)
      snprintf(debugfun, 256, "invalid or oversized SAAC message");
    logout_game_server(fd);
    return -1;
  }
  GetMessageInfo(&msgid, funcname, sizeof(funcname), ws->token_list);
  SAAC_LOG_RECV(funcname, "msgid=%u fd=%d", msgid, fd);

  /* Authentication is a protocol invariant, not an individual RPC option. */
  if (strcmp(funcname, "ACServerLogin") != 0 &&
      !is_game_server_login(fd)) {
    if (debugfun != NULL)
      snprintf(debugfun, 256, "unauthenticated SAAC function: %.200s",
               funcname);
    logout_game_server(fd);
    return -1;
  }

  if (strcmp(funcname, "ACServerLogin") == 0) {
    char *servername;
    char *serverpass;
#if _ATTESTAION_ID == 1
    int id;
    if (token_count < 5) {
      logout_game_server(fd);
      return -1;
    }
    id = demkstr_int(ws->token_list[2]);
    servername = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                             demkstr_string(ws->token_list[3]));
    serverpass = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                             demkstr_string(ws->token_list[4]));
    SaacServer_ACServerLogin_recv(fd, id, servername, serverpass);
    return 0;
#else
    if (token_count < 4) {
      logout_game_server(fd);
      return -1;
    }
    servername = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                             demkstr_string(ws->token_list[2]));
    serverpass = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                             demkstr_string(ws->token_list[3]));
    SaacServer_ACServerLogin_recv(fd, servername, serverpass);
    return 0;
#endif
  }

  if (strcmp(funcname, "ACServerLogout") == 0) {
    SaacServer_ACServerLogout_recv(fd);
    return 0;
  }

  if (strcmp(funcname, "ACCharList") == 0) {
    char *id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                           demkstr_string(ws->token_list[2]));
    char *pas = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                            demkstr_string(ws->token_list[3]));
    char *ip = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    char *mac = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                            demkstr_string(ws->token_list[5]));
    const int msg_id = demkstr_int(ws->token_list[6]);
    const int char_list_flag = demkstr_int(ws->token_list[7]);
    SaacServer_ACCharList_recv(fd, id, pas, ip, mac, msg_id, char_list_flag);
    return 0;
  }

  if (strcmp(funcname, "ACCharLoad") == 0) {
    char *id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                           demkstr_string(ws->token_list[2]));
    char *pas = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                            demkstr_string(ws->token_list[3]));
    char *charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                                 demkstr_string(ws->token_list[4]));
    int lock = demkstr_int(ws->token_list[5]);
    char *opt = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                            demkstr_string(ws->token_list[6]));
    int msg_id = demkstr_int(ws->token_list[7]);
    SaacServer_ACCharLoad_recv(fd, id, pas, charname, lock, opt, msg_id);
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
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    charname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    opt = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                      demkstr_string(ws->token_list[4]));
    charinfo = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                           demkstr_string(ws->token_list[5]));
    unlock = demkstr_int(ws->token_list[6]);
    mesgid = demkstr_int(ws->token_list[7]);

#ifdef _NewSave
    charindex = demkstr_int(ws->token_list[8]);
    SaacServer_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid,
                               charindex);
#else
    SaacServer_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid);
#endif
    return 0;
  }

  if (strcmp(funcname, "ACCharDelete") == 0) {
    char *id;
    char *passwd;
    char *charname;
    char *option;
    int mesgid;
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    passwd = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                         demkstr_string(ws->token_list[3]));
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    option = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    mesgid = demkstr_int(ws->token_list[6]);
    SaacServer_ACCharDelete_recv(fd, id, passwd, charname, option, mesgid);
    return 0;
  }

  if (strcmp(funcname, "ACLock") == 0) {
    char *id;
    int lock;
    int mesgid;
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    lock = demkstr_int(ws->token_list[3]);
    mesgid = demkstr_int(ws->token_list[4]);
    SaacServer_ACLock_recv(fd, id, lock, mesgid);
    return 0;
  }

#ifdef _WAEI_KICK
  if (strcmp(funcname, "ACKick") == 0) {
    char *id;  // 欲踢的帐号
    int flg;   // 事件flg
    int clifd; // GM
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    clifd = demkstr_int(ws->token_list[3]);
    flg = demkstr_int(ws->token_list[4]);
    SaacServer_ACKick_recv(fd, id, flg, clifd);
    return 0;
  }
#endif

  if (strcmp(funcname, "ACUCheck") == 0) {
    int status;
    char *mem_id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                               demkstr_string(ws->token_list[2]));
    status = demkstr_int(ws->token_list[3]);
    SaacServer_ACUCheck_recv(fd, mem_id, status);
    return 0;
  }

  if (strcmp(funcname, "DBUpdateEntryString") == 0) {
    char *table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                              demkstr_string(ws->token_list[2]));
    char *key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                            demkstr_string(ws->token_list[3]));
    char *value = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                              demkstr_string(ws->token_list[4]));
    const int msg_id = demkstr_int(ws->token_list[5]);
    const int msg_id2 = demkstr_int(ws->token_list[6]);
    SaacServer_DBUpdateEntryString_recv(fd, table, key, value, msg_id, msg_id2);
    return 0;
  }

  if (strcmp(funcname, "DBDeleteEntryString") == 0) {
    char *table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                              demkstr_string(ws->token_list[2]));
    char *key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                            demkstr_string(ws->token_list[3]));
    const int msg_id = demkstr_int(ws->token_list[4]);
    const int msg_id2 = demkstr_int(ws->token_list[5]);
    SaacServer_DBDeleteEntryString_recv(fd, table, key, msg_id, msg_id2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryString") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                      demkstr_string(ws->token_list[3]));
    msgid = demkstr_int(ws->token_list[4]);
    msgid2 = demkstr_int(ws->token_list[5]);
    SaacServer_DBGetEntryString_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBUpdateEntryInt") == 0) {
    char *table;
    char *key;
    int value;
    char *info;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                      demkstr_string(ws->token_list[3]));
    value = demkstr_int(ws->token_list[4]);
    info = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                       demkstr_string(ws->token_list[5]));
    msgid = demkstr_int(ws->token_list[6]);
    msgid2 = demkstr_int(ws->token_list[7]);
    SaacServer_DBUpdateEntryInt_recv(fd, table, key, value, info, msgid,
                                     msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryRank") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                      demkstr_string(ws->token_list[3]));
    msgid = demkstr_int(ws->token_list[4]);
    msgid2 = demkstr_int(ws->token_list[5]);
    SaacServer_DBGetEntryRank_recv(fd, table, key, msgid, msgid2);
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
    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    name = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                       demkstr_string(ws->token_list[3]));
    title = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                        demkstr_string(ws->token_list[4]));
    level = demkstr_int(ws->token_list[5]);
    trns = demkstr_int(ws->token_list[6]);
    time = demkstr_int(ws->token_list[7]);
    floor = demkstr_int(ws->token_list[8]);
    SaacServer_UpdataStele_recv(fd, cdkey, name, title, level, trns, time,
                                floor);
    return 0;
  }
#endif

  if (strcmp(funcname, "DBDeleteEntryInt") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                      demkstr_string(ws->token_list[3]));
    msgid = demkstr_int(ws->token_list[4]);
    msgid2 = demkstr_int(ws->token_list[5]);
    SaacServer_DBDeleteEntryInt_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryInt") == 0) {
    char *table;
    char *key;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    key = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                      demkstr_string(ws->token_list[3]));
    msgid = demkstr_int(ws->token_list[4]);
    msgid2 = demkstr_int(ws->token_list[5]);
    SaacServer_DBGetEntryInt_recv(fd, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByRank") == 0) {
    char *table;
    int rank_start;
    int rank_end;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    rank_start = demkstr_int(ws->token_list[3]);
    rank_end = demkstr_int(ws->token_list[4]);
    msgid = demkstr_int(ws->token_list[5]);
    msgid2 = demkstr_int(ws->token_list[6]);
    SaacServer_DBGetEntryByRank_recv(fd, table, rank_start, rank_end, msgid,
                                     msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByCount") == 0) {
    char *table;
    int count_start;
    int num;
    int msgid;
    int msgid2;
    table = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    count_start = demkstr_int(ws->token_list[3]);
    num = demkstr_int(ws->token_list[4]);
    msgid = demkstr_int(ws->token_list[5]);
    msgid2 = demkstr_int(ws->token_list[6]);
    SaacServer_DBGetEntryByCount_recv(fd, table, count_start, num, msgid,
                                      msgid2);
    return 0;
  }

  if (strcmp(funcname, "Broadcast") == 0) {
    char *id;
    char *charname;
    char *message;
    int flag;
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    charname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    message = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                          demkstr_string(ws->token_list[4]));
    flag = demkstr_int(ws->token_list[5]);
    SaacServer_Broadcast_recv(fd, id, charname, message, flag);

    return 0;
  }

  if (strcmp(funcname, "Message") == 0) {
    char *id_from;
    char *charname_from;
    char *id_to;
    char *charname_to;
    char *message;
    int option;
    id_from = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    charname_from = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                demkstr_string(ws->token_list[3]));
    id_to = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                        demkstr_string(ws->token_list[4]));
    charname_to = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                              demkstr_string(ws->token_list[5]));
    message = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                          demkstr_string(ws->token_list[6]));
    option = demkstr_int(ws->token_list[7]);
    SaacServer_Message_recv(fd, id_from, charname_from, id_to, charname_to,
                            message, option);
    return 0;
  }

  if (strcmp(funcname, "MessageAck") == 0) {
    char *id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                           demkstr_string(ws->token_list[2]));
    char *charname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                 demkstr_string(ws->token_list[3]));
    char *result = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                               demkstr_string(ws->token_list[4]));
    const int msg_id = demkstr_int(ws->token_list[5]);
    SaacServer_MessageAck_recv(fd, id, charname, result, msg_id);
    return 0;
  }

  if (strcmp(funcname, "MessageFlush") == 0) {
    char *id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                           demkstr_string(ws->token_list[2]));
    char *char_name = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                  demkstr_string(ws->token_list[3]));
    SaacServer_MessageFlush_recv(fd, id, char_name);
    return 0;
  }
  // 成立家族
  if (strcmp(funcname, "ACAddFM") == 0) {
    int fmleaderlv, fmsprite, charfdid, fmleadergrano;
    char *fmname, *fmleadername, *fmleaderid, *petname, *petattr, *fmrule;
#ifdef _PERSONAL_FAME
    int fame;
#endif
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmleadername = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                               demkstr_string(ws->token_list[3]));
    fmleaderid = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                             demkstr_string(ws->token_list[4]));
    fmleaderlv = demkstr_int(ws->token_list[5]);
    petname = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                          demkstr_string(ws->token_list[6]));
    petattr = strncpysafe(ws->string_buffer[6], ws->work_buf_size,
                          demkstr_string(ws->token_list[7]));
    fmrule = strncpysafe(ws->string_buffer[7], ws->work_buf_size,
                         demkstr_string(ws->token_list[8]));
    fmsprite = demkstr_int(ws->token_list[9]);
    fmleadergrano = demkstr_int(ws->token_list[10]);
#ifdef _PERSONAL_FAME
    fame = demkstr_int(ws->token_list[11]);
    charfdid = demkstr_int(ws->token_list[12]);
#ifdef _FAMILYBADGE_
    int fmbadge = demkstr_int(ws->token_list[13]);
#endif
    SaacServer_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid, fmleaderlv,
                            petname, petattr, fmrule, fmsprite, fmleadergrano,
                            fame
#ifdef _FAMILYBADGE_
                            ,
                            fmbadge
#endif
                            ,
                            charfdid);
#else
    charfdid = demkstr_int(ws->token_list[11]);
    SaacServer_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid, fmleaderlv,
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
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charid = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    charlv = demkstr_int(ws->token_list[6]);
    index = demkstr_int(ws->token_list[7]);
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
    fame = demkstr_int(ws->token_list[8]);
    charfdid = demkstr_int(ws->token_list[9]);
    SaacServer_ACJoinFM_recv(fd, fmname, fmindex, charname, charid, charlv,
                             index, fame, charfdid);
#else
    charfdid = demkstr_int(ws->token_list[8]);
    SaacServer_ACJoinFM_recv(fd, fmname, fmindex, charname, charid, charlv,
                             index, charfdid);
#endif
    return 0;
  }
  // 退出家族
  if (strcmp(funcname, "ACLeaveFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname, *charname, *charid;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charid = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    index = demkstr_int(ws->token_list[6]);
    charfdid = demkstr_int(ws->token_list[7]);
    SaacServer_ACLeaveFM_recv(fd, fmname, fmindex, charname, charid, index,
                              charfdid);
    return 0;
  }
  // 解散家族
#ifdef _LEADERFUNCHECK
  if (strcmp(funcname, "ACDelFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname, *charname, *charid;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    charname = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                           demkstr_string(ws->token_list[5]));
    charid = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                         demkstr_string(ws->token_list[6]));
    charfdid = demkstr_int(ws->token_list[7]);
    SaacServer_ACDelFM_recv(fd, fmname, fmindex, index, charname, charid,
                            charfdid);
    return 0;
  }
#else
  if (strcmp(funcname, "ACDelFM") == 0) {
    int fmindex, charfdid, index;
    char *fmname;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    charfdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACDelFM_recv(fd, fmname, fmindex, index, charfdid);
    return 0;
  }
#endif
  // 列出家族列表
  if (strcmp(funcname, "ACShowFMList") == 0) {
    SaacServer_ACShowFMList_recv(fd);
    return 0;
  }
#ifdef _FAMILY_TOTEM
  // 列出家族图腾
  if (strcmp(funcname, "ACShowFMTotem") == 0) {
    SaacServer_ACShowFMTotem_recv(fd);
    return 0;
  }
#endif
  // 列出成员列表
  if (strcmp(funcname, "ACShowMemberList") == 0) {
    int index;
    char *tmpbuf;
    tmpbuf = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    SaacServer_ACShowMemberList_recv(fd, index);
    return 0;
  }
  // 列出家族详细资料
  if (strcmp(funcname, "ACFMDetail") == 0) {
    int fmindex, charfdid, index;
    char *fmname;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    charfdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACFMDetail_recv(fd, fmname, fmindex, index, charfdid);
    return 0;
  }
  // 列出家族留言
  if (strcmp(funcname, "ACFMReadMemo") == 0) {
    int index;
    char *tmpbuf;
    tmpbuf = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    SaacServer_ACFMReadMemo_recv(fd, index);
    return 0;
  }
  // 写入家族留言
  if (strcmp(funcname, "ACFMWriteMemo") == 0) {
    int index, fmindex;
    char *fmname, *data;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    index = demkstr_int(ws->token_list[5]);
    SaacServer_ACFMWriteMemo_recv(fd, fmname, fmindex, data, index);
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
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charid = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    charlv = demkstr_int(ws->token_list[6]);

#ifdef _FMVER21
    eventflag = demkstr_int(ws->token_list[7]);
    charfdid = demkstr_int(ws->token_list[8]);
#ifdef _FM_MODIFY
    gsnum = demkstr_int(ws->token_list[9]);
    SaacServer_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                  eventflag, charfdid, gsnum);
#else
    SaacServer_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                  eventflag, charfdid);
#endif
#else
    charfdid = demkstr_int(ws->token_list[7]);
    SaacServer_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv,
                                  charfdid);
#endif
    return 0;
  }
  // 玩家logout
  if (strcmp(funcname, "ACFMCharLogout") == 0) {
    int fmindex, charfdid, charlv, index;
    char *fmname, *charname, *charid;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charid = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    charlv = demkstr_int(ws->token_list[6]);
    index = demkstr_int(ws->token_list[7]);
    charfdid = demkstr_int(ws->token_list[8]);
    SaacServer_ACFMCharLogout_recv(fd, fmname, fmindex, charname, charid,
                                   charlv, index, charfdid);
    return 0;
  }
  // 族长审核成员加入家族、修改家族成员职位
  if (strcmp(funcname, "ACMemberJoinFM") == 0) {
    int fmindex, charfdid, index, charindex, result;
    char *fmname, *charname;
#ifdef _FMVER21
    int meindex;
#endif
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charindex = demkstr_int(ws->token_list[5]);
    index = demkstr_int(ws->token_list[6]);
    result = demkstr_int(ws->token_list[7]);
#ifdef _FMVER21
    meindex = demkstr_int(ws->token_list[8]);
    charfdid = demkstr_int(ws->token_list[9]);
    SaacServer_ACMemberJoinFM_recv(fd, fmname, fmindex, charname, charindex,
                                   index, result, meindex, charfdid);
#else
    charfdid = demkstr_int(ws->token_list[8]);
    SaacServer_ACMemberJoinFM_recv(fd, fmname, fmindex, charname, charindex,
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
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    charname = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                           demkstr_string(ws->token_list[4]));
    charindex = demkstr_int(ws->token_list[5]);
    index = demkstr_int(ws->token_list[6]);
#ifdef _FMVER21
    meindex = demkstr_int(ws->token_list[7]);
    charfdid = demkstr_int(ws->token_list[8]);
    SaacServer_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname, charindex,
                                    index, meindex, charfdid);
#else
    charfdid = demkstr_int(ws->token_list[7]);
    SaacServer_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname, charindex,
                                    index, charfdid);
#endif
    return 0;
  }
  // 列出家族据点
  if (strcmp(funcname, "ACFMPointList") == 0) {
    SaacServer_ACFMPointList_recv(fd);
    return 0;
  }

  // 申请家族据点
  if (strcmp(funcname, "ACSetFMPoint") == 0) {
    int fl, x, y, fmindex, index, fmpointindex, charfdid;
    char *fmname;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    fmpointindex = demkstr_int(ws->token_list[5]);
    fl = demkstr_int(ws->token_list[6]);
    x = demkstr_int(ws->token_list[7]);
    y = demkstr_int(ws->token_list[8]);
    charfdid = demkstr_int(ws->token_list[9]);
    SaacServer_ACSetFMPoint_recv(fd, fmname, fmindex, index, fmpointindex, fl,
                                 x, y, charfdid);
    return 0;
  }
  // 设定家族据点
  if (strcmp(funcname, "ACFixFMPoint") == 0) {
    int winindex, winfmindex, loseindex, losefmindex, village;
    char *winfmname, *losefmname;
    winfmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                            demkstr_string(ws->token_list[2]));
    winfmindex = demkstr_int(ws->token_list[3]);
    winindex = demkstr_int(ws->token_list[4]);
    losefmname = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                             demkstr_string(ws->token_list[5]));
    losefmindex = demkstr_int(ws->token_list[6]);
    loseindex = demkstr_int(ws->token_list[7]);
    village = demkstr_int(ws->token_list[8]);
    SaacServer_ACFixFMPoint_recv(fd, winfmname, winfmindex, winindex,
                                 losefmname, losefmindex, loseindex, village);
    return 0;
  }
  // 族长对全员广播
  if (strcmp(funcname, "ACFMAnnounce") == 0) {
    int index, fmindex, color;
    char *fmname, *data;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    data = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                       demkstr_string(ws->token_list[5]));
    color = demkstr_int(ws->token_list[6]);
    SaacServer_ACFMAnnounce_recv(fd, fmname, fmindex, index, data, color);
    return 0;
  }
  // 列出家族排行榜
  if (strcmp(funcname, "ACShowTopFMList") == 0) {
    int kindflag;
    char *tmpbuf;
    tmpbuf = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    kindflag = demkstr_int(ws->token_list[3]);
    SaacServer_ACShowTopFMList_recv(fd, kindflag);
    return 0;
  }
  // 修正家族资料
  if (strcmp(funcname, "ACFixFMData") == 0) {
    int fmindex, charfdid, index, kindflag, charindex;
    char *fmname, *data1, *data2;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    kindflag = demkstr_int(ws->token_list[5]);
    data1 = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                        demkstr_string(ws->token_list[6]));
    data2 = strncpysafe(ws->string_buffer[6], ws->work_buf_size,
                        demkstr_string(ws->token_list[7]));
    charindex = demkstr_int(ws->token_list[8]);
    charfdid = demkstr_int(ws->token_list[9]);
    SaacServer_ACFixFMData_recv(fd, fmname, fmindex, index, kindflag, data1,
                                data2, charindex, charfdid);
    return 0;
  }

  // 家族ＰＫ後修正声望
  if (strcmp(funcname, "ACFixFMPK") == 0) {
    int winindex, winfmindex, loseindex, losefmindex;
    char *winfmname, *losefmname;
    winfmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                            demkstr_string(ws->token_list[2]));
    winfmindex = demkstr_int(ws->token_list[3]);
    winindex = demkstr_int(ws->token_list[4]);
    losefmname = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                             demkstr_string(ws->token_list[5]));
    losefmindex = demkstr_int(ws->token_list[6]);
    loseindex = demkstr_int(ws->token_list[7]);
    SaacServer_ACFixFMPK_recv(fd, winfmname, winfmindex, winindex, losefmname,
                              losefmindex, loseindex);
    return 0;
  }
  // GM修正家族资料
  if (strcmp(funcname, "ACGMFixFMData") == 0) {
    int index, charfdid;
    char *cmd, *data, *tmpbuf, *charid;
    tmpbuf = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    charid = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                         demkstr_string(ws->token_list[4]));
    cmd = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                      demkstr_string(ws->token_list[5]));
    data = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                       demkstr_string(ws->token_list[6]));
    charfdid = demkstr_int(ws->token_list[7]);
    SaacServer_ACGMFixFMData_recv(fd, index, charid, cmd, data, charfdid);
    return 0;
  }
  // 取得家族资料
  if (strcmp(funcname, "ACGetFMData") == 0) {
    int fmindex, charfdid, index, kindflag;
    char *fmname;
    fmname = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                         demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    kindflag = demkstr_int(ws->token_list[5]);
    charfdid = demkstr_int(ws->token_list[6]);
    SaacServer_ACGetFMData_recv(fd, fmname, fmindex, index, kindflag, charfdid);
    return 0;
  }
  // 广播踢馆人的资料
  if (strcmp(funcname, "ACManorPKAck") == 0) {
    char *data;
    data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                       demkstr_string(ws->token_list[2]));
    SaacServer_ACManorPKAck_recv(fd, data);
    return 0;
  }

  if (strcmp(funcname, "ACreLoadFmData") == 0) {
    int type, data;
    type = demkstr_int(ws->token_list[2]);
    data = demkstr_int(ws->token_list[3]);
    SaacServer_ACreLoadFmData_recv(fd, type, data);
    return 0;
  }

#ifdef _AC_SEND_FM_PK
  if (strcmp(funcname, "ACLoadFmPk") == 0) {
    int fmpks_pos;
    fmpks_pos = demkstr_int(ws->token_list[2]);
    SaacServer_ACLoadFmPk_recv(fd, fmpks_pos);
    return 0;
  }
#ifdef _ACFMPK_LIST
  if (strcmp(funcname, "ACSendFmPk") == 0) {
    int fmpks_pos, userindex, flg;
    char *data;
    fmpks_pos = demkstr_int(ws->token_list[2]);
    userindex = demkstr_int(ws->token_list[3]);
    flg = demkstr_int(ws->token_list[4]);
    data = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                       demkstr_string(ws->token_list[5]));
    SaacServer_ACSendFmPk_recv(fd, fmpks_pos, userindex, flg, data);
    return 0;
  }
#else
  if (strcmp(funcname, "ACSendFmPk") == 0) {
    int fmpks_pos;
    char *data;
    fmpks_pos = demkstr_int(ws->token_list[2]);
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                       demkstr_string(ws->token_list[3]));
    SaacServer_ACSendFmPk_recv(fd, fmpks_pos, data);
    return 0;
  }
#endif

#endif

#ifdef _CHAR_POOLITEM
  if (strcmp(funcname, "ACInsertPoolItem") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    Pooldataarg = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    userindex = demkstr_int(ws->token_list[4]);
    clifdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACCharInsertPoolItem_recv(fd, cdkey, userindex, clifdid,
                                         Pooldataarg);
    return 0;
  }
  if (strcmp(funcname, "ACSavePoolItem") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    Pooldataarg = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    userindex = demkstr_int(ws->token_list[4]);
    clifdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACCharSavePoolItem_recv(fd, cdkey, userindex, clifdid,
                                       Pooldataarg);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolItem") == 0) {
    char *cdkey;
    int clifdid, userindex, npcid;
    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    userindex = demkstr_int(ws->token_list[3]);
    clifdid = demkstr_int(ws->token_list[4]);
    npcid = demkstr_int(ws->token_list[5]);

    SaacServer_ACCharGetPoolItem_recv(fd, cdkey, userindex, clifdid, npcid);
    return 0;
  }
#endif

#ifdef _CHAR_POOLPET
  if (strcmp(funcname, "ACInsertPoolPet") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    Pooldataarg = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    userindex = demkstr_int(ws->token_list[4]);
    clifdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACCharInsertPoolPet_recv(fd, cdkey, userindex, clifdid,
                                        Pooldataarg);
    return 0;
  }
  if (strcmp(funcname, "ACSavePoolPet") == 0) {
    char *cdkey, *Pooldataarg;
    int clifdid, userindex;

    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    Pooldataarg = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    userindex = demkstr_int(ws->token_list[4]);
    clifdid = demkstr_int(ws->token_list[5]);
    SaacServer_ACCharSavePoolPet_recv(fd, cdkey, userindex, clifdid,
                                      Pooldataarg);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolPet") == 0) {
    char *cdkey;
    int clifdid, userindex, npcid;
    cdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    userindex = demkstr_int(ws->token_list[3]);
    clifdid = demkstr_int(ws->token_list[4]);
    npcid = demkstr_int(ws->token_list[5]);

    SaacServer_ACCharGetPoolPet_recv(fd, cdkey, userindex, clifdid, npcid);
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

    num = demkstr_int(ws->token_list[2]);
    type = demkstr_int(ws->token_list[3]);
    data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    // charaindex = demkstr_int( ws->token_list[5] );
    angelinfo = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                            demkstr_string(ws->token_list[5]));

    SaacServer_ACMissionTable_recv(fd, num, type, data, angelinfo);
    return 0;
  }
#endif

#ifdef _TEACHER_SYSTEM
  if (strcmp(funcname, "ACCheckCharacterOnLine") == 0) {
    int charaindex, flag;
    char *id;
    char *name;

    charaindex = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    name = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    flag = demkstr_int(ws->token_list[5]);

    SaacServer_ACCheckCharacterOnLine_recv(fd, charaindex, id, name, flag);
    return 0;
  }
#endif

  if (strcmp(funcname, "ACCharLogin") == 0) {
    int clifd;
    char *id, *pas, *ip;

    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    pas = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                      demkstr_string(ws->token_list[4]));
    ip = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                     demkstr_string(ws->token_list[5]));
#ifdef _NEWCLISETMAC
    char *mac;
    mac = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                      demkstr_string(ws->token_list[6]));
    SaacServer_ACCharLogin_recv(fd, clifd, id, pas, ip, mac);
#else
    SaacServer_ACCharLogin_recv(fd, clifd, id, pas, ip);
#endif
    return 0;
  }

  if (strcmp(funcname, "LockLogin") == 0) {
    int flag;
    char *id, *ip;
    id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                     demkstr_string(ws->token_list[2]));
    ip = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    flag = demkstr_int(ws->token_list[4]);
    SaacServer_LockLogin_recv(fd, id, ip, flag);
    return 0;
  }

#ifdef _NEW_VIP_SHOP
  if (strcmp(funcname, "QueryPoint") == 0) {
    int clifd;
    char *id;

    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    SaacServer_QueryPoint_recv(fd, clifd, id);
    return 0;
  }

  if (strcmp(funcname, "NewVipShop") == 0) {
    int clifd;
    int point;
    char *id;
    char *buf;
    int flag;
    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    point = demkstr_int(ws->token_list[4]);
    buf = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                      demkstr_string(ws->token_list[5]));
    flag = demkstr_int(ws->token_list[6]);
    SaacServer_NewVipShop_recv(fd, clifd, id, point, buf, flag);
    return 0;
  }

#ifdef _COST_ITEM
  if (strcmp(funcname, "CostItem") == 0) {
    int clifd;
    char *id;
    int point;
    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    point = demkstr_int(ws->token_list[4]);
    SaacServer_CostItem_recv(fd, clifd, id, point);
    return 0;
  }
#endif
#endif

#ifdef _ITEM_PET_LOCKED
  if (strcmp(funcname, "ItemPetLocked") == 0) {
    int clifd;
    char *id;
    char *safepasswd;

    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    safepasswd = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                             demkstr_string(ws->token_list[4]));
    SaacServer_ItemPetLocked_recv(fd, clifd, id, safepasswd);
    return 0;
  }

  if (strcmp(funcname, "ItemPetLockedPasswd") == 0) {
    int clifd;
    char *id;
    char *safepasswd;

    clifd = demkstr_int(ws->token_list[2]);
    id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                     demkstr_string(ws->token_list[3]));
    safepasswd = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                             demkstr_string(ws->token_list[4]));
    SaacServer_ItemPetLockedPasswd_recv(fd, clifd, id, safepasswd);
    return 0;
  }
#endif

#ifdef _ONLINE_COST
  if (strcmp(funcname, "OnlineCost") == 0) {
    int clifd = demkstr_int(ws->token_list[2]);
    char *id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    char *costpasswd = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                                   demkstr_string(ws->token_list[4]));
    int fmindex = demkstr_int(ws->token_list[5]);
    char *fmname = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                               demkstr_string(ws->token_list[6]));
    SaacServer_OnlineCost_recv(fd, clifd, id, costpasswd, fmindex, fmname);
    return 0;
  }
#endif

#ifdef _SQL_BUY_FUNC
  if (strcmp(funcname, "OnlineBuy") == 0) {
    int clifd = demkstr_int(ws->token_list[2]);
    char *id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    char *costpasswd = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                                   demkstr_string(ws->token_list[4]));
    SaacServer_OnlineBuy_recv(fd, clifd, id, costpasswd);
    return 0;
  }
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
  if (strcmp(funcname, "OldToNew") == 0) {
    int clifd = demkstr_int(ws->token_list[2]);
    char *id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    int point = demkstr_int(ws->token_list[4]);
    SaacServer_OldToNew_recv(fd, clifd, id, point);
    return 0;
  }
#endif

#ifdef _FORMULATE_AUTO_PK
  if (strcmp(funcname, "FormulateAutoPk") == 0) {
    int clifd = demkstr_int(ws->token_list[2]);
    char *id = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                           demkstr_string(ws->token_list[3]));
    int point = demkstr_int(ws->token_list[4]);
    SaacServer_FormulateAutoPk_recv(fd, clifd, id, point);
    return 0;
  }
#endif

#ifdef _LOTTERY_SYSTEM
  if (strcmp(funcname, "LotterySystem") == 0) {
    SaacServer_LotterySystem_recv(fd);
    return 0;
  }
#endif

#ifdef _ALL_SERV_SEND
  if (strcmp(funcname, "AllServSend") == 0) {
    char *data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                             demkstr_string(ws->token_list[2]));
    SaacServer_AllServSend_recv(data);
    return 0;
  }
#endif
  sprintf(debugfun, "%s", funcname);
  return -1;
}

void SaacServer_ACServerLogin_send(const int fd, const char *result,
                                   const char *data) {
  CreateHeader(ws->work, "ACServerLogin");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACCharList_send(int fd, char *result, char *output, int id) {
  CreateHeader(ws->work, "ACCharList");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(output), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
static int CharDataLens = 0;
#ifdef _NewSave
void SaacServer_ACCharLoad_send(int fd, char *result, char *data, int id,
                                int charindex)
#else
void SaacServer_ACCharLoad_send(int fd, char *result, char *data, int id)
#endif
{
  if (strstr(result, "successful") != NULL && strlen(data) > CharDataLens) {
    CharDataLens = strlen(data);
    // andy_log
    logErr("最大数据长度:%d\n", CharDataLens);
  }

  CreateHeader(ws->work, "ACCharLoad");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
#ifdef _NewSave
  strncatsafe(ws->work, mkstr_int(charindex), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

void SaacServer_ACCharSave_send(int fd, char *result, char *data, int id) {
  CreateHeader(ws->work, "ACCharSave");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACCharDelete_send(int fd, char *result, char *data, int id) {
  CreateHeader(ws->work, "ACCharDelete");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACLock_send(int fd, char *result, char *data, int id) {
  CreateHeader(ws->work, "ACLock");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACUCheck_send(int fd, char *mem_id) {
  CreateHeader(ws->work, "ACUCheck");
  strncatsafe(ws->work, mkstr_string(mem_id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBUpdateEntryString_send(int fd, char *result, char *table,
                                         char *key, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBUpdateEntryString");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBDeleteEntryString_send(int fd, char *result, char *table,
                                         char *key, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBDeleteEntryString");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBGetEntryString_send(int fd, char *result, char *value,
                                      char *table, char *key, int msgid,
                                      int msgid2) {
  CreateHeader(ws->work, "DBGetEntryString");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(value), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void SaacServer_UpdataStele_send(int fd, char *data) {
  CreateHeader(ws->work, "UpdataStele");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_S_UpdataStele_send(int fd, char *ocdkey, char *oname,
                                   char *ncdkey, char *nname, char *title,
                                   int level, int trns, int floor) {
  CreateHeader(ws->work, "S_UpdataStele");
  strncatsafe(ws->work, mkstr_string(ocdkey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(oname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(ncdkey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(nname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(title), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(level), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(trns), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(floor), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif
void SaacServer_DBUpdateEntryInt_send(int fd, char *result, char *table,
                                      char *key, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBUpdateEntryInt");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBGetEntryRank_send(int fd, char *result, int rank, int count,
                                    char *table, char *key, int msgid,
                                    int msgid2) {
  CreateHeader(ws->work, "DBGetEntryRank");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(rank), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(count), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBDeleteEntryInt_send(int fd, char *result, char *table,
                                      char *key, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBDeleteEntryInt");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBGetEntryInt_send(int fd, char *result, int value, char *table,
                                   char *key, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBGetEntryInt");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(value), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBGetEntryByRank_send(int fd, char *result, char *list,
                                      char *table, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBGetEntryByRank");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(list), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_DBGetEntryByCount_send(int fd, char *result, char *list,
                                       char *table, int count_start, int msgid,
                                       int msgid2) {
  CreateHeader(ws->work, "DBGetEntryByCount");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(list), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(count_start), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_Broadcast_send(int fd, char *id, char *charname,
                               char *message) {
  CreateHeader(ws->work, "Broadcast");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(message), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_Message_send(int fd, char *id_from, char *charname_from,
                             char *id_to, char *charname_to, char *message,
                             int option, int mesgid) {
  CreateHeader(ws->work, "Message");
  strncatsafe(ws->work, mkstr_string(id_from), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname_from), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id_to), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname_to), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(message), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(option), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(mesgid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 成立家族
void SaacServer_ACAddFM_send(int fd, char *result, int fmindex, int index,
                             int charfdid) {
  CreateHeader(ws->work, "ACAddFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// 加入家族
void SaacServer_ACJoinFM_send(int fd, char *result, int recv, int charfdid) {
  CreateHeader(ws->work, "ACJoinFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(recv), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 退出家族
void SaacServer_ACLeaveFM_send(int fd, char *result, int resultflag,
                               int charfdid) {
  CreateHeader(ws->work, "ACLeaveFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(resultflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 解散家族
void SaacServer_ACDelFM_send(int fd, char *result, int charfdid) {
  CreateHeader(ws->work, "ACDelFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 列出家族列表
void SaacServer_ACShowFMList_send(int fd, char *result, int num, char *data) {
  CreateHeader(ws->work, "ACShowFMList");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#ifdef _FAMILY_TOTEM
// 列出家族图腾
void SaacServer_ACShowFMTotem_send(int fd, char *result, int num, char *data) {
  CreateHeader(ws->work, "ACShowFMTotem");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif
// 列出成员列表
void SaacServer_ACShowMemberList_send(int fd, char *result, int index, int num,
                                      int acceptflag, int fmjoinnum, char *data
#ifdef _FAMILYBADGE_
                                      ,
                                      int badge
#endif
) {
  CreateHeader(ws->work, "ACShowMemberList");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(acceptflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmjoinnum), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
#ifdef _FAMILYBADGE_
  strncatsafe(ws->work, mkstr_int(badge), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

// 列出家族详细资料
void SaacServer_ACFMDetail_send(int fd, char *result, char *data,
                                int charfdid) {
  CreateHeader(ws->work, "ACFMDetail");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 族长审核成员加入家族
void SaacServer_ACMemberJoinFM_send(int fd, char *result, int charfdid) {
  CreateHeader(ws->work, "ACMemberJoinFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 族长审核成员离开家族
void SaacServer_ACMemberLeaveFM_send(int fd, char *result, int charfdid) {
  CreateHeader(ws->work, "ACMemberLeaveFM");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 家族成员login
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
void SaacServer_ACFMCharLogin_send(int fd, char *result, int index, int floor,
                                   int fmpopular, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfame,
                                   int charfdid,
#ifdef _NEW_MANOR_LAW
                                   int momentum
#endif
)
#else
void SaacServer_ACFMCharLogin_send(int fd, char *result, int index, int floor,
                                   int fmpopular, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfdid)
#endif
{
  CreateHeader(ws->work, "ACFMCharLogin");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(floor), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmpopular), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(joinflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmsetupflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charindex), ws->work_buf_size);
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
  strncatsafe(ws->work, mkstr_int(charfame), ws->work_buf_size);
#endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
#ifdef _NEW_MANOR_LAW
  strncatsafe(ws->work, mkstr_int(momentum), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

// 家族成员logout
void SaacServer_ACFMCharLogout_send(int fd, char *result, int charfdid) {
  CreateHeader(ws->work, "ACFMCharLogout");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 读取家族留言板
void SaacServer_ACFMReadMemo_send(int fd, char *result, int index, int num,
                                  int dataindex, char *data) {
  CreateHeader(ws->work, "ACFMReadMemo");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(dataindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 写入家族留言板
void SaacServer_ACFMWriteMemo_send(int fd, char *result, int index) {
  CreateHeader(ws->work, "ACFMWriteMemo");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 列出家族据点
void SaacServer_ACFMPointList_send(int fd, char *result, char *data) {
  CreateHeader(ws->work, "ACFMPointList");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 申请家族据点
void SaacServer_ACSetFMPoint_send(int fd, char *result, int r, int charfdid) {
  CreateHeader(ws->work, "ACSetFMPoint");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(r), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 设定家族据点
void SaacServer_ACFixFMPoint_send(int fd, char *result, int r) {
  CreateHeader(ws->work, "ACFixFMPoint");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(r), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// 族长对星系家族成员广播
void SaacServer_ACFMAnnounce_send(int fd, char *result, char *fmname,
                                  int fmindex, int index, int kindflag,
                                  char *data, int color) {
  CreateHeader(ws->work, "ACFMAnnounce");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(color), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// 列出家族排行榜
void SaacServer_ACShowTopFMList_send(int fd, char *result, int kindflag,
                                     int num, char *data) {
  CreateHeader(ws->work, "ACShowTopFMList");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACFixFMData_send(int fd, char *result, int kindflag,
                                 char *data1, char *data2, int charfdid) {
  CreateHeader(ws->work, "ACFixFMData");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data1), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data2), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// 设定家族据点
void SaacServer_ACFixFMPK_send(int fd, char *result, int award, int winindex,
                               int loseindex) {
  CreateHeader(ws->work, "ACFixFMPK");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(award), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(winindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(loseindex), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// GM修正家族资料
void SaacServer_ACGMFixFMData_send(int fd, char *result, char *fmname,
                                   int charfdid) {
  CreateHeader(ws->work, "ACGMFixFMData");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// 取得家族资料
void SaacServer_ACGetFMData_send(int fd, char *result, int kindflag, int data,
                                 int charfdid) {
  CreateHeader(ws->work, "ACGetFMData");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
//
void SaacServer_ACFMClearPK_send(int fd, char *result, char *fmname,
                                 int fmindex, int index) {
  CreateHeader(ws->work, "ACFMClearPK");
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
// 通知其他星球的踢馆人
void SaacServer_ACManorPKAck_send(int fd, char *data) {
  CreateHeader(ws->work, "ACManorPKAck");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
void SaacServer_ACLoadFmPk_send(int fd, char *data) {
  CreateHeader(ws->work, "ACLoadFmPk");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#ifdef _ACFMPK_LIST
void SaacServer_ACSendFmPk_send(int fd, int userindex, int flg) {
  CreateHeader(ws->work, "ACSendFmPk");
  strncatsafe(ws->work, mkstr_int(userindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flg), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#endif

#ifdef _WAEI_KICK
void SaacServer_ACKick_send(int fd, int act, char *data, int id) {
  CreateHeader(ws->work, "ACKick");
  strncatsafe(ws->work, mkstr_int(act), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
void SaacServer_SendEffect_send(int fd, char *effect) {
  CreateHeader(ws->work, "EFFECT");
  strncatsafe(ws->work, mkstr_string(effect), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _CHAR_POOLITEM
void SaacServer_ACCharSavePoolItem_send(int fd, char *Acces, char *Pooldataarg,
                                        int clifdid) {
  CreateHeader(ws->work, "ACSavePoolItem");
  strncatsafe(ws->work, mkstr_string(Acces), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACCharGetPoolItem_send(int fd, char *Acces, char *Pooldataarg,
                                       int clifdid, int npcid) {
  CreateHeader(ws->work, "ACGetPoolItem");
  strncatsafe(ws->work, mkstr_string(Acces), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(npcid), ws->work_buf_size);

  Send(ws, fd, ws->work);
}
#endif

#ifdef _CHAR_POOLPET
void SaacServer_ACCharSavePoolPet_send(int fd, char *Acces, char *Pooldataarg,
                                       int clifdid) {
  CreateHeader(ws->work, "ACSavePoolPet");
  strncatsafe(ws->work, mkstr_string(Acces), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ACCharGetPoolPet_send(int fd, char *Acces, char *Pooldataarg,
                                      int clifdid, int npcid) {
  CreateHeader(ws->work, "ACGetPoolPet");
  strncatsafe(ws->work, mkstr_string(Acces), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(npcid), ws->work_buf_size);

  Send(ws, fd, ws->work);
}
#endif

#ifdef _ANGEL_SUMMON
void SaacServer_ACMissionTable_send(int fd, int num, int type, char *data,
                                    char *angelinfo) {
  CreateHeader(ws->work, "ACMissionTable");
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(type), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(angelinfo), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _TEACHER_SYSTEM
void SaacServer_ACCheckCharacterOnLine_send(int fd, int charaindex, int iOnline,
                                            char *data, int flag) {
  CreateHeader(ws->work, "ACCheckCharacterOnLine");
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(iOnline), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

void SaacServer_ACCharLogin_send(int fd, int clifd, int flag) {
  CreateHeader(ws->work, "ACCharLogin");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  printf("[SAAC发送结果] fd=%d clifd=%d flag=%d" 
         "(0=成功,1=参数错误,2=账号锁定,3=IP/MAC锁定,5=未注册,6=密码错)"
         " content=%s......",
         fd, clifd, flag, ws->work);
  Send(ws, fd, ws->work);
  printf("成功\n");
}

#ifdef _NEW_VIP_SHOP
void SaacServer_QueryPoint_send(int fd, int clifd, int point) {
  CreateHeader(ws->work, "QueryPoint");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_NewVipShop_send(int fd, int clifd, int point, char *buf,
                                int flag) {
  CreateHeader(ws->work, "NewVipShop");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(buf), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _ITEM_PET_LOCKED
void SaacServer_ItemPetLocked_send(int fd, int clifd, int flag, char *data) {
  CreateHeader(ws->work, "ItemPetLocked");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacServer_ItemPetLockedPasswd_send(int fd, int clifd, char *data) {
  CreateHeader(ws->work, "ItemPetLockedPasswd");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _ONLINE_COST
void SaacServer_OnlineCost_send(int fd, int clifd, char *data) {
  CreateHeader(ws->work, "OnlineCost");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _SQL_BUY_FUNC
void SaacServer_OnlineBuy_send(int fd, int clifd, char *data) {
  CreateHeader(ws->work, "OnlineBuy");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void SaacServer_OldToNew_send(int fd, int clifd, char *data) {
  CreateHeader(ws->work, "OldToNew");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void SaacServer_FormulateAutoPk_send(int fd, int clifd, char *data) {
  CreateHeader(ws->work, "FormulateAutoPk");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _LOTTERY_SYSTEM
void SaacServer_LotterySystem_send(int fd, char *award) {
  CreateHeader(ws->work, "LotterySystem");
  strncatsafe(ws->work, mkstr_string(award), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _ALL_SERV_SEND
void SaacServer_AllServSend_send(int fd, char *data) {
  CreateHeader(ws->work, "AllServSend");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif
