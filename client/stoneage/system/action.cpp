#define __ACTION_CPP__
#include "systeminc/system.h"

void InitAction(void) {
  pActTop = (ACTION *)CALLOC(1, sizeof(ACTION));
  pActBtm = (ACTION *)CALLOC(1, sizeof(ACTION));
  if (pActBtm == nullptr || pActTop == nullptr) {
    // TODO: 记录异常
    exit(-1);
  }
  memset(pActTop, 0, sizeof(ACTION));
  memset(pActBtm, 0, sizeof(ACTION));
  /* Top是哨兵头 */
  pActTop->pPrev = NULL;
  pActTop->pNext = pActBtm;
  pActTop->func = NULL;
  pActTop->prio = PRIO_TOP;
  pActTop->bmpNo = -1;
  /* Btm是哨兵尾 */
  pActBtm->pPrev = pActTop;
  pActBtm->pNext = NULL;
  pActBtm->func = NULL;
  pActBtm->prio = PRIO_BTM;
  pActBtm->bmpNo = -1;
}

// Action由Top和Btm定义了一个双链表
// 其实是NewAction? 需要根据Prio将Action插入到链表合适的位置
ACTION *GetAction(UCHAR prio, UINT yobiSize) {
  ACTION *pActCurr;
  ACTION *pAct = (ACTION *)CALLOC(1, sizeof(ACTION));
  if (pAct == nullptr) {
    // TODO: 记录异常
    exit(-1);
  }
  memset(pAct, 0, sizeof(ACTION));
  if (yobiSize > 0) {
    pAct->pYobi = CALLOC(1, yobiSize);
    if (pAct->pYobi == NULL) {
      FREE(pAct);
      return NULL;
    }
    memset(pAct->pYobi, 0, yobiSize);
  }
  pAct->func = NULL; // 设置Function
  pAct->prio = prio; // 设置优先级
  pAct->bmpNo = -1;  // 设置图号
  pAct->hitDispNo = -2;  // 这个是啥？
  for (pActCurr = pActTop->pNext; pActCurr != pActBtm->pNext;
       pActCurr = pActCurr->pNext) {
    if (pActCurr->prio > prio) {
      pAct->pPrev = pActCurr->pPrev;
      pAct->pNext = pActCurr;
      pActCurr->pPrev->pNext = pAct;
      pActCurr->pPrev = pAct;
      break;
    }
  }
  return pAct;
}

// 执行所有的动作，当deathFlag设定为TRUE时，会造成此动作的释放
void RunAction(void) {
  ACTION *pActCurr = pActTop->pNext; /* pActTop 和 pActBtm 是哨兵节点 */
  ACTION *pActNext;                  /* 用于删除当前的Action */
  while (true) {
    if (pActCurr == pActBtm)
      break;
    if (pActCurr->deathFlag == FALSE) {
      if (pActCurr->func != NULL) {
        pActCurr->func(pActCurr);
      }
      pActCurr = pActCurr->pNext;
    } else { // 删除这个已经dead的Action.
      pActCurr->pPrev->pNext = pActCurr->pNext;
      pActCurr->pNext->pPrev = pActCurr->pPrev;
      pActNext = pActCurr->pNext;
      ClearAction(pActCurr);
      pActCurr = pActNext;
    }
  }
}

extern ACTION *ptActMenuWin;
extern short helpBtn;
extern short actBtn;

void DeathAction(ACTION *pAct) {
  if (pAct == NULL)
    return;
  if (ptActMenuWin == pAct) {
    helpBtn = 0;
    actBtn = 0;
  }
  pAct->deathFlag = TRUE;
}

// ?
void DeathAction1(ACTION *pAct) {
  if (ptActMenuWin == pAct) {
    helpBtn = 0;
    actBtn = 0;
  }
  if (pAct == nullptr)
    return;
  pAct->deathFlag = TRUE;
}

// 先释放Yobi, 然后释放自身
void ClearAction(ACTION *pAct) {
  if (pAct->pYobi != nullptr) {
    FREE(pAct->pYobi);
    pAct->pYobi = nullptr;
  }
  FREE(pAct);
  pAct = nullptr;
}

// 只是将所有的Action->deathFlag设置为TRUE
void DeathAllAction(void) {
  ACTION *pActCurr = pActTop->pNext;
  while (pActCurr != pActBtm) {
    pActCurr->deathFlag = TRUE;
    pActCurr = pActCurr->pNext;
  }
}

void EndAction(void) {
  DeathAllAction();
  RunAction();
  FREE(pActTop);
  FREE(pActBtm);
}
