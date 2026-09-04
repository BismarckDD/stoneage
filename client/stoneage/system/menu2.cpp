#include "systeminc/system.h"
//
#include "systeminc/font.h"
#include "sdk/caryime.h"
#include "game/anim_tbl.h"
#include "systeminc/pc.h"
#include "game/battle_menu.h"
#include "game/battle_proc.h"
#include "proto/lssproto_cli.h"
#include "systeminc/netmain.h"
#include "systeminc/loadsprbin.h"
#include "systeminc/savedata.h"
#include "systeminc/t_music.h"
#include "systeminc/menu.h"
#include "systeminc/tool.h"
#include "systeminc/map.h"
#include "systeminc/field.h"
#include "systeminc/pet_skillinfo.h"
#include "wgs/descrypt.h"
#include "systeminc/character.h"
#ifdef _TALK_WINDOW
#include "systeminc/talkwindow.h"
#endif

#ifdef _NPC_ITEMUP
void ShowItemup(int value, int x, int y)
{
  char itemup_msg[256] = "";
  const int sx=-50, sy=-20;
  const int s = ((((value)&0xff000000)>>24)&0x000000FF); // 攻
  const int t = ((((value)&0x00ff0000)>>16)&0x000000FF); // 防
  const int d = ((((value)&0x0000ff00)>>8 )&0x000000FF); // 敏
  const int h = ((value)&0x000000ff);//保留
  if ( s > 0 ) {
    sprintf_s( itemup_msg, "攻增加 %d", s ); 
    StockFontBuffer( x+sx , y+sy, FONT_PRIO_FRONT, FONT_PAL_WHITE, itemup_msg, 0 );
    sy -= 20;
  }
  if ( t > 0 ) {
    sprintf_s( itemup_msg, "防增加 %d", t ); 
    StockFontBuffer( x+sx , y+sy, FONT_PRIO_FRONT, FONT_PAL_WHITE, itemup_msg, 0 );
    sy -= 20;    
  }
  if ( d > 0 ) {
    sprintf_s( itemup_msg, "敏增加 %d", d ); 
    StockFontBuffer( x+sx , y+sy, FONT_PRIO_FRONT, FONT_PAL_WHITE, itemup_msg, 0 );
    sy -= 20;
  }
}
#endif

#ifdef _ITEM_COUNTDOWN
void ShowCounttime(int sectime, int x, int y)
{
    char buf[256];
    int  day;
    int  hour;
    int  min;

    if( sectime <= 0 ) return;
    if (sectime > 86400)
    {
        day = sectime / 86400;
        hour = (sectime / 3600) - (day * 24);
        min = (sectime / 60) - (day * 1440) - (hour * 60);
        sprintf_s( buf, "约%d天%d时%d分", day, hour, min); 
    }
    else if( sectime > 3600 ){
        hour = sectime/3600;
        min  = (sectime/60)-(hour*60);
        sprintf_s( buf,"约%d时%d分",hour,min); 
    }else if(sectime > 60 ){
        min  = sectime/60;
        sprintf_s( buf,"  约%d分",min); 
    }else{
        sprintf_s( buf,"  约1分"); 
    }
    //StockFontBuffer( x-20 , y-20, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf , 0 );    
    StockFontBuffer(x + 30 , y + 10, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf , 0);
}
#endif


#ifdef _PET_SKINS
static int btnShowSkinWin = -2;
static int btnShowSkinWinState = FALSE;
ACTION *petActSkins = NULL;
int originalPetSkin;

void initPetSkinWin()
{
    if (btnShowSkinWin != -2)
    {
        btnShowSkinWin = -2;
        btnShowSkinWinState = FALSE;
        if (petActSkins != NULL)
        {
            DeathAction(petActSkins);
            petActSkins = NULL;
        }
    }
}

void petChange(int graphNo)
{
  if (petActSkins != NULL)
  {
    petActSkins->anim_chr_no = graphNo;
  }
}

