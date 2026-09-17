#ifndef __GMSV_SERVER_H__
#define __GMSV_SERVER_H__
//
#include "common.h"
#include "version.h"
//
#include "server_util.h"

#define SEPARATOR ";"

#define LSSPROTO_W_RECV 0
#define LSSPROTO_W2_RECV 1
#define LSSPROTO_XYD_SEND 2
#define LSSPROTO_EV_RECV 3
#define LSSPROTO_EV_SEND 4
#define LSSPROTO_EN_RECV 5
#define LSSPROTO_DU_RECV 6
#define LSSPROTO_EN_SEND 7
#define LSSPROTO_EO_RECV 8
#define LSSPROTO_BU_RECV 9
#define LSSPROTO_JB_RECV 10
#define LSSPROTO_LB_RECV 11
#define LSSPROTO_RS_SEND 12
#define LSSPROTO_RD_SEND 13
#define LSSPROTO_B_RECV 14
#define LSSPROTO_B_SEND 15
#define LSSPROTO_SKD_RECV 16
#define LSSPROTO_ID_RECV 17
#define LSSPROTO_PI_RECV 18
#define LSSPROTO_DI_RECV 19
#define LSSPROTO_DG_RECV 20
#define LSSPROTO_DP_RECV 21
#define LSSPROTO_I_SEND 22
#define LSSPROTO_MI_RECV 23
#define LSSPROTO_SI_SEND 24
#define LSSPROTO_MSG_RECV 25
#define LSSPROTO_MSG_SEND 26
#define LSSPROTO_PMSG_RECV 27
#define LSSPROTO_PME_SEND 28
#define LSSPROTO_AB_RECV 29
#define LSSPROTO_AB_SEND 30
#define LSSPROTO_ABI_SEND 31
#define LSSPROTO_DAB_RECV 32
#define LSSPROTO_AAB_RECV 33
#define LSSPROTO_L_RECV 34
#define LSSPROTO_TK_RECV 35
#define LSSPROTO_TK_SEND 36
#define LSSPROTO_MC_SEND 37
#define LSSPROTO_M_RECV 38
#define LSSPROTO_M_SEND 39
#define LSSPROTO_C_RECV 40
#define LSSPROTO_C_SEND 41
#define LSSPROTO_CA_SEND 42
#define LSSPROTO_CD_SEND 43
#define LSSPROTO_R_SEND 44
#define LSSPROTO_S_RECV 45
#define LSSPROTO_S_SEND 46
#define LSSPROTO_D_SEND 47
#define LSSPROTO_FS_RECV 48
#define LSSPROTO_FS_SEND 49
#define LSSPROTO_HL_RECV 50
#define LSSPROTO_HL_SEND 51
#define LSSPROTO_PR_RECV 52
#define LSSPROTO_PR_SEND 53
#define LSSPROTO_KS_RECV 54
#define LSSPROTO_KS_SEND 55
#define LSSPROTO_AC_RECV 56
#define LSSPROTO_MU_RECV 57
#define LSSPROTO_PS_RECV 58
#define LSSPROTO_PS_SEND 59
#define LSSPROTO_ST_RECV 60
#define LSSPROTO_DT_RECV 61
#define LSSPROTO_FT_RECV 62
#define LSSPROTO_SKUP_SEND 63
#define LSSPROTO_SKUP_RECV 64
#define LSSPROTO_KN_RECV 65
#define LSSPROTO_WN_SEND 66
#define LSSPROTO_WN_RECV 67
#define LSSPROTO_EF_SEND 68
#define LSSPROTO_SE_SEND 69
#define LSSPROTO_SP_RECV 70
#define LSSPROTO_CLIENTLOGIN_RECV 71
#define LSSPROTO_CLIENTLOGIN_SEND 72
#define LSSPROTO_CREATENEWCHAR_RECV 73
#define LSSPROTO_CREATENEWCHAR_SEND 74
#define LSSPROTO_CHARDELETE_RECV 75
#define LSSPROTO_CHARDELETE_SEND 76
#define LSSPROTO_CHARLOGIN_RECV 77
#define LSSPROTO_CHARLOGIN_SEND 78
#define LSSPROTO_CHARLIST_RECV 79
#define LSSPROTO_CHARLIST_SEND 80
#define LSSPROTO_CHARLOGOUT_RECV 81
#define LSSPROTO_CHARLOGOUT_SEND 82
#define LSSPROTO_PROCGET_RECV 83
#define LSSPROTO_PROCGET_SEND 84
#define LSSPROTO_PLAYERNUMGET_RECV 85
#define LSSPROTO_PLAYERNUMGET_SEND 86
#define LSSPROTO_ECHO_RECV 87
#define LSSPROTO_ECHO_SEND 88
#define LSSPROTO_SHUTDOWN_RECV 89
#define LSSPROTO_NU_SEND 90
#define LSSPROTO_TD_RECV 91
#define LSSPROTO_TD_SEND 92
#define LSSPROTO_FM_SEND 93
#define LSSPROTO_FM_RECV 94
#define LSSPROTO_WO_SEND 95
#define LSSPROTO_PETST_RECV 96

