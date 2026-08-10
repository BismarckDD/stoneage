#define __LOCK_C__
#include "lock.h"

void Lock_Init(void) {
  int i;
  g_lock = (LockNode **)calloc(1, sizeof(LockNode *) * 256);
  memset(g_lock, 0, sizeof(g_lock));
  for (i = 0; i < 256; i++) {
    g_lock[i] = (LockNode *)calloc(1, sizeof(LockNode));
    g_lock[i]->use = 0;
    g_lock[i]->next = NULL;
    g_lock[i]->prev = NULL;
    memset(g_lock[i]->cdkey, 0, sizeof(g_lock[i]->cdkey));
    memset(g_lock[i]->server, 0, sizeof(g_lock[i]->server));
#ifdef _LOCK_ADD_NAME
    memset(g_lock[i]->name, 0, sizeof(g_lock[i]->name));
#endif
  }
  logErr("全局锁初始化完毕.\n");
}

LockNode *Lock_create(void) {
  LockNode *lock_node = (LockNode *)calloc(1, sizeof(LockNode));
  if (lock_node == NULL) {
    logErr("CANNOT alloc:%d lock node!!\n", sizeof(LockNode));
    return 0;
  }
  lock_node->use = 0;
  lock_node->next = NULL;
  memset(lock_node->cdkey, 0, sizeof(lock_node->cdkey));
  memset(lock_node->server, 0, sizeof(lock_node->server));
#ifdef _LOCK_ADD_NAME
  memset(lock_node->name, 0, sizeof(lock_node->name));
#endif
  return lock_node;
}

#ifdef _LOCK_ADD_NAME
int InsertMemLock(int entry, char *cdkey, char *name, char *passwd,
                  char *server, int process, char *deadline)
#else
int InsertMemLock(int entry, char *cdkey, char *passwd, char *server,
                  int process, char *deadline)
#endif
{
  int j;
  LockNode *lock_node = g_lock[entry];
#ifdef _LOCK_ADD_NAME
  logErr("进入游戏:目录:char/0x%x 账号:%s 名称:%s 服务器:%s\n", entry, cdkey, name,
      server);
#else
  logErr("进入游戏:目录:%x 账号:%s 服务器:%s\n", entry, cdkey, server);
#endif

  while ((lock_node != NULL) && (lock_node->use != 0))
    lock_node = lock_node->next;

  if (lock_node == NULL) {
    LockNode *fhead = NULL;
    LockNode *p = g_lock[entry];
    logErr("Add more lock nodes.\n");
    while (p->next != NULL)
      p = p->next;
    fhead = p;
    for (j = 0; j < 32; j++) { // allocate more nodes
      if ((lock_node = Lock_create()) == NULL)
        return 0;
      lock_node->prev = p;
      p->next = lock_node;
      p = lock_node;
    }
    while ((fhead != NULL) && (fhead->use != 0))
      fhead = fhead->next;
    lock_node = fhead;
  }

  if (lock_node->use != 0)
    return 0;
  lock_node->use = 1;
  strcpy(lock_node->cdkey, cdkey);
  strcpy(lock_node->server, server);
#ifdef _LOCK_ADD_NAME
  strcpy(lock_node->name, name);
#endif
  lock_node->process = process;
#ifdef _SASQL
  sasql_online(cdkey, name, NULL, NULL, 2);
#endif
  return 1;
}

int DeleteMemLock(int entry, char *cdkey, int *process) {
  LockNode *lock_node = g_lock[entry];

  logErr("删除内存信息 位置=%x 账号=%s ..", entry, cdkey);

  while (lock_node != NULL) {
    if (lock_node->use != 0) {
      if (strcmp(lock_node->cdkey, cdkey) == 0)
        break;
    }
    lock_node = lock_node->next;
  }
  if (lock_node != NULL) {
    lock_node->use = 0;
    memset(lock_node->cdkey, 0, sizeof(lock_node->cdkey));
    memset(lock_node->server, 0, sizeof(lock_node->server));
#ifdef _LOCK_ADD_NAME
    memset(lock_node->name, 0, sizeof(lock_node->name));
#endif
    *process = lock_node->process;
    logErr("删除成功\n");
#ifdef _SQL_BACKGROUND
    sasql_online(cdkey, NULL, NULL, NULL, 0);
#endif
    return 1;
  }
  logErr("删除失败!!\n");
  return 0;
}

void DeleteMemLockServer(const char *sname) {
  int i;
  LockNode *lock_node;
  for (i = 0; i < 256; i++) {
    lock_node = g_lock[i];
    while (lock_node != NULL) {
      if (lock_node->use != 0) {
        if (strcmp(lock_node->server, sname) == 0) {
          lock_node->use = 0;
        }
      }
      lock_node = lock_node->next;
    }
  }
}

int isMemLocked(int entry, char *cdkey) {
  LockNode *lock_node = g_lock[entry];
  while (lock_node != NULL) {
    if (lock_node->use != 0) {
      if (strcmp(lock_node->cdkey, cdkey) == 0) {
        if (!strcmp(lock_node->server, "星系移民"))
          logErr("星系移民中");
        break;
      }
    }
    lock_node = lock_node->next;
  }
  if (lock_node != NULL)
    return 1;
  else
    return 0;
}

int GetMemLockState(int entry, char *cdkey, char *result) {
  LockNode *lock_node = g_lock[entry];

  while (lock_node != NULL) {
    if (lock_node->use != 0) {
      if (strcmp(lock_node->cdkey, cdkey) == 0) {
        sprintf(result, "%s 是在 %s 被锁的.", cdkey, lock_node->server);
        return 1;
      }
    }
    lock_node = lock_node->next;
  }
  sprintf(result, "%s 没有被锁.", cdkey);
  return 0;
}

int GetMemLockServer(int entry, char *cdkey, char *result) {
  LockNode *lock_node = g_lock[entry];
  while (lock_node != NULL) {
    if (lock_node->use != 0) {
      if (strcmp(lock_node->cdkey, cdkey) == 0) {
        strcpy(result, lock_node->server);
        return 1;
      }
    }
    lock_node = lock_node->next;
  }
  return 0;
}

int LockNode_getGname(int entries, char *id, char *gname) {
  LockNode *lock_node = g_lock[entries];
  while (lock_node != NULL) {
    if (lock_node->use != 0) {
      if (!strcmp(lock_node->cdkey, id)) {
        sprintf(gname, "%s", lock_node->server);
        return 1;
      }
    }
    lock_node = lock_node->next;
  }
  return 0;
}
