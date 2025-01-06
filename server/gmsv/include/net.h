#ifndef __NET_H__
#define __NET_H__

#include "common.h"
#include "linux_platform.h"

#undef EXTERN
#ifdef __NET_C__
#define EXTERN
#else
#define EXTERN extern
#endif

#define LSGENWORKINGBUFFER SLICE_SIZE

typedef enum {
  NULLCONNECT,
  NOTLOGIN,         /*  ����̼��ƻ���ئ��  */
  LOGIN,            /*  ����̼�            */
  WHILELOGIN,       /*  ����̼��ƻ�����    */
  WHILECREATE,      /*  ���Ȼ�����          */
  WHILELOGOUTSAVE,  /*  ���ʧ����������Ƥ      */
  WHILECANNOTLOGIN, /*  ����̼�ƥ��ئ�������ݼ�ʧ����������        */
  WHILECHARDELETE,  /*  ƽ�ҷ´���      */
  WHILEDOWNLOADCHARLIST, /*  ƽ�ҷ�������ĸ���������    */
  WHILECHANGEPASSWD,        /*  �ɵ�������  ��      */
  WHILELOSTCHARSAVE,        /*  ����ƥƽ�ҷ±���Ƥ��      */
  WHILELOSTCHARDELETE,      /*  ����ƥƽ�ҷ�����      */
  WHILECLOSEALLSOCKETSSAVE, /* closeallsockets ƥƽ�ҷ±���Ƥ��  */
  WHILESAVEWAIT, /* ���ʧ����������Ƥ�����󡼰��  ����
                  */
} LoginType;

typedef enum {
  NOTDETECTED,
  AC,
  CLI,
#ifdef _OTHER_SAAC_LINK
  SQL,
#endif
  ADM
} ConnectType;

#define CDKEYLEN 16
#define PASSWDLEN 33
/* ��ľ��  �ʷ��ص�(����ئ�� Char ��STRING64 �����ئ�����Ի���³���*/
#define CHARNAMELEN 32

#define CLITIMEOUT_SEC 120

#define NET_STRING_SUCCESSFULL "successful"
#define NET_STRING_FAILED "failed"

#define RBSIZE (1024 * 64)
#define WBSIZE (1024 * 256)

#define AC_RBSIZE (1024 * 1024 * 6)
extern int AC_WBSIZE;

EXTERN int epfd;
EXTERN int nfds;
EXTERN int bindedfd;
EXTERN int acfd;
EXTERN int svfd;
#ifdef _OTHER_SAAC_LINK
EXTERN int osfd;
#endif
EXTERN int ConnectLen;
#define CONNECT_WINDOWBUFSIZE 7

BOOL initConnect(int size);
void endConnect(void);
#define CONNECT_endOne(sockfd, lin)                                            \
  _CONNECT_endOne(__FILE__, __LINE__, sockfd, lin)
BOOL _CONNECT_endOne(char *file, int fromline, int sockfd, int lin);
BOOL netloop_faster(void);
int lsrpcClientWriteFunc(int fd, char *buf, int size);
char *GetOneLine(int fd);
BOOL initConnectOne(int sockfd, struct sockaddr_in *sin, int len);

#ifdef _MO_LNS_NLSUOXU
void SetTimerLua(void);
BOOL DelTimer_net(int ID);
#endif

/* CA , CD */
void CAcheck(void);
void CAflush(int charaindex);
void CAsend(int fd);
void CDcheck(void);
void CDflush(int charaindex);
void CDsend(int fd);
BOOL CONNECT_appendCAbuf(int fd, char *data, int size);
BOOL CONNECT_appendCDbuf(int fd, char *data, int size);

/* Serverstate */
void SERVSTATE_decrementCloseallsocketnum(void);
int SERVSTATE_getCloseallsocketnum(void);
int SERVSTATE_SetAcceptMore(int nvalue);
int SERVSTATE_getShutdown(void);
void SERVSTATE_setShutdown(int a);
int SERVSTATE_getLimittime(void);
int SERVSTATE_getDsptime(void);
void SERVSTATE_setDsptime(int a);
void SERVSTATE_setLimittime(int a);

/* �����þ� */
void outputNetProcLog(int fd, int mode);
void chardatasavecheck(void);
void closeAllConnectionandSaveData(void);
BOOL SetShutdown(BOOL nvalue);
int GetShutdown(void);

/* ������ƻ��������� */
int getfdFromCdkey(const char *cdkey);
int getfdFromCdkeyWithLogin(const char *cdkey);
int getfdFromCharaIndex(int charaindex);
int getcdkeyFromCharaIndex(int charaindex, char *out, int outlen);
int getCharindexFromFdid(int fdid);
int getFdidFromCharaIndex(int charind);
int getfdFromFdid(int fdid);

/* CONNECT member function. */
INLINE int CONNECT_checkfd(const int fd);
BOOL CONNECT_isCLI(const int fd);
BOOL CONNECT_isAC(const int fd);
BOOL CONNECT_isUnderLogin(const int fd);
BOOL CONNECT_isWhileLogout(const int fd);
BOOL CONNECT_isWhileLogin(const int fd);
BOOL CONNECT_isNOTLOGIN(const int fd);
BOOL CONNECT_isLOGIN(const int fd);

int CONNECT_getUse(int fd);
int CONNECT_getUse_debug(int fd, int i);