#ifdef _MIND_ICON
#define LSSPROTO_MA_RECV 98
#endif

#ifdef _ITEM_CRACKER
#define LSSPROTO_IC_SEND 100
#endif

#ifdef _MAGIC_NOCAST // 精灵:沉默
#define LSSPROTO_NC_SEND 101
#endif

#ifdef _TEAM_KICKPARTY
#define LSSPROTO_KTEAM_RECV 106
#endif
#ifdef _PETS_SELECTCON
#define LSSPROTO_PETST_SEND 107
#endif
#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
#define LSSPROTO_RESIST_RECV 108
#define LSSPROTO_RESIST_SEND 109
#endif
#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
#define LSSPROTO_BATTLESKILL_RECV 110
#define LSSPROTO_BATTLESKILL_SEND 111
#endif
#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
#define LSSPROTO_CHATROOM_RECV 112
#define LSSPROTO_CHATROOM_SEND 113
#endif

#define LSSPROTO_SPET_RECV 114 // Robin 待机宠
#define LSSPROTO_SPET_SEND 115

#ifdef _STREET_VENDOR
#define LSSPROTO_STREET_VENDOR_RECV 116 // 摆摊功能
#define LSSPROTO_STREET_VENDOR_SEND 117
#endif

#ifdef _RIGHTCLICK
#define LSSPROTO_RCLICK_RECV 118
#define LSSPROTO_RCLICK_SEND 119
#endif

#ifdef _JOBDAILY
#define LSSPROTO_JOBDAILY_SEND 120 // CYG　任务日志功能
#define LSSPROTO_JOBDAILY_RECV 121
#endif

#ifdef _TEACHER_SYSTEM
#define LSSPROTO_TEACHER_SYSTEM_RECV 122 // 导师功能
#define LSSPROTO_TEACHER_SYSTEM_SEND 123
#endif

#ifdef _ADD_STATUS_2
#define LSSPROTO_S2_RECV 124
#define LSSPROTO_S2_SEND 125
#endif

#ifdef _PET_ITEM
#define LSSPROTO_PET_ITEM_RECV 127
#endif

#ifdef _MOVE_SCREEN
#define LSSPROTO_MOVE_SCREEN_SEND 128
#endif
#ifdef _THEATER
#define LSSPROTO_THEATER_DATA_SEND 131
#define LSSPROTO_THEATER_DATA_RECV 132
#endif

#ifdef _NPC_MAGICCARD
#define LSSPROTO_MAGICCARD_ACTION_SEND 133
#define LSSPROTO_MAGICCARD_DAMAGE_SEND 134
#endif

#ifdef _FAMILYBADGE_
#define LSSPROTO_CHAREFFECT_SEND 146
#endif

#ifdef _ASSESS_ABILITY
#define LSSPROTO_ASSESS_ABILITY_RECV 135
#define LSSPROTO_ASSESS_ABILITY_SEND 136
#endif

#ifdef _SECONDARY_WINDOW_DATA_
#define LSSPROTO_SECONDARY_WINDOW_SEND 137
#endif

#ifdef _TRUNTABLE_
#define LSSPROTO_TRUNTABLE_SEND 138
#endif

#ifdef _ONLINE_SHOP
#define LSSPROTO_VIP_SHOP_RECV 140
#define LSSPROTO_VIP_SHOP_SEND 141
#define LSSPROTO_VIP_SHOP_BUY_RECV 143
#define LSSPROTO_VIP_SHOP_AMPOINT_BUY_RECV 145
#endif

#ifdef _CHARSIGNDAY_
#define LSSPROTO_SIGNDAY_RECV 141
#endif

