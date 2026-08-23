#define __SAAC_CLIENT_SEND_C__
#include "saac_client.h"

#ifdef _SEND_EFFECT
#include "chatmagic.h"
#endif

extern WorkSpace gSaacWorkSpace;
WorkSpace *ws = &gSaacWorkSpace;

int SaacClient_InitClient(int (*write_func)(int, const char *, int),
                          int work_buffer_size, int fd) {
  (void)fd;
  if (ws->work != NULL) {
    ws->write_func = write_func;
    return 0;
  }
  return InitWorkSpace(ws, write_func, work_buffer_size,
                       SAAC_CLIENT_MAXLSRPCARGS);
}

// AccountServeFD 定义在其他地方, 使用acfd的地方会有很多bug，这里要注意
extern int acfd;

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
void SaacClient_UpdataStele_send(int fd, char *cdkey, char *name, char *title,
                                 int level, int trans, int time, int floor) {
  CreateHeader(ws->work, "UpdataStele");
  strncatsafe(ws->work, mkstr_string(cdkey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(name), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(title), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(level), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(trans), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(time), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(floor), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#if _ATTESTAION_ID == 1
void SaacClient_ACServerLogin_send(int fd, int id, char *servername,
                                   char *serverpass)
#else
void SaacClient_ACServerLogin_send(int fd, char *servername, char *serverpass)
#endif
{
  CreateHeader(ws->work, "ACServerLogin");
#if _ATTESTAION_ID == 1
  strncatsafe(ws->work, mkstr_int(id), ws->work_buf_size);
#endif
  strncatsafe(ws->work, mkstr_string(servername), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(serverpass), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACServerLogout_send(int fd) {
  CreateHeader(ws->work, "ACServerLogout");
  strncatsafe(ws->work, "", ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _PKSEVER_VER
void SaacClient_ACCharList_send(int fd, char *id, char *pas, int mesgid,
                                int star)
#else
void SaacClient_ACCharList_send(int fd, char *id, char *pas, char *ip,
                                char *mac, int msg_id, int char_list_flag)
#endif
{
  CreateHeader(ws->work, "ACCharList");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(pas), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(ip), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(mac), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msg_id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(char_list_flag), ws->work_buf_size);
#ifdef _PKSEVER_VER
  strncatsafe(ws->work, mkstr_int(star), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

void SaacClient_ACCharLoad_send(int fd, char *id, char *pas, char *char_name,
                                int lock, char *opt, int msg_id) {
  CreateHeader(ws->work, "ACCharLoad");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(pas), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(char_name), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(lock), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(opt), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msg_id), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _NEWSAVE // USE
void SaacClient_ACCharSave_send(int fd, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int msg_id,
                                int saveindex)
#else
void SaacClient_ACCharSave_send(int fd, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int msg_id)
#endif
{
  CreateHeader(ws->work, "ACCharSave");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(opt), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charinfo), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(unlock), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msg_id), ws->work_buf_size);
#ifdef _NEWSAVE
  strncatsafe(ws->work, mkstr_int(saveindex), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

void SaacClient_ACCharDelete_send(int fd, char *id, char *passwd,
                                  char *charname, char *option, int mesgid) {
  CreateHeader(ws->work, "ACCharDelete");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(passwd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(option), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(mesgid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACLock_send(int fd, char *id, int lock, int mesgid) {
  CreateHeader(ws->work, "ACLock");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(lock), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(mesgid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACUCheck_send(int fd, char *mem_id, int status) {
  CreateHeader(ws->work, "ACUCheck");
  strncatsafe(ws->work, mkstr_string(mem_id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(status), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBUpdateEntryString_send(int fd, char *table, char *key,
                                         char *value, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBUpdateEntryString");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(value), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBDeleteEntryString_send(int fd, char *table, char *key,
                                         int msgid, int msgid2) {
  CreateHeader(ws->work, "DBDeleteEntryString");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBGetEntryString_send(int fd, char *table, char *key, int msg_id,
                                      int msg_id2) {
  CreateHeader(ws->work, "DBGetEntryString");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msg_id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msg_id2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBUpdateEntryInt_send(int fd, char *table, char *key, int value,
                                      char *info, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBUpdateEntryInt");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(value), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(info), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBGetEntryRank_send(int fd, char *table, char *key, int msgid,
                                    int msgid2) {
  CreateHeader(ws->work, "DBGetEntryRank");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBDeleteEntryInt_send(int fd, char *table, char *key, int msgid,
                                      int msgid2) {
  CreateHeader(ws->work, "DBDeleteEntryInt");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBGetEntryInt_send(int fd, char *table, char *key, int msgid,
                                   int msgid2) {
  CreateHeader(ws->work, "DBGetEntryInt");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(key), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBGetEntryByRank_send(int fd, char *table, int rank_start,
                                      int rank_end, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBGetEntryByRank");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(rank_start), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(rank_end), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_DBGetEntryByCount_send(int fd, char *table, int count_start,
                                       int num, int msgid, int msgid2) {
  CreateHeader(ws->work, "DBGetEntryByCount");
  strncatsafe(ws->work, mkstr_string(table), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(count_start), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(msgid2), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_Broadcast_send(const int fd, const char *id,
                               const char *charname, const char *message,
                               const int flag) {}

void SaacClient_Message_send(int fd, char *id_from, char *char_name_from,
                             char *id_to, char *char_name_to, char *message,
                             int option) {
  CreateHeader(ws->work, "Message");
  strncatsafe(ws->work, mkstr_string(id_from), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(char_name_from), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id_to), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(char_name_to), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(message), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(option), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_MessageAck_send(int fd, char *id, char *charname, char *result,
                                int mesgid) {
  CreateHeader(ws->work, "MessageAck");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(result), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(mesgid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_MessageFlush_send(int fd, char *id, char *charname) {
  CreateHeader(ws->work, "MessageFlush");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _PERSONAL_FAME
void SaacClient_ACAddFM_send(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petattr, char *fmrule, int fmsprite,
                             int fmleadergrano, int fame,
#ifdef _FAMILYBADGE_
                             int fmbadge,
#endif
                             int charfdid)
#else
void SaacClient_ACAddFM_send(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petattr, char *fmrule, int fmsprite,
                             int fmleadergrano, int charfdid)
#endif
{
  CreateHeader(ws->work, "ACAddFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmleadername), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmleaderid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmleaderlv), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(petname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(petattr), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmrule), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmsprite), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmleadergrano), ws->work_buf_size);
  // #ifdef _PERSONAL_FAME
  strncatsafe(ws->work, mkstr_int(fame), ws->work_buf_size);
  // #endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
#ifdef _FAMILYBADGE_
  strncatsafe(ws->work, mkstr_int(fmbadge), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
void SaacClient_ACJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
                              char *charid, int charlv, int index, int fame,
                              int charfdid)
#else
void SaacClient_ACJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
                              char *charid, int charlv, int index, int charfdid)
#endif
{
  CreateHeader(ws->work, "ACJoinFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charlv), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  // #ifdef _PERSONAL_FAME  // Arminius: 家族个人声望
  strncatsafe(ws->work, mkstr_int(fame), ws->work_buf_size);
  // #endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACLeaveFM_send(int fd, char *fmname, int fmindex,
                               char *charname, char *charid, int index,
                               int charfdid) {
  CreateHeader(ws->work, "ACLeaveFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACDelFM_send(int fd, char *fmname, int fmindex, int index,
                             char *charname, char *charid, int charfdid) {
  CreateHeader(ws->work, "ACDelFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACShowFMList_send(int fd) {
  CreateHeader(ws->work, "ACShowFMList");
  Send(ws, fd, ws->work);
}

void SaacClient_ACShowMemberList_send(int fd, int index) {
  CreateHeader(ws->work, "ACShowMemberList");
  strncatsafe(ws->work, mkstr_string("MemberList"), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFMDetail_send(int fd, char *fmname, int fmindex, int index,
                                int charfdid) {
  CreateHeader(ws->work, "ACFMDetail");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#ifdef _FMVER21
void SaacClient_ACMemberJoinFM_send(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int meindex, int charfdid)
#else
void SaacClient_ACMemberJoinFM_send(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int charfdid)
#endif
{
  CreateHeader(ws->work, "ACMemberJoinFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(result), ws->work_buf_size);
#ifdef _FMVER21
  strncatsafe(ws->work, mkstr_int(meindex), ws->work_buf_size);
#endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#ifdef _FMVER21
void SaacClient_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int meindex, int charfdid)
#else
void SaacClient_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int charfdid)
#endif
{
  CreateHeader(ws->work, "ACMemberLeaveFM");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
#ifdef _FMVER21
  strncatsafe(ws->work, mkstr_int(meindex), ws->work_buf_size);
#endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#ifdef _FM_MODIFY
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int eventflag, int charfdid, int gsnum)
#else
#ifdef _FMVER21
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int eventflag, int charfdid)
#else
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int charfdid)
#endif
#endif
{
  CreateHeader(ws->work, "ACFMCharLogin");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charlv), ws->work_buf_size);
#ifdef _FMVER21
  strncatsafe(ws->work, mkstr_int(eventflag), ws->work_buf_size);
#endif
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
#ifdef _FM_MODIFY
  strncatsafe(ws->work, mkstr_int(gsnum), ws->work_buf_size);
#endif
  Send(ws, fd, ws->work);
}

void SaacClient_ACFMCharLogout_send(int fd, char *fmname, int fmindex,
                                    char *charname, char *charid, int charlv,
                                    int index, int charfdid) {
  CreateHeader(ws->work, "ACFMCharLogout");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charlv), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFMReadMemo_send(int fd, int index) {
  CreateHeader(ws->work, "ACFMReadMemo");
  strncatsafe(ws->work, mkstr_string("ReadMemo"), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFMWriteMemo_send(int fd, char *fmname, int fmindex,
                                   char *data, int index) {
  CreateHeader(ws->work, "ACFMWriteMemo");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFMPointList_send(int fd) {
  CreateHeader(ws->work, "ACFMPointList");
  Send(ws, fd, ws->work);
}

void SaacClient_ACSetFMPoint_send(int fd, char *fmname, int fmindex, int index,
                                  int fmpointindex, int fl, int x, int y,
                                  int charfdid) {
  CreateHeader(ws->work, "ACSetFMPoint");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmpointindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fl), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(x), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(y), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFixFMPoint_send(int fd, char *winfmname, int winfmindex,
                                  int winindex, char *losefmname,
                                  int losefmindex, int loseindex, int village) {
  CreateHeader(ws->work, "ACFixFMPoint");
  strncatsafe(ws->work, mkstr_string(winfmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(winfmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(winindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(losefmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(losefmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(loseindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(village), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFMAnnounce_send(int fd, char *fmname, int fmindex, int index,
                                  char *data, int color) {
  CreateHeader(ws->work, "ACFMAnnounce");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(color), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACShowTopFMList_send(int fd, int kindflag) {
  CreateHeader(ws->work, "ACShowTopFMList");
  strncatsafe(ws->work, mkstr_string("ShowTopFMList"), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACFixFMData_send(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, char *data1, char *data2,
                                 int charindex, int charfdid) {
  CreateHeader(ws->work, "ACFixFMData");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data1), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data2), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACFixFMPK_send(int fd, char *winfmname, int winfmindex,
                               int winindex, char *losefmname, int losefmindex,
                               int loseindex) {
  CreateHeader(ws->work, "ACFixFMPK");
  strncatsafe(ws->work, mkstr_string(winfmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(winfmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(winindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(losefmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(losefmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(loseindex), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACGMFixFMData_send(int fd, int index, char *charid, char *cmd,
                                   char *data, int charfdid) {
  CreateHeader(ws->work, "ACGMFixFMData");
  strncatsafe(ws->work, mkstr_string("ACGMFixFMData"), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(charid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(cmd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
void SaacClient_ACGetFMData_send(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, int charfdid) {
  CreateHeader(ws->work, "ACGetFMData");
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(index), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kindflag), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charfdid), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

// Arminius 7.26 manor pk acknowledge
void SaacClient_ACManorPKAck_send(int fd, char *data) {
  CreateHeader(ws->work, "ACManorPKAck");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACreLoadFmData_send(int fd, int type, int data) {
  CreateHeader(ws->work, "ACreLoadFmData");
  strncatsafe(ws->work, mkstr_int(type), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _ACFMPK_LIST
void SaacClient_ACLoadFmPk_send(int fd, int fmpks_pos) {
  CreateHeader(ws->work, "ACLoadFmPk");
  strncatsafe(ws->work, mkstr_int(fmpks_pos), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

void SaacClient_ACSendFmPk_send(int fd, int toindex, int PkFlg, int fmpks_pos,
                                char *msg) {
  CreateHeader(ws->work, "ACSendFmPk");
  strncatsafe(ws->work, mkstr_int(fmpks_pos), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(toindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(PkFlg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(msg), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

void SaacClient_ACAuctionSold_send(int fd, char *data) {
  CreateHeader(ws->work, "ACAuctionSold");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

extern char saacretfunc[255];
#ifdef _ABSOLUTE_DEBUG
extern int lastfunctime;
#endif
int SaacClient_ClientDispatchMessage(int fd, char *line) {
  int msgid;
  char funcname[255];
  strncpysafe(ws->work, ws->work_buf_size, line);
  if (SplitString(ws->work, ws) < 2) {
    saacretfunc[0] = '\0';
    return -1;
  }
  GetMessageInfo(&msgid, funcname, sizeof(funcname), ws->token_list);
  SAAC_LOG_RECV(funcname, "msgid=%u fd=%d", msgid, fd);
  strcpy(saacretfunc, funcname);
#ifdef _ABSOLUTE_DEBUG
  lastfunctime = 1;
#endif
  if (strcmp(funcname, "ACGmsvDownRequest") == 0) {
    int min;
    min = demkstr_int(ws->token_list[2]);
    SaacClient_ACGmsvDownRequest_recv(fd, min);
    return 0;
  }

  if (strcmp(funcname, "ACServerLogin") == 0) {
    char *result;
    char *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    SaacClient_ACServerLogin_recv(fd, result, data);
    return 0;
  }

  if (strcmp(funcname, "ACCharList") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *output = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                demkstr_string(ws->token_list[3]));
    const int id = demkstr_int(ws->token_list[4]);
    SaacClient_ACCharList_recv(fd, result, output, id);
    return 0;
  }

  if (strcmp(funcname, "ACCharLoad") == 0) {
    char *result = NULL;
    char *data = NULL;
    int id;
#ifdef _NEWSAVE
    int saveindex;
#endif
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    if (data == NULL)
      return 0;
    if (result == NULL)
      return 0;

#ifdef _NEWSAVE
    saveindex = demkstr_int(ws->token_list[5]);
    SaacClient_ACCharLoad_recv(fd, result, data, id, saveindex);
#else
    SaacClient_ACCharLoad_recv(fd, result, data, id, -1);
#endif
    return 0;
  }

  if (strcmp(funcname, "ACCharSave") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    const int id = demkstr_int(ws->token_list[4]);
    SaacClient_ACCharSave_recv(fd, result, data, id);
    return 0;
  }

  if (strcmp(funcname, "ACCharDelete") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    const int id = demkstr_int(ws->token_list[4]);
    SaacClient_ACCharDelete_recv(fd, result, data, id);
    return 0;
  }

  if (strcmp(funcname, "ACLock") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    const int id = demkstr_int(ws->token_list[4]);
    SaacClient_ACLock_recv(fd, result, data, id);
    return 0;
  }

#ifdef _WAEI_KICK
  if (strcmp(funcname, "ACKick") == 0) {
    int act;
    char *data;
    int id;
    act = demkstr_int(ws->token_list[2]);
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    SaacClient_ACKick_recv(fd, act, data, id);
    return 0;
  }
#endif

#ifdef _CHAR_POOLITEM

  if (strcmp(funcname, "ACSavePoolItem") == 0) {
    char *result;
    char *data;
    int id;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    SaacClient_ACCharSavePoolItem_recv(fd, result, data, id);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolItem") == 0) {
    char *result;
    char *data;
    int id, npcid;

    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    npcid = demkstr_int(ws->token_list[5]);
    SaacClient_ACCharGetPoolItem_recv(fd, result, data, id, npcid);
    return 0;
  }
#endif

#ifdef _CHAR_POOLPET

  if (strcmp(funcname, "ACSavePoolPet") == 0) {
    char *result;
    char *data;
    int id;

    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    SaacClient_ACCharSavePoolPet_recv(fd, result, data, id);
    return 0;
  }

  if (strcmp(funcname, "ACGetPoolPet") == 0) {
    char *result;
    char *data;
    int id, npcid;

    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    id = demkstr_int(ws->token_list[4]);
    npcid = demkstr_int(ws->token_list[5]);
    SaacClient_ACCharGetPoolPet_recv(fd, result, data, id, npcid);
    return 0;
  }
#endif

  if (strcmp(funcname, "ACUCheck") == 0) {
    char *mem_id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    SaacClient_ACUCheck_recv(fd, mem_id);
    return 0;
  }
  if (strcmp(funcname, "DBUpdateEntryString") == 0) {
    char *result;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    table = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                         demkstr_string(ws->token_list[3]));
    key = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    msgid = demkstr_int(ws->token_list[5]);
    msgid2 = demkstr_int(ws->token_list[6]);
    SaacClient_DBUpdateEntryString_recv(fd, result, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBDeleteEntryString") == 0) {
    char *result;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    table = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                         demkstr_string(ws->token_list[3]));
    key = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    msgid = demkstr_int(ws->token_list[5]);
    msgid2 = demkstr_int(ws->token_list[6]);
    SaacClient_DBDeleteEntryString_recv(fd, result, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryString") == 0) {
    char *result;
    char *value;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    value = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                         demkstr_string(ws->token_list[3]));
    table = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                         demkstr_string(ws->token_list[4]));
    key = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                       demkstr_string(ws->token_list[5]));
    msgid = demkstr_int(ws->token_list[6]);
    msgid2 = demkstr_int(ws->token_list[7]);
    SaacClient_DBGetEntryString_recv(fd, result, value, table, key, msgid,
                                     msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBUpdateEntryInt") == 0) {
    char *result;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    table = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                         demkstr_string(ws->token_list[3]));
    key = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                       demkstr_string(ws->token_list[4]));
    msgid = demkstr_int(ws->token_list[5]);
    msgid2 = demkstr_int(ws->token_list[6]);
    SaacClient_DBUpdateEntryInt_recv(fd, result, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryRank") == 0) {
    char *result;
    int rank;
    int count;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    rank = demkstr_int(ws->token_list[3]);
    count = demkstr_int(ws->token_list[4]);
    table = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    key = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                       demkstr_string(ws->token_list[6]));
    msgid = demkstr_int(ws->token_list[7]);
    msgid2 = demkstr_int(ws->token_list[8]);
    SaacClient_DBGetEntryRank_recv(fd, result, rank, count, table, key, msgid,
                                   msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBDeleteEntryInt") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *table = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                               demkstr_string(ws->token_list[3]));
    char *key = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                             demkstr_string(ws->token_list[4]));
    const int msg_id = demkstr_int(ws->token_list[5]);
    const int msg_id2 = demkstr_int(ws->token_list[6]);
    SaacClient_DBDeleteEntryInt_recv(fd, result, table, key, msg_id, msg_id2);
    return 0;
  }
  if (strcmp(funcname, "DBGetEntryInt") == 0) {
    char *result;
    int value;
    char *table;
    char *key;
    int msgid;
    int msgid2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    value = demkstr_int(ws->token_list[3]);
    table = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                         demkstr_string(ws->token_list[4]));
    key = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                       demkstr_string(ws->token_list[5]));
    msgid = demkstr_int(ws->token_list[6]);
    msgid2 = demkstr_int(ws->token_list[7]);
    SaacClient_DBGetEntryInt_recv(fd, result, value, table, key, msgid, msgid2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByRank") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *list = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    char *table = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                               demkstr_string(ws->token_list[4]));
    const int msg_id = demkstr_int(ws->token_list[5]);
    const int msg_id2 = demkstr_int(ws->token_list[6]);
    SaacClient_DBGetEntryByRank_recv(fd, result, list, table, msg_id, msg_id2);
    return 0;
  }

  if (strcmp(funcname, "DBGetEntryByCount") == 0) {
    char *result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *list = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    char *table = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                               demkstr_string(ws->token_list[4]));
    const int count_start = demkstr_int(ws->token_list[5]);
    const int msg_id = demkstr_int(ws->token_list[6]);
    const int msg_id2 = demkstr_int(ws->token_list[7]);
    SaacClient_DBGetEntryByCount_recv(fd, result, list, table, count_start,
                                      msg_id, msg_id2);
    return 0;
  }
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
  if (strcmp(funcname, "UpdataStele") == 0) {
    char *token = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                               demkstr_string(ws->token_list[2]));
    SaacClient_UpdataStele_recv(fd, token);
    return 0;
  }
  if (strcmp(funcname, "S_UpdataStele") == 0) {
    char *ocdkey = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    char *oname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                               demkstr_string(ws->token_list[3]));
    char *ncdkey = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                                demkstr_string(ws->token_list[4]));
    char *nname = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                               demkstr_string(ws->token_list[5]));
    char *title = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                               demkstr_string(ws->token_list[6]));
    const int level = demkstr_int(ws->token_list[7]);
    const int trns = demkstr_int(ws->token_list[8]);
    const int floor = demkstr_int(ws->token_list[9]);
    SaacClient_S_UpdataStele_recv(fd, ocdkey, oname, ncdkey, nname, title,
                                  level, trns, floor);
    return 0;
  }
#endif

  if (strcmp(funcname, "Broadcast") == 0) {
    char *id = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                            demkstr_string(ws->token_list[2]));
    char *charname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                  demkstr_string(ws->token_list[3]));
    char *message = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                                 demkstr_string(ws->token_list[4]));
    SaacClient_Broadcast_recv(fd, id, charname, message);
    return 0;
  }

  if (strcmp(funcname, "Message") == 0) {
    char *id_from = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                 demkstr_string(ws->token_list[2]));
    char *charname_from = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                                       demkstr_string(ws->token_list[3]));
    char *id_to = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                               demkstr_string(ws->token_list[4]));
    char *charname_to = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                                     demkstr_string(ws->token_list[5]));
    char *message = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                                 demkstr_string(ws->token_list[6]));
    const int option = demkstr_int(ws->token_list[7]);
    const int mesgid = demkstr_int(ws->token_list[8]);
    SaacClient_Message_recv(fd, id_from, charname_from, id_to, charname_to,
                            message, option, mesgid);
    return 0;
  }

  // CoolFish: Family 2001/5/9
  if (strcmp(funcname, "ACAddFM") == 0) {
    int fmindex, index, charfdid;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    fmindex = demkstr_int(ws->token_list[3]);
    index = demkstr_int(ws->token_list[4]);
    charfdid = demkstr_int(ws->token_list[5]);
    SaacClient_ACAddFM_recv(fd, result, fmindex, charfdid, index);
    return 0;
  }
  if (strcmp(funcname, "ACJoinFM") == 0) {
    int charfdid, recv;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    recv = demkstr_int(ws->token_list[3]);
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACJoinFM_recv(fd, result, recv, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACLeaveFM") == 0) {
    int charfdid, resultflag;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    resultflag = demkstr_int(ws->token_list[3]);
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACLeaveFM_recv(fd, result, resultflag, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACDelFM") == 0) {
    int charfdid;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    charfdid = demkstr_int(ws->token_list[3]);
    SaacClient_ACDelFM_recv(fd, result, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACShowFMList") == 0) {
    int fmnum;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    fmnum = demkstr_int(ws->token_list[3]);
    data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                        demkstr_string(ws->token_list[4]));
    SaacClient_ACShowFMList_recv(fd, result, fmnum, data);
    return 0;
  }

  if (strcmp(funcname, "ACShowMemberList") == 0) {
    int index, fmmemnum, fmacceptflag, fmjoinnum;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    fmmemnum = demkstr_int(ws->token_list[4]);
    fmacceptflag = demkstr_int(ws->token_list[5]);
    fmjoinnum = demkstr_int(ws->token_list[6]);
    data = strncpysafe(ws->string_buffer[6], ws->work_buf_size,
                        demkstr_string(ws->token_list[7]));

#ifdef _FAMILYBADGE_
    int badge = demkstr_int(ws->token_list[8]);
#endif
    SaacClient_ACShowMemberList_recv(fd, result, index, fmmemnum, fmacceptflag,
                                     fmjoinnum, data
#ifdef _FAMILYBADGE_
                                     ,
                                     badge
#endif
    );
    return 0;
  }
  if (strcmp(funcname, "ACFMDetail") == 0) {
    int charfdid;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACFMDetail_recv(fd, result, data, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACMemberJoinFM") == 0) {
    int charfdid;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACMemberJoinFM_recv(fd, result, data, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACMemberLeaveFM") == 0) {
    int charfdid;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACMemberLeaveFM_recv(fd, result, data, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACFMCharLogin") == 0) {
    int charfdid, index, floor, fmdp, joinflag, fmsetupflag, flag, charindex;
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
    int charfame;
#endif
#ifdef _NEW_MANOR_LAW
    int momentum;
#endif
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    floor = demkstr_int(ws->token_list[4]);
    fmdp = demkstr_int(ws->token_list[5]);
    joinflag = demkstr_int(ws->token_list[6]);
    fmsetupflag = demkstr_int(ws->token_list[7]);
    flag = demkstr_int(ws->token_list[8]);
    charindex = demkstr_int(ws->token_list[9]);
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
    charfame = demkstr_int(ws->token_list[10]);
    charfdid = demkstr_int(ws->token_list[11]);
#ifdef _NEW_MANOR_LAW
    momentum = demkstr_int(ws->token_list[12]);
#endif
    SaacClient_ACFMCharLogin_recv(fd, result, index, floor, fmdp, joinflag,
                                  fmsetupflag, flag, charindex, charfame,
                                  charfdid
#ifdef _NEW_MANOR_LAW
                                  ,
                                  momentum
#endif
    );
#else
    charfdid = demkstr_int(ws->token_list[10]);
    SaacClient_ACFMCharLogin_recv(fd, result, index, floor, fmdp, joinflag,
                                  fmsetupflag, flag, charindex, charfdid);
#endif
    return 0;
  }
  if (strcmp(funcname, "ACFMCharLogout") == 0) {
    int charfdid;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    charfdid = demkstr_int(ws->token_list[3]);
    SaacClient_ACFMCharLogout_recv(fd, result, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACFMReadMemo") == 0) {
    int index, num, dataindex;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    num = demkstr_int(ws->token_list[4]);
    dataindex = demkstr_int(ws->token_list[5]);
    data = strncpysafe(ws->string_buffer[5], ws->work_buf_size,
                        demkstr_string(ws->token_list[6]));
    SaacClient_ACFMReadMemo_recv(fd, result, index, num, dataindex, data);
    return 0;
  }
  if (strcmp(funcname, "ACFMWriteMemo") == 0) {
    int index;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    index = demkstr_int(ws->token_list[3]);
    SaacClient_ACFMWriteMemo_recv(fd, result, index);
    return 0;
  }
  if (strcmp(funcname, "ACFMPointList") == 0) {
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                        demkstr_string(ws->token_list[3]));
    SaacClient_ACFMPointList_recv(fd, result, data);
    return 0;
  }

  if (strcmp(funcname, "ACSetFMPoint") == 0) {
    int charfdid, r;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    r = demkstr_int(ws->token_list[3]);
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACSetFMPoint_recv(fd, result, r, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACFixFMPoint") == 0) {
    int r;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    r = demkstr_int(ws->token_list[3]);
    SaacClient_ACFixFMPoint_recv(fd, result, r);
    return 0;
  }
  if (strcmp(funcname, "ACFMAnnounce") == 0) {
    int fmindex, index, color, kindflag;
    char *result, *fmname, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    fmname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                          demkstr_string(ws->token_list[3]));
    fmindex = demkstr_int(ws->token_list[4]);
    index = demkstr_int(ws->token_list[5]);
    kindflag = demkstr_int(ws->token_list[6]);
    data = strncpysafe(ws->string_buffer[6], ws->work_buf_size,
                        demkstr_string(ws->token_list[7]));
    color = demkstr_int(ws->token_list[8]);
    SaacClient_ACFMAnnounce_recv(fd, result, fmname, fmindex, index, kindflag,
                                 data, color);
    return 0;
  }
  if (strcmp(funcname, "ACShowTopFMList") == 0) {
    int num, kindflag;
    char *result, *data;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    kindflag = demkstr_int(ws->token_list[3]);
    num = demkstr_int(ws->token_list[4]);
    data = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                        demkstr_string(ws->token_list[5]));
    SaacClient_ACShowTopFMList_recv(fd, result, kindflag, num, data);
    return 0;
  }
  if (strcmp(funcname, "ACFixFMData") == 0) {
    int kindflag, charfdid;
    char *result, *data1, *data2;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    kindflag = demkstr_int(ws->token_list[3]);
    data1 = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                         demkstr_string(ws->token_list[4]));
    data2 = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                         demkstr_string(ws->token_list[5]));
    charfdid = demkstr_int(ws->token_list[6]);
    SaacClient_ACFixFMData_recv(fd, result, kindflag, data1, data2, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACFixFMPK") == 0) {
    int data, winindex, loseindex;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    data = demkstr_int(ws->token_list[3]);
    winindex = demkstr_int(ws->token_list[4]);
    loseindex = demkstr_int(ws->token_list[5]);
    SaacClient_ACFixFMPK_recv(fd, result, data, winindex, loseindex);
    return 0;
  }
  if (strcmp(funcname, "ACGMFixFMData") == 0) {
    int charfdid;
    char *result, *fmname;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    fmname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                          demkstr_string(ws->token_list[3]));
    charfdid = demkstr_int(ws->token_list[4]);
    SaacClient_ACGMFixFMData_recv(fd, result, fmname, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACGetFMData") == 0) {
    int kindflag, charfdid, data;
    char *result;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    kindflag = demkstr_int(ws->token_list[3]);
    data = demkstr_int(ws->token_list[4]);
    charfdid = demkstr_int(ws->token_list[5]);
    SaacClient_ACGetFMData_recv(fd, result, kindflag, data, charfdid);
    return 0;
  }
  if (strcmp(funcname, "ACFMClearPK") == 0) {
    int fmindex, index;
    char *result, *fmname;
    result = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                          demkstr_string(ws->token_list[2]));
    fmname = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                          demkstr_string(ws->token_list[3]));
    fmindex = demkstr_int(ws->token_list[4]);
    index = demkstr_int(ws->token_list[5]);
    SaacClient_ACFMClearPK_recv(fd, result, fmname, fmindex, index);
    return 0;
  }
  // Arminius 7.26 manor pk acknowledge
  if (strcmp(funcname, "ACManorPKAck") == 0) {
    char *data;
    data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    SaacClient_ACManorPKAck_recv(fd, data);
    return 0;
  }

#ifdef _ACFMPK_LIST
  if (strcmp(funcname, "ACLoadFmPk") == 0) {
    char *data;
    data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                        demkstr_string(ws->token_list[2]));
    SaacClient_ACLoadFmPk_recv(fd, data);
    return 0;
  }
  if (strcmp(funcname, "ACSendFmPk") == 0) {
    int toindex, flg;
    toindex = demkstr_int(ws->token_list[2]);
    flg = demkstr_int(ws->token_list[3]);
    SaacClient_ACSendFmPk_recv(fd, toindex, flg);
    return 0;
  }
#endif
#ifdef _SEND_EFFECT // WON ADD AC送下雪、下雨等特效
  if (strcmp(funcname, "EFFECT") == 0) {
    char *effect = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                                demkstr_string(ws->token_list[2]));
    print("\n AC send to GS weather effect !!");
    CHAR_CHAT_DEBUG_sendeffect(fd, effect);
    return 0;
  }
#endif

#ifdef _NEW_VIP_SHOP
  if (strcmp(funcname, "QueryPoint") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    int point = demkstr_int(ws->token_list[3]);
    SaacClient_QueryPoint_recv(fd, point);
    return 0;
  }

  if (strcmp(funcname, "NewVipShop") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    int point = demkstr_int(ws->token_list[3]);
    char *data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                              demkstr_string(ws->token_list[4]));
    int flag = demkstr_int(ws->token_list[5]);
    SaacClient_NewVipShop_recv(fd, point, data, flag);
    return 0;
  }
#endif

#ifdef _ITEM_PET_LOCKED
  if (strcmp(funcname, "ItemPetLocked") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    int flag = demkstr_int(ws->token_list[3]);
    char *data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                              demkstr_string(ws->token_list[4]));
    SaacClient_ItemPetLocked_recv(fd, flag, data);
    return 0;
  }

  if (strcmp(funcname, "ItemPetLockedPasswd") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    SaacClient_ItemPetLockedPasswd_recv(fd, data);
    return 0;
  }
#endif

#ifdef _ONLINE_COST
  if (strcmp(funcname, "OnlineCost") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    SaacClient_OnlineCost_recv(fd, data);
    return 0;
  }
#endif

#ifdef _SQL_BUY_FUNC
  if (strcmp(funcname, "OnlineBuy") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    SaacClient_OnlineBuy_recv(fd, data);
    return 0;
  }
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
  if (strcmp(funcname, "OldToNew") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    SaacClient_OldToNew_recv(fd, data);
    return 0;
  }
#endif

#ifdef _FORMULATE_AUTO_PK
  if (strcmp(funcname, "FormulateAutoPk") == 0) {
    int fd = demkstr_int(ws->token_list[2]);
    char *data = strncpysafe(ws->string_buffer[2], ws->work_buf_size,
                              demkstr_string(ws->token_list[3]));
    SaacClient_FormulateAutoPk_recv(fd, data);
    return 0;
  }
#endif

#ifdef _LOTTERY_SYSTEM
  if (strcmp(funcname, "LotterySystem") == 0) {
    char *data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                              demkstr_string(ws->token_list[2]));
    SaacClient_LotterySystem_recv(data);
    return 0;
  }
#endif

#ifdef _ALL_SERV_SEND
  if (strcmp(funcname, "AllServSend") == 0) {
    char *data = strncpysafe(ws->string_buffer[1], ws->work_buf_size,
                              demkstr_string(ws->token_list[2]));
    SaacClient_AllServSend_recv(data);
    return 0;
  }
#endif

#ifdef _ANGEL_SUMMON
  if (strcmp(funcname, "ACMissionTable") == 0) {
    const int num = demkstr_int(ws->token_list[2]);
    const int type = demkstr_int(ws->token_list[3]);
    char *data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                        demkstr_string(ws->token_list[4]));
    char *angelinfo = strncpysafe(ws->string_buffer[4], ws->work_buf_size,
                             demkstr_string(ws->token_list[5]));
    SaacClient_ACMissionTable_recv(fd, num, type, data, angelinfo);
    return 0;
  }
#endif

#ifdef _TEACHER_SYSTEM
  if (strcmp(funcname, "ACCheckCharacterOnLine") == 0) {
    const int char_index = demkstr_int(ws->token_list[2]);
    const int online = demkstr_int(ws->token_list[3]);
    char *data = strncpysafe(ws->string_buffer[3], ws->work_buf_size,
                        demkstr_string(ws->token_list[4]));
    const int flag = demkstr_int(ws->token_list[5]);
    SaacClient_ACCheckCharacterOnLine_recv(fd, char_index, online, data, flag);
    return 0;
  }
#endif
  if (strcmp(funcname, "ACCharLogin") == 0) {
    const int clifd = demkstr_int(ws->token_list[2]);
    const int flag = demkstr_int(ws->token_list[3]);
    SaacClient_ACCharLogin_recv(fd, clifd, flag);
    return 0;
  }
  printf("不支持的SaacFunctionName: %s.\n", funcname);
  return -1;
}

#ifdef _WAEI_KICK
void SaacClient_ACKick_send(int fd, char *kickid, int kickfd, int flg) {
  CreateHeader(ws->work, "ACKick");
  strncatsafe(ws->work, mkstr_string(kickid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(kickfd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flg), ws->work_buf_size);
  Send(ws, fd, ws->work);
}
#endif

#ifdef _CHAR_POOLITEM
void SaacClient_ACCharSavePoolItem_send(int acfd, int charaindex, int clifdid,
                                        char *CdKey, char *Pooldataarg) {
  CreateHeader(ws->work, "ACSavePoolItem");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

void SaacClient_ACCharGetPoolItem_send(int acfd, int meindex, int charaindex,
                                       int clifdid, char *CdKey) {
  CreateHeader(ws->work, "ACGetPoolItem");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(meindex), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

void SaacClient_ACCharInsertPoolItem_send(int acfd, int charaindex, int clifdid,
                                          char *CdKey, char *Pooldataarg,
                                          int TYPE) {
  CreateHeader(ws->work, "ACInsertPoolItem");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(TYPE), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _CHAR_POOLPET
void SaacClient_ACCharSavePoolPet_send(int acfd, int charaindex, int clifdid,
                                       char *CdKey, char *Pooldataarg) {
  CreateHeader(ws->work, "ACSavePoolPet");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

void SaacClient_ACCharGetPoolPet_send(int acfd, int meindex, int charaindex,
                                      int clifdid, char *CdKey) {
  CreateHeader(ws->work, "ACGetPoolPet");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(meindex), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

void SaacClient_ACCharInsertPoolPet_send(int acfd, int charaindex, int clifdid,
                                         char *CdKey, char *Pooldataarg,
                                         int TYPE) {
  CreateHeader(ws->work, "ACInsertPoolPet");
  strncatsafe(ws->work, mkstr_string(CdKey), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(Pooldataarg), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(clifdid), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(TYPE), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _ANGEL_SUMMON
void SaacClient_ACMissionTable_send(int acfd, int num, int type, char *data,
                                    char *angel_info) {
  CreateHeader(ws->work, "ACMissionTable");
  strncatsafe(ws->work, mkstr_int(num), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(type), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(angel_info), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _TEACHER_SYSTEM
void SaacClient_ACCheckCharacterOnLine_send(int acfd, int charaindex, char *id,
                                            char *name, int flag) {
  CreateHeader(ws->work, "ACCheckCharacterOnLine");
  strncatsafe(ws->work, mkstr_int(charaindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(name), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

void SaacClient_ACCharLogin_send(int fd, int clifd, char *id, char *pas,
                                 char *ip
#ifdef _NEWCLISETMAC
                                 ,
                                 char *mac
#endif
) {
  CreateHeader(ws->work, "ACCharLogin");
  strncatsafe(ws->work, mkstr_int(clifd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(pas), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(ip), ws->work_buf_size);
#ifdef _NEWCLISETMAC
  strncatsafe(ws->work, mkstr_string(mac), ws->work_buf_size);
#endif
  Send(ws, acfd, ws->work);
}

void SaacClient_LockLogin_send(int fd, char *id, char *ip, int flag) {
  CreateHeader(ws->work, "LockLogin");
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(ip), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  Send(ws, fd, ws->work);
}

#ifdef _NEW_VIP_SHOP
void SaacClient_QueryPoint_send(int acfd, int fd, char *id) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "QueryPoint");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

void SaacClient_NewVipShop_send(int acfd, int fd, char *id, int point,
                                char *buf, int flag) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "NewVipShop");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(buf), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(flag), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}

#ifdef _COST_ITEM
void SaacClient_CostItem_send(int acfd, int fd, char *id, int point) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "CostItem");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif
#endif

#ifdef _ITEM_PET_LOCKED
void SaacClient_ItemPetLocked_send(int fd, char *id, char *safepasswd) {
  CreateHeader(ws->work, "ItemPetLocked");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(safepasswd), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
void SaacClient_ItemPetLockedPasswd_send(int fd, char *id, char *safepasswd) {
  CreateHeader(ws->work, "ItemPetLockedPasswd");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(safepasswd), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _ONLINE_COST
void SaacClient_OnlineCost_send(int acfd, int fd, char *id, char *costpasswd,
                                int fmindex, char *fmname) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "OnlineCost");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(costpasswd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(fmindex), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(fmname), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _SQL_BUY_FUNC
void SaacClient_OnlineBuy_send(int acfd, int fd, char *id, char *costpasswd) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "OnlineBuy");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(costpasswd), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void SaacClient_OldToNew_send(int acfd, int fd, char *id, int point) {
  if (acfd == -1)
    return;
  CreateHeader(ws->work, "OldToNew");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void SaacClient_FormulateAutoPk_send(int fd, char *id, int point) {
  CreateHeader(ws->work, "FormulateAutoPk");
  strncatsafe(ws->work, mkstr_int(fd), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(id), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_int(point), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif

#ifdef _OTHER_SAAC_LINK
void SaacClient_OtherSaacLink_send(int acfd, char *filename, char *data) {
  CreateHeader(ws->work, "OtherSaacLink");
  strncatsafe(ws->work, mkstr_string(filename), ws->work_buf_size);
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif
#ifdef _LOTTERY_SYSTEM
void SaacClient_LotterySystem_send() {
  CreateHeader(ws->work, "LotterySystem");
  Send(ws, acfd, ws->work);
}
#endif
#ifdef _ALL_SERV_SEND
void SaacClient_AllServSend_send(char *data) {
  CreateHeader(ws->work, "AllServSend");
  strncatsafe(ws->work, mkstr_string(data), ws->work_buf_size);
  Send(ws, acfd, ws->work);
}
#endif