void petSkinProc()
{
    static int btnPrev;
    static int btnNext;
    static int btnPrevState;
    static int btnNextState;
    static int btnSkinSet;
    static int btnExit;
    static int x = 293, y = 120;

    btnShowSkinWin = StockDispBuffer(76, 290, DISP_PRIO_IME3, CG_PET_SKINS_SHOW_BTN + btnShowSkinWinState, 2);

    if (petActSkins == NULL && btnShowSkinWinState)
    {
        petActSkins = MakeAnimDisp(x + 65, y + 35, originalPetSkin, ANIM_DISP_PET);
    }

    if (HitDispNo == btnShowSkinWin){
        extern void ShowBottomLineString(int iColor, LPSTR lpstr);
        ShowBottomLineString(FONT_PAL_WHITE, "更换宠物皮肤。");
        if (mouse.onceState & MOUSE_LEFT_CRICK){

            if (btnShowSkinWinState){
                initPetSkinWin();
                play_se(203, 320, 240);
            }
            else
            {
                play_se(202, 320, 240);
                btnShowSkinWinState = TRUE;
            }
        }
    }

    if (btnShowSkinWinState)
    {
        //上一个皮肤
        if (HitDispNo == btnPrev){
            if (mouse.onceState & MOUSE_LEFT_CRICK){
                btnPrevState = TRUE;
            }
        }

        //下一个皮肤
        if (HitDispNo == btnNext){
            if (mouse.onceState & MOUSE_LEFT_CRICK){
                btnNextState = TRUE;
            }
        }

        //设定皮肤
        if (HitDispNo == btnSkinSet){
            if (mouse.onceState & MOUSE_LEFT_CRICK){

            }
        }

        //关闭预览
        if (HitDispNo == btnExit){
            if (mouse.onceState & MOUSE_LEFT_CRICK){
                initPetSkinWin();
            }
        }

        if (mouse.onceState & MOUSE_LEFT_CRICK_UP)
        {
            btnPrevState = FALSE;
            btnNextState = FALSE;
        }

        StockDispBuffer(358, 133, DISP_PRIO_MENU, CG_PET_SKINS_WIN, 0);
        btnPrev = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_PET_SKINS_PREV_BTN + btnPrevState, 2);
        btnNext = StockDispBuffer(x + 132, y, DISP_PRIO_IME3, CG_PET_SKINS_NEXT_BTN + btnNextState, 2);
        btnSkinSet = StockDispBuffer(323, 246, DISP_PRIO_IME3, CG_PET_SKINS_SET, 2);
        btnExit = StockDispBuffer(395, 246, DISP_PRIO_IME3, 55252, 2);
    }
}
#endif


void CloseMagicItemWin()
{
  extern ACTION* pActMenuWnd2,*pActPet2;
  DeathAction(pActMenuWnd2);
  pActMenuWnd2 = NULL;
  DeathAction(pActPet2);
  pActPet2 = NULL;
  extern int ItemMixPetNo, itemNo;
  ItemMixPetNo = -1;
  MenuToggleFlag ^= JOY_CTRL_I;
  // ??????????????????
  if (MenuToggleFlag & JOY_CTRL_S){
    MenuToggleFlag ^= JOY_CTRL_S;    // CTRL + S ??
    extern void DeathMenuAction(void);
    DeathMenuAction();
  }
  play_se(203, 320, 240);
  itemNo = -1;
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
  extern bool Moneyflag;
  GetKeyInputFocus(&MyChatBuffer);
  Moneyflag = false;
#endif
}


