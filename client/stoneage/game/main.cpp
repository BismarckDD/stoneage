#define __MAIN_CPP__
#include "systeminc/system.h"
#include <winnls32.h>
#include <ime.h>
#include <imm.h>
#include <tlhelp32.h>
#include "sdk/caryime.h"
#include "resource.h"
#include "game/battle_map.h"
#include "game/battle_proc.h"
#include "systeminc/login.h"
#include "systeminc/map.h"
#include "systeminc/text_encoding.h"

#ifdef _REMAKE_20
#include "sdk/mmogprotect.h"
#endif

#ifdef _TALK_WINDOW
#include "systeminc/talkwindow.h"
#endif

#include "wgs/message.h"

#include "systeminc/pc.h"
#include <io.h>
#include <direct.h>
#include <Nb30.h>
#pragma comment(lib, "netapi32.lib")
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

#ifdef _AIDENGLU_
// 自动登录和断线重登状态由多个模块共享，因此在主模块中提供唯一实体。
Landed PcLanded;
int 是否重登 = FALSE;
int 是否重登战斗了 = FALSE;
int 是否重登AI模式 = FALSE;
int 是否重登组队 = FALSE;
int 是否重登喊话 = FALSE;
int 是否重登人物方向 = FALSE;
int 是否重开登组队 = FALSE;
int 自动登陆是否开启 = 0;
#endif

#define REALBIN_DIR "data/real_136.bin"       // REAL_136.BIN
#define ADRNBIN_DIR "data/adrn_136.bin"       // ADRN_136.BIN
#define SPRBIN_DIR "data/spr_115.bin"         // SPR_115.BIN
#define SPRADRNBIN_DIR "data/spradrn_115.bin" // SPRADRN_115.BIN

#ifdef _READ16BITBMP
#define REALTRUEBIN_DIR "data/realtrue_13.bin"
#define ADRNTRUEBIN_DIR "data/adrntrue_5.bin"
#endif
//-------------------------------------------END------------------------
int MessageBoxNew(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    return MessageBoxGbk(hWnd, lpText, lpCaption, uType);
}


BOOL NoDelay = FALSE; // TCP_NODELAY
RECT g_clientRect;
POINT g_clientPoint;
RECT g_moveRect;
int g_iMallocCount = 0;

#ifdef _TALK_WINDOW
extern int MouseCursorFlag;
#endif

void GetSettings();
/* ???????? */
void AnalyzeCmdLine(void);
// ?????????????
void ChangeWindowMode(void);
// ???????
void SetgResolutionMode(int Mode);

// ?????????
WNDCLASS wndclass;

HANDLE hProcessSnap = NULL, hParentProcess = NULL;
DWORD dwPID;
PROCESSENTRY32 pe32;

#ifdef _TIME_GET_TIME
extern LARGE_INTEGER tickCount;
extern LARGE_INTEGER CurrentTick;
#endif

#ifdef _TALK_WINDOW
CTalkWindow TalkWindow;
#endif

int getMAC(char *mac)
{
    NCB ncb;
    typedef struct _ASTAT_
    {
        ADAPTER_STATUS adapt;
        NAME_BUFFER NameBuff[30];
    } ASTAT, *PASTAT;

    ASTAT Adapter;

    typedef struct _LANA_ENUM
    {
        UCHAR length;
        UCHAR lana[MAX_LANA];
    } LANA_ENUM;

    LANA_ENUM lana_enum;
    UCHAR uRetCode;
    memset(&ncb, 0, sizeof(ncb));
    memset(&lana_enum, 0, sizeof(lana_enum));
    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer = (unsigned char *)&lana_enum;
    ncb.ncb_length = sizeof(LANA_ENUM);
    uRetCode = Netbios(&ncb);

    if (uRetCode != NRC_GOODRET)
        return uRetCode;

    for (int lana = 0; lana < lana_enum.length; lana++)
    {
        ncb.ncb_command = NCBRESET;
        ncb.ncb_lana_num = lana_enum.lana[lana];
        uRetCode = Netbios(&ncb);
        if (uRetCode == NRC_GOODRET)
            break;
    }

    if (uRetCode != NRC_GOODRET)
        return uRetCode;

    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_enum.lana[0];
    strcpy((char *)ncb.ncb_callname, "*");
    ncb.ncb_buffer = (unsigned char *)&Adapter;
    ncb.ncb_length = sizeof(Adapter);
    uRetCode = Netbios(&ncb);

    if (uRetCode != NRC_GOODRET)
        return uRetCode;

    sprintf(mac, "%02X-%02X-%02X-%02X-%02X-%02X",
            Adapter.adapt.adapter_address[0],
            Adapter.adapt.adapter_address[1],
            Adapter.adapt.adapter_address[2],
            Adapter.adapt.adapter_address[3],
            Adapter.adapt.adapter_address[4],
            Adapter.adapt.adapter_address[5]);
    return 0;
}