#ifdef _RED_MEMOY_
#define LSSPROTO_REDMEMOY_RECV 147
#define LSSPROTO_REDMEMOY_SEND 148
#endif

#define LSSPROTO_DENGON_SEND 200
#define LSSPROTO_SAMENU_SEND 201
#define LSSPROTO_SAMENU_RECV 202
#define LSSPROTO_UPSHOP_RECV 203 // 更新商城数据

#define LSSPROTO_SHOPOK_SEND 209
#define LSSPROTO_SHOPOK_RECV 208

#ifdef _FAMILYBADGE_
#define LSSPROTO_FAMILYBADGE_RECV 210
#define LSSPROTO_FAMILYBADGE_SEND 211
#endif
#ifdef _NEW_TITLE
#define LSSPROTO_CHARTITLE_RECV 212
#define LSSPROTO_CHARTITLE_SEND 213
#endif

#ifdef _VISUAL_BEATITUDE
#define LSSPROTO_VB_RECV 218
#define LSSPROTO_VB_SEND 219
#endif

#ifdef _RIDEQUERY_
#define LSSPROTO_RIDEQUERY_RECV 220
#endif

void GmsvServer_W_recv(int client_fd, int x, int y, char *direction);
void GmsvServer_UpShopData_send(int client_fd, char *data, char *md5, int id);
void GmsvServer_ShopOK_send(int client_fd);
void GmsvServer_W2_recv(int client_fd, int x, int y, char *direction);
void GmsvServer_XYD_send(int client_fd, int x, int y, int dir);
void GmsvServer_EV_recv(int client_fd, int event, int seqno, int x, int y, int dir);
void GmsvServer_EV_send(int client_fd, int seqno, int result);
void GmsvServer_EN_recv(int client_fd, int x, int y);
void GmsvServer_DU_recv(int client_fd, int x, int y);
void GmsvServer_EN_send(int client_fd, int result, int field);
void GmsvServer_EO_recv(int client_fd, int dummy);
void GmsvServer_BU_recv(int client_fd, int dummy);
void GmsvServer_JB_recv(int client_fd, int x, int y);
void GmsvServer_LB_recv(int client_fd, int x, int y);
void GmsvServer_RS_send(int client_fd, char *data);
void GmsvServer_RD_send(int client_fd, char *data);
void GmsvServer_B_recv(int client_fd, char *command);
void GmsvServer_B_send(int client_fd, char *command);
void GmsvServer_SKD_recv(int client_fd, int dir, int index);
void GmsvServer_ID_recv(int client_fd, int x, int y, int haveitemindex, int toindex);
void GmsvServer_PI_recv(int client_fd, int x, int y, int dir);
void GmsvServer_DI_recv(int client_fd, int x, int y, int itemindex);
void GmsvServer_DG_recv(int client_fd, int x, int y, int amount);
void GmsvServer_DP_recv(int client_fd, int x, int y, int petindex);
void GmsvServer_I_send(int client_fd, char *data);
void GmsvServer_MI_recv(int client_fd, int fromindex, int toindex);
void GmsvServer_SI_send(int client_fd, int fromindex, int toindex);
void GmsvServer_MSG_recv(int client_fd, int index, char *message, int color);
void GmsvServer_MSG_send(int client_fd, int aindex, char *text, int color);
void GmsvServer_PMSG_recv(int client_fd, int index, int petindex, int itemindex,
                          char *message, int color);
void GmsvServer_PME_send(int client_fd, int objindex, int graphicsno, int x, int y,
                         int dir, int flg, int no, char *cdata);
void GmsvServer_AB_recv(int client_fd);
void GmsvServer_AB_send(int client_fd, char *data);
void GmsvServer_ABI_send(int client_fd, int num, char *data);
void GmsvServer_DAB_recv(int client_fd, int index);
void GmsvServer_AAB_recv(int client_fd, int x, int y);
void GmsvServer_L_recv(int client_fd, int dir);
void GmsvServer_TK_recv(int client_fd, int x, int y, char *message, int color,
                        int area);
void GmsvServer_TK_send(int client_fd, int index, char *message, int color);
void GmsvServer_MC_send(int client_fd, int fl, int x1, int y1, int x2, int y2,
                        int tilesum, int objsum, int eventsum, char *data);
