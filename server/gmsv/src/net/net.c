#define __NET_C__
#include "version.h"
#include "linux_platform.h"
#include <errno.h>
//
#include "net.h"
#include "buf.h"
#include "char.h"
#include "config_file.h"
#include "enemy.h"
#include "handletime.h"
#include "item_event.h"
#include "log.h"
#include "lssproto_serv.h"
#include "msignal.h"
#include "object.h"
#include "saacproto_cli.h"
#include "util.h"
// Arminius 7.31 cursed stone
#include "battle.h"
#include "char_talk.h"
#include "pet_event.h"
#include "petmail.h"
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "pet_event.h"
#ifdef _LUCK_STAR
#include "longzoro/luckstar.h"
#endif
#ifdef _PLAYER_DIY_MAP
#include "readmap.h"
extern Player_Diy_Map PlayerDiyMap[Player_Diy_Map_NUM];
#endif
#ifdef _GMSV_DEBUG
extern char *DebugMainFunction;
#endif
#include "longzoro/sasql.h"

char rbmess[1024 * 256];

#define MAXSIZE 64000
#define MAXEPS 256
// #define EVENTS 100
#define LISTENQ 32
#define SERV_PORT 8000

#ifdef _NEW_SERVER_
BOOL bNewServer = TRUE;
#else
BOOL bNewServer = FALSE;
#endif
extern time_t initTime;
// Nuke +1 0901: For state monitor
int StateTable[WHILESAVEWAIT + 1];
#ifdef _EPOLL
int epollfd;
struct epoll_event evEpoll;
struct epoll_event *eventsEpoll;
void procPlay();

typedef struct tagNode {
  int fd;
  Node *next;
} Node;

Node *EpollSendpNode[4];
extern BOOL lianjielog;
pthread_mutex_t lianbiaomutex;
pthread_mutex_t lianbiaoRecvmutex;

#define LIANBIAOSENDLOCK pthread_mutex_lock(&lianbiaomutex);
#define LIANBIAOSENDUNLOCK pthread_mutex_unlock(&lianbiaomutex);
#define LIANBIAORECVLOCK pthread_mutex_lock(&lianbiaoRecvmutex);
#define LIANBIAORECVUNLOCK pthread_mutex_unlock(&lianbiaoRecvmutex);

#define CONNECT_LOCK(i) pthread_mutex_lock(&Connect[i].sEndmutex);
#define CONNECT_UNLOCK(i) pthread_mutex_unlock(&Connect[i].sEndmutex);
#define CONNECT_RecvLOCK(i) pthread_mutex_lock(&Connect[i].rEcvmutex);
#define CONNECT_RecvUNLOCK(i) pthread_mutex_unlock(&Connect[i].rEcvmutex);
#endif

extern int luaplayernum;

int CHAR_players();
#define CONO_CHECK_LOGIN 0x001
#define CONO_CHECK_ITEM 0x010
#define CONO_CHECK_PET 0x100
int cono_check = 0x111;

int AC_WBSIZE = (1024 * 1024);
// ttom+1 for the performatce
static unsigned int MAX_item_use = 0;
int i_shutdown_time = 0;       // ttom
BOOL b_first_shutdown = FALSE; // ttom

int mfdfulll = 0;
int sendspeed = 0;
int recvspeed = 0;

#ifdef _NO_ATTACK
unsigned int atttime = 0;
static unsigned long useripnum = 100000;
static unsigned long *userip = 0;
static unsigned long *userip_count_data = 0;
static unsigned long useripcount = 0;
static unsigned long *useriptime = 0;
#endif

typedef struct tagServerState {
  BOOL acceptmore;
  unsigned int fdid; /* fd */
  unsigned int closeallsocketnum;
  int shutdown;  /*  �ӡ���ëshutdown����ƹ���� */
  int dsptime;   /* shutdown ƹ���񼰷�����  ��*/
  int limittime;
} ServerState;

#ifdef _MO_LNS_NLSUOXU
typedef struct tagFuncKk {
  char *FileName; /* file_name */
  char *FuncName; /*ִ func_name */
  unsigned int EspTime;
  unsigned int MespTime;
  int ID;
  unsigned int GetTime;
} FuncKk;

FuncKk TimerLua[50];
void SetTimerLua(void)
{
  int i;
  for (i = 0; i < arraysizeof(TimerLua); i++) {
    TimerLua[i].FileName = NULL;
    TimerLua[i].FuncName = NULL;
    TimerLua[i].EspTime = -1;
    TimerLua[i].MespTime = -1;
    TimerLua[i].ID = -1;
    TimerLua[i].GetTime = -1;
  }
}

int SetTimer_net(char *FileName, char *FuncName, unsigned int EspTime) {
  int i;
  for (i = 0; i < arraysizeof(TimerLua); i++) {
    if (TimerLua[i].ID == -1)
      break;
  }
  TimerLua[i].FileName = FileName;
  TimerLua[i].FuncName = FuncName;
  TimerLua[i].EspTime = NowTime.tv_sec;
  TimerLua[i].MespTime = NowTime.tv_usec;
  TimerLua[i].ID = i + 1;
  TimerLua[i].GetTime = EspTime * 1024;
  return i + 1;
}
#endif

typedef struct tagCONNECT {
  BOOL use;
  char *rb;
  int rbuse;
  char *wb;
  int wbuse;
  int check_rb_oneline_b;
  int check_rb_time;
  pthread_mutex_t mutex;
  struct sockaddr_in sin; /* �����Ƽ�ʧ������ */
  ConnectType ctype;      /* ��������������˒� */
  char cdkey[CDKEYLEN];   /* CDKEY */
  char passwd[PASSWDLEN]; /* �ɵ������� */
  LoginType state;        /* �ػ�������̼����� */
  int nstatecount;
  char charname[CHARNAMELEN]; /* ����̼��w��ƽ�ҷo */
  int charaindex; /* char?�d�߼��̼������͵��z
                   * ����̼�ԉ�����ɬ�ý�ľ�©z-1�������ɻ���
                   * ?�d��ئ���ݩz
                   */
  char CAbuf[2048]; /*  CA() ë�������¿м��������� */
  int CAbufsiz; /*  CAbuf ��������  */
  struct timeval lastCAsendtime; /*��ԉ��CAë˪�������� */
  char CDbuf[2048]; /*  CD() ë�������¿м��������� */
  int CDbufsiz; /*  CDbuf ��������  */
  struct timeval lastCDsendtime; /*��ԉ��CDë˪�������� */
  struct timeval lastCharSaveTime; /* ��ԉ��ƽ�ҷ·�����ë����Ƥ�������� */
  struct timeval lastprocesstime; /* ��ԉ����������ë�ʆA��������*/
  struct timeval lastreadtime; /* ��ԉ��read�������ީz���練�л���*/

// Nuke start 08/27 : For acceleration avoidance
// WALK_TOLERANCE: Permit n W messages in a second (3: is the most restricted)
#define WALK_TOLERANCE 4
#define WALK_SPOOL 5
#define WALK_RESTORE 100
  unsigned int Walktime;
  unsigned int lastWalktime;
  unsigned int Walkcount;
  int Walkspool; // For walk burst after release key F10
  int Walkrestore;
// B3_TOLERANCE: Time distance between recently 3 B message (8: is the latgest)
// BEO_TOLERANCE: Time distance between the lastmost B and EO (5: is the
// largest)
#define B3_TOLERANCE 5
#define BEO_TOLERANCE 3
#define BEO_SPOOL 10
#define BEO_RESTORE 100
  unsigned int Btime;
  unsigned int lastBtime;
  unsigned int lastlastBtime;
  unsigned int EOtime;
#ifdef _BATTLE_TIMESPEED
  // unsigned int  DefBtime;
  int BDTime;
  int CBTime;
#endif

#ifdef _TYPE_TOXICATION
  int toxication;
#endif

#ifdef _ITEM_ADDEXP // vincent ������N
  int EDTime;
#endif
  // unsigned int BEO;
  int BEOspool;
  int BEOrestore;
  int credit;
  int fcold;
  // Nuke 0406: New Flow Control
  int nu;
  int nu_decrease;
  // Nuke 1213: Flow Control 2
  int packetin;

  // Nuke 0624: Avoid Null Connection
  unsigned int cotime;
  // Nuke 0626: For no enemy
  int noenemy;
  // Arminius 7.2: Ra's amulet
  int eqnoenemy;
#ifdef _Item_MoonAct
  int eqrandenemy;
#endif

#ifdef _CHIKULA_STONE
  int chistone;
#endif
  // Arminius 7.31: cursed stone
  int stayencount;

  int battlecharaindex[CONNECT_WINDOWBUFSIZE];
  int duelcharaindex[CONNECT_WINDOWBUFSIZE];
  int tradecardcharaindex[CONNECT_WINDOWBUFSIZE];
  int joinpartycharaindex[CONNECT_WINDOWBUFSIZE];

  // CoolFish: Trade 2001/4/18
  int tradecharaindex[CONNECT_WINDOWBUFSIZE];
  int errornum;
  int fdid;
  int close_request; // the second have this
  int appendwb_overflow_flag; // what's wb && rb?

  BOOL b_shut_up; // for avoid the user wash the screen
  BOOL b_pass;    // for avoid the unlimited area
  struct timeval Wtime;
  struct timeval WLtime;
  BOOL b_first_warp;
  int state_trans;
  // CoolFish: Trade 2001/4/18
  char TradeTmp[256];

#ifdef _ITEM_PILEFORTRADE
  int tradelist;
#endif
  // Shan Recvdata Time
  struct timeval lastrecvtime; // 'FM' Stream Control time
  struct timeval lastrecvtime_d; // DENGON Talk Control time
  // Arminius: 6.22 encounter
  int CEP; // Current Encounter Probability
  // Arminius 7.12 login announce
  int announced;
  // shan battle delay time 2001/12/26
  struct timeval battle_recvtime;
  BOOL confirm_key;

#ifdef _NEWCLISETSERVID
  int servid;
#endif

#ifdef _NEWCLISETMAC
  char mac[128];
#endif

  int connecttime;
  unsigned int starttime;
#ifdef _NEW_FUNC_DECRYPT
  int newerrnum;
#endif
} CONNECT;

CONNECT *Connect;

#define SINGLETHREAD
#define MUTLITHREAD
#define ANY_THREAD

ServerState servstate;

pthread_mutex_t MTIO_servstate_m;
#define SERVSTATE_LOCK() pthread_mutex_lock(&MTIO_servstate_m);
#define SERVSTATE_UNLOCK() pthread_mutex_unlock(&MTIO_servstate_m);
#define CONNECT_LOCK_ARG2(i, j) pthread_mutex_lock(&Connect[i].mutex);
#define CONNECT_UNLOCK_ARG2(i, j) pthread_mutex_unlock(&Connect[i].mutex);
#define CONNECT_LOCK(i) pthread_mutex_lock(&Connect[i].mutex);
#define CONNECT_UNLOCK(i) pthread_mutex_unlock(&Connect[i].mutex);

void SetTcpBuf(int fd, fd_set *fds) {
  int yes = 1;
  int result = fcntl(fd, F_SETFL, O_NONBLOCK);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes,
             sizeof yes); // reuse fix
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&yes,
             sizeof yes); // reuse fix

  if (fd == -1) {
    perror("accept");
  } else {
    FD_SET(fd, fds);
  }
  result = fcntl(fd, F_SETFL, O_NONBLOCK);
}

#ifdef _SAME_IP_ONLINE_NUM
int SameIpOnlineNum(unsigned long ip) {
#ifdef _CHECK_SEVER_IP
  if (checkServerIp(ip) == TRUE) {
    return 0;
  }
#endif
  int MAX_USER = getFdnum();
  int i, num = 0;
  if (getSameIpOnlineNum() > 0) {
    for (i = 4; i < MAX_USER; i++) {
      int i_use;
      i_use = CONNECT_getUse(i);
      if (i_use) {
        if (ip == CONNECT_get_userip(i)) {
          num++;
          if (num >= getSameIpOnlineNum()) {
            return 1;
          }
        }
      }
    }
  }

  return 0;
}
#endif

ANY_THREAD static void SERVSTATE_initserverState(void) {
  SERVSTATE_LOCK();
  servstate.acceptmore = TRUE;
  servstate.fdid = 0;
  servstate.closeallsocketnum = -1;
  servstate.shutdown = 0;
  servstate.limittime = 0;
  servstate.dsptime = 0;
  SERVSTATE_UNLOCK();
}

ANY_THREAD int SERVSTATE_SetAcceptMore(int nvalue) {
  BOOL buf;
  SERVSTATE_LOCK();
  buf = servstate.acceptmore;
  servstate.acceptmore = nvalue;
  SERVSTATE_UNLOCK();
  return buf;
}
ANY_THREAD static int SERVSTATE_incrementFdid(void) {
  int ret;
  SERVSTATE_LOCK();
  ret = servstate.fdid++;
  SERVSTATE_UNLOCK();
  return ret;
}
ANY_THREAD static void SERVSTATE_setCloseallsocketnum(int a) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum = a;
  SERVSTATE_UNLOCK();
}
ANY_THREAD static void SERVSTATE_incrementCloseallsocketnum(void) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum++;
  SERVSTATE_UNLOCK();
}
ANY_THREAD void SERVSTATE_decrementCloseallsocketnum(void) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum--;
  SERVSTATE_UNLOCK();
}
ANY_THREAD int SERVSTATE_getCloseallsocketnum(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.closeallsocketnum;
  SERVSTATE_UNLOCK();
  return a;
}

ANY_THREAD static int SERVSTATE_getAcceptmore(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.acceptmore;
  SERVSTATE_UNLOCK();
  return a;
}
ANY_THREAD int SERVSTATE_getShutdown(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.shutdown;
  SERVSTATE_UNLOCK();
  return a;
}
ANY_THREAD void SERVSTATE_setShutdown(int a) {
  SERVSTATE_LOCK();
  servstate.shutdown = a;
  SERVSTATE_UNLOCK();
}
ANY_THREAD int SERVSTATE_getLimittime(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.limittime;
  SERVSTATE_UNLOCK();
  return a;
}
ANY_THREAD void SERVSTATE_setLimittime(int a) {
  SERVSTATE_LOCK();
  servstate.limittime = a;
  SERVSTATE_UNLOCK();
}
ANY_THREAD int SERVSTATE_getDsptime(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.dsptime;
  SERVSTATE_UNLOCK();
  return a;
}
ANY_THREAD void SERVSTATE_setDsptime(int a) {
  SERVSTATE_LOCK();
  servstate.dsptime = a;
  SERVSTATE_UNLOCK();
}