void 获取机器码()
{
    HANDLE m_hMapFile = OpenFileMapping( // 获得共享内存句柄
        FILE_MAP_READ | FILE_MAP_WRITE,
        FALSE,
#ifdef _SA_VERSION_25
        "shiqies");
#endif

    if (m_hMapFile == NULL)
    {
        HANDLE m_hMapFile = CreateFileMapping( // 创建一个有名的共享内存
            (HANDLE)0xFFFFFFFF,                // 0xFFFFFFFF表示创建一个进程间共享的对象
            NULL,
            PAGE_READWRITE, // 读写共享
            0,
            1032, // 共享区间大小4096
#ifdef _SA_VERSION_25
            "shiqies");
#endif

        char *str = (char *)MapViewOfFile( // 映射到本进程的地址空间
            m_hMapFile,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0);
        memset(str, 0, 1032);
        memset(gMachineData, 0, 512);
        char userName[MAX_PATH];
        DWORD size = MAX_PATH;
        char mac[64];
        GetComputerName(userName, &size);
        getMAC(mac);
        sprintf_s(gMachineData, "%s%s", mac, userName);
        memcpy(str, gMachineData, 512);
    }
    else
    {
        char *str = (char *)MapViewOfFile( // 映射到本进程的地址空间
            m_hMapFile,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0);
        memcpy(gMachineData, str, 512);
    }
}

BOOL IsFileExist(char *path, char *name)
{
    char 文件路径[256];
    sprintf(文件路径, "%s%s", path, name);
    BOOL ret = (access(文件路径, 0) == 0 ? TRUE : FALSE);
    if (ret)
    {
        ; // MessageBox(NULL,文件路径,"石器时代",0);
    }
    return ret;
}

BOOL DetectKey(char *path)
{
    return (IsFileExist(path, "QMScript") ||
        IsFileExist(path, "Recorder.exe") ||
        IsFileExist(path, "QMColorActionCtl.ocx") ||
        IsFileExist(path, "ShieldModule.dat") ||
        IsFileExist(path, "qmacro.ini") ||
        IsFileExist(path, "wqm.exe") ||
        IsFileExist(path, "Fairy_Ape") ||
        IsFileExist(path, "LAScriptX.dll") ||
        IsFileExist(path, "mly.dll") ||
        IsFileExist(path, "v5_hook.dll") ||
        IsFileExist(path, "v5_Log.dll") ||
        IsFileExist(path, "v5_Process_Manager.dll")) ? TRUE : FALSE;
}

void KeyDetection()
{
    char szProcessName[MAX_PATH];
    HMODULE hMods[4096];
    HANDLE hProcess;
    DWORD aProcesses[4096], cbNeeded, cbMNeeded;
    char szPath[256];
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return;

    for (int i = 0; i < (int)(cbNeeded / sizeof(DWORD)); i++)
    {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
        BOOL bret = EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
        if (bret)
        {
            DWORD dwret = GetModuleFileNameEx(hProcess, hMods[0], szProcessName, sizeof(szProcessName));
            CloseHandle(hProcess);
            if (dwret == 0)
                break;
            else
            {
                int i = strlen(szProcessName);
                for (i; i >= 0; i--)
                {
                    if (szProcessName[i] == '\\')
                        break;
                }
                memcpy(szPath, szProcessName, i + 1);
                szPath[i + 1] = 0;
                if (DetectKey(szPath))
                    ExitProcess(NULL);
            }
        }
    }
    return;
}

BOOL IsContainsProcess(char *strProName, BOOL 判断 = 1)
{
    PROCESSENTRY32 pe32;        // 定义结构体变量来保存进程的信息
    pe32.dwSize = sizeof(pe32); // 填充大小
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 创建快照
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        // MessageBox("进程快照失败","提示",MB_OK);
        exit(1);
    }

    // 遍历所有快照
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        if (判断 == 1)
        {
            if (strcmp(strProName, pe32.szExeFile) == 0)
            {
                return TRUE;   // 如果存在该进程，则返回TRUE
                bMore = FALSE; // 停止循环
            }
            else
            {
                bMore = ::Process32Next(hProcessSnap, &pe32);
            }
        }
        else
        {
            if (strstr(pe32.szExeFile, strProName))
            {
                return TRUE;   // 如果存在该进程，则返回TRUE
                bMore = FALSE; // 停止循环
            }
            else
            {
                bMore = ::Process32Next(hProcessSnap, &pe32);
            }
        }
    }
    // 扫尾
    CloseHandle(hProcessSnap);
    return FALSE;
}
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
    PCONTEXT ctx = ep->ContextRecord;

    ctx->Ebx = -1;                       // Not running VPC
    ctx->Eip += 4;                       // skip past the "call VPC" opcodes
    return EXCEPTION_CONTINUE_EXECUTION; // we can safely resume execution since we skipped faulty instruction
}

// high level language friendly version of IsInsideVPC()
bool IsInsideVPC()
{
    bool rc = false;

    __try
    {
        // Virtual PC backdoor probe. Keep each instruction intact; automatic
        // C++ formatters can otherwise split the operands and break compilation.
        _asm push ebx
        _asm mov ebx, 0
        _asm mov eax, 1
        _asm __emit 0Fh
        _asm __emit 3Fh
        _asm __emit 07h
        _asm __emit 0Bh
        _asm test ebx, ebx
        _asm setz [rc]
        _asm pop ebx
    }
    __except (IsInsideVPC_exceptionFilter(GetExceptionInformation()))
    {
    }

    return rc;
}