void GmsvServer_M_recv(int client_fd, int fl, int x1, int y1, int x2, int y2);
void GmsvServer_M_send(int client_fd, int fl, int x1, int y1, int x2, int y2,
                       char *data);
void GmsvServer_C_recv(int client_fd, int index);
void GmsvServer_C_send(int client_fd, char *data);
void GmsvServer_CA_send(const int client_fd, char *data);
void GmsvServer_CD_send(const int client_fd, char *data);
void GmsvServer_R_send(int client_fd, char *data);
void GmsvServer_S_recv(int client_fd, char *category);
void GmsvServer_S_send(int client_fd, char *data);
void GmsvServer_D_send(int client_fd, int category, int dx, int dy, char *data);
void GmsvServer_FS_recv(int client_fd, int flg);
void GmsvServer_FS_send(int client_fd, int flg);
void GmsvServer_HL_recv(int client_fd, int flg);
void GmsvServer_HL_send(int client_fd, int flg);
void GmsvServer_PR_recv(int client_fd, int x, int y, int request);
void GmsvServer_PR_send(int client_fd, int request, int result);
void GmsvServer_KS_recv(int client_fd, int petarray);
void GmsvServer_KS_send(int client_fd, int petarray, int result);

#ifdef _STANDBYPET
void GmsvServer_SPET_recv(int client_fd, int standbypet);
void GmsvServer_SPET_send(int client_fd, int standbypet, int result);
#endif

void GmsvServer_AC_recv(int client_fd, int x, int y, int actionno);
void GmsvServer_MU_recv(int client_fd, int x, int y, int array, int toindex);
void GmsvServer_PS_recv(int client_fd, int havepetindex, int havepetskill, int toindex,
                        char *data);
void GmsvServer_SPET_send(int client_fd, int standbypet, int result);
void GmsvServer_PS_send(int client_fd, int result, int havepetindex, int havepetskill,
                        int toindex);
void GmsvServer_ST_recv(int client_fd, int titleindex);
void GmsvServer_DT_recv(int client_fd, int titleindex);
void GmsvServer_FT_recv(int client_fd, char *data);
void GmsvServer_SKUP_send(int client_fd, int point);
void GmsvServer_SKUP_recv(int client_fd, int skillid);
void GmsvServer_KN_recv(int client_fd, int havepetindex, char *data);
void GmsvServer_WN_send(int client_fd, int windowtype, int buttontype, int seqno,
                        int objindex, char *data);
void GmsvServer_WN_recv(int client_fd, int x, int y, int seqno, int objindex,
                        int select, char *data);
void GmsvServer_EF_send(int client_fd, int effect, int level, char *option);
void GmsvServer_SE_send(int client_fd, int x, int y, int senumber, int sw);
void GmsvServer_SP_recv(int client_fd, int x, int y, int dir);
void GmsvServer_ClientLogin_recv(int client_fd, char *cdkey, char *passwd, char *mac,
                                 int servid, char *Newip);
void GmsvServer_ClientLogin_send(int client_fd, char *result);
void GmsvServer_CreateNewChar_recv(int client_fd, int dataplacenum, char *charname,
                                   int imgno, int faceimgno, int vital, int str,
                                   int tgh, int dex, int earth, int water,
                                   int fire, int wind, int hometown);
void GmsvServer_CreateNewChar_send(int client_fd, char *result, char *data);
void GmsvServer_CharDelete_recv(int client_fd, char *charname, char *a);
void GmsvServer_CharDelete_send(int client_fd, char *result, char *data);
void GmsvServer_CharLogin_recv(int client_fd, char *charname);
void GmsvServer_CharLogin_send(int client_fd, char *result, char *data);

#ifdef _PKSEVER_VER
void GmsvServer_CharList_recv(int client_fd, int star);
#else
void GmsvServer_CharList_recv(int client_fd);
#endif

void GmsvServer_CharList_send(int client_fd, char *result, char *data);
void GmsvServer_CharLogout_recv(int client_fd, int flg);
void GmsvServer_CharLogout_send(int client_fd, char *result, char *data);
void GmsvServer_ProcGet_recv(int client_fd);
void GmsvServer_ProcGet_send(int client_fd, char *data);
void GmsvServer_PlayerNumGet_recv(int client_fd);
void GmsvServer_PlayerNumGet_send(int client_fd, int logincount, int player);
void GmsvServer_Echo_recv(int client_fd, char *test);
void GmsvServer_Echo_send(int client_fd, char *test);
void GmsvServer_Shutdown_recv(int client_fd, char *passwd, int min);

