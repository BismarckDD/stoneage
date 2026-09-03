#define __GAME_MAIN_CPP__
#include "systeminc/system.h"
#include <winuser.h>
#include <winbase.h>
#include "systeminc/loadrealbin.h"
#include "systeminc/loadsprbin.h"
#include "oft/sys2.h"
#include "sdk/caryime.h"
#include "systeminc/ime_sa.h"
#include "game/anim_tbl.h"
#include "game/battle_map.h"
#include "systeminc/netmain.h"
#include "systeminc/savedata.h"
#include "systeminc/produce.h"
#include "game/battle_proc.h"
#include "systeminc/t_music.h"
#include "systeminc/menu.h"
#include "systeminc/handletime.h"
#include "game/battle_menu.h"
#include "systeminc/character.h"
#include "systeminc/login.h"
#include "systeminc/pc.h"
#include "systeminc/map.h"
#include "proto/autil.h"
#include "proto/protocol.h"
#include "systeminc/field.h"
#include "mylua/winlua.h"
#ifdef _OPTIMIZATIONFLIP_
#include <thread> 
#include <chrono>
#endif
#ifdef _CMD_MODE
#include <fcntl.h>
#endif
#ifdef _REMAKE_20
#include "sdk/mmogprotect.h"
#endif
#ifdef _TALK_WINDOW
#include "systeminc/talkwindow.h"
#endif
#ifdef _PTTERN_SEPARATION_BIN
#include "other/PtternSeparationBin.h"
#endif
#ifdef _AIDENGLU_
#include "systeminc/pc.h"
extern Landed PcLanded;
#endif

int 是否重开登组队_1;