BOOL 检测WARE()
{
    HKEY key;
    unsigned char buffer[512];
    DWORD len;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Services\\Disk\\Enum", 0, KEY_READ, &key) == ERROR_SUCCESS)
    {
        memset(buffer, 0, 512);
        len = 512;
        DWORD lpReserved = 0;
        DWORD lpType = REG_SZ;
        if (RegQueryValueEx(key, "0", NULL, &lpType, buffer, &len) == 0)
        {
            extern int strncmpi(char *s1, char *s2, int len);
            _strlwr_s((char *)buffer, strlen((char *)buffer) + 1);
            if (strstr((char *)buffer, "virtual") || strstr((char *)buffer, "vmware") || strstr((char *)buffer, "vbox"))
            {
                return TRUE;
            }
        }
        RegCloseKey(key);
    }
    return FALSE;
}

class A;

void fcn(A *);

class A
{
public:
    virtual void f() = 0;
    A() { fcn(this); }
};

class B : A
{
    void f() {}
};

void fcn(A *p)
{
    p->f();
}

int PureVirtualFunc()
{
    B b;
    int n = 0;
    return 0;
}

BOOL IsTheSelfRun(char *str)
{
    HANDLE m_hMutex = CreateMutex(NULL, FALSE, str);
    if (m_hMutex)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(m_hMutex);
            return TRUE;
        }
    }
    return FALSE;
}

void CreateCompatibleDEVMODE(DEVMODE *pdm, DWORD BitsPerPixel, DWORD Width, DWORD Height, DWORD Frequency)
{
    ZeroMemory(pdm, sizeof(DEVMODE));
    pdm->dmSize = sizeof(DEVMODE);

    if (BitsPerPixel)
    {
        pdm->dmBitsPerPel = BitsPerPixel;
        pdm->dmFields |= DM_BITSPERPEL;
    }

    if (Width)
    {
        pdm->dmPelsWidth = Width;
        pdm->dmFields |= DM_PELSWIDTH;
    }

    if (Height)
    {
        pdm->dmPelsHeight = Height;
        pdm->dmFields |= DM_PELSHEIGHT;
    }

    if (Frequency)
    {
        pdm->dmDisplayFrequency = Frequency;
        pdm->dmFields |= DM_DISPLAYFREQUENCY;
    }
}

// 入口
int PASCAL WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{

#ifndef SWITCH_MODE
  extern int displayBpp;
  HDC hDcDest = GetDC(hWnd);
  displayBpp = GetDeviceCaps(hDcDest, BITSPIXEL);
  ReleaseDC(hWnd, hDcDest);
  if(displayBpp != 32){
      DEVMODE dm;
      CreateCompatibleDEVMODE(&dm, 32, GetDeviceCaps(hDcDest, HORZRES),GetDeviceCaps(hDcDest, VERTRES), GetDeviceCaps(hDcDest, VREFRESH));
      ChangeDisplaySettings(&dm, 0);
      char buf[256];
      GetModuleFileName(NULL,buf,sizeof(buf));
      STARTUPINFO StartInfo;
      PROCESS_INFORMATION procStruct;
      memset(&StartInfo, 0, sizeof(STARTUPINFO));
      StartInfo.cb = sizeof(STARTUPINFO);
      CreateProcess(buf," OpenClient",NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&StartInfo,&procStruct);
      Sleep(3000);
      return FALSE;
  }
#endif
    SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    CreateMutex(NULL, FALSE, SA_MUTE);

    获取机器码();
    hInst = hInstance;
    CmdShow = nCmdShow;
    CmdLine = lpCmdLine;
    // 建立一个核心物件, 让更新程式可以判断是否有石器正在执行
    hCheckMutex = CreateMutex(NULL, FALSE, "CheckForUpdate");

#ifdef __NEW_CLIENT
    dwPID = GetCurrentProcessId();
    if (INVALID_HANDLE_VALUE == (hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL)))
        return 0;
    {
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &pe32))
        {
            do
            {
                if (pe32.th32ProcessID == dwPID)
                {
                    dwPID = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
    }
#endif

#ifdef _DEFENSETOOENNUM_
    FILE *tempfile = NULL;
    int ds = 0;
    char strname[128];

    BOOL checkclientflg = FALSE;
    char *pathvar;
    pathvar = getenv("TEMP");
    for (ds = 0; ds < _DEFENSETOOENNUM_; ds++)
    {
#ifdef _SA_VERSION_25
        sprintf(strname, "%s\\Etemp%d", pathvar, ds);
#endif
        tempfile = fopen(strname, "w");
        if (tempfile)
            fclose(tempfile);
        HANDLE file_handle;
        file_handle = CreateFile(strname,
                                 GENERIC_READ,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 NULL,
                                 NULL);
        if (INVALID_HANDLE_VALUE != file_handle)
        {
            if (INVALID_FILE_SIZE != (DWORD)file_handle)
            {
                checkclientflg = TRUE;
                break;
            }
        }
    }
    //    int xiangeshu=0;
    //    int ds=0;
    //    char strname[127];
    //    for(ds=0;ds <_DEFENSETOOENNUM_ ;ds++){
    //        sprintf(strname,"%s%d",SA_MUTE,ds);
    //        if(IsTheSelfRun(strname)) xiangeshu++;
    //        else break;
    //    }
    if (!checkclientflg)
    {
        //    sprintf_s(strname, "游戏限制%d开！", _DEFENSETOOENNUM_);
        sprintf_s(strname, "游戏限制2开！");
        MessageBoxNew(NULL, strname, DEF_APPNAME, MB_OK | MB_ICONSTOP);
        return FALSE;
    }
#endif

    if (!hPrevInstance)
    {
        wndclass.style = CS_BYTEALIGNCLIENT | CS_DBLCLKS;
        wndclass.lpfnWndProc = WindMsgProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(SA_ICON));
        wndclass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(SA_MOUSE1));
        wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndclass.lpszMenuName = NULL;
        // Window class names remain ASCII; visible text is set through UTF-16 APIs.
        wndclass.lpszClassName = DEF_APPNAME;
        if (!RegisterClass(&wndclass))
        {
            MessageBoxNew(NULL, "初始化视窗失败！\n，请执行重新安装或洽询客服人员！",
                          "确定", MB_OK | MB_ICONSTOP);
            return FALSE;
        }
    }