void GmsvServer_TD_send(int client_fd, char *message);
void GmsvServer_TD_recv(int client_fd, char *message);

#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
void GmsvServer_CHATROOM_recv(int client_fd, char *data);
void GmsvServer_CHATROOM_send(int client_fd, char *message);
#endif

#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
void GmsvServer_RESIST_recv(int client_fd);
void GmsvServer_RESIST_send(int client_fd, char *message);
#endif
#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
void GmsvServer_BATTLESKILL_recv(int client_fd, int iNum);
void GmsvServer_BATTLESKILL_send(int client_fd, char *message);
#endif

void GmsvServer_CHAREFFECT_send(int client_fd, char *message);
void GmsvServer_NU_send(int client_fd, int nu);

void GmsvServer_FM_send(int client_fd, char *message);
void GmsvServer_FM_recv(int client_fd, char *message);

void GmsvServer_WO_send(int client_fd, int effect);
void GmsvServer_PETST_recv(int client_fd, int nPet, int sPet);
void GmsvServer_BM_recv(int client_fd, int iindex);

#ifdef _MIND_ICON
void GmsvServer_MA_recv(int client_fd, int x, int y, int nMind);
#endif

#ifdef _ITEM_CRACKER
void GmsvServer_IC_send(int client_fd, int x, int y);
#endif

#ifdef _ITEM_CRACKER
void GmsvServer_NC_send(int client_fd, int flg);
#endif

#ifdef _TEAM_KICKPARTY
void GmsvServer_KTEAM_recv(int client_fd, int si);
#endif

#ifdef _PETS_SELECTCON
void GmsvServer_PETS_send(int client_fd, int petarray, int result);
// #define LSSPROTO_PETST_SEND 107
#endif

#ifdef _STREET_VENDOR
void GmsvServer_STREET_VENDOR_recv(int client_fd, char *message);
void GmsvServer_STREET_VENDOR_send(int client_fd, char *message);
#endif

#ifdef _RIGHTCLICK
void GmsvServer_RCLICK_recv(int client_fd, int type, char *data);
void GmsvServer_RCLICK_send(int client_fd, int type, char *data);
#endif

#ifdef _JOBDAILY
void GmsvServer_JOBDAILY_recv(int client_fd, char *data);
void GmsvServer_JOBDAILY_send(int client_fd, char *data);
#endif

#ifdef _TEACHER_SYSTEM
void GmsvServer_TEACHER_SYSTEM_recv(int client_fd, char *data);
void GmsvServer_TEACHER_SYSTEM_send(int client_fd, char *data);
#endif

#ifdef _ADD_STATUS_2
void GmsvServer_S2_recv(int client_fd, char *data);
void GmsvServer_S2_send(int client_fd, char *data);
#endif

#ifdef _PET_ITEM
void GmsvServer_PETITEM_recv(int client_fd, int x, int y, int petindex, int fromindex,
                             int toindex);
#endif

#ifdef _ASSESS_ABILITY
void GmsvServer_ASSESS_ABILITY_recv(int client_fd);
void GmsvServer_ASSESS_ABILITY_send(int client_fd, char *data);
#endif

#ifdef _ONLINE_SHOP
void GmsvServer_VIP_SHOP_recv(int client_fd, int type, int page);
void GmsvServer_VIP_SHOP_send(int client_fd, int num, int BJ, int type, int shoppage,
                              int page, char *data);
void GmsvServer_VIP_SHOP_buy_recv(int client_fd, int type, int page, int id, int num);
#endif
void GmsvServer_DENGON_send(int client_fd, char *data, int color, int num);

void GmsvServer_SaMenu_recv(int client_fd, int index);
void GmsvServer_SAMENU_send(int client_fd, int index, char *data);

#ifdef _FAMILYBADGE_
void GmsvServer_FamilyBadge_recv(int client_fd);
void GmsvServer_FamilyBadge_send(int client_fd, char *data);
#endif
#ifdef _NEW_TITLE
void GmsvServer_CharTitle_send(int client_fd, char *data); // 发送新数据
#endif
#endif

#ifdef _VISUAL_BEATITUDE
void GmsvServer_VisualBeatitude_send(int client_fd, char *data);
#endif
