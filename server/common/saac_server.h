#ifndef __SAAC_SERVER_H__
#define __SAAC_SERVER_H__

//
#include "version.h"
//
#include "server_util.h"

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void SaacServer_UpdataStele_recv(int gmsv_fd, char *cdkey, char *name, char *title,
                                int level, int trns, int time, int floor);
void SaacServer_UpdataStele_send(int gmsv_fd, char *data);
void SaacServer_S_UpdataStele_send(int gmsv_fd, char *ocdkey, char *oname,
                                  char *ncdkey, char *nname, char *title,
                                  int level, int trns, int floor);
#endif
#if _ATTESTAION_ID == 1
void SaacServer_ACServerLogin_recv(const int gmsv_fd, const int id,
                                  const char *servername, const char *password);
#else
void SaacServer_ACServerLogin_recv(const int gmsv_fd, const char *servername, const char *password);
#endif

void SaacServer_ACServerLogin_send(const int gmsv_fd, const char *result, const char *data);
void SaacServer_ACServerLogout_recv(int gmsv_fd);
void SaacServer_ACCharList_recv(int ti, char *id, char *pas, char *ip, char *mac,
                               int mesgid, int charlistflg);
void SaacServer_ACCharList_send(int gmsv_fd, char *result, char *output, int id);
void SaacServer_ACCharLoad_recv(int gmsv_fd, char *id, char *pas, char *charname,
                               int lock, char *opt, int mesgid);
// CoolFish: 2001/10/16
#ifdef _NewSave
void SaacServer_ACCharLoad_send(int gmsv_fd, char *result, char *data, int id,
                               int charindex);
void SaacServer_ACCharSave_recv(int gmsv_fd, char *id, char *charname, char *opt,
                               char *charinfo, int unlock, int mesgid,
                               int charindex);
#else
void SaacServer_ACCharLoad_send(int gmsv_fd, char *result, char *data, int id);
void SaacServer_ACCharSave_recv(int gmsv_fd, char *id, char *charname, char *opt,
                               char *charinfo, int unlock, int mesgid);
#endif
void SaacServer_ACCharSave_send(int gmsv_fd, char *result, char *data, int id);
void SaacServer_ACCharDelete_recv(int gmsv_fd, char *id, char *passwd, char *charname,
                                 char *option, int mesgid);
void SaacServer_ACCharDelete_send(int gmsv_fd, char *result, char *data, int id);
void SaacServer_ACLock_recv(int gmsv_fd, char *id, int lock, int mesgid);
void SaacServer_ACLock_send(int gmsv_fd, char *result, char *data, int id);
void SaacServer_ACUCheck_recv(int gmsv_fd, char *mem_id, int status);
void SaacServer_ACUCheck_send(int gmsv_fd, char *mem_id);
void SaacServer_DBUpdateEntryString_recv(int gmsv_fd, char *table, char *key,
                                        char *value, int msgid, int msgid2);
void SaacServer_DBUpdateEntryString_send(int gmsv_fd, char *result, char *table,
                                        char *key, int msgid, int msgid2);
void SaacServer_DBDeleteEntryString_recv(int gmsv_fd, char *table, char *key,
                                        int msgid, int msgid2);
void SaacServer_DBDeleteEntryString_send(int gmsv_fd, char *result, char *table,
                                        char *key, int msgid, int msgid2);
void SaacServer_DBGetEntryString_recv(int gmsv_fd, char *table, char *key, int msgid,
                                     int msgid2);
void SaacServer_DBGetEntryString_send(int gmsv_fd, char *result, char *value,
                                     char *table, char *key, int msgid,
                                     int msgid2);
void SaacServer_DBUpdateEntryInt_recv(int gmsv_fd, char *table, char *key, int value,
                                     char *info, int msgid, int msgid2);
void SaacServer_DBUpdateEntryInt_send(int gmsv_fd, char *result, char *table,
                                     char *key, int msgid, int msgid2);
void SaacServer_DBGetEntryRank_recv(int gmsv_fd, char *table, char *key, int msgid,
                                   int msgid2);
void SaacServer_DBGetEntryRank_send(int gmsv_fd, char *result, int rank, int count,
                                   char *table, char *key, int msgid,
                                   int msgid2);
void SaacServer_DBDeleteEntryInt_recv(int gmsv_fd, char *table, char *key, int msgid,
                                     int msgid2);