#ifdef _TIME_GET_TIME
    QueryPerformanceFrequency(&tickCount);
    tickCount.QuadPart = tickCount.QuadPart / 1000;
#endif
    strcpy(realBinName, REALBIN_DIR);
    strcpy(adrnBinName, ADRNBIN_DIR);
    strcpy(sprBinName, SPRBIN_DIR);
    strcpy(sprAdrnBinName, SPRADRNBIN_DIR);
#ifdef _READ16BITBMP
    strcpy(realtrueBinName, REALTRUEBIN_DIR);
    strcpy(adrntrueBinName, ADRNTRUEBIN_DIR);
#endif
    // 启动器传入的服务器地址、端口和显示选项在这里解析。
    AnalyzeCmdLine();
    memset(lpCmdLine, 0, strlen(lpCmdLine));
    // 必须先确定逻辑分辨率，再创建/调整窗口；后续 DirectDraw 初始化会使用这里的尺寸。
    SetgResolutionMode(gResolutionMode);
    ChangeWindowMode();
#ifdef _TALK_WINDOW
    TalkWindow.Init(hWnd, hInstance);
    if (g_bTalkWindow)
        TalkWindow.Create();
#endif

    // 进入主循环:登录,选择服务器,选择橘色,地图和战斗都由该函数按ProcNo进行Dispatch。
    /* !!! */
    GameMain();

#ifdef __NEW_CLIENT
    CloseHandle(hProcessSnap);
#endif

#ifdef SWITCH_MODE
    extern DEVMODE g_OriginalMode;
    if (g_OriginalMode.dmSize != 0)
    {
        DEVMODE DevMode;
        DevMode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
        if (DevMode.dmPelsWidth != g_OriginalMode.dmPelsWidth || DevMode.dmPelsHeight != g_OriginalMode.dmPelsHeight || DevMode.dmBitsPerPel != g_OriginalMode.dmBitsPerPel)
        {
            g_OriginalMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            ChangeDisplaySettingsEx(NULL, &g_OriginalMode, NULL, CDS_UPDATEREGISTRY, NULL);
        }
    }
#endif
    ReleaseMutex(hCheckMutex);
    return FALSE;
}

void DebugKey(char *str)
{
    char *work;
    int i = 0;
    if ((work = strstr(str, "KEY0:")))
    {
        work += 5;
        while (*work != ':')
        {
            DebugKey0[i] = *work++;
            i++;
            if (i == 255)
            {
                i = 0;
                break;
            }
        }
    }
    DebugKey0[i] = NULL;
    i = 0;
    if ((work = strstr(str, "KEY1:")))
    {
        work += 5;
        while (*work != ':')
        {
            DebugKey1[i] = *work++;
            i++;
            if (i == 255)
            {
                i = 0;
                break;
            }
        }
    }
    DebugKey1[i] = NULL; // ??
    i = 0;
    if ((work = strstr(str, "KEY2:")))
    {
        work += 5;
        while (*work != ':')
        {
            DebugKey2[i] = *work++;
            i++;
            if (i == 255)
            {
                i = 0;
                break;
            }
        }
    }
    DebugKey2[i] = NULL; // ??
}

void AnalyzeCmdLine(void)
{
    char *addr;
    if (addr = strstr(CmdLine, "realbin:"))
        sprintf_s(realBinName, "data/real_%d.bin", atoi(addr + strlen("realbin:")));
    if (addr = strstr(CmdLine, "adrnbin:"))
        sprintf_s(adrnBinName, "data/adrn_%d.bin", atoi(addr + strlen("adrnbin:")));
    if (addr = strstr(CmdLine, "sprbin:"))
        sprintf_s(sprBinName, "data/spr_%d.bin", atoi(addr + strlen("sprbin:")));
    if (addr = strstr(CmdLine, "spradrnbin:"))
        sprintf_s(sprAdrnBinName, "data/spradrn_%d.bin", atoi(addr + strlen("spradrnbin:")));
#ifdef _READ16BITBMP
    if (addr = strstr(CmdLine, "realtrue:"))
        sprintf_s(realtrueBinName, "data/realtrue_%d.bin", atoi(addr + strlen("realtrue:")));
    if (addr = strstr(CmdLine, "adrntrue:"))
        sprintf_s(adrntrueBinName, "data/adrntrue_%d.bin", atoi(addr + strlen("adrntrue:")));
    if (strstr(CmdLine, "usealpha"))
        g_bUseAlpha = TRUE; // ???
#endif
#ifdef _REMAKE_20
#ifdef _STONDEBUG_
    if (strstr(CmdLine, "windowmode"))
        WindowMode = TRUE;
#endif
#else
    if (strstr(CmdLine, "windowmode"))
        WindowMode = TRUE;
#endif
    if (strstr(CmdLine, "nodelay"))
        NoDelay = FALSE;

    if (strstr(CmdLine, "offline"))
    {
        ProcNo = PROC_GAME;
        offlineFlag = TRUE;
    }
    else
    {
#ifndef _80_LOGIN_PLAY
        ProcNo = PROC_ID_PASSWORD;
#else
        ProcNo = PROC_80_LOGIN;
#endif
        offlineFlag = FALSE;
    }
    if (strstr(CmdLine, "PROC_OHTA_TEST"))
    {
        ProcNo = PROC_OHTA_TEST;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "PROC_TAKE_TEST"))
    {
        ProcNo = PROC_TAKE_TEST;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "PROC_DWAF_TEST"))
    {
        ProcNo = PROC_DWAF_TEST;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "PROC_BATTLE"))
    {
        ProcNo = PROC_BATTLE;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "sprview"))
    {
        ProcNo = PROC_SPR_VIEW;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "animview"))
    {
        ProcNo = PROC_ANIM_VIEW;
        offlineFlag = TRUE;
    }
    if (strstr(CmdLine, "setest"))
    {
        ProcNo = PROC_SE_TEST;
        offlineFlag = TRUE;
    }
    // if (strstr(CmdLine, "encountoff")) {
    //    EncountOffFlag = TRUE;
    // }
    if (strstr(CmdLine, "lowreso"))
    {
        gResolutionMode = 1;
    }

