#ifndef __LOCK_H__
#define __LOCK_H__

//
#include "saac_common.h"

typedef struct tagLockNode {
  int use;
  char cdkey[16];
#ifdef _LOCK_ADD_NAME
  char name[32];
#endif
  char server[128];
  int process;
  struct tagLockNode *next, *prev;
} LockNode;

#ifdef __LOCK_C__
#define EXT
#else
#define EXT extern
#endif

EXT LockNode **g_lock;
#undef EXT // 必须在结束之前取消EXT的定义

void Lock_Init(void);
#ifdef _LOCK_ADD_NAME
int InsertMemLock(int entry, char *cdkey, char *name, char *passwd,
                  char *server, int process, char *deadline);
#else
int InsertMemLock(int entry, char *cdkey, char *passwd, char *server,
                  int process, char *deadline);
#endif
int DeleteMemLock(int entry, char *cdkey, int *process);
void DeleteMemLockServer(const char *sname);
int isMemLocked(int entry, char *cdkey);
int GetMemLockState(int entry, char *cdkey, char *result);
int GetMemLockServer(int entry, char *cdkey, char *result);

int LockNode_getGname(int entries, char *id, char *gname);

int ChangeMemDeadline(int entry, char *cdkey, char *deadline);


#endif /* __LOCK_H__ */