void SaacServer_DBDeleteEntryInt_send(int gmsv_fd, char *result, char *table,
                                     char *key, int msgid, int msgid2);
void SaacServer_DBGetEntryInt_recv(int gmsv_fd, char *table, char *key, int msgid,
                                  int msgid2);
void SaacServer_DBGetEntryInt_send(int gmsv_fd, char *result, int value, char *table,
                                  char *key, int msgid, int msgid2);
void SaacServer_DBGetEntryByRank_recv(int gmsv_fd, char *table, int rank_start,
                                     int rank_end, int msgid, int msgid2);
void SaacServer_DBGetEntryByRank_send(int gmsv_fd, char *result, char *list,
                                     char *table, int msgid, int msgid2);
void SaacServer_DBGetEntryByCount_recv(int gmsv_fd, char *table, int count_start,
                                      int num, int msgid, int msgid2);
void SaacServer_DBGetEntryByCount_send(int gmsv_fd, char *result, char *list,
                                      char *table, int count_start, int msgid,
                                      int msgid2);
void SaacServer_Broadcast_recv(int gmsv_fd, char *id, char *charname, char *message,
                              int flag);
void SaacServer_Broadcast_send(int gmsv_fd, char *id, char *charname, char *message);
void SaacServer_Message_recv(int gmsv_fd, char *id_from, char *charname_from,
                            char *id_to, char *charname_to, char *message,
                            int option);
void SaacServer_Message_send(int gmsv_fd, char *id_from, char *charname_from,
                            char *id_to, char *charname_to, char *message,
                            int option, int mesgid);
void SaacServer_MessageAck_recv(int gmsv_fd, char *id, char *charname, char *result,
                               int mesgid);
void SaacServer_MessageFlush_recv(int gmsv_fd, char *id, char *charname);
int SaacServer_ServerDispatchMessage(int gmsv_fd, char *encoded, char *debugfun);

// CoolFish: Family 2001/5/9
// 成立家族
#ifdef _PERSONAL_FAME
void SaacServer_ACAddFM_recv(int gmsv_fd, char *fmname, char *fmleadername,
                            char *fmleaderid, int fmleaderlv, char *petname,
                            char *petarrt, char *fmrule, int fmsprite,
                            int fmleadergrano, int fame,
#ifdef _FAMILYBADGE_
                            int fmbadge,
#endif
                            int charfdid);
#else
void SaacServer_ACAddFM_recv(int gmsv_fd, char *fmname, char *fmleadername,
                            char *fmleaderid, int fmleaderlv, char *petname,
                            char *petarrt, char *fmrule, int fmsprite,
                            int fmleadergrano, int charfdid);
#endif
void SaacServer_ACAddFM_send(int gmsv_fd, char *result, int fmindex, int index,
                            int charfdid);
// 加入家族
void SaacServer_ACJoinFM_recv(int gmsv_fd, char *fmname, int fmindex, char *charname,
                             char *charid, int charlv, int index, int fame,
                             int charfdid);
void SaacServer_ACJoinFM_send(int gmsv_fd, char *result, int recv, int charfdid);
// 退出家族
void SaacServer_ACLeaveFM_recv(int gmsv_fd, char *fmname, int fmindex, char *charname,
                              char *charid, int index, int charfdid);
void SaacServer_ACLeaveFM_send(int gmsv_fd, char *result, int resultflag,
                              int charfdid);
// 解散家族
#ifdef _LEADERFUNCHECK
void SaacServer_ACDelFM_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                            char *charname, char *charid, int charfdid);
#else
void SaacServer_ACDelFM_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                            int charfdid);
#endif
void SaacServer_ACDelFM_send(int gmsv_fd, char *result, int charfdid);
// 列出家族列表
void SaacServer_ACShowFMList_recv(int gmsv_fd);
void SaacServer_ACShowFMList_send(int gmsv_fd, char *result, int num, char *data);
#ifdef _FAMILY_TOTEM
// 列出家族图腾
void SaacServer_ACShowFMTotem_recv(int gmsv_fd);
void SaacServer_ACShowFMTotem_send(int gmsv_fd, char *result, int num, char *data);
#endif
// 列出成员列表
void SaacServer_ACShowMemberList_recv(int gmsv_fd, int index);
void SaacServer_ACShowMemberList_send(int gmsv_fd, char *result, int index, int num,
                                     int accept_flag, int fm_join_num, char *data
#ifdef _FAMILYBADGE_
                                     , int badge
#endif
);
// 列出家族详细资料
void SaacServer_ACFMDetail_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                               int charfdid);
void SaacServer_ACFMDetail_send(int gmsv_fd, char *result, char *data, int charfdid);
// 列出家族留言板
void SaacServer_ACFMReadMemo_recv(int gmsv_fd, int index);
void SaacServer_ACFMReadMemo_send(int gmsv_fd, char *result, int index, int num,
                                 int dataindex, char *data);