#ifdef _LOG_MSG
    if (strstr(CmdLine, "RECVDATA:"))
    {
        char *pt = strstr(CmdLine, "RECVDATA:") + 9;
        char *name;
        name = debugLogFileName;
        for (int i = 0; i < 255; ++i)
        {
            if (*pt != ' ' && *pt != '\0' && *pt != '\t')
                *name++ = *pt++;
            else
                break;
        }
        *name = '\0';
    }
#endif
    DebugKey(CmdLine);
}

void ChangeWindowMode(void)
{
    RECT clientRect;
    DWORD windowStyle;
    ChangeMode(WindowMode);
    if (WindowMode)
    {
        windowStyle = WS_MINIMIZEBOX | WS_SYSMENU |
                      // WS_THICKFRAME |
                      WS_CAPTION | WS_OVERLAPPED;
    }
    else
    {
        windowStyle = WS_VISIBLE | WS_POPUP;
    }
    SetRect(&clientRect, 0, 0, lpDraw->xSize, lpDraw->ySize);
    AdjustWindowRectEx(&clientRect, windowStyle, FALSE, NULL);
    if (hWnd == NULL)
    {
        hWnd = CreateWindowEx(NULL,
                                  DEF_APPNAME,
                                  DEF_APPNAME,
                                  windowStyle,
                                  // CW_USEDEFAULT,
                                  // CW_USEDEFAULT,
                                  0,
                                  0,
                                  clientRect.right - clientRect.left,
                                  clientRect.bottom - clientRect.top,
                                  NULL,
                                  NULL,
                                  hInst,
                                  NULL);
        if (hWnd != NULL)
            SetWindowTextGbk(hWnd, DEF_APPNAME);
    }
    else
    {
        // ???????????
        SetWindowLong(hWnd, GWL_STYLE, windowStyle);
        // ?????????
        ShowWindow(hWnd, CmdShow);
        // ?????????????
        SetWindowPos(hWnd,
                     HWND_NOTOPMOST,
                     0,
                     0,
                     clientRect.right - clientRect.left,
                     clientRect.bottom - clientRect.top,
                     // SWP_SHOWWINDOW );
                     SWP_FRAMECHANGED);
    }
    // ?????????
    ShowWindow(hWnd, CmdShow);
    // ???????
    UpdateWindow(hWnd);
}

void RecoverDirectDraw(void)
{
    // DirectDraw ?
    ReleaseDirectDraw();
    // ?????
    SetgResolutionMode(gResolutionMode);
    // DirectDraw ???
    if (InitDirectDraw() == FALSE)
    {
        MessageBoxNew(hWnd, "DirectDraw 初始化失败！", "确定", MB_OK | MB_ICONSTOP);
        // ???????? WM_CLOSE ??????????
        PostMessage(hWnd, WM_CLOSE, 0, 0L);
    }
    // ??????????
    // ChangeWindowMode();
    // ??????????????
    InitOffScreenSurface();
    // ???????
    if (InitPalette() == FALSE)
    {
        MessageBoxNew(hWnd, "Palette 初始化失败！", "确定", MB_OK | MB_ICONSTOP);
        PostMessage(hWnd, WM_CLOSE, 0, 0L);
    }
    // ??????????????????????
    if (ProcNo == PROC_BATTLE)
    {
        // ???????
        DispBuffer.DispCnt = 0;
        FontCnt = 0;
#ifdef _NEW_RESOMODE
        // drawMap();
        ReadBattleMap(BattleMapNo);
#else
        ReadBattleMap(BattleMapNo);
#endif
        // ???????????????
        ClearBackSurface();
        // ????????????????
#ifdef __SKYISLAND
        fastDrawTileFlag = 0;
        PutBmp();
        fastDrawTileFlag = 1;
#else
        PutBmp();
        // ??????????????????????
        lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
#endif
        // ???????
        DispBuffer.DispCnt = 0;
        FontCnt = 0;
        // ????????
        NowTime = TimeGetTime();
    }
    else if (fastDrawTile && ProcNo == PROC_GAME)
    {
        repairMap(); // ??????
    }
    // ??????????????
    if (lpDraw->lpPALETTE != NULL)
    {
        // ??????????
        if (WindowMode)
        {
            // ?????????????
            lpDraw->lpPALETTE->SetEntries(0, 0, 256, Palette);
        }
    }
    // ????????????
    mouse.state = MOUSE_NO_CRICK;
    mouse.onceState = MOUSE_NO_CRICK;
    // ????????
    NowTime = TimeGetTime();
}
//---------------------------------------------------------------------------//
// Function: SystemTask
// Param: void
//---------------------------------------------------------------------------//
BOOL SystemTask(void)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return FALSE;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}

