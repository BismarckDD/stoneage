#define __MOUSE_CPP__
#include "systeminc/system.h"
//
#include "systeminc/mouse.h"
//
#include "game/anim_tbl.h"
#include "game/battle_menu.h"
#include "systeminc/ime_sa.h"
#include "systeminc/loadrealbin.h"
#include "systeminc/login.h"
#include "systeminc/map.h"
#include "systeminc/menu.h"
#include "systeminc/pc.h"

#define MOUSE_HIT_SIZE_X 48
#define MOUSE_HIT_SIZE_Y 48
#define MOUSE_AUTO_REPEATE_TIME 100

// Robin
extern int transmigrationEffectFlag;

// Init mouse icon related.
void MouseInit(void) {
  memset(&mouse, 0, sizeof(MOUSE));
  mouse.itemNo = -1;
}

// ????????ㄅ????? //////////////////////////////////////////////
void MouseNowPoint(int x, int y) {
  if (gResolutionMode == 1) {
    if (x > 640)
      x = 640;
    if (y > 480)
      y = 480;
  } else {
    if (x > lpDraw->xSize)
      x = lpDraw->xSize;
    if (y > lpDraw->ySize)
      y = lpDraw->ySize;
  }
  // ?火????
  //???????????????
  if (gResolutionMode == 1) {
    mouse.nowPoint.x = x * 2;
    mouse.nowPoint.y = y * 2;
  } else {
    mouse.nowPoint.x = x;
    mouse.nowPoint.y = y;
  }
}

// ????????????????????ㄅ????? //////////////////////
void MouseCrickLeftDownPoint(int x, int y) {
  // ┤????
  mouse.state |= MOUSE_LEFT_CRICK;
  mouse.onceState |= MOUSE_LEFT_CRICK;
  mouse.autoState |= MOUSE_LEFT_CRICK;
  // ?火????
  mouse.crickLeftDownPoint.x = x;
  mouse.crickLeftDownPoint.y = y;
  // ????????????????　叉??〈??
  mouse.beforeLeftPushTime = TimeGetTime();
  mouse.leftPushTime = 0;
}

// ???????????????????ㄅ????? ////////////////////////
void MouseCrickLeftUpPoint(int x, int y) {
  // ┤????
  mouse.state &= (~MOUSE_LEFT_CRICK);
  mouse.onceState |= MOUSE_LEFT_CRICK_UP;
  mouse.autoState &= (~MOUSE_LEFT_CRICK);
  // ?火????
  mouse.crickLeftUpPoint.x = x;
  mouse.crickLeftUpPoint.y = y;
  // ????????????????
  mouse.beforeLeftPushTime = 0;
  mouse.leftPushTime = 0;
}

// ??????????????????ㄅ????? //////////////////////////
void MouseDblCrickLeftUpPoint(int x, int y) {
  // ┤????
  // mouse.onceState |= ( MOUSE_LEFT_DBL_CRICK | MOUSE_LEFT_CRICK );
  mouse.state |= MOUSE_LEFT_CRICK;
  mouse.onceState |= MOUSE_LEFT_DBL_CRICK;
  mouse.onceState |= MOUSE_LEFT_CRICK;
  mouse.autoState |= MOUSE_LEFT_CRICK;
  // ?火????
  mouse.crickLeftDownPoint.x = x;
  mouse.crickLeftDownPoint.y = y;
  // ????????????????　叉??〈??
  mouse.beforeLeftPushTime = TimeGetTime();
  mouse.leftPushTime = 0;
}

// ???????????????????ㄅ????? ////////////////////////
void MouseCrickRightDownPoint(int x, int y) {
  // ┤????
  mouse.state |= MOUSE_RIGHT_CRICK;
  mouse.onceState |= MOUSE_RIGHT_CRICK;
  mouse.autoState |= MOUSE_RIGHT_CRICK;
  // ?火????
  mouse.crickRightDownPoint.x = x;
  mouse.crickRightDownPoint.y = y;
  // ????????????????　叉??〈??
  mouse.beforeRightPushTime = TimeGetTime();
  mouse.rightPushTime = 0;
}

