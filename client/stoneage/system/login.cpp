#include "systeminc/system.h"
#include "systeminc/text_encoding.h"
#include "systeminc/process.h"
#include "systeminc/netproc.h"
#include "systeminc/netmain.h"
#include "systeminc/pc.h"
#include "systeminc/tool.h"
#include "systeminc/character.h"
#include "systeminc/loadsprbin.h"
#include "systeminc/savedata.h"
#include "systeminc/chat.h"
#include "sdk/caryime.h"
#include "systeminc/ime_sa.h"
#include "systeminc/menu.h"
#include "systeminc/t_music.h"
#include "systeminc/netmain.h"
#include "systeminc/map.h"
#include "proto/lssproto_cli.h"
#include "proto/protocol.h"
#include "proto/autil.h"
#include "systeminc/field.h"
#include "systeminc/directdraw.h"
#include "game/anim_tbl.h"
#include "systeminc/login.h"
#include "systeminc/produce.h"
#include "wgs/message.h"
#include "wgs/descrypt.h"
#include "mylua/winlua.h"
#ifdef _SHOWIPSLEEP_
#include "other/readip.h"
#endif
#ifdef _SAHOOK // Syu ADD Hook程式
#include "Sa_Hk.h"
#define UM_KEYEVENT (WM_APP + 0)
#endif
#ifdef _SHOWIPSLEEP_
#include "wgs/Ping.h"
#endif

#define SA_VERSION "2.5"
#define SA_VERSION_NUM 300
#ifdef _AIDENGLU_
extern Landed PcLanded;
#endif
int nGroup = 0;
int nServerGroup = 0;
BOOL logOutFlag = FALSE;
unsigned int MsgCooltime = 0;

short createCharFlag = 0;
char szUser[32];
char szPassword[32];

void initInputIdPassword(void);
void initServerSelectionBox(void);
int inputIdPassword(BOOL);
void initCommonMsgWin(void);
int commonMsgWin(char *);
void initCertifyIdPassword(void);
int renderGroupSelectionBox();
int renderServerSelectionBox();
void initConnecGameServer(void);
int connecGameServer(void);
void initDownloadCharList(void);
int downloadCharList(void);
void initDeleteCharacter(void);
int deleteCharacter(void);
int commonYesNoWindow(int, int);
// add code by shan
int fmYesNoWindow(int x, int y, int id);
int fmselectdetuyWindow(int x, int y, int id);
void initStrBuffer(STR_BUFFER *, int, int, int, int, int);
void getStrSplit(char *, char *, int, int, int);
int DelCharGraColorWin(void);

#ifdef _75_LOGIN
static ACTION *aEffect, *aEffect1;
#define RAND(x, y) ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))
#endif
// Terry add 2001/12/07
#ifdef __EDEN_EFFECT
void BankProc(void);
#endif
#ifdef _NEWSHOP_
ACTION *商城动作地址;
#endif
#ifdef _CHARSIGNDAY_
extern ACTION *签到动作地址;
#endif
#ifdef _MAGIC_ITEM_
extern ACTION *MagicItemActAddr;
extern ACTION *ShowMagicItemData();
extern ACTION *CreateMagicItemWin();
extern void InitMagicItemWin(char *内容);
#endif

// Terry end
// Terry add 2002/01/03
#ifdef __EDEN_AUCTION
void AuctionNewWT(void);
void AuctionListWT(void);
void AuctionSurveyModifyWT(void);
void AuctionModifyWT(void);
// server所传来的资料
typedef struct _AUCTION_LIST_DATA
{
    int index;                        // 委托的玩家的索引号码
    char cdkey[CHAR_NAME_LEN + 1];    // 玩家的cdkey
    char account[101];                // 委托内容
    char name[CHAR_NAME_LEN + 1];     // 委托人的名字
    char money[8];                    // 价钱
    int kind;                         // 卖宠物或是物品
    char sellname[ITEM_NAME_LEN + 1]; // 宠物或是物品的名称
    int grp_no;                       // 宠物或是物品的图号
    int lv, act, def, dex, hp, loyal; // 宠物的资料
    int name_color;                   // 道具名称的颜色
    char item_effect_string[64];      // 道具叙述
} ALD, *pALD;

char *szpALD = NULL;                            // server传来的资料内容
BOOL bNewData = FALSE;                          // 是否有新的资料传来
void AuctionGetString(int nWhichOne, pALD ald); // 处理字串的分析
void CheckNumber(char *buf, int num);           // 处理非数字的输入
void CheckSpace(char *buf);                     // 处理空白的输入
#endif

#ifdef _SAHOOK // Syu ADD Hook程式
bool hookflag = false;
#endif

static int selCharGraColorWinProcNo;
short skillShopWindow4ProcNo;
short skillShopWindow1ProcNo;

STR_BUFFER idKey;
STR_BUFFER passwd;
static STR_BUFFER *idPasswordFocus[] = {&idKey, &passwd};
STR_BUFFER *idPasswordStr = &idKey;
short idKeyReturn = 0;
static int idPasswordGraId[] = {-2, -2};
static int idPasswordFocusSw = 0;
static short usernameBoxX, usernameBoxY;
static short passwordBoxX, passwordBoxY;
static short OkButtonX, OkButtonY;
static short QuitButtonX, QuitButtonY;
#ifdef _DELBORNPLACE // Syu ADD 6.0版本后，人物统一出生于新手村
static ACTION *pActPet20;
int BornPetNum = 0;
#endif
#ifdef _PKSERVERCHARSEL // (不可开) Syu ADD PK服务器选择星系人物
ACTION *PkServerSel;
short PkMenuflag = 0;
int VersionXYZ = 1; // Pk Server版本号
#endif
// Nuke 0615: Avoid 7's lock
extern int isWGS7;
#ifdef _LOGINKICK
DWORD StartTime = -1;
#endif
#ifdef _READ16BITBMP
extern BOOL g_bUseAlpha;
#endif
#ifdef _NEW_COLOR_
extern BOOL g_bUseAlpha;
#endif

static char *gButtonList[] = {
    "确  定", "取  消",
    // " ＯＫ ", "CANCEL",
    "确  定", "取  消",
    "上一页", "下一页" };

const int attrColor[4][2] =
{
    {SYSTEM_PAL_GREEN, SYSTEM_PAL_GREEN2},
    {SYSTEM_PAL_AQUA, SYSTEM_PAL_AQUA2},
    {SYSTEM_PAL_RED, SYSTEM_PAL_RED2},
    {SYSTEM_PAL_YELLOW, SYSTEM_PAL_YELLOW2}
};

char data[256];
char msg[256];

const char cPlzInputIdPass[] = "请输入您的帐号与密码！";
const char cCheckingIn[] = "签入中";
const char cLoginingGame[] = "登入游戏中，请稍候！";
const char cCharDeleting[] = "人物删除中";

// 账号密码界面(启动后第一个页面)的生成和控制逻辑
void idPasswordProc(void)
{
    // 登录画面状态机（SubProcNo）：
    // 0 初始化输入框；1 恢复已保存账号；2 等待输入；3 开始连接；4 等待认证结果。
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    BOOL flag = FALSE;
    int ret;
    static char msg[256];
    if (SubProcNo == 0)
    {
        PaletteChange(DEF_PAL, 0);
        SubProcNo++;
        initInputIdPassword();
        idKeyReturn = 0;
    }
    if (SubProcNo == 1)
    {
        static bool isFirstFrame = true;
        if (!isFirstFrame)
        {
            CopyMemory(idKey.buffer, szUser, 32);
            ecb_crypt("f;encor1c", idKey.buffer, 32, DES_DECRYPT);
            CopyMemory(passwd.buffer, szPassword, 32);
            ecb_crypt("f;encor1c", passwd.buffer, 32, DES_DECRYPT);
            idKey.cursor = idKey.cnt = strlen(idKey.buffer);
            passwd.cursor = passwd.cnt = strlen(passwd.buffer);
        } else {
            // 只有第一帧的时候不执行上面的逻辑
            isFirstFrame = false;
        }
        idPasswordFocusSw = 0;
        GetKeyInputFocus(idPasswordFocus[idPasswordFocusSw]);
        SubProcNo++;
    }
    // 只有状态 2 才允许编辑输入框和点击 OK/QUIT，连接期间锁定输入。
    if (SubProcNo == 2)
        flag = TRUE;
    ret = inputIdPassword(flag);
    if (ret == 1)
    {
        SubProcNo = 3;
        play_se(217, 320, 240); // 播放Music
    }
    else if (ret == 2)
    {
        play_se(217, 320, 240);
        PostMessage(hWnd, WM_CLOSE, 0, 0L);
    }
    else if (ret < 0)
    {
        SubProcNo = 100;
        strcpy(msg, cPlzInputIdPass);
    }
    if (SubProcNo == 3)
    {
        w = (getUtf8CharNum(cLoginingGame) + 3) * 9 / 64 + 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        initConnectServer();
        SubProcNo = 4;
#ifdef _75_LOGIN
        if (g_bUseAlpha)
        {
            DeathAction(aEffect);
            DeathAction(aEffect1);
        }
#endif
    }
    if (SubProcNo == 4)
    {
        if (ptActMenuWin != NULL)
        {
            if (ptActMenuWin->hp >= 1)
            {
                if (iWGS == 7)
                    isWGS7 = 1; // Nuke 0615: Avoid 7's lock
                sprintf(msg, "%s(%d)", cLoginingGame, iWGS);
                int len = getUtf8CharNum(msg);
                int xx = (w * 64 - len * 8) / 2;
                int yy = (h * 48 - 16) / 2;
                StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
            }
        }
#ifdef NO_WGS
        int ret2 = 1;
        dwServer = GS;
#else
        int ret2 = ConnectWGS();
#endif
        if (ret2 == 1)
        {
            // 处理Title
            CopyMemory(szUser, idKey.buffer, 32);
            ecb_crypt("f;encor1c", szUser, 32, DES_ENCRYPT);
            CopyMemory(szPassword, passwd.buffer, 32);
            ecb_crypt("f;encor1c", szPassword, 32, DES_ENCRYPT);
            idKey.cnt = 0;
            idKey.cursor = 0;
            ZeroMemory(idKey.buffer, sizeof(idKey.buffer));
            passwd.cnt = 0;
            passwd.cursor = 0;
            ZeroMemory(passwd.buffer, sizeof(passwd.buffer));
            ChangeProc(PROC_TITLE_MENU);
            if (ptActMenuWin)
            {
                DeathAction(ptActMenuWin);
                ptActMenuWin = NULL;
            }
        }
        else if (ret2 == -7)
        {
            SubProcNo = 100;
            strcpy(msg, szError);
        }
        else if (ret2 < 0)
        {
            SubProcNo = 100;
#ifdef _AIDENGLU_
            PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
            strcpy(msg, netprocErrmsg);
        }
        else if (ret2 == -8)
        {
            SubProcNo = 100;
            strcpy(msg, szError);
        }
    }
    if (SubProcNo == 5)
    {
        SubProcNo = 100;
        strcpy(msg, "输入的账号或密码错误\n请再次输入!");
    }
    if (SubProcNo == 100)
    {
        cleanupNetwork();
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 101)
    {
        if (commonMsgWin(msg))
        {
            SubProcNo = 2;
            Sleep(1000);
        }
    }
}

void initInputIdPassword(void)
{
    for (int i = 0; i < sizeof(idPasswordGraId) / sizeof(int); i++)
        idPasswordGraId[i] = -2;
    // 以下坐标以 640 * 480 登录背景为基准；StrBuffer的坐标就是实际文字起点。
    usernameBoxX = 372;
    usernameBoxY = 95;
    initStrBuffer(&idKey, usernameBoxX, usernameBoxY, 18, FONT_PAL_WHITE, FONT_PRIO_BACK);
    idKey.cnt = 0;
    idKey.cursor = 0;
    passwordBoxX = 372;
    passwordBoxY = 135;
    initStrBuffer(&passwd, passwordBoxX, passwordBoxY, 18, FONT_PAL_WHITE, FONT_PRIO_BACK);
    passwd.cnt = 0;
    passwd.cursor = 0;
    // 普通模式使用掩码显示密码，避免明文直接出现在画面上。
    passwd.filterFlag = BLIND_TYPE;
    idPasswordFocusSw = 0;
    GetKeyInputFocus(idPasswordFocus[idPasswordFocusSw]);
}

int inputIdPassword(BOOL flag)
{
    int selOkFlag;
    static BOOL flag2 = FALSE;
    static int oldId = 0;
    int ret = 0;
    int x1, y1, x2, y2, cx, cy;
    // 检查上一帧登记的 OK/QUIT 图形是否被鼠标选中。
    // 返回值：0=继续等待，1=提交登录，2=退出客户端，-1=账号或密码为空。
    int id = selGraId(idPasswordGraId, sizeof(idPasswordGraId) / sizeof(int));
    if (id == 0)
    {
        if (strlen(idKey.buffer) > 0 && strlen(passwd.buffer) > 0)
        {
            idKey.buffer[idKey.cnt] = '\0';
            passwd.buffer[passwd.cnt] = '\0';
            ret = 1;
        }
        else
            ret = -1;
    }
    else if (id == 1)
        ret = 2;
    selOkFlag = flag ? 2 : 0;

    // 将账号和密码文本加入本帧文字绘制队列。
    StockFontBuffer2(&idKey);
    StockFontBuffer2(&passwd);
    // id == 0: usernameBox
    // id == 1: passwordBox
    id = -1;
    if (selOkFlag)
    {
        // 输入框命中区域：点击账号框或密码框时切换键盘输入焦点。
        x1 = usernameBoxX - 6;
        y1 = usernameBoxY - 3;
        x2 = x1 + 140;
        y2 = y1 + 26;
        // 在页面上画出BOX的UI,且将usernameBox画出来
        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX)) id = 0;
        x1 = passwordBoxX - 6;
        y1 = passwordBoxY - 3;
        x2 = x1 + 140;
        y2 = y1 + 26;
        // 在页面上画出BOX的UI,且将passwordBox画出来
        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX)) id = 1;
    }
    if (idKeyReturn)
    {
        // 在账号框按 Enter 后，将焦点移动到密码框。
        id = 1;
        idKeyReturn = 0;
    }

    if (joy_trg[1] & JOY_TAB)
    {
        if (oldId == 0)
            id = 1;
        else if (oldId == 1)
            id = 0;
    }
    if (!flag)
    {
        GetKeyInputFocus(NULL);
        flag2 = TRUE;
    }
    else if ((0 <= id && id <= 1) || flag2)
    {
        if (flag2)
            id = oldId;
        GetKeyInputFocus(idPasswordFocus[id]);
        flag2 = FALSE;
        oldId = id;
    }

    // 请注意cx/cy的作用！！！2026.08.16
    // OK 按钮的鼠标区域，以及按下状态图片的绘制中心。
    x1 = 320;
    y1 = 220;
    x2 = x1 + 60;
    y2 = y1 + 120;
    cx = 320;
    cy = 240;
    if (MakeHitBox(x1, y1, x2, y2, -1) && selOkFlag)
        idPasswordGraId[0] = StockDispBuffer(cx, cy, DISP_PRIO_BG, CG_TITLE_ID_PASS_OK, 1);
    else
        idPasswordGraId[0] = -2;
    // QUIT 按钮的鼠标区域，以及按下状态图片的绘制中心。
    x1 = 420;
    y1 = 230;
    x2 = x1 + 60;
    y2 = y1 + 130;
    cx = 320;
    cy = 240;
    if (MakeHitBox(x1, y1, x2, y2, -1) && selOkFlag)
        idPasswordGraId[1] = StockDispBuffer(cx, cy, DISP_PRIO_BG, CG_TITLE_ID_PASS_QUIT, 1);
    else
        idPasswordGraId[1] = -2;

    // 登录面板背景最后以最低优先级加入队列，按钮和文字会绘制在它上面。
    StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, DISP_PRIO_BG, CG_TITLE_ID_PASS);
    return ret;
}

static short commonMsgWinProcNo = 0;

void initCommonMsgWin(void)
{
    commonMsgWinProcNo = 0;
}

int commonMsgWin(char *msg)
{
    static int fontId[] = {-2};
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
#ifdef _NEW_WGS_MSG // WON ADD WGS的新视窗?
    char temp[256], out_msg[5][128];
    int j, count = 0, temp_size = 0;
#endif
    int msgCharLen = getUtf8CharNum(msg);

    if (commonMsgWinProcNo == 0)
    {
        commonMsgWinProcNo = 1;
        for (int i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
        w = msgCharLen * 8 / 64 + 2;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    int id = selFontId(fontId, sizeof(fontId) / sizeof(int));
    if (id == 0)
    {
        ret = 1;
        play_se(217, 320, 240);
    }
    if (ret != 0 && ptActMenuWin)
    {
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
        return ret;
    }
    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        int len = msgCharLen + 1;
        // 2026.09.07 每个字符占8个像素?
        int xx = (w * 64 - len * 8) / 2;
        StockFontBuffer(x + xx, y + 30, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
        xx = (w * 64 - getUtf8CharNum("ＯＫ") * 8) / 2;
        fontId[0] = StockFontBuffer(x + xx, y + 56, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "ＯＫ", 2);
    }
    return ret;
}

void initStrBuffer(STR_BUFFER *pt, int x, int y, int len, int color, int prio)
{
    pt->x = x;
    pt->y = y;
    pt->len = len;
    pt->color = color;
    pt->fontPrio = prio;
}

// mouse.onceState: 鼠标按下的那一帧, 不可以拖拽
// mouse.autoState: 鼠标按下后，每隔一段时间重复触发
// mouse.state: 鼠标按下，可以拖拽
int selGraId(int *id, int cnt)
{
    if ((mouse.onceState & MOUSE_LEFT_CRICK) == 0)
        return -1;
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitDispNo)
            return i;
    return -1;
}

int selRepGraId(int *id, int cnt)
{
    if ((mouse.autoState & MOUSE_LEFT_CRICK) == 0)
        return -1;
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitDispNo)
            return i;
    return -1;
}

int pushGraId(int *id, int cnt)
{
    // mouse.state == LEFT_CLICK
    if ((mouse.state & MOUSE_LEFT_CRICK) == 0)
        return -1;
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitDispNo)
            return i;
    return -1;
}

int selFontId(int *id, int cnt)
{
    if ((mouse.onceState & MOUSE_LEFT_CRICK) == 0)
        return -1;
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitFontNo)
            return i;
    return -1;
}

int focusGraId(int *id, int cnt)
{
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitDispNo)
            return i;
    return -1;
}

int focusFontId(int *id, int cnt)
{
    for (int i = 0; i < cnt; i++)
        if (id[i] == HitFontNo)
            return i;
    return -1;
}

static short userCertifyErrorMsgWinProcNo = 0;
extern char szAnnouncement[];

// 服务器选择页面
void SelectServerProc(void)
{
    int ret;
    static char msg[256];
    if (SubProcNo == 0)
    {
        char str[128];
        sprintf_s(str, "%s %s", DEF_APPNAME, SA_VERSION);
        SetWindowTextUtf8(hWnd, str);
        if (0 < strlen(szAnnouncement))
            SubProcNo = 200;
        else
            ++SubProcNo;
    }
    if (SubProcNo == 1) // 初始化
    {
        if (initNet())
        {
            SubProcNo = nGroup > 1 ? 2 : 3;
            ProduceInitFlag = TRUE;
            initServerSelectionBox();
        }
        else
        {
            SubProcNo = 100;
            strcpy(netprocErrmsg, NET_ERRMSG_SOCKLIBERROR);
        }
    }
    if (SubProcNo == 2) // 选择Group
    {
        ret = renderGroupSelectionBox();
        if (ret == 1)
            ++SubProcNo;
        else if (ret == 2)
        {
            idPasswordFocusSw = 0;
            GetKeyInputFocus(idPasswordFocus[idPasswordFocusSw]);
            ChangeProc(PROC_ID_PASSWORD);
            SubProcNo = 0;
            cleanupNetwork();
        }
        else if (ret == 3)
        {
            SubProcNo = 100;
            wsprintf(msg, "[%s]您使用的版本无法连接此星系！", gmgroup[nServerGroup].name);
            cleanupNetwork();
        }
    }
    if (SubProcNo == 3) // 选择Server
    {
        ret = renderServerSelectionBox();
        if (ret == 1)
            ++SubProcNo;
        else if (ret == 2)
        {
            if (nGroup < 2)
            {
                idPasswordFocusSw = 0;
                GetKeyInputFocus(idPasswordFocus[idPasswordFocusSw]);
                ChangeProc(PROC_ID_PASSWORD);
                SubProcNo = 0;
            }
            else
                SubProcNo = 2;
        }
    }
    if (SubProcNo == 4) // 初始化连接Server
    {
        initConnecGameServer();
        ++SubProcNo;
    }
    if (SubProcNo == 5) // 
    {
        // 2026.08.19 这一步很关键，涉及网络通信!!!
        ret = connecGameServer();
        if (ret == 1)
            ChangeProc(PROC_CHAR_SELECT);
        else if (ret == -2)
        {
            SubProcNo = 100;
            strcpy(msg, netprocErrmsg);
            cleanupNetwork();
        }
    }
    if (SubProcNo == 100)
    {
        initCommonMsgWin();
        ++SubProcNo;
    }
    if (SubProcNo == 101)
    {
        if (commonMsgWin(msg))
        {
            char str[128];
            SubProcNo = 1;
            sprintf_s(str, "%s %s", DEF_APPNAME, SA_VERSION);
            SetWindowTextUtf8(hWnd, str);
        }
    }
    if (SubProcNo == 200)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 201)
    {
        if (commonMsgWin(szAnnouncement))
            SubProcNo = 1;
    }
    // 标题画面的底图。当前版本使用全屏缩放绘制接口。
    StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, DISP_PRIO_BG, CG_TITLE);
}
// SELECT SERVER WINDOW
ACTION *ptActSelectServerWin = NULL;

inline void initServerSelectionBox(void)
{
    ptActSelectServerWin = NULL;
}

/*return:    0 ... 选择中
1 ... 决定Group
2 ... 按回上一页
3 ... 选择到网咖专用的服务器    */
int renderGroupSelectionBox()
{
    static int fontId[] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    static int x, y;
    int row;
    int id;
    int i;
    int ret = 0;
    if (ptActSelectServerWin == NULL)
    {
        y = 270;
        x = 0;
        row = (nGroup >> 2) + 1;
        if (row == 1)
            x = 256;
        else if (row == 2)
            x = 192;
        else if (row == 3)
            x = 128;
        else if (row == 4)
            x = 64;
        // TODO: 这里之是过的，不知道现在会如何？
        ptActSelectServerWin = MakeWindowDisp(x, y, row << 1, 3, NULL, 1);
        x += 22;
        y += 28;
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
    }
    if (ptActSelectServerWin != NULL)
    {
        // 游标在第几个选项
#ifdef _AIDENGLU_
        extern int 自动登陆是否开启;
#endif
        id = selFontId(fontId, nGroup + 1);
        if ((id >= 0 && id < nGroup)
#ifdef _AIDENGLU_
            || 自动登陆是否开启
#endif
        )
        {
#ifdef _AIDENGLU_
            if (自动登陆是否开启)
                nServerGroup = PcLanded.大区;
            else
#endif
                nServerGroup = id;
            char title[256];
            sprintf_s(title, "%s %s %s", DEF_APPNAME, SA_VERSION, gmgroup[nServerGroup].name);
            SetWindowTextUtf8(hWnd, title);
            DeathAction(ptActSelectServerWin);
            ptActSelectServerWin = NULL;
            play_se(217, 320, 240); // click声
            if (gmgroup[nServerGroup].used == 1)
                return 1;
            else
                return 3;
        }
        else if (id == nGroup)
        {
            DeathAction(ptActSelectServerWin);
            ptActSelectServerWin = NULL;
            play_se(217, 320, 240); // click声
            return 2;
        }
        if (ptActSelectServerWin->hp >= 1)
        {
            int Hit = 0;
            for (i = 0; i < nGroup; i++)
            {
                if (gmgroup[i].used)
                    Hit = 2;
                else
                    Hit = 0;
                fontId[i] = StockFontBuffer(x + (i >> 2) * 128, y + (i % 4) * 25, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmgroup[i].name, Hit);
            }
#ifdef _NEWFONT_
            fontId[i] = StockFontBuffer(x + (i >> 2) * 128 + 10, y + 75, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "  回上一页  ", 2);
#else
            fontId[i] = StockFontBuffer(x + (i >> 2) * 128, y + 75, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "  回上一页  ", 2);
#endif
        }
    }
    return 0;
}

// 渲染服务器选择页面
/*return:    0 ... 选择中
1 ... 决定Server
2 ... 按回上一页
3 ... 选择到网咖专用的服务器    */
int renderServerSelectionBox()
{
    static int fontId[] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    static int x, y;
    int row;
    int id;
    int i;
    int ret = 0;
    // 服务器选择窗口
    if (ptActSelectServerWin == NULL)
    {
        y = 270 + 0;
        x = 0;
        row = (gmgroup[nServerGroup].num >> 2) + 1;
        if (row == 1)
            x = 256;
        else if (row == 2)
            x = 192;
        else if (row == 3)
            x = 128;
        else if (row == 4)
            x = 64;
        // 创建这个窗口
        ptActSelectServerWin = MakeWindowDisp(x, y, row << 1, 3, NULL, 1);
#ifdef _SHOWIPSLEEP_
        extern DWORD WINAPI IP延时读取(PVOID pParam);
        if (IP线程 == NULL)
            IP线程 = CreateThread(NULL, 0, IP延时读取, 0, 0, NULL);
#endif
        x += 22;
        y += 28;
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
    }
    if (ptActSelectServerWin != NULL)
    {
        // 游标在第几个选项
        id = selFontId(fontId, gmgroup[nServerGroup].num + 1);
#ifdef _AIDENGLU_
        extern int 自动登陆是否开启;
#endif
        if (id >= 0 && id < gmgroup[nServerGroup].num
#ifdef _AIDENGLU_
            || 自动登陆是否开启
#endif
        )
        {
            char title[256];
#ifdef _AIDENGLU_
            if (自动登陆是否开启)
                selectServerIndex = gmgroup[nServerGroup].startindex + PcLanded.小区;
            else
#endif
                selectServerIndex = gmgroup[nServerGroup].startindex + id;

            DeathAction(ptActSelectServerWin);
            ptActSelectServerWin = NULL;
            play_se(217, 320, 240); // click声
            return 1;
        }
        else if (id == gmgroup[nServerGroup].num)
        {
            DeathAction(ptActSelectServerWin);
            ptActSelectServerWin = NULL;
            play_se(217, 320, 240); // click声
            return 2;
        }
        if (ptActSelectServerWin->hp >= 1)
        {
            int j = gmgroup[nServerGroup].startindex;
            int Hit = 0;
            for (i = 0; i < gmgroup[nServerGroup].num; i++, j++)
            {
                if ('1' == gmsv[j].used)
                    Hit = 2;
                else
                    Hit = 0;

#ifdef _SHOW_COUNT // WON ADD 秀服务器流量
                fontId[i] = StockFontBuffer(x + (i >> 2) * 128 + 10, y + (i % 4) * 25, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmsv[j].name, Hit);
                {
                    int img = 24324;

                    switch (atoi(gmsv[j].count))
                    {
                    case 1:
                        img = 24324;
                        break;
                    case 2:
                        img = 24327;
                        break;
                    case 3:
                        img = 24326;
                        break;
                    }
                    StockDispBuffer(x + (i >> 2) * 128, y + (i % 4) * 25 + 5, DISP_PRIO_TOP, img, 0);
                }
#else
#ifdef _SHOWIPSLEEP_
                char dechar[64];
                sprintf(dechar, "%d", gmsv[j].delay);
                StockFontBuffer(x + (i >> 2) * 128 + 70, y + (i % 4) * 25, FONT_PRIO_FRONT, FONT_PAL_GREEN, dechar, 0);
#endif
                fontId[i] = StockFontBuffer(x + (i >> 2) * 128, y + (i % 4) * 25, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmsv[j].name, Hit);
#endif
            }
            fontId[i] = StockFontBuffer(x + (i >> 2) * 128, y + 75, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "  回上一页  ", 2);
        }
    }
    return 0;
}

static short connectGameServerProcNo = 0;

inline void initConnecGameServer(void)
{
    connectGameServerProcNo = 0;
}

int connecGameServer(void)
{
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    int ret2;
    static char sServerConnectingStr[256];
    if (connectGameServerProcNo == 0)
    {
        connectGameServerProcNo = 1;
        sprintf_s(sServerConnectingStr, "%s服务器连线中",
            gmsv[selectServerIndex].name);
        w = (getUtf8CharNum(sServerConnectingStr) * 9 + 63) / 64;
        if (w < 2)
            w = 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    if (connectGameServerProcNo == 1)
    {
        cleanupNetwork();
        if (initNet())
        {
            dwServer = GS;
            connectGameServerProcNo = 2;
        }
        else
            ret = -1;
    }
    if (connectGameServerProcNo == 2)
    {
        // 注意这个是初始化connectServerCounter = 0;
        // 而不是 connectGameServerProcNo = 0; !!!!
        initConnectServer();
        connectGameServerProcNo = 3;
    }
    if (connectGameServerProcNo == 3)
    {
        // 正式进入网络连接
        ret2 = connectServer();
        if (ret2 == 1)
            ret = 1;
        else if (ret2 < 0)
            ret = -2;
    }
    if (ret != 0 && ptActMenuWin) {
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
    }
    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        int len = getUtf8CharNum(msg);
        int xx = (w * 64 - len * 8) / 2;
        int yy = (h * 48 - 16) / 2;
        StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
    }
    return ret;
}

// 选择角色页面(二选一)
void selectCharacterProc(void)
{
    int ret;
    int i, j;
    int login;
    int x;
    static char msg[256];
    static int btnGraId[] = {-2, -2, -2, -2, -2, -2, -2};
    int btnUseFlag = 0;

    int x1, y1, x2, y2;

    extern TCHAR 登陆错误内容[];

    if (SubProcNo == 0)
    {
        SubProcNo++;
        initDownloadCharList();
    }
    if (SubProcNo == 1)
    {
        ret = downloadCharList();
        if (ret == 1)
        {
            SubProcNo = 10;
            play_bgm(2);
        }
        else if (ret <= -1 && ret >= -13)
        {
            SubProcNo = 100;
            cleanupNetwork();
            strcpy(msg, 登陆错误内容);
        }
        else if (ret <= -101 && ret >= -105)
        {
            SubProcNo = 100;
            cleanupNetwork();
            strcpy(msg, 登陆错误内容);
        }
        else if (ret <= -201 && ret >= -205)
        {
            SubProcNo = 100;
            cleanupNetwork();
            strcpy(msg, 登陆错误内容);
        }
        else if (ret <= -301 && ret >= -304)
        {
            SubProcNo = 100;
            cleanupNetwork();
            strcpy(msg, 登陆错误内容);
        }
        else if (ret <= -401 && ret >= -402)
        {
            SubProcNo = 100;
            cleanupNetwork();
            strcpy(msg, 登陆错误内容);
        }
    }
    if (SubProcNo == 10)
    {
        for (i = 0; i < sizeof(btnGraId) / sizeof(int); i++)
            btnGraId[i] = -2;
        SubProcNo++;
    }
    if (SubProcNo == 11)
        btnUseFlag = 2;
    if (SubProcNo == 20)
    {
        // cary 按下删除人物后
        ret = DelCharGraColorWin();
        if (ret == 1)
            SubProcNo++;
        else if (ret == 2)
            SubProcNo = 10;
    }
    if (SubProcNo == 21)
    {
        initDeleteCharacter();
        SubProcNo++;
    }
    if (SubProcNo == 22)
    {
        ret = deleteCharacter();
        if (ret == 1)
        {
#ifdef _MORECHARACTERS_
            resetCharacterList(selectPcNo + 多人物当前页数 * 2);
            clearUserSetting(selectPcNo + 多人物当前页数 * 2);
#else
            resetCharacterList(selectPcNo);
            clearUserSetting(selectPcNo);
#endif
            saveNowState();
            if (maxPcNo > 0)
                maxPcNo--;
            SubProcNo = 10;
        }
        else if (ret == -2)
            SubProcNo = 300;
    }
    if (SubProcNo == 100)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    // 回到服务器选择页面
    if (SubProcNo == 101)
    {
        if (commonMsgWin(msg))
        {
            ChangeProc(PROC_TITLE_MENU);
            SubProcNo = 1;
        }
    }
    // 删除人物失败
    if (SubProcNo == 300)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 301)
    {
        if (commonMsgWin("人物无法删除."))
            SubProcNo = 10;
    }
    ret = selGraId(btnGraId, sizeof(btnGraId) / sizeof(int));
    if (ret == 0)
    {
        fade_out_bgm();
        ChangeProc(PROC_TITLE_MENU);
        SubProcNo = 1;
        play_se(217, 320, 240); // click声
    }
    else if (1 <= ret && ret <= 2)
    {
        selectPcNo = ret - 1;
        ChangeProc(PROC_CHAR_MAKE);
#ifdef _PKSERVERCHARSEL // (不可开) Syu ADD PK服务器选择星系人物
        PkMenuflag = 0;
#endif

        play_se(217, 320, 240); // click声
    }
    else if ((3 <= ret && ret <= 4))
    {
        int 选择人物;
        选择人物 = ret - 3;
        char name[CHAR_NAME_LEN + 1];
        strcpy(name, chartable[选择人物].name);
        strcpy(gamestate_login_charname, name);
        selectPcNo = ret - 3;
        newCharacterFaceGraNo = chartable[选择人物].faceGraNo;
        loginDp = chartable[选择人物].dp;
        ChangeProc(PROC_CHAR_LOGIN_START);
        play_se(217, 320, 240); // click声
        map_bgm_no = 0;
        if (ret == 3)
            sCharSide = 1;
        else
            sCharSide = 2;
    }
    else if (5 <= ret && ret <= 6)
    {
        char name[CHAR_NAME_LEN + 1];
        strcpy(name, chartable[ret - 5].name);
        strcpy(gamestate_deletechar_charname, name);
        selectPcNo = ret - 5;
        SubProcNo = 20;
        selCharGraColorWinProcNo = 0;
        play_se(217, 320, 240); // click声
    }

    if (SubProcNo >= 10)
    {
#ifdef _SPECIAL_LOGO
        int ix = 80;
        int iy = 84;
        int ii = 0;
        int backX = 80;
#else
        int ix = 0;
        int iy = 0;
        int ii = 0;
#endif
#ifdef _MORECHARACTERS_
        char buff[256];
        int 按钮ID = StockDispBuffer(563 + ix, 435 + iy, DISP_PRIO_CHAR, 55222, 2);
        if ((mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == 按钮ID))
        {
            if (!多人物当前页数)
                多人物当前页数 = 1;
        }
        按钮ID = StockDispBuffer(72 + ix, 435 + iy, DISP_PRIO_CHAR, 55221, 2);
        if ((mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == 按钮ID))
        {
            if (多人物当前页数)
                多人物当前页数 = 0;
        }
        sprintf_s(buff, "－%d/2－", 多人物当前页数 + 1);
        StockFontBuffer(300 + ix, 453 + iy, 0, 4, buff, 0);
        int b;
        for (b = 0; b < 2; b++)
#else
        for (i = 0; i < MAX_CHARACTER; i++)
#endif
        {
#ifdef _MORECHARACTERS_
            i = 多人物当前页数 * 2 + b;
            if (existCharacterListEntry(i))
            {
                StockDispBuffer(169 + b * (304 + ii) + ix, 84 + iy, DISP_PRIO_CHAR, chartable[i].faceGraNo, 0);
                x = (144 - getUtf8CharNum(chartable[i].name) * 9) / 2;
                StockFontBuffer(93 + b * (304 + ii) + x + ix, 127 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, chartable[i].name, 0);
                sprintf_s(msg, "%3d", chartable[i].level);
                StockFontBuffer(104 + b * (304 + ii) + ix, 152 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].hp);
                StockFontBuffer(104 + b * (304 + ii) + ix, 174 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].str);
                StockFontBuffer(104 + b * (304 + ii) + ix, 196 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].def);
                StockFontBuffer(104 + b * (304 + ii) + ix, 218 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].agi);
                StockFontBuffer(104 + b * (304 + ii) + ix, 240 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].app);
                StockFontBuffer(104 + b * (304 + ii) + ix, 262 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%8d", chartable[i].dp);
                StockFontBuffer(188 + b * (304 + ii) + ix, 262 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                for (j = 0; j < 4; j++)
                {
                    if (chartable[i].attr[j] > 0)
                    {
                        x1 = 208 + b * (304 + ii) + ix;
                        y1 = 177 + j * 22 + iy;
                        x2 = x1 + chartable[i].attr[j] * 8;
                        y2 = y1 + 8;
                        StockBoxDispBuffer(x1, y1, x2, y2, DISP_PRIO_IME2, attrColor[j][0], 1);
                        StockBoxDispBuffer(x1 + 1, y1 + 1, x2 + 1, y2 + 1, DISP_PRIO_IME1, attrColor[j][1], 0);
                        StockBoxDispBuffer(x1 + 2, y1 + 2, x2 + 2, y2 + 2, DISP_PRIO_IME1, attrColor[j][1], 0);
                    }
                }
                login = chartable[i].login;
                if (login >= 1000)
                    login = 9999;
                sprintf_s(msg, "%4d", login);
                StockFontBuffer(156 + b * (304 + ii) + ix, 284 + iy,
                                FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                btnGraId[3 + b] =
                    StockDispBuffer(100 + b * (304 + ii) + ix, 342 + iy,
                                    DISP_PRIO_CHAR, CG_CHR_SEL_LOGIN_BTN, btnUseFlag);
                btnGraId[5 + b] =
                    StockDispBuffer(230 + b * (304 + ii) + ix, 342 + iy,
                                    DISP_PRIO_CHAR, CG_CHR_SEL_DEL_BTN, btnUseFlag);
            }
#else
// 判断 i == (0, 1)角色是否存在
            if (existCharacterListEntry(i))
            {
                // 这里的ii,ix,iy是什么意思？
                StockDispBuffer(88 + i * (304 + ii) + ix, iy, DISP_PRIO_CHAR, chartable[i].faceGraNo, 0);
                int offset_by_name = (144 - getUtf8CharNum(chartable[i].name) * 9) / 2; // char name
                StockFontBuffer(13 + i * (304 + ii) + offset_by_name + ix, 48 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, chartable[i].name, 0);
                sprintf_s(msg, "%3d", chartable[i].level); // level
                StockFontBuffer(24 + i * (304 + ii) + ix, 70 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].hp);  // hp
                StockFontBuffer(24 + i * (304 + ii) + ix, 92 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].str); // strong
                StockFontBuffer(24 + i * (304 + ii) + ix, 114 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].def); // defence
                StockFontBuffer(24 + i * (304 + ii) + ix, 136 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].agi); // aiglity
                StockFontBuffer(24 + i * (304 + ii) + ix, 158 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%3d", chartable[i].app); //
                StockFontBuffer(24 + i * (304 + ii) + ix, 180 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                sprintf_s(msg, "%8d", chartable[i].dp); // dp
                StockFontBuffer(108 + i * (304 + ii) + ix, 180 + iy, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                for (j = 0; j < 4; j++)
                {
                    if (chartable[i].attr[j] > 0)
                    {
                        x1 = 128 + i * (304 + ii) + ix;
                        y1 = 92 + j * 22 + iy;
                        x2 = x1 + chartable[i].attr[j] * 8;
                        y2 = y1 + 8;
                        StockBoxDispBuffer(x1, y1, x2, y2, DISP_PRIO_IME2, attrColor[j][0], 1);
                        StockBoxDispBuffer(x1 + 1, y1 + 1, x2 + 1, y2 + 1, DISP_PRIO_IME1, attrColor[j][1], 0);
                        StockBoxDispBuffer(x1 + 2, y1 + 2, x2 + 2, y2 + 2, DISP_PRIO_IME1, attrColor[j][1], 0);
                    }
                }
                login = chartable[i].login;
                if (login >= 1000) login = 999;
                sprintf_s(msg, "%4d", login);
                StockFontBuffer(76 + i * (304 + ii) + ix, 224 + iy,
                                FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
                btnGraId[3 + i] =
                    StockDispBuffer(20 + i * (304 + ii) + ix, 282 + iy,
                                    DISP_PRIO_CHAR, CG_CHR_SEL_LOGIN_BTN, btnUseFlag);
                btnGraId[5 + i] =
                    StockDispBuffer(150 + i * (304 + ii) + ix, 282 + iy,
                                    DISP_PRIO_CHAR, CG_CHR_SEL_DEL_BTN, btnUseFlag);
            }
#endif
            else if (SubProcNo < 100 || 300 <= SubProcNo)
            {
#ifdef _MORECHARACTERS_
                btnGraId[1 + b] =
                    StockDispBuffer(160 + b * (304 + ii) + ix,
                                    342 + iy, DISP_PRIO_CHAR, CG_CHR_SEL_NEW_BTN, btnUseFlag);
#else
// 创建新角色的按钮
                btnGraId[1 + i] =
                    StockDispBuffer(80 + i * (304 + ii) + ix,
                                    282 + iy, DISP_PRIO_CHAR, CG_CHR_SEL_NEW_BTN, btnUseFlag);
#endif
            }
        }
// 后退按钮
        btnGraId[0] =
            StockDispBuffer(320 + backX, 432 + iy, DISP_PRIO_CHAR, CG_CHR_SEL_BACK_BTN, btnUseFlag);
    }
    StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, DISP_PRIO_BG, CG_CHR_SEL_BG);
#ifdef _LOGINKICK
    if (StartTime == -1)
        StartTime = TimeGetTime();
    if (TimeGetTime() > StartTime + 30 * 1000 /*1000为一秒*/)
    {
        ChangeProc(PROC_TITLE_MENU);
        StartTime = -1;
    }
#endif

#ifdef _PKSERVERCHARSEL // (不可开) Syu ADD PK服务器选择星系人物
    if (VersionXYZ == 1)
    {
        PkBtn[0] = StockDispBuffer(160, 430, DISP_PRIO_CHAR, CG_PKSERVER_BTN_UP + PkMenuflag, 2);
        if (HitDispNo == PkBtn[0])
        {
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                (PkMenuflag == 0) ? PkMenuflag = 1 : PkMenuflag = 0;
                play_se(217, 320, 240);
            }
        }
        if (PkMenuflag == 1)
        {
            if (PkServerSel == NULL)
            {
                PkServerSel = MakeWindowDisp(255, 5, 2, 7, 0, -1);
            }
            else
            {
                if (PkServerSel->hp > 0)
                {
                    x3 = PkServerSel->x;
                    y3 = PkServerSel->y;
                    StockDispBuffer(x3 + 60, y3 + 200, DISP_PRIO_IME3, CG_PKSERVER_PANEL, 0);
                    StockFontBuffer(x3 + 20, y3 + 155, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "选择星系人物", 1);
                    for (i = 0; i < 4; i++)
                    {
                        for (j = 0; j < 3; j++)
                        {
                            StockFontBuffer(x3 - 95 + i * 83, y3 + 178 + j * 22, FONT_PRIO_FRONT, FONT_PAL_WHITE, ServerName[i * 3 + j + 1], 2);
                            if (mouse.onceState & MOUSE_LEFT_CRICK)
                            {
                                // 送出i * 4 + j + 1给Gmsv取得所要的AC资料
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (PkServerSel != NULL)
            {
                DeathAction(PkServerSel);
                PkServerSel = NULL;
            }
        }
    }
#endif

    RunAction();
    StockTaskDispBuffer();
}

static short deleteCharacterProcNo = 0;

void initDeleteCharacter(void)
{
    deleteCharacterProcNo = 0;
}

int deleteCharacter(void)
{
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    if (deleteCharacterProcNo == 0)
    {
        deleteCharacterProcNo = 1;
        w = getUtf8CharNum(cCharDeleting) * 9 / 64 + 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }

    if (deleteCharacterProcNo == 1)
    {
        delCharStart();
        deleteCharacterProcNo++;
    }
    else if (deleteCharacterProcNo == 2)
        ret = delCharProc();

    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        if (ptActMenuWin->hp >= 1)
        {
            int len;
            int xx, yy;

            len = getUtf8CharNum(msg);
            xx = (w * 64 - len * 8) / 2;
            yy = (h * 48 - 16) / 2;
            StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
        }
    }

    return ret;
}

static short downloadCharListProcNo = 0;

void initDownloadCharList(void)
{
    downloadCharListProcNo = 0;
}

int downloadCharList(void)
{
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    static char msg[256];
    if (downloadCharListProcNo == 0)
    {
        downloadCharListProcNo = 1;
        strcpy(msg, "人物名单取得中");
        w = getUtf8CharNum(msg) * 8 / 64 + 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }

    if (downloadCharListProcNo == 1)
    {
        charListStart();
        downloadCharListProcNo++;
    }
    else if (downloadCharListProcNo == 2)
    {
        ret = charListProc();
    }

    if (ret != 0 && ptActMenuWin)
    {
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
    }
    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        int len = getUtf8CharNum(msg);
        int xx = (w * 64 - len * 8) / 2;
        int yy = (h * 48 - 16) / 2;
        StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
    }
    return ret;
}

int commonYesNoWindow(int x, int y)
{
    int ret = 0, i;
    static int btnGraId[] = {-2, -2};
    int id = selGraId(btnGraId, sizeof(btnGraId) / sizeof(int));
    if (id == 0)
    {
        ret = 1;
        play_se(217, 320, 240);
    }
    else if (id == 1)
    {
        ret = 2;
        play_se(217, 320, 240);
    }

    btnGraId[0] = StockDispBuffer(x, y, DISP_PRIO_YES_NO_BTN, CG_COMMON_YES_BTN, 2);
    btnGraId[1] = StockDispBuffer(x, y, DISP_PRIO_YES_NO_BTN, CG_COMMON_NO_BTN, 2);

    StockDispBuffer(x, y, DISP_PRIO_YES_NO_WND, CG_COMMON_WIN_YORO, 0);

    if (ret != 0)
    {
        for (i = 0; i < sizeof(btnGraId) / sizeof(int); i++)
        {
            btnGraId[i] = -2;
        }
    }

    return ret;
}

#define MaxSelectChar 12
static ACTION *ptActSelChar[MaxSelectChar];
static int selectGraNoTbl[MaxSelectChar] =
    {
        SPR_001em,
        SPR_011em,
        SPR_021em,
        SPR_031em,
        SPR_041em,
        SPR_051em,
        SPR_061em,
        SPR_071em,
        SPR_081em,
        SPR_091em,
        SPR_101em,
        SPR_111em,
#ifdef _MO_IMAGE_EXTENSION
        SPRNEW_001em,
        SPRNEW_005em,
        SPRNEW_009em,
        SPRNEW_013em,
        SPRNEW_017em,
        SPRNEW_021em,
#endif
};

// 2026.09.05: 这个是800*600的修正坐标
// 用于创建角色时人物形象
// int selectGraLocate[MaxSelectChar][2] =
//     {
//         {305, 290}, // 豆丁
//         {240, 360}, // 塞亚人
//         {410, 500}, // 辫子男孩
//         {330, 400}, // 酷哥
//         {200, 240}, // 熊皮男
//         {480, 430}, // 大个
//         {560, 410}, // 豆丁妹
//         {600, 340}, // 熊皮妹
//         {500, 310}, // 帽子妹
//         {380, 240}, // 短发妹
//         {460, 200}, // 手套女
//         {280, 200}, // 辣妹
// #ifdef _MO_IMAGE_EXTENSION
//         {180, 440},
//         {640, 420},
//         {530, 520},
//         {280, 500},
//         {120, 350},
//         {550, 240},
// #endif
// };

int selectGraLocate[MaxSelectChar][2] =
    {
        {198, 301},
        {113, 258},
        {248, 396},
        {130, 370},
        {137, 160},
        {343, 379},
        {479, 371},
        {420, 310},
        {376, 231},
        {297, 198},
        {381, 146},
        {242, 116},
};

int selectGraHitArea[MaxSelectChar][4] =
    {
    //     x,   y,  w,  h
        {-32, -56, 63, 65},
        {-21, -72, 42, 88},
        {-19, -78, 38, 94},
        {-20, -67, 41, 80},
        {-30, -80, 60, 100},
        {-36, -84, 72, 100},
        {-30, -51, 59, 64},
        {-19, -77, 40, 88},
        {-18, -72, 34, 83},
        {-22, -72, 43, 89},
        {-18, -78, 35, 90},
        {-20, -76, 40, 90},
#ifdef _MO_IMAGE_EXTENSION
        {-27, -75, 57, 100},
        {-22, -72, 43, 89},
        {-25, -60, 46, 77},
        {-27, -75, 57, 100},
        {-30, -80, 60, 100},
        {-36, -84, 72, 100},
#endif
};
static int selCharColor[MaxSelectChar]; // Char 顏色
static int selCharDir[MaxSelectChar];   // Char 方向
static int selCharGraNoProcNo;
static int nowSelCharGraNo;             // Char GraNo
static int nowSelCharMouthNo;           // Char 嘴形状
static int nowSelCharEyeNo;             // Char 眼睛形状
static int nowSelCharStatusPoint;       // Char 剩余的四维点数
static int nowSelCharStatus[4];         // Char 的攻防血敏四维
static int nowSelCharAttrPoint;         // Char 剩余的属性点数
static int nowSelCharAttr[4];           // Char 的地水火风属性
static int editCharParamProcNo;
STR_BUFFER selCharName;
static int selCharNameBoxX, selCharNameBoxY;
static int selHomeTownProcNo;
static int nowSelHomeTownNo;
static int createCharProcNo;

void initMakeCharacter(void);
void initSelCharGraNo(void);
void initSelCharGraNo2(void);
int selCharGraNo(void);
void initSelCharGraColorWin(void);
int selCharGraColorWin(void);
void initEditCharParam(void);
void initEditCharParam2(void);
int editCharParam(void);
void initCreateChar(void);
int createChar(void);
void initSelHomeTown(void);
int selHomeTown(void);

void initMakeCharacter(void) {}
void makeCharacterProc(void)
{
    int ret;
    if (SubProcNo == 0)
    {
        initMakeCharacter();
        SubProcNo++;
    }
    if (SubProcNo == 1)
    {
        initSelCharGraNo();
        SubProcNo = 3;
    }
    else if (SubProcNo == 2)
    {
        initSelCharGraNo2();
        SubProcNo++;
    }
    if (SubProcNo == 3)
    {
        ret = selCharGraNo();
        if (ret == 1)
            SubProcNo = 10;
        else if (ret == 2)
            ChangeProc(PROC_CHAR_SELECT, 10);
    }
    if (SubProcNo == 10 && ret == 1)
    {
        initEditCharParam();
        SubProcNo = 12;
    }
    else if (SubProcNo == 11)
    {
        initEditCharParam2();
        SubProcNo = 12;
    }
    if (SubProcNo == 12)
    {
        ret = editCharParam();
        if (ret == 1)
        {
            SubProcNo = 20;
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
            if (pActPet20 != NULL)
            {
                DeathAction(pActPet20);
                pActPet20 = NULL;
            }
#endif
        }
        if (ret == 2)
        {
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
            if (pActPet20 != NULL)
            {
                DeathAction(pActPet20);
                pActPet20 = NULL;
            }
#endif
            SubProcNo = 2;
        }
    }
    if (SubProcNo == 20)
    {
#ifndef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
        initSelHomeTown();
#endif
        SubProcNo = 21;
    }
    if (SubProcNo == 21)
    {
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
        ret = 1;
#else
        ret = selHomeTown();
#endif
        if (ret == 1)
        {
            SubProcNo = 30;
#ifdef _MORECHARACTERS_
            resetCharacterList(selectPcNo + 多人物当前页数 * 2);
            clearUserSetting(selectPcNo + 多人物当前页数 * 2);
#else
            resetCharacterList(selectPcNo);
            clearUserSetting(selectPcNo);
#endif
        }
        else
            if (ret == 2)
            {
                SubProcNo = 11;
            }
    }
    if (SubProcNo == 30)
    {
        initCreateChar();
        SubProcNo = 31;
    }
    if (SubProcNo == 31)
    {
        ret = createChar();
        if (ret == 1)
        {
            maxPcNo++;
            strcpy(gamestate_login_charname, newCharacterName);
            ChangeProc(PROC_CHAR_LOGIN_START);
            createCharFlag = 1;
        }
        else if (ret == 2)
            SubProcNo = 100;
        selHomeTown(); // 选择出生地
    }
    if (SubProcNo == 100)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 101)
    {
        extern char 创建人物内容提示[512];
        if (commonMsgWin(创建人物内容提示))
        {
            ChangeProc(PROC_CHAR_SELECT, 10);
        }
        selHomeTown(); // ?????
    }

    RunAction();
    StockTaskDispBuffer();
}

// 初始化select char页面的角色图像
void initSelCharGraNo(void)
{
    int i;
    selCharGraNoProcNo = 0;
    nowSelCharGraNo = -1;
    nowSelCharMouthNo = 0;
    for (i = 0; i < MaxSelectChar; i++)
    {
        selCharColor[i] = 0; // ????
        selCharDir[i] = 0;   // ??????
    }
    for (i = 0; i < MaxSelectChar; i++)
    {
        if (selectGraNoTbl[i] < 0)
        {
            ptActSelChar[i] = NULL;
            continue;
        }
        ptActSelChar[i] = GetAction(PRIO_CHR, 0);
        if (ptActSelChar[i] != NULL)
        {
            ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i];
            ptActSelChar[i]->anim_no = ANIM_STAND;
            ptActSelChar[i]->dispPrio = DISP_PRIO_CHAR;
            ptActSelChar[i]->x = selectGraLocate[i][0];
            ptActSelChar[i]->y = selectGraLocate[i][1];
            ptActSelChar[i]->anim_ang = selCharDir[i];
        }
    }
}

void initSelCharGraNo2(void)
{
    int i;
    selCharGraNoProcNo = 0;
    nowSelCharMouthNo = 0;
    nowSelCharEyeNo = 0;
    nowSelCharGraNo = -1;
    for (i = 0; i < MaxSelectChar; i++)
    {
        if (selectGraNoTbl[i] < 0)
        {
            ptActSelChar[i] = NULL;
            continue;
        }

        ptActSelChar[i] = GetAction(PRIO_CHR, 0);
        if (ptActSelChar[i] != NULL)
        {
            ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i];
            ptActSelChar[i]->anim_no = ANIM_STAND;
            ptActSelChar[i]->dispPrio = DISP_PRIO_CHAR;
            ptActSelChar[i]->x = selectGraLocate[i][0];
            ptActSelChar[i]->y = selectGraLocate[i][1];
            ptActSelChar[i]->anim_ang = selCharDir[i];
        }
    }
}

int selCharGraNo(void)
{
    int i;
    int ret = 0;
    int ret2;
    int x1, y1, x2, y2;
    int id;
    static int selCharCanselGraId[] = {-2};
    if (selCharGraNoProcNo == 0)
    {
        for (i = 0; i < sizeof(selCharCanselGraId) / sizeof(int); i++)
        {
            selCharCanselGraId[i] = -2;
        }
        selCharGraNoProcNo++;
    }

    if (selCharGraNoProcNo == 1)
    {
        for (i = 0; i < MaxSelectChar; i++)
        {
            if (ptActSelChar[i] != NULL)
            {
                x1 = ptActSelChar[i]->x + selectGraHitArea[i][0];
                y1 = ptActSelChar[i]->y + selectGraHitArea[i][1];
                x2 = x1 + selectGraHitArea[i][2];
                y2 = y1 + selectGraHitArea[i][3];
                if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX))
                {
                    ptActSelChar[i]->anim_no = ANIM_WALK;
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        selCharGraNoProcNo = 2;
                        nowSelCharGraNo = i;
                        play_se(217, 320, 240); // ?????
                    }
                }
                else
                    ptActSelChar[i]->anim_no = ANIM_STAND;
            }
        }
    }
    if (selCharGraNoProcNo == 2)
    {
        initSelCharGraColorWin();
        selCharGraNoProcNo++;
    }
    if (selCharGraNoProcNo == 3)
    {
        ret2 = selCharGraColorWin();
        if (ret2 == 1)
        {
            ret = 1;
        }
        else if (ret2 == 2)
        {
            selCharGraNoProcNo = 1;
            nowSelCharGraNo = -1;
        }
    }
    id = selGraId(selCharCanselGraId, sizeof(selCharCanselGraId) / sizeof(int));
    if (id == 0)
    {
        ret = 2;
        play_se(217, 320, 240);
    }
    if (ret != 0)
    {
        for (i = 0; i < MaxSelectChar; i++)
        {
            if (ptActSelChar[i] != NULL)
            {
                DeathAction(ptActSelChar[i]);
                ptActSelChar[i] = NULL;
            }
        }
    }

    for (i = 0; i < MaxSelectChar; i++)
    {
        if (ptActSelChar[i] != NULL)
        {
            if (nowSelCharGraNo == i)
            {
                ptActSelChar[i]->anim_no = ANIM_WALK;
                x1 = ptActSelChar[i]->x + selectGraHitArea[i][0];
                y1 = ptActSelChar[i]->y + selectGraHitArea[i][1];
                x2 = x1 + selectGraHitArea[i][2];
                y2 = y1 + selectGraHitArea[i][3];
                StockBoxDispBuffer(x1, y1, x2, y2, DISP_PRIO_BOX, BoxColor, 0);
            }
            ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i] + selCharColor[i] * 5;
            ptActSelChar[i]->anim_ang = selCharDir[i];
            pattern(ptActSelChar[i], ANM_NOMAL_SPD, ANM_LOOP);
        }
    }

    if (selCharGraNoProcNo == 1)
    {
        id = focusGraId(selCharCanselGraId, sizeof(selCharCanselGraId) / sizeof(int));
        if (id == 0)
            ShowBottomLineString(FONT_PAL_WHITE, "回到前一个画面。");
        else
            ShowBottomLineString(FONT_PAL_WHITE, "请选择一个人物。");
        selCharCanselGraId[0] =
            StockDispBuffer(64, 400, DISP_PRIO_BG, CG_CHR_MAKE_BACK_BTN, 2);
    }
    // 2026.08.23 修复角色选择页面背景问题
    if (ret == 0)
        StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER,
            DISP_PRIO_BG, CG_CHR_MAKE_SEL_BG);
    RunAction();           //
    StockTaskDispBuffer(); //

    return ret;
}

void initSelCharGraColorWin(void)
{
    selCharGraColorWinProcNo = 0;
}

int selCharGraColorWin(void)
{
    static int fontId[] = {-2, -2, -2, -2};
    static ACTION *ptActMenuWin = NULL;
    int id;
    int i;
    static int x, y, w, h;
    int ret = 0;
    char msg[][16] = { "   决定   ", " 改变颜色 ", " 转动方向 ", "   取消   "};
    if (selCharGraColorWinProcNo == 0)
    {
        selCharGraColorWinProcNo = 1;
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
        w = 2;
        h = 3;
        x = 496;
        y = 41;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    id = selFontId(fontId, sizeof(fontId) / sizeof(int));

    if (id == 0)
    {
        ret = 1;
        play_se(217, 320, 240);
    }
    else
    {
        if (id == 1)
        {
            selCharColor[nowSelCharGraNo]++;
            if (selCharColor[nowSelCharGraNo] > 3)
                selCharColor[nowSelCharGraNo] = 0;
            play_se(217, 320, 240);
        }
        else if (id == 2)
        {
            selCharDir[nowSelCharGraNo]++;
            if (selCharDir[nowSelCharGraNo] > 7)
                selCharDir[nowSelCharGraNo] = 0;
            play_se(217, 320, 240);
        }
        else if (id == 3)
        {
            ret = 2;
            play_se(217, 320, 240);
        }
    }
    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }

    id = focusFontId(fontId, sizeof(fontId) / sizeof(int));

    switch (id) {
        case 0:
            ShowBottomLineString(FONT_PAL_WHITE, "决定目前所选择的人物。");
            break;
        case 1: 
            ShowBottomLineString(FONT_PAL_WHITE, "改变人物的颜色。");
            break;
        case 2:
            ShowBottomLineString(FONT_PAL_WHITE, "转动人物的方向。");
            break;
        case 3: 
            ShowBottomLineString(FONT_PAL_WHITE, "回到选择人物的画面。");
            break;
        default:
            break;
    }

    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
        {
            fontId[i] = StockFontBuffer(x + 22, y + i * 22 + 32,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, msg[i], 2);
        }
    }
    return ret;
}

void initEditCharParam(void)
{
    editCharParamProcNo = 0;

    selCharNameBoxX = 104;
    selCharNameBoxY = 102;
    initStrBuffer(&selCharName, selCharNameBoxX, selCharNameBoxY,
                  CHAR_NAME_LEN, FONT_PAL_WHITE, FONT_PRIO_BACK);
    selCharName.buffer[0] = '\0';
    selCharName.cnt = 0;
    selCharName.cursor = 0;
    GetKeyInputFocus(&selCharName);

    nowSelCharStatusPoint = 0;
    nowSelCharStatus[0] = 10;
    nowSelCharStatus[1] = 10;
    // end modified by lsh
    nowSelCharStatus[2] = 0;
    nowSelCharStatus[3] = 0;
    nowSelCharAttrPoint = 10;
    nowSelCharAttr[0] = 0;
    nowSelCharAttr[1] = 0;
    nowSelCharAttr[2] = 0;
    nowSelCharAttr[3] = 0;
}

void initEditCharParam2(void)
{
    editCharParamProcNo = 0;

    selCharNameBoxX = 104;
    selCharNameBoxY = 102;
    initStrBuffer(&selCharName, selCharNameBoxX, selCharNameBoxY,
                  CHAR_NAME_LEN, FONT_PAL_WHITE, FONT_PRIO_BACK);
    GetKeyInputFocus(&selCharName);
}
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
typedef struct
{
    int spr_num;   // SPR图号
    int earth;     // 地属性
    int water;     // 水属性
    int fire;      // 火属性
    int wind;      // 风属性
    char name[16]; // 宠物名称
} BORNPETINFO;
#endif

// 编辑Char的初始状态
int editCharParam(void)
{
    int ret = 0;
    char msg[64];
    int x1, y1, x2, y2;
    int cnt = 1;
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
    BORNPETINFO petinfo[4] = {

        {100251, 0, 4, 6, 0, "乌力乌力"},
        {100296, 2, 0, 0, 8, "凯比"},
        {100292, 0, 7, 3, 0, "克克尔"},
        {100266, 6, 0, 0, 4, "威伯"}};
    int BornPreBtn, BornNextBtn;
    int attrx1, attry1, attrx2, attry2;
    int BornBaseX = 430, BornBaseY = 110;
#endif
    static int graId[] =
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
         -2, -2, -2, -2, -2, -2, -2, -2,
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
         -2, -2, -2, -2, -2, -2};
#else
         -2, -2, -2, -2};
#endif
    int i;
    int id, id2, id3;
    int statusLocate[4][2] = {
            {111, 250},
            {111, 278},
            {111, 305},
            {111, 334}};
    int attrLocate[4][2] = {
            {297, 255},
            {297, 279},
            {297, 303},
            {297, 327}};
    int attrColor[4][2] = {
            {SYSTEM_PAL_GREEN, SYSTEM_PAL_GREEN2},
            {SYSTEM_PAL_AQUA, SYSTEM_PAL_AQUA2},
            {SYSTEM_PAL_RED, SYSTEM_PAL_RED2},
            {SYSTEM_PAL_YELLOW, SYSTEM_PAL_YELLOW2}};
    int upDownBtnGraNo[][2] = {
            {CG_PREV_BTN, CG_PREV_BTN_DOWN},
            {CG_NEXT_BTN, CG_NEXT_BTN_DOWN}};
    int selUseFlag = 0;
    int btn1, btn2;

    GetKeyInputFocus(NULL);

    if (editCharParamProcNo == 0)
    {
        for (i = 0; i < sizeof(graId) / sizeof(int); i++)
            graId[i] = -2;
        editCharParamProcNo++;
    }

    id = -1;
    id3 = -1;
    if (editCharParamProcNo == 1)
    {
        selUseFlag = 2;
        GetKeyInputFocus(&selCharName);
        id = selRepGraId(graId, sizeof(graId) / sizeof(int));
        id3 = pushGraId(graId, sizeof(graId) / sizeof(int));

        if (id == 0)
        {
            if (selCharName.cnt <= 0) // 名字不合法
                editCharParamProcNo = 100;
            else if (nowSelCharStatusPoint > 0) // 状态点未分配完毕
                editCharParamProcNo = 104;
            else if (nowSelCharAttrPoint > 0) // 属性点未分配完毕
                    editCharParamProcNo = 106;
            else
            {
                strcpy(newCharacterName, selCharName.buffer);
                newCharacterName[selCharName.cnt] = '\0';
                if (cmpNameCharacterList(selCharName.buffer))
                    editCharParamProcNo = 102;
                else if (strstr(newCharacterName, " ") != NULL
                         || strstr(newCharacterName, "  ") != NULL)
                    editCharParamProcNo = 108;
                else
                {
                    ret = 1;
                    newCharacterGraNo =
                        selectGraNoTbl[nowSelCharGraNo] + selCharColor[nowSelCharGraNo] * 5;
                    newCharacterFaceGraNo =
                        CG_CHR_MAKE_FACE + nowSelCharGraNo * 100 + selCharColor[nowSelCharGraNo] * 25 + nowSelCharEyeNo * 5 + nowSelCharMouthNo;
                    if (nowSelCharGraNo < 12)
                        newCharacterFaceGraNo =
                            CG_CHR_MAKE_FACE + nowSelCharGraNo * 100 + selCharColor[nowSelCharGraNo] * 25 + nowSelCharEyeNo * 5 + nowSelCharMouthNo;
#ifdef _MO_IMAGE_EXTENSION
                    else
                        newCharacterFaceGraNo =
                            CG_CHR_MAKE_NEWFACE + (nowSelCharGraNo - 12) * 100 + selCharColor[nowSelCharGraNo] * 25 + nowSelCharEyeNo * 5 + nowSelCharMouthNo;
#endif
                    newCharacterVit = nowSelCharStatus[0];
                    newCharacterStr = nowSelCharStatus[1];
                    newCharacterTgh = nowSelCharStatus[2];
                    newCharacterDex = nowSelCharStatus[3];
                    newCharacterEarth = nowSelCharAttr[0];
                    newCharacterWater = nowSelCharAttr[1];
                    newCharacterFire = nowSelCharAttr[2];
                    newCharacterWind = nowSelCharAttr[3];
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
                     // 用原来的出生点变数纪录出生宠
                    newCharacterHomeTown = BornPetNum;
#endif
                    loginDp = 0;
                    play_se(217, 320, 240); 
                }
            }
        }
        else if (id == 1)
        {
            ret = 2;
            play_se(217, 320, 240); // ?????
        }
        else if (2 <= id && id <= 9)
        {
            id2 = id - 2;
            if ((id2 % 2) == 0)
            {
                if (nowSelCharStatus[id2 / 2] > 0)
                {
                    nowSelCharStatus[id2 / 2]--;
                    nowSelCharStatusPoint++;
                    play_se(217, 320, 240); // ?????
                }
            }
            else
            {
                if (nowSelCharStatusPoint > 0)
                {
                    nowSelCharStatus[id2 / 2]++;
                    nowSelCharStatusPoint--;
                    play_se(217, 320, 240);
                }
            }
        }
        else if (10 <= id && id <= 17)
        {
            id2 = id - 10; // id2 -> [0, 7]
            if ((id2 % 2) == 0)
            {
                if (nowSelCharAttr[id2 / 2] > 0)
                {
                    nowSelCharAttr[id2 / 2]--;
                    nowSelCharAttrPoint++;
                }
            }
            else
            {
                int no, no2;
                no = id2 / 2;
                if (no < 2)
                    no2 = no + 2;
                else
                    no2 = no - 2;
                // 配对属性为 0 且仍有剩余点数时才允许提升该属性。
                // 剩余点数为 0 时“+”按钮已在下方绘制逻辑中禁用(-2)，
                // 这里不再执行原先那段“互换”逻辑——那段逻辑会把值为 0 的
                // 属性扣成负数(nowSelCharAttr[i]--), 造成属性出现负值/超额分配。
                if (nowSelCharAttr[no2] == 0 && nowSelCharAttrPoint > 0)
                {
                    nowSelCharAttr[no]++;
                    nowSelCharAttrPoint--;
                }
            }
            play_se(217, 320, 240);
        }
        else if (id == 18)
        {
            if (nowSelCharEyeNo > 0)
                nowSelCharEyeNo--;
            else
                nowSelCharEyeNo = 4;
            play_se(217, 320, 240);
        }
        else if (id == 19)
        {
            if (nowSelCharEyeNo < 4)
                nowSelCharEyeNo++;
            else
                nowSelCharEyeNo = 0;
            play_se(217, 320, 240);
        }
        else if (id == 20)
        {
            if (nowSelCharMouthNo > 0)
                nowSelCharMouthNo--;
            else
                nowSelCharMouthNo = 4;
            play_se(217, 320, 240);
        }
        else if (id == 21)
        {
            if (nowSelCharMouthNo < 4)
                nowSelCharMouthNo++;
            else
                nowSelCharMouthNo = 0;
            play_se(217, 320, 240);
        }
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
        else if (id == 22 || id == 23)
        {
            if (pActPet20 != NULL)
            {
                DeathAction(pActPet20);
                pActPet20 = NULL;
            }
            if (id == 22)
            {
                if (BornPetNum > 0)
                    BornPetNum--;
                else
                    BornPetNum = 3;
            }
            else
            {
                if (BornPetNum < 3)
                    BornPetNum++;
                else
                    BornPetNum = 0;
            }
        }
#endif
    }
    if (editCharParamProcNo == 100)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240); // ???
    }
    if (editCharParamProcNo == 101)
    {
        if (commonMsgWin("没有输入名字！"))
            editCharParamProcNo = 1;
    }
    if (editCharParamProcNo == 102)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240);
    }
    if (editCharParamProcNo == 103)
    {
        if (commonMsgWin("已经有同名的人物存在了！"))
            editCharParamProcNo = 1;
    }
    if (editCharParamProcNo == 104)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240);
    }
    if (editCharParamProcNo == 105)
    {
        if (commonMsgWin("请将状态的点数分配完毕！"))
            editCharParamProcNo = 1;
    }
    if (editCharParamProcNo == 106)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240);
    }
    if (editCharParamProcNo == 107)
    {
        if (commonMsgWin("请将属性的点数分配完毕！"))
            editCharParamProcNo = 1;
    }
    if (editCharParamProcNo == 108)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240);
    }
    if (editCharParamProcNo == 109)
    {
        if (commonMsgWin("名字里面不可以有空白！"))
            editCharParamProcNo = 1;
    }
    if (editCharParamProcNo == 110)
    {
        initCommonMsgWin();
        editCharParamProcNo++;
        play_se(220, 320, 240);
    }
    if (editCharParamProcNo == 111)
    {
        if (commonMsgWin("你输入的名字有不允许字符"))
            editCharParamProcNo = 1;
    }

    if (editCharParamProcNo == 1)
    {
        id = focusGraId(graId, sizeof(graId) / sizeof(int));
        if (id == 0)
            ShowBottomLineString(FONT_PAL_WHITE, "决定人物的设定。");
        else if (id == 1)
            ShowBottomLineString(FONT_PAL_WHITE, "回到前一个画面。");
        else if (2 <= id && id <= 9)
        {
            id = (id - 10) >> 1;
            if (id == 0)
                ShowBottomLineString(FONT_PAL_WHITE, "这项主要与耐久力有关。");
            else if (id == 1)
                ShowBottomLineString(FONT_PAL_WHITE, "这项主要与攻击力有关。");
            else if (id == 2)
                ShowBottomLineString(FONT_PAL_WHITE, "这项主要与防御力有关。");
            else if (id == 3)
                ShowBottomLineString(FONT_PAL_WHITE, "这项主要与战斗时的行动顺序有关。");
        }
        else if (10 <= id && id <= 17)
        {
            id = (id - 10) >> 1;
            if (id == 0)
                ShowBottomLineString(FONT_PAL_WHITE, "地属性。与水属性的对手战斗时较为有利。");
            else if (id == 1)
                ShowBottomLineString(FONT_PAL_WHITE, "水属性。与火属性的对手战斗时较为有利。");
            else if (id == 2)
                ShowBottomLineString(FONT_PAL_WHITE, "火属性。与风属性的对手战斗时较为有利。");
            else if (id == 3)
                ShowBottomLineString(FONT_PAL_WHITE, "风属性。与地属性的对手战斗时较为有利。");
        }
        else if (id == 18 || id == 19)
        {
            if (nowSelCharGraNo != 0 && nowSelCharGraNo != 6)
                ShowBottomLineString(FONT_PAL_WHITE, "改变眼睛的形状。");
            else
                ShowBottomLineString(FONT_PAL_WHITE, "改变鼻子的形状。");
        }
        else if (id == 20 || id == 21)
            ShowBottomLineString(FONT_PAL_WHITE, "改变嘴巴的形状。");
        else
            ShowBottomLineString(FONT_PAL_WHITE, "设定姓名、长相、基本状态与属性。");
    }

    StockFontBuffer2(&selCharName);

    if (nowSelCharGraNo < 12)
        StockDispBuffer(93, 172, DISP_PRIO_BG,
                        CG_CHR_MAKE_FACE + nowSelCharGraNo * 100 + selCharColor[nowSelCharGraNo] * 25 + nowSelCharEyeNo * 5 + nowSelCharMouthNo,
                        0);
#ifdef _MO_IMAGE_EXTENSION
    else
        StockDispBuffer(93, 172, DISP_PRIO_BG,
                        CG_CHR_MAKE_NEWFACE + (nowSelCharGraNo - 12) * 100 + selCharColor[nowSelCharGraNo] * 25 + nowSelCharEyeNo * 5 + nowSelCharMouthNo,
                        0);
#endif

    sprintf_s(msg, "%3d", nowSelCharStatusPoint);
    StockFontBuffer(161, 224, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);

    for (i = 0; i < 4; i++)
    {
        sprintf_s(msg, "%3d", nowSelCharStatus[i]);
        StockFontBuffer(statusLocate[i][0], statusLocate[i][1],
                        FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
    }

    sprintf_s(msg, "%3d", nowSelCharAttrPoint);
    StockFontBuffer(350, 224, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0);
    for (i = 0; i < 4; i++)
    {
        if (nowSelCharAttr[i] > 0)
        {
            x1 = attrLocate[i][0];
            y1 = attrLocate[i][1];
            x2 = x1 + nowSelCharAttr[i] * 8;
            y2 = y1 + 8;
            StockBoxDispBuffer(x1, y1, x2, y2, DISP_PRIO_IME2, attrColor[i][0], 1);
            StockBoxDispBuffer(x1 + 1, y1 + 1, x2 + 1, y2 + 1, DISP_PRIO_IME1, attrColor[i][1], 0);
            StockBoxDispBuffer(x1 + 2, y1 + 2, x2 + 2, y2 + 2, DISP_PRIO_IME1, attrColor[i][1], 0);
        }
    }

    graId[0] = StockDispBuffer(320, 410, DISP_PRIO_CHAR, CG_CHR_MAKE_OK_BTN, selUseFlag);
    graId[1] = StockDispBuffer(420, 410, DISP_PRIO_CHAR, CG_CHR_MAKE_BACK_BTN, selUseFlag);

    for (i = 0; i < 4; i++)
    {
        btn1 = 0;
        btn2 = 0;
        if (2 <= id3 && id3 <= 9 && (id3 - 2) / 2 == i)
        {
            if (((id3 - 2) % 2) == 0)
                btn1 = 1;
            else
                btn2 = 1;
        }
        // “-”按钮(释放点数)：该状态值为 0 时禁用(-2：不可点击/不绘制)
        if (nowSelCharStatus[i] > 0)
            graId[i * 2 + 2] = StockDispBuffer(statusLocate[i][0] + 51, statusLocate[i][1] + 8,
                                               DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag);
        else
            graId[i * 2 + 2] = -2;
        // “+”按钮(分配点数)在剩余点数为 0 时禁用(-2：不可点击/不绘制)
        if (nowSelCharStatusPoint > 0)
            graId[i * 2 + 3] = StockDispBuffer(statusLocate[i][0] + 87, statusLocate[i][1] + 8,
                                               DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag);
        else
            graId[i * 2 + 3] = -2;
    }

    for (i = 0; i < 4; i++)
    {
        if (i == 0 && nowSelCharAttr[i + 2] == 0 || i == 1 && nowSelCharAttr[i + 2] == 0 || i == 2 && nowSelCharAttr[i - 2] == 0 || i == 3 && nowSelCharAttr[i - 2] == 0)
        {
            btn1 = 0;
            btn2 = 0;
            if (10 <= id3 && id <= 17 && (id3 - 10) / 2 == i)
            {
                if (((id3 - 10) % 2) == 0)
                    btn1 = 1;
                else
                    btn2 = 1;
            }
            // “-”按钮(释放点数)：该属性值为 0 时禁用(-2：不可点击/不绘制)
            if (nowSelCharAttr[i] > 0)
                graId[i * 2 + 10] = StockDispBuffer(attrLocate[i][0] + 101, attrLocate[i][1] + 5,
                                                    DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag);
            else
                graId[i * 2 + 10] = -2;
            // “+”按钮(分配点数)在剩余点数为 0 时禁用(-2：不可点击/不绘制)
            if (nowSelCharAttrPoint > 0)
                graId[i * 2 + 11] = StockDispBuffer(attrLocate[i][0] + 137, attrLocate[i][1] + 5,
                                                    DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag);
            else
                graId[i * 2 + 11] = -2;
        }
        else
        {
            graId[i * 2 + 10] = -2;
            graId[i * 2 + 11] = -2;
        }
    }

    for (i = 0; i < 2; i++)
    {
        btn1 = 0;
        btn2 = 0;
        if (18 <= id3 && id3 <= 21 && (id3 - 18) / 2 == i)
        {
            if (((id3 - 18) % 2) == 0)
                btn1 = 1;
            else
                btn2 = 1;
        }

        graId[i * 2 + 18] =
            StockDispBuffer(156, 172 + i * 24,
                            DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag);
        graId[i * 2 + 19] =
            StockDispBuffer(260, 172 + i * 24,
                            DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag);
    }

    if (nowSelCharGraNo != 0 && nowSelCharGraNo != 6)
        StockDispBuffer(320, 240, DISP_PRIO_CHAR, CG_CHR_MAKE_EYE_SEL, 0);
    else
        StockDispBuffer(320, 240, DISP_PRIO_CHAR, CG_CHR_MAKE_NOSE_SEL, 0);
#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
    BornPreBtn = 0;
    BornNextBtn = 0;
    if (id3 == 22 || id3 == 23)
    {
        if (id3 == 22)
            BornPreBtn = 1;
        else
            BornNextBtn = 1;
    }
    attrx1 = BornBaseX + 35;
    if (petinfo[BornPetNum].earth > 0)
    {
        attry1 = BornBaseY;
        attrx2 = attrx1 + petinfo[BornPetNum].earth * 8;
        attry2 = attry1 + 8;
        StockBoxDispBuffer(attrx1, attry1, attrx2, attry2, DISP_PRIO_IME2, SYSTEM_PAL_GREEN, 1);
        StockBoxDispBuffer(attrx1 + 1, attry1 + 1, attrx2 + 1, attry2 + 1, DISP_PRIO_IME1, SYSTEM_PAL_GREEN2, 0);
        StockBoxDispBuffer(attrx1 + 2, attry1 + 2, attrx2 + 2, attry2 + 2, DISP_PRIO_IME1, SYSTEM_PAL_GREEN2, 0);
    }
    if (petinfo[BornPetNum].water > 0)
    {
        attry1 = BornBaseY + 20;
        attrx2 = attrx1 + petinfo[BornPetNum].water * 8;
        attry2 = attry1 + 8;
        StockBoxDispBuffer(attrx1, attry1, attrx2, attry2, DISP_PRIO_IME2, SYSTEM_PAL_AQUA, 1);
        StockBoxDispBuffer(attrx1 + 1, attry1 + 1, attrx2 + 1, attry2 + 1, DISP_PRIO_IME1, SYSTEM_PAL_AQUA2, 0);
        StockBoxDispBuffer(attrx1 + 2, attry1 + 2, attrx2 + 2, attry2 + 2, DISP_PRIO_IME1, SYSTEM_PAL_AQUA2, 0);
    }
    if (petinfo[BornPetNum].fire > 0)
    {
        attry1 = BornBaseY + 40;
        attrx2 = attrx1 + petinfo[BornPetNum].fire * 8;
        attry2 = attry1 + 8;
        StockBoxDispBuffer(attrx1, attry1, attrx2, attry2, DISP_PRIO_IME2, SYSTEM_PAL_RED, 1);
        StockBoxDispBuffer(attrx1 + 1, attry1 + 1, attrx2 + 1, attry2 + 1, DISP_PRIO_IME1, SYSTEM_PAL_RED2, 0);
        StockBoxDispBuffer(attrx1 + 2, attry1 + 2, attrx2 + 2, attry2 + 2, DISP_PRIO_IME1, SYSTEM_PAL_RED2, 0);
    }
    if (petinfo[BornPetNum].wind > 0)
    {
        attry1 = BornBaseY + 60;
        attrx2 = attrx1 + petinfo[BornPetNum].wind * 8;
        attry2 = attry1 + 8;
        StockBoxDispBuffer(attrx1, attry1, attrx2, attry2, DISP_PRIO_IME2, SYSTEM_PAL_YELLOW, 1);
        StockBoxDispBuffer(attrx1 + 1, attry1 + 1, attrx2 + 1, attry2 + 1, DISP_PRIO_IME1, SYSTEM_PAL_YELLOW2, 0);
        StockBoxDispBuffer(attrx1 + 2, attry1 + 2, attrx2 + 2, attry2 + 2, DISP_PRIO_IME1, SYSTEM_PAL_YELLOW2, 0);
    }

    StockFontBuffer(BornBaseX + 15, BornBaseY - 33, FONT_PRIO_FRONT, FONT_PAL_WHITE, petinfo[BornPetNum].name, 0);
    graId[22] = StockDispBuffer(BornBaseX - 105, BornBaseY + 85, DISP_PRIO_CHAR, upDownBtnGraNo[0][BornPreBtn], selUseFlag);
    graId[23] = StockDispBuffer(BornBaseX - 30, BornBaseY + 85, DISP_PRIO_CHAR, upDownBtnGraNo[1][BornNextBtn], selUseFlag);
    StockDispBuffer(BornBaseX - 3, BornBaseY - 4, DISP_PRIO_CHAR, CG_NEWBORNPET_WND, 0);
    if (pActPet20 == NULL)
    {
        // 3~9
        pActPet20 = MakeAnimDisp(BornBaseX - 60, BornBaseY + 45, petinfo[BornPetNum].spr_num, 0);
    }
#endif
    // 人物编成
    StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER,
        DISP_PRIO_BG, CG_CHR_MAKE_BG);
    RunAction();
    StockTaskDispBuffer(); //
    FlashKeyboardCursor(); // 
    ImeProc();             // 初始化输入法
    return ret;
}

void initSelHomeTown(void)
{
    selHomeTownProcNo = 0;
    nowSelHomeTownNo = 0;
}

int selHomeTown(void)
{
    static int btnId[] = {-2, -2, -2, -2, -2};
    int id;
    int ret = 0;
    int ret2;
    char *homeTownName[] = {"萨姆吉尔村", "玛丽娜丝村", "加加村", "卡鲁它那村"};
    int selUseFlag = 0;
    int i;
    if (selHomeTownProcNo == 0)
    {
        for (int i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        selHomeTownProcNo++;
    }
    if (selHomeTownProcNo == 1)
        selUseFlag = 2;
    if (selHomeTownProcNo == 10)
    {
        ret2 = commonYesNoWindow(320, 240);
        if (ret2 == 1)
        {
            ret = 1;
            selHomeTownProcNo = 11; //
        }
        else if (ret2 == 2)
            selHomeTownProcNo = 1;
    }
    id = selGraId(btnId, sizeof(btnId) / sizeof(int));
    if (id == 0)
    {
        ret = 2;
        play_se(217, 320, 240); // ?????
    }
    if (1 <= id && id <= 4)
    {
        nowSelHomeTownNo = id - 1;
        newCharacterHomeTown = nowSelHomeTownNo;
        selHomeTownProcNo = 10;
        play_se(217, 320, 240); // ?????
    }
    if (selHomeTownProcNo == 1)
        id = focusGraId(btnId, sizeof(btnId) / sizeof(int));
#ifdef _SPECIAL_LOGO
    int ix = 80;
    int iy = 18;
#else
    int ix = 50;
    int iy = 0;
#endif

#ifdef _DELBORNPLACE // Syu ADD 6.0 统一出生于新手村
#else
    if (id == 1)
    {
        StockFontBuffer(454 + ix, 135 + iy,
                        FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[0], 0);
        StockFontBuffer(386 + ix, 158 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "位在萨伊那斯的东边，", 0);
        StockFontBuffer(386 + ix, 178 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "是四个村子中最大", 0);
        StockFontBuffer(386 + ix, 198 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "也是最整齐的村子。", 0);
    }
    else if (id == 2)
    {
        StockFontBuffer(454 + ix, 135 + iy,
                        FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[1], 0);
        StockFontBuffer(390 + ix, 168 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "位在萨伊那斯的西边", 0);
        StockFontBuffer(390 + ix, 188 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "面向美丽海滩的村子。", 0);
    }
    else if (id == 3)
    {
        StockFontBuffer(474 + ix, 135 + iy,
                        FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[2], 0);
        StockFontBuffer(390 + ix, 158 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "位在加鲁卡的东边，", 0);
        StockFontBuffer(390 + ix, 178 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "四周被森林围绕着", 0);
        StockFontBuffer(390 + ix, 198 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "绿意盎然的村子。", 0);
    }
    else if (id == 4)
    {
        StockFontBuffer(454 + ix, 135 + iy,
                        FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[3], 0);
        StockFontBuffer(390 + ix, 158 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "位在加鲁卡的西边。", 0);
        StockFontBuffer(390 + ix, 178 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "村子是在好几个小小", 0);
        StockFontBuffer(390 + ix, 198 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "的岛上建立起来的。", 0);
    }
    else if (id == 0)
        StockFontBuffer(390 + ix, 176 + iy,
                        FONT_PRIO_BACK, FONT_PAL_BLUE, "回到前一个画面。", 0);
    else
        StockFontBuffer(390 + ix, 176 + iy,
                       FONT_PRIO_BACK, FONT_PAL_BLUE, "请选择出身地。", 0);
    btnId[0] = StockDispBuffer(675, 450, DISP_PRIO_CHAR, CG_CHR_MAKE_BACK_BTN, selUseFlag);
    btnId[1] = StockDispBuffer(380, 295, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME0, selUseFlag);
    btnId[2] = StockDispBuffer(370, 295, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME1, selUseFlag);
    btnId[3] = StockDispBuffer(400, 295, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME2, selUseFlag);
    btnId[4] = StockDispBuffer(445, 295, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME3, selUseFlag);
    // 2026.08.23 修复选择出生地界面便宜的问题
    StockDispBufferScaled(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER,
        DISP_PRIO_BG, CG_CHR_MAKE_SEL2_BG);
#endif
    return ret;
}

void initCreateChar(void)
{
    createCharProcNo = 0;
}

int createChar(void)
{
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    int ret2;
    char msg[] = "人物制作中...";
    if (createCharProcNo == 0)
    {
        createCharProcNo++;
        w = (getUtf8CharNum(msg) * 9 + 63) / 64;
        if (w < 2)
            w = 2;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    if (createCharProcNo == 1)
    {
        createNewCharStart();
        createCharProcNo++;
    }
    if (createCharProcNo == 2)
    {
        ret2 = createNewCharProc();
        if (ret2 == 1)
            ret = 1;
        else
            if (ret2 < 0)
                ret = 2;
    }
    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }

    if (ptActMenuWin != NULL)
    {
        if (ptActMenuWin->hp >= 1)
        {
            int len = getUtf8CharNum(msg) / 2;
            int xx = (w * 64 - len * 17) / 2;
            int yy = (h * 48 - 16) / 2;
            StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
        }
    }

    return ret;
}

void initCharLogin(void);
int charLogin(void);

void characterLoginProc(void)
{
    int ret;
    static char msg[256];
    if (SubProcNo == 0)
    {
        initCharLogin();
        SubProcNo++;
    }
    if (SubProcNo == 1)
    {
        ret = charLogin();
        if (ret == 0)
        {
            // 签入成功
        }
        else if (ret == 1)
        {
            InitChat();
            ChangeProc(PROC_GAME); // 进入游戏
        }
        else if (ret == -1)
        {
            SubProcNo = 100;
            strcpy(msg, NET_ERRMSG_LOGINTIMEOUT);
        }
        else if (ret == -2)
        {
            // 2026.08.24 遇到这个问题
            SubProcNo = 100;
            strcpy(msg, "您的账号目前正在离线，请重新登录。");
        }
        else if (ret == -3)
        {
            SubProcNo = 100;
            strcpy(msg, "按照版署《网络游戏未成年人防沉迷系统》规定，您未通过验证，\n将被强制离线，如有疑问请登录网站查询或进行验证");
        }

#ifdef _NEW_WGS_MSG // WON ADD WGS的新视窗

        else if (ret == 101)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "无法与资料服务器取得连系");
        }
        else if (ret == 102)
        {
            SubProcNo = 100;
            strcpy(msg, "无法由资料库取得正确资讯");
        }
        else if (ret == 103)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "服务器忙碌中,请稍待一会再试");
        }
        else if (ret == 104)
        {
            SubProcNo = 100;
            strcpy(msg, "输入的资料格式不正确");
        }
        else if (ret == 201)
        {
            SubProcNo = 100;
            strcpy(msg, "错误的使用者名称或密码, 若您尚未加入会员, 请先到游戏网站免费加入会员");
        }
        else if (ret == 202)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "同一个使用者不得重覆登入");
        }
        else if (ret == 203)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "同一帐号已在线上");
        }
        else if (ret == 204)
        { // 2.1 New      // 同星系重覆登入
            SubProcNo = 100;
            // strcpy( msg, "您已在此星系登入" );
            strcpy(msg, "使用者已在进行游戏");
        }
        else if (ret == 205)
        {
            SubProcNo = 100;
            strcpy(msg, "未登入此游戏或登入错误");
        }
        else if (ret == 206)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "同名帐号已存在");
        }
        else if (ret == 207)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "会员帐号已经启动");
        }
        else if (ret == 208)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "错误的启动码");
        }
        else if (ret == 209)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "会员帐号尚未启动");
        }
        else if (ret == 210)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "会员帐号和密码只能使用英文字母和数字");
        }
        else if (ret == 211)
        {
            SubProcNo = 100;
            strcpy(msg, "此帐号已被停权, 如有问题请联系客服");
        }
        else if (ret == 212)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "请先到游戏网站重新确认个人资料, 再启动此游戏");
        }
        else if (ret == 213)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "登入逾时, 请重新登入");
        }
        else if (ret == 214)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "网页尚未登入");
        }
        else if (ret == 220)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "此帐号已转换为新游戏会员");
        }
        else if (ret == 222)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "请先到游戏网站更新游戏管理帐号的密码, 再启动此游戏");
        }
        else if (ret == 301)
        {
            SubProcNo = 100;
            strcpy(msg, "此产品尚未注册, 请先到游戏网站完成注册");
        }
        else if (ret == 402)
        {
            SubProcNo = 100;
            strcpy(msg, "游戏点数卡已用完点数, 请购买点数卡再至游戏网站完成注册");
        }
        else if (ret == 405)
        {
            SubProcNo = 100;
            strcpy(msg, "游戏点数不足, 请购买点数卡再至游戏网站注册");
        }
        else if (ret == 999)
        { // 2.1 New
            SubProcNo = 100;
            strcpy(msg, "网路连线错误");
        }
        else if (ret >= 2030 && ret < 2044)
        { // 不同星系重覆登入
            int star = -1;
            star = ret - 2030;
            SubProcNo = 100;

            // 依照 cs 的 acid 号码设的
            switch (star)
            {
            case 0:
                strcpy(msg, "您已在其他星系登入。");
                break;
            case 1:
                strcpy(msg, "您已在太阳星系登入");
                break;
            case 2:
                strcpy(msg, "您已在天神星系登入");
                break;
            case 3:
                strcpy(msg, "您已在北斗星系登入");
                break;
            case 4:
                strcpy(msg, "您已在紫微星系登入");
                break;
            case 5:
                strcpy(msg, "您已在苍龙星系登入");
                break;
            case 6:
                strcpy(msg, "您已在银河星系登入");
                break;
            case 7:
                strcpy(msg, "您已在香港星系登入");
                break;
            case 8:
                strcpy(msg, "您已在星乐园星系登入");
                break;
            case 9:
                strcpy(msg, "您已在网路家庭星系登入");
                break;
            case 10:
                strcpy(msg, "您已在圣兽星系登入");
                break;
            case 11:
                strcpy(msg, "您已在天鹰星系登入");
                break;
            case 12:
                strcpy(msg, "您已在新界星系登入");
                break;
            case 13:
                strcpy(msg, "您已在测试星系登入");
                break;
            default:
                strcpy(msg, "签入处理失败。(1002)");
                break;
            }
        }

        else
        {
            SubProcNo = 100;
            sprintf_s(msg, "签入处理失败(%d)。", ret);
        }
#endif
    }
    if (SubProcNo == 100)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 101)
    {
        if (commonMsgWin(msg))
            ChangeProc(PROC_TITLE_MENU);
    }
    RunAction();
    StockTaskDispBuffer();
}

static short charLoginProcNo = 0;

void initCharLogin(void)
{
    charLoginProcNo = 0;
}

int charLogin(void)
{
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    if (charLoginProcNo == 0)
    {
        charLoginProcNo = 1;
        w = getUtf8CharNum(cCheckingIn) * 9 / 64 + 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    if (charLoginProcNo == 1)
    {
        charLoginStart();
        charLoginProcNo++;
    }
    else if (charLoginProcNo == 2)
        ret = charLoginProc();

    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }

    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        int len = getUtf8CharNum(msg);
        int xx = (w * 64 - len * 8) / 2;
        int yy = (h * 48 - 16) / 2;
        StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, cCheckingIn, 0);
    }
    return ret;
}

static int produce_logout(void);
static int produce_vct_no = 0;

void initCharLogout(void);
int charLogout(void);

void characterLogoutProc(void)
{
    int ret;
    static char msg[256];

    if (SubProcNo == 0)
    {
        logOutFlag = TRUE;

        if (!produce_logout())
        { //????
            return;
        }
#ifdef __SKYISLAND
        extern void SkyIslandRelease();
        SkyIslandRelease();
#endif
        //???????
        produce_vct_no = 0;

#ifdef _AniRandom // Syu ADD 随机产生环境动画
        extern void AniRandomRelease();
        AniRandomRelease();
#endif

#ifdef _AniCrossFrame // Syu ADD 动画层游过画面生物
        extern void crossAniRelease();
        crossAniRelease();
#endif
#ifdef _AniCharBubble // Syu ADD 动画层人物吐出气泡
        extern void CharbubbleRelease();
        CharbubbleRelease();
#endif
#ifdef _AniImmobile // Syu ADD 定点产生特定动画
        extern void ImmobileAniRelease();
        ImmobileAniRelease();
#endif
#ifdef _SPECIALSPACEANIM // Syu ADD 特殊场景动画配置
        extern void ReleaseSpecAnim();
        ReleaseSpecAnim();
#endif
        resetPc();
        initCharObj();
        DeathAllAction();
        if (!offlineFlag)
            SubProcNo++;
        else
            PostMessage(hWnd, WM_CLOSE, 0, 0L);
    }

    if (SubProcNo == 1)
    {
        initCharLogout();
        SubProcNo++;
    }
    else if (SubProcNo == 2)
    {
        ret = charLogout();
        if (ret == 1)
        {
            cleanupNetwork();
            DeathAllAction();
            ChangeProc(PROC_ID_PASSWORD);
            SubProcNo = 0;
        }
        else if (ret == -1)
        {
            cleanupNetwork();
            SubProcNo = 100;
            strcpy(msg, NET_ERRMSG_LOGOUTTIMEOUT);
        }
        else if (ret == -2)
        {
            cleanupNetwork();
            SubProcNo = 100;
            strcpy(msg, "签出处理失败。");
        }
    }
    if (SubProcNo == 100)
    {
        initCommonMsgWin();
        SubProcNo++;
    }
    if (SubProcNo == 101)
    {
        if (commonMsgWin(msg))
        {
            ChangeProc(PROC_ID_PASSWORD); // ?????
            SubProcNo = 0;
        }
    }
    RunAction();
    StockTaskDispBuffer();
}

static int charLogoutProcNo;

void initCharLogout(void)
{
    charLogoutProcNo = 0;
}

int charLogout(void)
{
    extern unsigned int systemWndNo;
    static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h;
    int ret = 0;
    static char msg[] = "签出中";

    if (systemWndNo != 1)
    {
#ifdef _CHAR_NEWLOGOUT
        lssproto_CharLogout_send(sockfd, 0);
#endif
        return 1;
    }
    if (charLogoutProcNo == 0)
    {
        charLogoutProcNo = 1;
        w = getUtf8CharNum(msg) * 9 / 64 + 2;
        h = (16 + 47) / 48;
        if (h < 2)
            h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
    }
    if (charLogoutProcNo == 1)
    {
        charLogoutStart();
        charLogoutProcNo++;
    }
    else if (charLogoutProcNo == 2)
    {
        ret = charLogoutProc();
    }

    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }
    if (ptActMenuWin != NULL)
    {
        if (ptActMenuWin->hp >= 1)
        {
            int len;
            int xx, yy;
            len = getUtf8CharNum(msg);
            xx = (w * 64 - len * 8) / 2;
            yy = (h * 48 - 16) / 2;
            StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
        }
    }

    return ret;
}

int windowTypeWN = -1; // ????????  -1??
int buttonTypeWN = 0;  // ??????(???)
int indexWN;           // ??????
int idWN;              // ??????
// shan begin
// char msgWN[25][70];        // ??????
char msgWN[25][256];
// shan end
int msgWNLen; // ??????????
int msgWN_W;  // ????????????
int msgWN_H;  // ?????????????
ACTION *ptActMenuWin = NULL;

int selStartLine;      // ??????????????????
int wnOpenX, wnOpenY;  // ??????????????????
short wnCloseFlag = 0; // ????????????

#define MAX_SHOP_ITEM 8    // ??????????????????
#define MAX_SHOP_PAGE 8    // ?????????????????
short shopWindowMode;      // ??????????????0...????,1...?????
char shopWindow1Title[48]; // 物品商店标题: 例如【巫女】
char shopWindow1Msg[48];   // 物品商店欢迎语: 例如【我有卖很神奇的东西喔！】
#ifdef _NEW_SHOP_FRAME
char shopWindowCurrency[48];
#endif
typedef struct
{
    int level;
    int price;
    int graNo;
    short sealFlag;
    short tbl;
    char name[ITEM_NAME_LEN + 1];
    char info[3][29];
    int num;
#ifdef _NEW_MANOR_LAW
    int costfame;
#endif
} ITEM_SHOP;

int menuIndexWN;                                   // ??????(???????????)
char shopWindow2Msg[2][45];                        // ???????????????
short shopWondow2Page, shopWondow2MaxPage;         // ??????????
ITEM_SHOP sealItem[MAX_SHOP_ITEM * MAX_SHOP_PAGE]; // ?????
short selShopItemNo;

char shopWindow3Msg[41];
short sealItemCnt;

char shopWindow4Msg[2][39]; //
char shopWindow5Msg[2][39]; //
char shopWindow6Msg[2][39]; //

short nowUserItemCnt;

static int GetEmptyPlayerItemSlotCount(void)
{
    int emptyCount = 0;
#ifdef _NEW_ITEM_
    const int itemLimit = 判断玩家道具数量();
#else
    const int itemLimit = MAX_ITEM;
#endif

    for (int i = MAX_ITEMSTART; i < itemLimit; ++i)
    {
        if (pc.item[i].useFlag == 0)
            ++emptyCount;
    }
    ClientRuntimeLog("inventory-capacity",
        "range=[%d,%d) capacity=%d used=%d empty=%d"
#ifdef _NEW_ITEM_
        " state=0x%x"
#endif
        ,
        MAX_ITEMSTART, itemLimit, itemLimit - MAX_ITEMSTART,
        itemLimit - MAX_ITEMSTART - emptyCount, emptyCount
#ifdef _NEW_ITEM_
        ,
        pc.道具栏状态);
#else
        );
#endif
    return emptyCount;
}
char shopWindow7Msg[2][39];
char shopWindow8Msg[2][39];

// Shan Add  (Begin)
int FMmsgWN[16];
int FMelderFlag[16];
int FMpidWN[16];
int FMdengonidex;
char FMnameWN[11][20];
time_t FMHoldTime;
// end

#ifdef _BLACK_MARKET
struct BlackMarketItem
{
    int iIndex;
    char iName[128];
    int iGraphicsNum;
};
struct BlackMarketItem BMItem[12];
int sItem = -1;
#endif

ITEM_SHOP userItem[MAX_SHOP_ITEM * MAX_SHOP_PAGE]; // ?????????????

short buyOffFlag; // ????

#define MAX_SKILL_SHOP_SKILL 8 // ?????????????????
// Robin fix 好像太少
// #define MAX_SKILL_SHOP_PAGE        2                    // ??????????????
#define MAX_SKILL_SHOP_PAGE 10 // ??????????????

typedef struct
{
    int price;
    int icon;
#ifdef _PETKILL_COST_FAME
    int iCoseFame;
#endif
    char name[SKILL_NAME_LEN + 1];
    char info[3][25];
} SKILL_SHOP;

SKILL_SHOP sealSkill[MAX_SKILL_SHOP_SKILL * MAX_SKILL_SHOP_PAGE];

short skillShopWindowProcNo;
short selShopSkillNo;
short selShopSkillPetNo;
short selShopSkillSlotNo;

#define MAX_POOL_SHOP_ITEM 8 // ??????????????

#ifdef _ADD_POOL_ITEM // WON ADD 增加寄放道具
// #define MAX_POOL_SHOP_PAGE        10
#define MAX_POOL_SHOP_PAGE 13
#else
#define MAX_POOL_SHOP_PAGE 3 // ?????
#endif

typedef struct
{
    int level;
    int price;
    int graNo;
    short poolFlag;
    short tbl;
    char name[ITEM_NAME_LEN + 1];
    char info[3][29];
#ifdef _ITEM_PILENUMS
    int num;
#endif
} POOL_SHOP;

int restPoolSlot; // ????????????????

POOL_SHOP poolItem[MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE];

void initServerWindowType0(char *);
void initServerWindowType1(char *);
void initServerWindowType2(char *);
void initServerWindowType3(char *);
void initServerWindowType4(char *);
void initServerWindowType5(char *);
void initServerWindowType6(char *);
void initServerWindowType7(char *);
void serverWindowType0(int);
void serverWindowType1(void);
void serverWindowType2(void);
void serverWindowType3(void);
void serverWindowType4(void);
void serverWindowType5(void);
void serverWindowType6(void);
void serverWindowType7(void);
void serverWindowType9(void);

#ifdef _CHAR_PROFESSION // WON ADD 人物职业技能
void initServerWindowProfession(char *data);
void profession_windows();
int profession_windows_1();
int profession_windows_2();
char ProfessionShopTitle[27];
char ProfessionShopMsg[2][45];
#endif

#ifdef _NPC_WELFARE_2 // WON ADD 职业NPC-2
void initServerWindowProfession2(char *data);
void profession_windows2();
int profession_windows_12();
int profession_windows_22();
char ProfessionShopTitle2[27];
char ProfessionShopMsg2[2][45];
#endif

#ifdef _PETSKILL_CANNEDFOOD
void initPetSkillWindowType1(char *date);
void PetSkillShowType1(void);
#endif

void initFamilyAddWN(char *);
void familyAddWN(void);
void initFamilyList(char *);
void familyListWN(void);
void initFamilyDetail(char *);
int familyDetailWN(void);
void initMineFamilyDetail(char *);
void mineFamilyDetailWN(void);

void initItemman(char *);
int itemmanWN(void);

void initFamilyTaxWN(char *);
void familyTaxWN(void);

void initFamilyLeaderChange(char *);
void familyLeaderChangeWN(void);
void familyLeaderChangeQWN(void);
void familyLeaderChangeAWN(void);
void initFamilyLeaderChangeA(char *);

#ifdef _BLACK_MARKET
void initBlackMarket(char *);
void BMWindowType(void);
#endif

#ifdef _ADD_FAMILY_TAX // WON ADD 增加庄园税收
void FMTAXWindowsType(void);
#endif

// Shan Add
void initFMWindowType(char *);
void FMWindowType(void);
void initFMWindowType1(char *);
void FMWindowType1(void);
void initFMWindowType2(char *);
void FMWindowType2(void);
void initFMWindowType3(char *);
#ifdef _NEW_MANOR_LAW
void FMWindowType3(int king);
#else
void FMWindowType3(void);
#endif
#ifdef _FMVER21
void initFMWindowType4(char *, int king);
#ifdef _NEW_MANOR_LAW
void FMWindowType4(int king);
#else
void FMWindowType4(void);
#endif
#endif
// Robin Family PK
void initFMPKListWN(char *);
void FMPKListWN(int mode);
void initFMPKSelectWN(char *);
void FMPKSelectWN(int mode);
void initFMPKDetailWN(char *);
void FMPKDetailWN(int mode);

void initShowRidePetWN(void);
void showRidePetWN(void);

#ifdef _NPC_SELLSTH
void InitSellSth_Menu(char *data);
void InitSellSth_View(char *data);

void InitSellSth_Sell(char *data);
// void SellSth_Menu( void );
void SellSth_Sell(void);
#endif

#ifdef _MOUSE_DBL_CLICK
void initMouseGetNameWN(char *data);
void showMouseGetNameWN(void);
#endif

#ifdef _CONTRACT
void initContractWN(char *data);
void showContractWN();
#endif

#ifdef _RACEMAN
void initRacemanRankWN(char *data);
void showRacemanRankWN(void);
#endif

#ifdef _DRAGON_FUSION
void serverWindowDragonFusion(void);
#endif

#ifdef _NPC_DAYACTION
void initDayactionRankWN(char *data);
void showDayactionRankWN(void);
#endif

// ???
void initOpenServerWindow(void)
{
    windowTypeWN = -1;
    ptActMenuWin = NULL;
}

// Robin 0720 Login Announce
int LoginAnnounce = 0;
char AnnounceBuf[8192];
#ifdef _WIN_LUAJIT_
lua_State *WinLua = NULL;
int cloasewindows = 0;
SA_WINDOWS windows;
#endif
void openServerWindow(int windowtype, int buttontype, int index, int id, char *data)
{
#ifdef _ANGEL_SUMMON
    if (windowtype == WINDOW_MESSAGETYPE_ANGELMESSAGE)
    {
        if (index == 520 /*CHAR_WINDOWTYPE_ANGEL_ASK*/)
        {
            angelFlag = TRUE;
            angelRecvType = index;
            angelRecvButton = buttontype;
            strcpy(angelMsg, data);
            return;
        }
        else if (index = 521 /*CHAR_WINDOWTYPE_ANGEL_CLEAN*/)
        {
            angelFlag = FALSE;
            strcpy(angelMsg, "");
            return;
        }
    }
#endif

    if (windowTypeWN < 0 || windowtype == 33 || windowtype == 0 || windowtype == 36 || windowtype == 39 || windowtype == 42)
    {

        if (windowtype == 0 && windowTypeWN == 13)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
#ifdef _BLACK_MARKET
        if (windowtype == 0 && windowTypeWN == WINDOW_MESSAGETYPE_BLACKMARKET)
        {
            closeBlackMarketWN();
        }
#endif
        wnOpenX = nowGx;
        wnOpenY = nowGy;
        wnCloseFlag = 0;

        windowTypeWN = windowtype;
        buttonTypeWN = buttontype;
        indexWN = index;
        idWN = id;

        buyOffFlag = 0;

        // ?????
        if (windowTypeWN == WINDOW_MESSAGETYPE_MESSAGE || windowTypeWN == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT
#ifdef _NEW_MANOR_LAW
            || windowTypeWN == WINDOW_FMMESSAGETYPE_MANOR_SCHEDULE // 挑战庄园排行
#endif
        )
        {
            initServerWindowType0(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_SELECT)
        {
            // 
            initServerWindowType1(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU)
        {
            // ??????????
            initServerWindowType2(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMAIN)
        {
            initServerWindowType3(data);
        }
        else
            // ???????
            if (windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN)
            {
                // ??????????
                initServerWindowType3(data);
                buyOffFlag = 1;
            }
            else
                // ?????????????
                if (windowTypeWN == WINDOW_MESSAGETYPE_PETSKILLSHOP)
                {
                    initServerWindowType5(data);
                }
                else
#ifdef _CHAR_PROFESSION // WON ADD 人物职业技能
                    if (windowTypeWN == WINDOW_MESSAGETYPE_PROFESSIONSHOP)
                    {
                        initServerWindowProfession(data);
                    }
                    else
#endif
#ifdef _NPC_WELFARE_2 // WON ADD 职业NPC-2
                        if (windowTypeWN == WINDOW_MESSAGETYPE_PROFESSIONSHOP2)
                        {
                            initServerWindowProfession2(data);
                        }
                        else
#endif
#ifdef _PETSKILL_CANNEDFOOD
                            if (windowTypeWN == WINDOWS_MESSAGETYPE_PETSKILLSHOW)
                        {
                            initPetSkillWindowType1(data);
                        }
                        else
#endif
                            // ??????????
                            if (windowTypeWN == WINDOW_MESSAGETYPE_WIDEMESSAGE || windowTypeWN == WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT)
                            {
                                // ???
                                // ???????
                                // ?????????
                                initServerWindowType4(data);
                            }
                            else
                                // ????????????????
                                if (windowTypeWN == WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN)
                                {
                                    initServerWindowType6(data);
                                }
                                else
                                    // ????????????????????
                                    if (windowTypeWN == WINDOW_MESSAGETYPE_POOLITEMSHOPMENU)
                                    {
                                        initServerWindowType7(data);
                                    }
                                    else
                                        // Robin 0518 Family
                                        if (windowTypeWN == WINDOW_MESSAGETYPE_FAMILYADD)
                                        {
                                            initFamilyAddWN(data);
                                        }

#ifdef _BLACK_MARKET
                                        else if (windowTypeWN == WINDOW_MESSAGETYPE_BLACKMARKET)
                                        {
                                            initBlackMarket(data);
                                        }
#endif
#ifdef _ADD_FAMILY_TAX // WON ADD 增加庄园税收
                                        else if (windowTypeWN == WINDOWS_MESSAGETYPE_FAMILY_TAX)
                                        {
                                            initServerWindowType0(data);
                                        }
#endif
                                        else if (windowTypeWN == WINDOW_MESSAGETYPE_FAMILYJOIN)
                                        {
                                            initFamilyList(data);
                                        }

        if (windowTypeWN == WINDOW_MESSAGETYPE_FAMILYDETAIL)
        {
            initMineFamilyDetail(data);
        }

        // Shan Family Dengon
        // Member List
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_SELECT)
        {
            initFMWindowType(data);
        }
#ifdef _FMVER21
        // TOP 30 DP List
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_TOP30DP)
        {
            initFMWindowType4(data, 0);
        }
#endif
#ifdef _NEW_MANOR_LAW
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_FM_MEMONTUM)
            initFMWindowType4(data, 1);
#endif
        // Dp List
        else
#ifdef _NEW_MANOR_LAW
            if (windowTypeWN == WINDOW_FMMESSAGETYPE_DP || windowTypeWN == WINDOW_FMMESSAGETYPE_10_MEMONTUM)
#else
            if (windowTypeWN == WINDOW_FMMESSAGETYPE_DP)
#endif
        {
            initFMWindowType3(data);
        }
        // Point List
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_POINTLIST)
        {
            initFMWindowType2(data);
        }
        // Family Memo
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_DENGON)
        {
            initFMWindowType1(data);
        }
        // Familys Memo
        else if (windowTypeWN == WINDOW_FMMESSAGETYPE_FMSDENGON)
        {
            initFMWindowType1(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_PKSCHEDULELIST)
        {
            initFMPKListWN(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY)
        {
            initFMPKSelectWN(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL)
        {
            initFMPKDetailWN(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_SHOWRIDEPET)
        {
            initShowRidePetWN();
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_LOGINMESSAGE)
        {
            windowTypeWN = -1;
            LoginAnnounce = 1;
            strcpy(AnnounceBuf, data);
            // windowTypeWN = WINDOW_MESSAGETYPE_MESSAGE;
            // initServerWindowType0( data );
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_LEADERSELECT)
        {
            initFamilyLeaderChange(data);
        }
        else if (windowTypeWN == WINDOW_MESSAGETYPE_LEADERSELECTA)
        {
            initFamilyLeaderChangeA(data);
        }
#ifdef _NPC_SELLSTH
        if (windowTypeWN == WINDOWS_MESSAGETYPE_SELLSTHMENU)
        {
            InitSellSth_Menu(data);
        }
        else if (windowTypeWN == WINDOWS_MESSAGETYPE_SELLSTHVIEW)
        {
            InitSellSth_View(data);
        }
        else if (windowTypeWN == WINDOWS_MESSAGETYPE_SELLSTHSELL)
        {
            InitSellSth_Sell(data);
        }
#endif

        // Terry add 2001/12/06
        // 伊甸大陆--个人银行
#ifdef __EDEN_EFFECT
        else if (windowTypeWN == WINDOW_MESSAGETYPE_BANK)
        {
            // 设定玩家身上的银行存款
            pc.personal_bankgold = atoi(data);
            if (pc.personal_bankgold < 0)
                pc.personal_bankgold = 0;
            else if (pc.personal_bankgold > MAX_PERSONAL_BANKGOLD)
                pc.personal_bankgold = MAX_PERSONAL_BANKGOLD;
        }
#endif
        // Terry end
        // Terry add 2002/01/03
#ifdef __EDEN_AUCTION
        else if (windowTypeWN >= WINDOW_MESSAGETYPE_AUCTIONNEW &&
                 windowTypeWN <= WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY)
        {
            switch (windowTypeWN)
            {
                // 开启委托视窗
            case WINDOW_MESSAGETYPE_AUCTIONNEW:
                break;
                // 开启委托列表视窗
            case WINDOW_MESSAGETYPE_AUCTIONLIST_BUY:
            case WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY:
                szpALD = data;
                // szpALD = TESTDATA;
                bNewData = TRUE;
                break;
                // 开启委托详细内容视窗
            case WINDOW_MESSAGETYPE_AUCTIONSURVEY:
                // 开启修改委托内容视窗
            case WINDOW_MESSAGETYPE_AUCTIONMODIFY:
                szpALD = data;
                break;
            }
        }
#endif
        // Terry end

#ifdef _MOUSE_DBL_CLICK
        else if (windowTypeWN == WINDOW_MESSAGETYPE_MOUSEGETNAME)
        {
            initMouseGetNameWN(data);
        }
#endif

#ifdef _CONTRACT
        else if (windowTypeWN == WINDOW_MESSAGETYPE_CONTRACT)
        {
            initContractWN(data);
        }
#endif

#ifdef _RACEMAN
        else if (windowTypeWN == WINDOW_MESSAGETYPE_RACEMAN_RANK)
        {
            initRacemanRankWN(data);
        }
#endif

#ifdef _NPC_DAYACTION
        else if (windowTypeWN == WINDOW_MESSAGETYPE_DAYACTION_FMRANK)
        {
            initDayactionRankWN(data);
        }
#endif

        if (CheckMenuFlag())
        {
            InitMenu2();
        }

        closeEtcSwitch();
        closeCharActionAnimeChange();
        closeJoinChannelWN();
    }

#ifdef _NEWSHOP_
    if (windowtype == 1001)
    {
        cloasewindows = 0;
        extern void 在线商城窗口初始化(char *内容);
        extern ACTION *创建商城窗口();
        在线商城窗口初始化(data);
        商城动作地址 = 创建商城窗口();
        return;
    }
#endif
#ifdef _CHARSIGNDAY_
    if (windowtype == 1002)
    {
        cloasewindows = 0;
        extern void 签到窗口初始化(char *内容);
        extern ACTION *创建签到窗口();
        签到窗口初始化(data);
        创建签到窗口();
        return;
    }
#endif
#ifdef _MAGIC_ITEM_
    if (windowtype == 1004)
    {
        cloasewindows = 0;

        InitMagicItemWin(data);
        CreateMagicItemWin();
        return;
    }
#endif

#ifdef _WIN_LUAJIT_
    if (windowtype >= 1000)
    {
        cloasewindows = 0;
        WinLua = FindWinIntLua(windowtype, data);
    }
    else
    {
        if (windows.ptActMenuWin != NULL)
        {
            cloasewindows = 1;
        }
    }
#endif
}

void openServerWindowProc(void)
{
    // Robin 0720 Login Announce
    if (LoginAnnounce)
    {
        LoginAnnounce = 0;
        lssproto_WN_recv(sockfd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK, -1, -1, AnnounceBuf);
    }
    if (windowTypeWN < 0)
        return;
    if (ABS(wnOpenX - nowGx) >= 2 || ABS(wnOpenY - nowGy) >= 2)
    {
        wnCloseFlag = 1; // ?????????
    }
    switch (windowTypeWN)
    {
    case WINDOW_MESSAGETYPE_MESSAGE:
    case WINDOW_MESSAGETYPE_WIDEMESSAGE:
        serverWindowType0(0);
        break;

        // ?????????
    case WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT:
    case WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT:
        serverWindowType0(1);
        break;
#ifdef _NEW_MANOR_LAW
    case WINDOW_FMMESSAGETYPE_MANOR_SCHEDULE: // 挑战庄园排行
        serverWindowType0(2);
        break;
#endif
        // 
    case WINDOW_MESSAGETYPE_SELECT:
        serverWindowType1();
        break;

        // ???
    case WINDOW_MESSAGETYPE_PETSELECT:
        serverWindowType2();
        break;
#ifdef _NPC_FUSION
    case WINDOWS_MESSAGETYPE_PETFUSION: // andy_add 宠物融合
        serverWindowType9();
        break;
#endif
        // ??
    case WINDOW_MESSAGETYPE_PARTYSELECT:
        serverWindowType3();
        break;

        // ??????
    case WINDOW_MESSAGETYPE_PETANDPARTYSELECT:
        serverWindowType4();
        break;

        // ??????????
    case WINDOW_MESSAGETYPE_ITEMSHOPMENU:
    case WINDOW_MESSAGETYPE_ITEMSHOPMAIN:
    case WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN:
        serverWindowType5();
        break;

        // ??????????
    case WINDOW_MESSAGETYPE_PETSKILLSHOP:
        serverWindowType6();
        break;

#ifdef _CHAR_PROFESSION // WON ADD 人物职业技能
    case WINDOW_MESSAGETYPE_PROFESSIONSHOP:
        profession_windows();
        break;
#endif
#ifdef _NPC_WELFARE_2 // WON ADD 职业NPC-2
    case WINDOW_MESSAGETYPE_PROFESSIONSHOP2:
        profession_windows2();
        break;
#endif

#ifdef _PETSKILL_CANNEDFOOD
    case WINDOWS_MESSAGETYPE_PETSKILLSHOW:
        PetSkillShowType1();
        break;

#endif
        // ????????????????
    case WINDOW_MESSAGETYPE_POOLITEMSHOPMENU:
    case WINDOW_MESSAGETYPE_POOLITEMSHOPMAIN:
        serverWindowType7();
        break;

        // Robin 0517 family:
    case WINDOW_MESSAGETYPE_FAMILYADD:
        familyAddWN();
        break;

    case WINDOW_MESSAGETYPE_FAMILYJOIN:
        familyListWN();
        break;
#ifdef _NPC_SELLSTH
    case WINDOWS_MESSAGETYPE_SELLSTHMENU:
        //            SellSth_Menu();
    case WINDOWS_MESSAGETYPE_SELLSTHVIEW:
    case WINDOWS_MESSAGETYPE_SELLSTHSELL:
        SellSth_Sell();
        break;
#endif
    case WINDOW_MESSAGETYPE_FAMILYDETAIL:
        mineFamilyDetailWN();
        break;

        /*
        case WINDOW_MESSAGETYPE_ITEMMAN:
        itemmanWN();
        break;
        */
#ifdef _ADD_FAMILY_TAX // WON ADD 增加庄园税收
    case WINDOWS_MESSAGETYPE_FAMILY_TAX:
        FMTAXWindowsType();
        break;
#endif
#ifdef _BLACK_MARKET
    case WINDOW_MESSAGETYPE_BLACKMARKET:
        BMWindowType();
        break;
#endif
        // Shan family memberlist
    case WINDOW_FMMESSAGETYPE_SELECT:
        FMWindowType();
        break;
#ifdef _FMVER21
        // Shan family dplist top30
#ifdef _NEW_MANOR_LAW
    case WINDOW_FMMESSAGETYPE_FM_MEMONTUM:
        FMWindowType4(1);
        break;
    case WINDOW_FMMESSAGETYPE_TOP30DP:
        FMWindowType4(0);
        break;
#else
    case WINDOW_FMMESSAGETYPE_TOP30DP:
        FMWindowType4();
        break;
#endif
#endif
        // Shan family dplist
#ifdef _NEW_MANOR_LAW
    case WINDOW_FMMESSAGETYPE_10_MEMONTUM:
        FMWindowType3(1);
        break;
    case WINDOW_FMMESSAGETYPE_DP:
        FMWindowType3(0);
        break;
#else
    case WINDOW_FMMESSAGETYPE_DP:
        FMWindowType3();
        break;
#endif
        // Shan family pointlist
    case WINDOW_FMMESSAGETYPE_POINTLIST:
        FMWindowType2();
        break;
        // Shan family dengon
    case WINDOW_FMMESSAGETYPE_DENGON:
    case WINDOW_FMMESSAGETYPE_FMSDENGON:
        FMWindowType1();
        break;
        // Robin FM_PK
    case WINDOW_MESSAGETYPE_PKSCHEDULELIST:
        FMPKListWN(0);
        break;
    case WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY:
        FMPKSelectWN(0);
        break;
    case WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL:
        FMPKDetailWN(0);
        break;
    case WINDOW_MESSAGETYPE_FAMILYTAX:
        familyTaxWN();
        break;
    case WINDOW_MESSAGETYPE_SHOWRIDEPET:
        showRidePetWN();
        break;
    case WINDOW_MESSAGETYPE_LEADERSELECT:
        familyLeaderChangeWN();
        break;
    case WINDOW_MESSAGETYPE_LEADERSELECTQ:
        familyLeaderChangeQWN();
        break;
    case WINDOW_MESSAGETYPE_LEADERSELECTA:
        familyLeaderChangeAWN();
        break;
        // Terry 2001/12/07
#ifdef __EDEN_EFFECT
    case WINDOW_MESSAGETYPE_BANK:
        BankProc();
        break;
#endif
        // Terry end

        // Terry add 2002/01/03
#ifdef __EDEN_AUCTION
        // WT-->(W)indow(T)ype
        // 开启委托视窗
    case WINDOW_MESSAGETYPE_AUCTIONNEW:
        AuctionNewWT();
        break;
        // 开启委托列表视窗
    case WINDOW_MESSAGETYPE_AUCTIONLIST_BUY:
    case WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY:
        AuctionListWT();
        break;
        // 开启委托详细内容视窗
    case WINDOW_MESSAGETYPE_AUCTIONSURVEY:
        // 开启修改委托内容视窗
    case WINDOW_MESSAGETYPE_AUCTIONMODIFY:
        AuctionSurveyModifyWT();
        break;
        break;
#endif
        // Terry end

#ifdef _MOUSE_DBL_CLICK
    case WINDOW_MESSAGETYPE_MOUSEGETNAME:
        showMouseGetNameWN();
        break;
#endif

#ifdef _CONTRACT
    case WINDOW_MESSAGETYPE_CONTRACT:
        showContractWN();
        break;
#endif

#ifdef _RACEMAN
    case WINDOW_MESSAGETYPE_RACEMAN_RANK:
        showRacemanRankWN();
        break;
#endif

#ifdef _DRAGON_FUSION // 人龙进化
    case WINDOWS_MESSAGETYPE_DRAGONFUSION:
        serverWindowDragonFusion();
        break;
#endif

#ifdef _NPC_DAYACTION
    case WINDOW_MESSAGETYPE_DAYACTION_FMRANK:
        showDayactionRankWN();
        break;
#endif
#ifdef _NEWSHOP_
    case 1001:
        extern ACTION *商城数据显示();
        extern ACTION *创建商城窗口();
        if (商城动作地址 == NULL)
        {
            商城动作地址 = 创建商城窗口();
        }
        if (商城动作地址)
        {
            if (商城动作地址->hp >= 1)
            {
                商城动作地址 = 商城数据显示();
            }
        }
        break;
#endif
#ifdef _MAGIC_ITEM_
    case 1004:
        //    MagicItemActAddr = CreateMagicItemWin();
        if (MagicItemActAddr == NULL)
        {
            MagicItemActAddr = CreateMagicItemWin();
        }
        if (MagicItemActAddr)
        {
            if (MagicItemActAddr->hp >= 1)
            {
                ShowMagicItemData();
            }
        }
        break;
#endif
#ifdef _CHARSIGNDAY_
    case 1002:
        extern ACTION *签到数据显示();
        extern ACTION *创建签到窗口();
        if (签到动作地址 == NULL)
        {
            签到动作地址 = 创建签到窗口();
        }
        if (签到动作地址)
        {
            if (签到动作地址->hp >= 1)
            {
                签到数据显示();
            }
        }
        break;
#endif
    }
#ifdef _WIN_LUAJIT_
    if (windowTypeWN > 1004)
    {
        if (windows.ptActMenuWin == NULL)
        {
            windows.ptActMenuWin = FreeCreateWinTypeLua(WinLua);
            if (windows.ptActMenuWin == NULL)
            {
                InitMenu();
                initFieldProc();
            }
        }
        if (windows.ptActMenuWin != NULL && windows.ptActMenuWin->hp >= 1)
        {
            windows.ptActMenuWin = FreeServerWinTypeLua(WinLua);
        }
    }
#endif
}

static int Utf8SafePrefixLength(const char *text, int maxBytes)
{
    int offset = 0;
    while (text[offset] != '\0' && offset < maxBytes)
    {
        const unsigned char lead = (unsigned char)text[offset];
        int length;
        if (lead < 0x80)
            length = 1;
        else if (lead >= 0xC2 && lead <= 0xDF)
            length = 2;
        else if (lead >= 0xE0 && lead <= 0xEF)
            length = 3;
        else if (lead >= 0xF0 && lead <= 0xF4)
            length = 4;
        else
            return -1;

        if (offset + length > maxBytes)
            break;
        for (int i = 1; i < length; ++i)
            if (((unsigned char)text[offset + i] & 0xC0) != 0x80)
                return -1;
        offset += length;
    }
    return offset;
}

void getStrSplit(char *dist, char *src, int distSize, int line, int strLen)
{
    if (dist == NULL || src == NULL || distSize <= 0 || line <= 0)
        return;

    for (int i = 0; i < line; ++i)
        dist[i * distSize] = '\0';

    int sourceLine = 1;
    int outputLine = 0;
    while (outputLine < line)
    {
        const int isLastSourceLine =
            getStringToken(src, '\n', sourceLine++, sizeof(msg) - 1, msg);
        const char *remaining = msg;

        // strLen is a display-width limit (ASCII=1, CJK/full-width=2), not a
        // byte limit.  Treating it as bytes made UTF-8 Chinese wrap 50% too
        // early and displaced selectable rows and the bottom buttons.
        do
        {
            size_t copyBytes = getUtf8PrefixBytes(remaining, strLen);
            if (copyBytes == 0 && *remaining != '\0')
            {
                // Invalid legacy input: keep progressing without splitting a
                // valid UTF-8 sequence. Display conversion handles the legacy
                // compatibility case at the final Win32 boundary.
                const int legacyBytes = Utf8SafePrefixLength(remaining, strLen);
                if (legacyBytes < 0)
                    copyBytes = min(strlen(remaining), (size_t)(distSize - 1));
                else
                    copyBytes = (size_t)legacyBytes;
            }
            if (copyBytes > (size_t)(distSize - 1))
            {
                const int bufferPrefix =
                    Utf8SafePrefixLength(remaining, distSize - 1);
                copyBytes = bufferPrefix > 0 ? (size_t)bufferPrefix : 0;
            }

            char *output = dist + outputLine * distSize;
            memcpy(output, remaining, copyBytes);
            output[copyBytes] = '\0';
            ++outputLine;
            remaining += copyBytes;
        } while (*remaining != '\0' && outputLine < line);

        if (isLastSourceLine == 1)
            break;
    }
}

void initServerWindowType0(char *data)
{
    msgWN_W = 7;
    msgWN_H = 5;
    msgWNLen = 40;

    makeStringFromEscaped(data);
#ifdef _FONT_STYLE_
    int flg = FALSE;
    char *temp = sunday(data, "[style ");
    if (temp)
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), 256);
    }
    else
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), msgWNLen);
    }
    getStrSplitNew(msgWN);
#else
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), msgWNLen);
#endif
}

void initServerWindowType1(char *data)
{
    makeStringFromEscaped(data);
    if ('0' <= *data && *data <= '7')
    {
        selStartLine = getIntegerToken(data, '\n', 1);
        data += 2;
    }
    else
    {
        selStartLine = 1;
    }
#ifdef _FONT_STYLE_
    int flg = FALSE;
    char *temp = sunday(data, "[style ");
    if (temp)
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), 256);
    }
    else
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), 40);
    }
    getStrSplitNew(msgWN);
#else
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 40);
#endif
}

short shopWindowProcNo;

void initServerWindowType2(char *data)
{
    char msg[256];
    shopWindowProcNo = 0;
    getStringToken(data, '|', 1, sizeof(msg) - 1, msg);
    makeStringFromEscaped(msg);
    strncpy_s(shopWindow1Title, msg, sizeof(shopWindow1Title) - 1);
    shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';
    getStringToken(data, '|', 2, sizeof(msg) - 1, msg);
    makeStringFromEscaped(msg);
    strncpy_s(shopWindow1Msg, msg, sizeof(shopWindow1Msg) - 1);
    shopWindow1Msg[sizeof(shopWindow1Msg) - 1] = '\0';
#ifdef _NEW_MANOR_LAW
    getStringToken(data, '|', 3, sizeof(msg) - 1, msg);
    pc.fame = atoi(msg);
#endif
    menuIndexWN = indexWN;
}

void initServerWindowType3(char *data)
{
    char title1[128];
    char msg1[128];
    char msg2[256];
    char msg3[128];
    char msg4[128];
    char msg5[128];
    char msg6[128];
    char itemName[64];
    char info[256];
    int i, j;
    int flag;
    int datalen;
#ifdef _NEW_MANOR_LAW
    int offset = 7;
#else
    int offset = 6;
#endif

    datalen = 6;

    shopWindowMode = getIntegerToken(data, '|', 1);
    if (shopWindowMode == 0)
    {
        shopWindowProcNo = 10;
        shopWondow2Page = 0;
#ifdef _NEW_SHOP_FRAME
        getStringToken(data, '|', 2, sizeof(shopWindowCurrency) - 1, shopWindowCurrency);
#else
        if (getIntegerToken(data, '|', 2) == 0)
            return;
#endif
        menuIndexWN = getIntegerToken(data, '|', 3);

        getStringToken(data, '|', 4, sizeof(title1) - 1, title1);
        makeStringFromEscaped(title1);
        getStringToken(data, '|', 5, sizeof(msg1) - 1, msg1);
        makeStringFromEscaped(msg1);
        getStringToken(data, '|', 6, sizeof(msg2) - 1, msg2);
        makeStringFromEscaped(msg2);
        getStringToken(data, '|', 7, sizeof(msg3) - 1, msg3);
        makeStringFromEscaped(msg3);
        getStringToken(data, '|', 8, sizeof(msg4) - 1, msg4);
        makeStringFromEscaped(msg4);
        getStringToken(data, '|', 9, sizeof(msg5) - 1, msg5);
        makeStringFromEscaped(msg5);
        getStringToken(data, '|', 10, sizeof(msg6) - 1, msg6);
        makeStringFromEscaped(msg6);

        strncpy_s(shopWindow1Title, title1, sizeof(shopWindow1Title) - 1);
        shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';
        strncpy_s(shopWindow1Msg, msg1, sizeof(shopWindow1Msg) - 1);
        shopWindow1Msg[sizeof(shopWindow1Msg) - 1] = '\0';

        getStrSplit((char *)shopWindow2Msg, msg2,
                    sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg[0]) - 1);

        strncpy_s(shopWindow3Msg, msg3, sizeof(shopWindow3Msg) - 1);
        shopWindow3Msg[sizeof(shopWindow3Msg) - 1] = '\0';

        getStrSplit((char *)shopWindow4Msg, msg4,
                    sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg[0]) - 1);

        getStrSplit((char *)shopWindow5Msg, msg5,
                    sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg[0]) - 1);

        getStrSplit((char *)shopWindow6Msg, msg6,
                    sizeof(shopWindow6Msg[0]),
                    sizeof(shopWindow6Msg) / sizeof(shopWindow6Msg[0]),
                    sizeof(shopWindow6Msg[0]) - 1);

        shopWondow2MaxPage = MAX_SHOP_ITEM * MAX_SHOP_PAGE;
        for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
        {
            flag = getStringToken(data, '|', 11 + i * offset, sizeof(itemName) - 1, itemName);
            makeStringFromEscaped(itemName);
            if (flag)
            {
                sealItem[i].name[0] = '\0';
                continue;
            }

            j++;
            if (strlen(itemName) <= ITEM_NAME_LEN)
            {
                strcpy(sealItem[i].name, itemName);
            }
            else
            {
                strcpy(sealItem[i].name, "???");
            }
            sealItem[i].sealFlag = getIntegerToken(data, '|', 12 + i * offset);
            sealItem[i].level = getIntegerToken(data, '|', 13 + i * offset);
            sealItem[i].price = getIntegerToken(data, '|', 14 + i * offset);
            sealItem[i].graNo = getIntegerToken(data, '|', 15 + i * offset);
            getStringToken(data, '|', 16 + i * offset, sizeof(info) - 1, info);
            makeStringFromEscaped(info);
            getStrSplit((char *)(&sealItem[i].info), info,
                        sizeof(sealItem[0].info[0]),
                        sizeof(sealItem[0].info) / sizeof(sealItem[0].info[0]),
                        sizeof(sealItem[0].info[0]) - 1);
#ifdef _NEW_MANOR_LAW
            sealItem[i].costfame = getIntegerToken(data, '|', 17 + i * offset);
#endif
        }
        if (j > 0)
        {
            shopWondow2MaxPage = (j + 7) / MAX_SHOP_ITEM;
        }
        else
        {
            shopWondow2MaxPage = 1;
        }
    }
    else
    {
#ifdef _ITEM_PILENUMS
        datalen = 7;
#endif

        shopWindowProcNo = 100;
        shopWondow2Page = 0;
        if (getIntegerToken(data, '|', 2) == 0)
            return;
        menuIndexWN = getIntegerToken(data, '|', 3);
        getStringToken(data, '|', 4, sizeof(title1) - 1, title1);
        makeStringFromEscaped(title1);
        getStringToken(data, '|', 5, sizeof(msg1) - 1, msg1);
        makeStringFromEscaped(msg1);
        getStringToken(data, '|', 6, sizeof(msg2) - 1, msg2);
        makeStringFromEscaped(msg2);
        getStringToken(data, '|', 7, sizeof(msg3) - 1, msg3);
        makeStringFromEscaped(msg3);
        getStringToken(data, '|', 8, sizeof(msg4) - 1, msg4);
        makeStringFromEscaped(msg4);

        strncpy_s(shopWindow1Title, title1, sizeof(shopWindow1Title) - 1);
        shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';

        strncpy_s(shopWindow1Msg, msg1, sizeof(shopWindow1Msg) - 1);
        shopWindow1Msg[sizeof(shopWindow1Msg) - 1] = '\0';

        getStrSplit((char *)shopWindow7Msg, msg2,
                    sizeof(shopWindow7Msg[0]),
                    sizeof(shopWindow7Msg) / sizeof(shopWindow7Msg[0]),
                    sizeof(shopWindow7Msg[0]) - 1);

        getStrSplit((char *)shopWindow8Msg, msg3,
                    sizeof(shopWindow8Msg[0]),
                    sizeof(shopWindow8Msg) / sizeof(shopWindow8Msg[0]),
                    sizeof(shopWindow8Msg[0]) - 1);

        getStrSplit((char *)shopWindow5Msg, msg4,
                    sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg[0]) - 1);

        shopWondow2MaxPage = MAX_SHOP_ITEM * MAX_SHOP_PAGE;
        for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
        {
            flag = getStringToken(data, '|', 9 + i * datalen, sizeof(itemName) - 1, itemName);
            makeStringFromEscaped(itemName);
            if (flag)
            {
                userItem[i].name[0] = '\0';
                continue;
            }

            j++;
            if (strlen(itemName) <= ITEM_NAME_LEN)
            {
                strcpy(userItem[i].name, itemName);
            }
            else
            {
                strcpy(userItem[i].name, "???");
            }
            userItem[i].sealFlag = getIntegerToken(data, '|', 10 + i * datalen);
            userItem[i].price = getIntegerToken(data, '|', 11 + i * datalen);
            userItem[i].graNo = getIntegerToken(data, '|', 12 + i * datalen);
            getStringToken(data, '|', 13 + i * datalen, sizeof(info) - 1, info);
            makeStringFromEscaped(info);
            getStrSplit((char *)(&userItem[i].info), info,
                        sizeof(userItem[i].info[0]),
                        sizeof(userItem[i].info) / sizeof(userItem[i].info[0]),
                        sizeof(userItem[i].info[0]) - 1);
            userItem[i].tbl = getIntegerToken(data, '|', 14 + i * datalen);
#ifdef _ITEM_PILENUMS
            userItem[i].num = getIntegerToken(data, '|', 15 + i * datalen);
#endif
        }
        if (j > 0)
        {
            shopWondow2MaxPage = (j + 7) / MAX_SHOP_ITEM;
        }
        else
        {
            shopWondow2MaxPage = 1;
        }
    }
}

// ????????
//  ????????????????
void initServerWindowType4(char *data)
{
    msgWN_W = 9;
    msgWN_H = 9;
    msgWNLen = (msgWN_W * 64 - 48) / 9;

    makeStringFromEscaped(data);
#ifdef _FONT_STYLE_
    int flg = FALSE;
    char *temp = sunday(data, "[style ");
    if (temp)
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), 256);
    }
    else
    {
        getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                    sizeof(msgWN) / sizeof(msgWN[0]), msgWNLen);
    }
    getStrSplitNew(msgWN);
#else
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), msgWNLen);
#endif
}

#ifdef _PETSKILL_CANNEDFOOD
short PetskillShpwWindowProcNo;
void initPetSkillWindowType1(char *data)
{
    selShopSkillPetNo = atoi(data);
    PetskillShpwWindowProcNo = 0;
}
#endif

void initServerWindowType5(char *data)
{
    char title1[128], msg2[128], skillName[64], info[256];
    int i, j, flag;
    int iReadOffset = 3;
#ifdef _PETKILL_COST_FAME
    iReadOffset = 4;
#endif

    skillShopWindowProcNo = 0;
    shopWondow2Page = 0;

    // ?????????????????????
    if (getIntegerToken(data, '|', 1) == 0)
        return;

    getStringToken(data, '|', 2, sizeof(title1) - 1, title1);
    makeStringFromEscaped(title1);
    getStringToken(data, '|', 3, sizeof(msg2) - 1, msg2);
    makeStringFromEscaped(msg2);
    strncpy_s(shopWindow1Title, title1, sizeof(shopWindow1Title) - 1);
    shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';

    getStrSplit((char *)shopWindow2Msg, msg2,
                sizeof(shopWindow2Msg[0]), sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]),
                sizeof(shopWindow2Msg[0]) - 1);

    shopWondow2MaxPage = MAX_SKILL_SHOP_SKILL * MAX_SKILL_SHOP_PAGE;
    for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
    {
        // ???
        flag = getStringToken(data, '|', 4 + i * iReadOffset, sizeof(skillName) - 1, skillName);
        makeStringFromEscaped(skillName);
        if (flag)
        {
            sealSkill[i].name[0] = '\0';
            continue;
        }

        j++;
        if (strlen(skillName) <= SKILL_NAME_LEN)
            strcpy(sealSkill[i].name, skillName);
        else
            strcpy(sealSkill[i].name, "???");

        // 
        sealSkill[i].price = getIntegerToken(data, '|', 5 + i * iReadOffset);

        // ?????????
        getStringToken(data, '|', 6 + i * iReadOffset, sizeof(info) - 1, info);
        makeStringFromEscaped(info);
        getStrSplit((char *)(&sealSkill[i].info), info,
                    sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info[0]) - 1);
#ifdef _PETKILL_COST_FAME
        sealSkill[i].iCoseFame = getIntegerToken(data, '|', 7 + i * iReadOffset);
#endif
    }
    if (j > 0)
        shopWondow2MaxPage = (j + 7) / MAX_SKILL_SHOP_SKILL;
    else
        shopWondow2MaxPage = 1;
}

// ????????????????????
void initServerWindowType6(char *data)
{
    char title1[128];
    char msg1[128];
    char msg2[256];
    char msg3[128];
    char itemName[64];
    char info[256];
    int i, j;
    int flag;

#ifdef _ITEM_PILENUMS
    int datalen = 7;
#else
    int datalen = 6;
#endif

    shopWindowMode = getIntegerToken(data, '|', 1);
    if (shopWindowMode == 0)
    {
        shopWindowProcNo = 10;
        shopWondow2Page = 0;
        restPoolSlot = getIntegerToken(data, '|', 2);
        getStringToken(data, '|', 3, sizeof(title1) - 1, title1);
        makeStringFromEscaped(title1);
        getStringToken(data, '|', 4, sizeof(msg1) - 1, msg1);
        makeStringFromEscaped(msg1);
        getStringToken(data, '|', 5, sizeof(msg2) - 1, msg2);
        makeStringFromEscaped(msg2);
        getStringToken(data, '|', 6, sizeof(msg3) - 1, msg3);
        makeStringFromEscaped(msg3);

        strncpy_s(shopWindow1Title, title1, sizeof(shopWindow1Title) - 1);
        shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';

        getStrSplit((char *)shopWindow2Msg, msg1,
                    sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg[0]) - 1);

        getStrSplit((char *)shopWindow4Msg, msg2,
                    sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg[0]) - 1);

        getStrSplit((char *)shopWindow5Msg, msg3,
                    sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg[0]) - 1);

        shopWondow2MaxPage = MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE;
        for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
        {
            flag = getStringToken(data, '|', 7 + i * datalen, sizeof(itemName) - 1, itemName);
            makeStringFromEscaped(itemName);
            if (flag)
            {
                poolItem[i].name[0] = '\0';
                continue;
            }

            j++;
            if (strlen(itemName) <= ITEM_NAME_LEN)
            {
                strcpy(poolItem[i].name, itemName);
            }
            else
            {
                strcpy(poolItem[i].name, "???");
            }
            poolItem[i].poolFlag = getIntegerToken(data, '|', 8 + i * datalen);
            poolItem[i].price = getIntegerToken(data, '|', 9 + i * datalen);
            poolItem[i].graNo = getIntegerToken(data, '|', 10 + i * datalen);
            getStringToken(data, '|', 11 + i * datalen, sizeof(info) - 1, info);

#ifdef _ITEM_PILENUMS
            poolItem[i].num = getIntegerToken(data, '|', 12 + i * datalen);
#endif
            makeStringFromEscaped(info);
            getStrSplit((char *)(&poolItem[i].info), info,
                        sizeof(poolItem[0].info[0]),
                        sizeof(poolItem[0].info) / sizeof(poolItem[0].info[0]),
                        sizeof(poolItem[0].info[0]) - 1);
#ifdef _ITEM_PILENUMS
            poolItem[i].tbl = getIntegerToken(data, '|', 13 + i * datalen);
#else
            poolItem[i].tbl = getIntegerToken(data, '|', 12 + i * datalen);
#endif
        }
        if (j > 0)
        {
            shopWondow2MaxPage = (j + 7) / MAX_POOL_SHOP_ITEM;
        }
        else
        {
            shopWondow2MaxPage = 1;
        }
    }
    else
    {
        shopWindowProcNo = 100;
        shopWondow2Page = 0;
        getStringToken(data, '|', 2, sizeof(title1) - 1, title1);
        makeStringFromEscaped(title1);
        getStringToken(data, '|', 3, sizeof(msg1) - 1, msg1);
        makeStringFromEscaped(msg1);
        getStringToken(data, '|', 4, sizeof(msg2) - 1, msg2);
        makeStringFromEscaped(msg2);
        getStringToken(data, '|', 5, sizeof(msg3) - 1, msg3);
        makeStringFromEscaped(msg3);

        strncpy_s(shopWindow1Title, title1, sizeof(shopWindow1Title) - 1);
        shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';
        getStrSplit((char *)shopWindow2Msg, msg1,
                    sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]),
                    sizeof(shopWindow2Msg[0]) - 1);

        getStrSplit((char *)shopWindow4Msg, msg2,
                    sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]),
                    sizeof(shopWindow4Msg[0]) - 1);

        getStrSplit((char *)shopWindow5Msg, msg3,
                    sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]),
                    sizeof(shopWindow5Msg[0]) - 1);

        shopWondow2MaxPage = MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE;
        for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
        {
            // ????
            flag = getStringToken(data, '|', 6 + i * datalen, sizeof(itemName) - 1, itemName);
            makeStringFromEscaped(itemName);
            if (flag)
            {
                poolItem[i].name[0] = '\0';
                continue;
            }

            j++;
            if (strlen(itemName) <= ITEM_NAME_LEN)
            {
                strcpy(poolItem[i].name, itemName);
            }
            else
            {
                strcpy(poolItem[i].name, "???");
            }
            poolItem[i].poolFlag = getIntegerToken(data, '|', 7 + i * datalen);
            poolItem[i].level = getIntegerToken(data, '|', 8 + i * datalen);
            poolItem[i].price = getIntegerToken(data, '|', 9 + i * datalen);
            poolItem[i].graNo = getIntegerToken(data, '|', 10 + i * datalen);
            getStringToken(data, '|', 11 + i * datalen, sizeof(info) - 1, info);
#ifdef _ITEM_PILENUMS
            poolItem[i].num = getIntegerToken(data, '|', 12 + i * datalen);
#endif
            makeStringFromEscaped(info);
            getStrSplit((char *)(&poolItem[i].info), info,
                        sizeof(poolItem[i].info[0]),
                        sizeof(poolItem[i].info) / sizeof(poolItem[i].info[0]),
                        sizeof(userItem[i].info[0]) - 1);
        }
        if (j > 0)
        {
            shopWondow2MaxPage = (j + 7) / MAX_POOL_SHOP_ITEM;
        }
        else
        {
            shopWondow2MaxPage = 1;
        }
    }
}

void initServerWindowType7(char *data)
{
    shopWindowProcNo = 0;
    getStringToken(data, '|', 1, sizeof(msg) - 1, msg);
    makeStringFromEscaped(msg);
    strncpy_s(shopWindow1Title, msg, sizeof(shopWindow1Title) - 1);
    shopWindow1Title[sizeof(shopWindow1Title) - 1] = '\0';

    getStringToken(data, '|', 2, sizeof(msg) - 1, msg);
    makeStringFromEscaped(msg);
    strncpy_s(shopWindow1Msg, msg, sizeof(shopWindow1Msg) - 1);
    shopWindow1Msg[sizeof(shopWindow1Msg) - 1] = '\0';
}

void serverWindowType0(int mode)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static STR_BUFFER input;
    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        winW = msgWN_W;
        winH = msgWN_H;
#ifdef _NEW_MANOR_LAW
        if (mode == 2)
            winH = 7;
#endif
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);

        lineSkip = 20;
        totalMsgLine = (winH * 48 - 56) / lineSkip;

        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
        {
            fontId[i] = -2;
        }

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        // ?????????????
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = ((winH * 48 - 56) / totalMsgLine) * (totalMsgLine - 1) + (winH * 48 - totalMsgLine * lineSkip) / 2 + 8;
            }
        }

        // ?????????????
        if (mode == 0 || mode == 2)
        {
            msgLine = totalMsgLine - 1;
            input.buffer[0] = '\0';
        }
        else if (mode == 1)
        {
            msgLine = totalMsgLine - 2;
            input.buffer[0] = '\0';
            input.cnt = 0;
            input.cursor = 0;
            initStrBuffer(&input,
                          winX + (msgWN_W * 64 - msgWNLen * 9) / 2,
                          winY + ((winH * 48 - 56) / totalMsgLine) * (totalMsgLine - 2) + (winH * 48 - totalMsgLine * lineSkip) / 2,
                          msgWNLen, FONT_PAL_WHITE, FONT_PRIO_FRONT);
            GetKeyInputFocus(&input);
        }

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));

        // ?????????????????????
        if (CheckMenuFlag() || ((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        // ＯＫ
        if (0 <= id && id < 6)
        {
            btn = 1;
            btn <<= id;
            makeEscapeString(input.buffer, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }

        if (id >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (mode == 1)
            {
                GetKeyInputFocus(&MyChatBuffer);
            }
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
#ifdef _FONT_STYLE_
                    PutWinText(winX + (msgWN_W * 64 - msgWNLen * 9) / 2,
                               winY + ((winH * 48 - 56) / totalMsgLine) * i + (winH * 48 - totalMsgLine * lineSkip) / 2,
                               FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0, i);
#else
                    StockFontBuffer(winX + (msgWN_W * 64 - msgWNLen * 9) / 2,
                                    winY + ((winH * 48 - 56) / totalMsgLine) * i + (winH * 48 - totalMsgLine * lineSkip) / 2,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
#endif
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
            // ???
            if (mode == 1)
            {
                StockFontBuffer2(&input);
            }
        }
    }
}

void serverWindowType1(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 5;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 196;
            }
        }
        msgLine = 10; // msgLine = 8;
        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));
        id2 = -1;
        if (ptActMenuWin->hp >= 1) // ????????????
        {
            for (i = selStartLine; i < msgLine; i++)
            {
                // ????????????
                if (strlen(msgWN[i]) > 0)
                {
                    x1 = winX + 41;
                    y1 = winY + 18 + i * 21;
                    x2 = x1 + 366;
                    y2 = y1 + 20;
#ifdef _NEWFONT_
                    if (MakeHitBox(x1, y1 + 2, x2, y2 + 2, DISP_PRIO_BOX2))
#else
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
#endif
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id2 = i - selStartLine + 1;
                        }
                    }
                }
            }
        }

        // ?????????????????????
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 10 || id == 100)
        {
            if (id == 100)
                btn = WINDOW_BUTTONTYPE_CANCEL;
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d", id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);
#ifdef _MOVE_SCREEN
                if (pc.bMoveScreenMode)
                    lssproto_WN_send(sockfd, nowGx - iScreenMoveX, nowGy - iScreenMoveY, indexWN, idWN, btn, msg);
                else
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
#else
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
#endif
            windowTypeWN = -1;
        }

        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
#ifdef _NEWFONT_
#ifdef _FONT_STYLE_
                    PutWinText(winX + 80, winY + 21 + i * 21,
                               FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0, i);
#else
                    StockFontBuffer(winX + 80, winY + 21 + i * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
#endif
#else
                    StockFontBuffer(winX + 44, winY + 21 + i * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
#endif
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

void serverWindowType2(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    int len;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 5;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 196;
            }
        }
        strcpy(msgWN[0], "＝＝＝         请选择宠物         ＝＝＝");
        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));
        id2 = -1;
        if (ptActMenuWin->hp >= 1)
        {
            for (i = 1, j = 1; i <= MAX_PET; i++)
            {
                if (pet[i - 1].useFlag != 0)
                {
                    x1 = winX + 41;
                    y1 = winY + 18 + (j + 1) * 21;
                    x2 = x1 + 366;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id2 = i;
                        }
                    }
                    j++;
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 8)
        {
            if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d", id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }

        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 54, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0);
            for (i = 1, j = 1; i <= MAX_PET; i++)
            {
                len = getUtf8CharNum(pet[i - 1].name);
                if (pet[i - 1].useFlag != 0)
                {
                    len = getUtf8CharNum(pet[i - 1].freeName);
                    if (len > 0)
                    {
                        strcpy(msgWN[1], pet[i - 1].freeName);
                    }
                    else
                    {
                        len = getUtf8CharNum(pet[i - 1].name);
                        strcpy(msgWN[1], pet[i - 1].name);
                    }
                    StockFontBuffer(winX + 42, winY + 21 + (j + 1) * 21, // modified by zhuo 62改成42
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    sprintf_s(msgWN[1], "LV.%d", pet[i - 1].level);
                    StockFontBuffer(winX + 260, winY + 21 + (j + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    sprintf_s(msgWN[1], "MaxHP %d", pet[i - 1].maxHp);
                    StockFontBuffer(winX + 314, winY + 21 + (j + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    j++;
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

void serverWindowType3(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    int len;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 5;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        // ?????????????
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 196;
            }
        }

        strcpy(msgWN[0], "＝＝＝         请选择队友       ＝＝＝");

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ????
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;
        if (ptActMenuWin->hp >= 1) // ????????????
        {
            for (i = 1; i <= MAX_PARTY; i++)
            {
                // ????????????
                if (partyModeFlag == 0 && i == 1)
                {
                    strcpy(msgWN[1], pc.name);
                }
                else if (party[i - 1].useFlag != 0)
                {
                    strcpy(msgWN[1], party[i - 1].name);
                }
                else
                {
                    msgWN[1][0] = '\0';
                }
                len = getUtf8CharNum(msgWN[1]);
                if (len > 0)
                {
                    x1 = winX + 41;
                    y1 = winY + 18 + (i + 1) * 21;
                    x2 = x1 + 366;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id2 = i + 5;
                        }
                    }
                }
            }
        }

        // ?????????????????????
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }
        if (0 <= id && id < 6 || 0 <= id2 && id2 < 8)
        {
            if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d", id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }

        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 62, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0);
            for (i = 1; i <= MAX_PARTY; i++)
            {
                if (partyModeFlag == 0 && i == 1)
                {
                    strcpy(msgWN[1], pc.name);
                }
                else if (party[i - 1].useFlag != 0)
                {
                    strcpy(msgWN[1], party[i - 1].name);
                }
                else
                {
                    msgWN[1][0] = '\0';
                }
                len = getUtf8CharNum(msgWN[1]);
                if (len > 0)
                {
                    StockFontBuffer(winX + 152 + (144 - len * 9) / 2, winY + 21 + (i + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

// ?????????
// ??????
void serverWindowType4(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    int len;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 5;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 196;
            }
        }

        strcpy(msgWN[0], "＝＝＝    请选择宠物或是队友    ＝＝＝");

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ????
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;
        if (ptActMenuWin->hp >= 1) // ????????????
        {
            for (i = 1; i <= MAX_PET; i++)
            {
                // ????????????
                if (pet[i - 1].useFlag != 0)
                {
                    x1 = winX + 41;
                    y1 = winY + 18 + (i + 1) * 21;
                    x2 = x1 + 149;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id2 = i;
                        }
                    }
                }
            }
        }

        for (i = 1; i <= MAX_PARTY; i++)
        {
            // ????????????
            if (partyModeFlag == 0 && i == 1)
            {
                strcpy(msgWN[1], pc.name);
            }
            else if (party[i - 1].useFlag != 0)
            {
                strcpy(msgWN[1], party[i - 1].name);
            }
            else
            {
                msgWN[1][0] = '\0';
            }
            len = getUtf8CharNum(msgWN[1]);
            if (len > 0)
            {
                x1 = winX + 257;
                y1 = winY + 18 + (i + 1) * 21;
                x2 = x1 + 149;
                y2 = y1 + 20;
                if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                {
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        id2 = i + 5;
                    }
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 10)
        {
            if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d", id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }

        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 62, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0);

            // ???
            for (i = 1; i <= MAX_PET; i++)
            {
                len = getUtf8CharNum(pet[i - 1].name);
                if (pet[i - 1].useFlag != 0)
                {
                    len = getUtf8CharNum(pet[i - 1].freeName);
                    if (len > 0)
                    {
                        strcpy(msgWN[1], pet[i - 1].freeName);
                    }
                    else
                    {
                        len = getUtf8CharNum(pet[i - 1].name);
                        strcpy(msgWN[1], pet[i - 1].name);
                    }
                    StockFontBuffer(winX + 44 + (144 - len * 9) / 2, winY + 21 + (i + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);
                }
            }

            // ????
            for (i = 1; i <= MAX_PARTY; i++)
            {
                if (partyModeFlag == 0 && i == 1)
                {
                    strcpy(msgWN[1], pc.name);
                }
                else if (party[i - 1].useFlag != 0)
                {
                    strcpy(msgWN[1], party[i - 1].name);
                }
                else
                {
                    msgWN[1][0] = '\0';
                }
                len = getUtf8CharNum(msgWN[1]);
                if (len > 0)
                {
                    StockFontBuffer(winX + 260 + (144 - len * 9) / 2, winY + 21 + (i + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

void serverWindowType9(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int selectID[3] = {-1, -1, -1};
    static int selects = 0;

    int id, id2;
    int i, j;
    int mask, btn, len;
    int x1, y1, x2, y2;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 5;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 196;
            }
        }
        if (selects == 0)
        {
            strcpy(msgWN[0], "＝＝＝       请选择主体宠物     ＝＝＝");
        }
        else
        {
            strcpy(msgWN[0], "＝＝＝       请选择客体宠物     ＝＝＝");
        }
        play_se(202, 320, 240); // ????????
    }
    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));
        id2 = -1;
        if (ptActMenuWin->hp >= 1)
        {
            for (i = 1, j = 1; i <= MAX_PET; i++)
            {
                if (pet[i - 1].useFlag != 0)
                {
                    x1 = winX + 41;
                    y1 = winY + 18 + (j + 1) * 21;
                    x2 = x1 + 366;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id2 = i;
                            if (selects < 3)
                            {
                                int k;
                                for (k = 0; k < 3; k++)
                                {
                                    if (selectID[k] == i)
                                        break;
                                }
                                if (k >= 3)
                                {
                                    selectID[selects++] = i;
                                }
                            }
                        }
                    }
                    j++;
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6)
        {
            if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            // sprintf_s( data, "%d", id2 );//selectID
            sprintf_s(data, "%d|%d|%d", selectID[0], selectID[1], selectID[2]);
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            for (i = 0; i < 3; i++)
            {
                selectID[i] = -1;
            }
            selects = 0;
            windowTypeWN = -1;
        }

        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 54, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0);
            for (i = 1, j = 1; i <= MAX_PET; i++)
            {
                len = getUtf8CharNum(pet[i - 1].name);
                if (pet[i - 1].useFlag != 0)
                {
                    len = getUtf8CharNum(pet[i - 1].freeName);
                    if (len > 0)
                    {
                        strcpy(msgWN[1], pet[i - 1].freeName);
                    }
                    else
                    {
                        len = getUtf8CharNum(pet[i - 1].name);
                        strcpy(msgWN[1], pet[i - 1].name);
                    }
                    StockFontBuffer(winX + 62, winY + 21 + (j + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    // selectID
                    if (selectID[0] == i)
                    {
                        sprintf_s(msgWN[1], "主体");
                        StockFontBuffer(winX + 220, winY + 21 + (j + 1) * 21,
                                        FONT_PRIO_FRONT, FONT_PAL_RED, msgWN[1], 0);
                    }
                    else if (selectID[1] == i || selectID[2] == i)
                    {
                        sprintf_s(msgWN[1], "客体");
                        StockFontBuffer(winX + 220, winY + 21 + (j + 1) * 21,
                                        FONT_PRIO_FRONT, FONT_PAL_GREEN, msgWN[1], 0);
                    }
                    sprintf_s(msgWN[1], "LV.%d", pet[i - 1].level);
                    StockFontBuffer(winX + 260, winY + 21 + (j + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);
                    sprintf_s(msgWN[1], "MaxHP %d", pet[i - 1].maxHp);
                    StockFontBuffer(winX + 314, winY + 21 + (j + 1) * 21,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    j++;
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

void initShopWindow1(void);
int shopWindow1(void);
void initShopWindow2(void);
int shopWindow2(void);
void initShopWindow3(void);
int shopWindow3(void);
void initShopWindow4(void);
int shopWindow4(void);
void initShopWindow5(void);
int shopWindow5(void);
void initShopWindow6(void);
int shopWindow6(void);
void initShopWindow7(void);
int shopWindow7(void);
void initShopWindow8(void);
int shopWindow8(void);

#ifdef _ITEM_PILENUMS
void initShopWindow10(void);
int shopWindow10(void);
#endif

void serverWindowType5(void)
{
    int ret;

    if (shopWindowProcNo == 0)
    {
        initShopWindow1();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 1)
    {
        ret = shopWindow1();
    }

    if (shopWindowProcNo == 15)
    {
        initShopWindow2();
        shopWindowProcNo = 11;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 10)
    {
        // Do not carry the previous shop session's cached capacity into a new
        // conversation.  The item packet may have changed while the shop was
        // closed (move, drop, trade, bank, etc.).
        nowUserItemCnt = GetEmptyPlayerItemSlotCount();

        initShopWindow2();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 11)
    {
        ret = shopWindow2();
        if (ret == 1)
        {
            sprintf_s(data, "0");
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            windowTypeWN = -1;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 30;
        }
        else if (ret == 3)
        {
            shopWindowProcNo = 20;
        }
        if (ret == 4)
        {
            shopWindowProcNo = 50;
        }
    }
    if (shopWindowProcNo == 20)
    {
        initShopWindow4();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 21)
    {
        ret = shopWindow4();
        if (ret == 1)
        {
            shopWindowProcNo = 30;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 15;
        }
    }
    if (shopWindowProcNo == 30)
    {
        initShopWindow3();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 31)
    {
        ret = shopWindow3();
        if (ret == 1)
        {
            shopWindowProcNo = 40;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 15;
        }
    }
    if (shopWindowProcNo == 40)
    {
        initShopWindow5();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 41)
    {
        ret = shopWindow5();
        if (ret == 1)
        {
            sprintf_s(data, "%d|%d", selShopItemNo + 1, sealItemCnt);
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            nowUserItemCnt--;
            pc.gold -= sealItem[selShopItemNo].price * sealItemCnt;
#ifdef _NEW_MANOR_LAW
            if (sealItem[selShopItemNo].costfame > 0)
                pc.fame -= sealItem[selShopItemNo].costfame * sealItemCnt;
#endif
            shopWindowProcNo = 15;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 30;
        }
    }
    if (shopWindowProcNo == 50)
    {
        initShopWindow6();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 51)
    {
        ret = shopWindow6();
        if (ret == 1)
        {
            shopWindowProcNo = 15;
        }
    }

    if (shopWindowProcNo == 100)
    {
        initShopWindow7();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 101)
    {
        ret = shopWindow7();
        if (ret == 1)
        {
            sprintf_s(data, "0");
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            windowTypeWN = -1;
        }
        else if (ret == 2)
            shopWindowProcNo = 110;
        else if (ret == 3)
            shopWindowProcNo = 120;
    }

    if (shopWindowProcNo == 110)
    {
#ifdef _ITEM_PILENUMS
        initShopWindow10();
#else
        initShopWindow5();
#endif
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 111)
    {
#ifdef _ITEM_PILENUMS
        ret = shopWindow10();
#else
        ret = shopWindow5();
#endif
        if (ret == 1)
        {
#ifdef _ITEM_PILENUMS
            sprintf_s(data, "%d|%d", userItem[selShopItemNo].tbl, sealItemCnt);
#else
            sprintf_s(data, "%d|%d", userItem[selShopItemNo].tbl, userItem[selShopItemNo].price);
#endif
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);

            pc.gold += userItem[selShopItemNo].price;
            int i, j, k;
#ifdef _ITEM_PILENUMS
            shopWindowProcNo = 100;
            userItem[selShopItemNo].num -= sealItemCnt;
            if (userItem[selShopItemNo].num <= 0)
            {
#else
            shopWindowProcNo = 100;
#endif
                userItem[selShopItemNo].name[0] = '\0';
                for (i = 0; i < MAX_SHOP_ITEM * MAX_SHOP_PAGE - 1; i++)
                {
                    if (userItem[i].name[0] == '\0')
                    {
                        for (j = i + 1; j < MAX_SHOP_ITEM * MAX_SHOP_PAGE; j++)
                        {
                            if (userItem[j].name[0] != '\0')
                            {
                                strcpy(userItem[i].name, userItem[j].name);
                                userItem[i].sealFlag = userItem[j].sealFlag;
                                userItem[i].level = userItem[j].level;
                                userItem[i].price = userItem[j].price;
                                userItem[i].graNo = userItem[j].graNo;
                                userItem[i].tbl = userItem[j].tbl;
#ifdef _ITEM_PILENUMS
                                userItem[i].num = userItem[j].num;
#endif
                                for (k = 0; k < 3; k++)
                                {
                                    strcpy(userItem[i].info[k], userItem[j].info[k]);
                                }
                                userItem[j].name[0] = '\0';
                                break;
                            }
                        }
                    }
                }
#ifdef _ITEM_PILENUMS
            }
#endif
            for (i = 0; i < MAX_SHOP_ITEM * MAX_SHOP_PAGE; i++)
            {
                if (userItem[i].name[0] == '\0')
                    break;
            }
            if (i > 0)
            {
                shopWondow2MaxPage = (i + 7) / MAX_SHOP_ITEM;
            }
            else
            {
                shopWondow2MaxPage = 1;
            }
            if (shopWondow2Page >= shopWondow2MaxPage)
                shopWondow2Page = shopWondow2MaxPage - 1;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 100;
        }
    }

    if (shopWindowProcNo == 120)
    {
        initShopWindow8();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 121)
    {
        ret = shopWindow8();
        if (ret == 1)
        {
            shopWindowProcNo = 100;
        }
    }
}

short shopWindow1ProcNo;

void initShopWindow1(void)
{
    shopWindow1ProcNo = 0;
}

int shopWindow1(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id = 0;
    int i;
    int ret = 0;

    if (shopWindow1ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 4;
        h = 3;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        shopWindow1ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
            ret = id + 1;
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            ret = 3;
            wnCloseFlag = 0;
        }
        if (0 <= id && id <= 2)
        {
            sprintf_s(data, "%d", ret);
            makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
        }

        if (id >= 0)
        {
            if (ret == 3)
            {
                play_se(203, 320, 240); // ????????
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            windowTypeWN = -1;
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            xx = (w * 64 - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 12, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            xx = (w * 64 - getUtf8CharNum(shopWindow1Msg) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 32, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Msg, 0);

            btnId[0] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 66, DISP_PRIO_IME3, CG_BUY_BTN, 2);
            btnId[1] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 90, DISP_PRIO_IME3, CG_SEAL_BTN, 2);
            btnId[2] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 114, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
        }
    }

    return ret;
}

short shopWindow2ProcNo;
void initShopWindow2(void)
{
    shopWindow2ProcNo = 0;
}

int shopWindow2(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (shopWindow2ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        shopWindow2ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (sealItem[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {

                            if (sealItem[j + i].price <= pc.gold &&
                                sealItem[j + i].sealFlag == 0
#ifdef _NEW_MANOR_LAW
                                && sealItem[j + i].costfame <= pc.fame
#endif
                            )
                            {
#ifdef _EVIL_KILL
                                if (pc.ftype != 0)
                                {
                                    if (sealItem[j + i].costfame <= pc.newfame)
                                        selId = j + i;
                                    else
                                        play_se(220, 320, 240);
                                }
                                else
#endif
                                    selId = j + i;
                            }
                            else
                            {
#ifdef _EVIL_KILL
                                if (pc.ftype != 0)
                                {
                                    if (sealItem[j + i].costfame <= pc.newfame)
                                        selId = j + i;
                                    else
                                        play_se(220, 320, 240);
                                }
                                else
#endif
                                    play_se(220, 320, 240);
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            prevBtn = 0;
        }
        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            nextBtn = 0;
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2 || selId >= 0)
        {
            if (id == 2)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopItemNo = selId;
                sealItemCnt = 1;
                // Capacity is mutable while this window is open.  Re-read it
                // before rejecting the purchase, otherwise a stale zero makes
                // every merchant report that the inventory is full.
                nowUserItemCnt = GetEmptyPlayerItemSlotCount();
                if (nowUserItemCnt <= 0)
                {
                    ret = 4;
                }
                else if (sealItem[selId].level > pc.level)
                {
                    ret = 3;
                }
                else
                {
                    ret = 2;
                }
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;
            xx = (w - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);
            for (i = 0; i < sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]); i++)
            {
                if (shopWindow2Msg[0] != '\0')
                {
                    StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow2Msg[i], 0);
                }
            }
#ifdef _NEW_SHOP_FRAME
            if (atoi(shopWindowCurrency) != 1)
            {
                StockFontBuffer(x + 244, y + 64, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindowCurrency, 0);
            }
            else
#endif
#ifdef _NEW_MANOR_LAW
#ifdef _EVIL_KILL
                if (pc.ftype == 0)
                sprintf_s(tmsg, "声望 %8dＦ", pc.fame);
            else if (pc.ftype == 1)
                sprintf_s(tmsg, "白狼 %8dＤ", pc.newfame);
            else if (pc.ftype == 2)
                sprintf_s(tmsg, "法师 %8dＤ", pc.newfame);
            else if (pc.ftype == 3)
                sprintf_s(tmsg, "追猎 %8dＤ", pc.newfame);
            else
                sprintf_s(tmsg, "声望 %8dＦ", pc.fame);

            StockFontBuffer(x + 244, y + 64, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
#else
                sprintf_s(tmsg, "声望 %8dＦ", pc.fame);
            StockFontBuffer(x + 244, y + 64, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
#endif
#endif

            sprintf_s(tmsg, "金钱  %8dＳ", pc.gold);

            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (strlen(sealItem[j + i].name) == 0)
                    continue;
                color = itemColor[0];
                if (sealItem[j + i].level > pc.level)
                    color = itemColor[2];
                if (sealItem[j + i].sealFlag)
                    color = itemColor[1];
                if (sealItem[j + i].price > pc.gold)
                {
                    color = itemColor[1];
                }
#ifdef _NEW_MANOR_LAW
                if (sealItem[j + i].costfame > pc.fame)
                {
                    color = itemColor[1];
                }
#endif
#ifdef _EVIL_KILL
                if (pc.ftype != 0)
                {
                    if (sealItem[j + i].costfame > pc.newfame)
                        color = itemColor[1];
                    else
                        color = itemColor[0];
                }
#endif
                StockFontBuffer(x + 34, y + 118 + i * 21, FONT_PRIO_FRONT, color, sealItem[j + i].name, 0);
#ifdef _NEW_SHOP_FRAME
                if (sealItem[j + i].costfame > -1)
                {
                    sprintf_s(tmsg, "%8d点%8dＳ", sealItem[j + i].costfame, sealItem[j + i].price);
                    StockFontBuffer(x + 242, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
                }
                else
#else
#ifdef _NEW_MANOR_LAW
                if (sealItem[j + i].costfame > -1)
                {
#ifdef _EVIL_KILL
                    if (pc.ftype != 0)
                        sprintf_s(tmsg, "%8dＤ%8dＳ", sealItem[j + i].costfame, sealItem[j + i].price);
                    else
                        sprintf_s(tmsg, "%8dＦ%8dＳ", sealItem[j + i].costfame, sealItem[j + i].price);
                    StockFontBuffer(x + 242, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#else
#ifdef _NEWFONT_
                    sprintf_s(tmsg, "%8dＦ", sealItem[j + i].costfame);
                    StockFontBuffer(x + 242, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
                    sprintf_s(tmsg, "%8dＳ", sealItem[j + i].price);
                    StockFontBuffer(x + 242 + 120 - getTextLength(tmsg) / 2, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);

#else
                    sprintf_s(tmsg, "%8dＦ%8dＳ", sealItem[j + i].costfame, sealItem[j + i].price);
                    StockFontBuffer(x + 242, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#endif

#endif
                }
                else
#endif
#endif
                {
#ifdef _NEW_SHOP_FRAME
                    if (atoi(shopWindowCurrency) != 1)
                    {
                        sprintf_s(tmsg, "%8d点", sealItem[j + i].price);
                    }
                    else
#endif
                        sprintf_s(tmsg, "%8dＳ", sealItem[j + i].price);

                    StockFontBuffer(x + 312, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
                }
            }
            if (focusId >= 0)
            {
                StockDispBuffer(x + 64, y + 350, DISP_PRIO_IME3, sealItem[focusId].graNo, 0);
                for (i = 0; i < sizeof(sealItem[0].info) / sizeof(sealItem[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    sealItem[focusId].info[i], 0);
                }
            }
            btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1);
        }
    }
    return ret;
}

short shopWindow3ProcNo;
void initShopWindow3(void)
{
    shopWindow3ProcNo = 0;
}

int shopWindow3(void)
{
    static int x, y, w, h;
    static int btnId[4];
    int upBtnGraNo[] = {CG_UP_BTN, CG_UP_BTN_DOWN};
    int downBtnGraNo[] = {CG_DOWN_BTN, CG_DOWN_BTN_DOWN};
    int upBtn, downBtn;
    int id;
    int pushId = 0;
    int i;
    int ret = 0;
    char tmsg[256];

    if (shopWindow3ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 440;
        h = 160;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        shopWindow3ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            downBtn = 1;
            if (id == 0)
            {
                if (sealItemCnt > 1)
                {
                    sealItemCnt--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            downBtn = 0;
        }
        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            upBtn = 1;
            if (id == 1)
            {
                if (sealItemCnt + 1 <= nowUserItemCnt && sealItem[selShopItemNo].price * (sealItemCnt + 1) <= pc.gold
#ifdef _NEW_MANOR_LAW
                    && sealItem[selShopItemNo].costfame * (sealItemCnt + 1) <= pc.fame
#endif
                )
                {
                    sealItemCnt++;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            upBtn = 0;
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2)
        {
            if (id == 2)
            {
                ret = 1;
            }
            else if (id == 3)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }
        if (ptActMenuWin->hp >= 1)
        {
            int xx;
            xx = (w - getUtf8CharNum(shopWindow3Msg) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow3Msg, 0);
            StockFontBuffer(x + 16, y + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            sealItem[selShopItemNo].name, 0);
            sprintf_s(tmsg, "%2d个", sealItemCnt);
            StockFontBuffer(x + 300, y + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            sprintf_s(tmsg, "%8dＳ x %2d = %8dＳ", sealItem[selShopItemNo].price,
                      sealItemCnt, sealItem[selShopItemNo].price * sealItemCnt);
            StockFontBuffer(x + 16, y + 82, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            tmsg, 0);
#ifdef _NEW_MANOR_LAW
            if (sealItem[selShopItemNo].costfame > -1)
            {
#ifdef _EVIL_KILL
                if (pc.ftype != 0)
                    sprintf_s(tmsg, "%8dＤ x %2d = %8dＤ", sealItem[selShopItemNo].costfame, sealItemCnt,
                              sealItem[selShopItemNo].costfame * sealItemCnt);
                else
#endif
                    sprintf_s(tmsg, "%8dＦ x %2d = %8dＦ", sealItem[selShopItemNo].costfame, sealItemCnt,
                              sealItem[selShopItemNo].costfame * sealItemCnt);
                StockFontBuffer(x + 16, y + 102, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            }
#ifdef _EVIL_KILL
            if (pc.ftype == 1)
                sprintf_s(tmsg, "白狼 %8dＤ", pc.newfame);
            else if (pc.ftype == 2)
                sprintf_s(tmsg, "法师 %8dＤ", pc.newfame);
            else if (pc.ftype == 3)
                sprintf_s(tmsg, "追猎 %8dＤ", pc.newfame);
            else
#endif
                sprintf_s(tmsg, "声望 %8dＦ", pc.fame);
            StockFontBuffer(x + 248, y + 100, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
#endif
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 248, y + 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            btnId[0] = StockDispBuffer(x + 364, y + 60, DISP_PRIO_IME3, downBtnGraNo[downBtn], 2);
            btnId[1] = StockDispBuffer(x + 404, y + 60, DISP_PRIO_IME3, upBtnGraNo[upBtn], 2);

            btnId[2] = StockDispBuffer(x + 64, y + 140, DISP_PRIO_IME3, CG_OK_BTN, 2);
            btnId[3] = StockDispBuffer(x + 164, y + 140, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_KOSU_WIN, 1);
        }
    }

    return ret;
}
short shopWindow4ProcNo;
void initShopWindow4(void)
{
    shopWindow4ProcNo = 0;
}

int shopWindow4(void)
{
    static int x, y, w, h;
    static int btnId[2];
    int id;
    int i;
    int ret = 0;
    if (shopWindow4ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 6;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        shopWindow4ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]); i++)
            {
                if (shopWindow4Msg[0] != '\0')
                {
                    StockFontBuffer(x + 20, y + 20 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow4Msg[i], 0);
                }
            }

            btnId[0] = StockDispBuffer(x + 128, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + 256, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}

short shopWindow5ProcNo;

void initShopWindow5(void)
{
    shopWindow5ProcNo = 0;
}

int shopWindow5(void)
{
    static int x, y, w, h;
    static int btnId[2];
    int id;
    int i;
    int ret = 0;

    if (shopWindow5ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 6;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        shopWindow5ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }
        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]); i++)
            {
                if (shopWindow5Msg[0] != '\0')
                {
                    StockFontBuffer(x + 20, y + 20 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow5Msg[i], 0);
                }
            }
            btnId[0] = StockDispBuffer(x + 128, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + 256, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}
short shopWindow6ProcNo;
void initShopWindow6(void)
{
    shopWindow6ProcNo = 0;
}

int shopWindow6(void)
{
    static int x, y, w, h;
    static int btnId[1];
    int id;
    int i;
    int ret = 0;

    if (shopWindow6ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        w = 6;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        shopWindow6ProcNo++;
    }
    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            // ?????????
            for (i = 0; i < sizeof(shopWindow6Msg) / sizeof(shopWindow6Msg[0]); i++)
            {
                if (shopWindow6Msg[0] != '\0')
                {
                    StockFontBuffer(x + 20, y + 20 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow6Msg[i], 0);
                }
            }

            btnId[0] = StockDispBuffer(x + 192, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
        }
    }

    return ret;
}

short shopWindow7ProcNo;

void initShopWindow7(void)
{
    shopWindow7ProcNo = 0;
}

int shopWindow7(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (shopWindow7ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        shopWindow7ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            // ????
            // id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (userItem[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (userItem[j + i].sealFlag == 0)
                            {
                                selId = j + i;
                            }
#ifdef _EVIL_KILL // 以前玩家卖的话  sealflag只有 0 或 1
                            if (userItem[j + i].sealFlag == 2)
                            {
                                selId = j + i;
                            }
#endif
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        // ??????
        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            prevBtn = 0;
        }
        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
                id = 1;
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
            nextBtn = 0;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        // id = 0, id = 1 ??????????????
        if (id >= 2 || selId >= 0)
        {
            // ?????
            if (id == 2)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopItemNo = selId;
                sealItemCnt = 1;
                if (userItem[selShopItemNo].price + pc.gold > CHAR_getMaxHaveGold())
                {
                    ret = 3;
#ifdef _EVIL_KILL
                    if (userItem[selShopItemNo].sealFlag == 2)
                        ret = 2;
#endif
                }
                else
                {
                    ret = 2;
                }
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || (buyOffFlag == 1 && ret == 1))
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            // ????????
            xx = (w - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            // ?????????
            for (i = 0; i < sizeof(shopWindow7Msg) / sizeof(shopWindow7Msg[0]); i++)
            {
                if (shopWindow7Msg[0] != '\0')
                {
                    StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow7Msg[i], 0);
                }
            }

            // ???
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            // 页??
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (strlen(userItem[j + i].name) == 0)
                    continue;
                color = itemColor[0];
                if (userItem[j + i].sealFlag)
                    color = itemColor[1];

                if ((pc.gold + userItem[j + i].price) > CHAR_getMaxHaveGold())
                    color = 7;
#ifdef _EVIL_KILL
                if (userItem[j + i].sealFlag == 2) // _EVIL_KILL     NEW
                    color = itemColor[0];
#endif
                StockFontBuffer(x + 34, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, userItem[j + i].name, 0);
#ifdef _ITEM_PILENUMS
                sprintf_s(tmsg, "x%d", userItem[j + i].num);
                StockFontBuffer(x + 34 + 250, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, tmsg, 0);
#endif
#ifdef _EVIL_KILL
                if (userItem[j + i].sealFlag == 2)
                { // _EVIL_KILL     NEW
                    sprintf_s(tmsg, "%8dＤ", userItem[j + i].price);
                    StockFontBuffer(x + 312, y + 118 + i * 21,
                                    FONT_PRIO_FRONT, color, tmsg, 0);
                }
                else // _EVIL_KILL     NEW
#endif
                    sprintf_s(tmsg, "%8dＳ", userItem[j + i].price);
                StockFontBuffer(x + 312, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, tmsg, 0);
            }
            if (focusId >= 0)
            {
                StockDispBuffer(x + 64, y + 350, DISP_PRIO_IME3, userItem[focusId].graNo, 0);
                for (i = 0; i < sizeof(userItem[0].info) / sizeof(userItem[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    userItem[focusId].info[i], 0);
                }
            }
            if (buyOffFlag == 0)
            {
                btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
            }
            else
            {
                btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
            }
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1);
        }
    }
    return ret;
}

short shopWindow8ProcNo;

void initShopWindow8(void)
{
    shopWindow8ProcNo = 0;
}

int shopWindow8(void)
{
    static int x, y, w, h;
    static int btnId[1];
    int id;
    int i;
    int ret = 0;

    if (shopWindow8ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 6;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        shopWindow8ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
                ret = 1;
            else if (id == 100)
                ret = 100;

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
                windowTypeWN = -1;
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < sizeof(shopWindow8Msg) / sizeof(shopWindow8Msg[0]); i++)
            {
                if (shopWindow8Msg[0] != '\0')
                {
                    StockFontBuffer(x + 20, y + 20 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow8Msg[i], 0);
                }
            }
            btnId[0] = StockDispBuffer(x + 192, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
        }
    }

    return ret;
}
#ifdef _ITEM_PILENUMS
short shopWindow10ProcNo;
void initShopWindow10(void)
{
    shopWindow10ProcNo = 0;
}

int shopWindow10(void)
{
    static int x, y, w, h;
    static int btnId[4];
    int upBtnGraNo[] = {CG_UP_BTN, CG_UP_BTN_DOWN};
    int downBtnGraNo[] = {CG_DOWN_BTN, CG_DOWN_BTN_DOWN};
    int upBtn, downBtn;
    int id;
    int pushId;
    int i;
    int ret = 0;
    char tmsg[256];

    if (shopWindow10ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 440;
        h = 160;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        shopWindow10ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
                id = 0;
        }
        if (pushId == 0)
        {
            downBtn = 1;
            if (id == 0)
            {
                if (sealItemCnt > 1)
                {
                    sealItemCnt--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            downBtn = 0;
        }
        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            upBtn = 1;
            if (id == 1)
            {
                if (sealItemCnt + 1 <= userItem[selShopItemNo].num && (userItem[selShopItemNo].price * (sealItemCnt + 1) + pc.gold) <= CHAR_getMaxHaveGold())
                {
                    sealItemCnt++;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            upBtn = 0;
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2)
        {
            if (id == 2)
            {
                ret = 1;
            }
            else if (id == 3)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }
        if (ptActMenuWin->hp >= 1)
        { // userItem
            int xx;
            xx = (w - getUtf8CharNum(shopWindow3Msg) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow3Msg, 0);
            StockFontBuffer(x + 16, y + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            userItem[selShopItemNo].name, 0);
            sprintf_s(tmsg, "%2d个", sealItemCnt);
            StockFontBuffer(x + 300, y + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
#ifdef _EVIL_KILL
            if (userItem[selShopItemNo].sealFlag == 2)
                sprintf_s(tmsg, "%8dＤ x %2d = %8dＤ",
                          userItem[selShopItemNo].price,
                          sealItemCnt, userItem[selShopItemNo].price * sealItemCnt);
            else
#endif
                sprintf_s(tmsg, "%8dＳ x %2d = %8dＳ",
                          userItem[selShopItemNo].price,
                          sealItemCnt, userItem[selShopItemNo].price * sealItemCnt);
            StockFontBuffer(x + 146, y + 82, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            tmsg, 0);
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 248, y + 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            btnId[0] = StockDispBuffer(x + 364, y + 60, DISP_PRIO_IME3, downBtnGraNo[downBtn], 2);
            btnId[1] = StockDispBuffer(x + 404, y + 60, DISP_PRIO_IME3, upBtnGraNo[upBtn], 2);

            btnId[2] = StockDispBuffer(x + 64, y + 120, DISP_PRIO_IME3, CG_OK_BTN, 2);
            btnId[3] = StockDispBuffer(x + 164, y + 120, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_KOSU_WIN, 1);
        }
    }

    return ret;
}
#endif

void initSkillShopWindow1(void);
int skillShopWindow1(void);
void initSkillShopWindow2(void);
int skillShopWindow2(void);
void initSkillShopWindow3(void);
int skillShopWindow3(void);
void initSkillShopWindow4(void);
int skillShopWindow4(void);

void serverWindowType6(void)
{
    int ret;
    if (skillShopWindowProcNo == 0)
    {
        initSkillShopWindow1();
        skillShopWindowProcNo++;
        play_se(202, 320, 240); // ????????
    }
    if (skillShopWindowProcNo == 1)
    {
        ret = skillShopWindow1();
        if (ret == 2)
            skillShopWindowProcNo = 10;
    }
    if (skillShopWindowProcNo == 10)
    {
        initSkillShopWindow2();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 11)
    {
        ret = skillShopWindow2();
        if (ret == 1)
            skillShopWindowProcNo = 0;
        else if (ret == 2)
            skillShopWindowProcNo = 20;
    }
    if (skillShopWindowProcNo == 20)
    {
        initSkillShopWindow3();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 21)
    {
        ret = skillShopWindow3();
        if (ret == 1)
            skillShopWindowProcNo = 10;
        else if (ret == 2)
            skillShopWindowProcNo = 30;
    }
    if (skillShopWindowProcNo == 30)
    {
        initSkillShopWindow4();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 31)
    {
        ret = skillShopWindow4();
        if (ret == 1)
        {
            sprintf_s(data, "%d|%d|%d|%d",
                      selShopSkillNo + 1,
                      selShopSkillPetNo,
                      selShopSkillSlotNo,
                      sealSkill[selShopSkillNo].price);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            pc.gold -= sealSkill[selShopSkillNo].price;
            windowTypeWN = -1;
        }
        else if (ret == 2)
        {
            skillShopWindowProcNo = 20;
        }
    }
}

#ifdef _CHAR_PROFESSION // WON ADD 人物职业技能

void initServerWindowProfession(char *data)
{
    char title1[128];
    char msg2[128];
    char skillName[64];
    char info[256];
    int i, j;
    int flag;

    skillShopWindowProcNo = 0;
    shopWondow2Page = 0;

    if (getIntegerToken(data, '|', 1) == 0)
        return;

    getStringToken(data, '|', 2, sizeof(title1) - 1, title1);
    makeStringFromEscaped(title1);
    getStringToken(data, '|', 3, sizeof(msg2) - 1, msg2);
    makeStringFromEscaped(msg2);

    strncpy_s(ProfessionShopTitle, title1, sizeof(ProfessionShopTitle) - 1);
    ProfessionShopTitle[sizeof(ProfessionShopTitle) - 1] = '\0';

    getStrSplit((char *)ProfessionShopMsg, msg2,
                sizeof(ProfessionShopMsg[0]),
                sizeof(ProfessionShopMsg) / sizeof(ProfessionShopMsg[0]),
                sizeof(ProfessionShopMsg[0]) - 1);

    // Robin fix 20040707 回圈过大
    // shopWondow2MaxPage = MAX_SKILL_SHOP_SKILL* 9;
    shopWondow2MaxPage = MAX_SKILL_SHOP_SKILL * MAX_SKILL_SHOP_PAGE;

    for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
    {
        // 技能名称
        flag = getStringToken(data, '|', 4 + i * 4, sizeof(skillName) - 1, skillName);
        makeStringFromEscaped(skillName);
        if (flag)
        {
            sealSkill[i].name[0] = '\0';
            continue;
        }

        j++;

        if (strlen(skillName) <= SKILL_NAME_LEN)
            strcpy(sealSkill[i].name, skillName);
        else
            strcpy(sealSkill[i].name, "???");

        // 金额
        sealSkill[i].price = getIntegerToken(data, '|', 5 + i * 4);

        // 说明
        getStringToken(data, '|', 6 + i * 4, sizeof(info) - 1, info);
        makeStringFromEscaped(info);
        getStrSplit((char *)(&sealSkill[i].info), info,
                    sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info[0]) - 1);

        // 图示
        sealSkill[i].icon = getIntegerToken(data, '|', 7 + i * 4);
    }
    if (j > 0)
        shopWondow2MaxPage = (j + 7) / MAX_SKILL_SHOP_SKILL;
    else
        shopWondow2MaxPage = 1;
}

void profession_windows(void)
{
    int ret;

    if (skillShopWindowProcNo == 0)
    {
        initSkillShopWindow1();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 1)
    {
        ret = profession_windows_1();
        if (ret == 2)
        {
            skillShopWindowProcNo = 10;
        }
    }
    if (skillShopWindowProcNo == 10)
    {
        initSkillShopWindow4();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 11)
    {
        ret = profession_windows_2();
        if (ret == 1)
        {
            sprintf_s(data, "%d|%d",
                      selShopSkillNo + 1,
                      sealSkill[selShopSkillNo].price);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            // Robin fix 20040707 修改Client自动扣钱bug
            // pc.gold -= sealSkill[selShopSkillNo].price;
            windowTypeWN = -1;
        }
        else if (ret == 2)
        {
            skillShopWindowProcNo = 0;
        }
    }
}

int profession_windows_1(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (skillShopWindow1ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        skillShopWindow1ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (sealSkill[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (sealSkill[j + i].price <= pc.gold)
                            {
                                selId = j + i;
                            }
                            else
                            {
                                play_se(220, 320, 240);
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            prevBtn = 0;
        }

        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            nextBtn = 0;
        }

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2 || selId >= 0)
        {
            if (id == 2)
            {
                ret = 1;
                play_se(203, 320, 240);
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillNo = selId;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            xx = (w - getUtf8CharNum(ProfessionShopTitle) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, ProfessionShopTitle, 0);

            for (i = 0; i < sizeof(ProfessionShopMsg) / sizeof(ProfessionShopMsg[0]); i++)
            {
                if (ProfessionShopMsg[0] != '\0')
                {
                    StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    ProfessionShopMsg[i], 0);
                }
            }

            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (strlen(sealSkill[j + i].name) == 0)
                    continue;

                color = itemColor[0];

                if (sealSkill[j + i].price > pc.gold)
                {
                    color = itemColor[1];
                }

                StockFontBuffer(x + 34, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, sealSkill[j + i].name, 0);

                sprintf_s(tmsg, "%8dＳ", sealSkill[j + i].price);
                StockFontBuffer(x + 312, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, tmsg, 0);
            }

            if (focusId >= 0)
            {
                // 秀ICON
                StockDispBuffer(x + 58, y + 340, DISP_PRIO_IME3, sealSkill[focusId].icon, 0);
                for (i = 0; i < sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    sealSkill[focusId].info[i], 0);
                }
            }
            btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_SKILLSHOP_WIN, 1);
        }
    }
    return ret;
}

int profession_windows_2(void)
{
    static int x, y, w, h;
    static int btnId[2];
    static char msg1[128];
    int id;
    int i;
    int ret = 0;

    if (skillShopWindow4ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 8;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);

        char name[32];
        if (strlen(pc.freeName) > 0)
            strcpy(name, pc.freeName);
        else
            strcpy(name, pc.name);

        sprintf_s(msg1, "让[%s]将[%s]", name,
                  sealSkill[selShopSkillNo].name);
        skillShopWindow4ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            // ??????
            StockFontBuffer(x + 20, y + 20 + 0 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            msg1, 0);
            StockFontBuffer(x + 20, y + 20 + 1 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "学起来吗？", 0);

            btnId[0] = StockDispBuffer(x + w * 64 / 3, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + w * 64 / 3 * 2, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}

#endif

#ifdef _NPC_WELFARE_2 // WON ADD 职业NPC-2
void initServerWindowProfession2(char *data)
{
    char title1[128];
    char msg2[128];
    char skillName[64];
    char info[256];
    int i, j;
    int flag;

    skillShopWindowProcNo = 0;
    shopWondow2Page = 0;

    if (getIntegerToken(data, '|', 1) == 0)
        return;

    getStringToken(data, '|', 2, sizeof(title1) - 1, title1);
    makeStringFromEscaped(title1);
    getStringToken(data, '|', 3, sizeof(msg2) - 1, msg2);
    makeStringFromEscaped(msg2);

    strncpy_s(ProfessionShopTitle2, title1, sizeof(ProfessionShopTitle2) - 1);
    ProfessionShopTitle2[sizeof(ProfessionShopTitle2) - 1] = '\0';

    getStrSplit((char *)ProfessionShopMsg2, msg2,
                sizeof(ProfessionShopMsg2[0]),
                sizeof(ProfessionShopMsg2) / sizeof(ProfessionShopMsg2[0]),
                sizeof(ProfessionShopMsg2[0]) - 1);

    shopWondow2MaxPage = MAX_SKILL_SHOP_SKILL * 9;

    for (i = 0, j = 0; i < shopWondow2MaxPage; i++)
    {
        // 技能名称
        flag = getStringToken(data, '|', 4 + i * 4, sizeof(skillName) - 1, skillName);
        makeStringFromEscaped(skillName);
        if (flag)
        {
            sealSkill[i].name[0] = '\0';
            continue;
        }

        j++;

        if (strlen(skillName) <= SKILL_NAME_LEN)
            strcpy(sealSkill[i].name, skillName);
        else
            strcpy(sealSkill[i].name, "???");

        // 金额
        sealSkill[i].price = getIntegerToken(data, '|', 5 + i * 4);

        // 说明
        getStringToken(data, '|', 6 + i * 4, sizeof(info) - 1, info);
        makeStringFromEscaped(info);
        getStrSplit((char *)(&sealSkill[i].info), info,
                    sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]),
                    sizeof(sealSkill[0].info[0]) - 1);

        // 图示
        sealSkill[i].icon = getIntegerToken(data, '|', 7 + i * 4);
    }
    if (j > 0)
    {
        shopWondow2MaxPage = (j + 7) / MAX_SKILL_SHOP_SKILL;
    }
    else
    {
        shopWondow2MaxPage = 1;
    }
}

void profession_windows2(void)
{
    int ret;

    if (skillShopWindowProcNo == 0)
    {
        initSkillShopWindow1();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 1)
    {
        ret = profession_windows_12();
        if (ret == 2)
        {
            skillShopWindowProcNo = 10;
        }
    }
    if (skillShopWindowProcNo == 10)
    {
        initSkillShopWindow4();
        skillShopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (skillShopWindowProcNo == 11)
    {
        ret = profession_windows_22();
        if (ret == 1)
        {
            sprintf_s(data, "%d|%d",
                      selShopSkillNo + 1,
                      sealSkill[selShopSkillNo].price);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            pc.gold -= sealSkill[selShopSkillNo].price;
            windowTypeWN = -1;
        }
        else if (ret == 2)
        {
            skillShopWindowProcNo = 0;
        }
    }
}

int profession_windows_12(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (skillShopWindow1ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        skillShopWindow1ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (sealSkill[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (sealSkill[j + i].price <= pc.gold)
                            {
                                selId = j + i;
                            }
                            else
                            {
                                play_se(220, 320, 240);
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            prevBtn = 0;
        }

        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            nextBtn = 0;
        }

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2 || selId >= 0)
        {
            if (id == 2)
            {
                ret = 1;
                play_se(203, 320, 240);
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillNo = selId;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            xx = (w - getUtf8CharNum(ProfessionShopTitle2) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, ProfessionShopTitle2, 0);

            for (i = 0; i < sizeof(ProfessionShopMsg2) / sizeof(ProfessionShopMsg2[0]); i++)
            {
                if (ProfessionShopMsg2[0] != '\0')
                {
                    StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    ProfessionShopMsg2[i], 0);
                }
            }

            sprintf_s(tmsg, "职业：%s  点数：%3d", pc.profession_class_name, pc.profession_skill_point);
            StockFontBuffer(x + 235, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            // 页??
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (strlen(sealSkill[j + i].name) == 0)
                    continue;

                color = itemColor[0];

                if (sealSkill[j + i].price > pc.gold)
                {
                    color = itemColor[1];
                }

                StockFontBuffer(x + 34, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, sealSkill[j + i].name, 0);

                sprintf_s(tmsg, "熟练度：%5d", sealSkill[j + i].price);
                StockFontBuffer(x + 280, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, tmsg, 0);
            }

            if (focusId >= 0)
            {
                // 秀ICON
                StockDispBuffer(x + 58, y + 340, DISP_PRIO_IME3, sealSkill[focusId].icon, 0);

                for (i = 0; i < sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    sealSkill[focusId].info[i], 0);
                }
            }

            btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_EXIT_BTN, 2);

            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_SKILLSHOP_WIN, 1);
        }
    }

    return ret;
}

int profession_windows_22(void)
{
    static int x, y, w, h;
    static int btnId[2];
    static char msg1[128];
    int id;
    int i;
    int ret = 0;

    if (skillShopWindow4ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        // ??????
        w = 8;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1, FALSE);

        char name[32];
        if (strlen(pc.freeName) > 0)
            strcpy(name, pc.freeName);
        else
            strcpy(name, pc.name);
        sprintf_s(msg1, "要[%s]将[%s]", name,
                  sealSkill[selShopSkillNo].name);
        skillShopWindow4ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            // ????
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        }

        // ?????????????????????
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            // ??????
            StockFontBuffer(x + 20, y + 20 + 0 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            msg1, 0);
            StockFontBuffer(x + 20, y + 20 + 1 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "遗忘吗？", 0);

            btnId[0] = StockDispBuffer(x + w * 64 / 3, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + w * 64 / 3 * 2, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}

#endif

#ifdef _PETSKILL_CANNEDFOOD
void PetSkillShowType1(void)
{
    static int x, y, w, h;
    static int btnId[1];
    int id = -1;
    int i;
    int ret = 0;
    int x1, y1, x2, y2;
    char skillName[128];

    if (PetskillShpwWindowProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        w = 5;
        h = 6;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        PetskillShpwWindowProcNo++;
    }
    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            if ((id = selGraId(btnId, sizeof(btnId) / sizeof(int))) < 0)
            {
                for (i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo].maxSkill; i++)
                {
                    x1 = x + 41;
                    y1 = y + 33 + (i + 1) * 26;
                    x2 = x1 + 238;
                    y2 = y1 + 22;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id = i;
                        }
                    }
                }
            }
            else
            {
                id = 100;
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillSlotNo = id;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            else if (ret == 2)
            {
                sprintf_s(data, "%d",
                          selShopSkillSlotNo
                );
                makeEscapeString(data, msg, sizeof(msg) - 1);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
                pc.gold -= sealSkill[selShopSkillNo].price;
                windowTypeWN = -1;
            }
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(x + 75, y + 33,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "要在那个位置做呢？", 0);
            for (i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo].maxSkill; i++)
            {
                // /utf-8 keeps this prefix and the server skill name in one encoding.
                sprintf_s(skillName, "技 %d: ", i + 1);
                if (petSkill[selShopSkillPetNo][i].useFlag != 0)
                {
                    strcat_s(skillName, petSkill[selShopSkillPetNo][i].name);
                }
                StockFontBuffer(x + 46, y + 36 + (i + 1) * 26,
                                FONT_PRIO_FRONT, FONT_PAL_WHITE, skillName, 0);
            }
            btnId[0] = StockDispBuffer(x + 160, y + 256, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);
        }
    }
    return;
}
#endif

void initSkillShopWindow1(void)
{
    skillShopWindow1ProcNo = 0;
}

int skillShopWindow1(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (skillShopWindow1ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        skillShopWindow1ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (sealSkill[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (sealSkill[j + i].price <= pc.gold
#ifdef _PETKILL_COST_FAME
                                && sealSkill[j + i].iCoseFame <= pc.fame
#endif
                            )
                            {
                                selId = j + i;
                            }
                            else
                            {
                                play_se(220, 320, 240);
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            prevBtn = 0;
        }

        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240);
                }
            }
        }
        else
        {
            nextBtn = 0;
        }

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 2 || selId >= 0)
        {
            if (id == 2)
            {
                ret = 1;
                play_se(203, 320, 240);
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillNo = selId;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            xx = (w - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            for (i = 0; i < sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]); i++)
            {
                if (shopWindow2Msg[0] != '\0')
                {
                    StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow2Msg[i], 0);
                }
            }
#ifdef _PETKILL_COST_FAME
            sprintf_s(tmsg, "声望 %8dＦ", pc.fame);
            StockFontBuffer(x + 244, y + 64, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
#endif
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            j = shopWondow2Page * MAX_SKILL_SHOP_SKILL;
            for (i = 0; i < MAX_SKILL_SHOP_SKILL; i++)
            {
                if (strlen(sealSkill[j + i].name) == 0)
                    continue;

                color = itemColor[0];

                if (sealSkill[j + i].price > pc.gold)
                    color = itemColor[1];
#ifdef _PETKILL_COST_FAME
                if (sealSkill[j + i].iCoseFame > pc.fame)
                    color = itemColor[1];
#endif

                StockFontBuffer(x + 34, y + 118 + i * 21, FONT_PRIO_FRONT, color, sealSkill[j + i].name, 0);

#ifdef _PETKILL_COST_FAME
                sprintf_s(tmsg, "%8dＳ", sealSkill[j + i].price);
                StockFontBuffer(x + 212, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
                sprintf_s(tmsg, "%8dＦ", sealSkill[j + i].iCoseFame);
                StockFontBuffer(x + 312, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#else
                sprintf_s(tmsg, "%8dＳ", sealSkill[j + i].price);
                StockFontBuffer(x + 312, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#endif
            }

            if (focusId >= 0)
            {
                for (i = 0; i < sizeof(sealSkill[0].info) / sizeof(sealSkill[0].info[0]); i++)
                {
                    StockFontBuffer(x + 27, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    sealSkill[focusId].info[i], 0);
                }
            }

            btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_EXIT_BTN, 2);

            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_SKILLSHOP_WIN, 1);
        }
    }

    return ret;
}

short skillShopWindow2ProcNo;

void initSkillShopWindow2(void)
{
    skillShopWindow2ProcNo = 0;
}

int skillShopWindow2(void)
{
    static int x, y, w, h;
    static int btnId[1];
    int id;
    int i, j;
    int ret = 0;
    int x1, y1, x2, y2;
    int len;

    if (skillShopWindow2ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        w = 7;
        h = 5;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        skillShopWindow2ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            // ????
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));

            for (i = 0, j = 0; i < MAX_PET; i++)
            {
                // ????????????
                if (pet[i].useFlag != 0)
                {
                    x1 = x + 41;
                    y1 = y + 25 + (j + 1) * 26;
                    x2 = x1 + 366;
                    y2 = y1 + 22;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            id = i + 1;
                        }
                    }
                    j++;
                }
            }
        }

        // ?????????????????????
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            // ?????
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillPetNo = id;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(x + 145, y + 25,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "要让谁学呢？", 0);
            for (i = 0, j = 0; i < MAX_PET; i++)
            {
                if (pet[i].useFlag != 0)
                {
                    len = getUtf8CharNum(pet[i].freeName);
                    if (len > 0)
                    {
                        strcpy(msgWN[1], pet[i].freeName);
                    }
                    else
                    {
                        len = getUtf8CharNum(pet[i].name);
                        strcpy(msgWN[1], pet[i].name);
                    }
                    StockFontBuffer(x + 62, y + 28 + (j + 1) * 26,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    sprintf_s(msgWN[1], "LV.%d", pet[i].level);
                    StockFontBuffer(x + 260, y + 28 + (j + 1) * 26,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    sprintf_s(msgWN[1], "MaxHP %d", pet[i].maxHp);
                    StockFontBuffer(x + 314, y + 28 + (j + 1) * 26,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0);

                    j++;
                }
            }
            btnId[0] = StockDispBuffer(x + w * 64 / 2, y + 204, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
        }
    }

    return ret;
}

short skillShopWindow3ProcNo;

void initSkillShopWindow3(void)
{
    skillShopWindow3ProcNo = 0;
}

// Pet Skill Window 3: 指定在哪个地方做宠物?
int skillShopWindow3(void)
{
    static int x, y, w, h;
    static int btnId[1];
    int id;
    int i;
    int ret = 0;
    int x1, y1, x2, y2;
    char skillName[128];

    if (skillShopWindow3ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        w = 5;
        h = 6;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        skillShopWindow3ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
            for (i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo - 1].maxSkill; i++)
            {
                x1 = x + 41;
                y1 = y + 33 + (i + 1) * 26;
                x2 = x1 + 238;
                y2 = y1 + 22;
                if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                {
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        id = i + 1;
                    }
                }
            }
        }

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopSkillSlotNo = id;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(x + 75, y + 33,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "要在那个位置做呢？", 0);
            for (i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo - 1].maxSkill; i++)
            {
                // /utf-8 keeps this prefix and the server skill name in one encoding.
                sprintf_s(skillName, "技 %d: ", i + 1);
                if (petSkill[selShopSkillPetNo - 1][i].useFlag != 0)
                {
                    strcat_s(skillName, petSkill[selShopSkillPetNo - 1][i].name);
                }
                StockFontBuffer(x + 46, y + 36 + (i + 1) * 26,
                                FONT_PRIO_FRONT, FONT_PAL_WHITE, skillName, 0);
            }
            btnId[0] = StockDispBuffer(x + 160, y + 256, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
        }
    }

    return ret;
}

void initSkillShopWindow4(void)
{
    skillShopWindow4ProcNo = 0;
}

int skillShopWindow4(void)
{
    static int x, y, w, h;
    static int btnId[2];
    static char msg1[128];
    int id;
    int i;
    int ret = 0;

    if (skillShopWindow4ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 8;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        char name[32];
        if (strlen(pet[selShopSkillPetNo - 1].freeName) > 0)
            strcpy(name, pet[selShopSkillPetNo - 1].freeName);
        else
            strcpy(name, pet[selShopSkillPetNo - 1].name);
        sprintf_s(msg1, "让[%s]将[%s]", name,
                  sealSkill[selShopSkillNo].name);
        skillShopWindow4ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(x + 20, y + 20 + 0 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            msg1, 0);
            StockFontBuffer(x + 20, y + 20 + 1 * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "学起来吗？", 0);
            btnId[0] = StockDispBuffer(x + w * 64 / 3, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + w * 64 / 3 * 2, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}

#if 1

void initPoolShopWindow1(void);
int poolShopWindow1(void);
void initPoolShopWindow2(void);
int poolShopWindow2(void);
void initPoolShopWindow3(void);
int poolShopWindow3(void);

void initPoolShopWindow4(void);
int poolShopWindow4(void);

void serverWindowType7(void)
{
    int ret;
    if (shopWindowProcNo == 0)
    {
        initPoolShopWindow1();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 1)
    {
        ret = poolShopWindow1();
    }
    if (shopWindowProcNo == 10)
    {
        initPoolShopWindow2();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 11)
    {
        ret = poolShopWindow2();
        if (ret == 1)
        {
            sprintf_s(data, "0");
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            windowTypeWN = -1;
        }
        else if (ret == 2)
            shopWindowProcNo = 20;
    }
    if (shopWindowProcNo == 20)
    {
        initPoolShopWindow3();
        shopWindowProcNo++;
        play_se(202, 320, 240); // ????????
    }
    if (shopWindowProcNo == 21)
    {
        ret = poolShopWindow3();
        if (ret == 1)
        {
            sprintf_s(data, "%d", poolItem[selShopItemNo].tbl);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            pc.gold -= poolItem[selShopItemNo].price;
            restPoolSlot--;
            if (restPoolSlot < 0)
                restPoolSlot = 0;

            poolItem[selShopItemNo].name[0] = '\0';
            poolItem[selShopItemNo].info[0][0] = '\0';
            poolItem[selShopItemNo].info[1][0] = '\0';
            poolItem[selShopItemNo].info[2][0] = '\0';
#ifdef _ITEM_PILENUMS
            poolItem[selShopItemNo].num = 0;
#endif
            int i, j, k;
            for (i = 0; i < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE - 1; i++)
            {
                if (poolItem[i].name[0] == '\0')
                {
                    for (j = i + 1; j < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE; j++)
                    {
                        if (poolItem[j].name[0] != '\0')
                        {
                            strcpy(poolItem[i].name, poolItem[j].name);
                            poolItem[i].poolFlag = poolItem[j].poolFlag;
                            poolItem[i].level = poolItem[j].level;
                            poolItem[i].price = poolItem[j].price;
                            poolItem[i].graNo = poolItem[j].graNo;
                            poolItem[i].tbl = poolItem[j].tbl;
#ifdef _ITEM_PILENUMS
                            poolItem[i].num = poolItem[j].num;
#endif
                            for (k = 0; k < 3; k++)
                                strcpy(poolItem[i].info[k], poolItem[j].info[k]);
                            poolItem[j].name[0] = '\0';
                            break;
                        }
                    }
                }
            }
            for (i = 0; i < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE; i++)
                if (poolItem[i].name[0] == '\0')
                    break;
            if (i > 0)
                shopWondow2MaxPage = (i + 7) / MAX_POOL_SHOP_ITEM;
            else
                shopWondow2MaxPage = 1;
            if (shopWondow2Page >= shopWondow2MaxPage)
                shopWondow2Page = shopWondow2MaxPage - 1;
            shopWindowProcNo = 10;
        }
        else if (ret == 2)
            shopWindowProcNo = 10;
    }

    if (shopWindowProcNo == 100)
    {
        initPoolShopWindow4();
        shopWindowProcNo++;
        play_se(202, 320, 240); // ????????
    }
    if (shopWindowProcNo == 101)
    {
        ret = poolShopWindow4();
        if (ret == 1)
        {
            sprintf_s(data, "0");
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            windowTypeWN = -1;
        }
        else if (ret == 2)
            shopWindowProcNo = 110;
        else if (ret == 3)
            shopWindowProcNo = 120;
    }

    if (shopWindowProcNo == 110)
    {
        initPoolShopWindow3();
        shopWindowProcNo++;
        play_se(202, 320, 240);
    }
    if (shopWindowProcNo == 111)
    {
        ret = poolShopWindow3();
        if (ret == 1)
        {
            sprintf_s(data, "%d", selShopItemNo + 1);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
            poolItem[selShopItemNo].name[0] = '\0';
            poolItem[selShopItemNo].info[0][0] = '\0';
            poolItem[selShopItemNo].info[1][0] = '\0';
            poolItem[selShopItemNo].info[2][0] = '\0';
#ifdef _ITEM_PILENUMS
            poolItem[selShopItemNo].num = 0;
#endif
            int i, j, k;
            for (i = 0; i < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE - 1; i++)
            {
                if (poolItem[i].name[0] == '\0')
                {
                    for (j = i + 1; j < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE; j++)
                    {
                        if (poolItem[j].name[0] != '\0')
                        {
                            strcpy(poolItem[i].name, poolItem[j].name);
                            poolItem[i].poolFlag = poolItem[j].poolFlag;
                            poolItem[i].level = poolItem[j].level;
                            poolItem[i].price = poolItem[j].price;
                            poolItem[i].graNo = poolItem[j].graNo;
                            poolItem[i].tbl = poolItem[j].tbl;
#ifdef _ITEM_PILENUMS
                            poolItem[i].num = poolItem[j].num;
#endif
                            for (k = 0; k < 3; k++)
                            {
                                strcpy(poolItem[i].info[k], poolItem[j].info[k]);
                            }
                            poolItem[j].name[0] = '\0';
                            break;
                        }
                    }
                }
            }
            for (i = 0; i < MAX_POOL_SHOP_ITEM * MAX_POOL_SHOP_PAGE; i++)
            {
                if (poolItem[i].name[0] == '\0')
                    break;
            }
            if (i > 0)
            {
                shopWondow2MaxPage = (i + 7) / MAX_POOL_SHOP_ITEM;
            }
            else
            {
                shopWondow2MaxPage = 1;
            }
            if (shopWondow2Page >= shopWondow2MaxPage)
                shopWondow2Page = shopWondow2MaxPage - 1;

            shopWindowProcNo = 100;
        }
        else if (ret == 2)
        {
            shopWindowProcNo = 100;
        }
    }
}


// POOL SHOP 是啥？
short poolShopWindow1ProcNo;

void initPoolShopWindow1(void)
{
    poolShopWindow1ProcNo = 0;
}

int poolShopWindow1(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id = 0;
    int i;
    int ret = 0;

    if (poolShopWindow1ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 4;
        h = 3;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        poolShopWindow1ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
            ret = id + 1;
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            ret = 3; // ??
            wnCloseFlag = 0;
        }

        if (0 <= id && id <= 2)
        {
            sprintf_s(data, "%d", ret);
            makeEscapeString(data, msg, sizeof(msg) - 1);
            lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
        }

        if (id >= 0)
        {
            if (ret == 3)
            {
                play_se(203, 320, 240); // ????????
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            windowTypeWN = -1;
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            xx = (w * 64 - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 12, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            xx = (w * 64 - getUtf8CharNum(shopWindow1Msg) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 32, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Msg, 0);

            btnId[0] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 66, DISP_PRIO_IME3, CG_SEAL_BTN, 2);
            btnId[1] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 90, DISP_PRIO_IME3, CG_BUY_BTN, 2);
            btnId[2] =
                StockDispBuffer(x + w * 64 / 2, y + 16 / 2 + 114, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
        }
    }

    return ret;
}
short poolShopWindow2ProcNo;

void initPoolShopWindow2(void)
{
    poolShopWindow2ProcNo = 0;
}

int poolShopWindow2(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;

    if (poolShopWindow2ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        poolShopWindow2ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_POOL_SHOP_ITEM; i++)
            {
                if (poolItem[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            // ?????????????
                            if (poolItem[j + i].price <= pc.gold && poolItem[j + i].poolFlag == 0 && restPoolSlot > 0)
                            {
                                selId = j + i;
                            }
                            else
                            {
                                play_se(220, 320, 240); // ???
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        // ??????
        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
            prevBtn = 0;

        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
                id = 1;
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
            nextBtn = 0;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        // id = 0, id = 1 ??????????????
        if (id >= 2 || selId >= 0)
        {
            // ?????
            if (id == 2)
            {
                ret = 1;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            else
            {
                selShopItemNo = selId;
                ret = 2;
            }

            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx;

            // ????????
            xx = (w - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            // ?????????
            if (restPoolSlot > 0)
            {
                for (i = 0; i < sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]); i++)
                {
                    if (shopWindow2Msg[i][0] != '\0')
                    {
                        StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                        shopWindow2Msg[i], 0);
                    }
                }
            }
            else
            {
                for (i = 0; i < sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]); i++)
                {
                    if (shopWindow4Msg[i][0] != '\0')
                    {
                        StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                        shopWindow4Msg[i], 0);
                    }
                }
            }
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);
            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (strlen(poolItem[j + i].name) == 0)
                    continue;
                color = itemColor[0];
                if (poolItem[j + i].poolFlag)
                    color = itemColor[1];
                if (poolItem[j + i].price > pc.gold)
                    color = itemColor[1];
                if (restPoolSlot <= 0)
                    color = itemColor[1];
                StockFontBuffer(x + 34, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, poolItem[j + i].name, 0);
#ifdef _ITEM_PILENUMS
                sprintf_s(tmsg, "x%d", poolItem[j + i].num);
                StockFontBuffer(x + 34 + 250, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#endif
                // ??
                sprintf_s(tmsg, "%8dＳ", poolItem[j + i].price);
                StockFontBuffer(x + 312, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, tmsg, 0);
            }

            if (focusId >= 0)
            {
                StockDispBuffer(x + 64, y + 350, DISP_PRIO_IME3, poolItem[focusId].graNo, 0);
                for (i = 0; i < sizeof(sealItem[0].info) / sizeof(poolItem[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    poolItem[focusId].info[i], 0);
                }
            }

            btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1);
        }
    }

    return ret;
}

short poolShopWindow3ProcNo;

void initPoolShopWindow3(void)
{
    poolShopWindow3ProcNo = 0;
}

int poolShopWindow3(void)
{
    static int x, y, w, h;
    static int btnId[2];
    int id;
    int i;
    int ret = 0;

    if (poolShopWindow3ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
            btnId[i] = -2;
        w = 6;
        h = 2;
        x = (lpDraw->xSize - w * 64) / 2;
        y = (lpDraw->ySize - h * 48) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        poolShopWindow3ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        if (ptActMenuWin->hp >= 1)
        {
            id = selGraId(btnId, sizeof(btnId) / sizeof(int));
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        if (id >= 0)
        {
            if (id == 0)
            {
                ret = 1;
            }
            else if (id == 1)
            {
                ret = 2;
            }
            else if (id == 100)
            {
                ret = 100;
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100)
            {
                windowTypeWN = -1;
            }
            return ret;
        }
        if (ptActMenuWin->hp >= 1)
        {
            for (i = 0; i < sizeof(shopWindow5Msg) / sizeof(shopWindow5Msg[0]); i++)
            {
                if (shopWindow5Msg[i][0] != '\0')
                {
                    StockFontBuffer(x + 20, y + 20 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    shopWindow5Msg[i], 0);
                }
            }

            btnId[0] = StockDispBuffer(x + 128, y + 72, DISP_PRIO_IME3, CG_YES_BTN, 2);
            btnId[1] = StockDispBuffer(x + 256, y + 72, DISP_PRIO_IME3, CG_NO_BTN, 2);
        }
    }

    return ret;
}

short poolShopWindow4ProcNo;

// ???
void initPoolShopWindow4(void)
{
    poolShopWindow4ProcNo = 0;
}

int poolShopWindow4(void)
{
    static int x, y, w, h;
    static int btnId[3];
    int id;
    int pushId;
    int focusId;
    int selId;
    int i, j;
    int ret = 0;
    char tmsg[256];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};
    int color;
    int prevBtn, nextBtn;
    int x1, y1, x2, y2;
    short itemMaxFlag;

    if (poolShopWindow4ProcNo == 0)
    {
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        // ??????
        w = 432;
        h = 428;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        poolShopWindow4ProcNo++;
    }

    if (ptActMenuWin != NULL)
    {
        id = -1;
        pushId = -1;
        focusId = -1;
        selId = -1;

        if (ptActMenuWin->hp >= 1)
        {
            // ??????????????
            for (i = MAX_ITEMSTART, j = 0; i < MAX_ITEM; i++)
            {
                if (pc.item[i].useFlag)
                {
                    j++;
                }
            }
            itemMaxFlag = 0;
            if (j >= MAX_ITEM - MAX_ITEMSTART)
                itemMaxFlag = 1;

            // ????
            // id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
            id = selRepGraId(btnId, sizeof(btnId) / sizeof(int));
            pushId = pushGraId(btnId, sizeof(btnId) / sizeof(int));

            j = shopWondow2Page * MAX_POOL_SHOP_ITEM;
            for (i = 0; i < MAX_POOL_SHOP_ITEM; i++)
            {
                if (poolItem[j + i].name[0] != '\0')
                {
                    x1 = x + 32;
                    y1 = y + i * 21 + 116;
                    x2 = x1 + 372;
                    y2 = y1 + 20;
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (poolItem[j + i].poolFlag == 0 && !itemMaxFlag)
                            {
                                selId = j + i;
                            }
                            else
                            {
                                play_se(220, 320, 240); // ???
                            }
                        }
                        focusId = j + i;
                    }
                }
            }
        }

        // ??????
        if (joy_con[0] & JOY_A)
        {
            pushId = 0;
            if (joy_auto[0] & JOY_A)
            {
                id = 0;
            }
        }
        if (pushId == 0)
        {
            prevBtn = 1;
            if (id == 0)
            {
                if (shopWondow2Page > 0)
                {
                    shopWondow2Page--;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
        {
            prevBtn = 0;
        }
        if (joy_con[0] & JOY_B)
        {
            pushId = 1;
            if (joy_auto[0] & JOY_B)
            {
                id = 1;
            }
        }
        if (pushId == 1)
        {
            nextBtn = 1;
            if (id == 1)
            {
                if (shopWondow2Page + 1 < shopWondow2MaxPage)
                {
                    shopWondow2Page++;
                    play_se(217, 320, 240); // ?????
                }
            }
        }
        else
            nextBtn = 0;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            wnCloseFlag = 0;
        }

        // id = 0, id = 1 ??????????????
        if (id >= 2 || selId >= 0)
        {
            if (id == 2)
                ret = 1;
            else if (id == 100)
                ret = 100;
            else
            {
                selShopItemNo = selId;
                ret = 2;
            }
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            if (ret == 100 || ret == 1)
                windowTypeWN = -1;
            return ret;
        }

        if (ptActMenuWin->hp >= 1)
        {
            int xx = (w - getUtf8CharNum(shopWindow1Title) / 2 * 17) / 2;
            StockFontBuffer(x + xx, y + 18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0);

            if (!itemMaxFlag)
            {
                for (i = 0; i < sizeof(shopWindow2Msg) / sizeof(shopWindow2Msg[0]); i++)
                {
                    if (shopWindow2Msg[0] != '\0')
                    {
                        StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                        shopWindow2Msg[i], 0);
                    }
                }
            }
            else
            {
                for (i = 0; i < sizeof(shopWindow4Msg) / sizeof(shopWindow4Msg[0]); i++)
                {
                    if (shopWindow4Msg[0] != '\0')
                    {
                        StockFontBuffer(x + 18, y + 48 + i * 18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                        shopWindow4Msg[i], 0);
                    }
                }
            }

            // ???
            sprintf_s(tmsg, "金钱 %8dＳ", pc.gold);
            StockFontBuffer(x + 244, y + 84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            // 页??
            btnId[0] = StockDispBuffer(x + 32, y + 100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[1] = StockDispBuffer(x + 200, y + 100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
            sprintf_s(tmsg, "%2d/%2d 页", shopWondow2Page + 1, shopWondow2MaxPage);
            StockFontBuffer(x + 66, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0);

            j = shopWondow2Page * MAX_SHOP_ITEM;
            for (i = 0; i < MAX_SHOP_ITEM; i++)
            {
                if (strlen(poolItem[j + i].name) == 0)
                    continue;

                color = itemColor[0];
                if (poolItem[j + i].poolFlag)
                    color = itemColor[1];
                if (itemMaxFlag)
                    color = itemColor[1];
                StockFontBuffer(x + 34, y + 118 + i * 21,
                                FONT_PRIO_FRONT, color, poolItem[j + i].name, 0);

#ifdef _ITEM_PILENUMS
                char tmsg[256];
                sprintf_s(tmsg, "x%d", poolItem[j + i].num);
                StockFontBuffer(x + 34 + 250, y + 118 + i * 21, FONT_PRIO_FRONT, color, tmsg, 0);
#endif
            }

            if (focusId >= 0)
            {
                StockDispBuffer(x + 64, y + 350, DISP_PRIO_IME3, poolItem[focusId].graNo, 0);

                for (i = 0; i < sizeof(poolItem[0].info) / sizeof(poolItem[0].info[0]); i++)
                {
                    StockFontBuffer(x + 120, y + 308 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                    poolItem[focusId].info[i], 0);
                }
            }

            if (buyOffFlag == 0)
            {
                btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
            }
            else
            {
                btnId[2] = StockDispBuffer(x + 216, y + 402, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
            }
            StockDispBuffer(x + w / 2, y + h / 2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1);
        }
    }

    return ret;
}

#endif

// ??????
int produce_logout()
{
    switch (produce_vct_no)
    {
    case 0:
        // ????????????
        CopyBackBuffer();
        // ??????????
        BackBufferDrawType = DRAW_BACK_PRODUCE;
        // ???????????
        fade_out_bgm();
        // ????????
        NowTime = TimeGetTime();
        produce_vct_no++;
        // ???????
        InitMenu();
        break;
    case 1:
        // ??
        if (DrawProduce(PRODUCE_BRAN_SMALL) == TRUE)
        {
            BackBufferDrawType = DRAW_BACK_NORMAL;
            // ????????????
            PaletteChange(DEF_PAL, 0);
            return 1; //??
        }
        //        MenuProc();    // ??????
        //        ImeProc();    // ???????
        break;
    }
    return 0; //??
}

void StockDispHLine(int x, int y, int width)
{
    int i = 0;
    for (i = 0; i < width; i++)
        StockDispBuffer(x + 32 + (i * 64), y + 24, DISP_PRIO_IME3, 26022, 0);
}

void StockDispVLine(int x, int y, int height)
{
    int i = 0;
    for (i = 0; i < height; i++)
        StockDispBuffer(x + 32, y + 24 + (i * 48), DISP_PRIO_IME3, 26024, 0);
}

// Robin 0521 Family Add
static ACTION *pActPet10;
static int familyPetIndex = 0;
static char familyName[STR_BUFFER_SIZE];
int familyElf = 2;
STR_BUFFER familyNameInput;
STR_BUFFER familyRuleInput;
STR_BUFFER familyRuleShow;

static char familySpriteName[3][32] = {
    "光明精灵",
    "黑暗精灵",
    "        ",
};

/*
static char StrYesNo[2][10] = {
"Ｎ　Ｏ",
"ＹＥＳ",
};
*/
static int joinflg = 1;

// Robin 0518 Family
void initFamilyAddWN(char *data)
{
    makeStringFromEscaped(data);
    DeathAction(pActPet10);
    pActPet10 = NULL;
    familyElf = 2;
}

void closeFamilyAddWN(void)
{
    play_se(203, 320, 240);
    DeathAction(ptActMenuWin);
    DeathAction(pActPet10);
    pActPet10 = NULL;
    ptActMenuWin = NULL;
    windowTypeWN = -1;
    GetKeyInputFocus(&MyChatBuffer);
}

#ifdef _FAMILYBADGE_
int 徽章数据[200];
int 徽章个数;
#endif

void familyAddWN(void)
{
    static int x = 0, y = 0, w = 0, h = 0;
    static int btnId[5];
    static int fontId[6];
    static int prevBtn = 0, nextBtn = 0;
    static int Inputforce = 0;
#ifdef _FAMILYBADGE_
    static int 徽章窗口;
    static int 徽章索引;
    int 徽章总页数;
    static int 徽章当前页;
#endif
    char buf[1024], buf2[1024], buf3[1024];

    int selBtnId;
    int selFontBtnId;
    int i;
    int ret = 0;
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int itemColor[] = {FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED};

    if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        closeFamilyAddWN();
        return;
    }

    // ????
    if (ptActMenuWin == NULL)
    {
        // initSkillShopWindow1();
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
#ifdef _FAMILYBADGE_
        徽章窗口 = FALSE;
        徽章索引 = -1;
        徽章个数 = 0;
        徽章当前页 = 0;
        memset(徽章数据, -1, 4 * 200);
        extern unsigned int sockfd;
        lssproto_FamilyBadge_send(sockfd);
#endif
        w = 571;
        h = 295;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;

        Inputforce = 0;

        familyRuleInput.buffer[0] = NULL;
        familyRuleInput.cnt = 0;
        familyRuleInput.cursor = 0;
        familyRuleInput.len = 120;
        familyRuleInput.lineLen = 46;
        familyRuleInput.lineDist = 30;
        familyRuleInput.color = 0;
        familyRuleInput.x = x + 82;
        familyRuleInput.y = y + 160;
        familyRuleInput.fontPrio = FONT_PRIO_FRONT;

        familyNameInput.buffer[0] = NULL;
        familyNameInput.cnt = 0;
        familyNameInput.cursor = 0;
        familyNameInput.len = 16;
        familyNameInput.color = 0;
        familyNameInput.x = x + 1000;
        familyNameInput.y = y + 40;
        familyNameInput.fontPrio = FONT_PRIO_FRONT;

        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        play_se(202, 320, 240); // ????????
    }
    else if (ptActMenuWin->hp >= 1)
    {
#ifdef _FAMILYBADGE_
        徽章总页数 = 徽章个数 % 12 ? 徽章个数 / 12 + 1 : 徽章个数 / 12;
#endif
        selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));
        selFontBtnId = selFontId(fontId, sizeof(fontId) / sizeof(int));

        if (mouse.onceState & MOUSE_LEFT_CRICK)
        {
            switch (selBtnId)
            {
            case 0:
                if (徽章索引 == -1)
                {
                    StockChatBufferLine("请选择家族徽章！", FONT_PAL_WHITE);
                    break;
                }
                if (!familyNameInput.buffer[0])
                {
                    StockChatBufferLine("请输入家族名称！", FONT_PAL_WHITE);
                    break;
                }
                if (!familyRuleInput.buffer[0])
                {
                    StockChatBufferLine("请输入家族主旨！", FONT_PAL_WHITE);
                    break;
                }
                if (familyElf == 2)
                {
                    StockChatBufferLine("请选择守护精灵！", FONT_PAL_WHITE);
                    break;
                }
                if (pet[familyPetIndex].useFlag == NULL)
                {
                    StockChatBufferLine("请选择家族守护兽！", FONT_PAL_WHITE);
                    break;
                }
                makeEscapeString(familyNameInput.buffer, buf2, sizeof(buf2));
                makeEscapeString(familyRuleInput.buffer, buf3, sizeof(buf3));
                sprintf_s(buf, "A|%s|%d|%d|%s|%d", buf2, familyPetIndex, familyElf, buf3, 徽章索引);
                lssproto_FM_send(sockfd, buf);
                closeFamilyAddWN();
                return;
                break;
            case 1:
                closeFamilyAddWN();
                return;
                break;
            case 2:
                prevBtn = TRUE;
                i = 0;
                while (1)
                {
                    familyPetIndex--;
                    if (familyPetIndex < 0)
                        familyPetIndex = 4;
                    if (pet[familyPetIndex].useFlag != NULL)
                        break;
                    if ((i++) > 5)
                    {
                        familyPetIndex = -1;
                        break;
                    }
                }
                if (pActPet10 != NULL)
                {
                    DeathAction(pActPet10);
                    pActPet10 = NULL;
                }
                break;
            case 3:
                nextBtn = TRUE;
                i = 0;
                while (1)
                {
                    familyPetIndex++;
                    if (familyPetIndex >= 5)
                        familyPetIndex = 0;
                    if (pet[familyPetIndex].useFlag != NULL)
                        break;
                    if ((i++) > 5)
                    {
                        familyPetIndex = -1;
                        break;
                    }
                }
                if (pActPet10 != NULL)
                {
                    DeathAction(pActPet10);
                    pActPet10 = NULL;
                }

                break;
            }
            switch (selFontBtnId)
            {
            case 0:
                Inputforce = 1;
                break;
            case 1:
                if (familyElf == 0)
                    familyElf = 1;
                else
                    familyElf = 0;
                break;
            case 2:
                Inputforce = 2;
                break;
            }
        }

        if (mouse.state & MOUSE_LEFT_CRICK && prevBtn == TRUE)
            prevBtn = TRUE;
        else
            prevBtn = FALSE;

        if (mouse.state & MOUSE_LEFT_CRICK && nextBtn == TRUE)
            nextBtn = TRUE;
        else
            nextBtn = FALSE;

        btnId[0] = StockDispBuffer(x + 90, y + 274, DISP_PRIO_IME3, CG_OK_BTN, 2);
        btnId[1] = StockDispBuffer(x + 270, y + 274, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
        btnId[2] = StockDispBuffer(x + 435, y + 270, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
        btnId[3] = StockDispBuffer(x + 495, y + 270, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
#ifdef _FAMILYBADGE_
        fontId[5] = StockFontBuffer(x + 170, y + 135, FONT_PRIO_FRONT, FONT_PAL_RED, "设置徽章", 2);
        if (徽章索引 != -1)
            StockDispBuffer(x + 130, y + 139, DISP_PRIO_IME4, 徽章数据[徽章索引], 0);
        if (mouse.onceState & MOUSE_LEFT_CRICK)
            if (HitFontNo == fontId[5])
                徽章窗口 = !徽章窗口;
        if (徽章窗口)
        {
            StockDispBuffer(x + 290, y + 335, DISP_PRIO_MENU, 55232, 1);
            int 按钮ID = StockDispBuffer(x + 28, y + 333, DISP_PRIO_IME3, 55243, 2);
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                if (HitDispNo == 按钮ID)
                {
                    if (徽章当前页)
                        徽章当前页--;
                }
            }
            按钮ID = StockDispBuffer(x + 550, y + 333, DISP_PRIO_IME3, 55244, 2);
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                if (HitDispNo == 按钮ID)
                {
                    if (徽章当前页 != 徽章总页数 - 1)
                        徽章当前页++;
                }
            }
            int i = 0;
            for (; i < 12; i++)
            {
                if (徽章数据[i + 徽章当前页 * 12] != -1)
                {
                    按钮ID = StockDispBuffer(x + 70 + i * 40, y + 333, DISP_PRIO_IME3, 55245, 2);
                    StockDispBuffer(x + 70 + i * 40, y + 333, DISP_PRIO_IME4, 徽章数据[i + 徽章当前页 * 12], 0);
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        if (HitDispNo == 按钮ID)
                        {
                            徽章索引 = i + 徽章当前页 * 12;
                            徽章窗口 = FALSE;
                        }
                    }
                }
                else
                    break;
            }
        }
#endif
        // 家族名称：
        fontId[0] = StockFontBuffer(x + 100, y + 40, FONT_PRIO_FRONT, FONT_PAL_WHITE, "                  ", 2);
        // 族长名称：
        StockFontBuffer(x + 100, y + 70, FONT_PRIO_FRONT, FONT_PAL_WHITE, pc.name, 0);
        // 人数
        StockFontBuffer(x + 100, y + 100, FONT_PRIO_FRONT, FONT_PAL_WHITE, "无", 0);
        // pet
        if (pet[familyPetIndex].useFlag != NULL)
            if (pet[familyPetIndex].freeName[0] == NULL)
                sprintf_s(buf, " %s", pet[familyPetIndex].name);
            else
                sprintf_s(buf, " %s", pet[familyPetIndex].freeName);
        else
            sprintf_s(buf, "");
        StockFontBuffer(x + 436, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

        // sprite
        sprintf_s(buf, "  %s  ", familySpriteName[familyElf]);
        fontId[1] = StockFontBuffer(x + 436, y + 36, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 2);

        // rule
        sprintf_s(buf, "%38s", "");
        fontId[2] = StockFontBuffer(x + 82, y + 162, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 2);

        familyNameInput.x = x + 100;
        familyNameInput.y = y + 40;
        familyRuleInput.x = x + 82;
        familyRuleInput.y = y + 162;

        if (Inputforce == 1)
        {
            StockFontBuffer2(&familyNameInput);
            GetKeyInputFocus(&familyNameInput);
            if (familyRuleInput.buffer[0] != NULL)
                StockFontBufferFamily(&familyRuleInput);
        }
        else if (Inputforce == 2)
        {
            StockFontBufferFamily(&familyRuleInput);
            GetKeyInputFocus(&familyRuleInput);
            if (familyNameInput.buffer[0] != NULL)
                StockFontBuffer2(&familyNameInput);
        }
        else
        {
            GetKeyInputFocus(&MyChatBuffer);
            if (familyRuleInput.buffer[0] != NULL)
                StockFontBufferFamily(&familyRuleInput);
            if (familyNameInput.buffer[0] != NULL)
                StockFontBuffer2(&familyNameInput);
        }
        if (pActPet10 == NULL && (pet[familyPetIndex].useFlag != NULL))
            pActPet10 = MakeAnimDisp(x + 460, y + 240, pet[familyPetIndex].graNo, 0);
        StockDispBuffer(x + 312, y + 79, DISP_PRIO_IME3, pc.faceGraNo, 0);
        StockDispBuffer(x + 286, y + 148, DISP_PRIO_IME3, CG_FAMILY_DETAIL_WIN, 1);
    }
}

static short familyListProcNo = 0;
static int pagenum = 0;
static int familytotal = 0;
static int familynum = 0;
static ACTION *pActPet11;
typedef struct
{
    int index;
    int tempindex;
    char name[512];
    char leadername[512];
    int popu;
    int DP;
    int setupflag;
    int acceptflag;
} TfamilyList;

static TfamilyList familyList[10];

void closeFamilyList(void)
{
    play_se(203, 320, 240); // ????????
    familyListProcNo = 0;
    windowTypeWN = -1;
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;
    DeathAction(pActPet11);
    pActPet11 = NULL;
    // GetKeyInputFocus( &MyChatBuffer );
}

void initFamilyList(char *data)
{
    int i = 0, j = 0;
    char familybuf[128];
    char ibuf[16];

    familyListProcNo = 1;

    if (windowTypeWN != WINDOW_MESSAGETYPE_FAMILYJOIN)
    {
        windowTypeWN = WINDOW_MESSAGETYPE_FAMILYJOIN;
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
    }

    for (i = 0; i < 10; i++)
    {
        familyList[i].index = -1;
        familyList[i].name[0] = NULL;
        familyList[i].leadername[0] = NULL;
        familyList[i].popu = 0;
        familyList[i].DP = 0;
        familyList[i].tempindex = -1;
        familyList[i].setupflag = 0;
        familyList[i].acceptflag = 0;
    }

    getStringToken(data, '|', 3, sizeof(ibuf) - 1, ibuf);
    familytotal = atoi(ibuf);
    getStringToken(data, '|', 4, sizeof(ibuf) - 1, ibuf);
    pagenum = atoi(ibuf);
    // makeStringFromEscaped( pagenum );
    getStringToken(data, '|', 5, sizeof(ibuf) - 1, ibuf);
    familynum = atoi(ibuf);
    // makeStringFromEscaped( familynum );

    for (i = 0; i < familynum; i++)
    {
        getStringToken(data, '|', 6 + i, sizeof(familybuf) - 1, familybuf);

        getStringToken(familybuf, ' ', 1, sizeof(ibuf) - 1, ibuf);
        familyList[i].index = atoi(ibuf);

        getStringToken(familybuf, ' ', 2, sizeof(familyList[i].name) - 1, familyList[i].name);
        makeStringFromEscaped(familyList[i].name);

        getStringToken(familybuf, ' ', 3, sizeof(familyList[i].leadername) - 1, familyList[i].leadername);
        makeStringFromEscaped(familyList[i].leadername);

        getStringToken(familybuf, ' ', 5, sizeof(ibuf) - 1, ibuf);
        familyList[i].popu = atoi(ibuf);

        getStringToken(familybuf, ' ', 4, sizeof(ibuf) - 1, ibuf);
        familyList[i].DP = atoi(ibuf);

        getStringToken(familybuf, ' ', 6, sizeof(ibuf) - 1, ibuf);
        familyList[i].tempindex = atoi(ibuf);

        getStringToken(familybuf, ' ', 7, sizeof(ibuf) - 1, ibuf);
        familyList[i].setupflag = atoi(ibuf);

        getStringToken(familybuf, ' ', 8, sizeof(ibuf) - 1, ibuf);
        familyList[i].acceptflag = atoi(ibuf);
    }
}

void familyListWN(void)
{
    // static ACTION *ptActMenuWin = NULL;
    static int x, y, w, h, i, j;
    static int btnId[5];
    static int fontId[12];
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};
    int prevBtnGraNo10[] = {CG_PREV_BTN2, CG_PREV_BTN2_DOWN};
    int nextBtnGraNo10[] = {CG_NEXT_BTN2, CG_NEXT_BTN2_DOWN};

    static int prevBtn = 0, nextBtn = 0, prevBtn10 = 0, nextBtn10 = 0;
    int selBtnId = -1;
    int selFontBtnId = -1;
    int ret = 0;
    char buf[1024];
    char buf2[512];

    if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        closeFamilyList();
        return;
    }

    if (familyListProcNo == 10 || familyListProcNo == 20)
    {
        familyDetailWN();
    }

    if (familyListProcNo == 1)
    {
        if (ptActMenuWin == NULL)
        {
            familyListProcNo = 1;
            w = 7;
            h = 8;
            x = (lpDraw->xSize - w * 64) / 2;
            y = (lpDraw->ySize - h * 48) / 2;
            ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
            play_se(202, 320, 240); // ????????
        }
        else if (ptActMenuWin->hp >= 1)
        {

            selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));
            selFontBtnId = selFontId(fontId, sizeof(fontId) / sizeof(int));

            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                switch (selBtnId)
                {
                case 0:
                    windowTypeWN = -1;
                    DeathAction(ptActMenuWin);
                    ptActMenuWin = NULL;
                    return;
                case 1:
                    prevBtn = TRUE;
                    if (pagenum > 1)
                    {
                        sprintf_s(buf, "S|F|%d|0", pagenum - 1);
                        lssproto_FM_send(sockfd, buf);
                    }
                    else
                    {
                        sprintf_s(buf, "S|F|%d|0", (familytotal - 1) / 10 + 1);
                        lssproto_FM_send(sockfd, buf);
                    }
                    return;
                    break;
                case 2:
                    nextBtn = TRUE;
                    if (pagenum < (familytotal - 1) / 10 + 1)
                    {
                        sprintf_s(buf, "S|F|%d|0", pagenum + 1);
                        lssproto_FM_send(sockfd, buf);
                    }
                    else
                    {
                        sprintf_s(buf, "S|F|1|0");
                        lssproto_FM_send(sockfd, buf);
                    }
                    return;
                    break;
                case 3:
                    prevBtn10 = TRUE;
                    if (pagenum > 10)
                    {
                        sprintf_s(buf, "S|F|%d|0", pagenum - 10);
                            lssproto_FM_send(sockfd, buf);
                    }
                    else
                    {
                        sprintf_s(buf, "S|F|%d|0", (familytotal - 1) / 10 + 1);
                            lssproto_FM_send(sockfd, buf);
                    }
                    return;
                    break;
                case 4:
                    nextBtn10 = TRUE;
                    if (pagenum < (familytotal - 1) / 10 + 1 - 9)
                    {
                        sprintf_s(buf, "S|F|%d|0", pagenum + 10);
                        lssproto_FM_send(sockfd, buf);
                    }
                    else
                    {
                        sprintf_s(buf, "S|F|1|0");
                        lssproto_FM_send(sockfd, buf);
                    }
                    return;
                    break;
                }

                if (selFontBtnId != -1)
                {
                    sprintf_s(buf, "S|D|%s|%d|%d",
                              makeEscapeString(familyList[selFontBtnId].name, buf2, sizeof(buf2)),
                              familyList[selFontBtnId].index, familyList[selFontBtnId].tempindex);
                    lssproto_FM_send(sockfd, buf);
                }
            }

            if (mouse.state & MOUSE_LEFT_CRICK && prevBtn == TRUE)
                prevBtn = TRUE;
            else
                prevBtn = FALSE;
            if (mouse.state & MOUSE_LEFT_CRICK && nextBtn == TRUE)
                nextBtn = TRUE;
            else
                nextBtn = FALSE;
            if (mouse.state & MOUSE_LEFT_CRICK && prevBtn10 == TRUE)
                prevBtn10 = TRUE;
            else
                prevBtn10 = FALSE;
            if (mouse.state & MOUSE_LEFT_CRICK && nextBtn10 == TRUE)
                nextBtn10 = TRUE;
            else
                nextBtn10 = FALSE;

            StockFontBuffer(x + 190, y + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族列表", 0);
#ifdef _READ16BITBMP
            if (!g_bUseAlpha)
                StockDispHLine(x + 150, y + 40, 2);
#endif
            btnId[0] = StockDispBuffer(x + 220, y + 350, DISP_PRIO_IME3, CG_EXIT_BTN, 2);
            btnId[1] = StockDispBuffer(x + 16 + 60, y + 8 + 60, DISP_PRIO_IME4, prevBtnGraNo[prevBtn], 2);
            btnId[2] = StockDispBuffer(x + 16 + 210, y + 8 + 60, DISP_PRIO_IME4, nextBtnGraNo[nextBtn], 2);
            btnId[3] = StockDispBuffer(x + 16 + 20, y + 8 + 60, DISP_PRIO_IME4, prevBtnGraNo10[prevBtn10], 2);
            btnId[4] = StockDispBuffer(x + 16 + 250, y + 8 + 60, DISP_PRIO_IME4, nextBtnGraNo10[nextBtn10], 2);
            sprintf_s(buf, "%3d / %3d 页", pagenum, (familytotal - 1) / 10 + 1);
            StockFontBuffer(x + 100, y + 60, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

#ifdef _NEWFONT_
            sprintf_s(buf, "    [家 族 名 称]                        [族  长]            [人数]   [家族声望]");
            StockFontBuffer(x + 25, y + 90, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
#else
            sprintf_s(buf, "%-4s %-16s %-16s %6s %10s", "", "[家 族 名 称]", "[族  长]", "[人数]", "[家族声望]");
            StockFontBuffer(x + 25, y + 90, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
#endif

            for (i = 0; i < 10; i++)
            {
                if (familyList[i].name[0] == NULL)
                    break;
                if (familyList[i].acceptflag != 1)
                    j = FONT_PAL_AQUA;
                else if (familyList[i].setupflag != 1)
                    j = FONT_PAL_GREEN;
                else
                    j = FONT_PAL_WHITE;
#ifdef _NEWFONT_
                char *str = "                                                                                                        ";

                // sprintf_s( buf, "%4d: %-16s %-16s %6d %10d", (pagenum-1)*10+1+i, familyList[i].name, familyList[i].leadername,
                //         familyList[i].popu, (familyList[i].DP /100));
                sprintf_s(buf, "%4d:%s", (pagenum - 1) * 10 + 1 + i, familyList[i].name);
                StockFontBuffer(x + 15, y + 110 + (i * 20), FONT_PRIO_FRONT, j, buf, 0);

                StockFontBuffer(x + 240 - getTextLength(familyList[i].leadername) / 2, y + 110 + (i * 20), FONT_PRIO_FRONT, j, familyList[i].leadername, 0);

                sprintf_s(buf, "%d", familyList[i].popu);
                StockFontBuffer(x + 335 - getTextLength(buf) / 2, y + 110 + (i * 20), FONT_PRIO_FRONT, j, buf, 0);

                sprintf_s(buf, "%d", (familyList[i].DP / 100));
                StockFontBuffer(x + 400 - getTextLength(buf) / 2, y + 110 + (i * 20), FONT_PRIO_FRONT, j, buf, 0);

                fontId[i] = StockFontBuffer(x + 15, y + 110 + (i * 20), FONT_PRIO_FRONT, j, str, 2);

#else
                sprintf_s(buf, "%4d: %-16s %-16s %6d %10d", (pagenum - 1) * 10 + 1 + i, familyList[i].name, familyList[i].leadername,
                          familyList[i].popu, (familyList[i].DP / 100));
                fontId[i] = StockFontBuffer(x + 15, y + 110 + (i * 20), FONT_PRIO_FRONT, j, buf, 2);
#endif
            }
        }
    }
}

struct
{
    int index;
    int tempindex;
    char name[20];
    char leadername[20];
    int leadergraph;
    char petname[20];
    char petattr[64];
    int joinnum;
    int joinflag;
    char rule[130];
    int petgra;
    int sprite;
#ifdef _FAMILYBADGE_
    int badgeNo;
#endif
} familyDetail;

static int changeData[2] = {0, 0};
void initFamilyDetail(char *data)
{
    char ibuf[64];

    if (familyListProcNo == 1)
        familyListProcNo = 10;
    else if (familyListProcNo == 0)
    {
        familyListProcNo = 20;
        lssproto_WN_recv(sockfd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK, -1, -1, AnnounceBuf);
    }

    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;

    familyRuleShow.buffer[0] = NULL;
    familyRuleShow.cnt = 0;
    familyRuleShow.cursor = 0;
    familyRuleShow.len = 120;
    familyRuleShow.lineLen = 46;
    familyRuleShow.lineDist = 30;
    familyRuleShow.color = 0;
    // familyRuleShow.x = x+80;
    // familyRuleShow.y = y+160;
    familyRuleShow.fontPrio = FONT_PRIO_FRONT;

    getStringToken(data, '|', 3, sizeof(ibuf) - 1, ibuf);
    familyDetail.tempindex = atoi(ibuf);
    getStringToken(data, '|', 4, sizeof(ibuf) - 1, ibuf);
    familyDetail.index = atoi(ibuf);
    getStringToken(data, '|', 5, sizeof(familyDetail.name) - 1, familyDetail.name);
    makeStringFromEscaped(familyDetail.name);
    getStringToken(data, '|', 6, sizeof(familyDetail.leadername) - 1, familyDetail.leadername);
    makeStringFromEscaped(familyDetail.leadername);
    getStringToken(data, '|', 7, sizeof(ibuf) - 1, ibuf);
    familyDetail.leadergraph = atoi(ibuf);
    getStringToken(data, '|', 8, sizeof(familyDetail.petname) - 1, familyDetail.petname);
    makeStringFromEscaped(familyDetail.petname);
    getStringToken(data, '|', 9, sizeof(familyDetail.petattr) - 1, familyDetail.petattr);
    makeStringFromEscaped(familyDetail.petattr);
    getStringToken(data, '|', 10, sizeof(ibuf) - 1, ibuf);
    familyDetail.joinnum = atoi(ibuf);
    getStringToken(data, '|', 11, sizeof(ibuf) - 1, ibuf);
    familyDetail.joinflag = atoi(ibuf);
    getStringToken(data, '|', 12, sizeof(familyDetail.rule) - 1, familyDetail.rule);
    makeStringFromEscaped(familyDetail.rule);
    strcpy(familyRuleShow.buffer, familyDetail.rule);
    familyRuleShow.cnt = strlen(familyRuleShow.buffer);
    familyRuleShow.cursor = strlen(familyRuleShow.buffer);

    getStringToken(data, '|', 13, sizeof(ibuf) - 1, ibuf);
    familyDetail.sprite = atoi(ibuf);
#ifdef _FAMILYBADGE_
    getStringToken(data, '|', 14, sizeof(ibuf) - 1, ibuf);
    familyDetail.badgeNo = atoi(ibuf);
#endif
    getStringToken(familyDetail.petattr, ' ', 1, sizeof(ibuf) - 1, ibuf);
    familyDetail.petgra = atoi(ibuf);

    changeData[0] = 0;
    changeData[1] = 0;
}

int familyDetailWN(void)
{
#ifdef _FAMILYBADGE_
    static int 徽章窗口;
    static int 徽章索引;
    int 徽章总页数;
    static int 徽章当前页;
    static int 购买徽章窗口;
    extern int 徽章价格;
    static int 临时徽章索引;
#endif
    static int w, h, x, y, xx, yy, i;
    static int btnId[5];
    static int fontId[6];
    static int prevBtn = 0, nextBtn = 0;
    static int familyPetIndex = 0;
    int prevBtnGraNo[] = {CG_PREV_BTN, CG_PREV_BTN_DOWN};
    int nextBtnGraNo[] = {CG_NEXT_BTN, CG_NEXT_BTN_DOWN};

    // static int changeData[2] = {0,0};
    int selBtnId = -1;
    int selFontBtnId = -1;
    char buf[1024];
    char buf2[512];

    if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        GetKeyInputFocus(&MyChatBuffer);
        closeFamilyList();
        return 0;
    }

    if (ptActMenuWin == NULL)
    {

        // familyListProcNo = 11;
        for (int i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }

        w = 571;
        h = 295;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        // xx = 0;
        // yy = 0;
#ifdef _FAMILYBADGE_
        徽章窗口 = FALSE;
        徽章索引 = -1;
        徽章个数 = 0;
        徽章当前页 = 0;
        购买徽章窗口 = FALSE;
        memset(徽章数据, -1, 4 * 200);
        if ((pc.familyleader == FMMEMBER_LEADER) && (strcmp(pc.familyName, familyDetail.name) == 0))
        {
            extern unsigned int sockfd;
            lssproto_FamilyBadge_send(sockfd);
        }
#endif
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, -1);
        familyRuleShow.x = x + 82;
        familyRuleShow.y = y + 162;
        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin->hp >= 1)
    {
#ifdef _FAMILYBADGE_
        if (!购买徽章窗口)
        {
#endif
            selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));
            selFontBtnId = selFontId(fontId, sizeof(fontId) / sizeof(int));

            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                switch (selBtnId)
                {
                case 0:
                    if (pc.familyName[0] != NULL)
                    {
                        lssproto_TK_recv(sockfd, 0, "P|你已经加入其他家族了喔！", 0);
                        break;
                    }

                    sprintf_s(buf, "J|%d|%d|%s|%d",
                              familyDetail.tempindex, familyDetail.index,
                              makeEscapeString(familyDetail.name, buf2, sizeof(buf2)),
                              familyDetail.sprite);
                        lssproto_FM_send(sockfd, buf);
                    familyListProcNo = 1;
                    closeFamilyList();
                    GetKeyInputFocus(&MyChatBuffer);
                    return 0;
                    break;
                case 1:
                    familyListProcNo = 1;
                    DeathAction(ptActMenuWin);
                    ptActMenuWin = NULL;
                    DeathAction(pActPet11);
                    pActPet11 = NULL;
                    GetKeyInputFocus(&MyChatBuffer);
                    return 0;
                    break;

                case 2:
                    prevBtn = TRUE;
                    i = 0;
                    while (1)
                    {
                        familyPetIndex--;
                        if (familyPetIndex < 0)
                            familyPetIndex = 4;
                        if (pet[familyPetIndex].useFlag != NULL)
                            break;
                        if ((i++) > 5)
                        {
                            familyPetIndex = -1;
                            break;
                        }
                    }
                    if (pActPet11 != NULL)
                    {
                        DeathAction(pActPet11);
                        pActPet11 = NULL;
                    }
                    changeData[1] = 1;
                    break;
                case 3:
                    nextBtn = TRUE;
                    i = 0;
                    while (1)
                    {
                        familyPetIndex++;
                        if (familyPetIndex >= 5)
                            familyPetIndex = 0;
                        if (pet[familyPetIndex].useFlag != NULL)
                            break;
                        if ((i++) > 5)
                        {
                            familyPetIndex = -1;
                            break;
                        }
                    }
                    if (pActPet11 != NULL)
                    {
                        DeathAction(pActPet11);
                        pActPet11 = NULL;
                    }
                    changeData[1] = 1;
                    break;
                case 4:
                    if ((changeData[0] == 1) &&
                        (strcmp(familyRuleShow.buffer, familyDetail.rule) != 0))
                    {
                        sprintf_s(buf, "X|R|%s", familyRuleShow.buffer);
                            lssproto_FM_send(sockfd, buf);
                    }

                    if (changeData[1] == 1)
                    {
                        sprintf_s(buf, "X|P|%d", familyPetIndex);
                            lssproto_FM_send(sockfd, buf);
                    }
#ifdef _FAMILYBADGE_
                    if (徽章索引 != 1)
                    {
                        if (徽章数据[徽章索引] != familyDetail.badgeNo)
                        {
                            sprintf_s(buf, "X|B|%d", 徽章索引);
                                lssproto_FM_send(sockfd, buf);
                        }
                    }
#endif
                    familyListProcNo = 1;
                    DeathAction(ptActMenuWin);
                    ptActMenuWin = NULL;
                    DeathAction(pActPet11);
                    pActPet11 = NULL;
                    GetKeyInputFocus(&MyChatBuffer);
                    return 0;
                    break;
                }

                switch (selFontBtnId)
                {
                case 0:
                    changeData[0] = 1;
                    break;
                }
            }
#ifdef _FAMILYBADGE_
        }
#endif
        if (mouse.state & MOUSE_LEFT_CRICK && prevBtn == TRUE)
            prevBtn = TRUE;
        else
            prevBtn = FALSE;

        if (mouse.state & MOUSE_LEFT_CRICK && nextBtn == TRUE)
            nextBtn = TRUE;
        else
            nextBtn = FALSE;

        // StockFontBuffer( x+60, y+60, FONT_PRIO_FRONT, FONT_PAL_WHITE, "", 0 );
        // sprintf_s( buf, "", pagenum, (familytotal-1)/8 +1  );
        // StockFontBuffer( x+160, y+30, FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族资料", 0 );
        // StockDispHLine( x+140, y+ );

        // sprintf_s( buf, "家族名称：%s", familyDetail.name );
        StockFontBuffer(x + 100, y + 40, FONT_PRIO_FRONT, FONT_PAL_WHITE, familyDetail.name, 0);
        // sprintf_s( buf, "族长名称：%s", familyDetail.leadername );
        StockFontBuffer(x + 100, y + 70, FONT_PRIO_FRONT, FONT_PAL_WHITE, familyDetail.leadername, 0);
        sprintf_s(buf, "%d 人", familyDetail.joinnum);
        StockFontBuffer(x + 100, y + 100, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

#ifdef _FAMILYBADGE_
        if (徽章索引 != -1)
            StockDispBuffer(x + 130, y + 139, DISP_PRIO_IME4, 徽章数据[徽章索引], 0);
        else
            StockDispBuffer(x + 130, y + 139, DISP_PRIO_IME4, familyDetail.badgeNo, 0);
#endif
        // 家族主旨
        sprintf_s(buf, "%38s", "");
#ifdef _FMVER21
        if ((pc.familyleader == FMMEMBER_LEADER) && (strcmp(pc.familyName, familyDetail.name) == 0))
        {
#else
        if ((pc.familyleader == 1) && (strcmp(pc.familyName, familyDetail.name) == 0))
        {
#endif
            fontId[0] = StockFontBuffer(x + 82, y + 160, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 2);
            btnId[2] = StockDispBuffer(x + 435, y + 270, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2);
            btnId[3] = StockDispBuffer(x + 495, y + 270, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2);
#ifdef _FAMILYBADGE_
            int 按钮ID;
            徽章总页数 = 徽章个数 % 12 ? 徽章个数 / 12 + 1 : 徽章个数 / 12;
            fontId[5] = StockFontBuffer(x + 170, y + 135, FONT_PRIO_FRONT, FONT_PAL_RED, "设置徽章", 2);
            if (!购买徽章窗口)
            {
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                    if (HitFontNo == fontId[5])
                        徽章窗口 = !徽章窗口;
            }
            if (徽章窗口)
            {
                StockDispBuffer(x + 290, y + 335, DISP_PRIO_MENU, 55232, 1);
                按钮ID = StockDispBuffer(x + 28, y + 333, DISP_PRIO_IME3, 55243, 2);
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    if (HitDispNo == 按钮ID)
                    {
                        if (徽章当前页)
                            徽章当前页--;
                    }
                }
                按钮ID = StockDispBuffer(x + 550, y + 333, DISP_PRIO_IME3, 55244, 2);
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    if (HitDispNo == 按钮ID)
                    {
                        if (徽章当前页 != 徽章总页数 - 1)
                            徽章当前页++;
                    }
                }
                int i = 0;
                for (; i < 12; i++)
                {
                    if (徽章数据[i + 徽章当前页 * 12] != -1)
                    {
                        按钮ID = StockDispBuffer(x + 70 + i * 40, y + 333, DISP_PRIO_IME3, 55245, 2);
                        StockDispBuffer(x + 70 + i * 40, y + 333, DISP_PRIO_IME4, 徽章数据[i + 徽章当前页 * 12], 0);
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            if (HitDispNo == 按钮ID)
                            {
                                临时徽章索引 = i + 徽章当前页 * 12;
                                徽章窗口 = FALSE;
                                购买徽章窗口 = TRUE;
                            }
                        }
                    }
                    else
                        break;
                }
            }
            if (购买徽章窗口)
            {
                StockDispBuffer(x + 280, y + 340, DISP_PRIO_IME3, 40060, 1);
                char token[125];
                sprintf_s(token, "更换徽章将收取%d金币！", 徽章价格);
                StockFontBuffer(x + 200, y + 320, 2, 0, token, 0);
                按钮ID = StockDispBuffer(x + 220, y + 355, DISP_PRIO_IME4, 26093, 2);
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    if (HitDispNo == 按钮ID)
                    {
                        购买徽章窗口 = FALSE;
                        徽章索引 = 临时徽章索引;
                    }
                }
                按钮ID = StockDispBuffer(x + 340, y + 355, DISP_PRIO_IME4, 26042, 2);
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    if (HitDispNo == 按钮ID)
                    {
                        购买徽章窗口 = FALSE;
                    }
                }
            }
#endif
        }
        else
        {
            StockDispBuffer(x + 130, y + 139, DISP_PRIO_IME4, familyDetail.badgeNo, 0);
            fontId[0] = StockFontBuffer(x + 20, y + 160, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
        }
        // strcpy( familyRuleShow.buffer, familyDetail.rule );

        if (changeData[0] == 1)
        {
            StockFontBufferFamily(&familyRuleShow);
            GetKeyInputFocus(&familyRuleShow);
        }
        else
        {
            GetKeyInputFocus(&MyChatBuffer);
            if (familyRuleShow.buffer[0] != NULL)
                StockFontBufferFamily(&familyRuleShow);
        }

        // StockFontBuffer( x+20, y+160, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0 );
        // StockFontBuffer( x+20, y+190, FONT_PRIO_FRONT, FONT_PAL_WHITE, familyDetail.rule, 0 );
        // StockFontBuffer( x+20, y+220, FONT_PRIO_FRONT, FONT_PAL_WHITE, familyDetail.rule, 0 );
        // sprintf_s( buf, "守 护 兽：%s", familyDetail.petname );
        if (changeData[1] == 0)
            StockFontBuffer(x + 436, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, familyDetail.petname, 0);
        else
            StockFontBuffer(x + 436, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, pet[familyPetIndex].name, 0);

        StockFontBuffer(x + 436, y + 36, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                        familySpriteName[familyDetail.sprite], 0);

        // fontId[0] = StockFontBuffer( x+200, y+250, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "加 入", 2 );
        // fontId[1] = StockFontBuffer( x+300, y+250, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "离 开", 2 );
        if (pc.familyName[0] == NULL && familyDetail.joinflag == 1)
        {
            btnId[0] = StockDispBuffer(x + 90, y + 274, DISP_PRIO_IME3, CG_JOIN_BTN, 2);
            btnId[1] = StockDispBuffer(x + 270, y + 274, DISP_PRIO_IME3, CG_EXIT_BTN2, 2);
        }
        else if ((changeData[0] == 1) || (changeData[1] == 1) || 徽章索引 != -1)
        {
            btnId[4] = StockDispBuffer(x + 90, y + 274, DISP_PRIO_IME3, 26093, 2);
            btnId[1] = StockDispBuffer(x + 270, y + 274, DISP_PRIO_IME3, CG_EXIT_BTN2, 2);
        }
        else
            btnId[1] = StockDispBuffer(x + 180, y + 274, DISP_PRIO_IME3, CG_EXIT_BTN2, 2);

        if (pActPet11 == NULL)
            if (changeData[1] == 0)
                pActPet11 = MakeAnimDisp(x + 460, y + 240, familyDetail.petgra, 0);
            else
                pActPet11 = MakeAnimDisp(x + 460, y + 240, pet[familyPetIndex].graNo, 0);

        StockDispBuffer(x + 306, y + 76, DISP_PRIO_IME3, familyDetail.leadergraph, 0);
        StockDispBuffer(x + 280, y + 145, DISP_PRIO_IME3, CG_FAMILY_DETAIL_WIN, 1);
    }
    return 0;
}

#ifdef _FMVER21
/*----------  Shan (BEGIN) ----------*/
/*---------  AD 2001/05/31  ---------*/
// shan Family MemberList
void initFMWindowType(char *data)
{
    makeStringFromEscaped(data);

    selStartLine = 0;
    FMdengonidex = 0;

    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 64);

    time(&FMHoldTime);

    char namebuf[32], lvbuf[8], statusbuf[32], onlinebuf[32], dpbuf[64];
    int int_status, onlineflag;
    char tmp[32];
#ifdef _FM_MODIFY
    int iGSId;
    char szGSname[64], szPlanetId[8];
#endif
#ifdef _NEW_MANOR_LAW
    char momentum[12] = {0};
#endif
    // 家族传送的行数为 11 ，前三行标题，后十行为资料(作为EVENT之用)
    // 最后一行则为‘是否继续新增家族人员的选项’
    for (int i = 0; i < 11; i++)
    {
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                if (i != 10)
                {
                    getStringToken(msgWN[i], '|', 6, sizeof(statusbuf) - 1, tmp);
                    getStringToken(msgWN[i], '|', 7, sizeof(statusbuf) - 1, statusbuf);
                }
                else
                    getStringToken(msgWN[i], '|', 3, sizeof(statusbuf) - 1, statusbuf);
                // 存的 INDEX -> 1~10
                FMmsgWN[i - (selStartLine - 1)] = atoi(statusbuf);
                FMelderFlag[i - (selStartLine - 1)] = atoi(tmp);
                getStringToken(msgWN[i], '|', 1, sizeof(tmp) - 1, tmp);
                FMpidWN[i - (selStartLine - 1)] = atoi(tmp);
                getStringToken(msgWN[i], '|', 2, sizeof(FMnameWN[i - (selStartLine - 1)]) - 1,
                               FMnameWN[i - (selStartLine - 1)]);
                // 处理字串
                if (i < 10)
                {
                    getStringToken(msgWN[i], '|', 2, sizeof(namebuf) - 1, namebuf);
                    getStringToken(msgWN[i], '|', 3, sizeof(lvbuf) - 1, lvbuf);
                    getStringToken(msgWN[i], '|', 4, sizeof(onlinebuf) - 1, onlinebuf);
                    onlineflag = atoi(onlinebuf);
                    if (onlineflag > 0)
                        strcpy(onlinebuf, "OnLine");
                    else
                        strcpy(onlinebuf, "OffLine");
                    getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                    getStringToken(msgWN[i], '|', 7, sizeof(statusbuf) - 1, statusbuf);
                    int_status = atoi(statusbuf);
                    switch (int_status)
                    {
                        // 已 加 入
                    case FMMEMBER_MEMBER: // 一般族员
                        strcpy(statusbuf, "一般族员");
                        break;
                    case FMMEMBER_LEADER: // 家族族长
                        strcpy(statusbuf, "族    长");
                        break;
                    case FMMEMBER_ELDER: // 长老
                        strcpy(statusbuf, "长    老");
                        break;
                        /*                            case FMMEMBER_ELDER:                 // 长老
                        strcpy(statusbuf, "长    老");
                        break;
                        case FMMEMBER_INVITE:                 // 祭司
                        strcpy(statusbuf, "祭    司");
                        break;
                        case FMMEMBER_BAILEE:                 // 财务长
                        strcpy(statusbuf, "财 务 长");
                        break;*/
                    case FMMEMBER_APPLY: // 申请加入
                        strcpy(statusbuf, "申请加入");
                        break;
                        /*                            case FMMEMBER_VICELEADER:            // 副族长
                        strcpy(statusbuf, "副 族 长");
                        break;*/
                    default: // 退    出
                        strcpy(statusbuf, "退    出");
                        break;
                    }
#ifdef _FM_MODIFY
                    // 处理星球名称
                    getStringToken(msgWN[i], '|', 8, sizeof(szGSname) - 1, szGSname);
                    iGSId = atoi(szGSname);
                    if (iGSId > 0)
                    {
                        for (int j = 0; j < MAX_GMSV; j++)
                        {
                            if (gmsv[j].used == '1')
                            {
                                getStringToken(gmsv[j].ipaddr, '.', 4, sizeof(szPlanetId) - 1, szPlanetId);
                                if (iGSId == atoi(szPlanetId))
                                {
                                    sprintf_s(szGSname, "%s", gmsv[j].name);
                                    break;
                                }
                            }
                        }
                    }
                    else
                        sprintf_s(szGSname, "无");

#ifndef _NEW_MANOR_LAW
                    sprintf_s(msgWN[i], "%-9s%-18s%6s%13s%10s%6s", onlinebuf, namebuf, lvbuf, dpbuf, statusbuf, szGSname);
#else
                    getStringToken(msgWN[i], '|', 9, sizeof(momentum) - 1, momentum);
                    sprintf_s(msgWN[i], "%-9s%-18s%3s%12s%12s%10s%7s", onlinebuf, namebuf, lvbuf, dpbuf, momentum, statusbuf, szGSname);
#endif

#else
                    sprintf_s(msgWN[i], "%-12s%-18s%3s%13s%10s", onlinebuf, namebuf, lvbuf, dpbuf, statusbuf);
#endif
                }
                else
                {
                    getStringToken(msgWN[i], '|', 1, sizeof(namebuf) - 1, namebuf);
                    getStringToken(msgWN[i], '|', 2, sizeof(tmp) - 1, tmp);
                    FMdengonidex = atoi(tmp);
                    getStringToken(msgWN[i], '|', 3, sizeof(statusbuf) - 1, statusbuf);
                    int_status = atoi(statusbuf);
                    switch (int_status)
                    {
                    case 1: // 继续召募
                        strcpy(statusbuf, "继续召募");
                        break;
                    case 0: // 中止召募
                        strcpy(statusbuf, "中止召募");
                        break;
                    default:
                        break;
                    }
                    sprintf_s(msgWN[i], " %-20s                         %s", namebuf, statusbuf);
                }
            }
        }
    }
}

/*----------  Shan (BEGIN) ----------*/
/*---------  AD 2001/05/31  ---------*/
// 家族人员列表
int fmDutyproc = 0; // select number duty
int fmDutyRet = 0;
int fmYesNoproc = 0; // delete number
int fmYesNoRet = 0;

#ifdef _FM_MODIFY
extern int TalkMode;
extern char secretName[32];
extern STR_BUFFER *pNowStrBuffer;
#endif

void FMWindowType(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static int fmdelid;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    char buf[1024];
#ifdef _FM_MODIFY
    int iNameX1, iNameY1, iNameX2, iNameY2;
#endif

    if (fmDutyproc == 1)
    {
        fmDutyRet = fmselectdetuyWindow(200, 100, fmdelid);
        return;
    }
    if (fmDutyRet != 0)
        ptActMenuWin = NULL;

    if (ptActMenuWin == NULL)
    {
#ifndef _FM_MODIFY
        winW = 7;
#else
#ifdef _NEW_MANOR_LAW
        winW = 9;
#else
        winW = 8;
#endif
#endif
        winH = 8;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 10; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 335;
            }
        }
        msgLine = 11;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;

        if (fmDutyRet != 0)
        {
            if (fmDutyRet != FMMEMBER_LEADER || fmDutyRet != FMMEMBER_APPLY)
            {
                id2 = fmdelid;
                FMmsgWN[id2] = fmDutyRet;
            }
            fmDutyRet = 0;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = selStartLine; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    if (i == 10)
                    {
                        x1 = winX + 25;
                        y1 = winY + 28 + (i + 3) * 21;
                        x2 = x1 + 405;
                        y2 = y1 + 20;

#ifdef _NEWFONT_
                        x1 -= 60;
                        x2 -= 60;
                        y1 += 2;
                        y2 += 2;
#endif
                    }
                    else
                    {
#ifdef _NEW_MANOR_LAW
                        x1 = winX + 420;
#else
                        x1 = winX + 364;
#endif
                        y1 = winY + 18 + (i + 3) * 21;
                        x2 = x1 + 70;
                        y2 = y1 + 20;
#ifdef _FM_MODIFY
                        iNameX1 = winX + 95;
                        iNameY1 = winY + 18 + (i + 3) * 21;
                        iNameX2 = iNameX1 + 118;
                        iNameY2 = iNameY1 + 20;
#endif
                    }

                    // 只有族长、长老才可以修改
                    /*if( pc.familyleader == FMMEMBER_LEADER &&
                    FMmsgWN[i-selStartLine+1] > 0 && FMmsgWN[i-selStartLine+1] < FMMEMBER_NUMBER &&
                    FMmsgWN[i-selStartLine+1] != FMMEMBER_LEADER ||
                    ( pc.familyleader == FMMEMBER_ELDER || pc.familyleader == FMMEMBER_VICELEADER ) &&
                    ( FMmsgWN[i-selStartLine+1] == FMMEMBER_MEMBER || FMmsgWN[i-selStartLine+1] == FMMEMBER_APPLY ) ||
                    (i-selStartLine+1) == 11  &&
                    (pc.familyleader == FMMEMBER_LEADER || pc.familyleader == FMMEMBER_ELDER || pc.familyleader == FMMEMBER_VICELEADER))*/
                    if (pc.familyleader == FMMEMBER_LEADER &&
                            FMmsgWN[i - selStartLine + 1] > 0 && FMmsgWN[i - selStartLine + 1] < FMMEMBER_NUMBER &&
                            FMmsgWN[i - selStartLine + 1] != FMMEMBER_LEADER ||
                        pc.familyleader == FMMEMBER_ELDER &&
                            (FMmsgWN[i - selStartLine + 1] == FMMEMBER_MEMBER || FMmsgWN[i - selStartLine + 1] == FMMEMBER_APPLY) ||
                        (i - selStartLine + 1) == 11 &&
                            (pc.familyleader == FMMEMBER_LEADER || pc.familyleader == FMMEMBER_ELDER))
#ifdef _NEWFONT_
                        if (MakeHitBox(x1 + 55, y1, x2 + 55, y2, DISP_PRIO_BOX2))
#else
                        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
#endif
                        {
                            time_t now_time;
                            time(&now_time);
                            if (mouse.onceState & MOUSE_LEFT_CRICK && (now_time - FMHoldTime) >= 1)
                            {
                                time(&FMHoldTime);
                                id2 = i - selStartLine + 1;

                                if (id2 >= 1 && id2 <= 10)
                                    switch (FMmsgWN[id2])
                                    {
                                    case FMMEMBER_MEMBER: // 一般成员
                                    case FMMEMBER_APPLY:  // 申请加入
                                    case FMMEMBER_ELDER:  // 长老
                                        // case FMMEMBER_ELDER:          // 长老
                                        // case FMMEMBER_INVITE:          // 祭司
                                        // case FMMEMBER_BAILEE:           // 财务
                                        // case FMMEMBER_VICELEADER:   // 副族长
                                        fmDutyproc = 1;
                                        fmdelid = id2;
                                        id2 = -1;
                                        DeathAction(ptActMenuWin);
                                        ptActMenuWin = NULL;
                                        return;
                                        break;
                                    default:
                                        break;
                                    }
                                else if (id2 == 11)
                                    switch (FMmsgWN[id2])
                                    {
                                    case 1: // YES
                                        FMmsgWN[id2] = 0;
                                        break;
                                    case 0: // N O
                                        FMmsgWN[id2] = 1;
                                        break;
                                    default:
                                        break;
                                    }
                            }
                        }
#ifdef _FM_MODIFY
                    // 滑鼠移到人名上方显示方框
                    if (i < 10)
                    {
#ifdef _NEWFONT_
                        if (MakeHitBox(iNameX1, iNameY1 + 2, iNameX2 + 35, iNameY2 + 2, DISP_PRIO_BOX2))
#else
                        if (MakeHitBox(iNameX1, iNameY1, iNameX2, iNameY2, DISP_PRIO_BOX2))
#endif
                        {
                            time_t now_time;
                            time(&now_time);
                            if (mouse.onceState & MOUSE_LEFT_CRICK && (now_time - FMHoldTime) >= 1)
                            {
                                // 密语模式
#ifdef _TELLCHANNEL
                                TalkMode = 1;
                                sprintf_s(secretName, "%s ", FMnameWN[i + 1]);
#endif
                                pNowStrBuffer->buffer[0] = NULL;
                                pNowStrBuffer->cursor = 0;
                                pNowStrBuffer->cnt = 0;
#ifdef _FRIENDCHANNEL
                                StrToNowStrBuffer1(secretName);
#endif
                            }
                        }
                    }
#endif
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 12 || id == 100)
        {
            char data[256];
            char msg[256];
            if (id == 100)
            {
                btn = WINDOW_BUTTONTYPE_CANCEL;
            }
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
            {
                btn = 0;
            }
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d|%d|%d", FMdengonidex, id2, FMmsgWN[id2]);
            makeEscapeString(data, msg, sizeof(msg) - 1);

            if (1 <= id2 && id2 < 11)
            {
                sprintf_s(buf, "M|%s|%d|%d", FMnameWN[id2], FMpidWN[id2], FMmsgWN[id2]);
                    lssproto_FM_send(sockfd, buf);
            }
            if (id2 == 11)
            {
                sprintf_s(buf, "T|%d", FMmsgWN[id2]);
                    lssproto_FM_send(sockfd, buf);
            }
            if (FMmsgWN[id2] == FMMEMBER_NONE || FMmsgWN[id2] == FMMEMBER_MEMBER || FMmsgWN[id2] == 0)
            {
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            }
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
#ifndef _FM_MODIFY
            StockFontBuffer(winX + 34, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "              ‘家族成员列表’", 0);
            StockFontBuffer(winX + 24, winY + 21 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, " ‘状态’ ‘ 成 员 姓 名 ’ ‘等级’ ‘个人声望’ ‘加入’", 0);
#else

#ifdef _NEWFONT_
            StockFontBuffer(winX + 250, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "‘家族成员列表’", 0);
#else
            StockFontBuffer(winX + 34, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "                         ‘家族成员列表’", 0);
#endif

#ifdef _NEW_MANOR_LAW
            StockFontBuffer(winX + 24, winY + 21 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "‘状态’‘ 成 员 姓 名 ’            ‘等级’‘个人声望’‘个人气势’‘加入’", 0);
#else
            StockFontBuffer(winX + 24, winY + 21 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, " ‘状态’ ‘ 成 员 姓 名 ’ ‘等级’ ‘个人声望’ ‘加入’‘服务器’", 0);
#endif
#endif
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    if (i < selStartLine)
                    {
                        StockFontBuffer(winX + 34, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
                    }
                    else if ((i - selStartLine + 1) == 11)
                    {
                        int Font_Color = FONT_PAL_GREEN;
                        if (FMmsgWN[i + 1] == 1)
                            Font_Color = FONT_PAL_GREEN;
                        else if (FMmsgWN[i + 1] == 0)
                            Font_Color = FONT_PAL_RED;
                        StockFontBuffer(winX + 34, winY + 31 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
                    }
                    else
                    {
                        int Font_Color = FONT_PAL_WHITE;
                        if (FMmsgWN[i + 1] == 2)
                            Font_Color = FONT_PAL_BLUE2;
                        else if (FMmsgWN[i + 1] == -1)
                            Font_Color = FONT_PAL_RED;

#ifdef _NEWFONT_
                        extern void ltrim(char *str);
                        if (i != 10)
                        {
                            char strtemp[128];
                            memcpy(strtemp, msgWN[i], 7);
                            strtemp[7] = 0;
                            StockFontBuffer(winX + 34, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                            memcpy(strtemp, msgWN[i] + 9, 18);
                            strtemp[18] = 0;
                            StockFontBuffer(winX + 100, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                            memcpy(strtemp, msgWN[i] + 27, 4);
                            strtemp[4] = NULL;
                            StockFontBuffer(winX + 258, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                            memcpy(strtemp, msgWN[i] + 31, 12);
                            strtemp[12] = NULL;
                            ltrim(strtemp);
                            StockFontBuffer(winX + 300, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                            memcpy(strtemp, msgWN[i] + 43, 12);
                            strtemp[12] = NULL;
                            ltrim(strtemp);
                            StockFontBuffer(winX + 385, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                            memcpy(strtemp, msgWN[i] + 56, 8);
                            strtemp[8] = NULL;
                            StockFontBuffer(winX + 480, winY + 20 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, strtemp, 0);
                        }
                        else
                        {
                            StockFontBuffer(winX + 34, winY + 31 + (i + 3) * 21,
                                            FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
                        }
#else
                        StockFontBuffer(winX + 34, winY + 31 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
#endif
                    }
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}
#else
/*----------  Shan (BEGIN) ----------*/
/*---------  AD 2001/05/31  ---------*/
// shan Family MemberList
void initFMWindowType(char *data)
{
    makeStringFromEscaped(data);

    selStartLine = 0;
    FMdengonidex = 0;

    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 40);

    time(&FMHoldTime);

    char getstatus[3];
    char namebuf[30], lvbuf[5], statusbuf[16], onlinebuf[12], dpbuf[64];
    int int_status, onlineflag;
    char tmp[32];
    // 家族传送的行数为 11 ，前三行标题，后十行为资料(作为EVENT之用)
    // 最后一行则为‘是否继续新增家族人员的选项’
    for (int i = 0; i < 11; i++)
    {
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                strcpy(getstatus, msgWN[i] + (strlen(msgWN[i]) - 1));
                // 存的 INDEX -> 1~10
                FMmsgWN[i - (selStartLine - 1)] = atoi(getstatus);
                getStringToken(msgWN[i], '|', 1, sizeof(tmp) - 1, tmp);
                FMpidWN[i - (selStartLine - 1)] = atoi(tmp);
                getStringToken(msgWN[i], '|', 2, sizeof(FMnameWN[i - (selStartLine - 1)]) - 1,
                               FMnameWN[i - (selStartLine - 1)]);
                // 处理字串
                if (i < 10)
                {
                    getStringToken(msgWN[i], '|', 2, sizeof(namebuf) - 1, namebuf);
                    getStringToken(msgWN[i], '|', 3, sizeof(lvbuf) - 1, lvbuf);
                    getStringToken(msgWN[i], '|', 4, sizeof(onlinebuf) - 1, onlinebuf);
                    onlineflag = atoi(onlinebuf);
                    if (onlineflag > 0)
                        strcpy(onlinebuf, "OnLine");
                    else
                        strcpy(onlinebuf, "OffLine");
                    getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                    getStringToken(msgWN[i], '|', 6, sizeof(statusbuf) - 1, statusbuf);
                    sprintf_s(msgWN[i], "%-12s%-18s%-7s%9s%5s", onlinebuf, namebuf, lvbuf, dpbuf, statusbuf);
                }
                else
                {
                    getStringToken(msgWN[i], '|', 1, sizeof(namebuf) - 1, namebuf);
                    getStringToken(msgWN[i], '|', 2, sizeof(tmp) - 1, tmp);
                    FMdengonidex = atoi(tmp);
                    getStringToken(msgWN[i], '|', 3, sizeof(statusbuf) - 1, statusbuf);
                    int_status = atoi(statusbuf);
                    switch (int_status)
                    {
                    case 1: // 继续召募
                        strcpy(statusbuf, "继续召募");
                        break;
                    case 0: // 中止召募
                        strcpy(statusbuf, "中止召募");
                        break;
                    default:
                        break;
                    }
                    sprintf_s(msgWN[i], " %-20s                         %s", namebuf, statusbuf);
                }
            }
        }
    }
}

/*----------  Shan (BEGIN) ----------*/
/*---------  AD 2001/05/31  ---------*/
// 家族人员列表
int fmYesNoproc = 0; // delete number
int fmYesNoRet = 0;
void FMWindowType(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static int fmdelid;
    char getstatus[3];
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    char buf[1024];

    if (fmYesNoproc == 1)
    {
        fmYesNoRet = fmYesNoWindow(170, 170, fmdelid);
        return;
    }
    if (fmYesNoRet != 0)
        ptActMenuWin = NULL;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 8;
        winX = (lpDraw->XSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 10; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 335;
            }
        }
        msgLine = 11;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;

        if (fmYesNoRet != 0)
        {
            if (fmYesNoRet == 1)
            {
                id2 = fmdelid;
                FMmsgWN[id2] = 4;
            }
            fmYesNoRet = 0;
        }

        if (ptActMenuWin->hp >= 1)
        {
            for (i = selStartLine; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    if (i == 10)
                    {
                        x1 = winX + 25;
                        y1 = winY + 28 + (i + 3) * 21;
                        x2 = x1 + 405;
                        y2 = y1 + 20;
                    }
                    else
                    {
                        x1 = winX + 364;
                        y1 = winY + 18 + (i + 3) * 21;
                        x2 = x1 + 70;
                        y2 = y1 + 20;
                    }

                    // 只有族长才可以修改

                    if (pc.familyleader == 1 &&
                            FMmsgWN[i - selStartLine + 1] > 0 &&
                            FMmsgWN[i - selStartLine + 1] < 4 &&
                            FMpidWN[i - selStartLine + 1] != 0 ||
                        (i - selStartLine + 1) == 11 &&
                            pc.familyleader == 1)
                        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                        {
                            time_t now_time;
                            time(&now_time);
                            if (mouse.onceState & MOUSE_LEFT_CRICK && (now_time - FMHoldTime) >= 1)
                            {
                                time(&FMHoldTime);
                                id2 = i - selStartLine + 1;

                                if (id2 >= 1 && id2 <= 10)
                                    switch (FMmsgWN[id2])
                                    {
                                    case 1: // 已 加 入
                                        fmYesNoproc = 1;
                                        fmdelid = id2;
                                        id2 = -1;
                                        DeathAction(ptActMenuWin);
                                        break;
                                    case 2: // 申请加入
                                        FMmsgWN[id2] = 1;
                                        break;
                                    case 3: // 申请退出
                                        fmYesNoproc = 1;
                                        fmdelid = id2;
                                        id2 = -1;
                                        DeathAction(ptActMenuWin);
                                        break;
                                    case 4: // 退    出
                                        break;
                                    default:
                                        break;
                                    }
                                else if (id2 == 11)
                                    switch (FMmsgWN[id2])
                                    {
                                    case 1: // YES
                                        FMmsgWN[id2] = 0;
                                        break;
                                    case 0: // N O
                                        FMmsgWN[id2] = 1;
                                        break;
                                    default:
                                        break;
                                    }
                            }
                        }
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 12 || id == 100)
        {
            char data[256];
            char msg[256];
            if (id == 100)
            {
                btn = WINDOW_BUTTONTYPE_CANCEL;
            }
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
            {
                btn = 0;
            }
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d|%d", FMdengonidex, id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);

            if (1 <= id2 && id2 < 11)
            {
                sprintf_s(buf, "M|%s|%d|%d", FMnameWN[id2], FMpidWN[id2], FMmsgWN[id2]);
                    lssproto_FM_send(sockfd, buf);
            }
            if (id2 == 11)
            {
                sprintf_s(buf, "T|%d", FMmsgWN[id2]);
                    lssproto_FM_send(sockfd, buf);
            }
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 34, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "              ‘家族成员列表’", 0);
            StockFontBuffer(winX + 24, winY + 21 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, " ‘状态’ ‘ 成 员 姓 名 ’ ‘等级’ ‘个人声望’ ‘加入’", 0);
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    if (i < selStartLine)
                    {
                        StockFontBuffer(winX + 34, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
                    }
                    else if ((i - selStartLine + 1) == 11)
                    {
                        int Font_Color = FONT_PAL_GREEN;
                        if (FMmsgWN[i + 1] == 1)
                            Font_Color = FONT_PAL_GREEN;
                        else if (FMmsgWN[i + 1] == 0)
                            Font_Color = FONT_PAL_RED;
                        StockFontBuffer(winX + 34, winY + 31 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
                    }
                    else
                    {
                        int int_status;
                        strcpy(getstatus, msgWN[i] + (strlen(msgWN[i]) - 1));
                        int_status = atoi(getstatus);
                        // 显示的范围
                        if (int_status >= 1 && int_status <= 4)
                            switch (int_status)
                            {
                            case 1: // 已 加 入
                                strcpy(msgWN[i] + (strlen(msgWN[i]) - 3), "已 加 入");
                                break;
                            case 2: // 申请加入
                                strcpy(msgWN[i] + (strlen(msgWN[i]) - 3), "申请加入");
                                break;
                            case 3: // 申请退出
                                strcpy(msgWN[i] + (strlen(msgWN[i]) - 3), "申请退出");
                                break;
                            case 4: // 退    出
                                strcpy(msgWN[i] + (strlen(msgWN[i]) - 3), "退    出");
                                break;
                            default:
                                break;
                            }
                        int_status = -1;
                        int Font_Color = FONT_PAL_WHITE;
                        if (FMmsgWN[i + 1] == 1)
                            Font_Color = FONT_PAL_WHITE;
                        else if (FMmsgWN[i + 1] == 2)
                            Font_Color = FONT_PAL_BLUE2;
                        else if (FMmsgWN[i + 1] == 3)
                            Font_Color = FONT_PAL_GRAY;
                        else if (FMmsgWN[i + 1] == 4)
                            Font_Color = FONT_PAL_RED;

                        StockFontBuffer(winX + 34, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
                    }
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}
#endif

// shan Family Dengon
#define IS_2BYTEWORD(_a_) ((char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF))
void initFMWindowType1(char *data)
{
    char m_buf[128], m_buf1[512];
    FMdengonidex = 0;
    msgWN_W = 9;
    msgWN_H = 9;
    msgWNLen = (msgWN_W * 64 - 48) / 9;
    makeStringFromEscaped(data);
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), msgWNLen + 40);
    FMdengonidex = atoi(msgWN[7]);
    for (int i = 7; i < 14; i++)
    {
        if (strlen(msgWN[i - 7]) > 0)
        {
            if (i >= selStartLine)
            {
                int n = 0;
                char *p = msgWN[i - 7];
                for (size_t j = 0; j < strlen(msgWN[i - 7]); j++)
                {
                    if (IS_2BYTEWORD(*(p + j)) == 1)
                    {
                        j++;
                        continue;
                    }
                    else if (*(p + j) == '|')
                        n++;
                }
                getStringToken(msgWN[i - 7], '|', (n + 1), sizeof(msgWN[i]) - 1, msgWN[i]);
                getStringToken(msgWN[i - 7], '|', 1, sizeof(m_buf1) - 1, m_buf1);
                for (int k = 2; k < (n + 1); k++)
                {
                    getStringToken(msgWN[i - 7], '|', k, sizeof(m_buf) - 1, m_buf);
                    strcat_s(m_buf1, "|");
                    strcat_s(m_buf1, m_buf);
                }
                strcpy(msgWN[i - 7], m_buf1);
            }
        }
    }
}

// 家族留言板
void FMWindowType1(void)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static STR_BUFFER input;
    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        winW = msgWN_W;
        winH = msgWN_H;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        lineSkip = 20;
        totalMsgLine = (winH * 48 - 56) / lineSkip;

        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
        {
            fontId[i] = -2;
        }

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = ((winH * 48 - 56) / totalMsgLine) * (totalMsgLine - 1) + (winH * 48 - totalMsgLine * lineSkip) / 2 + 8;
            }
        }
        msgLine = totalMsgLine - 2;
        input.buffer[0] = '\0';
        input.cnt = 0;
        input.cursor = 0;
        initStrBuffer(&input,
                      winX + (msgWN_W * 64 - msgWNLen * 9) / 2,
                      winY + ((winH * 48 - 56) / totalMsgLine) * (totalMsgLine - 2) + (winH * 48 - totalMsgLine * lineSkip) / 2,
                      msgWNLen, FONT_PAL_WHITE, FONT_PRIO_FRONT);
        GetKeyInputFocus(&input);
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));

        if (CheckMenuFlag() || ((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        // OK
        if (id == 0 && strcmp(input.buffer, "") != 0 || id != 0)
            if (0 <= id && id < 6)
            {
                char msg[256], msg1[256];
                btn = 1;
                btn <<= id;

                makeEscapeString(input.buffer, msg1, sizeof(msg1) - 1);
                sprintf_s(msg, "%d|%s", FMdengonidex, msg1);
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
                windowTypeWN = -1;
            }
        if (id >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            GetKeyInputFocus(&MyChatBuffer);
            return;
        }
        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 200,
                            winY + 25,
                            FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘家 族 留 言 板’", 0);
            for (i = 0; i < 7; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    StockFontBuffer(winX + (msgWN_W * 64 - msgWNLen * 9) / 2,
                                    winY + ((winH * 48 - 56) / totalMsgLine) * (i * 2) + (winH * 48 - totalMsgLine * lineSkip) / 2 + 30,
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
                }
                if (strlen(msgWN[i + 7]) > 0)
                {
                    StockFontBuffer(winX + (msgWN_W * 64 - msgWNLen * 9) / 2 + 300,
                                    winY + ((winH * 48 - 56) / totalMsgLine) * (i * 2 + 1) + (winH * 48 - totalMsgLine * lineSkip) / 2 + 30,
                                    FONT_PRIO_FRONT, FONT_PAL_YELLOW, msgWN[i + 7], 0);
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
            StockFontBuffer2(&input);
        }
    }
}

// shan Family Point
void initFMWindowType2(char *data)
{
    makeStringFromEscaped(data);

    selStartLine = 0;
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 100);

    char getstatus[4];
    char buf0[16], buf1[8], buf2[8], buf3[20], buf4[20];
    ;
    char tmp[8];
    int tmp_i;

#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
    for (int i = 0; i < fm_point_num; i++)
    {
#else
    for (int i = 0; i < 4; i++)
    {
#endif
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                getStringToken(msgWN[i], '|', 5, sizeof(getstatus) - 1, getstatus);
                FMmsgWN[i - (selStartLine - 1)] = atoi(getstatus);
                getStringToken(msgWN[i], '|', 1, sizeof(tmp) - 1, tmp);
                FMpidWN[i - (selStartLine - 1)] = atoi(tmp);
                getStringToken(msgWN[i], '|', 2, sizeof(tmp) - 1, tmp);
                getStringToken(msgWN[i], '|', 6, sizeof(buf4) - 1, buf4);

                tmp_i = atoi(tmp);
                //(tmp_i==100)?(strcpy(buf0,"萨依那斯")):(strcpy(buf0,"加 鲁 卡"));
                if (tmp_i == 100)
                    strcpy(buf0, "萨姆吉尔");
                else if (tmp_i == 200)
                    strcpy(buf0, "玛丽娜丝");
                else if (tmp_i == 300)
                    strcpy(buf0, "加    加");
                else if (tmp_i == 400)
                    strcpy(buf0, "卡鲁它那");
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
                else if (tmp_i == 500)
                    strcpy(buf0, "伊    甸");
                else if (tmp_i == 600)
                    strcpy(buf0, "塔 尔 塔");
                else if (tmp_i == 700)
                    strcpy(buf0, "尼 克 斯");
                else if (tmp_i == 800)
                    strcpy(buf0, "弗 烈 顿");
                else if (tmp_i == 900)
                    strcpy(buf0, "亚 伊 欧");
#endif
#ifdef _FIX_10_FMPOINT
                else if (tmp_i == 1000)
                    strcpy(buf0, "瑞尔亚斯");
#endif
                (FMmsgWN[i - (selStartLine - 1)] > 0) ? (strcpy(buf3, buf4)) : (strcpy(buf3, "未使用"));
                // 处理字串
                if (i < 10)
                {
                    getStringToken(msgWN[i], '|', 3, sizeof(buf1) - 1, buf1);
                    getStringToken(msgWN[i], '|', 4, sizeof(buf2) - 1, buf2);
                    sprintf_s(msgWN[i], "%9s%5s %5s%18s", buf0, buf1, buf2, buf3);
                    sprintf_s(FMnameWN[i - (selStartLine - 1)], "%s|%s|%s", tmp, buf1, buf2);
                }
            }
        }
    }
}

// 家族据点
void FMWindowType2(void)
{
    static int winX, winY;
    static int winW, winH;
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
#ifdef _FIX_10_FMPOINT
    static int btnId[] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    static int btnLoc[12][2];
#else
    static int btnId[] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    static int btnLoc[11][2];
#endif
#else
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
#endif

    static int btnCnt;
    static int msgLine;
    int id, id2;
    int i, j;
    int mask;
    int btn;
    int x1, y1, x2, y2;
    char buf[1024];
    if (ptActMenuWin == NULL)
    {
        winW = 5;
        winH = 4 + fm_point_num / 3;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
#ifdef _FIX_10_FMPOINT
        for (i = 0; i < 16; i++, mask <<= 1)
#else
        for (i = 0; i < 15; i++, mask <<= 1)
#endif
#else
        for (i = 0; i < 10; i++, mask <<= 1)
#endif
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
                if (btnCnt == fm_point_num)
#else
                if (btnCnt == 4)
#endif
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = fm_point_num * 18;
            }
        }

#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
        msgLine = fm_point_num;
#else
        msgLine = 4;
#endif
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;
        if (ptActMenuWin->hp >= 1)
        {
            for (i = selStartLine; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    x1 = winX + 30;
                    y1 = winY + 18 + (i + 3) * 21;
                    x2 = x1 + 260;
                    y2 = y1 + 20;

                    // 只有族长才可以修改
#ifdef _FMVER21
                    if (pc.familyleader == FMMEMBER_LEADER && FMmsgWN[i - selStartLine + 1] <= 0)
#else
                    if (pc.familyleader == 1 && FMmsgWN[i - selStartLine + 1] <= 0)
#endif
                        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                        {
                            if (mouse.onceState & MOUSE_LEFT_CRICK)
                            {
                                id2 = i - selStartLine + 1;
                            }
                        }
                }
            }
        }
        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
#ifdef _FIX_10_FMPOINT
        if (0 <= id && id < 16 || 0 <= id2 && id2 < 17
#else
        if (0 <= id && id < 11 || 0 <= id2 && id2 < 16
#endif
#else
        if (0 <= id && id < 6 || 0 <= id2 && id2 < 11
#endif
            || id == 100)
        {
            char data[256];
            char msg[256];
            if (id == 100)
            {
                btn = WINDOW_BUTTONTYPE_CANCEL;
            }
            else
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
#ifdef _FIX_10_FMPOINT
                if (0 <= id && id < 12)
#else
                if (0 <= id && id < 11)
#endif
#else
                if (0 <= id && id < 6)
#endif
            {
                btn = 1;
                btn <<= id;
            }
            else
            {
                btn = 0;
            }
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d", id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);

            sprintf_s(buf, "P|%d|%s", FMpidWN[id2], FMnameWN[id2]);

#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
            if (1 <= id2 && id2 <= fm_point_num + 1)
            {
#else
            if (1 <= id2 && id2 < 5)
            {
#endif
                    lssproto_FM_send(sockfd, buf);
            }

#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
            if (0 <= id && id < fm_point_num + 2)
            {
#else
            if (0 <= id && id < 6)
            {
#endif
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            }
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 34, winY + 28,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "        ‘家族据点申请列表’", 0);
            StockFontBuffer(winX + 30, winY + 18 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘地 点’‘东’‘南’        ‘状  态’", 0);
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    if (i < selStartLine)
                    {
                        StockFontBuffer(winX + 30, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0);
                    }
                    else
                    {
                        int Font_Color = FONT_PAL_WHITE;
                        if (FMmsgWN[i + 1] <= 0)
                            Font_Color = FONT_PAL_GRAY;

#ifdef _NEWFONT_
                        char strtemp[128];
                        extern void ltrim(char *str);
                        memcpy(strtemp, msgWN[i], 11);
                        strtemp[11] = NULL;
                        StockFontBuffer(winX + 30, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 11, 6);
                        strtemp[6] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 100, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 14, 6);
                        strtemp[6] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 130, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 20, 18);
                        strtemp[18] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 180, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, strtemp, 0);
#else

                        StockFontBuffer(winX + 30, winY + 21 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, Font_Color, msgWN[i], 0);
#endif
                    }
                }
            }
            j = 0;
#ifdef _FIX_9_FMPOINT // WON ADD 九大庄园
#ifdef _FIX_10_FMPOINT
            for (i = 0, mask = 1; i < 12; i++, mask <<= 1)
#else
            for (i = 0, mask = 1; i < 11; i++, mask <<= 1)
#endif
#else
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
#endif
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + 110 + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

#ifdef _FMVER21
// (member list) select duty
int fmselectdetuyWindow(int x, int y, int id)
{
    static int btnId[] = {-2, -2, -2, -2};
    static int btnLoc[4][2];
    static ACTION *ptActMenuWin = NULL;
    int ret = 0;
    int dutytobtn = -1;
    char buff[64];
    char *fmButtonList[] =
        {
            "一般族员",
            "退    出",
            "长    老",
            //"长    老",
            //"祭    司",
            //"财 务 长",
            //"副 族 长",
            "取    消" };

    if (fmYesNoproc == 1)
    {
        fmYesNoRet = fmYesNoWindow(170, 170, id);
        return 0;
    }
    if (fmYesNoRet != 0)
    {
        if (fmYesNoRet == 1)
        {
            ret = FMMEMBER_NONE;
            fmDutyproc = 0;
            fmYesNoRet = 0;
            return ret;
        }
    }

    if (ptActMenuWin == NULL)
    {
        ptActMenuWin = MakeWindowDisp(x, y, 3, 5, NULL, 1);
        for (int i = 0; i < 4; i++)
            btnId[i] = -2;
        for (int j = 0; j < 4; j++)
        {
            btnLoc[j][0] = x + 65;
            btnLoc[j][1] = y + 45 + 30 * (j + 1);
        }
        play_se(202, 320, 240);
    }

    switch (FMmsgWN[id])
    {
    case FMMEMBER_ELDER:
        dutytobtn = 2;
        break;
    default:
        break;
    }

    sprintf_s(buff, "请选择");
    StockFontBuffer(x + 100, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, buff, 0);
    sprintf_s(buff, " %s ", FMnameWN[id]);
    StockFontBuffer(x + 145, y + 80, FONT_PRIO_FRONT, FONT_PAL_GREEN, buff, 0);
    sprintf_s(buff, "所担任的家族职务：");
    StockFontBuffer(x + 100, y + 105, FONT_PRIO_FRONT, FONT_PAL_WHITE, buff, 0);
    if (ptActMenuWin != NULL)
    {
        for (int i = 0; i < 4; i++)
            if (FMmsgWN[id] == 2 && i == 2)
            {
                StockFontBuffer(btnLoc[i][0], btnLoc[i][1], FONT_PRIO_FRONT, FONT_PAL_GRAY, fmButtonList[i], 0);
            }
            else if (pc.familyleader == FMMEMBER_ELDER && i == 2)
            {
                StockFontBuffer(btnLoc[i][0], btnLoc[i][1], FONT_PRIO_FRONT, FONT_PAL_GRAY, fmButtonList[i], 0);
            }
            else if (dutytobtn == i)
            {
                StockFontBuffer(btnLoc[i][0], btnLoc[i][1], FONT_PRIO_FRONT, FONT_PAL_GRAY, fmButtonList[i], 0);
            }
            else if (i == 3)
            {
                StockFontBuffer(btnLoc[i][0], btnLoc[i][1], FONT_PRIO_FRONT, FONT_PAL_WHITE, fmButtonList[i], 0);
            }
            else
            {
                StockFontBuffer(btnLoc[i][0], btnLoc[i][1], FONT_PRIO_FRONT, FONT_PAL_YELLOW, fmButtonList[i], 0);
            }
    }

    for (int i = 0; i < 4; i++)
    {
        int x1 = x + 130;
        int y1 = y + 30 * (i + 1) + 102;
        int x2 = x1 + 85;
        int y2 = y1 + 20;
        if (dutytobtn != i)
        {
            if (!(FMmsgWN[id] == 2 && i == 2))
            {
                if (pc.familyleader == FMMEMBER_LEADER ||
                    pc.familyleader == FMMEMBER_ELDER && (i >= 0 && i <= 1 || i == 3))
                {
                    if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
                    {
                        if (mouse.onceState & MOUSE_LEFT_CRICK)
                        {
                            switch (i)
                            {
                            case 0: // 一般成员
                                ret = FMMEMBER_MEMBER;
                                fmDutyproc = 0;
                                break;
                            case 1: // 退出
                                fmYesNoproc = 1;
                                break;
                            case 2: // 长老
                                ret = FMMEMBER_ELDER;
                                fmDutyproc = 0;
                                break;
                            case 3:
                                fmDutyproc = 0;
                                break;
                            default:
                                break;
                            }
                            DeathAction(ptActMenuWin);
                            ptActMenuWin = NULL;
                        }
                    }
                }
            }
        }
    }
    if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        fmDutyproc = 0;
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
    }
    return ret;
}
#endif

// (member list) delete member
int fmYesNoWindow(int x, int y, int id)
{
    static int btnId[] = {-2, -2};
    static int btnLoc[2][2];
    static ACTION *ptActMenuWin = NULL;
    int ret = 0;
    char buff[64];
    char *yesNoButtonList[] = { "确  定", "取  消" };

    if (ptActMenuWin == NULL)
    {
        ptActMenuWin = MakeWindowDisp(x, y, 5, 2, NULL, 1);
        for (int i = 0; i < 2; i++)
            btnId[i] = -2;
        for (int j = 0; j < 2; j++)
        {
            btnLoc[j][0] = x + 83 * (j + 1);
            btnLoc[j][1] = y + 60;
        }
        play_se(202, 320, 240);
    }

    sprintf_s(buff, "确定要让 %s 退出家族吗？", FMnameWN[id]);
    StockFontBuffer(x + 100, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, buff, 0);
    if (ptActMenuWin != NULL)
    {
        for (int i = 0; i < 2; i++)
            StockFontBuffer(btnLoc[i][0], btnLoc[i][1],
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, yesNoButtonList[i], 0);
    }

    for (int i = 0; i < 2; i++)
    {
        int x1 = x + 83 * (i + 1) - 10 + 80;
        int y1 = y + 58 + 59;
        int x2 = x1 + 60;
        int y2 = y1 + 20;
        if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
        {
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                ret = i + 1;
                fmYesNoproc = 0;
                DeathAction(ptActMenuWin);
                ptActMenuWin = NULL;
            }
        }
    }
    return ret;
}

#ifdef _FMVER21
// shan Family DpList
short lnmake;
void initFMWindowType4(char *data, int king)
{
    makeStringFromEscaped(data);

    FMdengonidex = 0;
    selStartLine = 0;
    lnmake = 0;
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 80);

    time(&FMHoldTime);

    char lvbuf[8], fmnamebuf[36], namebuf[36], dpbuf[20], makecolor[4];
    int sumdp = 0, taldp = 0;
#ifdef _NEW_MANOR_LAW
    int momentum = 0, fmmomentum = 0;
#endif

    FMdengonidex = atoi(msgWN[10]);
    for (int i = 0; i < 10; i++)
    {
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                if (king == 0)
                {
                    // 处理字串
                    getStringToken(msgWN[i], '|', 2, sizeof(lvbuf) - 1, lvbuf);
                    getStringToken(msgWN[i], '|', 3, sizeof(fmnamebuf) - 1, fmnamebuf);
                    makeStringFromEscaped(fmnamebuf);
                    getStringToken(msgWN[i], '|', 4, sizeof(namebuf) - 1, namebuf);
                    makeStringFromEscaped(namebuf);
                    getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                    sumdp = atoi(dpbuf);
                    getStringToken(msgWN[i], '|', 6, sizeof(dpbuf) - 1, dpbuf);
                    taldp = atoi(dpbuf);
                    getStringToken(msgWN[i], '|', 7, sizeof(makecolor) - 1, makecolor);
                    if (strcmp(makecolor, "1") == 0)
                        lnmake = i;

                    // printf("[ %-7s%-18s%-16s%13d%12d%14d%]\n\n", lvbuf, fmnamebuf, namebuf, sumdp, taldp-sumdp, taldp);
                    sprintf(msgWN[i], " %-7s%-18s%-16s%13d%12d%14d%", lvbuf, fmnamebuf, namebuf, sumdp, taldp - sumdp, taldp);
                }
                else if (king == 1)
                {
#ifdef _NEW_MANOR_LAW
                    getStringToken(msgWN[i], '|', 2, sizeof(lvbuf) - 1, lvbuf);
                    getStringToken(msgWN[i], '|', 3, sizeof(fmnamebuf) - 1, fmnamebuf);
                    makeStringFromEscaped(fmnamebuf);
                    getStringToken(msgWN[i], '|', 4, sizeof(namebuf) - 1, namebuf);
                    makeStringFromEscaped(namebuf);
                    getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                    fmmomentum = atoi(dpbuf);
                    getStringToken(msgWN[i], '|', 6, sizeof(dpbuf) - 1, dpbuf);
                    momentum = atoi(dpbuf);
                    sprintf(msgWN[i], "%-7s%-16s%-16s%8d%12d", lvbuf, fmnamebuf, namebuf, fmmomentum, momentum);
#endif
                }
            }
        }
    }
}

// 家族强者表
#ifdef _NEW_MANOR_LAW
void FMWindowType4(int king)
#else
void FMWindowType4(void)
#endif
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static int fmdelid;
    int id, id2;
    int i, j;
    int mask;
    int btn;

    if (ptActMenuWin == NULL)
    {
#ifdef _NEW_MANOR_LAW
        if (king == 0)
        {
            winW = 10;
            winH = 7;
        }
        else if (king == 1)
        {
            winW = 8;
            winH = 7;
        }
#else
        winW = 10;
        winH = 7;
#endif
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 10; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 295;
            }
        }
        msgLine = 10;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 12 || id == 100)
        {
            if (id == 100)
                btn = WINDOW_BUTTONTYPE_CANCEL;
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
                btn = 0;
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d|%d", FMdengonidex, id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);

                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
#ifdef _NEW_MANOR_LAW
            if (king == 0)
            {
                StockFontBuffer(winX + 235, winY + 21,
                                FONT_PRIO_FRONT, FONT_PAL_YELLOW, "‘史上最强之家族强者排行榜’", 0);
                StockFontBuffer(winX + 25, winY + 16 + 2 * 21,
                                FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘名次’ ‘家  族  名  称’ ‘族  长  名  称’ ‘综合声望’‘成员声望’‘家族总声望’", 0);
            }
            else if (king == 1)
            {
                StockFontBuffer(winX + 155, winY + 21,
                                FONT_PRIO_FRONT, FONT_PAL_YELLOW, "‘史上最强之家族强者排行榜’", 0);
                StockFontBuffer(winX + 25, winY + 16 + 2 * 21,
                                FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘名次’‘家 族 名 称’     ‘族 长 名 称’    ‘家族总气势’", 0);
            }
#else
            StockFontBuffer(winX + 235, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "‘史上最强之家族强者排行榜’", 0);
            StockFontBuffer(winX + 25, winY + 16 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘名次’ ‘家  族  名  称’ ‘族  长  名  称’ ‘综合声望’‘成员声望’‘家族总声望’", 0);
#endif
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    int fontcolor = FONT_PAL_WHITE;
                    if (lnmake != 0)
                    {
                        fontcolor = FONT_PAL_GRAY;
                        if (lnmake == i)
                            fontcolor = FONT_PAL_WHITE;
                    }
                    if (i < selStartLine)
                    {
#ifdef _NEW_MANOR_LAW
                        if (king == 0)
                            StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
                        else if (king == 1)
                            StockFontBuffer(winX + 60, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
#else
                        StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
#endif
                    }
#ifdef _NEW_MANOR_LAW
#ifdef _NEWFONT_
                    extern void ltrim(char *str);
                    char strtemp[128];
                    if (strlen(msgWN[i]) >= 77)
                    {
                        memcpy(strtemp, msgWN[i], 8);
                        strtemp[8] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 8, 26 - 8);
                        strtemp[26 - 8] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 85, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 26, 50 - 26);
                        strtemp[50 - 26] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 220, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 50, 59 - 50);
                        strtemp[59 - 50] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 350, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 59, 73 - 59);
                        strtemp[73 - 59] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 440, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 73, 81 - 73);
                        strtemp[81 - 73] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 540, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);
                    }
                    else
                    {
                        memcpy(strtemp, msgWN[i], 7);
                        strtemp[7] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 7, 23 - 7);
                        strtemp[23 - 7] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 85, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 23, 42 - 23);
                        strtemp[42 - 23] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 220, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                        memcpy(strtemp, msgWN[i] + 42, 57 - 42);
                        strtemp[57 - 42] = NULL;
                        ltrim(strtemp);
                        StockFontBuffer(winX + 350, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);
                    }

#else
                    if (king == 0)
                        StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
                    else if (king == 1)
                        StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
#endif
#else
                    StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
#endif
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}

void initFMWindowType3(char *data)
{
    makeStringFromEscaped(data);

    FMdengonidex = 0;
    selStartLine = 0;
    lnmake = 0;
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 50);

    time(&FMHoldTime);

    char lvbuf[8], fmnamebuf[36], namebuf[36], dpbuf[20], makecolor[4];

    FMdengonidex = atoi(msgWN[10]);
    for (int i = 0; i < 10; i++)
    {
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                // 处理字串
                getStringToken(msgWN[i], '|', 2, sizeof(lvbuf) - 1, lvbuf);
                getStringToken(msgWN[i], '|', 3, sizeof(fmnamebuf) - 1, fmnamebuf);
                makeStringFromEscaped(fmnamebuf);
                getStringToken(msgWN[i], '|', 4, sizeof(namebuf) - 1, namebuf);
                makeStringFromEscaped(namebuf);
                getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                getStringToken(msgWN[i], '|', 6, sizeof(makecolor) - 1, makecolor);
                if (strcmp(makecolor, "1") == 0)
                    lnmake = i;
                sprintf_s(msgWN[i], " %-9s%-19s%-17s%12s", lvbuf, fmnamebuf, namebuf, dpbuf);
            }
        }
    }
}

// 家族强者表 top30
#ifdef _NEW_MANOR_LAW
void FMWindowType3(int king)
#else
void FMWindowType3(void)
#endif
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static int fmdelid;
#ifdef _NEW_MANOR_LAW
    char szMessage[128];
#endif
    int id, id2;
    int i, j;
    int mask;
    int btn;

    if (ptActMenuWin == NULL)
    {
        winW = 8;
        winH = 7;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++) btnId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 10; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 295;
            }
        }
        msgLine = 10;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 12 || id == 100)
        {
            if (id == 100)
            {
                btn = WINDOW_BUTTONTYPE_CANCEL;
            }
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
            {
                btn = 0;
            }
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d|%d", FMdengonidex, id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);

                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 34, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "                  ‘史上最强之家族强者排行榜’", 0);

#ifdef _NEW_MANOR_LAW
#ifdef _NEWFONT_
            if (king == 0)
                sprintf_s(szMessage, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族声望值’");
            else if (king == 1)
                sprintf_s(szMessage, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族气势值’");
            StockFontBuffer(winX + 20, winY + 16 + 2 * 21, FONT_PRIO_FRONT, FONT_PAL_GREEN, szMessage, 0);
#else
            if (king == 0)
                sprintf_s(szMessage, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族声望值’");
            else if (king == 1)
                sprintf_s(szMessage, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族气势值’");
            StockFontBuffer(winX + 39, winY + 16 + 2 * 21, FONT_PRIO_FRONT, FONT_PAL_GREEN, szMessage, 0);
#endif
#else
            StockFontBuffer(winX + 39, winY + 16 + 2 * 21, FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族声望值’", 0);
#endif
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    int fontcolor = FONT_PAL_WHITE;
                    if (lnmake != 0)
                    {
                        fontcolor = FONT_PAL_GRAY;
                        if (lnmake == i)
                            fontcolor = FONT_PAL_WHITE;
                    }
                    if (i < selStartLine)
                    {
                        StockFontBuffer(winX + 58, winY + 14 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
                    }
#ifdef _NEWFONT_
                    extern void ltrim(char *str);
                    char strtemp[128];
                    memcpy(strtemp, msgWN[i], 10);
                    strtemp[10] = NULL;
                    ltrim(strtemp);
                    StockFontBuffer(winX + 40, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                    memcpy(strtemp, msgWN[i] + 10, 29 - 10);
                    strtemp[29 - 10] = NULL;
                    ltrim(strtemp);
                    StockFontBuffer(winX + 85, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                    memcpy(strtemp, msgWN[i] + 29, 53 - 29);
                    strtemp[53 - 29] = NULL;
                    ltrim(strtemp);
                    StockFontBuffer(winX + 220, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);

                    memcpy(strtemp, msgWN[i] + 53, 58 - 53);
                    strtemp[58 - 53] = NULL;
                    ltrim(strtemp);
                    StockFontBuffer(winX + 350, winY + 14 + (i + 3) * 21, FONT_PRIO_FRONT, fontcolor, strtemp, 0);
#else

                    StockFontBuffer(winX + 58, winY + 14 + (i + 3) * 21,
                                    FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
#endif
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}
#else
short lnmake;
void initFMWindowType3(char *data)
{
    makeStringFromEscaped(data);

    FMdengonidex = 0;
    selStartLine = 0;
    lnmake = 0;
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), 50);

    time(&FMHoldTime);

    char lvbuf[8], fmnamebuf[36], namebuf[36], dpbuf[20], makecolor[4];

    FMdengonidex = atoi(msgWN[10]);
    for (int i = 0; i < 10; i++)
    {
        if (strlen(msgWN[i]) > 0)
        {
            if (i >= selStartLine)
            {
                // 处理字串
                getStringToken(msgWN[i], '|', 2, sizeof(lvbuf) - 1, lvbuf);
                getStringToken(msgWN[i], '|', 3, sizeof(fmnamebuf) - 1, fmnamebuf);
                makeStringFromEscaped(fmnamebuf);
                getStringToken(msgWN[i], '|', 4, sizeof(namebuf) - 1, namebuf);
                makeStringFromEscaped(namebuf);
                getStringToken(msgWN[i], '|', 5, sizeof(dpbuf) - 1, dpbuf);
                getStringToken(msgWN[i], '|', 6, sizeof(makecolor) - 1, makecolor);
                if (strcmp(makecolor, "1") == 0)
                    lnmake = i;
                sprintf_s(msgWN[i], " %-9s%-19s%-17s%12s", lvbuf, fmnamebuf, namebuf, dpbuf);
            }
        }
    }
}

// 家族强者表
void FMWindowType3(void)
{
    static int winX, winY;
    static int winW, winH;
    static int btnId[] = {-2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int msgLine;
    static int fmdelid;
    int id, id2;
    int i, j;
    int mask;
    int btn;

    if (ptActMenuWin == NULL)
    {
        winW = 8;
        winH = 7;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
        {
            btnId[i] = -2;
        }
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 10; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = 295;
            }
        }
        msgLine = 10;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(btnId, sizeof(btnId) / sizeof(int));

        id2 = -1;

        if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
        }

        if (0 <= id && id < 6 || 0 <= id2 && id2 < 12 || id == 100)
        {
            if (id == 100)
            {
                btn = WINDOW_BUTTONTYPE_CANCEL;
            }
            else if (0 <= id && id < 6)
            {
                btn = 1;
                btn <<= id;
            }
            else
            {
                btn = 0;
            }
            if (id2 < 0)
                id2 = 0;
            sprintf_s(data, "%d|%d", FMdengonidex, id2);
            makeEscapeString(data, msg, sizeof(msg) - 1);

                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, msg);
            windowTypeWN = -1;
        }
        if (id >= 0 || id2 >= 0)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 34, winY + 21,
                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, "                  ‘史上最强之家族强者排行榜’", 0);
            StockFontBuffer(winX + 39, winY + 16 + 2 * 21,
                            FONT_PRIO_FRONT, FONT_PAL_GREEN, "‘名 次’ ‘家  族  名  称’ ‘族  长  名  称’  ‘家族声望值’", 0);
            for (i = 0; i < msgLine; i++)
            {
                if (strlen(msgWN[i]) > 0)
                {
                    int fontcolor = FONT_PAL_WHITE;
                    if (lnmake != 0)
                    {
                        fontcolor = FONT_PAL_GRAY;
                        if (lnmake == i)
                            fontcolor = FONT_PAL_WHITE;
                    }
                    if (i < selStartLine)
                    {
                        StockFontBuffer(winX + 58, winY + 14 + (i + 3) * 21,
                                        FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
                    }
                    StockFontBuffer(winX + 58, winY + 14 + (i + 3) * 21,
                                    FONT_PRIO_FRONT, fontcolor, msgWN[i], 0);
                }
            }
            j = 0;
            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                btnId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        btnId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }
        }
    }
}
#endif

// 删除人物输入密码的视窗
// return:        0 ... 处理中
//                1 ... 按下OK
//                2 ... 按下Cancel
int DelCharGraColorWin(void)
{
    static int fontId[] = {-2, -2};
    static BOOL bErr = FALSE;
    static ACTION *ptActMenuWin = NULL;
    int id;
    int i;
    static int x, y, w, h;
    int ret = 0;
    char msg[][16] = { " 确定 ", " 取消 "};
    // 初始化
    if (selCharGraColorWinProcNo == 0)
    {
        selCharGraColorWinProcNo = 1;
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
        // 视窗制作
        w = 3;
        h = 2;
        x = 250;
        y = 180;
        ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
        initStrBuffer(&passwd, x + 20, y + 44, 20, FONT_PAL_WHITE, FONT_PRIO_FRONT);
        // 设定输入密码的StrBuffer
        passwd.cnt = 0;
        passwd.cursor = 0;
        passwd.filterFlag = BLIND_TYPE;
        GetKeyInputFocus(&passwd);
        bErr = FALSE;
    }
    // 判断是否按下文字
    id = selFontId(fontId, sizeof(fontId) / sizeof(int));
    if (id == 0) // 按确定
    {
        ret = 1;
        bErr = FALSE;
        play_se(217, 320, 240);
    }
    else if (id == 1) // 按取消
    {
        ret = 2;
        play_se(217, 320, 240); // click声
    }
    // 已经输入结果
    if (ret != 0)
    {
        if (ptActMenuWin)
        {
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }
    }
    // 是否指向文字
    id = focusFontId(fontId, sizeof(fontId) / sizeof(int));
    // 确定
    if (id == 0)
        ShowBottomLineString(FONT_PAL_WHITE, "删除人物请先输入密码");
    else
        // 取消
        if (id == 1)
            ShowBottomLineString(FONT_PAL_WHITE, "取消删除人物");
    if (ptActMenuWin && ptActMenuWin->hp >= 1)
    {
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = StockFontBuffer(x + 20 + i * 48, y + 64,
                                        FONT_PRIO_FRONT, FONT_PAL_YELLOW, msg[i], 2);
        if (bErr)
            StockFontBuffer(x + 20, y + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, "输入错误请再试", 0);
        else
            StockFontBuffer(x + 20, y + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, "请输入安全码", 0);
        StockFontBuffer2(&passwd);
        FlashKeyboardCursor();
    }
    return ret;
}

// Robin 0712
struct FMPKData
{
    char time[6];
    char atkFMName[20];
    char defFMName[20];
    int readyTime;
    int member;
    int flag;
    int win;
};

FMPKData FMPKDataList[4];
int serverTime;
const char winStr[][16] = { "乱斗生存战", "满场打飞战"};

void initFMPKListWN(char *data)
{
    int i;
    char buf2[256];

    getStringToken(data, '|', 1, sizeof(buf2) - 1, buf2);
    serverTime = atoi(buf2);

    for (i = 0; i < 4; i++)
    {
        getStringToken(data, '|', i * 7 + 2, sizeof(buf2) - 1, buf2);
        strcpy(FMPKDataList[i].time, buf2);
        getStringToken(data, '|', i * 7 + 3, sizeof(buf2) - 1, buf2);
        strcpy(FMPKDataList[i].atkFMName, makeStringFromEscaped(makeStringFromEscaped(buf2)));
        getStringToken(data, '|', i * 7 + 4, sizeof(buf2) - 1, buf2);
        strcpy(FMPKDataList[i].defFMName, makeStringFromEscaped(makeStringFromEscaped(buf2)));
        getStringToken(data, '|', i * 7 + 5, sizeof(buf2) - 1, buf2);
        FMPKDataList[i].readyTime = atoi(buf2);
        getStringToken(data, '|', i * 7 + 6, sizeof(buf2) - 1, buf2);
        FMPKDataList[i].member = atoi(buf2);
        getStringToken(data, '|', i * 7 + 7, sizeof(buf2) - 1, buf2);
        FMPKDataList[i].flag = atoi(buf2);
        getStringToken(data, '|', i * 7 + 8, sizeof(buf2) - 1, buf2);
        FMPKDataList[i].win = atoi(buf2);
    }
}

void FMPKListWN(int mode)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    // static int msgLine;
    // static STR_BUFFER input;
    char dataBuf[512];
    char dataBuf2[256];
    char buf[256];

    char *dataStatusStr[] =
        {
            " 新  增 ",
            " 修  改 ",
            " 接  受 ",
            "设 定 中",
            "等待回答",
            "已 排 定"};
    int showcolor;
    int pushStatus;
    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        winW = 8;
        winH = 8;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++) fontId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }

        // ?????????????
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = winH * 48 - 40;
            }
        }

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));

        // ?????????????????????
        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }

        // ＯＫ
        if (0 <= id && id < 6)
        {
            btn = 1;
            btn <<= id;
            // makeEscapeString( input.buffer, msg, sizeof( msg )-1 );
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, "");
            windowTypeWN = -1;
        }
        else if ((6 <= id) && (id < 10))
        {

            int dataSel = id - 6;
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN,
                                 WINDOW_BUTTONTYPE_OK, FMPKDataList[dataSel].time);
        }

        if (id >= 0)
        {
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {

            j = 0;

            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }

            StockFontBuffer(winX + 200, winY + 30, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "家族对战时间表", 0);
            sprintf_s(dataBuf, "目前时间： %d:%d", serverTime / 100, serverTime % 100);
            StockFontBuffer(winX + 360, winY + 45, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            dataBuf, 0);
            // StockDispHLine( winX+80, winY+30, 2 );

            sprintf_s(dataBuf, "%10s  %16s  %16s  %8s  %8s", "开战时间", "挑战家族", "受邀家族", "准备时间", "参加人数");

            StockFontBuffer(winX + 20, winY + 70,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            for (i = 0; i < 4; i++)
            {
                // char min[3], sec[3];
                int pktime = atoi(FMPKDataList[i].time);
                if (pktime >= 10000)
                {
                    // if( strlen(FMPKDataList[i].time) > 4 ) {
                    // sscanf_s(FMPKDataList[i].time+1, "%2s%2s", min, sec);
                    // sprintf_s( buf, "明天 %d:%d", (pktime-10000)/100, pktime%100 );
                    sprintf_s(buf, "明天 %d:00", (pktime - 10000) / 100);
                    // sprintf_s( buf, "明天 %s:%s", min, sec );
                }
                else
                {
                    // sscanf_s(FMPKDataList[i].time, "%2s%2s", min, sec);
                    // sprintf_s( buf, "今天 %d:%d", pktime/100, pktime%100 );
                    sprintf_s(buf, "今天 %d:00", pktime / 100);
                    // sprintf_s( buf, "今天 %s:%s", min, sec );
                }
#ifdef _READ16BITBMP
                if (!g_bUseAlpha)
                    StockDispHLine(winX + 36, winY + 90 + (i * 60), 7);
#endif

                if (FMPKDataList[i].flag != -1)
                {
                    sprintf_s(dataBuf, "%10s  %16s  %16s  %4d分钟  %6d人", buf, FMPKDataList[i].atkFMName, FMPKDataList[i].defFMName, FMPKDataList[i].readyTime, FMPKDataList[i].member);
                    sprintf_s(dataBuf2, "胜利条件： %s", winStr[FMPKDataList[i].win]);

                    switch (FMPKDataList[i].flag)
                    {
                    case -1:
                        showcolor = FONT_PAL_WHITE;
                        pushStatus = 2;
                        break;
                    case 0:
                    case 1:
                        showcolor = FONT_PAL_YELLOW;
                        pushStatus = 2;
                        break;
                    case 2:
                    case 3:
                        showcolor = FONT_PAL_GREEN;
                        pushStatus = 0;
                        break;
                    case 4:
                        showcolor = FONT_PAL_RED;
                        pushStatus = 0;
                        break;
                    }
#ifdef _FMVER21
                    if (pc.familyleader != FMMEMBER_LEADER)
                        pushStatus = 0;
#else
                    if (pc.familyleader != 1)
                        pushStatus = 0;
#endif
                    StockFontBuffer(winX + 20, winY + 106 + (i * 60),
                                    FONT_PRIO_FRONT, showcolor, dataBuf, 0);
                    StockFontBuffer(winX + 50, winY + 130 + (i * 60),
                                    FONT_PRIO_FRONT, showcolor, dataBuf2, 0);

                    fontId[i + 6] = StockFontBuffer(winX + 400, winY + 100 + (i * 60) + 30,
                                                    FONT_PRIO_FRONT, showcolor,
                                                    dataStatusStr[FMPKDataList[i].flag + 1], pushStatus);
                }
                else
                {
                    sprintf_s(dataBuf, "%10s", buf);
                    StockFontBuffer(winX + 20, winY + 106 + (i * 60),
                                    FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

#ifdef _FMVER21
                    if (pc.familyleader != FMMEMBER_LEADER)
                        pushStatus = 0;
#else
                    if (pc.familyleader != 1)
                        pushStatus = 0;
#endif
                    else
                        pushStatus = 2;
                    fontId[i + 6] = StockFontBuffer(winX + 400, winY + 100 + (i * 60) + 30,
                                                    FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                                    dataStatusStr[0], pushStatus);
                }

            } // for List end
        }
    }
}

struct FMPKSelect
{
    char fmname[16 + 1];
    int fmindex;
};
FMPKSelect FMPKSelectData[8];
int FMPKSelectNum;

void initFMPKSelectWN(char *data)
{

    int i;
    char buf[256], buf2[256];

    getStringToken(data, '|', 1, sizeof(buf) - 1, buf);
    FMPKSelectNum = atoi(buf);

    for (i = 0; i < FMPKSelectNum; i++)
    {

        getStringToken(data, '|', i * 2 + 2, sizeof(buf2) - 1, buf2);
        FMPKSelectData[i].fmindex = atoi(buf2);
        getStringToken(data, '|', i * 2 + 3, sizeof(buf2) - 1, buf2);
        strcpy(FMPKSelectData[i].fmname, makeStringFromEscaped(buf2));
    }
}

void FMPKSelectWN(int mode)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2, -2, -2,
                           -2, -2, -2, -2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    // static int msgLine;
    // static STR_BUFFER input;
    char dataBuf[256];
    char buf[256];
    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        // winW = msgWN_W;
        // winH = msgWN_H;
        winW = 4;
        winH = 8;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++) fontId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }

        // ?????????????
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = winH * 48 - 56;
            }
        }

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));

        // ?????????????????????
        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }

        // ＯＫ
        if (0 <= id && id < 6)
        {
            btn = 1;
            btn <<= id;
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, "");
            windowTypeWN = -1;
        }
        else if ((6 <= id) && (id < 6 + 8))
        {

            int dataSel = id - 6;
            sprintf_s(dataBuf, "%d|%s", FMPKSelectData[dataSel].fmindex,
                      makeEscapeString(FMPKSelectData[dataSel].fmname, buf, sizeof(buf)));
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN,
                                 WINDOW_BUTTONTYPE_OK, dataBuf);
        }

        if (id >= 0)
        {
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {

            j = 0;

            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }

            sprintf_s(dataBuf, "%s", "请选择想挑战的家族：");
            StockFontBuffer(winX + 30, winY + 40,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            for (i = 0; i < FMPKSelectNum; i++)
            {
                sprintf_s(dataBuf, "%-16s", FMPKSelectData[i].fmname);

                fontId[i + 6] = StockFontBuffer(winX + 40, winY + 60 + (i * 30) + 30,
                                                FONT_PRIO_FRONT, FONT_PAL_WHITE,
                                                dataBuf, 2);

            } // for List end
        }
    }
}

struct FMPKDetail
{
    char time[4 + 1];
    char atkFMName[16 + 1];
    char defFMName[16 + 1];
    int readyTime;
    int member;
    int win;
};
FMPKDetail FMPKDetailData;

void initFMPKDetailWN(char *data)
{

    char buf2[256];

    getStringToken(data, '|', 1, sizeof(buf2) - 1, buf2);
    strcpy(FMPKDetailData.time, makeStringFromEscaped(buf2));
    getStringToken(data, '|', 2, sizeof(buf2) - 1, buf2);
    strcpy(FMPKDetailData.atkFMName, makeStringFromEscaped(buf2));
    getStringToken(data, '|', 3, sizeof(buf2) - 1, buf2);
    strcpy(FMPKDetailData.defFMName, makeStringFromEscaped(buf2));
    getStringToken(data, '|', 4, sizeof(buf2) - 1, buf2);
    FMPKDetailData.readyTime = atoi(buf2);
    getStringToken(data, '|', 5, sizeof(buf2) - 1, buf2);
    FMPKDetailData.member = atoi(buf2);
    getStringToken(data, '|', 6, sizeof(buf2) - 1, buf2);
    FMPKDetailData.win = atoi(buf2);
}

void FMPKDetailWN(int mode)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2, -2, -2, -2};
    static int btnId[] = {-2, -2, -2, -2, -2};
    static int btnLoc[6][2];
    static int btnCnt;
    static int pushBtnFlag[4];
    int selBtnId;
    char dataBuf[256];
    char buf[256];
    char buf2[256];
    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        winW = 6;
        winH = 8;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
        {
            fontId[i] = -2;
        }

        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }

        if (btnCnt > 0)
        {
            int w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = winH * 48 - 56;
            }
        }

        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));
        selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));

        // ?????????????????????
        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
        }

        // ＯＫ
        if (0 <= id && id < 6)
        {
            btn = 1;
            btn <<= id;

            sprintf_s(dataBuf, "%s|%s|%s|%d|%d|%d",
                      FMPKDetailData.time,
                      makeEscapeString(FMPKDetailData.atkFMName, buf, sizeof(buf)),
                      makeEscapeString(FMPKDetailData.defFMName, buf2, sizeof(buf2)),
                      FMPKDetailData.readyTime,
                      FMPKDetailData.member,
                      FMPKDetailData.win);
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, dataBuf);
            windowTypeWN = -1;
        }
        else if (id == 6)
        {
            FMPKDetailData.win++;
            if (FMPKDetailData.win > 1)
                FMPKDetailData.win = 0;
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)
            switch (selBtnId)
            {
            case 0:
                pushBtnFlag[0] = TRUE;
                FMPKDetailData.readyTime++;
                if (FMPKDetailData.readyTime > 40)
                    FMPKDetailData.readyTime = 1;
                break;
            case 1:
                pushBtnFlag[1] = TRUE;
                FMPKDetailData.readyTime--;
                if (FMPKDetailData.readyTime < 1)
                    FMPKDetailData.readyTime = 40;
                break;
            case 2:
                pushBtnFlag[2] = TRUE;
                FMPKDetailData.member++;
                if (FMPKDetailData.member > 50)
                    FMPKDetailData.member = 1;
                break;
            case 3:
                pushBtnFlag[3] = TRUE;
                FMPKDetailData.member--;
                if (FMPKDetailData.member < 1)
                    FMPKDetailData.member = 50;
                break;
            }

        if (id >= 0 && id < 6)
        {
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            return;
        }

        int i;
        for (i = 0; i < 4; i++)
        {
            if (mouse.state & MOUSE_LEFT_CRICK && pushBtnFlag[i] == TRUE)
                pushBtnFlag[i] = TRUE;
            else
                pushBtnFlag[i] = FALSE;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {

            j = 0;

            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }

            StockFontBuffer(winX + 140, winY + 30,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族对战申请表", 0);
#ifdef _READ16BITBMP
            if (!g_bUseAlpha)
                StockDispHLine(winX + 100, winY + 50, 3);
#endif

            sprintf_s(dataBuf, "%s %s", "挑战家族：", FMPKDetailData.atkFMName);
            StockFontBuffer(winX + 40, winY + 90,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            sprintf_s(dataBuf, "%s %s", "受邀家族：", FMPKDetailData.defFMName);
            StockFontBuffer(winX + 40, winY + 90 + 40,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            sprintf_s(dataBuf, "准备时间： %d 分钟", FMPKDetailData.readyTime);
            StockFontBuffer(winX + 40, winY + 90 + 80,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            btnId[0] = StockDispBuffer(winX + 190, winY + 98 + 80, DISP_PRIO_IME3,
                                       CG_UP_BTN + pushBtnFlag[0], 2);
            btnId[1] = StockDispBuffer(winX + 190 + 40, winY + 98 + 80, DISP_PRIO_IME3,
                                       CG_DOWN_BTN + pushBtnFlag[1], 2);

            sprintf_s(dataBuf, "参加人数： %d 人", FMPKDetailData.member);
            StockFontBuffer(winX + 40, winY + 90 + 120,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            btnId[2] = StockDispBuffer(winX + 190, winY + 98 + 120, DISP_PRIO_IME3,
                                       CG_UP_BTN + pushBtnFlag[2], 2);
            btnId[3] = StockDispBuffer(winX + 190 + 40, winY + 98 + 120, DISP_PRIO_IME3,
                                       CG_DOWN_BTN + pushBtnFlag[3], 2);

            sprintf_s(dataBuf, "胜利条件： %s", winStr[FMPKDetailData.win]);
            fontId[6] = StockFontBuffer(winX + 40, winY + 90 + 160,
                                        FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 2);
        }
    }
}

int familyTax;
int familyTaxChange;
int haveGold;

void initFamilyTaxWN(char *data)
{
    char buf[256];
    getStringToken(data, '|', 3, sizeof(buf) - 1, buf);
    familyTax = atoi(buf);

    familyTaxChange = familyTax;
    haveGold = pc.gold;

    if (windowTypeWN != WINDOW_MESSAGETYPE_FAMILYTAX)
    {
        windowTypeWN = WINDOW_MESSAGETYPE_FAMILYTAX;
        DeathAction(ptActMenuWin);
        ptActMenuWin = NULL;
    }
}

void closeFamilyTaxWN()
{
    windowTypeWN = -1;
    wnCloseFlag = 0;
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;
}

void familyTaxWN()
{

    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2};
    static int btnId[] = {-2, -2, -2, -2};
    static int pushBtnFlag[4];
    static int taxGoldInc = 0, taxGoldDec = 0;
    static int taxGoldCnt = 0;
    int selBtnId;
    char dataBuf[256];
    char buf[256];
    int id;
    int i;

    if (ptActMenuWin == NULL)
    {
        winW = 5;
        winH = 4;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++) fontId[i] = -2;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));
        selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));
        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            closeFamilyTaxWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)

            switch (selBtnId)
            {
            case 0:
                sprintf_s(buf, "B|T|%d", familyTaxChange - familyTax);
                lssproto_FM_send(sockfd, buf);
                closeFamilyTaxWN();
                return;
                break;
            case 1:
                closeFamilyTaxWN();
                return;
                break;
            }

#if 1
        switch (selBtnId)
        {
        case 2:
            if (mouse.onceState & MOUSE_LEFT_CRICK_UP && pushBtnFlag[2] == TRUE)
                pushBtnFlag[2] = FALSE;
            if ((haveGold < CHAR_getMaxHaveGold()) && (familyTaxChange > 0))
            {
                if (pushBtnFlag[2] == TRUE)
                {
                    haveGold += taxGoldInc;
                    // bankGold -= bankGoldInc;
                    taxGoldCnt++;
                    if (taxGoldCnt >= 30)
                    {
                        taxGoldCnt = 0;
                        if (taxGoldInc == 0)
                            taxGoldInc = 1;
                        else
                        {
                            taxGoldInc *= 5;
                            if (taxGoldInc > 10000)
                                taxGoldInc = 10000;
                        }
                    }
                    if (haveGold >= CHAR_getMaxHaveGold())
                    {
                        haveGold = CHAR_getMaxHaveGold();
                        play_se(220, 320, 240);
                    }
                    if (haveGold >= pc.gold + familyTax)
                    {
                        haveGold = pc.gold + familyTax;
                        play_se(220, 320, 240);
                    }
                }

                // ????????
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    // ???
                    haveGold++;
                    // ????????
                    if (haveGold >= CHAR_getMaxHaveGold())
                    {
                        haveGold = CHAR_getMaxHaveGold();
                        // ???
                        play_se(220, 320, 240);
                    }
                    else
                    {
                        // ?????
                        taxGoldInc = 0;
                        // ?????????????
                        taxGoldCnt = 0;
                        // ???????
                        pushBtnFlag[2] = TRUE;
                        // ?????
                        play_se(217, 320, 240);
                    }
                }
            }
            break;

        case 3:
            if (mouse.onceState & MOUSE_LEFT_CRICK_UP && pushBtnFlag[3] == TRUE)
                pushBtnFlag[3] = FALSE;
            if (haveGold > 0 && (familyTax + (pc.gold - haveGold)) < MAX_FMBANKGOLD)
            {
                if (pushBtnFlag[3] == TRUE)
                {
                    haveGold -= taxGoldDec;
                    // bankGold -= bankGoldInc;
                    taxGoldCnt++;

                    // ????
                    if (taxGoldCnt >= 30)
                    {
                        // ?????????????
                        taxGoldCnt = 0;
                        // ????????
                        if (taxGoldDec == 0)
                        {
                            taxGoldDec = 1;
                        }
                        else
                        {
                            // ?????
                            taxGoldDec *= 5;
                            // ????????
                            if (taxGoldDec > 10000)
                            {
                                taxGoldDec = 10000;
                            }
                        }
                    }
                    // ????????
                    if (haveGold < 0)
                    {
                        haveGold = 0;
                        play_se(220, 320, 240);
                    }
                    if ((familyTax + (pc.gold - haveGold)) > MAX_FMBANKGOLD)
                    {
                        haveGold = familyTax + pc.gold - MAX_FMBANKGOLD;
                        play_se(220, 320, 240);
                    }
                }
                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    haveGold--;
                    if (haveGold < 0)
                    {
                        haveGold = 0;
                        play_se(220, 320, 240);
                    }
                    else
                    {
                        taxGoldDec = 0;
                        taxGoldCnt = 0;
                        pushBtnFlag[3] = TRUE;
                        play_se(217, 320, 240);
                    }
                }
            }
            break;
        }

#endif

        for (int i = 0; i < 4; i++)
        {
            if (mouse.state & MOUSE_LEFT_CRICK && pushBtnFlag[i] == TRUE)
                pushBtnFlag[i] = TRUE;
            else
                pushBtnFlag[i] = FALSE;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            sprintf_s(dataBuf, "%s  家族的帐户", pc.familyName);
            StockFontBuffer(winX + 80, winY + 30,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);
#ifdef _READ16BITBMP
            if (!g_bUseAlpha)
                StockDispHLine(winX + 64, winY + 50, 3);
#endif
            StockFontBuffer(winX + 60, winY + 70,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "存  款：", 0);

            familyTaxChange = familyTax + (pc.gold - haveGold);
            sprintf_s(dataBuf, "%10d", familyTaxChange);
            StockFontBuffer(winX + 200, winY + 70,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            StockFontBuffer(winX + 60, winY + 120,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "现  金：", 0);
            sprintf_s(dataBuf, "%10d", haveGold);
            StockFontBuffer(winX + 200, winY + 120,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            btnId[3] = StockDispBuffer(winX + 190 + 40, winY + 105, DISP_PRIO_IME3,
                                       CG_UP_BTN + pushBtnFlag[3], 2);

#ifdef _FMVER21
            if (pc.familyleader == FMMEMBER_LEADER ||
                pc.familyleader == FMMEMBER_ELDER)
            {
#else
            if (pc.familyleader == 1)
            {
#endif
                btnId[2] = StockDispBuffer(winX + 190, winY + 105, DISP_PRIO_IME3,
                                           CG_DOWN_BTN + pushBtnFlag[2], 2);
                btnId[0] = StockDispBuffer(winX + 100, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_OK_BTN, 2);
                btnId[1] = StockDispBuffer(winX + 100 + 120, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_CANCEL_BTN, 2);
            }
            else
            {
                btnId[0] = StockDispBuffer(winX + 100, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_OK_BTN, 2);
                btnId[1] = StockDispBuffer(winX + 100 + 120, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_CANCEL_BTN, 2);
            }
        }
    }
}

int rideablePet[3];
int showNum;

void initShowRidePetWN(void)
{
    // int baseimageNo = pc.graNo - (pc.graNo % 5);
    int leaderimageNo = 100700 + ((pc.baseGraNo - 100000) / 20) * 10 + (pc.familySprite) * 5;
    int i = 0, j;

    rideablePet[0] = 0;
    rideablePet[1] = 0;
    rideablePet[2] = 0;
    for (j = 0; j < sizeof(ridePetTable) / sizeof(tagRidePetTable); j++)
    {
#ifdef _LEADERRIDE
        if (ridePetTable[j].charNo == pc.baseGraNo ||
            (ridePetTable[j].charNo == leaderimageNo && pc.big4fm != 0 && pc.familyleader != FMMEMBER_APPLY && pc.familyleader != FMMEMBER_NONE))
#else
        if (ridePetTable[j].charNo == pc.baseGraNo)
#endif
        {
            rideablePet[i] = ridePetTable[j].petNo;
            i++;
            showNum = i;
            if (i >= 3)
                break;
        }
    }
}

static ACTION *pActPet12;
static ACTION *pActPet13;
static ACTION *pActPet14;

void showRidePetWN(void)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2};

    static int btnLoc[6][2];
    static int btnCnt;
    // static int msgLine;
    // static STR_BUFFER input;
    char dataBuf[256];
    char buf[256];

    int id;
    int i, j;
    int mask;
    int btn;

    static int totalMsgLine;
    static int lineSkip;

    if (ptActMenuWin == NULL)
    {
        // winW = msgWN_W;
        // winH = msgWN_H;
        winW = 7;
        winH = 6;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++) fontId[i] = -2;
        mask = 1;
        btnCnt = 0;
        for (i = 0; i < 6; i++, mask <<= 1)
        {
            if (buttonTypeWN & mask)
            {
                btnCnt++;
                if (btnCnt == 4)
                    break;
            }
        }

        // ?????????????
        if (btnCnt > 0)
        {
            int w;
            w = winW * 64 / (btnCnt + 1);
            for (i = 0; i < btnCnt; i++)
            {
                btnLoc[i][0] = w * (i + 1) - 27;
                btnLoc[i][1] = winH * 48 - 35;
            }
        }

        play_se(202, 320, 240); // ????????
    }

    if (ptActMenuWin != NULL)
    {
        // ?????
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));

        // ?????????????????????
        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            DeathAction(pActPet12);
            pActPet12 = NULL;
            DeathAction(pActPet13);
            pActPet13 = NULL;
            DeathAction(pActPet14);
            pActPet14 = NULL;
        }

        // ＯＫ
        if (0 <= id && id < 6)
        {
            btn = 1;
            btn <<= id;
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, btn, "");
            windowTypeWN = -1;
        }
        else if ((6 <= id) && (id < 6 + 8))
        {

            int dataSel = id - 6;
            sprintf_s(dataBuf, "%d|%s", FMPKSelectData[dataSel].fmindex,
                      makeEscapeString(FMPKSelectData[dataSel].fmname, buf, sizeof(buf)));
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN,
                                 WINDOW_BUTTONTYPE_OK, dataBuf);
        }

        if (id >= 0)
        {
            windowTypeWN = -1;
            wnCloseFlag = 0;
            DeathAction(ptActMenuWin);
            ptActMenuWin = NULL;
            DeathAction(pActPet12);
            pActPet12 = NULL;
            DeathAction(pActPet13);
            pActPet13 = NULL;
            DeathAction(pActPet14);
            pActPet14 = NULL;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {

            j = 0;

            for (i = 0, mask = 1; i < 6; i++, mask <<= 1)
            {
                fontId[i] = -2;
                if (buttonTypeWN & mask)
                {
                    if (j < btnCnt)
                    {
                        fontId[i] =
                            StockFontBuffer(winX + btnLoc[j][0], winY + btnLoc[j][1],
                                            FONT_PRIO_FRONT, FONT_PAL_YELLOW, gButtonList[i], 2);
                        j++;
                    }
                }
            }

            sprintf_s(dataBuf, "%s", "适合你骑乘的宠物种类如下：");
            StockFontBuffer(winX + 30, winY + 40,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            if (pActPet12 == NULL)
                if (rideablePet[0] != 0)
                    pActPet12 = MakeAnimDisp(winX + 115, winY + 180, rideablePet[0], 0);

            if (pActPet13 == NULL)
                if (rideablePet[1] != 0)
                    pActPet13 = MakeAnimDisp(winX + 335, winY + 180, rideablePet[1], 0);

            if (pActPet14 == NULL)
                if (rideablePet[2] != 0)
                    pActPet14 = MakeAnimDisp(winX + 225, winY + 180, rideablePet[2], 0);

            sprintf_s(dataBuf, "%s", "赶快去把它们找出来吧！");
            StockFontBuffer(winX + 30, winY + 220,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);
        }
    }
}

char memberName[100][36];
int memberIndex[100];
int memberNum;
// int pagenum;
int changeWho;

void initFamilyLeaderChange(char *data)
{

    int i;
    char buf[256], buf2[256];

    pagenum = 1;
    changeWho = -1;
    getStringToken(data, '|', 4, sizeof(buf) - 1, buf);
    memberNum = atoi(buf);

    for (i = 0; i < memberNum; i++)
    {
        getStringToken(data, '|', 5 + i * 2, sizeof(buf2) - 1, buf2);
        memberIndex[i] = atoi(buf2);
        getStringToken(data, '|', 5 + i * 2 + 1, sizeof(buf2) - 1, buf2);
        strcpy(memberName[i], makeStringFromEscaped(buf2));
    }
}

void closeFamilyLeaderChangeWN()
{
    windowTypeWN = -1;
    // wnCloseFlag = 0;
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;
    changeWho = -1;
}

void familyLeaderChangeWN()
{

    static int winX, winY;
    static int winW, winH;
    static int fontId[30];
    static int fontId2[3];
    int selId, selId2;
    // char buf[256];
    int i;

    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 8;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
        for (i = 0; i < sizeof(fontId2) / sizeof(int); i++)
            fontId2[i] = -2;

        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        selId = selFontId(fontId, sizeof(fontId) / sizeof(int));
        selId2 = selFontId(fontId2, sizeof(fontId2) / sizeof(int));
        // selBtnId = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );

        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            // id = 100;
            closeFamilyLeaderChangeWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)
            if (selId >= 0 && selId < 30)
            {
                closeFamilyLeaderChangeWN();
                changeWho = (pagenum - 1) * 30 + selId;
                windowTypeWN = WINDOW_MESSAGETYPE_LEADERSELECTQ;
                return;
            }

        switch (selId2)
        {
        case 0:
            closeFamilyLeaderChangeWN();
            return;
        case 1:
            pagenum--;
            return;
        case 2:
            pagenum++;
            return;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            StockFontBuffer(winX + 50, winY + 30, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "请选择家族成员：", 0);

            fontId2[0] = StockFontBuffer(winX + 300, winY + 350, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                         "  离  开  ", 2);

            switch (pagenum)
            {
            case 1:
                for (i = 0; i < memberNum; i++)
                    fontId[i] = StockFontBuffer(winX + 30 + (i / 10) * 50, winY + 60 + (i % 10) * 20,
                                                FONT_PRIO_FRONT, FONT_PAL_WHITE, memberName[i], 2);
                if (memberNum > 30)
                    fontId2[2] = StockFontBuffer(winX + 100, winY + 350, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                                 "下一页", 2);
                break;
            case 2:
                for (i = 30; i < memberNum; i++)
                    fontId[i] = StockFontBuffer(winX + 30 + ((i - 30) / 10) * 50, winY + 60 + (i % 10) * 20,
                                                FONT_PRIO_FRONT, FONT_PAL_WHITE, memberName[i], 2);
                fontId2[1] = StockFontBuffer(winX + 30, winY + 350, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                             "上一页", 2);
                if (memberNum > 60)
                    fontId2[2] = StockFontBuffer(winX + 100, winY + 350, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                                 "下一页", 2);
                break;
            case 3:
                for (i = 60; i < memberNum; i++)
                    fontId[i] = StockFontBuffer(winX + 30 + ((i - 60) / 10) * 50, winY + 60 + (i % 10) * 20,
                                                FONT_PRIO_FRONT, FONT_PAL_WHITE, memberName[i], 2);
                fontId2[1] = StockFontBuffer(winX + 30, winY + 350, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                             "上一页", 2);
                break;
            }
        }
    }
}

void familyLeaderChangeQWN()
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[2];
    int selId;
    char buf[256];
    int i;

    if (ptActMenuWin == NULL)
    {
        winW = 6;
        winH = 2;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;

        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;

        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        selId = selFontId(fontId, sizeof(fontId) / sizeof(int));

        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            // id = 100;
            closeFamilyLeaderChangeWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)

            switch (selId)
            {
            case 0:
                closeFamilyLeaderChangeWN();
                return;
            case 1:
                sprintf_s(buf, "L|CHANGE|Q|%d|%s", memberIndex[changeWho], memberName[changeWho]);
                    lssproto_FM_send(sockfd, buf);
                closeFamilyLeaderChangeWN();
                return;
            }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            sprintf_s(buf, "你确定要将族长的位子交给%s吗？", memberName[changeWho]);
            StockFontBuffer(winX + 50, winY + 30, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            buf, 0);

            fontId[0] = StockFontBuffer(winX + 200, winY + 60, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                        "  离  开  ", 2);
            fontId[1] = StockFontBuffer(winX + 280, winY + 60, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                        "  确  定  ", 2);
        }
    }
}

char oldLeaderName[36];
int oldLeaderIndex;

void initFamilyLeaderChangeA(char *data)
{
    char buf[64];

    // windowTypeWN = -1;
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;

    getStringToken(data, '|', 1, sizeof(buf) - 1, buf);
    strcpy(oldLeaderName, makeStringFromEscaped(buf));
    getStringToken(data, '|', 2, sizeof(buf) - 1, buf);
    oldLeaderIndex = atoi(buf);
}

void familyLeaderChangeAWN()
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[2];
    int selId;
    char buf[256], buf2[64];
    int i;

    if (ptActMenuWin == NULL)
    {
        winW = 6;
        winH = 3;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;

        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        selId = selFontId(fontId, sizeof(fontId) / sizeof(int));

        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            // id = 100;
            closeFamilyLeaderChangeWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)

            switch (selId)
            {
            case 0:
                sprintf_s(buf, "L|CHANGE|A|0|%s|%d",
                          makeEscapeString(oldLeaderName, buf2, sizeof(buf2)),
                          oldLeaderIndex);
                    lssproto_FM_send(sockfd, buf);
                closeFamilyLeaderChangeWN();
                return;
            case 1:
                sprintf_s(buf, "L|CHANGE|A|1|%s|%d",
                          makeEscapeString(oldLeaderName, buf2, sizeof(buf2)),
                          oldLeaderIndex);
                    lssproto_FM_send(sockfd, buf);
                closeFamilyLeaderChangeWN();
                return;
            }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            sprintf_s(buf, "%s 希望将族长的位子交给你，", oldLeaderName);
            StockFontBuffer(winX + 50, winY + 30, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            buf, 0);
            StockFontBuffer(winX + 50, winY + 60, FONT_PRIO_FRONT, FONT_PAL_WHITE,
                            "你要接受吗？", 0);

            fontId[0] = StockFontBuffer(winX + 200, winY + 105, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                        "  不愿意  ", 2);
            fontId[1] = StockFontBuffer(winX + 280, winY + 105, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                        "  愿  意  ", 2);
        }
    }
}

struct
{
    char name[64];
    int member;
    char leadername[64];
    int rank;
    int fmdp;
    int personaldp;
    int job;
    int sprite;
    char pktime[32];
    int pkaddress;
    char pkopp[64];
    char pkdata[256];
#ifdef _NEW_MANOR_LAW
    int fmMomentum;
    int momentum;
#endif
#ifdef _FAMILYBADGE_
    int badgeNo;
#endif
} mineFamily;

void initMineFamilyDetail(char *data)
{

    char ibuf[64];

    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;

    makeStringFromEscaped(data);

    getStringToken(data, '|', 1, sizeof(mineFamily.name) - 1, mineFamily.name);
    makeStringFromEscaped(mineFamily.name);
    getStringToken(data, '|', 2, sizeof(ibuf) - 1, ibuf);
    mineFamily.member = atoi(ibuf);
    getStringToken(data, '|', 3, sizeof(mineFamily.leadername) - 1, mineFamily.leadername);
    makeStringFromEscaped(mineFamily.leadername);
    getStringToken(data, '|', 4, sizeof(ibuf) - 1, ibuf);
    mineFamily.rank = atoi(ibuf);
    getStringToken(data, '|', 5, sizeof(ibuf) - 1, ibuf);
    mineFamily.fmdp = atoi(ibuf);
    getStringToken(data, '|', 6, sizeof(ibuf) - 1, ibuf);
    mineFamily.personaldp = atoi(ibuf);
    getStringToken(data, '|', 7, sizeof(ibuf) - 1, ibuf);
    mineFamily.job = atoi(ibuf);
    if (mineFamily.job < 0)
        mineFamily.job = 0;
    getStringToken(data, '|', 8, sizeof(ibuf) - 1, ibuf);
    mineFamily.sprite = atoi(ibuf);
    /*
    getStringToken( data, '|', 9, sizeof( buf )-1, buf );
    makeStringFromEscaped( buf );
    getStringToken( buf, ' ', 1, sizeof( mineFamily.pktime )-1, mineFamily.pktime );
    makeStringFromEscaped( mineFamily.pktime );
    getStringToken( buf, ' ', 2, sizeof( ibuf )-1, ibuf );
    mineFamily.pkaddress = atoi( ibuf );
    getStringToken( buf, ' ', 3, sizeof( mineFamily.pkopp )-1, mineFamily.pkopp );
    makeStringFromEscaped( mineFamily.pkopp );
    */
    getStringToken(data, '|', 9, sizeof(mineFamily.pkdata) - 1, mineFamily.pkdata);
    makeStringFromEscaped(mineFamily.pkdata);
#ifdef _NEW_MANOR_LAW
    getStringToken(data, '|', 10, sizeof(ibuf) - 1, ibuf);
    mineFamily.fmMomentum = atoi(ibuf);
    getStringToken(data, '|', 11, sizeof(ibuf) - 1, ibuf);
    mineFamily.momentum = atoi(ibuf);
#endif
#ifdef _FAMILYBADGE_
    getStringToken(data, '|', 12, sizeof(ibuf) - 1, ibuf);
    mineFamily.badgeNo = atoi(ibuf);
#endif
}

void closeMineFamilyDetailWN()
{
    windowTypeWN = -1;
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;
}

char *familyJobName[] =
    {
        "未加入家族",
        "一般成员",
        "待 审 核",
        " 族  长 ",
        " 长  老 ",
        //    " 长  老 ",
        //    " 祭  司 ",
        //    "财 务 长",
    };

char *familyTownName[] =
    {
        "",
        "萨姆吉尔庄园",
        "玛丽娜丝庄园",
        "加加庄园",
        "卡鲁它那庄园"};

void mineFamilyDetailWN()
{

    static int winX, winY;
    static int winW, winH;
    static int fontId[2];
    int selId;
    char buf[256];
    int i;
    if (ptActMenuWin == NULL)
    {
        winW = 7;
        winH = 6;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;

        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        selId = selFontId(fontId, sizeof(fontId) / sizeof(int));

        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            closeMineFamilyDetailWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)

            switch (selId)
            {
            case 0:
                closeFamilyLeaderChangeWN();
                return;
            }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {

            StockFontBuffer(winX + 190, winY + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族资料", 0);
#ifdef _READ16BITBMP
            if (!g_bUseAlpha)
                StockDispHLine(winX + 160, winY + 40, 2);
#endif

            sprintf_s(buf, "家族名称：%s", mineFamily.name);
            StockFontBuffer(winX + 30, winY + 60, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
            sprintf_s(buf, "族长名称：%s", mineFamily.leadername);
            StockFontBuffer(winX + 240, winY + 60, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

            sprintf_s(buf, "家族人数：%d", mineFamily.member);
            StockFontBuffer(winX + 30, winY + 90, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
            sprintf_s(buf, "守护精灵：%s", familySpriteName[mineFamily.sprite]);
            StockFontBuffer(winX + 240, winY + 90, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
#ifdef _NEW_MANOR_LAW
            sprintf_s(buf, "声望排名：%d", mineFamily.rank);
#else
            sprintf_s(buf, "家族排名：%d", mineFamily.rank);
#endif
            StockFontBuffer(winX + 30, winY + 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
            sprintf_s(buf, "家族声望：%d", mineFamily.fmdp);
            StockFontBuffer(winX + 240, winY + 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

            sprintf_s(buf, "家族职位：%s", familyJobName[mineFamily.job]);
            StockFontBuffer(winX + 30, winY + 150, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
            sprintf_s(buf, "个人声望：%d", mineFamily.personaldp);
            StockFontBuffer(winX + 240, winY + 150, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
#ifdef _NEW_MANOR_LAW
            sprintf_s(buf, "家族气势：%d", mineFamily.fmMomentum);
            StockFontBuffer(winX + 240, winY + 180, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
            sprintf_s(buf, "个人气势：%d", mineFamily.momentum);
            StockFontBuffer(winX + 240, winY + 210, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
#endif
#ifdef _FAMILYBADGE_
            StockFontBuffer(winX + 30, winY + 210, FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族徽章：", 0);
            StockDispBuffer(winX + 120, winY + 215, DISP_PRIO_IME4, mineFamily.badgeNo, 0);
#endif
            StockFontBuffer(winX + 30, winY + 180, FONT_PRIO_FRONT, FONT_PAL_WHITE, "家族踢馆：", 0);
#ifdef _NEW_MANOR_LAW
            StockFontBuffer(winX + 30, winY + 230, FONT_PRIO_FRONT, FONT_PAL_WHITE, mineFamily.pkdata, 0);
#else
            StockFontBuffer(winX + 30, winY + 230, FONT_PRIO_FRONT, FONT_PAL_WHITE, mineFamily.pkdata, 0);
#endif
            fontId[0] = StockFontBuffer(winX + 180, winY + 250, FONT_PRIO_FRONT, FONT_PAL_YELLOW,
                                        "  离  开  ", 2);
        }
    }
}

// Terry add 2001/12/06
#ifdef __EDEN_EFFECT
ACTION *pBankProcWnd = NULL;

void BankProc(void)
{
    static int BankBtNo[20];    // 按钮的编号
    static int BankBtFlag[20];  // 按钮的状态
    static int Gold = 0;        // 要存入或取出的钱
    static int BankGoldInc = 0; // 金钱每加一次的值
    static int BankGoldCnt = 0; // 按钮计数
    int x, y;

    if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        if (pBankProcWnd != NULL)
        {
            windowTypeWN = -1;
            DeathAction(pBankProcWnd);
            pBankProcWnd = NULL;
            play_se(217, 320, 240);
            return;
        }
    }

    // 如果目前没有秀出银行介面
    if (pBankProcWnd == NULL)
    {
        // 秀出银行介面
#ifdef _READ16BITBMP
        if (g_bUseAlpha)
            pBankProcWnd = MakeWindowDisp(185, 160, 270, 160, OLD_GRAPHICS_START + 46, -1); // x=(640-w)/2 y=(480-h)/2 w=270 h=160
        else
#endif
            pBankProcWnd = MakeWindowDisp(185, 160, 270, 160, 26287, -1); // x=(640-w)/2 y=(480-h)/2 w=270 h=160
        Gold = 0;
        // 播音效
        play_se(202, 320, 240);
    }
    else
    {
        // 如果视窗已经开好了
        if (pBankProcWnd->hp > 0)
        {
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                // 如果按下的是取消,清掉介面
                if (HitDispNo == BankBtNo[0])
                {
                    ZeroMemory(msg, sizeof(msg));
                        lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 0, msg);
                    windowTypeWN = -1;
                    DeathAction(pBankProcWnd);
                    pBankProcWnd = NULL;
                    play_se(217, 320, 240);
                    return;
                }
                // 如果按下的是确定,把玩家存或领多少钱送给gs
                if (HitDispNo == BankBtNo[1])
                {
                    sprintf_s(msg, "%d", Gold);
                    pc.gold -= Gold;
                    pc.personal_bankgold += Gold;
                        lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, 4, msg);
                    windowTypeWN = -1;
                    DeathAction(pBankProcWnd);
                    pBankProcWnd = NULL;
                    play_se(217, 320, 240);
                    return;
                }
            }

            // 玩家存钱
            if (HitDispNo == BankBtNo[3])
            {
                // 如果按钮被放开了(滑鼠左键放开),设定按钮没被按下
                if (mouse.onceState & MOUSE_LEFT_CRICK_UP && BankBtFlag[3] == TRUE)
                    BankBtFlag[3] = FALSE;

                // 如果要存入的钱加上已在银行里的钱比银行上限小及玩家身上的钱不小于要存入的钱
                if (Gold + pc.personal_bankgold <= MAX_PERSONAL_BANKGOLD && pc.gold - Gold > 0)
                {
                    // 存入的按钮设为true(此时为一直按着钮的状态)
                    if (BankBtFlag[3] == TRUE)
                    {
                        Gold += BankGoldInc;
                        BankGoldCnt++;

                        // 如果BankGoldCnt大于30,增加一次的钱变为五倍
                        if (BankGoldCnt >= 30)
                        {
                            // 把count清为0
                            BankGoldCnt = 0;
                            if (BankGoldInc == 0)
                                BankGoldInc = 1;
                            else
                            {
                                // 增加变为五倍
                                BankGoldInc *= 5;
                                // 最多一次加10000
                                if (BankGoldInc > 10000)
                                    BankGoldInc = 10000;
                            }
                        }
                        // 如果存入的钱比银行上限大
                        if (Gold + pc.personal_bankgold >= MAX_PERSONAL_BANKGOLD)
                        {
                            Gold = MAX_PERSONAL_BANKGOLD - pc.personal_bankgold;
                            play_se(220, 320, 240);
                        }
                        // 如果玩家身上的钱已小于0
                        if (pc.gold - Gold < 0)
                        {
                            Gold = pc.gold;
                            play_se(220, 320, 240);
                        }
                    }

                    // 按下左键时
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        // 存入的钱增加
                        Gold++;
                        // 如果存入的钱加上玩家已在银行里的钱大于银行上限
                        if (Gold + pc.personal_bankgold >= MAX_PERSONAL_BANKGOLD)
                        {
                            Gold = MAX_PERSONAL_BANKGOLD - pc.personal_bankgold;
                            // 播音效
                            play_se(220, 320, 240);
                        }
                        // 如果玩家身上的钱已小于0
                        if (pc.gold - Gold < 0)
                        {
                            Gold = pc.gold;
                            play_se(220, 320, 240);
                        }
                        else
                        {
                            // 一次增加的钱清为0
                            BankGoldInc = 0;
                            // 计数清为0
                            BankGoldCnt = 0;
                            // 设定按钮被按下
                            BankBtFlag[3] = TRUE;
                            // 播音效
                            play_se(217, 320, 240);
                        }
                    }
                }
                else
                {
                    BankGoldInc = 0;
                    BankGoldCnt = 0;
                }
            }
            else
            {
                // 设定按键没被按下
                BankBtFlag[3] = FALSE;
            }

            // 玩家领钱
            // 因为是领钱,所以Gold为负数
            if (HitDispNo == BankBtNo[2])
            {
                // 如果按钮被放开了(滑鼠左键放开),设定按钮没被按下
                if (mouse.onceState & MOUSE_LEFT_CRICK_UP && BankBtFlag[2] == TRUE)
                    BankBtFlag[2] = FALSE;
                // 如果要领出的钱比玩家已在银行里的钱少且领出的钱加上玩家身上的钱不大于玩家身上钱的上限
                if (pc.personal_bankgold + Gold > 0 && pc.gold - Gold <= CHAR_getMaxHaveGold())
                {
                    // 如果按钮被按下
                    if (BankBtFlag[2] == TRUE)
                    {
                        // 领出的钱增加
                        Gold -= BankGoldInc;
                        BankGoldCnt++;

                        // 如果BankGoldCnt大于30,增加一次的钱变为五倍
                        if (BankGoldCnt >= 30)
                        {
                            // 把count清为0
                            BankGoldCnt = 0;
                            if (BankGoldInc == 0)
                                BankGoldInc = 1;
                            else
                            {
                                // 增加变为五倍
                                BankGoldInc *= 5;
                                // 最多一次加10000
                                if (BankGoldInc > 10000)
                                    BankGoldInc = 10000;
                            }
                        }
                        // 如果要领出的钱比玩家已在银行里的钱多
                        if (pc.personal_bankgold + Gold < 0)
                        {
                            Gold = pc.personal_bankgold * (-1);
                            play_se(220, 320, 240);
                        }
                        // 如果领出的钱加上玩家身上的钱大于玩家身上钱的上限
                        if (pc.gold - Gold >= CHAR_getMaxHaveGold())
                        {
                            Gold = (CHAR_getMaxHaveGold() - pc.gold) * (-1);
                            play_se(220, 320, 240);
                        }
                    }
                    // 按下左键时
                    if (mouse.onceState & MOUSE_LEFT_CRICK)
                    {
                        Gold--;

                        // 如果要领出的钱比玩家已在银行里的钱多
                        if (pc.personal_bankgold + Gold < 0)
                        {
                            Gold = pc.personal_bankgold * (-1);
                            play_se(220, 320, 240);
                        }
                        else
                            // 如果领出的钱加上玩家身上的钱大于玩家身上钱的上限
                            if (pc.gold - Gold >= CHAR_getMaxHaveGold())
                            {
                                Gold = (CHAR_getMaxHaveGold() - pc.gold) * (-1);
                                play_se(220, 320, 240);
                            }
                            else
                            {
                                // 一次增加的钱清为0
                                BankGoldInc = 0;
                                // 计数清为0
                                BankGoldCnt = 0;
                                // 设定按钮被按下
                                BankBtFlag[2] = TRUE;
                                play_se(217, 320, 240);
                            }
                    }
                }
                else
                {
                    BankGoldInc = 0;
                    BankGoldCnt = 0;
                }
            }
            else
            {
                // 设定按键没被按下
                BankBtFlag[2] = FALSE;
            }

            if (pBankProcWnd != NULL)
            {
                // 座标设定
                x = pBankProcWnd->x;
                y = pBankProcWnd->y;

                BankBtNo[0] = StockDispBuffer(x + 200, y + 140, DISP_PRIO_IME3, CG_TRADE_CANCEL_BTN, 2);
                BankBtNo[1] = StockDispBuffer(x + 75, y + 140, DISP_PRIO_IME3, CG_TRADE_OK_BTN, 2);
                BankBtNo[2] = StockDispBuffer(x + 140, y + 74, DISP_PRIO_IME3, CG_UP_BTN + BankBtFlag[2], 2);
                BankBtNo[3] = StockDispBuffer(x + 200, y + 74, DISP_PRIO_IME3, CG_DOWN_BTN + BankBtFlag[3], 2);

                sprintf_s(msg, "%7d", pc.gold - Gold);
                StockFontBuffer(x + 180, y + 42, FONT_PRIO_FRONT, 0, msg, 0);
                sprintf_s(msg, "%7d", pc.personal_bankgold + Gold);
                StockFontBuffer(x + 180, y + 90, FONT_PRIO_FRONT, 0, msg, 0);
#ifdef _READ16BITBMP
                if (g_bUseAlpha)
                    StockDispBuffer(320, 240, DISP_PRIO_IME3, OLD_GRAPHICS_START + 46, 1);
                else
#endif
                    StockDispBuffer(lpDraw->xSize / 2, lpDraw->ySize / 2, DISP_PRIO_IME3, 26287, 1);
            }
        }
    }
}
#endif
// Terry end

// Terry add 2002/01/03
#ifdef __EDEN_AUCTION
#define AUCTION_NEW 26266         // 委托拍卖介面
#define AUCTION_OK_U 26267        // ok按钮(未按下)
#define AUCTION_OK_D 26268        // ok按钮(按下)
#define AUCTION_PLANK 26269       // 在物品底下的石板
#define AUCTION_UP_U 26188        // 上箭头按钮(未按下)
#define AUCTION_UP_D 26189        // 上箭头按钮(按下)
#define AUCTION_DOWN_U 26190      // 下箭头按钮(未按下)
#define AUCTION_DOWN_D 26191      // 下箭头按钮(按下)
#define AUCTION_LIST 26270        // 拍卖列表(购买)介面
#define AUCTION_LIST_1 26271      // 拍卖列表(修改)介面
#define AUCTION_GET_BACK_U 26272  // 领回按钮(未按下)
#define AUCTION_GET_BACK_D 26273  // 领回按钮(按下)
#define AUCTION_GET_BACK_N 26274  // 领回按钮(无作用)
#define AUCTION_SEE_DATA_U 26275  // 观看资料(未按下)
#define AUCTION_SEE_DATA_D 26276  // 观看资料(按下)
#define AUCTION_SEE_DATA_N 26277  // 观看资料(无作用)
#define AUCTION_PAGE_UP_U 26278   // 上一页(未按下)
#define AUCTION_PAGE_UP_D 26279   // 上一页(按下)
#define AUCTION_PAGE_UP_N 26280   // 上一页(无作用)
#define AUCTION_PAGE_DOWN_U 26281 // 下一页(未按下)
#define AUCTION_PAGE_DOWN_D 26282 // 下一页(按下)
#define AUCTION_PAGE_DOWN_N 26283 // 下一页(无作用)
#define AUCTION_CANCEL_Y_U 26284  // 取消(未按下)(黄色的)
#define AUCTION_CANCEL_Y_D 26285  // 取消(按下)(黄色的)
#define AUCTION_CANCEL_Y_N 26286  // 取消(无作用)(黄色的)
#define AUCTION_DECIDE_U 26289    // 确定(未按下)
#define AUCTION_DECIDE_D 26288    // 确定(按下)
#define AUCTION_CANCEL_R_U 26291  // 取消(未按下)(红色的)
#define AUCTION_CANCEL_R_D 26290  // 取消(按下)(红色的)
#define AUCTION_SURVEY 26292      // 观看(购买)介面
#define AUCTION_SURVEY_1 26293    // 观看(修改)介面
#define AUCTION_PET_STAT 26297    // 宠物五种资料的底图

STR_BUFFER AuctionStr;   // 拍卖叙述共,50个字
STR_BUFFER AuctionMoney; // 拍卖价钱最多一百万
STR_BUFFER AuctionItem;  // 道具叙述
ACTION *pActAuctionWT = NULL;

void AuctionNewWT(void)
{
    static ACTION *pShowPet1 = NULL, *pShowPet2 = NULL;
    static int nButton1[3][2]; // nBotton1[x][y]-> x:0->上箭头 x:1->ok x:2->下箭头  y:0->被点选时的按钮编号
    //                                                   y:1->点选状态 0->没被点选到 1->被点选到
    static int nButton2[2][2]; // nBotton2[x][y]-> x:0->确定 x:1->取消 y:0->被点选时的按钮编号
    //                                      y:1->点选状态 0->没被点选到 1->被点选到
    static int nItem[MAX_ITEM - MAX_ITEMSTART][2]; // nItem[x][y]-> x:->第几个物品(右上开始向右算起0~14) y:0->被点选时的按钮编号
    //                                                    y:1->点选状态 0->没被点选到 1->被点选到
    static int nPet, nPetShow; // nPet:右边列表的宠物是第几只 nPetShow:要卖的宠物
    static int nItemSelect;    // 目前选到的道具是第几个
    static int nAuctionKind;   // 要拍卖的是宠物还是道具 1:Pet 2:Item
    static BOOL bPress;
    int x, y, i, j, k;
    char temp[8];
    char szSendMsg[3][260]; // szSendMsg[x][256]-> x:0->拍卖叙述 x:1->送到server端的资料

    if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        if (pActAuctionWT != NULL)
        {
            windowTypeWN = -1;
            DeathAction(pActAuctionWT);
            pActAuctionWT = NULL;
            DeathAction(pShowPet1);
            pShowPet1 = NULL;
            DeathAction(pShowPet2);
            pShowPet2 = NULL;
            play_se(217, 320, 240);
            GetKeyInputFocus(&MyChatBuffer);
            return;
        }
    }
    // 如果目前没有产生委托介面
    if (pActAuctionWT == NULL)
    {
        // 产生委托介面
        pActAuctionWT = MakeWindowDisp(50, 15, 540, 450, AUCTION_NEW, -1);
        ZeroMemory(szSendMsg, sizeof(szSendMsg));
        ZeroMemory(nButton1, sizeof(nButton1));
        ZeroMemory(nButton2, sizeof(nButton2));
        ZeroMemory(nItem, sizeof(nItem));
        for (i = 0; i < MAX_ITEM; i++)
        {
#ifdef _NEW_ITEM_
            int 道具起始 = MAX_ITEMSTART + MAX_MAXHAVEITEM * gCurrInventoryPage;
            int 道具结束 = 道具起始 + MAX_MAXHAVEITEM;
            if (i >= MAX_ITEMSTART)
            {
                if (i < 道具起始 || i >= 道具结束)
                    continue;
            }
            nItem[i][0] = -1;
#endif
        }
        nPet = nPetShow = 0;
        nItemSelect = 0;
        pShowPet1 = pShowPet2 = NULL;
        bPress = TRUE;
        // 叙述文字初始化
        ZeroMemory(AuctionStr.buffer, sizeof(AuctionStr.buffer));
        AuctionStr.cnt = 0;
        AuctionStr.cursor = 0;
        AuctionStr.color = 0;
        AuctionStr.len = 100;
        AuctionStr.lineLen = 24;
        AuctionStr.lineDist = 20;
        AuctionStr.x = pActAuctionWT->x + 30;
        AuctionStr.y = pActAuctionWT->y + 260;
        AuctionStr.fontPrio = FONT_PRIO_FRONT;
        // 价钱文字初始化
        ZeroMemory(AuctionMoney.buffer, sizeof(AuctionMoney.buffer));
        AuctionMoney.cnt = 0;
        AuctionMoney.cursor = 0;
        AuctionMoney.color = 0;
        AuctionMoney.len = 7;
        AuctionMoney.lineLen = 7;
        AuctionMoney.lineDist = 0;
        AuctionMoney.x = pActAuctionWT->x + 95;
        AuctionMoney.y = pActAuctionWT->y + 387;
        AuctionMoney.fontPrio = FONT_PRIO_FRONT;
        // 道具叙述初始化
        ZeroMemory(AuctionItem.buffer, sizeof(AuctionItem.buffer));
        AuctionItem.cnt = 0;
        AuctionItem.cursor = 0;
        AuctionItem.color = 0;
        AuctionItem.len = 100;
        AuctionItem.lineLen = 28;
        AuctionItem.lineDist = 20;
        AuctionItem.x = pActAuctionWT->x + 26;
        AuctionItem.y = pActAuctionWT->y + 170;
        AuctionItem.fontPrio = FONT_PRIO_FRONT;
        // 播音效
        play_se(202, 320, 240);
    }
    else
    {
        // 如果视窗已经开好了
        if (pActAuctionWT->hp > 0)
        {
            // 座标设定
            x = pActAuctionWT->x;
            y = pActAuctionWT->y;

            // 如果滑鼠左键按着
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                for (i = 0; i < 3; i++)
                {
                    if (HitDispNo == nButton1[i][0] && !nButton1[i][1])
                    {
                        nButton1[i][1] = 1;
                        bPress = TRUE;
                    }
                    if (i < 2)
                    {
                        if (HitDispNo == nButton2[i][0] && !nButton2[i][1])
                        {
                            nButton2[i][1] = 1;
                            bPress = TRUE;
                        }
                    }
                }
            }
            // 如果滑鼠左键放开
            if (mouse.onceState & MOUSE_LEFT_CRICK_UP)
            {
                for (i = 0; i < 3; i++)
                {
                    if (nButton1[i][1])
                        nButton1[i][1] = 0;
                    if (i < 2)
                        if (nButton2[i][1])
                            nButton2[i][1] = 0;
                }
                bPress = FALSE;
            }

            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 5; j++)
                {
                    if (MakeHitBox(x + 261 + j * 50 - 24, y + 272 + i * 48 - 24, x + 261 + j * 50 + 25, y + 272 + i * 48 + 23, DISP_PRIO_IME3) == TRUE)
                    {
                        // 如果滑鼠左键double-click
                        if (mouse.onceState & MOUSE_LEFT_DBL_CRICK)
                        {
                            for (k = 0; k < MAX_ITEM - MAX_ITEMSTART; k++)
                                nItem[k][1] = 0;
#ifdef _NEW_ITEM_
                            int 道具起始 = MAX_ITEMSTART + MAX_MAXHAVEITEM * gCurrInventoryPage;
                            int 道具结束 = 道具起始 + MAX_MAXHAVEITEM;
                            if (i >= MAX_ITEMSTART)
                            {
                                if (i < 道具起始 || i >= 道具结束)
                                    continue;
                            }
#endif
                            nItem[i * 5 + j][1] = 1;
                            // 那一个道具被点选了
                            nItemSelect = i * 5 + j + 5;
                            nAuctionKind = 2;
                            DeathAction(pShowPet2);
                            pShowPet2 = NULL;
                            play_se(217, 320, 240);
                            break;
                        }
                    }
                }
            }

            // 按下了确定
            if (nButton2[0][1] || nButton2[1][1])
            {
                if (nButton2[0][1])
                {
                    // 若是要卖宠物
                    if (nAuctionKind == 1)
                    {
                        CheckSpace(AuctionStr.buffer);
                        // 有这只宠物
                        if (pet[nPetShow].useFlag == 1)
                        {
                            makeEscapeString(AuctionStr.buffer, szSendMsg[0], sizeof(AuctionStr.buffer));
                            sprintf_s(szSendMsg[1], "1|%d|%s|%d", nPetShow, szSendMsg[0], atoi(AuctionMoney.buffer));
                            makeEscapeString(szSendMsg[1], szSendMsg[2], sizeof(szSendMsg[1]));
                                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, WINDOW_BUTTONTYPE_YES, szSendMsg[2]);
                        }
                    }
                    // 若是要卖道具
                    if (nAuctionKind == 2)
                    {
                        // 有这个道具
                        if (pc.item[nItemSelect].useFlag == 1)
                        {
                            makeEscapeString(AuctionStr.buffer, szSendMsg[0], sizeof(AuctionStr.buffer));
                            sprintf_s(szSendMsg[1], "2|%d|%s|%d", nItemSelect, szSendMsg[0], atoi(AuctionMoney.buffer));
                            makeEscapeString(szSendMsg[1], szSendMsg[2], sizeof(szSendMsg[1]));
                                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, WINDOW_BUTTONTYPE_YES, szSendMsg[2]);
                        }
                    }
                }
                windowTypeWN = -1;
                DeathAction(pActAuctionWT);
                pActAuctionWT = NULL;
                DeathAction(pShowPet1);
                pShowPet1 = NULL;
                DeathAction(pShowPet2);
                pShowPet2 = NULL;
                play_se(217, 320, 240);
                GetKeyInputFocus(&MyChatBuffer);
                return;
            }

            // 按下了上箭头
            if (nButton1[0][1] && bPress)
            {
                bPress = FALSE;
                nPet = (nPet - 1 < 0 ? 4 : nPet - 1);
                DeathAction(pShowPet1);
                pShowPet1 = NULL;
                // 秀宠物
                for (i = 0; i < MAX_PET; i++)
                {
                    if (pet[nPet].useFlag == 1)
                    {
                        pShowPet1 = MakeAnimDisp(x + 235 + 70, y + 55 + 120, pet[nPet].graNo, 0);
                        break;
                    }
                    else
                        nPet = (nPet - 1 < 0 ? 4 : nPet - 1);
                }
                play_se(217, 320, 240);
            }
            // 按下了ok
            if (nButton1[1][1] && bPress)
            {
                nAuctionKind = 1;
                bPress = FALSE;
                nItem[nItemSelect - 5][1] = 0;
                // if(nPetShow != nPet && pShowPet1 != NULL)
                if (pShowPet1 != NULL)
                {
                    nPetShow = nPet;
                    DeathAction(pShowPet2);
                    pShowPet2 = NULL;
                    // 秀要被拍卖的宠物
                    if (pet[nPetShow].useFlag == 1)
                        pShowPet2 = MakeAnimDisp(x + 15 + 70, y + 55 + 120, pet[nPetShow].graNo, 0);
                }
                play_se(217, 320, 240);
            }
            // 按下了下箭头
            if (nButton1[2][1] && bPress)
            {
                bPress = FALSE;
                nPet = (nPet + 1 >= 5 ? 0 : nPet + 1);
                DeathAction(pShowPet1);
                pShowPet1 = NULL;
                // 秀宠物
                for (i = 0; i < MAX_PET; i++)
                {
                    if (pet[nPet].useFlag == 1)
                    {
                        pShowPet1 = MakeAnimDisp(x + 235 + 70, y + 55 + 120, pet[nPet].graNo, 0);
                        break;
                    }
                    else
                        nPet = (nPet + 1 >= 5 ? 0 : nPet + 1);
                }
                play_se(217, 320, 240);
            }

            // 滑鼠在叙述区内
            if (MakeHitBox(AuctionStr.x, AuctionStr.y, AuctionStr.x + 170, AuctionStr.y + 95, DISP_PRIO_BOX2) == TRUE)
            {
                GetKeyInputFocus(&AuctionStr);
            }

            // 滑鼠在价钱区内
            if (MakeHitBox(AuctionMoney.x, AuctionMoney.y, AuctionMoney.x + 115, AuctionMoney.y + 18, DISP_PRIO_BOX2) == TRUE)
            {
                GetKeyInputFocus(&AuctionMoney);
            }

            if (pActAuctionWT != NULL)
            {
                // 显示及设定button出现的位置
                nButton1[0][0] = StockDispBuffer(x + 485, y + 70, DISP_PRIO_IME3, AUCTION_UP_U + nButton1[0][1], 2);
                nButton1[1][0] = StockDispBuffer(x + 484, y + 100, DISP_PRIO_IME3, AUCTION_OK_U + nButton1[1][1], 2);
                nButton1[2][0] = StockDispBuffer(x + 485, y + 130, DISP_PRIO_IME3, AUCTION_DOWN_U + nButton1[2][1], 2);
                nButton2[0][0] = StockDispBuffer(x + 312, y + 422, DISP_PRIO_IME3, AUCTION_DECIDE_U + nButton2[0][1], 2);
                nButton2[1][0] = StockDispBuffer(x + 432, y + 422, DISP_PRIO_IME3, AUCTION_CANCEL_R_U, 2);

                // 显示玩家身上的道具
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 5; j++)
                    {
                        // 如果这一栏有道具且没被点选到,秀在右下角的身上道具栏
                        if (!nItem[i * 5 + j][1])
                        {
                            // 秀道具图
                            if (pc.item[i * 5 + j + 5].useFlag == 1)
                                nItem[i * 5 + j][0] = StockDispBuffer(x + 262 + j * 50, y + 273 + i * 48, DISP_PRIO_ITEM, pc.item[i * 5 + j + 5].graNo, 0);
                        }
                        // 如果这一栏的道具被点选到,秀在左上角的要被拍卖的栏位
                        else
                        {
                            // 秀道具图
                            StockDispBuffer(x + 120, y + 125, DISP_PRIO_IME3, pc.item[i * 5 + j + 5].graNo, 0);
                            // 秀底下的石板
                            StockDispBuffer(x + 120, y + 125, DISP_PRIO_IME3, AUCTION_PLANK, 0);
                            // 秀道具名称
                            StockFontBuffer(x + 36, y + 28, FONT_PRIO_FRONT, 0, pc.item[i * 5 + j + 5].name, 0);
                            // 秀道具叙述
                            strncpy_s(AuctionItem.buffer, pc.item[i * 5 + j + 5].memo, 64);
                            StockFontBuffer2(&AuctionItem);
                        }
                    }
                }
                // 显示玩家身上的宠物资料
                // 把选到的宠物资料秀在右边
                if (pet[nPet].useFlag == 1)
                {
                    if (pShowPet1 != NULL)
                    {
                        // 秀宠物名称
                        StockFontBuffer(x + 256, y + 28, FONT_PRIO_FRONT, 0, pet[nPet].name, 0);
                        // 秀宠物的属性
                        _itoa_s(pet[nPet].level, temp, 10);
                        StockFontBuffer(x + 400, y + 58, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPet].atk, temp, 10);
                        StockFontBuffer(x + 400, y + 58 + 28, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPet].def, temp, 10);
                        StockFontBuffer(x + 400, y + 58 + 56, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPet].quick, temp, 10);
                        StockFontBuffer(x + 400, y + 58 + 84, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPet].maxHp, temp, 10);
                        StockFontBuffer(x + 400, y + 58 + 112, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPet].ai, temp, 10);
                        StockFontBuffer(x + 400, y + 58 + 140, FONT_PRIO_FRONT, 0, temp, 0);
                    }
                    // 如果有按下ok 把宠物资料秀在左边
                    if (nAuctionKind == 1 && pShowPet2 != NULL)
                    {
                        // 秀宠物名称
                        StockFontBuffer(x + 36, y + 28, FONT_PRIO_FRONT, 0, pet[nPetShow].name, 0);
                        // 秀宠物属性底下的底图
                        StockDispBuffer(x + 180, y + 137, DISP_PRIO_IME3, AUCTION_PET_STAT, 0);
                        // 秀宠物的属性
                        _itoa_s(pet[nPetShow].level, temp, 10);
                        StockFontBuffer(x + 180, y + 58, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPetShow].atk, temp, 10);
                        StockFontBuffer(x + 180, y + 58 + 28, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPetShow].def, temp, 10);
                        StockFontBuffer(x + 180, y + 58 + 56, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPetShow].quick, temp, 10);
                        StockFontBuffer(x + 180, y + 58 + 84, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPetShow].maxHp, temp, 10);
                        StockFontBuffer(x + 180, y + 58 + 112, FONT_PRIO_FRONT, 0, temp, 0);
                        _itoa_s(pet[nPetShow].ai, temp, 10);
                        StockFontBuffer(x + 180, y + 58 + 140, FONT_PRIO_FRONT, 0, temp, 0);
                    }
                }
                StockFontBuffer2(&AuctionStr);
                CheckNumber(AuctionMoney.buffer, -1);
                StockFontBuffer2(&AuctionMoney);
                StockDispBuffer(320, 240, DISP_PRIO_IME3, AUCTION_NEW, 1);
            }
        }
    }
}

void AuctionListWT(void)
{
    static ALD aldArea[10];   // server一次传最多十个委托的内容过来
    static int nButton[5][2]; // nButton[x][y]-> x:0->领回 x:1->观看资料 x:2->上一页 x:3->下一页 x:4->取消
    //                 y:0->被点选时的按钮编号 y:1->点选状态 0->没点选 1->有点选 2->无作用
    static int nList[10][2]; // 传来的十个委托有无被点选到 nList[x][y]-> x:0~9->十个委托内容
    //                                                                        y:0->被点选时的按钮编号 y:1->点选状态 0->没点选 1->有点选
    static int nListSelect; // 选到第几个列表
    int x, y, i;
    char temp[16], szSendMsg[2][260];
    static char id[16]; // 玩家的cd key
    static BOOL bPress;
    static BOOL bIsThisPlayerAuction; // 选到的选项是否为这个玩家的委托
    static int nListNum;              // 列表的编号
    static int nCurrentPage;          // 目前页数

    if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        if (pActAuctionWT != NULL)
        {
            windowTypeWN = -1;
            DeathAction(pActAuctionWT);
            pActAuctionWT = NULL;
            bNewData = FALSE;
            play_se(217, 320, 240);
            return;
        }
    }

    if (bNewData == TRUE)
    {
        ZeroMemory(aldArea, sizeof(aldArea));
        AuctionGetString(0, aldArea);
        bIsThisPlayerAuction = FALSE;
        bNewData = FALSE;
        nButton[2][1] = 2;
        nButton[3][1] = 2;
        if (buttonTypeWN & WINDOW_BUTTONTYPE_PRE)
            nButton[2][1] = 0;
        if (buttonTypeWN & WINDOW_BUTTONTYPE_NEXT)
            nButton[3][1] = 0;
    }

    // 如果目前没有产生委托介面
    if (pActAuctionWT == NULL)
    {
        // 产生委托介面
        pActAuctionWT = MakeWindowDisp(50, 15, 540, 450, AUCTION_LIST, -1);
        ZeroMemory(nList, sizeof(nList));
        nButton[0][1] = 2; // 一开始设定为无作用
        nButton[1][1] = 2;
        nButton[2][1] = 2;
        if (!(buttonTypeWN & WINDOW_BUTTONTYPE_NEXT))
            nButton[3][1] = 2;
        else
            nButton[3][1] = 0;
        nButton[4][1] = 0;
        nListSelect = -1;
        bPress = TRUE;
        nListNum = 0;
        bIsThisPlayerAuction = FALSE;
        extern char szUser[];
        CopyMemory(id, szUser, 16);
        ecb_crypt("f;encor1c", id, 16, DES_DECRYPT);
        // 播音效
        play_se(202, 320, 240);
    }
    else
    {
        // 如果视窗已经开好了
        if (pActAuctionWT->hp > 0)
        {
            // 座标设定
            x = pActAuctionWT->x;
            y = pActAuctionWT->y;

            // 如果滑鼠左键按着
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                for (i = 0; i < 10; i++)
                {
                    nList[i][1] = 0;
                    if (HitFontNo == nList[i][0])
                    {
                        // 如果点选到这个委托的玩家是这个委托的原委托者
                        if (!strcmp(aldArea[i].cdkey, id))
                        {
                            bIsThisPlayerAuction = TRUE;
                            nButton[0][1] = 0;
                        }
                        else
                            bIsThisPlayerAuction = FALSE;
                        nListSelect = i;
                        nList[i][1] = 1;
                        nButton[1][1] = 0;
                        bPress = TRUE;
                    }
                    if (i < 5)
                    {
                        if (HitDispNo == nButton[i][0] && nButton[i][1] == 0)
                        {
                            nButton[i][1] = 1;
                            bPress = TRUE;
                        }
                    }
                }
            }
            // 如果滑鼠左键放开
            if (mouse.onceState & MOUSE_LEFT_CRICK_UP)
            {
                if (!bIsThisPlayerAuction)
                    nButton[0][1] = 2;
                else
                    nButton[0][1] = 0;
                nButton[1][1] = 2;
                for (i = 0; i < 10; i++)
                {
                    if (nList[i][1])
                    {
                        nButton[1][1] = 0;
                        break;
                    }
                }
            }
            // 按了领回或按了观看资料  传aldArea[nListSelect].index回去server
            if (nButton[0][1] == 1 || nButton[1][1] == 1)
            {
                int type;

                if (nButton[0][1] == 1)
                    type = WINDOW_BUTTONTYPE_OK;
                else
                    type = WINDOW_BUTTONTYPE_YES;

                ZeroMemory(szSendMsg, sizeof(szSendMsg));
                sprintf_s(szSendMsg[1], "%d|", aldArea[nListSelect].index);
                makeEscapeString(szSendMsg[1], szSendMsg[0], sizeof(szSendMsg[0]));
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, type, szSendMsg[0]);
                windowTypeWN = -1;
                bNewData = FALSE;
                DeathAction(pActAuctionWT);
                pActAuctionWT = NULL;
                play_se(217, 320, 240);
                return;
            }
            // 按了上一页或按了下一页 传aldArea[0].index回去server
            if ((nButton[2][1] == 1 || nButton[3][1] == 1) && bPress)
            {
                int type;

                if (nButton[2][1] == 1)
                {
                    type = WINDOW_BUTTONTYPE_PRE;
                    nListNum -= 10;
                    nCurrentPage = nListNum / 10 + 1;
                }
                else
                {
                    type = WINDOW_BUTTONTYPE_NEXT;
                    nListNum += 10;
                    nCurrentPage = nListNum / 10 + 1;
                }
                ZeroMemory(szSendMsg, sizeof(szSendMsg));
                sprintf_s(szSendMsg[1], "%d|", aldArea[0].index);
                makeEscapeString(szSendMsg[1], szSendMsg[0], sizeof(szSendMsg[0]));
                lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, type, szSendMsg[0]);
                bPress = FALSE;
            }
            // 如果按了取消
            if (nButton[4][1])
            {
                StockDispBuffer(x + 439, y + 386, DISP_PRIO_IME3, AUCTION_CANCEL_Y_U + nButton[4][1], 2);
                windowTypeWN = -1;
                bNewData = FALSE;
                DeathAction(pActAuctionWT);
                pActAuctionWT = NULL;
                play_se(217, 320, 240);
                return;
            }

            // 显示及设定button出现的位置
            nButton[0][0] = StockDispBuffer(x + 102, y + 386, DISP_PRIO_IME3, AUCTION_GET_BACK_U + nButton[0][1], nButton[0][1] == 2 ? 0 : 2);
            nButton[1][0] = StockDispBuffer(x + 172, y + 386, DISP_PRIO_IME3, AUCTION_SEE_DATA_U + nButton[1][1], nButton[1][1] == 2 ? 0 : 2);
            nButton[2][0] = StockDispBuffer(x + 299, y + 386, DISP_PRIO_IME3, AUCTION_PAGE_UP_U + nButton[2][1], nButton[2][1] == 2 ? 0 : 2);
            nButton[3][0] = StockDispBuffer(x + 369, y + 386, DISP_PRIO_IME3, AUCTION_PAGE_DOWN_U + nButton[3][1], nButton[3][1] == 2 ? 0 : 2);
            nButton[4][0] = StockDispBuffer(x + 439, y + 386, DISP_PRIO_IME3, AUCTION_CANCEL_Y_U, 2);
            // 秀出目前页数
            sprintf_s(temp, "%d/9 页", nCurrentPage);
            StockFontBuffer(x + 420, y + 350, FONT_PRIO_FRONT, 0, temp, 0);
            for (i = 0; i < 10; i++)
            {
                if (strlen(aldArea[i].account) > 0)
                {
                    // 秀出编号
                    _itoa_s(nListNum + i, temp, 16, 10);
                    StockFontBuffer(x + 92, y + 110 + i * 25, FONT_PRIO_FRONT, 0, temp, 0);
                    // 秀出叙述
                    // 修改列表
                    if (windowTypeWN == WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY)
                    {
                        // 如果是这个玩家的委托,叙述秀红字;点选到秀黄字
                        if (!strcmp(aldArea[i].cdkey, id))
                            nList[i][0] = StockFontBuffer(x + 132, y + 110 + i * 25, FONT_PRIO_FRONT, nListSelect == i ? FONT_PAL_YELLOW : FONT_PAL_RED, aldArea[i].account, 2);
                        // 其它的叙述不能点选秀白字
                        else
                            StockFontBuffer(x + 132, y + 110 + i * 25, FONT_PRIO_FRONT, 0, aldArea[i].account, 0);
                    }
                    // 购买列表
                    else
                        nList[i][0] = StockFontBuffer(x + 132, y + 110 + i * 25, FONT_PRIO_FRONT, nListSelect == i ? FONT_PAL_YELLOW : 0, aldArea[i].account, 2);
                }
            }
            if (windowTypeWN == WINDOW_MESSAGETYPE_AUCTIONLIST_MODIFY)
                StockDispBuffer(244, 72, DISP_PRIO_IME3, AUCTION_LIST_1, 1);
            StockDispBuffer(320, 240, DISP_PRIO_IME3, AUCTION_LIST, 1);
        }
    }
}

void AuctionSurveyModifyWT(void)
{
    static ACTION *pShowPet = NULL;
    static ALD ald;           // 委托物的各项资料
    static int nButton[2][2]; // nButton[x][y]-> x:0->确定 x:1->取消
    //                 y:0->被点选时的按钮编号 y:1->点选状态 0->没点选 1->有点选
    static BOOL bPress;
    int x, y, i;
    char szSendMsg[2][8];
    char szPetAttrib[8];

    if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        if (pActAuctionWT != NULL)
        {
            windowTypeWN = -1;
            DeathAction(pActAuctionWT);
            pActAuctionWT = NULL;
            if (pShowPet != NULL)
            {
                DeathAction(pShowPet);
                pShowPet = NULL;
            }
            GetKeyInputFocus(&MyChatBuffer);
            play_se(217, 320, 240);
            return;
        }
    }
    // 如果目前没有产生购买介面
    if (pActAuctionWT == NULL)
    {
        // 产生购买介面
        pActAuctionWT = MakeWindowDisp(50, 15, 540, 450, AUCTION_SURVEY, -1);
        ZeroMemory(&ald, sizeof(ald));
        ZeroMemory(nButton, sizeof(nButton));
        AuctionGetString(1, &ald);
        bPress = FALSE;
        // 叙述文字初始化
        strcpy(AuctionStr.buffer, ald.account);
        AuctionStr.cnt = strlen(AuctionStr.buffer);
        AuctionStr.cursor = strlen(AuctionStr.buffer);
        AuctionStr.color = 0;
        AuctionStr.len = 100;
        AuctionStr.lineLen = 24;
        AuctionStr.lineDist = 20;
        AuctionStr.x = pActAuctionWT->x + 230;
        AuctionStr.y = pActAuctionWT->y + 265;
        AuctionStr.fontPrio = FONT_PRIO_FRONT;
        // 价钱文字初始化
        strcpy(AuctionMoney.buffer, ald.money);
        AuctionMoney.cnt = strlen(AuctionMoney.buffer);
        AuctionMoney.cursor = strlen(AuctionMoney.buffer);
        AuctionMoney.color = 0;
        AuctionMoney.len = 100;
        AuctionMoney.lineLen = 28;
        AuctionMoney.lineDist = 20;
        AuctionMoney.x = pActAuctionWT->x + 270;
        AuctionMoney.y = pActAuctionWT->y + 385;
        AuctionMoney.fontPrio = FONT_PRIO_FRONT;
        // 道具叙述初始化
        if (ald.kind == 2)
        {
            strcpy(AuctionItem.buffer, ald.item_effect_string);
            AuctionItem.cnt = 0;
            AuctionItem.cursor = 0;
            AuctionItem.color = 0;
            AuctionItem.len = 100;
            AuctionItem.lineLen = 28;
            AuctionItem.lineDist = 20;
            AuctionItem.x = pActAuctionWT->x + 220;
            AuctionItem.y = pActAuctionWT->y + 200;
            AuctionItem.fontPrio = FONT_PRIO_FRONT;
        }
        // 播音效
        play_se(202, 320, 240);
    }
    else
    {
        // 如果视窗已经开好了
        if (pActAuctionWT->hp > 0)
        {
            // 座标设定
            x = pActAuctionWT->x;
            y = pActAuctionWT->y;

            // 如果滑鼠左键按着
            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                for (i = 0; i < 2; i++)
                    if (HitDispNo == nButton[i][0])
                        nButton[i][1] = 1;
                bPress = TRUE;
            }
            // 如果滑鼠左键放开
            if (mouse.onceState & MOUSE_LEFT_CRICK_UP)
            {
                for (i = 0; i < 2; i++)
                    nButton[i][1] = 0;
                bPress = FALSE;
            }
            // 如果按下确定或取消
            if (nButton[0][1] || nButton[1][1])
            {
                if (nButton[0][1])
                {
                    ZeroMemory(szSendMsg, sizeof(szSendMsg));
                    sprintf_s(szSendMsg[0], "%d|", ald.index);
                    makeEscapeString(szSendMsg[0], szSendMsg[1], sizeof(szSendMsg[0]));
                    lssproto_WN_send(sockfd, nowGx, nowGy, indexWN, idWN, WINDOW_BUTTONTYPE_OK, szSendMsg[1]);
                }
                windowTypeWN = -1;
                DeathAction(pActAuctionWT);
                pActAuctionWT = NULL;
                if (pShowPet != NULL)
                {
                    DeathAction(pShowPet);
                    pShowPet = NULL;
                }
                GetKeyInputFocus(&MyChatBuffer);
                play_se(217, 320, 240);
                return;
            }

            if (windowTypeWN == WINDOW_MESSAGETYPE_AUCTIONMODIFY)
            {
                // 滑鼠在叙述区内
                if (MakeHitBox(AuctionStr.x, AuctionStr.y, AuctionStr.x + 170, AuctionStr.y + 95, DISP_PRIO_BOX2) == TRUE)
                {
                    GetKeyInputFocus(&AuctionStr);
                }

                // 滑鼠在价钱区内
                if (MakeHitBox(AuctionMoney.x, AuctionMoney.y, AuctionMoney.x + 115, AuctionMoney.y + 18, DISP_PRIO_BOX2) == TRUE)
                {
                    GetKeyInputFocus(&AuctionMoney);
                }
            }

            // 显示及设定button出现的位置
            nButton[0][0] = StockDispBuffer(x + 210, y + 433, DISP_PRIO_IME3, AUCTION_DECIDE_U, 2);
            nButton[1][0] = StockDispBuffer(x + 320, y + 433, DISP_PRIO_IME3, AUCTION_CANCEL_R_U, 2);
            // 显示委托人名字
            StockFontBuffer(x + 235, y + 22, FONT_PRIO_FRONT, 0, ald.name, 1);
            // 若是宠物
            if (ald.kind == 1)
            {
                if (pShowPet == NULL)
                    pShowPet = MakeAnimDisp(x + 280, y + 200, ald.grp_no, 0);
                // 显示宠物名字
                StockFontBuffer(x + 235, y + 55, FONT_PRIO_FRONT, 0, ald.sellname, 1);
                // 秀宠物属性底下的底图
                StockDispBuffer(x + 375, y + 170, DISP_PRIO_IME3, AUCTION_PET_STAT, 0);
                // 秀宠物资料
                _itoa_s(ald.lv, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 90, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
                _itoa_s(ald.act, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 120, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
                _itoa_s(ald.def, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 148, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
                _itoa_s(ald.dex, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 174, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
                _itoa_s(ald.hp, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 202, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
                _itoa_s(ald.loyal, szPetAttrib, 10);
                StockFontBuffer(x + 375, y + 230, FONT_PRIO_FRONT, 0, szPetAttrib, 1);
            }
            else
            {
                // 显示道具名字
                StockFontBuffer(x + 235, y + 55, FONT_PRIO_FRONT, ald.name_color, ald.sellname, 1);
                // 显示道具叙述
                StockFontBuffer2(&AuctionItem);
                // 秀道具图
                StockDispBuffer(x + 315, y + 155, DISP_PRIO_IME3, ald.grp_no, 0);
                // 秀底下的石板
                StockDispBuffer(x + 315, y + 155, DISP_PRIO_IME3, AUCTION_PLANK, 0);
            }
            // 秀出叙述及价钱
            StockFontBuffer2(&AuctionStr);
            CheckNumber(AuctionMoney.buffer, -1);
            StockFontBuffer2(&AuctionMoney);

            // 如果是修改委托内容,秀出"修改"的图把"购买"的字盖掉
            if (windowTypeWN == WINDOW_MESSAGETYPE_AUCTIONMODIFY)
                StockDispBuffer(x + 160, y, DISP_PRIO_IME3, AUCTION_SURVEY_1, 1);
            StockDispBuffer(320, 240, DISP_PRIO_IME3, AUCTION_SURVEY, 1);
        }
    }
}

void AuctionGetString(int nWhichOne, pALD ald)
{
    int i;
    char buf[256];
    char temp[256];

    ZeroMemory(buf, sizeof(buf));
    ZeroMemory(temp, sizeof(temp));
    makeStringFromEscaped(szpALD);
    // 若是开了list
    if (nWhichOne == 0)
    {
        for (i = 0; i < 10; i++)
        {
            getStringToken(szpALD, '|', i + 1, sizeof(buf), buf);
            makeStringFromEscaped(buf);
            getStringToken(buf, '|', 1, sizeof(temp), temp); // 索引值
            ald[i].index = atoi(temp);
            getStringToken(buf, '|', 2, sizeof(ald[i].cdkey), ald[i].cdkey); // 委托人的cdkey
            makeStringFromEscaped(ald[i].cdkey);
            getStringToken(buf, '|', 3, sizeof(ald[i].account), ald[i].account); // 叙述
            makeStringFromEscaped(ald[i].account);
        }
    }
    // 若是开了观看资料
    if (nWhichOne == 1)
    {
        getStringToken(szpALD, '|', 1, sizeof(temp), temp); // 索引值
        ald->index = atoi(temp);
        getStringToken(szpALD, '|', 2, sizeof(ald->name), ald->name); // 委托人名字
        makeStringFromEscaped(ald->name);
        getStringToken(szpALD, '|', 3, sizeof(ald->account), ald->account); // 叙述
        makeStringFromEscaped(ald->account);
        getStringToken(szpALD, '|', 4, sizeof(ald->money), ald->money); // 价钱
        getStringToken(szpALD, '|', 5, sizeof(temp), temp);             // 卖宠物或是物品
        ald->kind = atoi(temp);
        getStringToken(szpALD, '|', 6, sizeof(buf), buf); // 取得宠物或是物品的资料
        makeStringFromEscaped(buf);
        getStringToken(buf, '|', 1, sizeof(ald->sellname), ald->sellname); // 宠物或是物品的名称
        getStringToken(buf, '|', 2, sizeof(temp), temp);                   // 宠物或是物品的图号
        ald->grp_no = atoi(temp);
        // 若是卖宠物
        if (ald[0].kind == 1)
        {
            getStringToken(buf, '|', 3, sizeof(temp), temp); // 宠物的等级
            ald->lv = atoi(temp);
            getStringToken(buf, '|', 4, sizeof(temp), temp); // 宠物的攻
            ald->act = atoi(temp);
            getStringToken(buf, '|', 5, sizeof(temp), temp); // 宠物的防
            ald->def = atoi(temp);
            getStringToken(buf, '|', 6, sizeof(temp), temp); // 宠物的敏
            ald->dex = atoi(temp);
            getStringToken(buf, '|', 7, sizeof(temp), temp); // 宠物的血
            ald->hp = atoi(temp);
            getStringToken(buf, '|', 8, sizeof(temp), temp); // 宠物的忠诚
            ald->loyal = atoi(temp);
        }
        // 卖道具
        else
        {
            getStringToken(buf, '|', 3, sizeof(temp), temp); // 道具名称的颜色 0:白字 4:黄字 5:绿字
            ald->name_color = atoi(temp);
            getStringToken(buf, '|', 4, sizeof(temp), temp); // 道具叙述
            makeStringFromEscaped(temp);
            strncpy_s(ald->item_effect_string, temp, 64);
        }
    }
}

// 检查价钱是否为数字
void CheckNumber(char *buf, int num)
{
    int i;

    for (i = 0; i < (int)strlen(buf); i++)
        if (*(buf + i) < '0' || *(buf + i) > '9')
            *(buf + i) = '0';
    if (num > -1)
    {
        if (atoi(buf) > num)
            sprintf_s(buf, 260, "%d", num);
    }
    else if (atoi(buf) > CHAR_getMaxHaveGold())
        sprintf_s(buf, 260, "%d", CHAR_getMaxHaveGold());
}

// 移走叙述内的空白
void CheckSpace(char *buf)
{
    int i, j;
    char *temp;

    for (i = 0; i < (int)strlen(buf); i++)
    {
        if (*(buf + i) == 0x20)
        {
            temp = (buf + i);
            for (j = 0; j < (int)strlen(buf + i); j++)
            {
                *temp = *(temp + 1);
                temp++;
            }
            i = 0;
        }
    }
    strcat_s(buf, 260, "\0");
}
#endif
// Terry end

#ifdef _BLACK_MARKET
void closeBlackMarketWN(void)
{
    play_se(203, 320, 240);
    DeathAction(ptActMenuWin);
    ptActMenuWin = NULL;
    windowTypeWN = -1;
    sItem = -1;
    GetKeyInputFocus(&MyChatBuffer);
}
void initBlackMarket(char *data)
{
    int i = 0;
    char sTmp[128] = "";
    makeStringFromEscaped(data);
    getStrSplit((char *)msgWN, data, sizeof(msgWN[0]),
                sizeof(msgWN) / sizeof(msgWN[0]), sizeof(msgWN[0]) - 1);
    for (i = 0; i < 12; i++)
    {
        getStringToken(msgWN[i], '|', 1, sizeof(sTmp) - 1, sTmp);
        BMItem[i].iIndex = atoi(sTmp);
        getStringToken(msgWN[i], '|', 2, sizeof(sTmp) - 1, sTmp);
        BMItem[i].iGraphicsNum = atoi(sTmp);
        getStringToken(msgWN[i], '|', 3, sizeof(BMItem[i].iName) - 1, BMItem[i].iName);
    }
}

#ifdef _ADD_FAMILY_TAX // WON ADD 增加庄园税收
void FMTAXWindowsType(void)
{
    static int winX, winY;
    static int winW, winH;
    static int fontId[] = {-2, -2, -2, -2};
    static int btnId[] = {-2, -2, -2, -2};
    static int pushBtnFlag[4];

    int selBtnId;
    char dataBuf[256];
    char buf[256];
    int id;
    int i;

    static int fm_tax_num = 0;

    if (ptActMenuWin == NULL)
    {
        winW = 5;
        winH = 4;
        winX = (lpDraw->xSize - winW * 64) / 2;
        winY = (lpDraw->ySize - winH * 48) / 2;
        ptActMenuWin = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
        for (i = 0; i < sizeof(fontId) / sizeof(int); i++)
            fontId[i] = -2;
        play_se(202, 320, 240);
    }

    if (ptActMenuWin != NULL)
    {
        id = selFontId(fontId, sizeof(fontId) / sizeof(int));
        selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));

        if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
        {
            id = 100;
            closeFamilyTaxWN();
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK)

            switch (selBtnId)
            {
            case 0:
                sprintf_s(buf, "L|TAX|%d|%d", 1, fm_tax_num);
                lssproto_FM_send(sockfd, buf);
                closeFamilyTaxWN();
                return;
                break;
            case 1:
                closeFamilyTaxWN();
                return;
                break;
            }

        switch (selBtnId)
        {

            // Dec TAX
        case 2:

            if (mouse.onceState & MOUSE_LEFT_CRICK_UP && pushBtnFlag[2] == TRUE)
            {

                pushBtnFlag[2] = FALSE;
            }

            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                fm_tax_num--;
                if (fm_tax_num <= 0)
                {
                    fm_tax_num = 0;
                    play_se(220, 320, 240);
                }
            }

            break;

            // Add TAX
        case 3:

            if (mouse.onceState & MOUSE_LEFT_CRICK_UP && pushBtnFlag[3] == TRUE)
            {

                pushBtnFlag[3] = FALSE;
            }

            if (mouse.onceState & MOUSE_LEFT_CRICK)
            {
                fm_tax_num++;
                if (fm_tax_num >= 30)
                {
                    fm_tax_num = 30;
                    play_se(220, 320, 240);
                }
            }

            break;
        }

        for (int i = 0; i < 4; i++)
        {
            if (mouse.state & MOUSE_LEFT_CRICK && pushBtnFlag[i] == TRUE)
                pushBtnFlag[i] = TRUE;
            else
                pushBtnFlag[i] = FALSE;
        }

        if (ptActMenuWin != NULL && ptActMenuWin->hp >= 1)
        {
            sprintf_s(dataBuf, "%s  庄园税率调整", pc.familyName);
            StockFontBuffer(winX + 80, winY + 30,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);
#ifdef _READ16BITBMP
            if (!g_bUseAlpha)
                StockDispHLine(winX + 64, winY + 50, 3);
#endif
            StockFontBuffer(winX + 60, winY + 70,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, "税  率：", 0);

            sprintf_s(dataBuf, "%5d    ％", fm_tax_num);
            StockFontBuffer(winX + 200, winY + 70,
                            FONT_PRIO_FRONT, FONT_PAL_WHITE, dataBuf, 0);

            btnId[3] = StockDispBuffer(winX + 190 + 40, winY + 105, DISP_PRIO_IME3,
                                       CG_UP_BTN + pushBtnFlag[3], 2);

#ifdef _FMVER21
            if (pc.familyleader == FMMEMBER_LEADER ||
                pc.familyleader == FMMEMBER_ELDER)
            {
#else
            if (pc.familyleader == 1)
            {
#endif

                btnId[2] = StockDispBuffer(winX + 190, winY + 105, DISP_PRIO_IME3,
                                           CG_DOWN_BTN + pushBtnFlag[2], 2);

                btnId[0] = StockDispBuffer(winX + 100, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_OK_BTN, 2);
                btnId[1] = StockDispBuffer(winX + 100 + 120, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_CANCEL_BTN, 2);
            }
            else
            {
                btnId[0] = StockDispBuffer(winX + 100, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_OK_BTN, 2);
                btnId[1] = StockDispBuffer(winX + 100 + 120, winY + 160, DISP_PRIO_IME3,
                                           CG_TRADE_CANCEL_BTN, 2);
            }
        }
    }
}
#endif

void BMWindowType(void)
{
    static int btnId[2];
    int selBtnId;
    int x, y;
    int i;

    x = 0;
    y = 0;

    if (CheckMenuFlag() || joy_trg[0] & JOY_ESC || actBtn == 1 || menuBtn == 1 || disconnectServerFlag == TRUE || wnCloseFlag == 1)
    {
        closeBlackMarketWN();
        return;
    }

    if (ptActMenuWin == NULL)
    {
        for (i = 0; i < 2; i++)
            btnId[i] = -2;
        ptActMenuWin = MakeWindowDisp(x + 100, y + 100, 100, 100, NULL, -1);
        play_se(202, 320, 240);
    }
    else if (ptActMenuWin->hp >= 1)
    {
        selBtnId = focusGraId(btnId, sizeof(btnId) / sizeof(int));
        if (mouse.onceState & MOUSE_LEFT_CRICK)
        {
            switch (selBtnId)
            {
            case 0:
                if (sItem != -1)
                {
                    lssproto_BM_send(sockfd, BMItem[sItem].iIndex);
                    closeBlackMarketWN();
                    return;
                }
                break;
            case 1:
                closeBlackMarketWN();
                return;
                break;
            }
        }

        for (i = 0; i < 12; i++)
        {
            if (sItem == i)
                StockBoxDispBuffer(x + sItem % 4 * 62 + 18, y + sItem / 4 * 48 + 28, x + sItem % 4 * 62 + 68, y + sItem / 4 * 48 + 68, DISP_PRIO_BOX2, 213, 0);
            if (MakeHitBox(x + i % 4 * 62 + 18, y + i / 4 * 48 + 28, x + i % 4 * 62 + 68, y + i / 4 * 48 + 68, DISP_PRIO_BOX2))
            {
                char iName[64] = "";
                char *sp = BMItem[i].iName;
                int j = 0;
                int ip = 0;
                int lineChars = 0;
                char Tmp[256] = "";
                char *iip;

                iip = Tmp;

                while (1)
                {
                    const int charBytes = getUtf8SequenceLength(
                        BMItem[i].iName + ip,
                        strlen(BMItem[i].iName + ip));

                    if (BMItem[i].iName[ip] == '\0')
                    {
                        *iip = '\0';
                        StockFontBuffer(x + 20, y + 205 + j * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, Tmp, 1);
                        break;
                    }

                    const int copyBytes = charBytes > 0 ? charBytes : 1;
                    memcpy(iip, BMItem[i].iName + ip, copyBytes);
                    iip += copyBytes;
                    ip += copyBytes;
                    lineChars++;

                    if ((iip - Tmp) >= 33 || lineChars >= 16)
                    {
                        *iip = '\0';
                        StockFontBuffer(x + 20, y + 205 + j * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, Tmp, 1);
                        strcpy(Tmp, "");
                        iip = Tmp;
                        lineChars = 0;
                        j++;
                    }
                }

                if (mouse.onceState & MOUSE_LEFT_CRICK)
                {
                    sItem = i;
                }
                StockDispBuffer(x + i % 4 * 62 + 46, y + i / 4 * 48 + 48, DISP_PRIO_IME3, BMItem[i].iGraphicsNum + 1, 1);

                if (BMItem[i].iGraphicsNum == 22268)
                    strcpy(iName, "不知名的枪");
                else if (BMItem[i].iGraphicsNum == 22270)
                    strcpy(iName, "不知名的兜");
                else if (BMItem[i].iGraphicsNum == 22272)
                    strcpy(iName, "不知名的服");
                else if (BMItem[i].iGraphicsNum == 22274)
                    strcpy(iName, "不知名的铠");
                else if (BMItem[i].iGraphicsNum == 22276)
                    strcpy(iName, "不知名的斧头");
                else if (BMItem[i].iGraphicsNum == 22278)
                    strcpy(iName, "不知名的回旋标");
                else if (BMItem[i].iGraphicsNum == 22280)
                    strcpy(iName, "不知名的爪");
                else if (BMItem[i].iGraphicsNum == 22282)
                    strcpy(iName, "不知名的棍棒");
                else if (BMItem[i].iGraphicsNum == 22284)
                    strcpy(iName, "不知名的戒指");
                else if (BMItem[i].iGraphicsNum == 22286)
                    strcpy(iName, "不知名的首饰");
                else if (BMItem[i].iGraphicsNum == 22288)
                    strcpy(iName, "不知名的石");
                else if (BMItem[i].iGraphicsNum == 22290)
                    strcpy(iName, "不知名的耳饰");
                StockFontBuffer(x + 20, y + 180, FONT_PRIO_FRONT, FONT_PAL_YELLOW, iName, 1);
            }
            StockDispBuffer(x + i % 4 * 62 + 46, y + i / 4 * 48 + 48, DISP_PRIO_IME3, BMItem[i].iGraphicsNum, 1);
        }

        btnId[0] = StockDispBuffer(x + 80, y + 272, DISP_PRIO_IME3, CG_OK_BTN, 2);
        btnId[1] = StockDispBuffer(x + 190, y + 272, DISP_PRIO_IME3, CG_EXIT_BTN, 2);

        StockDispBuffer(x + 140, y + 150, DISP_PRIO_IME3, CG_BM_WND, 1);
    }
}
#endif