//---------------------------------------------------------------------------//
// Function:
// Param: HWND hWnd:
//        UINT Message:
//        WPARAM wParam:
//        LPARAM lParam:
//---------------------------------------------------------------------------//
#ifdef _REMAKE_20
extern short mouseCursorMode;
#endif
LRESULT CALLBACK WindMsgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
#ifdef _REMAKE_20
    case WM_TIMER:
        // 重设8253晶片的clock
        RestoreCounter(1196);
        break;
#endif
        /** ??????????? *****************************************************/
    case WM_ACTIVATE: // ????????????????????????????
#ifdef SA_25
        static BOOL bInactive = FALSE;
        if (bInactive && (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE))
        {
            SetWindowPos(hWnd, NULL, 0, -100, 0, 0, NULL);
            ShowWindow(hWnd, SW_MINIMIZE);
            if (lpDraw && lpDraw->lpDD2)
            {
                lpDraw->lpDD2->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
                ReleaseDirectDraw();
            }
        }
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            bInactive = TRUE;
            cleanupNetwork();
            PostQuitMessage(0);
        }
        break;
#endif
        // ?????????????????
        if ((wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) && lpDraw != NULL)
        {

            // ?????? LOST ?????
            if (CheckSurfaceLost() == TRUE)
            {
                // ??????????????
                RecoverDirectDraw();
            }
            // ??????????????
            if (lpDraw->lpPALETTE != NULL)
            {
                // ??????????
                if (WindowMode)
                {
                    // ?????????????
                    lpDraw->lpPALETTE->SetEntries(0, 0, 256, Palette);
                }
            }
        }
        if (keyboad_flg == TRUE)
        {
            if (pDInputDevice != NULL)
            {
                if (wParam == WA_INACTIVE)
                {
                    DInputActiveFlag = FALSE;
                    for (int i = 0; i < 256; i++)
                        di_key[i] = 0;
                    pDInputDevice->Unacquire();
                }
                else
                {
                    // DInput ??
                    DInputActiveFlag = TRUE;
                    pDInputDevice->Acquire();
                }
            }
        }

        if (joy_flg == TRUE)
        {
            if (pDInputDevice2 == NULL)
                break;
            if (wParam == WA_INACTIVE)
            {
                DInputActiveFlag = FALSE;
                pDInputDevice2->Unacquire();
            }
            else
            {
                DInputActiveFlag = TRUE;
                pDInputDevice2->Acquire();
            }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd); // ????????????
        // WM_DESTROY ??????????
        break;
    case WM_DESTROY: // ????????????

        PostQuitMessage(0); // WM_QUIT ???????? ( ???? )
        break;