// 写入家族留言板
void SaacServer_ACFMWriteMemo_recv(int gmsv_fd, char *fmname, int fmindex, char *data,
                                  int index);
void SaacServer_ACFMWriteMemo_send(int gmsv_fd, char *result, int index);
// 玩家login
#ifdef _FM_MODIFY
void SaacServer_ACFMCharLogin_recv(int gmsv_fd, char *fmname, int fmindex,
                                  char *charname, char *charid, int charlv,
                                  int eventflag, int charfdid, int gsnum);
#else
#ifdef _FMVER21
void SaacServer_ACFMCharLogin_recv(int gmsv_fd, char *fmname, int fmindex,
                                  char *charname, char *charid, int charlv,
                                  int eventflag, int charfdid);
#else
void SaacServer_ACFMCharLogin_recv(int gmsv_fd, char *fmname, int fmindex,
                                  char *charname, char *charid, int charlv,
                                  int charfdid);
#endif
#endif
#ifdef _PERSONAL_FAME // Arminius: 家族个人声望
void SaacServer_ACFMCharLogin_send(int gmsv_fd, char *result, int index, int floor,
                                  int fmpopular, int joinflag, int fmsetupflag,
                                  int flag, int charindex, int charfame,
                                  int charfdid
#ifdef _NEW_MANOR_LAW
                                  ,
                                  int momentum
#endif
);
#else
void SaacServer_ACFMCharLogin_send(int gmsv_fd, char *result, int index, int floor,
                                  int fmpopular, int joinflag, int fmsetupflag,
                                  int flag, int charindex, int charfdid);
#endif

// 玩家logout
void SaacServer_ACFMCharLogout_recv(int gmsv_fd, char *fmname, int fmindex,
                                   char *charname, char *charid, int charlv,
                                   int index, int charfdid);
void SaacServer_ACFMCharLogout_send(int gmsv_fd, char *result, int charfdid);
// 族长审核成员加入家族、修改家族成员职位
#ifdef _FMVER21
void SaacServer_ACMemberJoinFM_recv(int gmsv_fd, char *fmname, int fmindex,
                                   char *charname, int charindex, int index,
                                   int result, int meindex, int charfdid);
#else
void SaacServer_ACMemberJoinFM_recv(int gmsv_fd, char *fmname, int fmindex,
                                   char *charname, int charindex, int index,
                                   int result, int charfdid);
#endif
void SaacServer_ACMemberJoinFM_send(int gmsv_fd, char *result, int charfdid);
// 族长审核成员离开家族
#ifdef _FMVER21
void SaacServer_ACMemberLeaveFM_recv(int gmsv_fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int meindex, int charfdid);
#else
void SaacServer_ACMemberLeaveFM_recv(int gmsv_fd, char *fmname, int fmindex,
                                    char *charname, int charindex, int index,
                                    int charfdid);
#endif
void SaacServer_ACMemberLeaveFM_send(int gmsv_fd, char *result, int charfdid);
// 列出家族据点
void SaacServer_ACFMPointList_recv(int gmsv_fd);
void SaacServer_ACFMPointList_send(int gmsv_fd, char *result, char *data);

// 申请家族据点
void SaacServer_ACSetFMPoint_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                                 int fmpointindex, int fl, int x, int y,
                                 int charfdid);
void SaacServer_ACSetFMPoint_send(int gmsv_fd, char *result, int r, int charfdid);
// 设定家族据点
void SaacServer_ACFixFMPoint_recv(int gmsv_fd, char *winfmname, int winfmindex,
                                 int winindex, char *losefmname,
                                 int losefmindex, int loseindex, int village);
void SaacServer_ACFixFMPoint_send(int gmsv_fd, char *result, int r);
// 族长对星系家族成员广播
// kindflag 1:族长广播 2:家族被系统删除 3:系统通知讯息
void SaacServer_ACFMAnnounce_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                                 char *data, int color);
void SaacServer_ACFMAnnounce_send(int gmsv_fd, char *result, char *fmname,
                                 int fmindex, int index, int kindflag,
                                 char *data, int color);
