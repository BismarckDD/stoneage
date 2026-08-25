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

// ============================================================
// GameProc SubProcNo (PROC_GAME)
// ============================================================
enum {
  GAME_SUBPROC_INIT,            // 0  场景初始化
  GAME_SUBPROC_CREATE_WAY_1,    // 1  PRODUCE_4WAY 转场中
  GAME_SUBPROC_CREATE_WAY_2,    // 2  PRODUCE_4WAY 等待完成
  GAME_SUBPROC_FRAME_NORMAL,    // 3  正常游戏主帧
  GAME_SUBPROC_COPY_BACK,       // 4  备份 BackBuffer
  GAME_SUBPROC_FADE_TO_BATTLE,  // 5  PRODUCE_HAGARE_OCHI 转场中
  GAME_SUBPROC_REPAIR_MAP = 20, // 20 修复地图(fastDrawTile)
  GAME_SUBPROC_FIRST_DRAW = 100, // 100 首帧绘制
  GAME_SUBPROC_WAIT_LOGIN,      // 101 等待 loginFlag
  GAME_SUBPROC_INIT_DRAW,       // 102 初始化绘制链路
  GAME_SUBPROC_BRAN_FADE,       // 103 PRODUCE_BRAN_BIG 转场中
  GAME_SUBPROC_ENTRY_SHORT = 150, // 150 简短入口(直接进 100)
  GAME_SUBPROC_WARP_PREPARE = 200, // 200 warp 转场准备
  GAME_SUBPROC_WARP_PRODUCE,    // 201 warp PRODUCE_CENTER_PRESSIN
  GAME_SUBPROC_WARP_WAIT,       // 202 warp 等待动画完成
  GAME_SUBPROC_WARP_REDRAW,     // 203 warp 重绘地图
  GAME_SUBPROC_WARP_OUT,        // 204 warp PRODUCE_CENTER_PRESSOUT
};

// ============================================================
// idPasswordProc SubProcNo (PROC_ID_PASSWORD)
// ============================================================
enum {
  LOGIN_SUBPROC_INIT,           // 0  初始化界面
  LOGIN_SUBPROC_SEND_CMD,       // 1  发送账号密码
  LOGIN_SUBPROC_WAIT_CHALLENGE, // 2  等待 Challenge
  LOGIN_SUBPROC_WAIT_AUTH,      // 3  等待认证结果
  LOGIN_SUBPROC_CONNECT,        // 4  初始化连接
  LOGIN_SUBPROC_WAIT_CERTIFY,   // 5  等待服务器认证
  LOGIN_SUBPROC_SHOW_CHAR_LIST = 100, // 100 显示角色列表
  LOGIN_SUBPROC_SELECT_CONFIRM, // 101 选择确认
};

// ============================================================
// selectServerProc SubProcNo (PROC_TITLE_MENU / PROC_CHAR_LOGIN)
// ============================================================
enum {
  TITLE_SUBPROC_GROUP_INIT = 200, // 200 Group 选择初始化
};

// ============================================================
// characterLogoutProc SubProcNo (PROC_CHAR_LOGOUT)
// ============================================================
enum {
  LOGOUT_SUBPROC_INIT,          // 0  开始退出
  LOGOUT_SUBPROC_FADE,          // 1  播放退出特效
  LOGOUT_SUBPROC_WAIT,          // 2  等待退出完成
};

EXTERN UINT ProcNo;
EXTERN UINT SubProcNo;
EXTERN int ProcNo2;
EXTERN int SubProcNo2;
EXTERN int palNo;
EXTERN int oldPalNo;
EXTERN int palTime; // the third

void Process(void);
void ChangeProc(int procNo);
void ChangeProc(int procNo, int subProcNo);
void ChangeProc2(int procNo);
void ChangeProc2(int procNo, int subProcNo);

/* 石器主循环 */
void GameProc(void);

void warpEffectProc(void);
void repairMap(void);
void paletteProc(void);

void StartWarpProduce(void);

#ifdef _SURFACE_ANIM
void AniProc(void);
#endif

#endif /* __PROCESS_H__ */
