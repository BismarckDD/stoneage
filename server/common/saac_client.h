#ifndef __SAAC_CLIENT_H__
#define __SAAC_CLIENT_H__
//
#include "common.h"
#include "version.h"
//
#include "server_util.h"

int SaacClient_InitClient(int (*write_func)(int, char *, int),
                          int work_buffer_size, int fd);

/* Compatibility with older generated call sites. */
#define SaacClient__ACAddFM_send SaacClient_ACAddFM_send
#define SaacClient__ACDelFM_send SaacClient_ACDelFM_send
#define SaacClient__ACFixFMData_send SaacClient_ACFixFMData_send
#define SaacClient__ACFMDetail_send SaacClient_ACFMDetail_send
#define SaacClient__ACJoinFM_send SaacClient_ACJoinFM_send
#define SaacClient__ACLeaveFM_send SaacClient_ACLeaveFM_send
#define SaacClient__ACMemberJoinFM_send SaacClient_ACMemberJoinFM_send
#define SaacClient__ACMemberLeaveFM_send SaacClient_ACMemberLeaveFM_send
#define SaacClient__ACSetFMPoint_send SaacClient_ACSetFMPoint_send
#define SaacClient__ACShowFMList_send SaacClient_ACShowFMList_send
#define SaacClient__ACShowMemberList_send SaacClient_ACShowMemberList_send
#define SaacClient__ACShowTopFMList_send SaacClient_ACShowTopFMList_send
#define SaacClient__QueryPoint_send SaacClient_QueryPoint_send

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
void SaacClient_UpdataStele_send(int fd, char *cdkey, char *name, char *title,
                                 int level, int trns, int time, int floor);
void SaacClient_UpdataStele_recv(int fd, char *token);
void SaacClient_S_UpdataStele_recv(int i, char *ocdkey, char *oname,
                                   char *ncdkey, char *nname, char *title,
                                   int level, int trns, int floor);
#endif

void SaacClient_ACGmsvDownRequest_recv(int fd, int min);
#if _ATTESTAION_ID == 1
void SaacClient_ACServerLogin_send(int fd, int id, char *servername,
                                   char *serverpas);
#else
void SaacClient_ACServerLogin_send(int fd, char *servername, char *serverpas);
#endif
void SaacClient_ACServerLogin_recv(int fd, char *result, char *data);
void SaacClient_ACServerLogout_send(int fd);

#ifdef _PKSEVER_VER
void SaacClient_ACCharList_send(int fd, char *id, char *pas, int mesgid,
                                int star);
#else
void SaacClient_ACCharList_send(int fd, char *id, char *pas, char *ip,
                                char *mac, int mesgid, int charlistflg);
#endif

void SaacClient_ACCharList_recv(int fd, char *result, char *output, int id);
void SaacClient_ACCharLoad_send(int fd, char *id, char *pas, char *char_name,
                                int lock, char *opt, const int msg_id);
void SaacClient_ACCharLoad_recv(int fd, char *result, char *data, const int id,
                                const int save_index);
#ifdef _NEWSAVE
void SaacClient_ACCharSave_send(int fd, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int mesgid,
                                int saveindex);
#else
void SaacClient_ACCharSave_send(int fd, char *id, char *charname, char *opt,
                                char *charinfo, int unlock, int mesgid);
#endif
void SaacClient_ACCharSave_recv(int fd, char *result, char *data, int id);
void SaacClient_ACCharDelete_send(int fd, char *id, char *passwd,
                                  char *charname, char *option, int mesgid);
void SaacClient_ACCharDelete_recv(int fd, char *result, char *data, int id);
void SaacClient_ACLock_send(int fd, char *id, int lock, int mesgid);
void SaacClient_ACLock_recv(int fd, char *result, char *data, int id);
void SaacClient_ACUCheck_send(int fd, char *mem_id, int status);
void SaacClient_ACUCheck_recv(int fd, char *mem_id);
void SaacClient_DBUpdateEntryString_send(int fd, char *table, char *key,
                                         char *value, int msgid, int msgid2);