static int appendWB(int fd, char *buf, int size) {
#ifdef _OTHER_SAAC_LINK
  if (CONNECT_getCtype(fd) != AC) {
#else
  if (fd != acfd) {
#endif
    if (Connect[fd].wbuse + size >= WBSIZE) {
      print("appendWB:err buffer over[%d]:%s \n", Connect[fd].wbuse + size,
            Connect[fd].cdkey);
      return -1;
    }
  } else {
    if (Connect[fd].wbuse + size > AC_WBSIZE) {
      return -1;
    }
  }
  memcpy(Connect[fd].wb + Connect[fd].wbuse, buf, size);
  Connect[fd].wbuse += size;
  return size;
}

static int appendRB(int fd, char *buf, int size) {
#ifdef _OTHER_SAAC_LINK
  if (CONNECT_getCtype(fd) != AC) {
#else
  if (fd != acfd) {
#endif
    if (Connect[fd].rbuse + size > RBSIZE) {
      // print( "appendRB:OTHER(%d) err buffer over \n", fd );
      return -1;
    }
  } else {
    if (strlen(buf) > size) {
      print("appendRB AC buffer len err : %d/%d=\n", strlen(buf), size);
    }
    if (Connect[fd].rbuse + size > AC_RBSIZE) {
      printf("%s\n", buf);
      print("appendRB AC err buffer over: len:%d - AC_RBSIZE:%d \n",
            Connect[fd].rbuse + size, AC_RBSIZE);
      return -1;
    }
  }
  memcpy(Connect[fd].rb + Connect[fd].rbuse, buf, size);
  Connect[fd].rbuse += size;
  return size;
}

static int shiftWB(int fd, int len) {
  if (Connect[fd].wbuse < len) {
    print("shiftWB: err\n");
    return -1;
  }

  memmove(Connect[fd].wb, Connect[fd].wb + len, Connect[fd].wbuse - len);
  Connect[fd].wbuse -= len;

  if (Connect[fd].wbuse < 0) {
    print("shiftWB:wbuse err\n");
    Connect[fd].wbuse = 0;
  }

  return len;
}

static int shiftRB(int fd, int len) {
  if (Connect[fd].rbuse < len) {
    print("shiftRB: err\n");
    return -1;
  }

  memmove(Connect[fd].rb, Connect[fd].rb + len, Connect[fd].rbuse - len);
  Connect[fd].rbuse -= len;

  if (Connect[fd].rbuse < 0) {
    print("shiftRB:rbuse err\n");
    Connect[fd].rbuse = 0;
  }

  return len;
}

SINGLETHREAD int lsrpcClientWriteFunc(int fd, char *buf, int size) {
  int r;

  if (Connect[fd].use == FALSE) {
    return FALSE;
  }

  if (Connect[fd].appendwb_overflow_flag) {
    print("lsrpcClientWriteFunc: buffer overflow fd:%d\n", fd);
    return -1;
  }

  r = appendWB(fd, buf, size);

  // Nuke *1 0907: Ignore acfd from WB error
#ifdef _OTHER_SAAC_LINK
  if ((r < 0) && (CONNECT_getCtype(fd) != AC && CONNECT_getCtype(fd) != SQL)) {
#else
  if ((r < 0) && (fd != acfd)) {
#endif
    Connect[fd].appendwb_overflow_flag = 1;
#ifdef _NETLOG_
    char cdkey[16];
    char charname[32];
    CONNECT_getCharname(fd, charname, 32);
    CONNECT_getCdkey(fd, cdkey, 16);
    char token[128];
    sprintf(token, "appendWB����  r=%d    Connect[ fd ].wbuse + size=%d", r,
            Connect[fd].wbuse + size);
    LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif

    CONNECT_endOne_debug(fd);
    // Nuke + 1 0901: Why close
    //  print("closed in lsrpcClientWriteFunc");
#ifdef _EPOLL
  } else {
    ModEpollOut(fd);

#endif
  }
  return r;
}

static int logRBuseErr = 0;
SINGLETHREAD BOOL GetOneLine_fix(int fd, char *buf, int max) {
  int i;

  if (Connect[fd].rbuse == 0)
    return FALSE;

  if (Connect[fd].check_rb_oneline_b == 0 &&
      Connect[fd].check_rb_oneline_b == Connect[fd].rbuse) {
    return FALSE;
  }

  for (i = 0; i < Connect[fd].rbuse && i < (max - 1); i++) {
    if (Connect[fd].rb[i] == '\n') {
      memcpy(buf, Connect[fd].rb, i + 1);
      buf[i + 1] = '\0';
      shiftRB(fd, i + 1);
      logRBuseErr = 0;
      Connect[fd].check_rb_oneline_b = 0;
      Connect[fd].check_rb_time = 0;
      return TRUE;
    }
  }

  if (fd == acfd) {
    logRBuseErr++;
  }

  if (logRBuseErr >= 50) {
    Connect[fd].rb[Connect[fd].rbuse] = '\n';
    printf("rebuse err %d:%d\n", logRBuseErr, Connect[fd].rbuse);
    logRBuseErr = 0;
  }

#ifdef _OTHER_SAAC_LINK
  if (CONNECT_getCtype(fd) == AC &&
      strstr(Connect[fd].rb, "ACCharLoad") != NULL &&
#else
  if (fd == acfd && strstr(Connect[fd].rb, "ACCharLoad") != NULL &&
#endif
      logRBuseErr >= 50) { // Connect[fd].rb
    char buf[AC_RBSIZE];
    memcpy(buf, Connect[fd].rb, Connect[fd].rbuse + 1);
    buf[Connect[fd].rbuse + 1] = 0;
    LogAcMess(fd, "RBUFFER", buf);
    logRBuseErr = 0;
  }

  Connect[fd].check_rb_oneline_b = Connect[fd].rbuse;

  return FALSE;
}

ANY_THREAD BOOL initConnectOne(int sockfd, struct sockaddr_in *sin, int len) {
  CONNECT_LOCK(sockfd);

  Connect[sockfd].use = TRUE;
  Connect[sockfd].ctype = NOTDETECTED;
  Connect[sockfd].wbuse = Connect[sockfd].rbuse = 0;
  Connect[sockfd].check_rb_oneline_b = 0;
  Connect[sockfd].check_rb_time = 0;

  memset(Connect[sockfd].cdkey, 0, sizeof(Connect[sockfd].cdkey));
  memset(Connect[sockfd].passwd, 0, sizeof(Connect[sockfd].passwd));

  Connect[sockfd].state = NULLCONNECT;
  Connect[sockfd].nstatecount = 0;
  memset(Connect[sockfd].charname, 0, sizeof(Connect[sockfd].charname));
  Connect[sockfd].charaindex = -1;

  Connect[sockfd].CAbufsiz = 0;
  Connect[sockfd].CDbufsiz = 0;
  Connect[sockfd].rbuse = 0;
  Connect[sockfd].wbuse = 0;
  Connect[sockfd].check_rb_oneline_b = 0;
  Connect[sockfd].check_rb_time = 0;

  Connect[sockfd].close_request = 0; /* �Ӭۢ�ư׷º� */
  // Nuke 08/27 For acceleration avoidance
  Connect[sockfd].Walktime = 0;
  Connect[sockfd].lastWalktime = 0;
  Connect[sockfd].Walkcount = 0;
  Connect[sockfd].Walkspool = WALK_SPOOL;
  Connect[sockfd].Walkrestore = WALK_RESTORE;
  Connect[sockfd].Btime = 0;
  Connect[sockfd].lastBtime = 0;
  Connect[sockfd].lastlastBtime = 0;
  Connect[sockfd].EOtime = 0;
  Connect[sockfd].nu_decrease = 0;
#ifdef _BATTLE_TIMESPEED
  // Connect[sockfd].DefBtime = 0;
  Connect[sockfd].BDTime = 0;
  Connect[sockfd].CBTime = 0;
#endif
#ifdef _TYPE_TOXICATION
  Connect[sockfd].toxication = 0;
#endif
#ifdef _ITEM_ADDEXP // vincent ��������
  Connect[sockfd].EDTime = 0;
#endif
  //      Connect[sockfd].BEO = 0;
  Connect[sockfd].BEOspool = BEO_SPOOL;
  Connect[sockfd].BEOrestore = BEO_RESTORE;
  // ttom
  Connect[sockfd].b_shut_up = FALSE;
  Connect[sockfd].Wtime.tv_sec = 0;   //
  Connect[sockfd].Wtime.tv_usec = 0;  //
  Connect[sockfd].WLtime.tv_sec = 0;  //
  Connect[sockfd].WLtime.tv_usec = 0; //
  Connect[sockfd].b_first_warp = FALSE;
  Connect[sockfd].state_trans = 0; // avoid the trans

  Connect[sockfd].credit = 3;
  Connect[sockfd].fcold = 0;
  // Nuke 0406: New Flow Control
  Connect[sockfd].nu = 30;

  // Nuke 1213: Flow Control 2
  Connect[sockfd].packetin = 30; // if 10x10 seconds no packet, drop the line

  // Nuke 0624: Avoid Useless Connection
  Connect[sockfd].cotime = 0;
  // Nuke 0626: For no enemy
  Connect[sockfd].noenemy = 0;
  // Arminius 7.2: Ra's amulet
  Connect[sockfd].eqnoenemy = 0;

#ifdef _Item_MoonAct
  Connect[sockfd].eqrandenemy = 0;
#endif
#ifdef _CHIKULA_STONE
  Connect[sockfd].chistone = 0;
#endif
  // Arminius 7.31: cursed stone
  Connect[sockfd].stayencount = 0;

  // CoolFish: Init Trade 2001/4/18
  memset(&Connect[sockfd].TradeTmp, 0, sizeof(Connect[sockfd].TradeTmp));
#ifdef _ITEM_PILEFORTRADE
  Connect[sockfd].tradelist = -1;
#endif
  // Arminius 6.22 Encounter
  Connect[sockfd].CEP = 0;

  // Arminius 7.12 login announce
  Connect[sockfd].announced = 0;

  Connect[sockfd].confirm_key = FALSE; // shan trade(DoubleCheck)

#ifdef _NEW_FUNC_DECRYPT
  Connect[sockfd].newerrnum = 0;
#endif

  if (sin != NULL)
    memcpy(&Connect[sockfd].sin, sin, len);

  memset(&Connect[sockfd].lastprocesstime, 0,
         sizeof(Connect[sockfd].lastprocesstime));

  memcpy(&Connect[sockfd].lastCAsendtime, &NowTime,
         sizeof(Connect[sockfd].lastCAsendtime));

  memcpy(&Connect[sockfd].lastCDsendtime, &NowTime,
         sizeof(Connect[sockfd].lastCDsendtime));

  memcpy(&Connect[sockfd].lastCharSaveTime, &NowTime,
         sizeof(Connect[sockfd].lastCharSaveTime));

  // Shan Add
  memcpy(&Connect[sockfd].lastrecvtime, &NowTime,
         sizeof(Connect[sockfd].lastrecvtime));

  memcpy(&Connect[sockfd].lastrecvtime_d, &NowTime,
         sizeof(Connect[sockfd].lastrecvtime_d));

  memcpy(&Connect[sockfd].battle_recvtime, &NowTime,
         sizeof(Connect[sockfd].battle_recvtime));

  memcpy(&Connect[sockfd].lastreadtime, &NowTime,

         sizeof(struct timeval));

  Connect[sockfd].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;

  Connect[sockfd].errornum = 0;

  Connect[sockfd].fdid = SERVSTATE_incrementFdid();

  CONNECT_UNLOCK(sockfd);

  Connect[sockfd].appendwb_overflow_flag = 0;

  Connect[sockfd].connecttime = time(NULL);

  memset(Connect[sockfd].mac, 0, sizeof(Connect[sockfd].mac));
  return TRUE;
}

ANY_THREAD BOOL _CONNECT_endOne(char *file, int fromline, int sockfd, int line) {
  if (errno == 113 || errno == 104) {
    return TRUE;
  }
  CONNECT_LOCK_ARG2(sockfd, line);

  if (Connect[sockfd].use == FALSE) {
    CONNECT_UNLOCK_ARG2(sockfd, line);
    print("Connect [%d] is not used.!!!\n", sockfd);
    return TRUE;
  }
  Connect[sockfd].use = FALSE;
  if (Connect[sockfd].ctype == CLI &&
      CHAR_CHECKINDEX(Connect[sockfd].charaindex) == TRUE) {
    CONNECT_UNLOCK_ARG2(sockfd, line);
#ifdef _OFFLINE_SYSTEM
    if (CHAR_getWorkInt(Connect[sockfd].charaindex, CHAR_WORK_OFFLINE) != 0) {
      CHAR_setWorkInt(Connect[sockfd].charaindex, CHAR_WORKFD, -1);
    } else
#endif
    {
      if (!CHAR_logout(Connect[sockfd].charaindex, TRUE)) {
      }
      print("Connect cd key=%s.\n", Connect[sockfd].cdkey);
    }
    CONNECT_LOCK_ARG2(sockfd, line);
  }
#ifdef _NO_ATTACK
  else {
    if (strlen(Connect[sockfd].cdkey) < 1) {
      time_t curtime;
      struct tm *p;
      time(&curtime);
      p = localtime(&curtime);
      curtime = mktime(p);
      if (curtime - Connect[sockfd].starttime <= getAttTime()) {
        unsigned long tmpip = CONNECT_get_userip(sockfd);
        unsigned long i = 0;
        int ipa, ipb, ipc, ipd;
        char ip[32];
        ipa = (tmpip % 0x100);
        tmpip = tmpip / 0x100;
        ipb = (tmpip % 0x100);
        tmpip = tmpip / 0x100;
        ipc = (tmpip % 0x100);
        tmpip = tmpip / 0x100;
        ipd = (tmpip % 0x100);
        sprintf(ip, "%d.%d.%d.%d", ipa, ipb, ipc, ipd);
        for (; i < useripcount; ++i) {
          if (userip[i] == tmpip) {
            if (userip_count_data[i] >= getAttCnt()) {
              Connect[sockfd].wbuse = 0;
              Connect[sockfd].rbuse = 0;
              Connect[sockfd].CAbufsiz = 0;
              Connect[sockfd].CDbufsiz = 0;
              CONNECT_UNLOCK_ARG2(sockfd, line);
              print("Connect sockfd=%d, ip=%s \n", sockfd, ip);
              close(sockfd);
              userip_count_data[i] = 1;
              useriptime[i] = curtime;
              return TRUE;
            }
            if (curtime - useriptime[i] <= getAttSafeTime()) {
              ++userip_count_data[i];
              break;
            } else {
              useriptime[i] = curtime;
              break;
            }
          }
        }
        if (strcmp(ip, getNoAttIp(0)) != 0 && strcmp(ip, getNoAttIp(1)) != 0 &&
            strcmp(ip, getNoAttIp(2)) != 0 && strcmp(ip, getNoAttIp(3)) != 0 &&
            strcmp(ip, getNoAttIp(4)) != 0) {
          if (i >= useripcount && useripcount < useripnum) {
            userip[useripcount] = tmpip;
            userip_count_data[useripcount] = 1;
            useriptime[useripcount] = curtime;
            useripcount++;
          }
        }
      }
    }
  }
#endif
  Connect[sockfd].charaindex = -1;
  Connect[sockfd].wbuse = 0;
  Connect[sockfd].rbuse = 0;
  Connect[sockfd].CAbufsiz = 0;
  Connect[sockfd].CDbufsiz = 0;
  CONNECT_UNLOCK_ARG2(sockfd, line);
  close(sockfd);

  return TRUE;
}
SINGLETHREAD BOOL initConnect(int size) {
  int i, j;
  ConnectLen = size;
  Connect = calloc(1, sizeof(CONNECT) * size);

  if (Connect == NULL)
    return FALSE;

  for (i = 0; i < size; i++) {
    memset(&Connect[i], 0, sizeof(CONNECT));
    Connect[i].charaindex = -1;
    Connect[i].rb = calloc(1, RBSIZE);

    if (Connect[i].rb == NULL) {
      fprint("calloc err\n");

      for (j = 0; j < i; j++) {
        free(Connect[j].rb);
        free(Connect[j].wb);
      }

      return FALSE;
    }

    memset(Connect[i].rb, 0, RBSIZE);
    Connect[i].wb = calloc(1, WBSIZE);

    if (Connect[i].wb == NULL) {
      fprint("calloc err\n");

      for (j = 0; j < i; j++) {
        free(Connect[j].rb);
        free(Connect[j].wb);
      }

      free(Connect[j].rb);
      return FALSE;
    }

    memset(Connect[i].wb, 0, WBSIZE);
  }

  print("ԤԼ %d ����...���� %.2f MB �ռ�...", size,
        (sizeof(CONNECT) * size + RBSIZE * size + WBSIZE * size) / 1024.0 /
            1024.0);

  SERVSTATE_initserverState();

  // ttom for the performance of gmsv
  MAX_item_use = getItemnum() * 0.98;

#ifdef _NO_ATTACK
  if (userip == 0)
    userip = malloc(sizeof(unsigned long) * useripnum);
  if (userip_count_data == 0)
    userip_count_data = malloc(sizeof(unsigned long) * useripnum);
  if (useriptime == 0)
    useriptime = malloc(sizeof(unsigned long) * useripnum);
#endif
  return TRUE;
}
BOOL CONNECT_acfdInitRB(int fd) {
#ifdef _OTHER_SAAC_LINK

  Connect[fd].rb = realloc(Connect[fd].rb, AC_RBSIZE);

  if (Connect[fd].rb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }

  memset(Connect[fd].rb, 0, AC_RBSIZE);
  return TRUE;
#else
  if (fd != acfd)
    return FALSE;

  Connect[fd].rb = realloc(Connect[acfd].rb, AC_RBSIZE);

  if (Connect[acfd].rb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }

  memset(Connect[acfd].rb, 0, AC_RBSIZE);
  return TRUE;
#endif
}
BOOL CONNECT_acfdInitWB(int fd) {
#ifdef _OTHER_SAAC_LINK

  Connect[fd].wb = realloc(Connect[fd].wb, AC_WBSIZE);

  if (Connect[fd].wb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }

  memset(Connect[fd].wb, 0, AC_WBSIZE);
  return TRUE;
#else
  if (fd != acfd)
    return FALSE;

  Connect[fd].wb = realloc(Connect[acfd].wb, AC_WBSIZE);

  if (Connect[acfd].wb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }

  memset(Connect[acfd].wb, 0, AC_WBSIZE);
  return TRUE;
#endif
}

ANY_THREAD void endConnect(void) {
  int i;
  print("connect: %d, connect_len: %d.\n", Connect, ConnectLen);
  if (Connect == NULL) return;
  for (i = 0; i < ConnectLen; i++) {
    int lco;
    lco = close(i);
    if (lco == 0) {
      CONNECT_endOne_debug(i);
    }
    print("free connect %d.\n", i);
    free(Connect[i].rb);
    free(Connect[i].wb);
  }
  free(Connect);
  print("free connect.\n");
}

ANY_THREAD BOOL CONNECT_appendCAbuf(int fd, char *data, int size) {
#ifdef _MASK_ENCOUNTER
  int charaindex = CONNECT_getCharaindex(fd);
  if (charaindex > -1) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
      if (getStayEncount(fd))
        return FALSE;
    } else {
      int oyaindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
      if (getStayEncount(getfdFromCharaIndex(oyaindex)))
        return FALSE;
    }
  } else {
    return FALSE;
  }
#endif
  CONNECT_LOCK(fd);
  /*  �Ǹ�Υǥ�ߥ��� ',' ��ʬ¿�����ݤ��ʤ������������   */
  if ((Connect[fd].CAbufsiz + size) >= sizeof(Connect[fd].CAbuf)) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  }

  memcpy(Connect[fd].CAbuf + Connect[fd].CAbufsiz, data, size);
  Connect[fd].CAbuf[Connect[fd].CAbufsiz + size] = ',';
  Connect[fd].CAbufsiz += (size + 1);
  CONNECT_UNLOCK(fd);
#ifdef _EPOLL
  // ModEpollOut(fd);
#endif
  return TRUE;
}

ANY_THREAD static int CONNECT_getCAbuf(int fd, char *out, int outmax,
                                      int *outlen) {
  CONNECT_LOCK(fd);
  if (Connect[fd].use == TRUE) {
    int cplen = min(outmax, Connect[fd].CAbufsiz);
    memcpy(out, Connect[fd].CAbuf, cplen);
    *outlen = cplen;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}
ANY_THREAD static int CONNECT_getCDbuf(int fd, char *out, int outmax,
                                      int *outlen) {
  CONNECT_LOCK(fd);

  if (Connect[fd].use == TRUE) {
    int cplen = min(outmax, Connect[fd].CDbufsiz);
    memcpy(out, Connect[fd].CDbuf, cplen);
    *outlen = cplen;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return 0;
  }
}

ANY_THREAD static int CONNECT_setCAbufsiz(int fd, int len) {
  CONNECT_LOCK(fd);

  if (Connect[fd].use == TRUE) {
    Connect[fd].CAbufsiz = len;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}
ANY_THREAD static int CONNECT_setCDbufsiz(int fd, int len) {
  CONNECT_LOCK(fd);

  if (Connect[fd].use == TRUE) {
    Connect[fd].CDbufsiz = len;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}

ANY_THREAD static void CONNECT_setLastCAsendtime(int fd, struct timeval *t) {
  CONNECT_LOCK(fd);
  Connect[fd].lastCAsendtime = *t;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD static void CONNECT_getLastCAsendtime(int fd, struct timeval *t) {
  CONNECT_LOCK(fd);
  *t = Connect[fd].lastCAsendtime;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD static void CONNECT_setLastCDsendtime(int fd, struct timeval *t) {
  CONNECT_LOCK(fd);
  Connect[fd].lastCDsendtime = *t;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD static void CONNECT_getLastCDsendtime(int fd, struct timeval *t) {
  CONNECT_LOCK(fd);
  *t = Connect[fd].lastCDsendtime;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getUse_debug(int fd, int i) {
  int a;
  if (0 > fd || fd >= ConnectLen) {
    return 0;
  }
  CONNECT_LOCK_ARG2(fd, i);
  a = Connect[fd].use;
  CONNECT_UNLOCK_ARG2(fd, i);
  return a;
}

ANY_THREAD int CONNECT_getUse(int fd) {
  int a;
  if (0 > fd || fd >= ConnectLen) {
    return 0;
  }
  CONNECT_LOCK(fd);
  a = Connect[fd].use;
  CONNECT_UNLOCK(fd);
  return a;
}
void CONNECT_setUse(int fd, int a)
// ANY_THREAD static void CONNECT_setUse( int fd , int a)
{
  CONNECT_LOCK(fd);
  Connect[fd].use = a;
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_checkStatecount(int a) {
  int i;
  int count = 0;

  for (i = 0; i < ConnectLen; i++) {
    if (Connect[i].use == FALSE || Connect[i].state != a)
      continue;

    if (Connect[i].nstatecount <= 0) {
      Connect[i].nstatecount = (int)time(NULL) + 60;
    } else {
      if (Connect[i].nstatecount < (int)time(NULL)) {
#ifdef _NETLOG_
        char cdkey[16];
        char charname[32];
        CONNECT_getCharname(CONNECT_getCharaindex(i), charname, 32);
        CONNECT_getCdkey(CONNECT_getCharaindex(i), cdkey, 16);
        char token[128];
        sprintf(token, "CONNECT_checkStatecount  T�� ");
        LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif

        CONNECT_endOne_debug(i);
        count++;
      }
    }
  }

  {
    memset(StateTable, 0, sizeof(StateTable));

    for (i = 0; i < ConnectLen; i++)
      if (Connect[i].use == TRUE)
        StateTable[Connect[i].state]++;
  }
}

ANY_THREAD void CONNECT_setState(int fd, int a) {
  CONNECT_LOCK(fd);
  Connect[fd].state = a;
  Connect[fd].nstatecount = 0;

  // Nuke start 0829: For debugging
  {
    char temp[128], buffer[256];
    int i;
    memset(StateTable, 0, sizeof(StateTable));

    for (i = 0; i < ConnectLen; i++) {
      if (Connect[i].use == TRUE) {
        if (Connect[i].state > WHILESAVEWAIT)
          continue;
        StateTable[Connect[i].state]++;
      }
    }

    buffer[0] = 0;

    for (i = 0; i <= WHILESAVEWAIT; i++) {
      sprintf(temp, "%-6d", StateTable[i]);
      strcat(buffer, temp);
    }
    //    print( "\nFILE:%s,LINE:%d", file,fromline );
    //    print( "\n{{%s}}", buffer );
  }
  // Nuke end

  CONNECT_UNLOCK(fd);
}

ANY_THREAD int CONNECT_getState(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].state;
  CONNECT_UNLOCK(fd);
  return a;
}

ANY_THREAD void CONNECT_setCharaindex(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].charaindex = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getCharaindex(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].charaindex;
  CONNECT_UNLOCK(fd);
  return a;
}
ANY_THREAD void CONNECT_getCdkey(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].cdkey);
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_setCdkey(int fd, char *cd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  snprintf(Connect[fd].cdkey, sizeof(Connect[fd].cdkey), "%s", cd);
  CONNECT_UNLOCK(fd);
}

#ifdef _NEWCLISETSERVID
ANY_THREAD void CONNECT_setServid(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].servid = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getServid(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].servid;
  CONNECT_UNLOCK(fd);
  return a;
}
#endif

#ifdef _NEWCLISETMAC
ANY_THREAD void CONNECT_getMAC(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].mac);
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_setMAC(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].mac, sizeof(Connect[fd].mac), in);
  CONNECT_UNLOCK(fd);
}
#endif

ANY_THREAD void CONNECT_getPasswd(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].passwd);
  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_setPasswd(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].passwd, sizeof(Connect[fd].passwd), in);
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getCtype(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].ctype;
  CONNECT_UNLOCK(fd);
  return a;
}
ANY_THREAD void CONNECT_setCtype(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].ctype = a;
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_getCharname(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].charname);
  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_setCharname(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].charname, sizeof(Connect[fd].charname), in);
  CONNECT_UNLOCK(fd);
}

