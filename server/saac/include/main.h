#ifndef __MAIN_H__
#define __MAIN_H__

#include "saac_common.h"

#define SA_NODEFER 0x40000000
#define SA_NOMASK SA_NODEFER
#define CHARDATASIZE (1024 * 1024)

char *chartime();

typedef struct tagGmsv {
  int use;
  int fd;
  char name[128];
} gmsv;

int get_rotate_count(void);
void checkGSUCheck(char *id);
int logout_game_server(int ti);
int is_game_server_login(int ti);

void gmsvBroadcast(int fd, char *p1, char *p2, char *p3, int flag);

#if _ATTESTAION_ID == 1
int login_game_server(const int ti, const int id, const char *svname,
                      const char *svpas, char *result, const int resultlen,
                      char *retdata, int retdatalen);
#else
int login_game_server(const int ti, const char *svname, const char *svpas,
                      char *result, const int resultlen, char *retdata,
                      const int retdatalen);
#endif

#ifdef _ANGEL_SUMMON
void delMissionTableOnedata(int index);

typedef enum {
  MISSION_NONE = 0,
  MISSION_WAIT_ANSWER,
  MISSION_DOING,
  MISSION_HERO_COMPLETE,
  MISSION_TIMEOVER,
} ANGEL_MISSIONFLAG;
#endif

typedef struct tagMemBuffer {
  int use;
  char buf[512];
  int len;                       //
  int next;                      //
} MemBuffer;

typedef struct tagConnection {
  int use;                       // 链接是否使用
  int fd;                        // 连接fd
  int mbtop_ri;                  // membuf top_ri 读缓存
  int mbtop_wi;                  // membuf top_wi 写缓存
  struct sockaddr_in remoteaddr; // 远端地址，即客户端的socket地址
  int closed_by_remote;
} Connection;

#ifdef __MAIN_C__
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN MemBuffer *g_mem_buffer;
EXTERN int g_mem_buffer_size;
EXTERN int g_mem_buffer_used;
EXTERN int g_mem_buffer_finder;
EXTERN char g_temp_buffer[1 << 20];
EXTERN Connection *g_con; // SAAC-Client连接
EXTERN int g_main_sock_fd; // 主sock文件描述符
EXTERN struct sockaddr_in g_local_addr; // 本地SAAC服务器的TCP地址
EXTERN struct timeval select_timeout; // 选择超时时间
EXTERN time_t sys_time; // Robin add
EXTERN gmsv gs[MAXCONNECTION]; // SAAC-GMSV连接
#undef EXTERN

int findregBlankCon(void);
int getFreeMem(void);
int appendReadBuffer(int index, char *data, int len);
int appendWriteBuffer(int index, char *data, int len);
int appendMemBufList(int top, char *data, int len);
int consumeMemBufList(int top, char *out, int len, int flag, int copyflag);
int getLineReadBuffer(int index, char *buf, int len);
#endif