// 列出家族排行榜
void SaacServer_ACShowTopFMList_recv(int gmsv_fd, int kindflag);
void SaacServer_ACShowTopFMList_send(int gmsv_fd, char *result, int kindflag, int num,
                                    char *data);
// 修正家族资料(是否招收成员)
// kindflag 1:是否继续招募家族成员 2:家族ＰＫ声望 3:家族守护兽 4:家族主旨
//          5:解散家族时间 6:家族基金 7:家族冒险声望 8:家族饲育声望
//          9:家族合成、加工声望 10:家族料理声望
void SaacServer_ACFixFMData_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                                int kindflag, char *data1, char *data2,
                                int charindex, int charfdid);
void SaacServer_ACFixFMData_send(int gmsv_fd, char *result, int kindflag, char *data1,
                                char *data2, int charfdid);
// ＰＫ後更新家族声望
void SaacServer_ACFixFMPK_recv(int gmsv_fd, char *winfmname, int winfmindex,
                              int winindex, char *losefmname, int losefmindex,
                              int loseindex);
void SaacServer_ACFixFMPK_send(int gmsv_fd, char *result, int data, int winindex,
                              int loseindex);
// 取得家族资料
// kindflag 1:家族银行
void SaacServer_ACGetFMData_recv(int gmsv_fd, char *fmname, int fmindex, int index,
                                int kindflag, int charfdid);
void SaacServer_ACGetFMData_send(int gmsv_fd, char *result, int kindflag, int data,
                                int charfdid);
// ＧＭ修正家族资料
void SaacServer_ACGMFixFMData_recv(int gmsv_fd, int fmindex, char *charid, char *cmd,
                                  char *data, int charfdid);
void SaacServer_ACGMFixFMData_send(int gmsv_fd, char *result, char *fmname,
                                  int charfdid);
// 清除对战排程
void SaacServer_ACFMClearPK_send(int gmsv_fd, char *result, char *fmname, int fmindex,
                                int index);
// CoolFish end

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
void SaacServer_ACLoadFmPk_send(int gmsv_fd, char *data);
#ifdef _ACFMPK_LIST // ACSendFmPk
void SaacServer_ACSendFmPk_send(int gmsv_fd, int userindex, int flg);
#endif
#endif

// Arminius 8.1 manor pk
void SaacServer_ACManorPKAck_recv(int gmsv_fd, char *data);
void SaacServer_ACManorPKAck_send(int gmsv_fd, char *data);
void SaacServer_ACreLoadFmData_recv(int gmsv_fd, int type, int data);
#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
void SaacServer_ACLoadFmPk_recv(int gmsv_fd, int fmpks_pos);
#ifdef _ACFMPK_LIST
void SaacServer_ACSendFmPk_recv(int gmsv_fd, int fmpks_pos, int userindex, int flg,
                               char *data);
#else
void SaacServer_ACSendFmPk_recv(int gmsv_fd, int fmpks_pos, char *data);
#endif
#endif
#ifdef _WAEI_KICK
void SaacServer_ACKick_recv(int ti, char *id, int lock, int mesgid);
void SaacServer_ACKick_send(int gmsv_fd, int act, char *data, int id);
#endif
#ifdef _SEND_EFFECT // 特效
void SaacServer_SendEffect_send(int gmsv_fd, char *effect);
#endif

#ifdef _CHAR_POOLITEM
void SaacServer_ACCharInsertPoolItem_recv(int gmsv_fd, char *cdkey, int userindex,
                                         int clifdid, char *Pooldataarg);
void SaacServer_ACCharSavePoolItem_recv(int gmsv_fd, char *cdkey, int userindex,
                                       int clifdid, char *Pooldataarg);
void SaacServer_ACCharSavePoolItem_send(int gmsv_fd, char *Acces, char *Pooldataarg,
                                       int clifdid);
void SaacServer_ACCharGetPoolItem_recv(int gmsv_fd, char *cdkey, int userindex,
                                      int clifdid, int npcid);
void SaacServer_ACCharGetPoolItem_send(int gmsv_fd, char *Acces, char *Pooldataarg,
                                      int clifdid, int npcid);
#endif

#ifdef _CHAR_POOLPET
void SaacServer_ACCharInsertPoolPet_recv(int gmsv_fd, char *cdkey, int userindex,
                                        int clifdid, char *Pooldataarg);