ANY_THREAD int CONNECT_getFdid(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].fdid;
  CONNECT_UNLOCK(fd);
  return a;
}
ANY_THREAD void CONNECT_setDuelcharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].duelcharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getDuelcharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].duelcharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}
ANY_THREAD void CONNECT_setBattlecharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].battlecharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getBattlecharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].battlecharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}
ANY_THREAD void CONNECT_setJoinpartycharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].joinpartycharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getJoinpartycharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);

  a = Connect[fd].joinpartycharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}

// CoolFish: Trade 2001/4/18
ANY_THREAD void CONNECT_setTradecharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].tradecharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}

// Shan Begin
ANY_THREAD void CONNECT_setLastrecvtime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastrecvtime = *a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_getLastrecvtime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  *a = Connect[fd].lastrecvtime;
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_setLastrecvtime_D(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastrecvtime_d = *a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_getLastrecvtime_D(int fd, struct timeval *a) {
  CONNECT_LOCK(fd);
  *a = Connect[fd].lastrecvtime_d;
  CONNECT_UNLOCK(fd);
}

// 2001/12/26
ANY_THREAD void CONNECT_SetBattleRecvTime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].battle_recvtime = *a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_GetBattleRecvTime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  *a = Connect[fd].battle_recvtime;
  CONNECT_UNLOCK(fd);
}
// Shan End

#ifdef _ITEM_PILEFORTRADE
ANY_THREAD void CONNECT_setTradeList(int fd, int num) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].tradelist = num;
}
ANY_THREAD int CONNECT_getTradeList(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].tradelist;
}
#endif

ANY_THREAD void CONNECT_setTradeTmp(int fd, char *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].TradeTmp, sizeof(Connect[fd].TradeTmp), a);

  CONNECT_UNLOCK(fd);
}
ANY_THREAD void CONNECT_getTradeTmp(int fd, char *trademsg, int trademsglen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(trademsg, trademsglen, Connect[fd].TradeTmp);
  CONNECT_UNLOCK(fd);
}

ANY_THREAD void CONNECT_setTradecardcharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].joinpartycharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANY_THREAD int CONNECT_getTradecardcharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].joinpartycharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}

ANY_THREAD void CONNECT_setCloseRequest(int fd, int count) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  //    print("\n�ر���������Ϊ FILE:%s,LINE:%d ", file, line);
  CONNECT_LOCK(fd);
  Connect[fd].close_request = count;
  // Nuke
  //    print("\n�ر���������Ϊ %d ",fd);
  CONNECT_UNLOCK(fd);
}

/*------------------------------------------------------------
 * CAcheck �ʤɤ˻Ȥ���ؿ����ºݤ����롣
 * ����
 *  fd      int     �ե�����ǥ�������ץ�
 * �֤���
 *  �ʤ�
 ------------------------------------------------------------*/
ANY_THREAD void CAsend(int fd) {
  char buf[sizeof(Connect[0].CAbuf)];
  int bufuse = 0;

  if (CONNECT_getCAbuf(fd, buf, sizeof(buf), &bufuse) < 0)
    return;
  if (bufuse == 0)
    return;

  // print("\nshan--->(CAsend)->%s fd->%d", buf, fd);

  /*�Ǹ�Υǥ�ߥ� ',' ��'\0' �Ȥ�����*/
  buf[bufuse - 1] = '\0';
  lssproto_CA_send(fd, buf);

  CONNECT_setCAbufsiz(fd, 0);
}

/*------------------------------------------------------------
 * CA�����롣
 * ����
 * �֤���
 *  �ʤ�
 ------------------------------------------------------------*/
ANY_THREAD void CAcheck(void) {
  int i;
#ifdef _SYSTEM_SPEAD
  unsigned int interval_us = getCAsendinterval_ms() * 200;
#else
  unsigned int interval_us = getCAsendinterval_ms() * 1000;
#endif

  /* Connect�������з���֧�¾��պ����Ϸ�ئ�� */
  for (i = 0; i < ConnectLen; i++) {
    struct timeval t;
    if (!CONNECT_getUse_debug(i, 1008))
      continue;
    CONNECT_getLastCAsendtime(i, &t);
    if (time_diff_us(NowTime, t) > interval_us) {
      CAsend(i);
      CONNECT_setLastCAsendtime(i, &NowTime);
    }
  }
}
ANY_THREAD void CAflush(int charaindex) {
  int i;
  i = getfdFromCharaIndex(charaindex);
  if (i == -1)
    return;
  CAsend(i);
}

/*------------------------------------------------------------
 * CDbuf ���ɲä��롣
 * ����
 *  fd      int     �ե�����ǥ�������ץ�
 *  data    char*   �ǡ���
 *  size    int     �ǡ����Υ�����
 * �֤���
 *  ����    TRUE(1)
 *  ����    FALSE(0)
 ------------------------------------------------------------*/
ANY_THREAD BOOL CONNECT_appendCDbuf(int fd, char *data, int size) {
  CONNECT_LOCK(fd);

  if ((Connect[fd].CDbufsiz + size) >= sizeof(Connect[fd].CDbuf)) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  }
  memcpy(Connect[fd].CDbuf + Connect[fd].CDbufsiz, data, size);
  Connect[fd].CDbuf[Connect[fd].CDbufsiz + size] = ',';
  Connect[fd].CDbufsiz += (size + 1);
  CONNECT_UNLOCK(fd);
#ifdef _EPOLL
  // ModEpollOut(fd);
#endif
  return TRUE;
}

/*------------------------------------------------------------
 * CDcheck �ʤɤ˻Ȥ���ؿ����ºݤ����롣
 * ����
 *  fd      int     �ե�����ǥ�������ץ�
 * �֤���
 *  �ʤ�
 ------------------------------------------------------------*/
ANY_THREAD void CDsend(int fd) {
  char buf[sizeof(Connect[0].CAbuf)];
  int bufuse = 0;
  if (CONNECT_getCDbuf(fd, buf, sizeof(buf), &bufuse) < 0)
    return;
  if (bufuse == 0)
    return;
  buf[bufuse - 1] = '\0';
  lssproto_CD_send(fd, buf);
  CONNECT_setCDbufsiz(fd, 0);
}

/*------------------------------------------------------------
 * CD�����롣
 ------------------------------------------------------------*/
