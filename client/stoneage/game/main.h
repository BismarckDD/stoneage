#ifndef __MAIN_H__
#define __MAIN_H__

#include "version.h"
#include "systeminc/netmain.h"
#include "systeminc/login.h"

LRESULT CALLBACK PASCAL WindMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL SystemTask(void);

#ifdef __MAIN_CPP__
#define EXTERN 
#else
#define EXTERN extern
#endif

#ifdef __MAIN_CPP__
EXTERN int DISPLACEMENT_X = 160;
EXTERN int DISPLACEMENT_Y = 120;
EXTERN int DEF_APPSIZEX = 800;
EXTERN int DEF_APPSIZEY = 600;
EXTERN int SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
EXTERN int SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
// TRUE  : WindowMode
// FALSE : FullScreen
EXTERN BOOL WindowMode = TRUE;
// 客户端逻辑分辨率模式：0/2=640x480，1=320x240，3=800x600，4=1024x768。
// 此值不仅决定窗口大小，也会影响 DirectDraw 后台缓冲区和界面坐标的计算。
EXTERN int gResolutionMode = 0;
EXTERN int LowResoCmdFlag = 0;
EXTERN HINSTANCE hInst;
EXTERN HWND hWnd;
EXTERN int CmdShow;
EXTERN LPSTR CmdLine;
EXTERN HANDLE hMutex;
EXTERN HANDLE hCheckMutex;
#else
EXTERN int DISPLACEMENT_X;
EXTERN int DISPLACEMENT_Y;
EXTERN int DEF_APPSIZEX;
EXTERN int DEF_APPSIZEY;
EXTERN int SCREEN_WIDTH_CENTER;
EXTERN int SCREEN_HEIGHT_CENTER;
EXTERN BOOL WindowMode;
EXTERN int gResolutionMode;
EXTERN int LowResoCmdFlag; // 
EXTERN HINSTANCE hInst;
EXTERN HWND hWnd;
EXTERN int CmdShow;        // WinMain是否以全屏展开
EXTERN LPSTR CmdLine;      // WinMain的Cmd命令行参数
EXTERN HANDLE hMutex;      // 可??去???????????????
EXTERN HANDLE hCheckMutex;
#endif

#define BIN_NAME_LEN 256
EXTERN char realBinName[BIN_NAME_LEN];
EXTERN char adrnBinName[BIN_NAME_LEN];
EXTERN char sprBinName[BIN_NAME_LEN];
EXTERN char sprAdrnBinName[BIN_NAME_LEN];
#ifdef _READ16BITBMP
EXTERN char realtrueBinName[BIN_NAME_LEN];
EXTERN char adrntrueBinName[BIN_NAME_LEN];
#endif
EXTERN GameServer gmsv[MAX_GMSV];
EXTERN GameGroup gmgroup[MAX_GMGROUP];

#ifdef __MAIN_CPP__
BOOL offlineFlag = FALSE;
#else
EXTERN BOOL offlineFlag;
#endif
EXTERN char DebugKey0[256];
EXTERN char DebugKey1[256];
EXTERN char DebugKey2[256];
EXTERN char gMachineData[512];

#endif