// ???????????????????ㄅ????? ////////////////////////
void MouseCrickRightUpPoint(int x, int y) {
  // ┤????
  mouse.state &= (~MOUSE_RIGHT_CRICK);
  mouse.onceState |= MOUSE_RIGHT_CRICK_UP;
  mouse.autoState &= (~MOUSE_RIGHT_CRICK);
  // ?火????
  mouse.crickRightUpPoint.x = x;
  mouse.crickRightUpPoint.y = y;
  // ????????????????
  mouse.beforeRightPushTime = 0;
  mouse.rightPushTime = 0;
}

// ??????????????????ㄅ????? //////////////////////////
void MouseDblCrickRightUpPoint(int x, int y) {
  // ┤????
  mouse.state |= MOUSE_RIGHT_CRICK;
  mouse.onceState |= MOUSE_RIGHT_DBL_CRICK;
  mouse.onceState |= MOUSE_RIGHT_CRICK;
  mouse.autoState |= MOUSE_RIGHT_CRICK;
  // ?火????
  mouse.crickRightDownPoint.x = x;
  mouse.crickRightDownPoint.y = y;
  // ????????????????　叉??〈??
  mouse.beforeRightPushTime = TimeGetTime();
  mouse.rightPushTime = 0;
}

// ?????  ///////////////////////////////////////////////////////////////
void MouseProc(void) {
  static UINT leftPushTimeBak;
  static UINT rightPushTimeBak;

  // ┤????
  mouse.onceState = MOUSE_NO_CRICK;
  // ???????????????????
  if (mouse.beforeLeftPushTime > 0) {
    mouse.leftPushTime = TimeGetTime() - mouse.beforeLeftPushTime;
  }
  // ???????????????????
  if (mouse.beforeRightPushTime > 0) {
    mouse.rightPushTime = TimeGetTime() - mouse.beforeRightPushTime;
  }
  // ?????????
  // ????????????
  // ???企??????
  if (mouse.leftPushTime > 500) {
    // ???????
    if (leftPushTimeBak == 0) {
      mouse.autoState |= MOUSE_LEFT_CRICK;  // ?????
      leftPushTimeBak = mouse.leftPushTime; // ?????
    } else
      // ????????刺?
      if (mouse.leftPushTime - leftPushTimeBak >= MOUSE_AUTO_REPEATE_TIME) {
        mouse.autoState |= MOUSE_LEFT_CRICK;    // ?????
        leftPushTimeBak = mouse.leftPushTime;   // ?????
      } else {                                  // ?????????
        mouse.autoState &= (~MOUSE_LEFT_CRICK); // ??????
      }
  } else {
    mouse.autoState &= (~MOUSE_LEFT_CRICK); // ??????
    leftPushTimeBak = 0;
  }

  // ????????????
  // ???企??????
  if (mouse.rightPushTime > 500) {
    // ???????
    if (rightPushTimeBak == 0) {
      mouse.autoState |= MOUSE_RIGHT_CRICK;   // ?????
      rightPushTimeBak = mouse.rightPushTime; // ?????
    } else
      // ????????刺?
      if (mouse.rightPushTime - rightPushTimeBak >= MOUSE_AUTO_REPEATE_TIME) {
        mouse.autoState |= MOUSE_RIGHT_CRICK;    // ?????
        rightPushTimeBak = mouse.rightPushTime;  // ?????
      } else {                                   // ?????????
        mouse.autoState &= (~MOUSE_RIGHT_CRICK); // ??????
      }
  } else {
    mouse.autoState &= (~MOUSE_RIGHT_CRICK); // ??????
    rightPushTimeBak = 0;
  }

  // ????????????
  // if( mouse.rightPushTime > 0 ){
  //}else mouse.state &= (~MOUSE_RIGHT_AUTO_CRICK);
  // ????????????
  // if( !WindowMode ){
  // if( 0 <= mouse.nowPoint.x && mouse.nowPoint.x < 640 &&
  //    0 <= mouse.nowPoint.y && mouse.nowPoint.y < 480 ){

  // ?????????
  if (MouseCursorFlag == FALSE) {
    if (mouse.flag == FALSE) {
      StockDispBuffer(mouse.nowPoint.x + 16, mouse.nowPoint.y + 16,
                      DISP_PRIO_MOUSE, CG_MOUSE_CURSOR, 0);
    }
  }
}
// ????︻????? *********************************************************/
void CheckGroupSelect(int no) {
  int i;
  DISP_INFO *pDispInfo;
  DISP_SORT *pDispSort;
  /* ???牙?????????牙??！????????? */
  for (i = 0; i < DispBuffer.DispCnt; i++) {

    pDispInfo = DispBuffer.DispInfo + i;
    pDispSort = DispBuffer.DispSort + i;

    // ??????????
    if (pDispInfo->hitFlag == no) {
      // ??????????
      if (pDispSort->dispPrio >= DISP_PRIO_MENU) {
        // ????牙????????????
#ifndef __CARYTEST
        StockBoxDispBuffer(
            pDispInfo->x - 2, pDispInfo->y - 2,
            pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
            pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
            DISP_PRIO_BOX2, BoxColor, 0);
#else
        StockBoxDispBuffer(
            pDispInfo->x - 2, pDispInfo->y - 2,
            pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
            pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
            DISP_PRIO_BOX2, BoxColor, 0);
#endif
      } else {
        // ????牙????????????
#ifndef __CARYTEST
        StockBoxDispBuffer(
            pDispInfo->x - 2, pDispInfo->y - 2,
            pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
            pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
            DISP_PRIO_BOX, BoxColor, 0);
        // pDispSort->dispPrio, 250, 0 );
#else
        StockBoxDispBuffer(
            pDispInfo->x - 2, pDispInfo->y - 2,
            pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
            pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
            DISP_PRIO_BOX, BoxColor, 0);
#endif
      }
    }
  }
}