ANY_THREAD void CDcheck(void) {
  int i;
  unsigned int interval_us = getCDsendinterval_ms() * 1000;
  for (i = 0; i < ConnectLen; i++) {
    struct timeval t;
    if (!CONNECT_getUse_debug(i, 1082))
      continue;
    CONNECT_getLastCDsendtime(i, &t);
    if (time_diff_us(NowTime, t) > interval_us) {
      CDsend(i);
      CONNECT_setLastCDsendtime(i, &NowTime);
    }
  }
}

ANY_THREAD void CDflush(int charaindex) {
  int i;
  i = getfdFromCharaIndex(charaindex);
  if (i == -1)
    return;
  CDsend(i);
}

void chardatasavecheck(void) {
  int i;
  int interval = getCharSavesendinterval();
  static struct timeval chardatasavecheck_store;
  if (NowTime.tv_sec > (chardatasavecheck_store.tv_sec + 10)) {
    chardatasavecheck_store = NowTime;

#ifdef _FIX_SAVE_CHAR
    for (i = 0; i < CHAR_getPlayerMaxNum(); i++) {
      if (CHAR_CHECKINDEX(i) == FALSE)
        continue;
      int tv_sec = NowTime.tv_sec;
      if (CHAR_getWorkInt(i, CHAR_WORK_SAVE_CHAR) == 0) {
        CHAR_setWorkInt(i, CHAR_WORK_SAVE_CHAR, tv_sec);
      } else {
        if (tv_sec - CHAR_getWorkInt(i, CHAR_WORK_SAVE_CHAR) > interval) {
          CHAR_charSaveFromConnect(i, FALSE);
          CHAR_setWorkInt(i, CHAR_WORK_SAVE_CHAR, tv_sec);
        }
      }
    }
#else
    for (i = 0; i < ConnectLen; i++) {
      CONNECT_LOCK(i);
      if (Connect[i].use == TRUE && Connect[i].state == LOGIN &&
          NowTime.tv_sec - Connect[i].lastCharSaveTime.tv_sec > interval) {
        Connect[i].lastCharSaveTime = NowTime;
        CONNECT_UNLOCK(i);
        CHAR_charSaveFromConnect(CONNECT_getCharaindex(i), FALSE);
      } else {
        CONNECT_UNLOCK(i);
      }
    }
#endif
  } else {
    ;
  }
}

/*------------------------------------------------------------
 * fd �� valid ئ�ּ���������ëƩ����
 * ¦��
 *  fd          int         fd
 * ߯Ի��
 *  valid   TRUE(1)
 *  invalid FALSE(0)
 ------------------------------------------------------------*/
ANY_THREAD INLINE int CONNECT_checkfd(int fd) {
  if (0 > fd || fd >= ConnectLen) {
    return FALSE;
  }
  CONNECT_LOCK(fd);
  if (Connect[fd].use == FALSE) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  } else {
    CONNECT_UNLOCK(fd);
    return TRUE;
  }
}

ANY_THREAD int getfdFromCdkey(const char *cd) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && strcmp(Connect[i].cdkey, cd) == 0) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANY_THREAD int getfdFromCharaIndex(int charaindex) {
#if 1
  int ret;
  if (!CHAR_CHECKINDEX(charaindex))
    return -1;
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return -1;
  ret = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  if (ret < 0 || ret >= ConnectLen)
    return -1;
  return ret;
#else
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charaindex) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
#endif
}

ANY_THREAD int getcdkeyFromCharaIndex(int charaindex, char *out, int outlen) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charaindex) {
      snprintf(out, outlen, "%s", Connect[i].cdkey);
      CONNECT_UNLOCK(i);
      return 0;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANY_THREAD int getfdFromFdid(const int fdid) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].fdid == fdid) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANY_THREAD int getCharindexFromFdid(int fdid) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].fdid == fdid &&
        Connect[i].charaindex >= 0) {
      const int char_index = Connect[i].charaindex;
      CONNECT_UNLOCK(i);
      return char_index;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANY_THREAD int getFdidFromCharaIndex(int charind) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charind) {
      const int fdid = Connect[i].fdid;
      CONNECT_UNLOCK(i);
      return fdid;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

// 连接是否是命令行连接.
ANY_THREAD BOOL CONNECT_isCLI(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].ctype == CLI ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

// 连接是否是客户端连接.
ANY_THREAD BOOL CONNECT_isAC(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].ctype == AC ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

// 连接是否已经登录.
ANY_THREAD BOOL CONNECT_isUnderLogin(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == LOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

// 连接是否已经登录.
ANY_THREAD BOOL CONNECT_isWhileLogin(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == WHILELOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

// 连接是否已经登录.
ANY_THREAD BOOL CONNECT_isNOTLOGIN(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == NOTLOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANY_THREAD BOOL CONNECT_isLOGIN(const int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == LOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

void closeAllConnectionandSaveData(void) {
  int i;
  int num;
  /*  ����ʾ� accept ���ʤ��褦�ˤ���    */
  SERVSTATE_setCloseallsocketnum(0);
  /*  �����������    */
  for (i = 0; i < ConnectLen; i++) {
    if (CONNECT_getUse_debug(i, 1413) == TRUE) {
      BOOL clilogin = FALSE;
      if (CONNECT_isAC(i))
        continue;
      if (CONNECT_isCLI(i) && CONNECT_isLOGIN(i))
        clilogin = TRUE;
      CONNECT_endOne_debug(i);
      // Nuke +1 0901: Why close
      // print("closed in closeAllConnectionandSaveData");

      if (clilogin) {
        CONNECT_setUse(i, TRUE);
        CONNECT_setState(i, WHILECLOSEALLSOCKETSSAVE);
        SERVSTATE_incrementCloseallsocketnum();
      }
    }
  }
  num = SERVSTATE_getCloseallsocketnum();
  if (num == 0) {
    SERVSTATE_SetAcceptMore(-1);
  } else {
    SERVSTATE_SetAcceptMore(0);
  }
  print("\n��������������Ŀ:%d\n", num);
}

// andy_add 2003/02/12
void CONNECT_SysEvent_Loop(void) {
  static time_t checkT = 0;
  static int chikulatime = 0;

  int NowTimes = time(NULL);

  if (checkT != NowTimes && (checkT + 10) <= NowTimes) {
    int i;
    checkT = time(NULL);
    chikulatime++; // ÿ10��

    if (chikulatime > 10000)
      chikulatime = 0;
#ifdef _MO_RELOAD_NPC
    if (getReloadNpcTime() > 0) {
      if (chikulatime % getReloadNpcTime() == 0) {
        NPC_reloadNPC();
        MAPPOINT_resetMapWarpPoint(1);
        MAPPOINT_loadMapWarpPoint();
      }
    }
#endif
#ifdef _LUCK_STAR
    if (getLuckStarTime() > 0) {
      if (chikulatime % getLuckStarTime() == 0) { // ÿСʱ
        LuckStar();
      }
    }
#endif
    int playernum = CHAR_getPlayerMaxNum();
    int charaindex;
    for (charaindex = 0; charaindex < playernum; charaindex++) {
      if (!CHAR_CHECKINDEX(charaindex))
        continue;
#ifdef _CANCEL_STREET_VENDOR
      char token[256];
      if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) == 3) {
        if (CHAR_getWorkInt(charaindex, CHAR_WORK_STREET_VENDOR_TIME) <
            time(NULL) - 90) {
          int toindex = CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR_WHO);
          // �����趨Ϊ��̯,�����״̬
          CHAR_setWorkInt(charaindex, CHAR_WORKSTREETVENDOR, -1);
          CHAR_setWorkInt(toindex, CHAR_WORKSTREETVENDOR, 1);
          // �������ͼʾ
          CHAR_sendTradeEffect(charaindex, 0);
          CHAR_sendTradeEffect(toindex, 0);
          CHAR_setWorkInt(toindex, CHAR_WORKSTREETVENDOR_WHO, -1);
          CHAR_setWorkInt(charaindex, CHAR_WORKSTREETVENDOR_WHO, -1);
          lssproto_STREET_VENDOR_send(charaindex, "C|");
          CHAR_talkToCli(charaindex, -1, "�鿴��̯ʱ�����,�����Զ�ȡ����Ĳ鿴",
                         CHAR_COLORYELLOW);
        }
      }
#endif
      if (chikulatime % 6 == 0) { // ÿ60��

#ifdef _PETSKILL_BECOMEPIG
        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { // ���������״̬

          if ((CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 1) <=
              0) { // ����ʱ�������
            CHAR_setInt(charaindex, CHAR_BECOMEPIG, 0);

            if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) { // ������ս��״̬��
              CHAR_setInt(charaindex, CHAR_BECOMEPIG, -1); // ��������״̬
              CHAR_complianceParameter(charaindex);
              CHAR_sendCToArroundCharacter(
                  CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
              CHAR_send_P_StatusString(charaindex,
                                       CHAR_P_STRING_BASEBASEIMAGENUMBER);
              CHAR_talkToCli(charaindex, -1, "������ʧЧ�ˡ�", CHAR_COLORWHITE);
            }
          } else {
            CHAR_setInt(charaindex, CHAR_BECOMEPIG,
                        CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 10);
          }
        }
#endif

#ifdef _SPECIAL_MAP
        int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
        int x;
        for (x = 0; x < 32; x++) {
          if (floor == getSpecialMap(x)) {
            break;
          }
        }
        if (x < 32)
#endif
        {
#ifdef _BOUND_TIME
          if (CHAR_getInt(charaindex, CHAR_BOUNDTIME) <= NowTimes) {
            int fl = 0, x = 0, y = 0;
            CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
                                  &fl, &x, &y);
            CHAR_warpToSpecificPoint(charaindex, fl, x, y);

            CHAR_talkToCli(charaindex, -1,
                           "���������Ȩʱ���ѵ������԰����ͻؼ�¼�"
                           "㣡",
                           CHAR_COLORYELLOW);
          }
#endif
        }
      }

      if (chikulatime % 30 == 0) { // ÿ300��

#ifdef _ITEM_ADDEXP // vincent ��������
#ifdef _PET_ADD_EXP
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          int exptime;
          if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP) > 0) {
            exptime =
                CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME) - 300;

            if (exptime <= 0) {
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXP, 0);
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
              CHAR_talkToCli(charaindex, -1, "����ѧϰ�����������ʧ��!",
                             CHAR_COLORYELLOW);
            } else {
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
              // print("\n ���ADDEXPTIME %d ", exptime);

              if ((exptime % (60 * 60)) < 300 && exptime >= (60 * 60)) {
                char msg[1024];
                sprintf(msg, "��������ѧϰ���������ʣ��Լ %d Сʱ��",
                        (int)(exptime / (60 * 60)));
                CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
              }
            }
          }
          int i;
          for (i = 0; i < CHAR_MAXPETHAVE; i++) {
            int petindex = CHAR_getCharPet(charaindex, i);
            if (!CHAR_CHECKINDEX(petindex))
              continue;
            if (CHAR_getWorkInt(petindex, CHAR_WORKITEM_ADDEXP) > 0) {
              exptime =
                  CHAR_getWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME) - 300;

              if (exptime <= 0) {
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXP, 0);
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME, 0);
                CHAR_talkToCli(charaindex, -1, "����ѧϰ�����������ʧ��!",
                               CHAR_COLORYELLOW);
              } else {
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
                // print("\n ���ADDEXPTIME %d ", exptime);

                if ((exptime % (60 * 60)) < 300 && exptime >= (60 * 60)) {
                  char msg[1024];
                  sprintf(msg, "����%s����ѧϰ���������ʣ��Լ %d Сʱ��",
                          CHAR_getChar(petindex, CHAR_NAME),
                          (int)(exptime / (60 * 60)));
                  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
                }
              }
            }
          }
        }
#else
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP) > 0 &&
            CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          int exptime;
          exptime = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME) - 300;

          if (exptime <= 0) {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXP, 0);
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
            CHAR_talkToCli(charaindex, -1, "����ѧϰ�����������ʧ��!",
                           CHAR_COLORYELLOW);
          } else {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
            // print("\n ���ADDEXPTIME %d ", exptime);

            if ((exptime % (60 * 60)) < 300 && exptime >= (60 * 60)) {
              char msg[1024];
              sprintf(msg, "����ѧϰ���������ʣ��Լ %d Сʱ��",
                      (int)(exptime / (60 * 60)));
              // sprintf( msg, "����ѧϰ���������ʣ��Լ %d �֡�", (int)(exptime/(60))
              // );
              CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
            }
          }
          CHAR_setInt(charaindex, CHAR_ADDEXPPOWER,
                      CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP));
          CHAR_setInt(charaindex, CHAR_ADDEXPTIME,
                      CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME));
        }
#endif

#endif
#ifdef _ITEM_METAMO
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) < NowTime.tv_sec &&
            CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) != 0) {
          CHAR_setWorkInt(charaindex, CHAR_WORKITEMMETAMO, 0);
          CHAR_setWorkInt(charaindex, CHAR_WORKNPCMETAMO,
                          0); // ��npc�Ի���ı���ҲҪ�����
          CHAR_complianceParameter(charaindex);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
          CHAR_send_P_StatusString(charaindex,
                                   CHAR_P_STRING_BASEBASEIMAGENUMBER);
          CHAR_talkToCli(charaindex, -1, "����ʧЧ�ˡ�", CHAR_COLORWHITE);
        }

#endif
#ifdef _ITEM_TIME_LIMIT
        ITEM_TimeLimit(
            charaindex); // (�ɿ���) shan time limit of item. code:shan

#endif

      } //%30

#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_NONE) { // ������ս��״̬��

        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { // ���������״̬
          char temp[256];
          CHAR_setInt(charaindex, CHAR_BECOMEPIG,
                      CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 1);
          sprintf(temp, "����ʱ��:%d��",
                  CHAR_getInt(charaindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(charaindex, -1, temp, CHAR_COLORWHITE);
        }
      }

#endif
      // 10��
#ifdef _MAP_TIME
      if (CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) > 0 &&
          CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
              BATTLE_CHARMODE_NONE) {
        CHAR_setWorkInt(charaindex, CHAR_WORK_MAP_TIME,
                        CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) - 10);

        if (CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) <= 0) {
          // ʱ�䵽��,�������
          CHAR_talkToCli(charaindex, -1, "����Ϊ�ܲ��˸��ȶ������������ѷ���ڡ�",
                         CHAR_COLORRED);
          CHAR_warpToSpecificPoint(charaindex, 30008, 39, 38);
          CHAR_setInt(charaindex, CHAR_HP, 1);
          CHAR_AddCharm(charaindex, -3);
          CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP);
          CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_CHARM);
        } else {
          char szMsg[64];
          sprintf(szMsg, "������ȵĻ�������ֻ���ٴ� %d �롣",
                  CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME));
          CHAR_talkToCli(charaindex, -1, szMsg, CHAR_COLORRED);
        }
      }