void SaacClient_DBUpdateEntryString_recv(int fd, char *result, char *table,
                                         char *key, int msgid, int msgid2);
void SaacClient_DBDeleteEntryString_send(int fd, char *table, char *key,
                                         int msgid, int msgid2);
void SaacClient_DBDeleteEntryString_recv(int fd, char *result, char *table,
                                         char *key, int msgid, int msgid2);
void SaacClient_DBGetEntryString_send(int fd, char *table, char *key, int msgid,
                                      int msgid2);
void SaacClient_DBGetEntryString_recv(int fd, char *result, char *value,
                                      char *table, char *key, int msgid,
                                      int msgid2);
void SaacClient_DBUpdateEntryInt_send(int fd, char *table, char *key, int value,
                                      char *info, int msgid, int msgid2);
void SaacClient_DBUpdateEntryInt_recv(int fd, char *result, char *table,
                                      char *key, int msgid, int msgid2);
void SaacClient_DBGetEntryRank_send(int fd, char *table, char *key, int msgid,
                                    int msgid2);
void SaacClient_DBGetEntryRank_recv(int fd, char *result, int rank, int count,
                                    char *table, char *key, int msgid,
                                    int msgid2);
void SaacClient_DBDeleteEntryInt_send(int fd, char *table, char *key, int msgid,
                                      int msgid2);
void SaacClient_DBDeleteEntryInt_recv(int fd, char *result, char *table,
                                      char *key, int msgid, int msgid2);
void SaacClient_DBGetEntryInt_send(int fd, char *table, char *key, int msgid,
                                   int msgid2);
void SaacClient_DBGetEntryInt_recv(int fd, char *result, int value, char *table,
                                   char *key, int msgid, int msgid2);
void SaacClient_DBGetEntryByRank_send(int fd, char *table, int rank_start,
                                      int rank_end, int msgid, int msgid2);
void SaacClient_DBGetEntryByRank_recv(int fd, char *result, char *list,
                                      char *table, int msgid, int msgid2);
void SaacClient_DBGetEntryByCount_send(int fd, char *table, int count_start,
                                       int num, int msgid, int msgid2);
void SaacClient_DBGetEntryByCount_recv(int fd, char *result, char *list,
                                       char *table, int count_start, int msgid,
                                       int msgid2);
void SaacClient_Broadcast_send(int fd, const char *id, const char *char_name,
                               const char *message, int flag);
void SaacClient_Broadcast_recv(int fd, const char *id, const char *char_name,
                               const char *message);
void SaacClient_Message_send(int fd, char *id_from, char *charname_from,
                             char *id_to, char *charname_to, char *message,
                             int option);
void SaacClient_Message_recv(const int fd, const char *id_from,
                             const char *char_name_from, const char *id_to,
                             const char *char_name_to, const char *message,
                             const int option, const int mesgid);
void SaacClient_MessageAck_send(int fd, char *id, char *charname, char *result,
                                int mesgid);
void SaacClient_MessageFlush_send(int fd, char *id, char *charname);
int SaacClient_ClientDispatchMessage(int fd, char *line);

// 成立家族
#ifdef _PERSONAL_FAME
void SaacClient_ACAddFM_send(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petarrt, char *fmrule, int fmsprite,
                             int fmleadergrano, int fame,
#ifdef _FAMILYBADGE_
                             int fmbadge,
#endif
                             int charfdid);
#else
void SaacClient_ACAddFM_send(int fd, char *fmname, char *fmleadername,
                             char *fmleaderid, int fmleaderlv, char *petname,
                             char *petarrt, char *fmrule, int fmsprite,
                             int fmleadergrano, int charfdid);
#endif
void SaacClient_ACAddFM_recv(int fd, char *result, int fmindex, int index,
                             int charfdid);
// 加入家族
// #ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
void SaacClient_ACJoinFM_send(int fd, char *fmname, int fmindex, char *charname,
                              char *charid, int charlv, int index, int fame,
                              int charfdid);
