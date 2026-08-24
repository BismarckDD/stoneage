#ifndef __PROCESS_H__
#define __PROCESS_H__

#ifdef __PROCESS_CPP__
#define EXTERN
#else
#define EXTERN extern
#endif /* __PROCESS_CPP__ */

enum {
  PROC_INIT,              // = 0
  PROC_ID_PASSWORD,       // = 1 账号密码界面
  PROC_TITLE_MENU,        // = 2 服务器选择界面
  PROC_CHAR_SELECT,       // = 3 角色选择界面
  PROC_CHAR_MAKE,         // = 4 角色创建界面
  PROC_CHAR_LOGIN_START,  // = 5 登入
  PROC_CHAR_LOGIN,        // = 6 角色登入界面
  PROC_CHAR_LOGOUT,       // = 6 角色登出界面
  PROC_OPENNING,          //
  PROC_GAME,              // 正常游戏的页面
  PROC_BATTLE,            // 游戏战斗的页面
  PROC_DISCONNECT_SERVER, // 和服务器失去连接(掉线)的页面
  PROC_TAKE_TEST,         //
  PROC_OHTA_TEST,         //
  PROC_DWAF_TEST,         //
  PROC_SPR_VIEW,          // ??????囝????
  PROC_ANIM_VIEW,         // ????????囝????
  PROC_SE_TEST,           // ???囝????
#ifdef _80_LOGIN_PLAY
  PROC_80_LOGIN,
#endif
#ifdef _PK2007
  PROC_PKSERVER_SELECT,
#endif
  PROC_ENDING
};

EXTERN UINT ProcNo;
EXTERN UINT SubProcNo;
EXTERN int ProcNo2;
EXTERN int SubProcNo2;
EXTERN int palNo;
EXTERN int oldPalNo;
EXTERN int palTime; // the third

/* ?????? */
void Process(void);
/* ????阪??? */
void ChangeProc(int procNo);
/* ????阪??? */
void ChangeProc(int procNo, int subProcNo);
/* ????阪??? */
void ChangeProc2(int procNo);
/* ????阪??? */
void ChangeProc2(int procNo, int subProcNo);

/* 石器主循环 */
void GameProc(void);

void warpEffectProc(void);
void repairMap(void);
void paletteProc(void);

#ifdef _SURFACE_ANIM
void AniProc(void);
#endif

#endif /* __PROCESS_H__ */