#endif
    }

    for (i = 0; i < ConnectLen; i++) {
#ifdef _OTHER_SAAC_LINK
      if ((Connect[i].use) && (CONNECT_getCtype(i) != AC))
#else
      if ((Connect[i].use) && (i != acfd))
#endif
        if (!CONNECT_getUse(i))
          continue;
      if (!CHAR_CHECKINDEX(Connect[i].charaindex))
        continue;

      {
#ifdef _NEW_AUTO_PK
        {
          if (getAutoPkTime() > -1) {
            if (getBAward() == FALSE) {
              struct tm *p;
              time_t timep;
              time(&timep);
              p = localtime(&timep); /*ȡ�õ���ʱ��*/

              if (getBAward() == FALSE) {
                if ((getAutoPkTime() == p->tm_hour) && (p->tm_min == 0)) {
                  AutoPk_PKTimeSet(15);
                  AutoPk_GetAwardStr();
                }
              }
            }
          }
        }
#endif

#ifndef _USER_CHARLOOPS
        // here ԭ������
        if (Connect[i].stayencount) {
          if (Connect[i].BDTime < time(NULL)) {
            if (CHAR_getWorkInt(Connect[i].charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) {
              lssproto_EN_recv(i, CHAR_getInt(Connect[i].charaindex, CHAR_X),
                               CHAR_getInt(Connect[i].charaindex, CHAR_Y));
              Connect[i].BDTime = time(NULL);
            }
          }
        }

#endif
#ifdef _CHIKULA_STONE
        if (chikulatime % 3 == 0 && getChiStone(i) > 0) { // �Զ���Ѫ
          CHAR_AutoChikulaStone(Connect[i].charaindex, getChiStone(i));
        }

#endif

        if (chikulatime % 6 == 0) { // ˮ����״̬

#ifdef _STATUS_WATERWORD
          CHAR_CheckWaterStatus(Connect[i].charaindex);
#endif
          // Nuke 0626: No enemy

          if (Connect[i].noenemy > 0) {
            Connect[i].noenemy--;

            if (Connect[i].noenemy == 0) {
              CHAR_talkToCli(CONNECT_getCharaindex(i), -1, "�ػ���ʧ�ˡ�",
                             CHAR_COLORWHITE);
            }
          }
        }

        // ÿ10��
#ifdef _TYPE_TOXICATION // �ж�
        if (Connect[i].toxication > 0) {
          CHAR_ComToxicationHp(Connect[i].charaindex);
        }

#endif
        // Nuke 0624 Avoid Useless Connection
        if (Connect[i].state == NOTLOGIN) {
          Connect[i].cotime++;

          if (Connect[i].cotime > 30) {
            print("LATE");
#ifdef _NETLOG_
            char cdkey[16];
            char charname[32];
            CONNECT_getCharname(CONNECT_getCharaindex(i), charname, 32);
            CONNECT_getCdkey(CONNECT_getCharaindex(i), cdkey, 16);
            char token[128];
            sprintf(token, "���û�е�½��Ϸ����T�� ");
            LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__,
                       token);
#endif

            CONNECT_endOne_debug(i);
          }
        } else {
          Connect[i].cotime = 0;
        }

        if ((Connect[i].nu <= 22)) {

          if (Connect[i].nu <= 0) {
            Connect[i].nu_decrease++;

            if (Connect[i].nu_decrease >= 30)
              Connect[i].nu_decrease = 30;

            if (Connect[i].nu_decrease > 22)
              logSpeed(i);
          } else {
            Connect[i].nu_decrease -= 1;

            if (Connect[i].nu_decrease < 0)
              Connect[i].nu_decrease = 0;
          }
          lssproto_NU_send(i, 0);
          /*
                    r = 22 - Connect[ i ].nu_decrease;
                    r = ( r >= 15 ) ? r : 15;
                    lssproto_NU_send( i, r );
                    Connect[ i ].nu += r;
          */
        }
      }
    }
  }
}

// Nuke 0126: Resource protection
int isThereThisIP(unsigned long ip) {
  int i;
  unsigned long ipa;

  for (i = 0; i < ConnectLen; i++)
    if (!Connect[i].use)
      continue;

  if (Connect[i].state == NOTLOGIN ||
      Connect[i].state == WHILEDOWNLOADCHARLIST) {
    memcpy(&ipa, &Connect[i].sin.sin_addr, 4);

    if (ipa == ip)
      return 1;
  }

  return 0;
}

int player_online = 0;
int player_maxonline = 0;

#ifdef _KEEP_UP_NO_LOGIN
char keepupnologin[256] = "";
#endif
struct timeval speedst, speedet;
SINGLETHREAD BOOL netloop_faster(void) {
  static unsigned int total_item_use = 0;
  static int petcnt = 0;
  struct timeval st, et;
  unsigned int looptime_us;
  looptime_us = getOnelooptime_ms() * 1000;

  int ret, loop_num;
  struct timeval tmv; /*timeval*/
  static int fdremember = 0;
  int acceptmore = SERVSTATE_getAcceptmore();

  //    static unsigned int nu_time=0;
  unsigned int casend_interval_us, cdsend_interval_us;
  fd_set rfds, wfds, efds;
  int allowerrornum = getAllowerrornum();
  int acwritesize = getAcwriteSize();

#ifdef _AC_PIORITY
  static int flag_ac = 1;
  static int fdremembercopy = 0;
  static int totalloop = 0;
  static int totalfd = 0;
  static int totalacfd = 0;
  static int counter = 0;
#endif

#ifdef _SYSTEM_SPEAD
  casend_interval_us = getCAsendinterval_ms() * 200;
  cdsend_interval_us = getCDsendinterval_ms() * 200;
#else
  casend_interval_us = getCAsendinterval_ms() * 1000;
  cdsend_interval_us = getCDsendinterval_ms() * 1000;
#endif
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);
  FD_SET(bindedfd, &rfds);
  FD_SET(bindedfd, &wfds);
  FD_SET(bindedfd, &efds);
  tmv.tv_sec = tmv.tv_usec = 0;
  ret = select(bindedfd + 1, &rfds, &wfds, &efds, &tmv);
  if (ret < 0 && (errno != EINTR)) {
    ;
  }
  if (ret > 0 && FD_ISSET(bindedfd, &rfds)) {
    struct sockaddr_in sin;
    int addrlen = sizeof(struct sockaddr_in);
    int sockfd;

    sockfd = accept(bindedfd, (struct sockaddr *)&sin, &addrlen);

    SetTcpBuf(sockfd, &rfds);

    if (sockfd == -1 && errno == EINTR) {
      print("accept err:%s\n", strerror(errno));
    } else if (sockfd > 1000) {
      print("sockfd:%d\n", sockfd);
      close(sockfd);
    } else if (sockfd != -1) {
      unsigned long sinip;
      int cono = 1, from_acsv = 0;
      if (cono_check & CONO_CHECK_LOGIN) {
        if (StateTable[WHILELOGIN] + StateTable[WHILELOGOUTSAVE] >
                QUEUE_LENGTH1 ||
            StateTable[WHILEDOWNLOADCHARLIST] > QUEUE_LENGTH2) {
          print("err State[%d,%d,%d]!!\n", StateTable[WHILELOGIN],
                StateTable[WHILELOGOUTSAVE], StateTable[WHILEDOWNLOADCHARLIST]);

          CONNECT_checkStatecount(WHILEDOWNLOADCHARLIST);
          cono = 0;
        }
      }
      if (cono_check & CONO_CHECK_ITEM)
        if (total_item_use >= MAX_item_use) {
          print("��ʹ����Ʒ������!!");
          cono = 0;
        }
      if (cono_check & CONO_CHECK_PET)
        if (petcnt >= CHAR_getPetMaxNum()) {
          print("��ʹ�ó���������!!");
          cono = 0;
        }

      // print("CO");

      {
        float fs = 0.0;
        if ((fs = ((float)Connect[acfd].rbuse / AC_RBSIZE)) > 0.6) {
          print("andy AC rbuse: %3.2f [%4d]\n", fs, Connect[acfd].rbuse);
          if (fs > 0.78)
            cono = 0;
        }
      }

      memcpy(&sinip, &sin.sin_addr, 4);
      // Nuke *1 0126: Resource protection

      if ((cono == 0) || (acceptmore <= 0) || isThereThisIP(sinip)) {
        // Nuke +2 Errormessage
        char mess[64] = "E�ŷ���æ���У����Ժ����ԡ�";
        if (!from_acsv)
          write(sockfd, mess, strlen(mess) + 1);
        print("accept but drop[cono:%d,acceptmore:%d]\n", cono, acceptmore);
        close(sockfd);
      }
#ifdef _SAME_IP_ONLINE_NUM
      else if (SameIpOnlineNum(sinip) == 1) {
        char mess[64];
        sprintf(mess, "E����IP����%d�������ˣ����������½���Ծ�ά����Ϸƽ�⣡\n",
                getSameIpOnlineNum());
        write(sockfd, mess, strlen(mess) + 1);
        close(sockfd);
      }
#endif
#ifdef _KEEP_UP_NO_LOGIN
      else if (strlen(keepupnologin) > 0) {
        char mess[64];
        sprintf(mess, "E%s", keepupnologin);
        write(sockfd, mess, strlen(mess) + 1);
        close(sockfd);
      }
#endif
      else if (sockfd < ConnectLen) {
        char mess[64] = "A"; // Nuke +2 Errormessage
        if (bNewServer) {
          mess[0] = _SA_VERSION; // 7.0

        } else
          mess[0] = '$';

        // char mess[1024]="E�ŷ���æ���У����Ժ����ԡ�";
        if (!from_acsv) {
#ifdef _NO_FULLPLAYER_ATT
          if (sockfd - player_online >= getNoFullPlayer()) {
            time_t curtime;
            struct tm *p;
            time(&curtime);
            p = localtime(&curtime);
            curtime = mktime(p);
            if (curtime - atttime > getNoFullTime()) {
              atttime = curtime;
              int j, ipa, ipb, ipc, ipd;
              char ip[32];
              char systemstr[256];
              unsigned long tmpip;
              for (j = 12; j < ConnectLen; j++) {
                tmpip = CONNECT_get_userip(j);
                if (j != acfd && j != sockfd && CONNECT_getUse(j) &&
                    CONNECT_getState(j) == NOTLOGIN) {
                  ipa = (tmpip % 0x100);
                  tmpip = tmpip / 0x100;
                  ipb = (tmpip % 0x100);
                  tmpip = tmpip / 0x100;
                  ipc = (tmpip % 0x100);
                  tmpip = tmpip / 0x100;
                  ipd = (tmpip % 0x100);
                  sprintf(ip, "%d.%d.%d.%d", ipa, ipb, ipc, ipd);
                  if (strcmp(ip, getNoAttIp(0)) != 0 &&
                      strcmp(ip, getNoAttIp(1)) != 0 &&
                      strcmp(ip, getNoAttIp(2)) != 0 &&
                      strcmp(ip, getNoAttIp(3)) != 0 &&
                      strcmp(ip, getNoAttIp(4)) != 0) {
                    if (getFengType() == 1) {
                      if (sasql_query_online_ip(ip) == 0) {
                        CONNECT_endOne_debug(j);
                      }
                    } else if (getFengType() == 2) {
                      CONNECT_endOne_debug(j);
                    } else {
                      CONNECT_endOne_debug(j);
                    }
                  }
                }
              }
            }
          }
#endif
          send(sockfd, mess, strlen(mess) + 1, 0);
        }
        initConnectOne(sockfd, &sin, addrlen);

        if (getNodelay()) {
          int flag = 1;
          int result = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                                  (char *)&flag, sizeof(int));

          if (result < 0) {
            close(sockfd);
            print("setsockopt TCP_NODELAY failed:%s\n", strerror(errno));
          } else {
            print("NO");
          }
        }
      } else {
        int i;
        for (i = 0; i < ConnectLen; i++) {
          if (Connect[i].use == FALSE)
            continue;
          if (i == acfd)
            continue;
          if (Connect[i].charaindex != -1)
            continue;
          char mess[64] = "E�ŷ�����æ�����Ժ����ԡ�";
          if (!from_acsv)
            write(i, mess, strlen(mess) + 1);
          close(i);
          // Nuke +1 0901: Why close
        }
      }
    }
  }
  loop_num = 0;
  gettimeofday(&st, NULL);

  while (1) {

    int j;

    // ttom+1 for the debug
    static int i_tto = 0;
    static int i_timeNu = 0;

    gettimeofday(&et, NULL);
    if (time_diff_us(et, st) >= looptime_us) // ִ��ÿ����0.1����Ҫ���ĵĹ���
    {
#define LOOP_NUM_ADD_CREDIT 5
#define CREDIT_SPOOL 3

      switch (acceptmore) {
      case -1:
        print("#");
        break;
      case 0:
        print("$");

        if (!b_first_shutdown) {
          b_first_shutdown = TRUE;
          i_shutdown_time = SERVSTATE_getLimittime();
          print("\n �رշ�����ʱ��=%d", i_shutdown_time);
        }
        break;
      default: {
        static int i_counter = 0;
        // Syu ADD ��ʱ��ȡAnnounce
        static int j_counter = 0;
        // Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
        static int h_counter = 0;
        // �������ļ�ʱ��
        static long total_count = 0;

#ifdef _AUTO_PK
        static int h_autopk = 0;
#endif

#ifdef _LOOP_ANNOUNCE
        static int loop_counter = 0;
#endif
        int i;
        int item_max;
        if (i_counter > 10) {
          player_online = 0;
#ifdef _AC_PIORITY
          totalloop = 0;
          totalfd = 0;
          totalacfd = 0;
#endif
          i_counter = 0;
          item_max = ITEM_getITEM_sItemNum();
          total_item_use = ITEM_getITEM_sUseItemNum();
          for (i = 0; i < ConnectLen; i++) {
            if ((Connect[i].use) && (i != acfd)) {
              if (CHAR_CHECKINDEX(Connect[i].charaindex))
                player_online++;
            }
          }
          if (player_online > player_maxonline) {
            player_maxonline = player_online;
          }
          {
            int max, min;
            char buff1[512];
            char szBuff1[256];
#ifdef _ASSESS_SYSEFFICACY
            {
              float TVsec;
              ASSESS_getSysEfficacy(&TVsec);
              sprintf(szBuff1, "Sys:[%2.4f] \n", TVsec);
            }
#endif

            memset(buff1, 0, sizeof(buff1));
            CHAR_getCharOnArrayPercentage(1, &max, &min, &petcnt);
            sprintf(buff1,
                    "\n�������=%d �������=%d ����=%d ��Ʒ=%d �ʼ�:%d ս��:%d %s",
                    player_online, luaplayernum, petcnt, total_item_use,
                    PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(),
                    szBuff1);

            buff1[strlen(buff1) + 1] = 0;
            print("%s", buff1);

            gettimeofday(&speedet, NULL);
            float speedtime = time_diff_us(speedet, speedst) / 1000000.0;
            sprintf(buff1,
                    "�ϴ�:[%2.3f K]ƽ��[%2.3f K]"
                    "����:[%2.3f K]ƽ��[%2.3f K]"
                    "��ʱ%2.3f��\n",
                    sendspeed / 1024.0, sendspeed / speedtime / 1024.0,
                    recvspeed / 1024.0, recvspeed / speedtime / 1024.0,
                    speedtime);
            gettimeofday(&speedst, NULL);
            sendspeed = 0;
            recvspeed = 0;

            buff1[strlen(buff1) + 1] = 0;
            print("%s", buff1);

#ifdef _ASSESS_SYSEFFICACY_SUB
            {
              float TVsec;
              ASSESS_getSysEfficacy_sub(&TVsec, 1);
              sprintf(szBuff1, "NT:[%2.4f] ", TVsec);
              strcpy(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 2);
              sprintf(szBuff1, "NG:[%2.4f] ", TVsec);
              strcat(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 3);
              sprintf(szBuff1, "BT:[%2.4f] ", TVsec);
              strcat(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 4);
              sprintf(szBuff1, "CH:[%2.4f] \n", TVsec);
              strcat(buff1, szBuff1);

              buff1[strlen(buff1) + 1] = 0;
              print("%s.", buff1);
            }
#endif
          }
#ifdef _TIME_TICKET
          check_TimeTicket();
#endif
        }
#ifdef _LOOP_ANNOUNCE
        if (loop_counter > 60 * getLoopAnnounceTime() &&
            getLoopAnnounceMax() > 0) {
          int i;
          int playernum = CHAR_getPlayerMaxNum();
          static int index;

          for (i = 0; i < playernum; i++) {
            if (CHAR_getCharUse(i) != FALSE) {
              char buff[36];
              snprintf(buff, sizeof(buff), "%s���档", getGameservername());
              CHAR_talkToCli(i, -1, buff, CHAR_COLORYELLOW);
              CHAR_talkToCli(i, -1,
                             getLoopAnnounce(index % getLoopAnnounceMax()),
                             CHAR_COLORYELLOW);
            }
          }
          index++;
          loop_counter = 0;
        }
#endif

#ifdef _AUTO_PK
        if (AutoPk_PKTimeGet() > -1) {
          if (h_autopk > 60) {
            h_autopk = 0;
            AutoPk_PKSystemInfo();
          }
        }
#endif

        // Syu ADD ��ʱ��ȡAnnounce
        if (j_counter > 60 * 60) { // 6000 Լ 600��=10����
          j_counter = 0;
          print("\n��ʱ��ȡ����");
          LoadAnnounce();
        }

        /*
        #ifdef _ALLDOMAN     // Syu ADD ���а�NPC
        // Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
        if ( h_counter > 60*60 ){//36000 Լ 3600��=60����
        h_counter=0;
        print("\nSyu log LoadHerolist");
        saacproto_UpdataStele_send ( acfd , "FirstLoad", "LoadHerolist" , "����"
        , 0 , 0 , 0 , 999 ) ;
        }
        #endif
        */
        if (i_timeNu != time(NULL)) // ÿ����ִ��һ��
        {
          i_timeNu = time(NULL);
#ifdef _PLAYER_DIY_MAP
          if (total_count % 60 == 0) { // ÿ����ִ��
            int i;
            for (i = 0; i < Player_Diy_Map_NUM; i++) {
              if (PlayerDiyMap[i].exp > 0) {
                int playernum = 0;
                int j;
                for (j = 0; j < CHAR_getPlayerMaxNum(); j++) {
                  if (CHAR_CHECKINDEX(j) == FALSE)
                    continue;
                  if (CHAR_getInt(j, CHAR_FLOOR) == PlayerDiyMap[i].ff) {
                    playernum++;
                  }
                }
                if (playernum > 0) {
                  for (j = 0; j < CHAR_getPlayerMaxNum(); j++) {
                    if (CHAR_CHECKINDEX(j) == FALSE)
                      continue;
                    if (CHAR_getInt(j, CHAR_FLOOR) == PlayerDiyMap[i].ff) {
                      int UpLevel;
                      char token[256];
                      CHAR_AddMaxExp(j, PlayerDiyMap[i].exp * playernum);
                      sprintf(token, "��ϲ���� %d ����!\n",
                              PlayerDiyMap[i].exp * playernum);
                      CHAR_talkToCli(j, -1, token, CHAR_COLORGREEN);
                      CHAR_send_P_StatusString(j, CHAR_P_STRING_EXP);

                      UpLevel = CHAR_LevelUpCheck(j, -1);
                      if (UpLevel > 0) {
                        int work;
#ifdef _SKILLUPPOINT_CF
                        CHAR_setInt(j, CHAR_SKILLUPPOINT,
                                    CHAR_getInt(j, CHAR_SKILLUPPOINT) +
                                        UpLevel * getSkup());
#else
                        CHAR_setInt(j, CHAR_SKILLUPPOINT,
                                    CHAR_getInt(j, CHAR_SKILLUPPOINT) +
                                        UpLevel * 3);
#endif
                        work = CHAR_getInt(j, CHAR_CHARM);
                        work += CH_FIX_PLAYERLEVELUP;
                        CHAR_setInt(j, CHAR_CHARM, min(100, work));
                        CHAR_complianceParameter(j);
                        CHAR_sendStatusString(j, "P");
                        CHAR_Skillupsend(j);
                        CHAR_sendCToArroundCharacter(
                            CHAR_getWorkInt(j, CHAR_WORKOBJINDEX));
                        CHAR_PartyUpdate(j, CHAR_N_STRING_LV);
#ifndef _NET_REDUCESEND
                        CHAR_send_DpDBUpdate_AddressBook(j, TRUE);
#endif
                        sprintf(token, "�����ڵȼ�Ϊ %d ��!\n",
                                CHAR_getInt(j, CHAR_LV));
                        CHAR_talkToCli(j, -1, token, CHAR_COLORGREEN);
                      }
                      int k;
                      for (k = 0; k < CHAR_MAXPETHAVE; k++) {
                        int petindex = CHAR_getCharPet(j, k);
                        if (CHAR_CHECKINDEX(petindex) == FALSE)
                          continue;

                        CHAR_AddMaxExp(petindex,
                                       PlayerDiyMap[i].exp * playernum);
                        UpLevel = CHAR_LevelUpCheck(petindex, j);

                        if (UpLevel > 0) {
                          int l;
                          for (l = 0; l < UpLevel; l++) {
                            CHAR_PetLevelUp(petindex);
                            CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
                          }
                          sprintf(token, "��ĳ��� %s %d ��!\n",
                                  CHAR_getChar(petindex, CHAR_NAME),
                                  CHAR_getInt(petindex, CHAR_LV));
                          CHAR_talkToCli(j, -1, token, CHAR_COLORGREEN);
                        }
                        CHAR_complianceParameter(petindex);
                        snprintf(token, sizeof(token), "K%d", k);
                        CHAR_sendStatusString(j, token);
                      }
                    }
                  }
                }
              }
            }
          }
#endif
#ifdef _DEL_DROP_GOLD
          if (total_count % 60 == 0) { // ÿ����ִ��
            GOLD_DeleteTimeCheckLoop();
          }
#endif
#ifdef _ALLBLUES_LUA_1_5
          if (total_count % 60 == 0) { // ÿ����ִ��
            NetLoopFunction();
          }
#endif

#ifdef _DEL_DROP_PET
          if (total_count % 60 == 0) { // ÿ����ִ��
            int objindex;
            int objmaxnum = OBJECT_getNum();

            for (objindex = 0; objindex < objmaxnum; objindex++) {
              if (CHECKOBJECT(objindex) == FALSE)
                continue;
              if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
                int petindex;
                petindex = OBJECT_getIndex(objindex);
                if (CHAR_CHECKINDEX(petindex) == FALSE)
                  continue;
                if (CHECKOBJECT(petindex) == FALSE)
                  continue;
                if (CHAR_getInt(petindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
                  continue;
                if (CHAR_getInt(petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE)
                  continue;
                if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWMODE) ==
                    CHAR_PETFOLLOW_NOW)
                  continue;
                PET_CHECKFreePetIsIt(petindex);
              }
            }
          }
#endif

          // if( total_count % 60*10 == 0 ) { //ÿ10����ִ��
          // }

          // if( total_count % 60*60 == 0 ) { //ÿ60����ִ��
          // }

#ifdef _AUTO_PK
          if (AutoPk_PKTimeGet() > 0)
            h_autopk++;
#endif
          i_counter++;
          // Syu ADD ��ʱ��ȡAnnounce
          j_counter++;
          // Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
          h_counter++;

          total_count++;

#ifdef _LOOP_ANNOUNCE
          loop_counter++;
#endif
        }
      }

        if ((i_tto % 60) == 0) {
          i_tto = 0;
          print(".");
        }
        i_tto++;

        // andy add 2003/0212------------------------------------------
        CONNECT_SysEvent_Loop();

        //------------------------------------------------------------
      } // switch()

#ifdef _AC_PIORITY
      if (flag_ac == 2)
        fdremember = fdremembercopy;

      flag_ac = 1;

      totalloop++;

#endif
      break; // Break while
    }        // if(>0.1sec)

    loop_num++;

#ifdef _AC_PIORITY
    switch (flag_ac) {
    case 1:
      fdremembercopy = fdremember;
      fdremember = acfd;
      flag_ac = 2;
      break;
    case 2:
      counter++;

      if (counter >= 3) {
        counter = 0;
        fdremember = fdremembercopy + 1;
        flag_ac = 0;
      }

      break;
    default:
      fdremember++;
      break;
    }

#else
    fdremember++;

#endif

    if (fdremember == ConnectLen)
      fdremember = 0;

    if (Connect[fdremember].use == FALSE)
      continue;

    if (Connect[fdremember].state == WHILECLOSEALLSOCKETSSAVE)
      continue;

    if (Connect[fdremember].state == NULLCONNECT) {
      time_t new_t;
      time(&new_t);
      new_t -= initTime;
      if (new_t > 60) {
        if (fdremember != acfd) {
          if (Connect[fdremember].connecttime + 15 < time(NULL)) {
#ifdef _NETLOG_
            char cdkey[16];
            char charname[32];
            CONNECT_getCharname(fdremember, charname, 32);
            CONNECT_getCdkey(fdremember, cdkey, 16);
            char token[128];
            sprintf(token, "û�е�½����һֱ�ȴ�T��");
            LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__,
                       token);
#endif
            CONNECT_endOne_debug(fdremember);
            continue;
          }
        }
      }
    }