// #else
// void SaacClient_ACJoinFM_send(int fd, char *fmname, int fmindex, char
// *charname,
//        char *charid, int charlv, int index, int charfdid);
// #endif
void SaacClient_ACJoinFM_recv(int fd, char *result, int recv, int charfdid);
// 退出家族
void SaacClient_ACLeaveFM_send(int fd, char *fmname, int fmindex,
                               char *charname, char *charid, int index,
                               int charfdid);
void SaacClient_ACLeaveFM_recv(int fd, char *result, int resultflag,
                               int charfdid);
// 解散家族
void SaacClient_ACDelFM_send(int fd, char *fmname, int fmindex, int index,
                             char *charname, char *charid, int charfdid);

void SaacClient_ACDelFM_recv(int fd, char *result, int charfdid);
// 列出家族列表
void SaacClient_ACShowFMList_send(int fd);
void SaacClient_ACShowFMList_recv(int fd, char *result, int num, char *data);
// 列出成员列表
void SaacClient_ACShowMemberList_send(int fd, int index);
void SaacClient_ACShowMemberList_recv(int fd, char *result, int index, int num,
                                      int fmacceptflag, int fmjoinnum,
                                      char *data
#ifdef _FAMILYBADGE_
                                      ,
                                      int badge
#endif
);
// 列出家族详细资料
void SaacClient_ACFMDetail_send(int fd, char *fmname, int fmindex, int index,
                                int charfdid);
void SaacClient_ACFMDetail_recv(int fd, char *result, char *data, int charfdid);
// 族长审核成员加入家族、修改家族成员职位
#ifdef _FMVER21
void SaacClient_ACMemberJoinFM_send(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int meindex, int charfdid);
#else
void SaacClient_ACMemberJoinFM_send(int fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int result, int charfdid);
#endif
void SaacClient_ACMemberJoinFM_recv(int fd, char *result, char *data,
                                    int charfdid);
// 族长审核成员离开家族
#ifdef _FMVER21
void SaacClient_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int meindex, int charfdid);
#else
void SaacClient_ACMemberLeaveFM_send(int fd, char *fmname, int fmindex,
                                     char *charname, int charindex, int index,
                                     int charfdid);
#endif
void SaacClient_ACMemberLeaveFM_recv(int fd, char *result, char *data,
                                     int charfdid);
// 玩家 Login
#ifdef _FM_MODIFY
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int eventflag, int charfdid, int gsnum);
#else
#ifdef _FMVER21
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int result,
                                   int eventflag, int charfdid);
#else
void SaacClient_ACFMCharLogin_send(int fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int result,
                                   int charfdid);
#endif
#endif
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
void SaacClient_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
                                   int fmdp, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfame,
                                   int charfdid
#ifdef _NEW_MANOR_LAW
                                   ,
                                   int momentum
#endif
);
#else
void SaacClient_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
                                   int fmdp, int joinflag, int fmsetupflag,
                                   int flag, int charindex, int charfdid);
#endif
// 玩家 Logout
void SaacClient_ACFMCharLogout_send(int fd, char *fmname, int fmindex,
                                    char *charname, char *charid, int result,
                                    int index, int charfdid);
void SaacClient_ACFMCharLogout_recv(int fd, char *result, int charfdid);
// 列出家族留言板
void SaacClient_ACFMReadMemo_send(int fd, int index);
void SaacClient_ACFMReadMemo_recv(int fd, char *result, int index, int num,
                                  int dataindex, char *data);
// 写入家族留言板
void SaacClient_ACFMWriteMemo_send(int fd, char *fmname, int fmindex,
                                   char *data, int index);
void SaacClient_ACFMWriteMemo_recv(int fd, char *result, int index);
// 列出家族据点
void SaacClient_ACFMPointList_send(int fd);
void SaacClient_ACFMPointList_recv(int fd, char *result, char *data);

// 申请家族据点
void SaacClient_ACSetFMPoint_send(int fd, char *fmname, int fmindex, int index,
                                  int fmpointindex, int fl, int x, int y,
                                  int charfdid);
void SaacClient_ACSetFMPoint_recv(int fd, char *result, int r, int charfdid);
// 设定家族据点
void SaacClient_ACFixFMPoint_send(int fd, char *winfmname, int winfmindex,
                                  int winindex, char *losefmname,
                                  int losefmindex, int loseindex, int village);