void SaacServer_ACCharSavePoolPet_recv(int gmsv_fd, char *cdkey, int userindex,
                                      int clifdid, char *Pooldataarg);
void SaacServer_ACCharSavePoolPet_send(int gmsv_fd, char *Acces, char *Pooldataarg,
                                      int clifdid);
void SaacServer_ACCharGetPoolPet_recv(int gmsv_fd, char *cdkey, int userindex,
                                     int clifdid, int npcid);
void SaacServer_ACCharGetPoolPet_send(int gmsv_fd, char *Acces, char *Pooldataarg,
                                     int clifdid, int npcid);
#endif

#ifdef _ANGEL_SUMMON
void SaacServer_ACMissionTable_recv(int gmsv_fd, int num, int type, char *data,
                                   char *angelinfo);
void SaacServer_ACMissionTable_send(int gmsv_fd, int num, int type, char *data,
                                   char *angelinfo);
#endif
#ifdef _TEACHER_SYSTEM
void SaacServer_ACCheckCharacterOnLine_send(int gmsv_fd, int charaindex, int iOnline,
                                           char *data, int flag);
void SaacServer_ACCheckCharacterOnLine_recv(int gmsv_fd, int charaindex, char *id,
                                           char *name, int flag);
#endif
void SaacServer_ACCharLogin_recv(int gmsv_fd, int client_fdid, char *id, char *pas,
                                 char *ip
#ifdef _NEWCLISETMAC
                                 ,
                                 char *mac
#endif
);
void SaacServer_ACCharLogin_send(int gmsv_fd, int client_fdid, int flag);
#ifdef _SASQL
void SaacServer_LockLogin_recv(int gmsv_fd, char *id, char *ip, int flag);

#ifdef _NEW_VIP_SHOP
void SaacServer_QueryPoint_send(int gmsv_fd, int client_fd, int point);
void SaacServer_QueryPoint_recv(int gmsv_fd, int client_fd, char *id);
void SaacServer_NewVipShop_recv(int gmsv_fd, int client_fd, char *id, int point,
                               char *buf, int flag);
void SaacServer_NewVipShop_send(int gmsv_fd, int client_fd, int point, char *buf,
                               int flag);
#endif
#ifdef _ITEM_PET_LOCKED
void SaacServer_ItemPetLocked_recv(int gmsv_fd, int client_fd, char *id,
                                  char *safepasswd);
void SaacServer_ItemPetLocked_send(int gmsv_fd, int client_fd, int flag, char *data);
void SaacServer_ItemPetLockedPasswd_recv(int gmsv_fd, int client_fd, char *id,
                                        char *safepasswd);
void SaacServer_ItemPetLockedPasswd_send(int gmsv_fd, int client_fd, char *data);
#endif
#ifdef _COST_ITEM
void SaacServer_CostItem_recv(int gmsv_fd, int client_fd, char *id, int point);
#endif
#ifdef _ONLINE_COST
void SaacServer_OnlineCost_recv(int gmsv_fd, int client_fd, char *id,
                                char *safepasswd, int fmindex, char *fmname);
void SaacServer_OnlineCost_send(int gmsv_fd, int client_fd, char *data);
#endif
#ifdef _SQL_BUY_FUNC
void SaacServer_OnlineBuy_recv(int gmsv_fd, int client_fd, char *id,
                               char *safepasswd);
void SaacServer_OnlineBuy_send(int gmsv_fd, int client_fd, char *data);
#endif
#ifdef _VIPPOINT_OLD_TO_NEW
void SaacServer_OldToNew_recv(int gmsv_fd, int client_fd, char *id, int point);
void SaacServer_OldToNew_send(int gmsv_fd, int client_fd, char *data);
#endif
#ifdef _FORMULATE_AUTO_PK
void SaacServer_FormulateAutoPk_recv(int gmsv_fd, int client_fd, char *id,
                                     int point);
void SaacServer_FormulateAutoPk_send(int gmsv_fd, int client_fd, char *data);
#endif
#endif
#ifdef _LOTTERY_SYSTEM
void SaacServer_LotterySystem_recv(int gmsv_fd);
void SaacServer_LotterySystem_send(int gmsv_fd, char *award);
#endif
#ifdef _ALL_SERV_SEND
void SaacServer_AllServSend_recv(char *data);
void SaacServer_AllServSend_send(int gmsv_fd, char *data);
#endif

#endif // __SAAC_SERVER_H__