#ifdef _AC_PIORITY
    totalfd++;

    if (fdremember == acfd)
      totalacfd++;

#endif
    /* read select */
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    FD_SET(fdremember, &rfds);
    FD_SET(fdremember, &wfds);
    FD_SET(fdremember, &efds);
    tmv.tv_sec = tmv.tv_usec = 0;
    ret = select(fdremember + 1, &rfds, &wfds, &efds, &tmv);

    if (ret > 0 && FD_ISSET(fdremember, &rfds)) {
      errno = 0;
      char buf[1024 * 128];
      memset(buf, 0, sizeof(buf));

      ret = read(fdremember, buf, sizeof(buf));

      if (ret > 0 && sizeof(buf) <= ret) {
#ifdef _OTHER_SAAC_LINK
        print("��ȡ(%s)���峤��:%d - %d !!\n",
              (CONNECT_getCtype(fdremember) == AC) ? "SAAC" : "����", ret,
              sizeof(buf));
#else
        print("��ȡ(%s)���峤��:%d - %d !!\n",
              (fdremember == acfd) ? "SAAC" : "����", ret, sizeof(buf));
#endif
      }

      if ((ret == -1 && errno != EINTR) || ret == 0) {
#ifdef _OTHER_SAAC_LINK
        if (CONNECT_getCtype(fdremember) == AC)
#else
        if (fdremember == acfd)
#endif
        {
          print("��ȡ����:%d %s\n", ret, strerror(errno));
          print("gmsv��acsvʧȥ����! �쳣��ֹ...\n");
          sigshutdown(-1);
          exit(1);
        }
#ifdef _OTHER_SAAC_LINK
        else if (CONNECT_getCtype(fdremember) == SQL) {
          print("����������ʧȥ����...\n");
          CONNECT_endOne_debug(fdremember);
          osfd = -1;
        }
#endif
        else {
          if (ret == -1) {
            //      print( "��ȡ����: %d %d %s \n", ret, errno, strerror(
            //      errno));
          }
#ifdef _NETLOG_
          char cdkey[16];
          char charname[32];
          CONNECT_getCharname(fdremember, charname, 32);
          CONNECT_getCdkey(fdremember, cdkey, 16);
          char token[128];
          sprintf(token, "read��ȡ����: %d %d %s \n", ret, errno,
                  strerror(errno));
          LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif

          CONNECT_endOne_debug(fdremember);
          continue;
        }
      } else {
        if (appendRB(fdremember, buf, ret) == -2 && getErrUserDownFlg() == 1) {
#ifdef _NETLOG_
          char cdkey[16];
          char charname[32];
          CONNECT_getCharname(fdremember, charname, 32);
          CONNECT_getCdkey(fdremember, cdkey, 16);
          char token[128];
          sprintf(token, "appendRB����");
          LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif

          CONNECT_endOne_debug(fdremember);
          continue;
        } else {
#ifdef _OTHER_SAAC_LINK
          if (CONNECT_getCtype(fdremember) != AC)
#else
          if (fdremember != acfd)
#endif
          {
            recvspeed += ret;
          }
          Connect[fdremember].lastreadtime = NowTime;
          Connect[fdremember].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;
          Connect[fdremember].packetin = 30;
        }
      }

    } else if (ret < 0 && errno != EINTR) {
#ifdef _OTHER_SAAC_LINK
      if (CONNECT_getCtype(fdremember) == AC &&
          CONNECT_getCtype(fdremember) != SQL)
#else
      if (fdremember != acfd)
#endif
      {
        //  print( "\n��ȡ���Ӵ���:%d %s\n", errno, strerror( errno ));
        continue;
      }
    }

    for (j = 0; j < 3; j++) {
      memset(rbmess, 0, sizeof(rbmess));

      if (GetOneLine_fix(fdremember, rbmess, sizeof(rbmess)) == FALSE)
        continue;

      if (!((rbmess[0] == '\r' && rbmess[1] == '\n') || rbmess[0] == '\n')) {
#ifdef _OTHER_SAAC_LINK
        if (CONNECT_getCtype(fdremember) == AC)
#else
        if (fdremember == acfd)
#endif
        {

#ifdef _DEBUG
          printf("��ȡSAAC����:%s\n", rbmess);
#endif
          if (saacproto_ClientDispatchMessage(fdremember, rbmess) < 0) {
            print("\nSAAC���������ݳ���!!!\n");
          }
        }
#ifdef _OTHER_SAAC_LINK
        else if (CONNECT_getCtype(fdremember) == SQL) {
#ifdef _DEBUG
          printf("��ȡ�������:%s\n", rbmess);
#endif
          if (saacproto_ClientDispatchMessage(fdremember, rbmess) < 0) {
            print("\n������������ݳ���!!!\n");
          }
        }
#endif
        else {
          int retval = lssproto_ServerDispatchMessage(fdremember, rbmess);
          if (retval == -1) {
            if (++Connect[fdremember].errornum > allowerrornum)
              break;
          }
#ifdef _NEW_FUNC_DECRYPT
          else if (retval == -2) {
            if (++Connect[fdremember].newerrnum > getAllowerrornum2())
              break;
          }
#endif
        }
      }
    }
    if (Connect[fdremember].errornum > allowerrornum) {
      print("�û�:%s����̫������ˣ�����ǿ�ƹر�\n",
            inet_ntoa(Connect[fdremember].sin.sin_addr));

#ifdef _NETLOG_
      char cdkey[16];
      char charname[32];
      CONNECT_getCharname(fdremember, charname, 32);
      CONNECT_getCdkey(fdremember, cdkey, 16);
      char token[128];
      sprintf(token, "�û�:%s����̫������ˣ�����ǿ�ƹر�\n",
              inet_ntoa(Connect[fdremember].sin.sin_addr));
      LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif
      CONNECT_endOne_debug(fdremember);

      continue;
    }
#ifdef _NEW_FUNC_DECRYPT

    if (Connect[fdremember].newerrnum > getAllowerrornum2()) {
      print("�û�:%s����̫������ˣ�����(��IP)�Ͽ�����\n",
            inet_ntoa(Connect[fdremember].sin.sin_addr));

#ifdef _NETLOG_
      char cdkey[16];
      char charname[32];
      CONNECT_getCharname(fdremember, charname, 32);
      CONNECT_getCdkey(fdremember, cdkey, 16);
      char token[128];
      sprintf(token, "�û�:%s����̫������ˣ�����(��IP)�Ͽ�����\n",
              inet_ntoa(Connect[fdremember].sin.sin_addr));
      LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif

      CONNECT_endOne_debug(fdremember);
      continue;
    }