void SaacClient_ACFixFMPoint_recv(int fd, char *result, int r);
// 族长对星系家族成员广播
// kindflag 1:族长广播 2:家族被系统删除 3:系统通知讯息
void SaacClient_ACFMAnnounce_send(int fd, char *fmname, int fmindex, int index,
                                  char *data, int color);
void SaacClient_ACFMAnnounce_recv(int fd, char *result, char *fmname,
                                  int fmindex, int index, int kindflag,
                                  char *data, int color);
// 列出 TOP 50 家族列表
void SaacClient_ACShowTopFMList_send(int fd, int kindflag);
void SaacClient_ACShowTopFMList_recv(int fd, char *result, int kindflag,
                                     int num, char *data);
// 修正家族资料(是否招收成员)
// flag 1:acceptjoinflag 2:dpchanged 3:change fmpet 4:change fminfo 5:predel FM
// Time
void SaacClient_ACFixFMData_send(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, char *data1, char *data2,
                                 int charindex, int charfdid);

void SaacClient_ACFixFMData_recv(int fd, char *result, int kindflag,
                                 char *data1, char *data2, int charfdid);

// ＰＫ後更新家族声望
void SaacClient_ACFixFMPK_send(int fd, char *winfmname, int winfmindex,
                               int winindex, char *losefmname, int losefmindex,
                               int loseindex);
void SaacClient_ACFixFMPK_recv(int fd, char *result, int data, int winindex,
                               int loseindex);
// 取得家族资料
// kindflag 1:家族银行
void SaacClient_ACGetFMData_send(int fd, char *fmname, int fmindex, int index,
                                 int kindflag, int charfdid);
void SaacClient_ACGetFMData_recv(int fd, char *result, int kindflag, int data,
                                 int charfdid);
// ＧＭ修正家族资料
void SaacClient_ACGMFixFMData_send(int fd, int index, char *charid, char *cmd,
                                   char *data, int charfdid);
void SaacClient_ACGMFixFMData_recv(int fd, char *result, char *fmindex,
                                   int charfdid);
// 删除家族後清除对战排程
void SaacClient_ACFMClearPK_recv(int fd, char *result, char *fmname,
                                 int fmindex, int index);

// Arminius 7.26 manor pk acknowledge
void SaacClient_ACManorPKAck_send(int fd, char *data);
void SaacClient_ACManorPKAck_recv(int fd, char *data);

void SaacClient_ACreLoadFmData_send(int fd, int type, int data);

#ifdef _ACFMPK_LIST
void SaacClient_ACLoadFmPk_recv(int fd, char *data);
void SaacClient_ACLoadFmPk_send(int fd, int fmpks_pos);
void SaacClient_ACSendFmPk_recv(int fd, int toindex, int flg);
void SaacClient_ACSendFmPk_send(int fd, int toindex, int PkFlg, int fmpks_pos,
                                char *msg);
#endif

void SaacClient_ACAuctionSold_send(int fd, char *data);
void SaacClient_ACAuctionSold_recv(int fd, char *data);

#ifdef _WAEI_KICK // 跨星球踢人
void SaacClient_ACKick_send(int fd, char *kickid, int kickfd, int flg);
void SaacClient_ACKick_recv(int fd, int act, char *data, int retfd);
#endif

#ifdef _CHAR_POOLITEM
void SaacClient_ACCharInsertPoolItem_send(int acfd, int charaindex, int clifdid,
                                          char *CdKey, char *Pooldataarg,
                                          int TYPE);

void SaacClient_ACCharSavePoolItem_recv(int fd, char *result, char *data,
                                        int retfd);
void SaacClient_ACCharSavePoolItem_send(int acfd, int charaindex, int clifdid,
                                        char *CdKey, char *Pooldataarg);

void SaacClient_ACCharGetPoolItem_recv(int fd, char *result, char *data,
                                       int retfd, int meindex);
void SaacClient_ACCharGetPoolItem_send(int acfd, int meindex, int charaindex,
                                       int clifdid, char *CdKey);