#ifdef _REMAKE_20
#ifndef _STONDEBUG_
    case WM_ACTIVATEAPP:
        // 如果被切到别的视窗,离开游戏
        if (!wParam)
        {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
#endif
#endif

#if 0            
    case WM_PALETTECHANGED:        // ???????????
        // DirectDraw??????????
        if( lpDraw == NULL ) break;
        // ??????????????
        if( lpDraw->lpPALETTE == NULL ) break;
        // ??????????
        //if( WindowMode ){
        // ?????????????
        lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
        //}
        break;
        //if( (HWND)wParam == hWnd )  break;

    case WM_PALETTEISCHANGING:        // ???????????
        // DirectDraw??????????
        if( lpDraw == NULL ) break;
        // ??????????????
        if( lpDraw->lpPALETTE == NULL ) break;
        // ??????????
        //if( WindowMode ){
        // ?????????????
        lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
        //}
        break;

    case WM_QUERYNEWPALETTE:    // ????????????????????????

        // DirectDraw??????????
        if( lpDraw == NULL ) break;
        // ??????????????
        if( lpDraw->lpPALETTE == NULL ) break;

        // ??????????
        //if( WindowMode ){
        // ?????????????
        lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
        //}

        break;
#endif

    case WM_KEYDOWN: // ?????
        if (SurfaceBusyFlag == TRUE)
        {
            SurfaceBusyFlag = FALSE;
            RecoverDirectDraw();
        }
        switch (wParam)
        {
        case VK_LEFT:
            KeyboardLeft();
            break;

        case VK_RIGHT:
            KeyboardRight();
            break;

        case VK_RETURN:
            KeyboardReturn();
            break;

        case VK_BACK:
            if (joy_con[1] & JOY_RSHIFT || joy_con[1] & JOY_LSHIFT)
            {
                if (pNowStrBuffer != NULL)
                {
                    pNowStrBuffer->cnt = 0;
                    pNowStrBuffer->buffer[0] = NULL;
                }
            }
            else
            {
                KeyboardBackSpace();
            }

            break;

        case VK_DELETE:
            ClearChatBuffer();
#ifdef _TALK_WINDOW
            TalkWindow.ClearChatBuffer();
            TalkWindow.Update();
#endif
            break;

        case VK_ESCAPE:
            break;
        }
#ifdef _TALK_WINDOW
        if (g_bTalkWindow)
            TalkWindow.Update();
#endif
        break;
#ifdef _REMAKE_20
    case WM_HOTKEY:
        // 如果被切到别的视窗,离开游戏
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        break;
#endif
    case WM_SYSKEYDOWN: // ALT????????
        // ???????????
        if (SurfaceBusyFlag == TRUE)
        {
            SurfaceBusyFlag = FALSE;
            MessageBoxNew(hWnd, "SurfaceBusyFlag error!", "确定", MB_OK | MB_ICONSTOP);
            RecoverDirectDraw();
        }
        switch (wParam)
        {
        case VK_RETURN:
#ifdef _REMAKE_20
#ifndef _STONDEBUG_
            // 缩成视窗模式离开游戏
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
#endif
#else
            // ?????
            if (BackBufferDrawType == DRAW_BACK_PRODUCE)
                break;

            // DirectDraw??????????
            if (lpDraw == NULL)
                break;

            // ????????????
            if (WindowMode == TRUE)
                WindowMode = FALSE;
            else
                WindowMode = TRUE;
#ifdef _TALK_WINDOW
            if (g_bTalkWindow)
            {
                g_bTalkWindow = FALSE;
                TalkWindow.Visible(FALSE);
            }
#endif
            ChangeMode(WindowMode);
            // DirectDraw ?
            ReleaseDirectDraw();
            // ?????
            SetgResolutionMode(gResolutionMode);
            // DirectDraw ???
            InitDirectDraw();
            // ??????????????
            InitOffScreenSurface();
            // ??????????
            ChangeWindowMode();
            // ??????????
            // SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
            // ???????
            if (InitPalette() == FALSE)
            {
                // PostMessage( hWnd, WM_CLOSE, 0, 0L );
                PostMessage(hWnd, WM_SYSKEYDOWN, VK_RETURN, 0L);
            }
            // ??????????????????????
            if (ProcNo == PROC_BATTLE)
            {
                // ???????
                DispBuffer.DispCnt = 0;
                FontCnt = 0;
#ifdef _NEW_RESOMODE
                // drawMap();
                ReadBattleMap(BattleMapNo);
#else
                ReadBattleMap(BattleMapNo);
#endif
                // ???????????????
                ClearBackSurface();
                // ????????????????
#ifdef __SKYISLAND
                fastDrawTileFlag = 0;
                PutBmp();
                fastDrawTileFlag = 1;
#else
                PutBmp();
                // ??????????????????????
                lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
#endif
                // ???????
                DispBuffer.DispCnt = 0;
                FontCnt = 0;
                // ????????
                NowTime = TimeGetTime();
            }
            else if (fastDrawTile && ProcNo == PROC_GAME)
            {
                repairMap(); // ??????
            }
            // ??????????????
            if (lpDraw->lpPALETTE != NULL)
            {
                // ??????????
                if (WindowMode)
                {
                    // ?????????????
                    lpDraw->lpPALETTE->SetEntries(0, 0, 256, Palette);
                }
            }
            // ????????????
            mouse.state = MOUSE_NO_CRICK;
            mouse.onceState = MOUSE_NO_CRICK;
            // ????????
            NowTime = TimeGetTime();
            break;
#endif
            break;
        default:
            if (wParam != VK_F10)
            {
                return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;
        }
        break;
    case WM_CHAR: // ????
        // ?????????
        StockStrBufferChar((char)wParam);
        break;
    case WM_MOUSEMOVE: // ????????
        // SetCursor( wndclass.hCursor );
        //  ?????????????
        MouseNowPoint(LOWORD(lParam), HIWORD(lParam));
#ifndef _TALK_WINDOW
        if (mouse.flag == TRUE)
        {
            ShowCursor(FALSE); // ?????????
            mouse.flag = FALSE;
        }
#else
        if (MouseCursorFlag == FALSE && g_iCursorCount > -1)
        {
            while (g_iCursorCount > -1)
                g_iCursorCount = ShowCursor(FALSE);
            mouse.flag = FALSE;
        }
        else if (MouseCursorFlag == TRUE && g_iCursorCount < 0)
        {
            while (g_iCursorCount < 0)
                g_iCursorCount = ShowCursor(TRUE);
        }
#endif
        // ???????????
        if (SurfaceBusyFlag == TRUE)
        {
            SurfaceBusyFlag = FALSE;
            // ??????????????
            RecoverDirectDraw();
        }
        break;
    case WM_NCMOUSEMOVE: // ???????????????
        // ????????????
        mouse.state = MOUSE_NO_CRICK;
        mouse.onceState = MOUSE_NO_CRICK;
        if (mouse.flag == FALSE)
        {
#ifdef _TALK_WINDOW
            g_iCursorCount = ShowCursor(TRUE);
#else
            ShowCursor(TRUE); // ????????
#endif
            mouse.flag = TRUE;
        }
        break;
    case WM_LBUTTONDOWN: // ?????(????)
#ifdef _80_LOGIN_PLAY
        if (ProcNo == PROC_80_LOGIN)
            break;
#endif

#ifdef _REMAKE_20
        // 检查使用者是否真的按下滑鼠
        if (!IsLMouseButtonDown())
            mouseCursorMode = 0;
        else
#endif
            // ????????????????????
            MouseCrickLeftDownPoint(LOWORD(lParam), HIWORD(lParam));

        break;
    case WM_LBUTTONUP: // ?????(????)
        // ????????????????????
        MouseCrickLeftUpPoint(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONDBLCLK: // ????????
#ifdef _80_LOGIN_PLAY
        if (ProcNo == PROC_80_LOGIN)
            break;
#endif
        // ???????????????????????
        MouseDblCrickLeftUpPoint(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_RBUTTONDOWN: // ?????(????)
#ifdef _REMAKE_20
                         // 检查使用者是否真的按下滑鼠
        if (!IsRMouseButtonDown())
            mouseCursorMode = 0;
        else
#endif
            // ????????????????????
            MouseCrickRightDownPoint(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_RBUTTONUP: // ?????(????)
        // ????????????????????
        MouseCrickRightUpPoint(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_RBUTTONDBLCLK: // ????????
        // ???????????????????????
        MouseDblCrickRightUpPoint(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MBUTTONDOWN: // ?????(????)
        break;
    case WM_MOVE:
    {
        g_clientPoint.x = g_clientPoint.y = 0;
        GetClientRect(hWnd, &g_clientRect);
        ClientToScreen(hWnd, &g_clientPoint);
        OffsetRect(&g_clientRect, g_clientPoint.x, g_clientPoint.y);
        SetRect(&g_moveRect, 0, 0, lpDraw->xSize, lpDraw->ySize);
#ifdef _TALK_WINDOW
        if (g_bTalkWindow)
            SendMessage(TalkWindow.GetTalkWindowWND(), WM_MOVE, 0, 0);
#endif
    }
    break;

    // 自订Hook Msg传送Type
#ifdef _SAHOOK // Syu ADD Hook程式
    case UM_KEYEVENT:
        TCHAR ac[2];
        BYTE bKeyState[256];
        GetKeyboardState(bKeyState);
        if (ToAscii(wParam, (HIWORD(lParam) & (0x00FF | KF_UP)), bKeyState, (PWORD)ac, 0) == 1)
        {
            extern int HOOK_PASSWD_NUM;
            if ((HOOK_PASSWD_NUM < 12) && (wParam != 13) && (wParam != 9) && (wParam != 27))
                HOOK_PASSWD_NUM++;
        }
        if (wParam == 8)
            if (HOOK_PASSWD_NUM > 0)
                HOOK_PASSWD_NUM--;
#endif
    default:
        if (ImeMessage(Message, wParam, lParam))
            return 0;
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }

    return 0;
}

void SetgResolutionMode(const int mode)
{
    // 保存当前模式，并重新建立保存 DirectDraw 尺寸和 Surface 指针的结构。
    gResolutionMode = mode;
    lpDraw = (DIRECT_DRAW *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (DWORD)sizeof(DIRECT_DRAW));
    if (lpDraw == NULL)
    {
        MessageBoxNew(hWnd, "HeapAlloc Error ( DIRECT_DRAW )", "确定", MB_OK | MB_ICONSTOP);
        return;
    }
    switch (mode)
    {
    case 0:
        // 标准 640x480 模式，也是大部分旧版界面素材的设计基准。
        lpDraw->xSize = 640;
        lpDraw->ySize = 480;
        SurfaceSizeX = 64;
        SurfaceSizeY = 48;
        DISPLACEMENT_X = 0;
        DISPLACEMENT_Y = 0;
        break;
    case 1:
        // 低分辨率模式。
        lpDraw->xSize = 320;
        lpDraw->ySize = 240;
        SurfaceSizeX = 64;
        SurfaceSizeY = 48;
        DISPLACEMENT_X = 0;
        DISPLACEMENT_Y = 0;
        break;
    case 2:
        // 兼容用的 640x480 模式。
        lpDraw->xSize = 640;
        lpDraw->ySize = 480;
        SurfaceSizeX = 64;
        SurfaceSizeY = 48;
        DISPLACEMENT_X = 0;
        DISPLACEMENT_Y = 0;
        break;
#ifdef _NEW_RESOMODE
    case 3:
        // 800x600：相对 640x480，横向和纵向分别多出 160、120 像素。
        lpDraw->xSize = 800;
        lpDraw->ySize = 600;
        SurfaceSizeX = 64;
        SurfaceSizeY = 48;
        DISPLACEMENT_X = 160;
        DISPLACEMENT_Y = 120;
        break;
    case 4:
        // 1024x768：位移量仍表示相对 640x480 多出的尺寸。
        lpDraw->xSize = 1024;
        lpDraw->ySize = 768;
        SurfaceSizeX = 64;
        SurfaceSizeY = 48;
        DISPLACEMENT_X = 384;
        DISPLACEMENT_Y = 288;
        break;
#endif
    }
    // 同步全局窗口尺寸、屏幕中心和场景观察中心，供 UI 与地图绘制共同使用。
    DEF_APPSIZEX = lpDraw->xSize;
    DEF_APPSIZEY = lpDraw->ySize;
    SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
    SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
    viewOffsetX = SCREEN_WIDTH_CENTER;
    viewOffsetY = SCREEN_HEIGHT_CENTER;
    PostMessage(hWnd, WM_MOVE, 0, 0L);
}