#endif
    if (Connect[fdremember].CAbufsiz > 0 &&
        time_diff_us(et, Connect[fdremember].lastCAsendtime) >
            casend_interval_us) {
      CAsend(fdremember);
      Connect[fdremember].lastCAsendtime = et;
    }

    if (Connect[fdremember].CDbufsiz > 0 &&
        time_diff_us(et, Connect[fdremember].lastCDsendtime) >
            cdsend_interval_us) {
      CDsend(fdremember);
      Connect[fdremember].lastCDsendtime = et;
    }

    if (Connect[fdremember].wbuse > 0) {
      FD_ZERO(&rfds);
      FD_ZERO(&wfds);
      FD_ZERO(&efds);

      FD_SET(fdremember, &rfds);
      FD_SET(fdremember, &wfds);
      FD_SET(fdremember, &efds);
      tmv.tv_sec = tmv.tv_usec = 0;
      ret = select(fdremember + 1, &rfds, &wfds, &efds, &tmv);

      if (ret > 0 && FD_ISSET(fdremember, &wfds)) {
        // Nuke start 0907: Protect gmsv

#ifdef _OTHER_SAAC_LINK
        if (CONNECT_getCtype(fdremember) == AC)
#else
        if (fdremember == acfd)
#endif
        {
#ifdef _DEBUG
          printf("����SAAC����:%s\n", Connect[fdremember].wb);
#endif
          ret = write(fdremember, Connect[fdremember].wb,
                      (Connect[fdremember].wbuse < acwritesize)
                          ? Connect[fdremember].wbuse
                          : acwritesize);
        } else {
          ret = write(fdremember, Connect[fdremember].wb,
                      (Connect[fdremember].wbuse < 1024 * 64)
                          ? Connect[fdremember].wbuse
                          : 1024 * 64);
          sendspeed += ret;
        }

        // Nuke end

        if (ret == -1 && errno != EINTR) {
#ifdef _NETLOG_
          char cdkey[16];
          char charname[32];
          CONNECT_getCharname(fdremember, charname, 32);
          CONNECT_getCdkey(fdremember, cdkey, 16);
          char token[128];
          sprintf(token, "���ͷ��д�뷵��:%d %s\n", errno, strerror(errno));
          LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif
          CONNECT_endOne_debug(fdremember);
          continue;
        } else if (ret > 0) {
          shiftWB(fdremember, ret);
        }
      } else if (ret < 0 && errno != EINTR) {
#ifdef _NETLOG_
        char cdkey[16];
        char charname[32];
        CONNECT_getCharname(fdremember, charname, 32);
        CONNECT_getCdkey(fdremember, cdkey, 16);
        char token[128];
        sprintf(token, "���ͷ��д�����Ӵ���:%d %s\n", errno, strerror(errno));
        LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif
        CONNECT_endOne_debug(fdremember);
      }
    }
    /* �����ॢ���Ȥν��� */
#ifdef _OTHER_SAAC_LINK
    if (CONNECT_getCtype(fdremember) == AC)
#else
    if (fdremember == acfd)
#endif
      continue;

    // ttom start : because of the second have this
    if (Connect[fdremember].close_request) {
      //      print( "ǿ�ȹر�:%s \n",inet_ntoa(Connect[fdremember].sin.sin_addr
      //      ));
      CONNECT_endOne_debug(fdremember);
      continue;
    }
  }
  return TRUE;
}

ANY_THREAD void outputNetProcLog(int fd, int mode) {
  int i;
  int c_use = 0, c_notdetect = 0;
  int c_ac = 0, c_cli = 0, c_adm = 0, c_max = 0;
  int login = 0;
  char buffer[4096];
  char buffer2[4096];

  strcpysafe(buffer, sizeof(buffer), "Server Status\n");
  c_max = ConnectLen;

  for (i = 0; i < c_max; i++) {
    CONNECT_LOCK(i);

    if (Connect[i].use) {
      c_use++;
      switch (Connect[i].ctype) {
      case NOTDETECTED:
        c_notdetect++;
        break;
      case AC:
        c_ac++;
        break;
      case CLI:
        c_cli++;
        break;
      case ADM:
        c_adm++;
        break;
      }

      if (Connect[i].charaindex >= 0) {
        login++;
      }
    }

    CONNECT_UNLOCK(i);
  }

  snprintf(buffer2, sizeof(buffer2),
           "connect_use=%d\n"
           "connect_notdetect=%d\n"
           "connect_ac=%d\n"
           "connect_cli=%d\n"
           "connect_adm=%d\n"
           "connect_max=%d\n"
           "login=%d\n",
           (c_use * getConnectnum() / 100), c_notdetect, c_ac,
           (c_cli * getConnectnum() / 100), c_adm, c_max,
           (login * getConnectnum() / 100));
  strcatsafe(buffer, sizeof(buffer), buffer2);
  {
    int char_max = CHAR_getCharNum();
    int char_use = 0;
    int pet_use = 0;

    for (i = 0; i < char_max; i++) {
      if (CHAR_getCharUse(i)) {
        char_use++;

        if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          pet_use++;
        }
      }
    }

    snprintf(buffer2, sizeof(buffer2),
             "char_use=%d\n"
             "char_max=%d\n"
             "pet_use=%d\n",
             char_use, char_max, pet_use);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  {

    int i;
    int item_max = ITEM_getITEM_sItemNum();
    int item_use = 0;

    for (i = 0; i < item_max; i++) {
      if (ITEM_getITEM_use(i)) {
        item_use++;
      }
    }

    snprintf(buffer2, sizeof(buffer2),
             "item_use=%d\n"
             "item_max=%d\n",
             item_use, item_max);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  {
    int i, obj_use = 0;
    int obj_max = OBJECT_getNum();

    for (i = 0; i < obj_max; i++) {
      if (OBJECT_getType(i) != OBJTYPE_NOUSE) {
        obj_use++;
      }
    }

    snprintf(buffer2, sizeof(buffer2),
             "object_use=%d\n"
             "object_max=%d\n",
             obj_use, obj_max);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  if (mode == 0) {
    LogHelper(LOG_PROC, buffer);
  } else if (mode == 1) {
    lssproto_ProcGet_send(fd, buffer);
  }
}

ANY_THREAD int getfdFromCdkeyWithLogin(const char *cdkey) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE
        && Connect[i].state != NOTLOGIN // Avoid duplicated login.
        && strcmp(Connect[i].cdkey, cdkey) == 0) {
      CONNECT_UNLOCK(i);
      return i;
    }

    CONNECT_UNLOCK(i);
  }
  return -1;
}

#define m_cktime 500

int CHAR_players() {
  int i;
  int chars = 0;
  int players = 0, pets = 0, others = 0;
  int whichtype = -1;
  int objnum = OBJECT_getNum();

  for (i = 0; i < objnum; i++) {
    switch (OBJECT_getType(i)) {
    case OBJTYPE_CHARA:
      chars++;
      whichtype = CHAR_getInt(OBJECT_getIndex(i), CHAR_WHICHTYPE);

      if (whichtype == CHAR_TYPEPLAYER)
        players++;
      else if (whichtype == CHAR_TYPEPET)
        pets++;
      else
        others++;
      break;
    default:
      break;
    }
  }
  return players;
}
void sigusr1(int i) {
  signal(SIGUSR1, sigusr1);
  cono_check = (cono_check + 1) % 4;
  print("Cono Check is login:%d item:%d", cono_check & 1, cono_check & 2);
}
// Arminius 6.26
void sigusr2(int i) {
  signal(SIGUSR2, sigusr2);
  print("\nReceived Shutdown signal...\n\n");
  lssproto_Shutdown_recv(0, "hogehoge", 5); // 5������ά��
}

unsigned long CONNECT_get_userip(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  unsigned long ip;
  memcpy(&ip, &Connect[fd].sin.sin_addr, sizeof(long));
  return ip;
}
void CONNECT_set_pass(int fd, BOOL b_ps) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].b_pass = b_ps;
}
BOOL CONNECT_get_pass(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  BOOL B_ret;
  B_ret = Connect[fd].b_pass;
  return B_ret;
}
void CONNECT_set_first_warp(int fd, BOOL b_ps) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].b_first_warp = b_ps;
}
BOOL CONNECT_get_first_warp(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  BOOL B_ret;
  B_ret = Connect[fd].b_first_warp;
  return B_ret;
}
void CONNECT_set_state_trans(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].state_trans = a;
}
int CONNECT_get_state_trans(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int i_ret;
  i_ret = Connect[fd].state_trans;
  return i_ret;
}
// ttom end

// Arminius 6.22 encounter
int CONNECT_get_CEP(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].CEP;
}

void CONNECT_set_CEP(int fd, int cep) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].CEP = cep;
}

int CONNECT_get_confirm(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].confirm_key;
}
void CONNECT_set_confirm(int fd, BOOL b) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].confirm_key = b;
}

int checkNu(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  Connect[fd].nu--;
  // print("NU=%d\n",Connect[fd].nu);

  if (Connect[fd].nu < 0)
    return -1;

  return 0;
}

// Nuke start 0626: For no enemy function
void setNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].noenemy = 6;
}
void clearNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].noenemy = 0;
}
int getNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].noenemy;
}
// Nuke end

// Arminius 7/2: Ra's amulet
void setEqNoenemy(int fd, int level) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqnoenemy = level;
}

void clearEqNoenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqnoenemy = 0;
}

int getEqNoenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].eqnoenemy;
}

#ifdef _Item_MoonAct
void setEqRandenemy(int fd, int level) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqrandenemy = level;
}

void clearEqRandenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqrandenemy = 0;
}

int getEqRandenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].eqrandenemy;
}

#endif

#ifdef _CHIKULA_STONE
void setChiStone(int fd, int nums) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].chistone = nums;
}
int getChiStone(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].chistone;
}
#endif

// Arminius 7.31 cursed stone
void setStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].stayencount = 1;
}

void clearStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].stayencount = 0;
}

int getStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].stayencount;
}
#ifdef _BATTLE_TIMESPEED
void CONNECT_setBDTime(int fd, int nums) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].BDTime = nums;
}

int CONNECT_getBDTime(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].BDTime;
}
#endif
#ifdef _TYPE_TOXICATION
void setToxication(int fd, int flg) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].toxication = flg;
}
int getToxication(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].toxication;
}
#endif

#ifdef _BATTLE_TIMESPEED
void RescueEntryBTime(int charaindex, int fd, unsigned int lowTime,
                      unsigned int battletime) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  int NowTime = (int)time(NULL);

  Connect[fd].CBTime = NowTime;
  // Connect[fd].CBTime+battletime
}

BOOL CheckDefBTime(int charaindex, int fd, unsigned int lowTime,
                   unsigned int battletime,
                   unsigned int addTime) // lowTime�ӳ�ʱ��
{
  if (fd < 0 || fd >= ConnectLen) {
    return TRUE;
  }
  unsigned int NowTime = (unsigned int)time(NULL);

  // print(" NowTime=%d lowTime=%d battleTime=%d CBTime=%d", NowTime, lowTime,
  // battletime, Connect[fd].CBTime);

  lowTime += battletime;

  if ((Connect[fd].CBTime + battletime) > lowTime)
    lowTime = Connect[fd].CBTime + battletime;
  /*
    if ( NowTime < lowTime ) { //lowTimeӦ�õ�ս������ʱ��
      int r = 0;
      delayTime = lowTime - NowTime;
      delayTime = ( delayTime <= 0 ) ? 1 : delayTime;
      r = ( -4 ) * ( delayTime + 2 );
      lssproto_NU_send( fd, r );
      Connect[ fd ].nu += r;
    }
  */
  lssproto_NU_send(fd, 0);
  // Connect[fd].BDTime = (NowTime+20)+delayTime;
#ifdef _FIX_CHARLOOPS
  if (getCharloops() > 0)
    Connect[fd].BDTime =
        NowTime + rand() % getCharloops(); // �񱦵ȴ�ʱ��
  else
    Connect[fd].BDTime = NowTime;
#else
  Connect[fd].BDTime =
      (NowTime + rand() % 5) + delayTime + addTime; // �񱦵ȴ�ʱ��
#endif
  // print(" BDTime=%d ", Connect[fd].BDTime);
  return TRUE;
}
#endif

BOOL MSBUF_CHECKbuflen(int size, float defp) { return TRUE; }

#ifdef _OTHER_SAAC_LINK
char servername[] = "pt.allblues.com.cn";
BOOL OtherSaacConnect(void) {
  if (servername[0] != 'p' || servername[1] != 't' || servername[2] != '.' ||
      servername[3] != 'a' || servername[4] != 'l' || servername[5] != 'l' ||
      servername[6] != 'b' || servername[7] != 'l' || servername[8] != 'u' ||
      servername[9] != 'e' || servername[10] != 's' || servername[11] != '.' ||
      servername[12] != 'c' || servername[13] != 'o' || servername[14] != 'm' ||
      servername[15] != '.' || servername[16] != 'c' || servername[17] != 'n') {
    exit(0);
  } else {
    print("�������ӵ��������... ");
#if _ATTESTAION_ID == 1
    char ip[256] = "192.168.1.11";
    osfd = connectHost(ip, 18888);
#else
    osfd = connectHost(servername, 18888);
#endif
    if (osfd == -1) {
      print("ʧ��\n");
      return FALSE;
    } else {
      print("���\n");
      initConnectOne(osfd, NULL, 0);
      if (!CONNECT_acfdInitRB(osfd) || !CONNECT_acfdInitWB(osfd) ||
          saacproto_InitClient(lsrpcClientWriteFunc, LSGENWORKINGBUFFER, osfd) <
              0) {
        CONNECT_endOne_debug(osfd);
        osfd = -1;
        return FALSE;
      }
      CONNECT_setCtype(osfd, SQL);

      saacproto_ACServerLogin_send(osfd, _ATTESTAION_ID, getGameserverID(),
                                   getAccountserverpasswd());
    }
  }
  return TRUE;
}
#endif

void saveforsaac() {

  int acwritesize = getAcwriteSize();

  while (Connect[acfd].wbuse > 0) {
    struct timeval tmv; /*timeval*/
    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    FD_SET(acfd, &rfds);
    FD_SET(acfd, &wfds);
    FD_SET(acfd, &efds);
    tmv.tv_sec = tmv.tv_usec = 0;
    int ret = select(acfd + 1, &rfds, &wfds, &efds, &tmv);

    if (ret > 0 && FD_ISSET(acfd, &wfds)) {
      // Nuke start 0907: Protect gmsv

#ifdef _DEBUG
      printf("����SAAC����:%s\n", Connect[acfd].wb);
#endif
      ret = write(acfd, Connect[acfd].wb,
                  (Connect[acfd].wbuse < acwritesize) ? Connect[acfd].wbuse
                                                      : acwritesize);

      if (ret == -1 && errno != EINTR) {
#ifdef _NETLOG_
        char log[512];
        sprintf(log, "д�뷵��: %d %s \n", errno, strerror(errno));
        LogCharOut("", "", __FILE__, __FUNCTION__, __LINE__, log);
#endif
        CONNECT_endOne_debug(acfd);
        continue;
      } else if (ret > 0) {
        shiftWB(acfd, ret);
      }
    } else if (ret < 0 && errno != EINTR) {
#ifdef _NETLOG_
      char log[512];
      sprintf(log, "saveforsaac д�����Ӵ���:%d %s", errno, strerror(errno));
      LogCharOut("", "", __FILE__, __FUNCTION__, __LINE__, log);
#endif
      CONNECT_endOne_debug(acfd);
    }
  }
}

#ifdef _EPOLL
void initEpoll() {
  print("��ʼ������ģ��... ");
  epollfd = epoll_create(getFdnum() + 1);
  eventsEpoll = allocateMemory(sizeof(evEpoll) * (getFdnum() + 1));
  print("���\n");
  initList();
}

void AddEpoll(int fd) {
  evEpoll.data.fd = fd;
  evEpoll.events = EPOLLIN | EPOLLHUP | EPOLLERR;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &evEpoll);
}

void DelEpoll(int fd) {
  evEpoll.data.fd = fd;
  evEpoll.events = NULL;
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &evEpoll);
}

void ModEpollOut(int fd) {
  evEpoll.events = EPOLLOUT | EPOLLHUP | EPOLLERR;
  evEpoll.data.fd = fd;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &evEpoll);
}