void CONNECT_setCharaindex(int fd, int a);
int CONNECT_getCharaindex(int fd);
void CONNECT_getCdkey(int fd, char *out, int outlen);
void CONNECT_setCdkey(int fd, char *in);
#ifdef _NEWCLISETSERVID
void CONNECT_setServid(int fd, int a);
int CONNECT_getServid(int fd);
#endif
#ifdef _NEWCLISETMAC
void CONNECT_getMAC(int fd, char *out, int outlen);
void CONNECT_setMAC(int fd, char *in);
#endif
void CONNECT_getPasswd(int fd, char *out, int outlen);
void CONNECT_setPasswd(int fd, char *in);
void CONNECT_getCharname(int fd, char *out, int outlen);
void CONNECT_setCharname(int fd, char *in);
int CONNECT_getFdid(int fd);
int CONNECT_getCtype(int fd);
void CONNECT_setCtype(int fd, int a);
void CONNECT_setDuelcharaindex(int fd, int i, int a);
int CONNECT_getDuelcharaindex(int fd, int i);
void CONNECT_setBattlecharaindex(int fd, int i, int a);
int CONNECT_getBattlecharaindex(int fd, int i);
void CONNECT_setJoinpartycharaindex(int fd, int i, int a);
int CONNECT_getJoinpartycharaindex(int fd, int i);
void CONNECT_setTradecardcharaindex(int fd, int i, int a);
int CONNECT_getTradecardcharaindex(int fd, int i);
/* MT����������Ѩ�ͷ� */
#define CONNECT_endOne_debug(a) CONNECT_endOne((a), __LINE__)

/* ��������������(��������ľ������ɱ��) */
void CONNECT_setCDKEY(int sockfd, char *cd);
void CONNECT_getCDKEY(int sockfd, char *out, int outlen);
void CONNECT_setState(int fd, int s);
int CONNECT_getState(int fd);
void CONNECT_checkStatecount(int a);

BOOL CONNECT_acfdInitRB(int fd);
BOOL CONNECT_acfdInitWB(int fd);

#ifdef _BATTLE_TIMESPEED
// void setDefBTime( int fd, unsigned int times);
// unsigned int getDefBTime( int fd);
BOOL CheckDefBTime(int charaindex, int fd, unsigned int lowTime,
                   unsigned int battletime, unsigned int addTime);
#endif

#ifdef _TYPE_TOXICATION
void setToxication(int fd, int flg);
int getToxication(int fd);
#endif
void sigusr1(int i);
void sigusr2(int i); // Arminius 6.26 signal

unsigned long CONNECT_get_userip(int fd);
void CONNECT_set_pass(int fd, BOOL b_ps);
BOOL CONNECT_get_pass(int fd);
void CONNECT_set_state_trans(int fd, int a);
int CONNECT_get_state_trans(int fd);
// ttom end

// CoolFish: +9 2001/4/18
void CONNECT_setCloseRequest(int fd, int count);
void CONNECT_set_first_warp(int fd, BOOL b_ps);
BOOL CONNECT_get_first_warp(int fd);
int isDie(int fd);
void setDie(int fd);

// Arminius 6/22 encounter
int CONNECT_get_CEP(int fd);
void CONNECT_set_CEP(int fd, int cep);

// Arminius 7.12 login announce
int CONNECT_get_announced(int fd);
void CONNECT_set_announced(int fd, int a);

// shan trade(DoubleCheck) begin
int CONNECT_get_confirm(int fd);
void CONNECT_set_confirm(int fd, BOOL b);
// end

void CONNECT_setTradecharaindex(int fd, int i, int a);

void CONNECT_setLastrecvtime(int fd, struct timeval *a);
void CONNECT_getLastrecvtime(int fd, struct timeval *a);
void CONNECT_setLastrecvtime_D(int fd, struct timeval *a);
void CONNECT_getLastrecvtime_D(int fd, struct timeval *a);
void CONNECT_SetBattleRecvTime(int fd, struct timeval *a);
void CONNECT_GetBattleRecvTime(int fd, struct timeval *a);
void CONNECT_setTradeTmp(int fd, char *a);
void CONNECT_getTradeTmp(int fd, char *trademsg, int trademsglen);
int checkNu(int fd);

// Arminius 7.2 Ra's amulet
void setNoenemy(int fd);
void clearNoenemy(int fd);
int getNoenemy(int fd);
void setEqNoenemy(int fd, int level);
void clearEqNoenemy(int fd);
int getEqNoenemy(int fd);

#ifdef _Item_MoonAct
void setEqRandenemy(int fd, int level);
void clearEqRandenemy(int fd);
int getEqRandenemy(int fd);
#endif

#ifdef _CHIKULA_STONE
void setChiStone(int fd, int nums);
int getChiStone(int fd);
#endif

// Arminius 7.31 cursed stone
void setStayEncount(int fd);
void clearStayEncount(int fd);
int getStayEncount(int fd);
void CONNECT_setBDTime(int fd, int nums);
int CONNECT_getBDTime(int fd);

// Arminius debug
void CONNECT_setUse(int fd, int a);

#ifdef _ITEM_PILEFORTRADE
void CONNECT_setTradeList(int fd, int num);
int CONNECT_getTradeList(int fd);
#endif

#define QUEUE_LENGTH1 10
#define QUEUE_LENGTH2 10

BOOL MSBUF_CHECKbuflen(int size, float defp);

void SetTcpBuf(int fd, fd_set *fds);

#ifdef _OTHER_SAAC_LINK
BOOL OtherSaacConnect(void);
#endif

#ifdef _EPOLL_ET_MODE
int doSocketAccept();
// ����
void doSocketRecv(struct epoll_event ev);
// ����
void doSocketSend(struct epoll_event ev);
// void doSocketSend();
BOOL CONNECT_checkthreadflag(int fd, int flag);
int CONNECT_getversionpass(int fd);
void Dispatch_read_buffer(int fdremember, int threadflag);
void System_Loop();
#endif

#endif