int MessageBoxNew(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

#ifdef _OPTIMIZATIONFLIP_
#define NO_DRAW_MAX_CNT 5    // ??????????
#else
#define NO_DRAW_MAX_CNT 6    // ??????????
//#define FRAME_SKIP    1        // ???????? ??：?  ???：?
#endif

void ChangeWindowMode(void);
void SetgResolutionMode(int Mode);
void StrToNowStrBuffer(char *str);
void StrToNowStrBuffer1(char *str);

#define _BATTLESLEPP_ 0
static char no_wait_cnt = 0;
int debug_info_flag = 0;

int FrameRate;
int DrawFrameCnt;
DWORD DrawFrameTime;

#ifdef _OPTIMIZATIONFLIP_
// 过程时间
DWORD ProcTime = 8;
// 循环时间 
DWORD SystemTime = 16;
#else
// 过程时间
DWORD ProcTime = 14;
// 循环时间 
#ifdef _SYSTEM_SLEEP_
DWORD SystemTime = _SYSTEM_SLEEP_;
#else
DWORD SystemTime = 6;
#endif
#endif


#ifdef _NEW_SPEED_
BOOL GameSpeedFlag = FALSE;
#endif
int NoDrawCnt = 1;
int BackBufferDrawType;
#ifdef _READ16BITBMP
typedef struct sa_cpu_info {
  bool avx2;
  bool sse41;
  bool mmx;
} g_sa_cpu_info;
#endif
#ifdef _REMAKE_20
static DWORD dwDisableInputScriptTime = 0;    // 每五分钟执行一次DisableInputScriptTime()
#endif

//DWORD WINAPI CheckTickCount(LPVOID lpParam);
#ifdef _CMD_MODE 
void InitConsoleWindow(void);
void ConsoleThread(void);
#endif

#ifdef _CHECK_GAMESPEED
void HighSpeedCheck( void )
{
#define HIGH_SPEED_CHECK_INTERVAL 60*60
    static int SpeedCnt=0;
    static float delays=0;
    if( init_net == FALSE ) return;
    SpeedCnt++;
    if( SpeedCnt >= HIGH_SPEED_CHECK_INTERVAL ){
        int delaytimes = lssproto_getdelaytimes();
        if( delaytimes > 0 ){
            lssproto_setdelaytimes( 0);
            delays += delaytimes*10;
        }else{
            if( delays > 0 ) delays--;
        }
        SpeedCnt = 0;
        lssproto_CS_send( sockfd);    //送探针给server
    }
    if( (int)delays > 0 ){
        for( int i=0; i<((int)delays+1000); i++){}
    }
}
#endif

bool GameMain(void)
{
    if (GameInit() == false){
        char msg[200];
        sprintf(msg, "[%s: %d] GameInit失败.", __FILE__, __LINE__);
        MessageBoxNew(hWnd, msg, "确定", MB_OK | MB_ICONSTOP);
        return false;
    }
    char sz[32];
    DWORD nowtime;
    DWORD nowttime1 = TimeGetTime();
    DWORD nowttime3 = TimeGetTime();

#ifdef _OPTIMIZATIONFLIP_
    BOOL finishLoop = TRUE;
    unsigned int startloopTime = TimeGetTime();
    unsigned int endloopTime;
    unsigned int endProcTime;

    unsigned int loopAmend = 0;
    unsigned int skipAmend = 0;
    extern BOOL  isXP();
    BOOL isXPSystem = isXP();
#endif
    CreatFontHdc();

// 2026.08.18 Game 主循环 @Franklin
    while (true){

        // SystemTask用于从系统获取消息
        if (SystemTask() == FALSE){
            GameEnd();
            return false;
        }

#ifdef _OPTIMIZATIONFLIP_
        if (finishLoop)
        {
            startloopTime = TimeGetTime();
            endloopTime = startloopTime + SystemTime;
            endProcTime = startloopTime + ProcTime;
            finishLoop = FALSE;
        }
#endif
#ifdef _CHECK_GAMESPEED
        HighSpeedCheck();
#endif
        RealTimeToSATime(&SaTime);
        DispBuffer.DispCnt = 0;
        FontCnt = 0;

        if (!offlineFlag)
            networkLoop();
        
        cdda_play(cdda_no);
        // 读取按键？
        joy_read();
        // F12: 用于截屏, 加一个时间判断，防止反复截屏
        if (joy_trg[1] & JOY_F12){
            static unsigned int prevSnapShotTime = 0;
            if (prevSnapShotTime + 500 <= TimeGetTime()){
                snapShot();
                prevSnapShotTime = TimeGetTime();
            }
        }
        // F11: 是否显示角色名称
        if (joy_trg[1] & JOY_F11)
            nameOverTheHeadFlag = !nameOverTheHeadFlag;
        Process();
        MouseProc();
#ifdef _SURFACE_ANIM
        AniProc();
#endif
#ifdef _ANNOUNCEMENT_
        if (ProcNo == PROC_GAME){
            if (!actBtn){
                announce();
            }
        }
#endif
        SortDispBuffer();
        HitMouseCursor();
        PaletteProc();
        if (joy_trg[1] & JOY_F9  && WindowMode && LowResoCmdFlag){

            if (BackBufferDrawType != DRAW_BACK_PRODUCE){
                if (lpDraw != NULL){
                    // DirectDraw ?
                    ReleaseDirectDraw();
                    DeleteObject(hFont);
                    SetgResolutionMode(((gResolutionMode + 1) % 5));
                    InitDirectDraw();
                    InitOffScreenSurface();
                    InitFont(0);
                    ChangeWindowMode();
                    if (InitPalette() == FALSE){
                        PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    }
                    if (ProcNo == PROC_BATTLE){
                        DispBuffer.DispCnt = 0;
                        FontCnt = 0;
                        ReadBattleMap(BattleMapNo);
                        ClearBackSurface();
#ifdef __SKYISLAND
                        fastDrawTileFlag = 0;
                        PutBmp();
                        fastDrawTileFlag = 1;
#else
                        PutBmp();
                        lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
#endif
                        DispBuffer.DispCnt = 0;
                        FontCnt = 0;
                        NowTime = TimeGetTime();
                    }
                    else
                    if (fastDrawTile && ProcNo == PROC_GAME){
                        repairMap();
                    }
                }
            }
        }


#ifdef __NEW_CLIENT_MEM
#ifndef __NEW_CLIENT_ONLY_WRITE
        static DWORD reset = TimeGetTime() + 120000;
        static DWORD flip = 1;
        DWORD oldprotect;
        if (reset < TimeGetTime()){
            char *newread, *newwrite;
            reset += 120000;
            flip = (flip + 1) & 1;
            if (flip == 1){
                if ((newread = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
                    if (net_readbuflen){
                        VirtualProtect(net_readbuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);
                        CopyMemory(newread, net_readbuf, net_readbuflen);
                    }
                    VirtualFree(net_readbuf, NULL, MEM_RELEASE);
                    net_readbuf = newread;
                    VirtualProtect(net_readbuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
                }
            }
            else{
                if ((newwrite = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
                    if (net_writebuflen){
                        VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);
                        CopyMemory(newwrite, net_writebuf, net_writebuflen);
                    }
                    VirtualFree(net_writebuf, NULL, MEM_RELEASE);
                    net_writebuf = newwrite;
                    VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
                }
            }
        }
#else
        static DWORD reset = TimeGetTime() + 120000;
        DWORD oldprotect;
        if( reset < TimeGetTime()){
            char *newwrite;
            reset += 120000;
            if( (newwrite=(char*)VirtualAlloc( NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
                VirtualProtect( net_writebuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);
                CopyMemory( newwrite, net_writebuf, net_writebuflen);
                VirtualFree( net_writebuf, NULL, MEM_RELEASE); 
                net_writebuf = newwrite;
                VirtualProtect( net_writebuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
                newwrite = NULL;
            }
        }
#endif
#endif

#ifdef _OPTIMIZATIONFLIP_
        if (endProcTime > TimeGetTime())
        {
            //修正最大跳帧数
            if (NoDrawCnt < NO_DRAW_MAX_CNT + skipAmend)
            {
                NoDrawCnt++;
                continue;
            }
            else
            {
                while (endProcTime > TimeGetTime())
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        }
#ifdef _MO_SHOW_FPS
        sprintf(sz, "skip frames: %d", skipAmend);
        StockFontBuffer(380, 10, 1, 1, sz, false);    
#endif

#else
        NowTime = TimeGetTime(); //获取当前的时间戳(ms)
        nowtime = nowttime1;
        static DWORD OldTime = nowttime1;
        if (NowTime > (nowtime + ProcTime)){
            if (OldTime != nowtime){
                OldTime = nowtime;
                if (NoDrawCnt < NO_DRAW_MAX_CNT){
                    if (NowTime>nowttime3 + 2000){
                        nowttime3 = NowTime;
                        nowtime = NowTime + SystemTime;
                    }
                    else
                        nowtime += (SystemTime);
                    nowttime1 = nowtime;
                    nowtime = 0;
                    NoDrawCnt++;
                    Sleep(0);
                    continue;
                }
            }
        }
#endif
        nowtime = 0;
        //cary AI的切换
        if (di_key[DIK_NEXT] & 0x80){
            di_key[DIK_NEXT] &= ~0x80;
            static DWORD switchmodetime = TimeGetTime();
            if ((switchmodetime + 500) < NowTime){
                switchmodetime = NowTime;
#ifdef __AI
                if (AI == AI_SELECT){
                    AI = AI_NONE;
                    pc.etcFlag &= (~PC_AI_MOD);
                    lssproto_FS_send(sockfd, pc.etcFlag);
                    // 修复战斗AI转换的BUG  AI->玩家
#ifdef _FIX_BATTLE_AI
                    battleMenuFix();
#endif
                }
                else{
                    AI = AI_SELECT;
                    pc.etcFlag |= PC_AI_MOD;
                    lssproto_FS_send(sockfd, pc.etcFlag);
                }
                extern BOOL AI_Save();
                AI_Save();
#else
                extern int battleWazaTargetBak;
                if (PROC_BATTLE != ProcNo || BATTLE_SUBPROC_CMD_INPUT != SubProcNo){
                    battleWazaTargetBak = -1;
                    AI++;
                    if (AI > AI_SELECT)
                        AI = AI_NONE;
                }
                else{
                    if (AI == AI_NONE){
                        battleWazaTargetBak = -1;
                        AI = AI_ATTACK;
                    }
                }
#endif
            }
        }
        else if (di_key[DIK_PRIOR] & 0x80){
            di_key[DIK_PRIOR] &= ~0x80;
            static DWORD switchmodetime = TimeGetTime();
            if ((switchmodetime + 500) < NowTime){
                switchmodetime = NowTime;
#ifdef __AI
                if (AI == AI_SELECT){
                    AI = AI_NONE;
                    // 修复战斗AI转换的BUG  玩家->AI
#ifdef _FIX_BATTLE_AI
                    battleMenuFix();
#endif
                    pc.etcFlag &= (~PC_AI_MOD);
                    lssproto_FS_send(sockfd, pc.etcFlag);
                }
                else{
                    AI = AI_SELECT;
                    pc.etcFlag |= PC_AI_MOD;
                    lssproto_FS_send(sockfd, pc.etcFlag);
                }
                extern BOOL AI_Save();
                AI_Save();
#else
                extern int battleWazaTargetBak;
                if (PROC_BATTLE != ProcNo || BATTLE_SUBPROC_CMD_INPUT != SubProcNo){
                    battleWazaTargetBak = -1;
                    AI--;
                    if( AI < AI_NONE)
                        AI = AI_SELECT;
                }else{
                    if( AI == AI_NONE){
                        battleWazaTargetBak = -1;
                        AI = AI_SELECT;
                    }
                }
#endif
            }
        }

#ifndef __SKYISLAND
        switch( BackBufferDrawType ){

        case DRAW_BACK_NON:
            break;

        case DRAW_BACK_NORMAL:
            ClearBackSurface();
            break;

        case DRAW_BACK_PRODUCE:
            break;

        case DRAW_BACK_BATTLE:    // ???????

            DrawBattleMap();
            break;

        }
#else
        if (BackBufferDrawType != DRAW_BACK_PRODUCE){
#endif
#ifdef _NEW_SPEED_
            no_wait_cnt++;
            no_wait_cnt &= 3;
#ifdef _OPTIMIZATIONFLIP_
            loopAmend = TimeGetTime();
#endif
            PutBmp();
#ifdef _OPTIMIZATIONFLIP_
            loopAmend = TimeGetTime() - loopAmend;
            if (loopAmend < 8)
            {
                skipAmend = 0;
            }
            else if (loopAmend >=8)
            {
                skipAmend = 1;
            }        
#endif
            baseXFastDraw = nowXFastDraw;
            baseYFastDraw = nowYFastDraw;
            baseXFastDraw2 = nowXFastDraw2;
            baseYFastDraw2 = nowYFastDraw2;

#else
            PutBmp();
            baseXFastDraw = nowXFastDraw;
            baseYFastDraw = nowYFastDraw;
            baseXFastDraw2 = nowXFastDraw2;
            baseYFastDraw2 = nowYFastDraw2;

#endif //_SPEED
#ifdef __SKYISLAND
        }
#endif
        Flip();    // ????????
#ifdef _NEW_SPEED_
        //nowtime = atoi( sz);
        //nowtime ^= 0xffffbcde;
#ifndef _OPTIMIZATIONFLIP_
        nowtime = nowttime1;
        while (nowtime >= TimeGetTime()){
            Sleep(1);
        }
        nowtime = 0;
#endif
#else
        nowtime = atoi( sz);
        nowtime ^= 0xffffbcde;
        while( nowtime >= TimeGetTime() ){
            Sleep(1);
        }
        nowtime = 0;

#endif

        if (PalChangeFlag == TRUE){

#ifdef __NEW_CLIENT
            extern HANDLE hProcessSnap, hParentProcess;
            extern DWORD dwPID;
            extern PROCESSENTRY32 pe32;
            if( dwPID){
                pe32.dwSize = sizeof(PROCESSENTRY32);
                if( Process32First( hProcessSnap, &pe32)){ 
                    do{
                        if (pe32.th32ProcessID == dwPID){
                            if (!strstr(pe32.szExeFile, "explorer.exe") && (hParentProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))){
#ifndef NO_TERMINATER
                                TerminateProcess(hParentProcess, 0);
#endif
                                CloseHandle(hParentProcess);
                            }
                            break;
                        }
                    }while (Process32Next(hProcessSnap, &pe32));
                }
                dwPID = 0;
            }
#endif
            extern int displayBpp;
            extern int rBitLShift, gBitLShift, bBitLShift;
            extern int rBitRShift, gBitRShift, bBitRShift;
            if (displayBpp >= 16){
#ifdef _HI_COLOR_32
                if (displayBpp == 32){
                    extern unsigned int highColor32Palette[256];
                    for (int i = 1; i < 256; i++){
                        highColor32Palette[i] =
                            //cary 2001 10 16
                            ((Palette[i].peBlue >> bBitRShift) << bBitLShift)
                            + ((Palette[i].peGreen >> gBitRShift) << gBitLShift)
                            + ((Palette[i].peRed >> rBitRShift) << rBitLShift);
                    }
                }
                else
#endif
                if (displayBpp == 16){
                    extern unsigned short highColorPalette[256];
                    for (int i = 1; i < 256; i++){
                        highColorPalette[i] =
                            ((Palette[i].peBlue >> bBitRShift) << bBitLShift)
                            + (((Palette[i].peGreen >> gBitRShift) | 1) << gBitLShift)
                            + ((Palette[i].peRed >> rBitRShift) << rBitLShift);
                    }
                }
#ifdef __HI_COLOR
                InitSpriteInfo();
                InitSurfaceInfo();
                extern int BackBufferDrawType;
                if (BackBufferDrawType == DRAW_BACK_BATTLE){
#ifdef _NEW_RESOMODE
                    //drawMap();
                    ReadBattleMap(BattleMapNo);
#else
                    ReadBattleMap(BattleMapNo);
#endif
                    SortDispBuffer();
                    ClearBackSurface();
                    fastDrawTileFlag = 0;
                    PutBmp();
                    fastDrawTileFlag = 1;
                }
                else repairMap();
#endif
#ifdef __CARYTEST
                extern LPREALBIN    g_lpRealHead;
                LPREALBIN node = g_lpRealHead;
                while (node){
                    node->state = 0;
                }
#endif
            }
            else if (displayBpp == 8){
                lpDraw->lpPALETTE->SetEntries(0, 0, 256, Palette);
            }
            PalChangeFlag = FALSE;
        }
#ifdef _NEW_SPEED_
        if (GameSpeedFlag){
            nowtime = TimeGetTime();
            nowttime1 = nowtime;
            //_itoa_s( nowtime^0xffffbcde, sz, 10);
            GameSpeedFlag = FALSE;
        }
#endif
        //nowtime = atoi( sz);
        //nowtime ^= 0xffffbcde;

#ifdef    _OPTIMIZATIONFLIP_
        finishLoop = TRUE;
        NoDrawCnt = 1;

        while (endloopTime > TimeGetTime()){
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
#else
        nowttime1 += SystemTime;
        nowtime = 0;
        Sleep(1);
        NoDrawCnt = 1;
#endif
        SurfaceDate++;
    }
    return false;
}

void DisplayFrameRate(void)
{
  if (TimeGetTime() - DrawFrameTime >= 1000){
    FrameRate = DrawFrameCnt;
    DrawFrameTime = TimeGetTime();
    DrawFrameCnt = 0;
  }
}

bool GameInit(void)
{
  InitDInput();
#ifdef _REMAKE_20
  DisableCheated();
  RestoreLibrary();
  InitRestore();
  SetTimer(hWnd, 0, 55, NULL);
#endif
  util_Init();
  if (InitDirectDraw() == FALSE){
      MessageBoxNew(hWnd, "初始化DirectDraw失败！", "确定", MB_OK | MB_ICONSTOP);
      return false;
  }
  if (InitOffScreenSurface() == FALSE){
      MessageBoxNew(hWnd, "初始化OffScreanSurface失败！", "确定", MB_OK | MB_ICONSTOP);
      return false;
  }
  if (InitPalette() == FALSE) {
      MessageBoxNew(hWnd, "初始化调色板失败！", "确定", MB_OK | MB_ICONSTOP);
      return false;
  }
  if (initRealbinFileOpen(realBinName, adrnBinName) == FALSE) {
      MessageBoxNew(hWnd, "开启Real.bin失败！", "确定", MB_OK | MB_ICONSTOP);
      return false;
  }
  initAutoMapColor(adrnBinName);
  if (InitSprBinFileOpen(sprBinName, sprAdrnBinName) == FALSE){
      MessageBoxNew(hWnd, "开启Spr.bin失败！", "确定", MB_OK | MB_ICONSTOP);
      return false;
  }
#ifdef _READ16BITBMP
    int ref;
    if ((ref = InitRealTruebinFileOpen(realtrueBinName, adrntrueBinName)) < 0){
        if (ref == -1) MessageBoxNew(hWnd, "开启 adrntrue.bin 失败！", "确定", MB_OK | MB_ICONSTOP);
        else if (ref == -2) MessageBoxNew(hWnd, "开启 realtrue.bin 失败！", "确定", MB_OK | MB_ICONSTOP);
        return false;
    }
#endif

#ifdef _PTTERN_SEPARATION_BIN
#ifdef _SA_VERSION_25
    InitPteernSeparationBin(".//path");
#endif
#endif

#ifdef _WIN_LUAJIT_
    memset(gmsv, 0, sizeof(GameServer)*MAX_GMSV);
    memset(gmgroup, 0, sizeof(GameGroup)*MAX_GMGROUP);
    LoadStoneAgeLUA(_LUA_PATCH_);
#endif
    InitAction();
    InitFont(0);
    srand(TimeGetTime());
    initRand2();
    t_music_init();

    if (!InitIme(hWnd, StrToNowStrBuffer)){
        MessageBoxNew(hWnd, "初始化输入法失败！", "确定", MB_OK | MB_ICONSTOP);
        return false;
    }
    MouseInit();
    if (loadUserSetting() == FALSE){
        char msg[1024];
        sprintf_s(msg, "加载用户设置失败 (code = %d)!", savedataErrorCode);
        MessageBoxNew(hWnd, msg, "确定", MB_OK | MB_ICONSTOP);
        return false;
    }
    LoadChatRegistyStr();
    NowTime = TimeGetTime();
    DrawFrameTime = TimeGetTime();
    if (MouseCursorFlag == FALSE){
#ifdef _TALK_WINDOW
        g_iCursorCount = ShowCursor(FALSE);
#else
        ShowCursor(FALSE);
#endif
    }

    ProcNo2 = -1;
#ifdef __NEW_CLIENT_MEM
#ifndef __NEW_CLIENT_ONLY_WRITE
    if (!net_readbuf){
        if (!(net_readbuf = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
            MessageBoxNew(hWnd, "net_readbuf Init 失败!", "确定", MB_OK | MB_ICONSTOP);
            return false;
        }
    }
#else
    if( !net_readbuf){
        if( !(net_readbuf=(char*)VirtualAlloc( NULL, NETBUFSIZ, MEM_COMMIT, PAGE_NOACCESS))){
            MessageBoxNew( hWnd, "net_readbuf Init 失败!", "确定", MB_OK | MB_ICONSTOP );
            return false;
        }
    }
#endif
    if (!net_writebuf){
        if (!(net_writebuf = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_NOACCESS))){
            MessageBoxNew(hWnd, "net_writebuf Init 失败!", "确定", MB_OK | MB_ICONSTOP);
            return false;
        }
    }
#else
    if (!net_readbuf){
        if (!(net_readbuf = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
            MessageBoxNew(hWnd, "net_readbuf Init 失败!", "确定", MB_OK | MB_ICONSTOP);
            return false;
        }
    }
    if (!net_writebuf){
        if (!(net_writebuf = (char*)VirtualAlloc(NULL, NETBUFSIZ, MEM_COMMIT, PAGE_READWRITE))){
            MessageBoxNew(hWnd, "net_writebuf Init 失败!", "确定", MB_OK | MB_ICONSTOP);
            return false;
        }
    }
#endif
#ifdef _CHANNEL_MODIFY
    CreateDirectory("chat\\", NULL);
#endif
    return true;
}

#ifdef _CHANNEL_MODIFY
void SaveChatData(char *msg, char KindOfChannel, bool bCloseFile);
#endif

void GameEnd(void)
{
#ifdef __NEW_CLIENT
    extern HANDLE hPing;
    extern SOCKET sockRaw;
    if (hPing){
        closesocket(sockRaw);
        sockRaw = INVALID_SOCKET;
        TerminateThread(hPing, 0);
        CloseHandle(hPing);
        hPing = NULL;
    }
#endif
    if (net_readbuf){
        VirtualFree(net_readbuf, NULL, MEM_RELEASE);
        net_readbuf = NULL;
    }
    if (net_writebuf){
        VirtualFree(net_writebuf, NULL, MEM_RELEASE);
        net_writebuf = NULL;
    }
    //DirectDraw ?
    ReleaseDirectDraw();
    // ???????????
    EndDInput();
    // DirectSound ?
    t_music_end();
    // ????
    cdda_stop();
    // Rialbin ??
    cleanupRealbin();
    // ????????????
    DeleteObject(hFont);
    // ?????
    EndIme();
    // ????????
    EndAction();
    // ?????????
    cleanupNetwork();
    // ?????????????
    ReleaseMutex(hMutex);
    // ????????
    saveUserSetting();
    // ????????
    SaveChatRegistyStr();

#ifdef _CHANNEL_MODIFY
    SaveChatData(NULL, 0, true);
#endif
    util_Release();
}
#ifdef _CMD_MODE


HANDLE hConsoleThread;
DWORD ConsoleThreadID;
volatile bool is_console_thread_running;

void ConsoleThread(void)
{
    char cmd[256];
    memset(cmd, 0, sizeof(cmd));
    int c;

    while(is_console_thread_running)
    {
        printf("\nStoneAge:\\> ");
        *cmd = 'a ';
        fflush(stdin);
        if( strcmp(cmd,"q") == 0)
        {
            is_console_thread_running = false;
            FreeConsole();
            ExitThread(ConsoleThreadID);
            return;
        }
        if ( cmd[0] == 'a' && cmd[1] == ' ' )
        {
            int level;
            sscanf_s(cmd,"a %i",&level);
            if ( level >= 0 && level <= 14 )
            {
                printf("SUCCESS: Acceleration level has been switched to %i.", level);
                continue;
            }
            else
            {
                printf("ERROR: Acceleration level out of range(0-14).");
                continue;
            }
        }
        if (cmd[0] == 'l' && cmd[1] == ' ')
        {
            char addr[32];
            char port[6];
            char* p,*next_p;
            p = NULL;
            next_p = NULL;
            p = strtok_s(cmd," ", &next_p);
            if(p) {
                p = strtok_s(NULL," ",&next_p);
                if(p) {
                    sprintf_s(addr,sizeof(addr),p);
                    p = strtok_s(NULL," ",&next_p);
                    if(p) {
                        if(strtok_s(NULL," ",&next_p)) {
                            continue;
                        } else {
                            sprintf_s(port,sizeof(port),p);
                            printf("%s %s",addr,port);
                            continue;
                        }
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }
        }
    }
}


// Bind stdin && stdout to the console window this function created.
void InitConsoleWindow(void)  
{
    int hCrt;
    FILE *hf;
    
    // create a windows console. AllocConsole <-> FreeConsole.
    AllocConsole();
    SetConsoleTitle("石器时代 (Built By @Franklin 2024.12.30).");

    // _O_TEXT is a specific optimization for Microsoft platform. O_TEXT is more generalized.
    // unify (O_TEXT && _O_TEXT) these flag here (_O_TEXT).
    hCrt = _open_osfhandle( (long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT );
    hf = _fdopen( hCrt, "w" );

    *stdout = *hf;
    setvbuf( stdout, NULL, _IONBF, 0 );

    hCrt = _open_osfhandle( (long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT );
    hf = _fdopen( hCrt, "r" );

    *stdin = *hf;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // fprintf(stdout, const char *) == printf(const char *)
    fprintf(stdout, 
            "===================================\n\
             ======  Console for StoneAge ======\n\
             ===================================\n\
             /*----------COMMAND LINE---------*/");

    hConsoleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleThread, NULL, 0, &ConsoleThreadID);
    if (hConsoleThread != NULL)
        is_console_thread_running = true;
}

#endif

#ifdef _ANNOUNCEMENT_
void announce()
{
    if (gAnnouncementNum == 0) return;
    int showcolor;
    gAnnouncementTime += 4;
    int left = ((gAnnouncementTime / 25) % (800 + (strlen(gAnnouncementContent) * 8)));
    if (left == 0){
        gAnnouncementNum--;
        if (gAnnouncementColor == -1){
            showcolor = rand() % 10;
        }
        else{
            showcolor = gAnnouncementColor;
        }
    }
    StockFontBuffer(800 - left, 60, 1, showcolor, gAnnouncementContent, 0);
}
#endif