void ModEpollIn(int fd) {
  evEpoll.events = EPOLLIN | EPOLLHUP | EPOLLERR;
  evEpoll.data.fd = fd;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &evEpoll);
}

extern pthread_t Sendthread[3];
extern sigset_t g_waitSig[3];
void quick(int a[][2], int i, int j) {
  int m, n, temp1, temp2;
  int k;
  m = i;
  n = j;
  k = a[(i + j) / 2][0]; /*ѡȡ�Ĳ���*/
  do {
    while (a[m][0] < k && m < j)
      m++;
    while (a[n][0] > k && n > i)
      n--;
    if (m <= n) {
      temp1 = a[m][0];
      temp2 = a[m][1];
      a[m][0] = a[n][0];
      a[m][1] = a[n][1];
      a[n][0] = temp1;
      a[n][1] = temp2;
      m++;
      n--;
    }
  } while (m <= n);
  if (m < j)
    quick(a, m, j);
  if (n > i)
    quick(a, i, n);
}

void DelList(Node **pHead)
{
  Node *pTmp = *pHead;
  *pHead = pTmp->next;
  free(pTmp);
}

int GetSendListFd(Node **pHead) {
  LIANBIAOSENDLOCK
  int ret = -1;
  if (*pHead != NULL) {
    Node *pTmp = *pHead;
    ret = pTmp->fd;
    DelList(pHead);
  }
  LIANBIAOSENDUNLOCK
  return ret;
}

int GetRecvListFd(Node **pHead) {
  LIANBIAORECVLOCK
  int ret = -1;
  if (*pHead != NULL) {
    Node *pTmp = *pHead;
    ret = pTmp->fd;
    DelList(pHead);
  }
  LIANBIAORECVUNLOCK
  return ret;
}

void initList() {
  EpollSendpNode[0] = EpollSendpNode[1] = EpollSendpNode[2] =
      EpollSendpNode[3] = NULL;
  printf("Init epoll sendp node list.\n");
}

int SendsizeList(Node *pHead) {
  int size = 0;
  while (pHead != NULL) {
    size++;
    pHead = pHead->next;
  }
  return size; // ������ʵ�ʳ���
}

void insertSendPlayDataLastList(Node **pNode, int fd) {
  if (*pNode == NULL) {
    Node *pInsert = (Node *)malloc(sizeof(Node));
    memset(pInsert, 0, sizeof(Node));
    pInsert->fd = fd;
    *pNode = pInsert;
  } else {
    Node *pInsert;
    Node *pHead;
    Node *pTmp;
    pHead = *pNode;
    pTmp = pHead;
    pInsert = (Node *)malloc(sizeof(Node)); // ����һ���½ڵ�
    memset(pInsert, 0, sizeof(Node));
    pInsert->fd = fd;
    while (pHead->next != NULL) {
      pHead = pHead->next;
    }
    pHead->next = pInsert; // ������ĩβ�ڵ����һ���ָ�������ӵĽڵ�
    *pNode = pTmp;
  }
}

BOOL checkRecvPlayDataList(Node **pNode, int fd) {
  if (NULL == *pNode) {
    return FALSE;
  } else {
    Node *pTmp = *pNode;
    while (NULL != pTmp) {
      if (tMep->fd == fd)
        return TRUE;
      tMep = tMep->next;
    }
  }
  return FALSE;
}

void insertRecvPlayDataLastList(Node **pNode, int fd) {
  LIANBIAORECVLOCK
  if (checkRecvPlayDataList(pNode, fd)) {
    LIANBIAORECVUNLOCK
    return;
  }
  if (*pNode == NULL) {
    Node *pInsert = (Node *)malloc(sizeof(Node));
    memset(pInsert, 0, sizeof(Node));
    pInsert->fd = fd;
    *pNode = pInsert;
  } else {
    Node *pInsert;
    Node *pHead;
    Node *pTmp;
    pHead = *pNode;
    pTmp = pHead;
    pInsert = (Node *)malloc(sizeof(Node));
    memset(pInsert, 0, sizeof(Node));
    pInsert->fd = fd;
    while (pHead->next != NULL) {
      pHead = pHead->next;
    }
    pHead->next = pInsert;
    *pNode = pTmp;
  }
  LIANBIAORECVUNLOCK
}

void SendPlayDataAddList(int fd) {
  LIANBIAOSENDLOCK
  int shuzi[3][2];
  shuzi[0][0] = SendsizeList(EpollSendpNode[1]);
  shuzi[0][1] = 1;
  shuzi[1][0] = SendsizeList(EpollSendpNode[2]);
  shuzi[1][1] = 2;
  shuzi[2][0] = SendsizeList(EpollSendpNode[3]);
  shuzi[2][1] = 3;
  quick(shuzi, 0, 2);
  insertSendPlayDataLastList(&EpollSendpNode[shuzi[0][1]], fd);
  pthread_kill(Sendthread[shuzi[0][1] - 1], SIGUSR1);
  LIANBIAOSENDUNLOCK
}

BOOL setnonblocking(int sockfd) {
  int opts;
  // int yes = 1;
  // setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&yes,sizeof yes); //
  // reuse fix setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,(char *)&yes,sizeof
  // yes); // reuse fix

  opts = fcntl(sockfd, F_GETFL);
  if (opts < 0) {
    perror("fcntl(F_GETFL)\n");
    return FALSE;
  }
  opts = (opts | O_NONBLOCK);
  if (fcntl(sockfd, F_SETFL, opts) < 0) {
    perror("fcntl(F_SETFL)\n");
    return FALSE;
  }
  return TRUE;
}

void procAcceptEpoll() {
  int acceptmore = SERVSTATE_getAcceptmore();
  struct sockaddr_in sin;
  int addrlen = sizeof(struct sockaddr_in);
  int sockfd;
  if ((sockfd = accept(bindedfd, (struct sockaddr *)&sin, &addrlen)) > 0) {
    if (Connect[sockfd].use)
      return;
    if (lianjielog)
      printf("���뵽������A    sockfd=%d   Connect[ sockfd ].use=%d\n", sockfd,
             Connect[sockfd].use);
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
      close(sockfd);
      return;
    }
    if (strlen(keepupnologin) > 0) {
      char mess[64];
      sprintf(mess, "E��Ϸά���С���������");
      mess[strlen(mess)] = 0;
      write(sockfd, mess, strlen(mess) + 1);
      close(sockfd);
      return;
    }
    int cono = 1;
    if (cono_check & CONO_CHECK_LOGIN) {
      if (StateTable[WHILELOGIN] + StateTable[WHILELOGOUTSAVE] >
              QUEUE_LENGTH1 ||
          StateTable[WHILEDOWNLOADCHARLIST] > QUEUE_LENGTH2) {
        CONNECT_checkStatecount(WHILEDOWNLOADCHARLIST);
        cono = 0;
      }
    }
    if (cono_check & CONO_CHECK_ITEM) {
      if ((total_item_use) >= MAX_item_use) {
        print("��ʹ����Ʒ������!!");
        cono = 0;
      }
    }
    if (cono_check & CONO_CHECK_PET) {
      if ((petcnt) >= CHAR_getPetMaxNum()) {
        print("��ʹ�ó���������!!");
        cono = 0;
      }
    }
    float fs = 0.0;
    if ((fs = ((float)Connect[acfd].rbuse / AC_RBSIZE)) > 0.6) {
      print("SAAC�������ռ�Σ��: %3.2f [%4d]\n", fs, Connect[acfd].rbuse);
      if (fs > 0.78)
        cono = 0;
    }
    if ((cono == 0) || (acceptmore <= 0)) {
      char mess[64] = "E�ŷ���æ���У����Ժ����ԡ�";
      write(sockfd, mess, strlen(mess) + 1);
      close(sockfd);
      return;
    } else if (sockfd < ConnectLen) {
      if (lianjielog)
        printf("�ɹ�������A    sockfd=%d\n", sockfd);
      char mess[2] = {};
      strcpy(mess, getSaVersion());
      send(sockfd, mess, strlen(mess) + 1, 0);
      initConnectOne(sockfd, &sin, addrlen);
      if (lianjielog)
        printf("�ɹ�������B    sockfd=%d     %d\n", sockfd, Connect[sockfd].use);
      AddEpoll(sockfd);
      return;
    } else {
      int i;
      for (i = 0; i < ConnectLen; i++) {
        if (Connect[i].use == FALSE)
          continue;
        if (i == acfd)
          continue;
        if (i == bindedfd)
          continue;
        if (Connect[i].charaindex != -1)
          continue;
        char mess[64] = "E�ŷ�����æ�����Ժ����ԡ�";
        write(i, mess, strlen(mess) + 1);
        close(i);
      }
      char mess[64] = "E�ŷ����������������Ժ����ԡ�";
      write(sockfd, mess, strlen(mess) + 1);
      close(sockfd);
      return;
    }
  }
}

BOOL procRecvEpoll(int sockfd) {
  char recvBuff[131072];
  memset(recvBuff, 0, sizeof(recvBuff));
  int ret = read(sockfd, recvBuff, 131072);
  if (ret == -1) {
    if (errno != EAGAIN && errno != EINTR) {
      if (sockfd == acfd) {
        print("��ȡ����:%d %s\n", ret, strerror(errno));
        print("gmsv��acsvʧȥ����! �쳣��ֹ...\n");
        sigshutdown(-1);
        exit(1);
      } else {
        char cdkey[16];
        char charname[32];
        CONNECT_getCharname(sockfd, charname, 32);
        CONNECT_getCdkey(sockfd, cdkey, 16);
        char yuanyin[128];
        snprintf(yuanyin, 128, "���recv���� error=%d  %s", errno,
                 strerror(errno));
        LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, yuanyin);
        CONNECT_endOne_debug(sockfd);
        return FALSE;
      }
    }
    return FALSE;
  } else if (ret == 0) {
    CONNECT_endOne_debug(sockfd);
    return FALSE;
  }
  CONNECT_RecvLOCK(sockfd);
  int appret = appendRB(sockfd, recvBuff, ret);
  if (appret == -1 && getErrUserDownFlg() == 1) {
    char cdkey[16];
    char charname[32];
    CONNECT_getCharname(sockfd, charname, 32);
    CONNECT_getCdkey(sockfd, cdkey, 16);
    LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, "���ʹ�÷Ƿ��ֶ�");
    CONNECT_endOne_debug(sockfd);
    CONNECT_RecvUNLOCK(sockfd);
    return FALSE;
  }
  CONNECT_RecvUNLOCK(sockfd);
  Connect[sockfd].lastreadtime = NowTime;
  Connect[sockfd].packetin = 30;
  Connect[sockfd].lastreadtime.tv_sec = 0;
  insertRecvPlayDataLastList(&EpollSendpNode[0], sockfd);
  return TRUE;
}

void procSendEpoll(int id) {
  siginfo_t info;
  sigset_t sig;
  sigemptyset(&sig);
  sigaddset(&sig, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &sig, NULL);
  int acwritesize = getAcwriteSize();
  while (TRUE) {
    int sockfd = GetSendListFd(&EpollSendpNode[id]);
    if (sockfd != -1) {
      if (CONNECT_getUse(sockfd)) {
        int nwrite;
        if (Connect[sockfd].wbuse > 0) {
          CONNECT_LOCK(sockfd);
          if (sockfd == acfd) {
            nwrite = write(sockfd, Connect[sockfd].wb,
                           (Connect[sockfd].wbuse < acwritesize)
                               ? Connect[sockfd].wbuse
                               : acwritesize);
          } else {
            nwrite = write(sockfd, Connect[sockfd].wb,
                           (Connect[sockfd].wbuse < 1024 * 64)
                               ? Connect[sockfd].wbuse
                               : 1024 * 64);
          }
          if (nwrite == -1) {
            if (errno != EAGAIN && EINTR != errno && EWOULDBLOCK != errno)
              CONNECT_setCloseRequest(sockfd, 1);
          } else
            shiftWB(sockfd, nwrite);
          CONNECT_UNLOCK(sockfd);
        }
        if (Connect[sockfd].wbuse > 0)
          ModEpollOut(sockfd);
        else
          ModEpollIn(sockfd);
      }
    } else
      sigwaitinfo(&sig, &info);
  }
}

void procSelectEpoll() {
  int epollnum = getFdnum() + 1;
  static struct timeval et;
  unsigned int casend_interval_us = getCAsendinterval_ms() * 200;
  unsigned int cdsend_interval_us = getCDsendinterval_ms() * 200;
  while (TRUE) {
    gettimeofday(&et, NULL);
    int nfds = epoll_wait(epollfd, eventsEpoll, epollnum, -1);
    if (nfds > -1) {
      int n;
      for (n = 0; n < nfds; ++n) {
        // printf("socdfd=%d     events=%d   EPOLLIN=%d
        // EPOLLOUT=%d\n",eventsEpoll[n].data.fd,eventsEpoll[n].events,EPOLLIN,EPOLLOUT);
        if (eventsEpoll[n].data.fd == bindedfd) { // ���յ�������
          if (lianjielog)
            printf("socdfd=%d     events=%d   EPOLLIN=%d   EPOLLOUT=%d\n",
                   eventsEpoll[n].data.fd, eventsEpoll[n].events, EPOLLIN,
                   EPOLLOUT);
          procAcceptEpoll();
          continue;
        } else if (eventsEpoll[n].events & EPOLLIN) { // ���ܵ���Ϣ
          if (CONNECT_getUse(eventsEpoll[n].data.fd))
            if (!procRecvEpoll(eventsEpoll[n].data.fd))
              continue;
        } else if (eventsEpoll[n].events & EPOLLOUT) { // ������Ϣ
          int sockfd = eventsEpoll[n].data.fd;
          if (CONNECT_getUse(sockfd)) {
            if (Connect[sockfd].CAbufsiz > 0 &&
                time_diff_us(et, Connect[sockfd].lastCAsendtime) >
                    casend_interval_us) {
              CAsend(sockfd);
              Connect[sockfd].lastCAsendtime = et;
            }
            if (Connect[sockfd].CDbufsiz > 0 &&
                time_diff_us(et, Connect[sockfd].lastCDsendtime) >
                    cdsend_interval_us) {
              CDsend(sockfd);
              Connect[sockfd].lastCDsendtime = et;
            }
            if (Connect[sockfd].wbuse > 0)
              SendPlayDataAddList(sockfd);
            else
              ModEpollIn(sockfd);
          }
        } else if (eventsEpoll[n].events & EPOLLHUP ||
                   eventsEpoll[n].events & EPOLLERR) {
          CONNECT_endOne_debug(eventsEpoll[n].data.fd);
          continue;
        }
        if (eventsEpoll[n].data.fd == acfd)
          continue;
        else if (eventsEpoll[n].data.fd > -1) {
          if (Connect[eventsEpoll[n].data.fd].close_request) {
            CONNECT_endOne_debug(eventsEpoll[n].data.fd);
          }
        }
      }
    }
  }
}
#endif
#ifdef _MO_LNS_NLSUOXU
BOOL DelTimer_net(int ID) {
  if (ID - 1 < 0 || ID - 1 > arraysizeof(TimerLua))
    return FALSE;
  TimerLua[ID - 1].FileName = NULL;
  TimerLua[ID - 1].FuncName = NULL;
  TimerLua[ID - 1].EspTime = -1;
  TimerLua[ID].MespTime = -1;
  TimerLua[ID - 1].ID = -1;
  TimerLua[ID - 1].GetTime = -1;
  return TRUE;
}
#endif

#ifdef _MO_LNS_CHARSUOXU
char *CONNECT_get_userip2(int fd) {
  return inet_ntoa(Connect[fd].sin.sin_addr);
}
int CONNECT_get_userport(int fd) { return Connect[fd].sin.sin_port; }
#endif