#ifdef _AIDENGLU_
BOOL 自动登陆窗口 = FALSE;
ACTION *p自动登陆窗口 = NULL;
ACTION * 方向动作;
ACTION * 人物动作;
extern struct GameServer gmsv[];
extern struct GameGroup  gmgroup[];
void 自动登陆窗口回调()
{
    static int 方向窗口是否打开;
    static int btnId[15], btnState[15];
    static int x = 0, y = 0;
    static DWORD dwPressTime;
    static char msg[][8] = {
        "队长",
        "队员",
        "单人"
    };
    static char  chardir[][8] = {
        "下",
        "左下",
        "左",
        "左上",
        "上",
        "右上",
        "右",
        "右下"
    };
    if (p自动登陆窗口 == NULL){
        方向窗口是否打开 = FALSE;
        方向动作 = NULL;
        人物动作 = NULL;
        int w = 412;
        int h = 301;
        x = (lpDraw->xSize - w) / 2;
        y = (lpDraw->ySize - h) / 2;
        p自动登陆窗口 = MakeWindowDisp(x, y, w, h, 55241, -1, FALSE);
        for (int i = 0; i < 14; i++){
            btnId[i] = -2;
            if (i < 10)
                btnState[i] = 0;
        }
        dwPressTime = 0;
    }
    else{
        if (p自动登陆窗口->hp >= 1){
            if (joy_trg[0] & JOY_ESC){
                自动登陆窗口 = FALSE;
                DeathAction(p自动登陆窗口);
                DeathAction(方向动作);
                DeathAction(人物动作);
                人物动作 = NULL;
                方向窗口是否打开 = FALSE;
                方向动作 = NULL;
                p自动登陆窗口 = NULL;
                actBtn = 0;
                return;
            }
            if (dwPressTime){
                if (TimeGetTime() > (dwPressTime + 100)){
                    dwPressTime = 0;
                    if (btnState[0] == 1){//大区减
                        btnState[0] = 0;
                        play_se(217, 320, 240);
                        while (TRUE){
                            PcLanded.大区--;
                            if (PcLanded.大区 < 0){
                                PcLanded.大区 = 18;
                            }
                            if (gmgroup[PcLanded.大区].used){
                                break;
                            }
                        }
                    }
                    if (btnState[1] == 1){//大区加
                        btnState[1] = 0;
                        play_se(217, 320, 240);
                        while (TRUE){
                            PcLanded.大区++;
                            if (PcLanded.大区 > 18){
                                PcLanded.大区 = 0;
                            }
                            if (gmgroup[PcLanded.大区].used){
                                break;
                            }
                        }
                    }

                    if (btnState[2] == 1){//小区减
                        btnState[2] = 0;
                        play_se(217, 320, 240);
                        if (PcLanded.大区 != -1)
                        {
                            while (TRUE){
                                PcLanded.小区--;
                                if (PcLanded.小区 < 0){
                                    PcLanded.小区 = gmgroup[PcLanded.大区].num - 1;
                                }
                                if (gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].used){
                                    break;
                                }
                            }
                        }
                    }
                    if (btnState[3] == 1){//小区加
                        btnState[3] = 0;
                        play_se(217, 320, 240);
                        if (PcLanded.大区 != -1)
                        {
                            while (TRUE){
                                PcLanded.小区++;
                                if (PcLanded.小区 >= gmgroup[PcLanded.大区].num){
                                    PcLanded.小区 = 0;
                                }
                                if (gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].used){
                                    break;
                                }
                            }
                        }
                    }
                    if (btnState[4] == 1){//队模减
                        btnState[4] = 0;
                        play_se(217, 320, 240);
                        PcLanded.队模--;
                        if (PcLanded.队模 < 0){
                            PcLanded.队模 = 2;
                        }
                        if (PcLanded.队模 == 0 || PcLanded.队模 == 2){
                            PcLanded.是否自动喊话 = FALSE;
                        }
                        if (PcLanded.队模 == 1){
                            PcLanded.是否自动遇敌 = FALSE;
                        }
                    }
                    if (btnState[5] == 1){//队模加
                        btnState[5] = 0;
                        play_se(217, 320, 240);
                        PcLanded.队模++;
                        if (PcLanded.队模 > 2){
                            PcLanded.队模 = 0;
                        }
                        if (PcLanded.队模 == 0 || PcLanded.队模 == 2){
                            PcLanded.是否自动喊话 = FALSE;
                        }
                        if (PcLanded.队模 == 1){
                            PcLanded.是否自动遇敌 = FALSE;
                        }
                    }
                    if (btnState[6] == 1){//角色减
                        btnState[6] = 0;
                        play_se(217, 320, 240);
                        while (TRUE){
                            PcLanded.人物--;
                            if (PcLanded.人物 < 0){
                                PcLanded.人物 = 3;
                            }
                            if (PcLanded.登陆人物名称[PcLanded.人物][0]) break;
                        }
                    }
                    if (btnState[7] == 1){//角色加
                        btnState[7] = 0;
                        play_se(217, 320, 240);
                        while (TRUE){
                            PcLanded.人物++;
                            if (PcLanded.人物 > 3){
                                PcLanded.人物 = 0;
                            }
                            if (PcLanded.登陆人物名称[PcLanded.人物][0]) break;
                        }
                    }
                    if (btnState[8] == 1){//确定
                        btnState[8] = 0;
                        play_se(217, 320, 240);
                        DeathAction(p自动登陆窗口);
                        p自动登陆窗口 = NULL;
                        自动登陆窗口 = FALSE;
                        if (方向动作){
                            DeathAction(方向动作);
                            方向动作 = NULL;
                            DeathAction(人物动作);
                            人物动作 = NULL;
                        }
                        extern short actBtn;
                        actBtn = 0;
                        return;
                    }
                    if (btnState[9] == 1){//取消
                        btnState[9] = 0;
                        play_se(217, 320, 240);
                        DeathAction(p自动登陆窗口);
                        p自动登陆窗口 = NULL;
                        自动登陆窗口 = FALSE;
                        if (方向动作){
                            DeathAction(方向动作);
                            方向动作 = NULL;
                            DeathAction(人物动作);
                            人物动作 = NULL;
                        }
                        extern short actBtn;
                        actBtn = 0;
                        return;
                    }
                    if (btnState[10] == 1){//方向
                        btnState[10] = 0;
                        play_se(217, 320, 240);
                        DeathAction(方向动作);
                        方向动作 = NULL;
                        DeathAction(人物动作);
                        人物动作 = NULL;
                        方向窗口是否打开 = FALSE;
                    }
                }
            }
            else{
                for (int i = 0; i < 10; i++){
                    if (HitDispNo == btnId[i]){
                        if (i == 0 || i == 1) ShowBottomLineString(FONT_PAL_WHITE, "设置大区。");
                        if (i == 2 || i == 3) ShowBottomLineString(FONT_PAL_WHITE, "设置线路,需设置大区方可设置。");
                        if (i == 4 || i == 5) ShowBottomLineString(FONT_PAL_WHITE, "队长：可设自动遇敌 队员：可设喊话和自动组队 单人：可设自动遇敌 ps：人物掉线重登自动开启AI模式。");
                        if (i == 6 || i == 7) ShowBottomLineString(FONT_PAL_WHITE, "设置登陆人物。");
                        if (mouse.onceState & MOUSE_LEFT_CRICK){
                            dwPressTime = TimeGetTime();
                            btnState[i] = 1;
                            break;
                        }
                    }
                }
                if (HitDispNo == btnId[14]){
                    if (mouse.onceState & MOUSE_LEFT_CRICK){
                        dwPressTime = TimeGetTime();
                        btnState[10] = 1;
                    }
                }
            }

            extern int 自动登陆是否开启;
            if (HitFontNo == btnId[10]){
                ShowBottomLineString(FONT_PAL_WHITE, "登陆游戏中可按F9开启和关闭,队员模式登陆游戏后喊话可关闭该功能停止喊话。");
                if (mouse.onceState & MOUSE_LEFT_CRICK){
                    if (PcLanded.大区 != -1 && PcLanded.小区 != -1 && PcLanded.人物 != -1){
                        自动登陆是否开启 = !自动登陆是否开启;
                        if (!自动登陆是否开启){
                            extern int 是否重登组队, 是否重登喊话, 是否重开登组队_1;
                            是否重登组队 = FALSE;
                            是否重登喊话 = FALSE;
                            是否重开登组队_1 = FALSE;
                        }
                    }
                    else{
                        StockChatBufferLine("请把大区、线路和角色选择后才能操作！", FONT_PAL_RED);
                    }
                }
            }
            if (HitFontNo == btnId[12]){
                if (mouse.onceState & MOUSE_LEFT_CRICK){
                    ShowBottomLineString(FONT_PAL_WHITE, "队长和单人可设置该功能。");
                    if (PcLanded.队模 == 0 || PcLanded.队模 == 2)
                        PcLanded.是否自动遇敌 = !PcLanded.是否自动遇敌;
                    else{
                        PcLanded.是否自动遇敌 = FALSE;
                        StockChatBufferLine("遇敌模式只有队长和单人可开启！", FONT_PAL_RED);
                    }
                }
            }
            if (HitFontNo == btnId[13]){
                if (mouse.onceState & MOUSE_LEFT_CRICK){
                    ShowBottomLineString(FONT_PAL_WHITE, "只有队员可设置该功能。");
                    if (PcLanded.队模 == 1)
                        PcLanded.是否自动喊话 = !PcLanded.是否自动喊话;
                    else{
                        PcLanded.是否自动喊话 = FALSE;
                        StockChatBufferLine("自动喊话只有队员模式可开启！", FONT_PAL_RED);
                    }
                }
            }
            if (HitFontNo == btnId[11]){
                ShowBottomLineString(FONT_PAL_WHITE, "掉线人物登陆后人物的方向。");
                if (mouse.onceState & MOUSE_LEFT_CRICK){
                    if (方向窗口是否打开){
                        方向窗口是否打开 = FALSE;
                        if (方向动作){
                            DeathAction(方向动作);
                            方向动作 = NULL;
                            DeathAction(人物动作);
                            人物动作 = NULL;
                        }
                    }
                    else 方向窗口是否打开 = TRUE;
                }
            }
            int 方向偏移X = 0;
            if (方向窗口是否打开) 方向偏移X = -90;
            if (方向窗口是否打开){
                if (!方向动作){
                    方向动作 = MakeWindowDisp(607 + 方向偏移X, 147, 192, 148, 55242, -1, FALSE);
                    人物动作 = MakeAnimDisp(660 + 方向偏移X, 240, pc.graNo, 0);
                    人物动作->anim_ang = PcLanded.人物方向;
                    pattern(人物动作, ANM_NOMAL_SPD, ANM_LOOP);
                    btnId[14] = -2;
                    btnState[10] = 0;
                }
                else if (方向动作->hp >= 1){
                    StockDispBuffer(((WINDOW_DISP *)方向动作->pYobi)->mx, ((WINDOW_DISP *)方向动作->pYobi)->my, DISP_PRIO_MENU, 55242, 1);
                    PcLanded.人物方向 = 人物动作->anim_ang;
                    btnId[14] = StockDispBuffer(((WINDOW_DISP *)方向动作->pYobi)->mx - 55, ((WINDOW_DISP *)方向动作->pYobi)->my - 90, DISP_PRIO_IME3, 26262 + btnState[10], 2);
                }
            }
            StockDispBuffer(((WINDOW_DISP *)p自动登陆窗口->pYobi)->mx + 方向偏移X, ((WINDOW_DISP *)p自动登陆窗口->pYobi)->my, DISP_PRIO_MENU, 55241, 1);
            btnId[0] = StockDispBuffer(x + 128 + 方向偏移X, y + 92 + 70, DISP_PRIO_IME3, 26064 + btnState[0], 2);
            btnId[1] = StockDispBuffer(x + 109 + 方向偏移X, y + 107 + 71, DISP_PRIO_IME3, 26066 + btnState[1], 2);

            btnId[2] = StockDispBuffer(x + 273 + 方向偏移X, y + 92 + 70, DISP_PRIO_IME3, 26064 + btnState[2], 2);
            btnId[3] = StockDispBuffer(x + 254 + 方向偏移X, y + 107 + 71, DISP_PRIO_IME3, 26066 + btnState[3], 2);

            btnId[4] = StockDispBuffer(x + 128 + 方向偏移X, y + 92 + 106, DISP_PRIO_IME3, 26064 + btnState[4], 2);
            btnId[5] = StockDispBuffer(x + 109 + 方向偏移X, y + 107 + 107, DISP_PRIO_IME3, 26066 + btnState[5], 2);

            btnId[6] = StockDispBuffer(x + 128 + 方向偏移X, y + 92 + 142, DISP_PRIO_IME3, 26064 + btnState[6], 2);
            btnId[7] = StockDispBuffer(x + 109 + 方向偏移X, y + 107 + 143, DISP_PRIO_IME3, 26066 + btnState[7], 2);

            btnId[8] = StockDispBuffer(x + 207 + 方向偏移X, y + 152, DISP_PRIO_IME3, 26262 + btnState[8], 2);
            btnId[9] = StockDispBuffer(x + 207 + 方向偏移X, y + 152, DISP_PRIO_IME3, 26264 + btnState[9], 2);

            char moji[256];
            if (PcLanded.大区 != -1){
                CenteringStr(gmgroup[PcLanded.大区].name, moji, CHAR_NAME_LEN);
                StockFontBuffer(x + 105 + 方向偏移X, y + 64, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);
            }
            if (PcLanded.队模 != -1){
                StockFontBuffer(x + 150 + 方向偏移X, y + 100, FONT_PRIO_FRONT, FONT_PAL_YELLOW, msg[PcLanded.队模], 0);
            }
            if (PcLanded.人物 != -1){
                CenteringStr(PcLanded.登陆人物名称[PcLanded.人物], moji, CHAR_NAME_LEN);
                StockFontBuffer(x + 105 + 方向偏移X, y + 136, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);
            }

            if (自动登陆是否开启)
                btnId[10] = StockFontBuffer(x + 150 + 方向偏移X, y + 171, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "开启", 2);
            else
                btnId[10] = StockFontBuffer(x + 150 + 方向偏移X, y + 171, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "关闭", 2);
            if (PcLanded.小区 != -1){
                StockFontBuffer(x + 305 + 方向偏移X, y + 64, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].name, 0);
            }
            btnId[11] = StockFontBuffer(x + 320 + 方向偏移X, y + 100, FONT_PRIO_FRONT, FONT_PAL_YELLOW, chardir[PcLanded.人物方向], 2);
            if (PcLanded.是否自动遇敌)
                btnId[12] = StockFontBuffer(x + 320 + 方向偏移X, y + 136, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "开启", 2);
            else
                btnId[12] = StockFontBuffer(x + 320 + 方向偏移X, y + 136, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "关闭", 2);
            if (PcLanded.是否自动喊话)
                btnId[13] = StockFontBuffer(x + 320 + 方向偏移X, y + 171, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "开启", 2);
            else
                btnId[13] = StockFontBuffer(x + 320 + 方向偏移X, y + 171, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "关闭", 2);
        }
    }
}

#endif