UCHAR BoxColorTbl[] = {
    // 255, 255, 255, 255, 255,
    // 8,8,8,8,8,
    250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
    250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
    250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
    15,  15,  15,  15,  15,  2,   2,   2,   2,   2,   15,  15,  15,  15,  15,
    // 250,250,250,250,250,
    // 8,8,8,8,8,
};


// 2026.08.31 是什么Hit到MouseCursor呢？
void HitMouseCursor(void) {
  int i;                              // 
  int hitFlag = FALSE;                //
  static int cnt = 0;                 //
  int itemNameColor = FONT_PAL_WHITE; //

  DISP_SORT *pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt - 1;
  DISP_INFO *pDispInfo;

  if (cnt >= sizeof(BoxColorTbl) - 1)
    cnt = 0;
  else
    cnt++;
  BoxColor = BoxColorTbl[cnt];
#ifdef _ITEM_PATH
  ITEMPATHFLAG = FALSE;
#endif
  for (i = 0; i < FontCnt; i++) {
    if (FontBuffer[i].hitFlag == 0)
      continue;
    SIZE textExtent = {0, FONT_SIZE};
    if (!GetFontTextExtent(FontBuffer[i].str, &textExtent)) {
      textExtent.cx = getTextLength(FontBuffer[i].str);
      textExtent.cy = FONT_SIZE;
    }
    const int strWidth = textExtent.cx;
    const int strHeight = textExtent.cy > 0 ? textExtent.cy : FONT_SIZE;

    if ((mouse.nowPoint.x <= FontBuffer[i].x + strWidth + 2) &&
        (mouse.nowPoint.x >= FontBuffer[i].x - 2) &&
        (mouse.nowPoint.y <= FontBuffer[i].y + strHeight + 2) &&
        (mouse.nowPoint.y >= FontBuffer[i].y - 2))
    {
      HitFontNo = i;
      if (FontBuffer[i].hitFlag == 2) {
        StockBoxDispBuffer(FontBuffer[i].x - 3,
#ifdef _NEWFONT_
                           FontBuffer[i].y - 2,
#else
                           FontBuffer[i].y - 4,
#endif
                           FontBuffer[i].x + strWidth + 2,
#ifdef _NEWFONT_
                           FontBuffer[i].y + strHeight + 6,
#else
                           FontBuffer[i].y + strHeight + 4,
#endif

                           DISP_PRIO_BOX2, BoxColor, 0);

        SortDispBuffer();
      }
      HitDispNo = -1;
      mouse.level = DISP_PRIO_MENU;
      if (TaskBarFlag == FALSE && !transmigrationEffectFlag) {
        ShowBottomLineString(itemNameColor, OneLineInfoStr);
      }
      OneLineInfoStr[0] = '\0';
      return; //
    }
  }
  HitFontNo = -1;

  for (i = DispBuffer.DispCnt - 1; i >= 0; i--, pDispSort--) {
    pDispInfo = DispBuffer.DispInfo + pDispSort->no;
    if (pDispInfo->hitFlag == 0)
      continue;
    if (LoadBmp(pDispInfo->bmpNo) == FALSE)
      continue;

    // ?????????
    if (pDispInfo->pAct == NULL) {
      // ?????刪叉
#ifndef __CARYTEST
      if (mouse.nowPoint.x <=
              pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width &&
          pDispInfo->x <= mouse.nowPoint.x &&
          mouse.nowPoint.y <=
              pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height &&
          pDispInfo->y <= mouse.nowPoint.y)
#else
      if (mouse.nowPoint.x <=
              pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width &&
          pDispInfo->x <= mouse.nowPoint.x &&
          mouse.nowPoint.y <=
              pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height &&
          pDispInfo->y <= mouse.nowPoint.y)
#endif
        hitFlag = TRUE;
    } else {
      // World character x/y is the projected map position (feet). Sprite
      // padding and animation offsets can move the old bitmap-bottom hit box
      // away from that position, so retain it and also accept the foot area.
      if (ProcNo == PROC_GAME &&
          (pDispInfo->pAct->atr & ACT_ATR_INFO) &&
          mouse.nowPoint.x >= pDispInfo->pAct->x - MOUSE_HIT_SIZE_X / 2 &&
          mouse.nowPoint.x <= pDispInfo->pAct->x + MOUSE_HIT_SIZE_X / 2 &&
          mouse.nowPoint.y >= pDispInfo->pAct->y - MOUSE_HIT_SIZE_Y / 2 &&
          mouse.nowPoint.y <= pDispInfo->pAct->y + MOUSE_HIT_SIZE_Y / 2)
        hitFlag = TRUE;
      // ?????刪叉????????
#ifndef __CARYTEST
      if (mouse.nowPoint.x <= pDispInfo->x +
                                  SpriteInfo[pDispInfo->bmpNo].width * 0.5 +
                                  MOUSE_HIT_SIZE_X * 0.5 &&
          pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width * 0.5 -
                  MOUSE_HIT_SIZE_X * 0.5 <=
              mouse.nowPoint.x &&
          mouse.nowPoint.y <=
              pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height &&
          pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height -
                  MOUSE_HIT_SIZE_Y <=
              mouse.nowPoint.y)
#else
      if (mouse.nowPoint.x <= pDispInfo->x +
                                  g_lpRealAdrn[pDispInfo->bmpNo].width * 0.5 +
                                  MOUSE_HIT_SIZE_X * 0.5 &&
          pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width * 0.5 -
                  MOUSE_HIT_SIZE_X * 0.5 <=
              mouse.nowPoint.x &&
          mouse.nowPoint.y <=
              pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height &&
          pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height -
                  MOUSE_HIT_SIZE_Y <=
              mouse.nowPoint.y)
#endif
        hitFlag = TRUE;
    }

    // ???????
    if (hitFlag == TRUE) {

#ifdef __ATTACK_MAGIC

      int cnt;

      for (cnt = 0; cnt < BATTLKPKPLYAERNUM; cnt++) {
        if (p_party[cnt] == pDispInfo->pAct)
          break;
      }

      // 酘奻善衵狟晤瘍峈6 , 7 , 8 , 9
      // 絞躺夔恁寁等齬醴垀恁寁腔婓扂源腔弇离
      if (BattleMyNo >= 10 &&
          (6 == pDispInfo->hitFlag || 7 == pDispInfo->hitFlag)) {
        hitFlag = FALSE;
        continue;
      }
      // 絞躺夔恁寁等齬醴垀恁寁腔婓扂源腔弇离
      if (BattleMyNo < 10 &&
          (8 == pDispInfo->hitFlag || 9 == pDispInfo->hitFlag)) {
        hitFlag = FALSE;
        continue;
      }
#ifdef _SKILL_ADDBARRIER
      // 硐夔恁扂源
      if (BattleMyNo < 10 &&
          (10 == pDispInfo->hitFlag || 11 == pDispInfo->hitFlag)) {
        hitFlag = FALSE;
        continue;
      }
      if (BattleMyNo >= 10 &&
          (12 == pDispInfo->hitFlag || 13 == pDispInfo->hitFlag)) {
        hitFlag = FALSE;
        continue;
      }
#endif
#endif

      // 更?牙????????
      //            StockFontBuffer( pDispInfo->x, pDispInfo->y,
      //            FONT_PRIO_FRONT, 0, "Hit", 1 );

      // ???????吻????
      HitDispNo = pDispSort->no;

      // ?????牙???
      if (pDispInfo->hitFlag >= 2) {
        // ??????????
        if (pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND) {
          // ????牙????????????
#ifndef __CARYTEST
          StockBoxDispBuffer(
              pDispInfo->x - 2, pDispInfo->y - 2,
              pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
              pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
              // DISP_PRIO_BOX2, 250, 0 );
              DISP_PRIO_BOX3, BoxColor, 0);
#else
          StockBoxDispBuffer(
              pDispInfo->x - 2, pDispInfo->y - 2,
              pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
              pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
              DISP_PRIO_BOX3, BoxColor, 0);
#endif
        } else
          // ??????????
          if (pDispSort->dispPrio >= DISP_PRIO_MENU) {
            // ????牙????????????
#ifndef __CARYTEST
            StockBoxDispBuffer(
                pDispInfo->x - 2, pDispInfo->y - 2,
                pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
                pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
                // DISP_PRIO_BOX2, 250, 0 );
                DISP_PRIO_BOX2, BoxColor, 0);
#else
            StockBoxDispBuffer(
                pDispInfo->x - 2, pDispInfo->y - 2,
                pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
                pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
                DISP_PRIO_BOX2, BoxColor, 0);
#endif
          } else {
            // ????牙????????????
#ifndef __CARYTEST
            StockBoxDispBuffer(
                pDispInfo->x - 2, pDispInfo->y - 2,
                pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
                pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
                // DISP_PRIO_BOX, 250, 0 );
                DISP_PRIO_BOX, BoxColor, 0);
            // pDispSort->dispPrio, 250, 0 );
#else
            StockBoxDispBuffer(
                pDispInfo->x - 2, pDispInfo->y - 2,
                pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
                pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
                DISP_PRIO_BOX, BoxColor, 0);
#endif
          }
        if (pDispInfo->hitFlag >= 3)
          CheckGroupSelect(pDispInfo->hitFlag);
        SortDispBuffer(); // 牙???????
      }

      // 2026.08.30: 主角的 OneLineStr 总是
      // 自己：【家族】，名称，称号，等级，生命值，气力，【坐骑名称】，【坐骑等级】
      // 其他：【家族】，名称，称号，【坐骑名称】，【坐骑等级】
      if (TaskBarFlag == FALSE) {
        if (pDispInfo->pAct != NULL) {
          if (pDispInfo->pAct->atr & ACT_ATR_INFO) {
            if (ProcNo == PROC_GAME) {
              // 2026.08.30: 设置在平时场景下的OneLineInfoStr.
#ifdef _MOUSE_SHOW_INFO_FOR_HEAD
              int left = getTextLength(pDispInfo->pAct->name) / 2;
              itemNameColor = pDispInfo->pAct->itemNameColor;
              StockFontBuffer(pDispInfo->x + 20 - left, pDispInfo->y - 10,
                              FONT_PRIO_FRONT, itemNameColor,
                              pDispInfo->pAct->name, 0);
#endif

              if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PC) {
                itemNameColor = pDispInfo->pAct->itemNameColor;
                if (pDispInfo->pAct->freeName[0] != NULL) {
                  if (pDispInfo->pAct->petName[0] != NULL)
                    if (pc.familyName[0] != NULL)
                      sprintf_s(OneLineInfoStr,
                          "{%s} %s [%s] Lv:%d HP:%d/%d MP:%d,  %s Lv:%d",
                          pc.familyName, pDispInfo->pAct->name,
                          pDispInfo->pAct->freeName, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp,
                          pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
                    else // 没有家族称谓
                      sprintf_s(OneLineInfoStr,
                        "%s [%s] Lv:%d HP:%d/%d MP:%d,  %s Lv:%d",
                        pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                        pDispInfo->pAct->level, pDispInfo->pAct->hp,
                        pDispInfo->pAct->maxHp, pc.mp,
                        pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
                  else if (pc.familyName[0] != NULL)
                    sprintf_s(OneLineInfoStr,
                      "{%s} %s [%s] Lv:%d HP:%d/%d MP:%d",
                      pc.familyName, pDispInfo->pAct->name,
                      pDispInfo->pAct->freeName, pDispInfo->pAct->level,
                      pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp);
                  else
                    sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d HP:%d/%d MP:%d",
                      pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                      pDispInfo->pAct->level, pDispInfo->pAct->hp,
                      pDispInfo->pAct->maxHp, pc.mp);
                } else {
                  if (pDispInfo->pAct->petName[0] != NULL)
                    if (pc.familyName[0] != NULL)
                      sprintf_s(OneLineInfoStr,
                          "{%s} %s Lv:%d HP:%d/%d MP:%d, %s Lv:%d",
                          pc.familyName, pDispInfo->pAct->name,
                          pDispInfo->pAct->level, pDispInfo->pAct->hp,
                          pDispInfo->pAct->maxHp, pc.mp,
                          pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel);
                    else
                      sprintf_s(OneLineInfoStr,
                          "%s Lv:%d HP:%d/%d MP:%d,  %s Lv:%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp,
                          pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
                  else if (pc.familyName[0] != NULL)
                    sprintf_s(OneLineInfoStr, "{%s} %s Lv:%d HP%d/%d MP:%d",
                        pc.familyName, pDispInfo->pAct->name,
                        pDispInfo->pAct->level, pDispInfo->pAct->hp,
                        pDispInfo->pAct->maxHp, pc.mp);
                  else
                    sprintf_s(OneLineInfoStr, "%s Lv:%d HP:%d/%d MP:%d",
                        pDispInfo->pAct->name, pDispInfo->pAct->level,
                        pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
                        pc.mp);
                }
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER_PC) {
                // 2026.08.30: OTHER_PC 就是 其他玩家，看不到具体的生命
#ifdef _MOUSE_DBL_CLICK
                if (mouseDblRightOn) {
                  openServerWindow(WINDOW_MESSAGETYPE_MOUSEGETNAME, 0, 0, 0,
                                   pDispInfo->pAct->name);
                  return;
                }
#endif
                itemNameColor = pDispInfo->pAct->itemNameColor;
                if (pDispInfo->pAct->freeName[0] != NULL) {
                  if (pDispInfo->pAct->petName[0] != NULL)
                    if (pDispInfo->pAct->fmname[0] != NULL)
                      sprintf_s(OneLineInfoStr, "{%s} %s [%s] Lv:%d,  %s Lv:%d",
                          pDispInfo->pAct->fmname, pDispInfo->pAct->name,
                          pDispInfo->pAct->freeName, pDispInfo->pAct->level,
                          pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
                    else
                      sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d,  %s Lv:%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                          pDispInfo->pAct->level, pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel);
                  else if (pDispInfo->pAct->fmname[0] != NULL)
                    sprintf_s(OneLineInfoStr, "{%s} %s [%s]  Lv:%d",
                        pDispInfo->pAct->fmname, pDispInfo->pAct->name,
                        pDispInfo->pAct->freeName,
                        pDispInfo->pAct->level);
                  else
                    sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d",
                        pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                        pDispInfo->pAct->level);
                } else {
                  if (pDispInfo->pAct->petName[0] != NULL)
                    if (pDispInfo->pAct->fmname[0] != NULL)
                      sprintf_s(OneLineInfoStr, "{%s} %s Lv:%d,  %s Lv:%d",
                          pDispInfo->pAct->fmname, pDispInfo->pAct->name,
                          pDispInfo->pAct->level,
                          pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel);
                    else
                      sprintf_s(OneLineInfoStr, "%s Lv:%d,  %s Lv:%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel);
                  else if (pDispInfo->pAct->fmname[0] != NULL)
                    sprintf_s(OneLineInfoStr, "{%s} %s Lv:%d",
                        pDispInfo->pAct->fmname, pDispInfo->pAct->name,
                        pDispInfo->pAct->level);
                  else
                    sprintf_s(OneLineInfoStr, "%s Lv:%d", pDispInfo->pAct->name,
                        pDispInfo->pAct->level);
                }
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PET) {
                if (pDispInfo->pAct->freeName[0] != NULL) {
                  sprintf_s(OneLineInfoStr, "%s Lv:%d",
                      pDispInfo->pAct->freeName, pDispInfo->pAct->level);
                } else {
                  sprintf_s(OneLineInfoStr, "%s Lv:%d", pDispInfo->pAct->name,
                      pDispInfo->pAct->level);
                }
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM) {
                itemNameColor = pDispInfo->pAct->itemNameColor;
                sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
#ifdef _ITEM_PATH
                ITEMPATHFLAG = TRUE;
#endif
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_GOLD) {
                sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER) {
                sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
              }
            } else if (ProcNo == PROC_BATTLE) {
              // 2026.08.30: 战斗场景下
              if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PET) {
                if (pDispInfo->pAct->freeName[0] != NULL) {
                  sprintf_s(OneLineInfoStr, "%s Lv:%d",
                            pDispInfo->pAct->freeName, pDispInfo->pAct->level);
                } else {
                  sprintf_s(OneLineInfoStr, "%s Lv:%d", pDispInfo->pAct->name,
                            pDispInfo->pAct->level);
                }
              } else if (pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM) {
                sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
              } else if (BattleMyNo < BATTLKPKPLYAERNUM) {
                if (((ATR_EQU *)pDispInfo->pAct->pYobi)->place_no ==
                    BattleMyNo) {
                  if (pDispInfo->pAct->freeName[0] != NULL) {
                    // Robin 0728 ride Pet
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr,
                          "%s [%s] Lv:%d HP:%d/%d MP:%d,  %s Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                          pDispInfo->pAct->level, pDispInfo->pAct->hp,
                          pDispInfo->pAct->maxHp, pDispInfo->pAct->mp,
                          pDispInfo->pAct->petName, pDispInfo->pAct->petLevel,
                          pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
                    else
                      sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d HP:%d/%d MP:%d",
                                pDispInfo->pAct->name,
                                pDispInfo->pAct->freeName,
                                pDispInfo->pAct->level, pDispInfo->pAct->hp,
                                pDispInfo->pAct->maxHp, pDispInfo->pAct->mp);
                  } else {
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr,
                          "%s Lv/%d HP:%d/%d MP:%d,  %s Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
                          pDispInfo->pAct->mp, pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel,
                          pDispInfo->pAct->petHp,
                          pDispInfo->pAct->petMaxHp);
                    else
                      sprintf_s(OneLineInfoStr, "%s Lv:%d HP:%d/%d MP:%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
                          pDispInfo->pAct->mp);
                  }
                } else if (((ATR_EQU *)p_party[BattleMyNo]->pYobi)->group_flg ==
                           ((ATR_EQU *)pDispInfo->pAct->pYobi)->group_flg) {
                  if (pDispInfo->pAct->freeName[0] != NULL) {
                    // Robin 0728
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr,
                          "%s [%s] Lv:%d HP:%d/%d,  %s Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                          pDispInfo->pAct->level, pDispInfo->pAct->hp,
                          pDispInfo->pAct->maxHp, pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp,
                          pDispInfo->pAct->petMaxHp);
                    else
                      sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name,
                          pDispInfo->pAct->freeName,
                          pDispInfo->pAct->level, pDispInfo->pAct->hp,
                          pDispInfo->pAct->maxHp);
                  } else {
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr,
                          "%s Lv:%d HP:%d/%d,  %s Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
                          pDispInfo->pAct->petName, pDispInfo->pAct->petLevel,
                          pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
                    else
                      sprintf_s(OneLineInfoStr, "%s Lv:%d HP:%d/%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->level,
                          pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
                  }
                } else {
                  if (pDispInfo->pAct->freeName[0] != NULL) {
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d,  %s Lv:%d",
                          pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                          pDispInfo->pAct->level, pDispInfo->pAct->petName,
                          pDispInfo->pAct->petLevel);
                    else
                      sprintf_s(OneLineInfoStr, "%s [%s] Lv:%d",
                                pDispInfo->pAct->name,
                                pDispInfo->pAct->freeName,
                                pDispInfo->pAct->level);
                  } else {
                    if (pDispInfo->pAct->onRide == 1)
                      sprintf_s(OneLineInfoStr, "%s Lv:%d,  %s Lv:%d",
                                pDispInfo->pAct->name, pDispInfo->pAct->level,
                                pDispInfo->pAct->petName,
                                pDispInfo->pAct->petLevel);
                    else
                      sprintf_s(OneLineInfoStr, "%s  Lv:%d",
                                pDispInfo->pAct->name, pDispInfo->pAct->level);
                  }
                }
              } else { //
                if (pDispInfo->pAct->freeName[0] != NULL) {
                  sprintf_s(OneLineInfoStr, "%s [%s]  Lv:%d",
                            pDispInfo->pAct->name, pDispInfo->pAct->freeName,
                            pDispInfo->pAct->level);
                } else {
                  sprintf_s(OneLineInfoStr, "%s Lv:%d", pDispInfo->pAct->name,
                            pDispInfo->pAct->level);
                }
              }
            }
          }
        }
        if (!transmigrationEffectFlag)
          ShowBottomLineString(itemNameColor, OneLineInfoStr);
      }
      OneLineInfoStr[0] = '\0';
      mouse.level = pDispSort->dispPrio;
      return;
    }
  }
  mouse.level = DISP_PRIO_TILE;
  HitDispNo = -1;

  if (TaskBarFlag == FALSE) {
    ShowBottomLineString(itemNameColor, OneLineInfoStr);
  }
  OneLineInfoStr[0] = '\0';
}