#endif

#ifdef _CHAR_POOLPET
void SaacClient_ACCharInsertPoolPet_send(int acfd, int charaindex, int clifdid,
                                         char *CdKey, char *Pooldataarg,
                                         int TYPE);

void SaacClient_ACCharSavePoolPet_recv(int fd, char *result, char *data,
                                       int retfd);
void SaacClient_ACCharSavePoolPet_send(int acfd, int charaindex, int clifdid,
                                       char *CdKey, char *Pooldataarg);

void SaacClient_ACCharGetPoolPet_recv(int fd, char *result, char *data,
                                      int retfd, int meindex);
void SaacClient_ACCharGetPoolPet_send(int acfd, int meindex, int charaindex,
                                      int clifdid, char *CdKey);
#endif

#ifdef _ANGEL_SUMMON
void SaacClient_ACMissionTable_recv(int fd, int num, int type, char *data,
                                    char *angelinfo);
void SaacClient_ACMissionTable_send(int fd, int num, int type, char *data,
                                    char *angelinfo);
#endif

#ifdef _TEACHER_SYSTEM
// RETURN_FUNCTION -> R_F_XXXXXXX
enum { R_F_TEACHER_SYSTEM, R_F_END };
void SaacClient_ACCheckCharacterOnLine_recv(int acfd, int charaindex,
                                            int iOnline, char *data, int flag);
void SaacClient_ACCheckCharacterOnLine_send(int acfd, int charaindex, char *id,
                                            char *name, int flag);
#endif

void SaacClient_ACCharLogin_send(int fd, int clifd, char *id, char *pas,
                                 char *ip
#ifdef _NEWCLISETMAC
                                 ,
                                 char *mac
#endif
);
void SaacClient_ACCharLogin_recv(int fd, int clifd, int flag);

void SaacClient_LockLogin_send(int fd, char *id, char *ip, int flag);

#ifdef _NEW_VIP_SHOP
void SaacClient_QueryPoint_send(int acfd, int fd, char *id);
void SaacClient_QueryPoint_recv(int fd, int point);
#ifdef _COST_ITEM
void SaacClient_CostItem_send(int acfd, int fd, char *id, int point);
#endif

void SaacClient_NewVipShop_send(int acfd, int fd, char *id, int point,
                                char *buf, int flag);
void SaacClient_NewVipShop_recv(int fd, int point, char *buf, int flag);
#endif

#ifdef _ITEM_PET_LOCKED
void SaacClient_ItemPetLocked_send(int fd, char *id, char *safepasswd);
void SaacClient_ItemPetLocked_recv(int fd, int flag, char *data);
void SaacClient_ItemPetLockedPasswd_send(int fd, char *id, char *safepasswd);
void SaacClient_ItemPetLockedPasswd_recv(int fd, char *data);
#endif

#ifdef _ONLINE_COST
void SaacClient_OnlineCost_send(int acfd, int fd, char *id, char *costpasswd,
                                int fmindex, char *fmname);
void SaacClient_OnlineCost_recv(int fd, char *data);
#endif
#ifdef _SQL_BUY_FUNC
void SaacClient_OnlineBuy_send(int acfd, int fd, char *id, char *costpasswd);
void SaacClient_OnlineBuy_recv(int fd, char *data);
#endif
#ifdef _VIPPOINT_OLD_TO_NEW
void SaacClient_OldToNew_send(int acfd, int fd, char *id, int point);
void SaacClient_OldToNew_recv(int fd, char *data);
#endif
#ifdef _LOTTERY_SYSTEM
void SaacClient_LotterySystem_send();
void SaacClient_LotterySystem_recv(char *data);
#endif
#ifdef _ALL_SERV_SEND
void SaacClient_AllServSend_recv(char *data);
void SaacClient_AllServSend_send(char *data);
#endif
#ifdef _OTHER_SAAC_LINK
void SaacClient_OtherSaacLink_send(int acfd, char *filename, char *data);
#endif

void SaacClient_ACServerKey_recv(int acfd, int key);

#endif // __SAAC_CLIENT_H__
