#define __PROCESS_CPP__
#include "systeminc/system.h"
#include "game/battle_menu.h"
#include "game/battle_proc.h"
#include "sdk/caryime.h"
#include "systeminc/character.h"
#include "systeminc/field.h"
#include "systeminc/handletime.h"
#include "systeminc/ime_sa.h"
#include "systeminc/login.h"
#include "systeminc/lssproto_cli.h"
#include "systeminc/map.h"
#include "systeminc/menu.h"
#include "systeminc/netmain.h"
#include "systeminc/netproc.h"
#include "systeminc/pc.h"
#include "systeminc/produce.h"
#include "systeminc/savedata.h"
#include "systeminc/t_music.h"

#ifdef _NEW_MUISC_
bool new_music = true;
#endif

#ifdef _PET_TRACE_MOUSE
extern SCPlayPet PlayPet;
#endif

void Process(void) {
  if (ProcNo2 >= 0) {
    ProcNo = ProcNo2;
    ProcNo2 = -1;
    SubProcNo = SubProcNo2;
  }
  switch (ProcNo) {
  case PROC_OPENNING:
    break;
  case PROC_INIT: /* ??????? */
    InitProc();
    initMapEffect(TRUE); // ???????????
    break;
  // ????????向???
  case PROC_ID_PASSWORD:
#ifdef _NEW_MUISC_
    if (new_music) {
      new_music = false;
      play_bgm(27);
    }
#endif
    BackBufferDrawType = DRAW_BACK_NORMAL;
    idPasswordProc();
#ifdef _PET_TRACE_MOUSE
    PlayPet.Proc();
#endif
    RunAction();           //
    StockTaskDispBuffer(); //
    FlashKeyboardCursor(); //
    ImeProc();             //
#ifdef _LOGINKICK
    extern DWORD StartTime;
    StartTime = -1;
#endif
    break;
  case PROC_TITLE_MENU:
    BackBufferDrawType = DRAW_BACK_NORMAL;
    titleProc();
    TitleProduce();        // ?????
    RunAction();           // ?????????
    StockTaskDispBuffer(); // ???牙????????????
    break;
  case PROC_CHAR_SELECT: // ???︻?
    BackBufferDrawType = DRAW_BACK_NORMAL;
    selectCharacterProc();
    break;
  case PROC_CHAR_MAKE:
    BackBufferDrawType = DRAW_BACK_NORMAL;
    makeCharacterProc();
    break;
  case PROC_CHAR_LOGIN_START:
    BackBufferDrawType = DRAW_BACK_NORMAL;
    initMap();              // 初始化地图
    initPcAll();            // 初始化游戏状态
    initCharObj();          //
    initOpenServerWindow(); //
    initFieldProc();        //
    initMapEffect(FALSE);   // 初始化地图效果
    EncountFlag = FALSE;
    logOutFlag = FALSE;
    InitOhtaParam(); // ??????????兣?﹐??
#ifdef __SKYISLAND
    extern void SkyIslandInit();
    SkyIslandInit();
#endif
    ChangeProc(PROC_CHAR_LOGIN);
    fade_out_bgm();
    break;
  case PROC_CHAR_LOGIN: /* 角色登出 */
    BackBufferDrawType = DRAW_BACK_NORMAL;
    characterLoginProc();
    break;
  // ?????
  case PROC_CHAR_LOGOUT: /* 角色登入 */
    characterLogoutProc();
    break;
  case PROC_GAME: /* 游戏正常运行 */
    GameProc();
    break;
  case PROC_DISCONNECT_SERVER: /* 掉线了 */
    switch (SubProcNo) {
    case 0:
      resetPc();
      initCharObj();
      DeathAllAction();
      ProduceInitFlag = TRUE;
      CopyBackBuffer();
      BackBufferDrawType = DRAW_BACK_PRODUCE;
      fade_out_bgm();
      NowTime = TimeGetTime();
      SubProcNo++;
      break;
    case 1:
      if (DrawProduce(PRODUCE_DOWN_ACCELE) == TRUE) {
        BackBufferDrawType = DRAW_BACK_NORMAL;
        SubProcNo++;
      }
      break;
    case 2:
      break;
    }
    if (disconnectServer()) {
      cleanupNetwork();
      PaletteChange(DEF_PAL, 0);
      ChangeProc(PROC_ID_PASSWORD);
      SubProcNo = 0;
      DeathAllAction();
      disconnectServerFlag = FALSE;
      oldDisconnectServerFlag = FALSE;
      break;
    }
    RunAction();           // ?????????
    StockTaskDispBuffer(); // ???牙????????????
    break;
  case PROC_BATTLE:
    BattleProc();
    break;
#ifdef _80_LOGIN_PLAY
  case PROC_80_LOGIN:
    BackBufferDrawType = DRAW_BACK_NORMAL;
    _80LoginProc();
    RunAction();
    StockTaskDispBuffer();
    break;
#endif
  case PROC_ENDING:
    break;
  }

  // Keep the client-space mouse coordinates visible in every interactive scene.
  // A fixed position is easier to read and does not interfere with hit testing.
  if (lpDraw != NULL && ProcNo != PROC_INIT) {
    char mousePosition[64];
    sprintf_s(mousePosition, "Mouse: (%d, %d)", mouse.nowPoint.x,
              mouse.nowPoint.y);
    StockFontBuffer(8, 8, FONT_PRIO_FRONT, FONT_PAL_YELLOW, mousePosition, 0);
    char ProcNoStr[64];
    sprintf_s(ProcNoStr, "ProcNo: %d", ProcNo);
    StockFontBuffer(8, 24, FONT_PRIO_FRONT, FONT_PAL_YELLOW, ProcNoStr, 0);
  }
}

void ChangeProc(int procNo) {
#ifdef _NEW_MUISC_
  if (PROC_ID_PASSWORD == procNo) {
    new_music = true;
    // t_music_se_volume = t_music_bgm_volume = 15;
  }
#endif
  ProcNo = procNo;
  SubProcNo = 0;
}
void ChangeProc(int procNo, int subProcNo) {
  ProcNo = procNo;
  SubProcNo = subProcNo;
}

void ChangeProc2(int procNo) {
  ProcNo2 = procNo;
  SubProcNo2 = 0;
}

void ChangeProc2(int procNo, int subProcNo) {
  ProcNo2 = procNo;
  SubProcNo2 = subProcNo;
}

void GameProc(void) {
  static int sNowBgm;
  static BOOL produceFlag;
  switch (SubProcNo) {
  case 0:
    extern short helpBtn, actBtn;
    helpBtn = 0;
    actBtn = 0;
    ClearIme();
    InitChat();
    produceFlag = FALSE;
    EncountFlag = FALSE;
    LoadAlbum();
    extern void AI_Init();
    AI_Init();
    nameOverTheHeadFlag = 0;
    SubProcNo++;
  case 150:
    SubProcNo = 100;
  case 100:
    initPc();
    resetFieldProc();
    restorePtActCharObjAll();
    initOpenServerWindow();
    InitMenu();
    //            BattleStatusBak[ 0 ] = NULL;// ???┤???
    //            BattleStatusReadPointer = BattleStatusWritePointer =0;
    GetKeyInputFocus(&MyChatBuffer);
    SubProcNo++;
  case 101:
    if (loginFlag) {
      BackBufferDrawType = DRAW_BACK_NORMAL;
      break;
    }
    SubProcNo++;
  case 102:
    paletteProc();
    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    initCharPartsPrio();      // ?????????！???叉??????
    RunAction();              // ?????????
    StockTaskDispBuffer();    // ???牙????????????
    stop_bgm();               //???于
    drawMap();                //    ???牙
    MenuProc();               // ??????
    ChatProc();               // ??????
    ChatBufferToFontBuffer(); // ??????????????????????
    ImeProc();                // ???????
    SortDispBuffer();         // 牙???????
    ClearBackSurface();
    PutBmp();
    lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    play_bgm(map_bgm_no);
    NowTime = TimeGetTime();
    BackBufferDrawType = DRAW_BACK_PRODUCE;
    ProduceInitFlag = TRUE;
    DrawProduce(PRODUCE_BRAN_BIG);
    SubProcNo++;
    break;
  case 103:
    BackBufferDrawType = DRAW_BACK_PRODUCE;
    if (DrawProduce(PRODUCE_BRAN_BIG) == TRUE) {
      produceFlag = TRUE;
      fieldInfoTime = TimeGetTime();
      if (fastDrawTile) {
        SubProcNo = 20;
      } else {
        SubProcNo = 3;
      }
    }
    break;
  case 200:
    ProduceInitFlag = TRUE;
    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    NowTime = TimeGetTime();
    SubProcNo++;
  case 201:
    BackBufferDrawType = DRAW_BACK_PRODUCE;
    if (DrawProduce(PRODUCE_CENTER_PRESSIN) == TRUE) {
      SubProcNo++;
    }
    drawField(); // ????????????牙
    MenuProc();  // ??????
    ImeProc();   // ???????
    break;
  case 202:
    if (!warpEffectStart || !warpEffectOk) {
      BackBufferDrawType = DRAW_BACK_NORMAL;
      drawField(); // ????????????牙
      MenuProc();  // ??????
      ImeProc();   // ???????
      break;
    }
    warpEffectStart = FALSE;
    warpEffectOk = FALSE;
    SubProcNo++;
  case 203:
    // ?????
    // ???????
    paletteProc();

    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    initCharPartsPrio();   // ?????????！???叉??????
    RunAction();           // ?????????
    StockTaskDispBuffer(); // ???牙????????????
    redrawMap();
    drawMap(); //    ???牙

    // ?????????
    if ((mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0) ||
        (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0))
      initMapEffect(FALSE);
    ChatProc();               // ??????
    ChatBufferToFontBuffer(); // ??????????????????????
    SortDispBuffer();         // 牙???????
    ClearBackSurface();
    if ((mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0) ||
        (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0))
      mapEffectProc2(80); // ???????????．??
    PutBmp();
    lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    NowTime = TimeGetTime();
    ProduceInitFlag = TRUE;
    SubProcNo++;
  case 204:
    BackBufferDrawType = DRAW_BACK_PRODUCE;
    if (DrawProduce(PRODUCE_CENTER_PRESSOUT) == TRUE) {
      fieldInfoTime = TimeGetTime();
      if (fastDrawTile) {
        SubProcNo = 20;
      } else {
        SubProcNo = 3;
      }
#ifdef _NPC_DANCE
      if (nowFloor == 7027)
        play_bgm(24);
#endif
    }
    drawField(); // ????????????牙
    MenuProc();  // ??????
    ImeProc();   // ???????
    break;

  case 1:
    initPc();                 // ?????????
    resetFieldProc();         // ????????????
    restorePtActCharObjAll(); // ?????????????????
    initOpenServerWindow();   // ?????牙???????????
    InitMenu();               // ??????????
    GetKeyInputFocus(&MyChatBuffer);

    if (produceFlag == TRUE) {
      DispBuffer.DispCnt = 0;
      FontCnt = 0;
      initCharPartsPrio();   // ?????????！???叉??????
      RunAction();           // ?????????
      StockTaskDispBuffer(); // ???牙????????????
      stop_bgm();            //???于
      updateMapArea();
      redrawMap();
      drawMap(); //    ???牙
      //????????
      if (!draw_map_bgm_flg) {
        //????????
        play_bgm(map_bgm_no = sNowBgm);
        draw_map_bgm_flg = 1;
      }
      MenuProc(); // ??????
      // ???????主???????
      // StockBoxDispBuffer( 0, 456, lpDraw->xSize, lpDraw->ySize,
      // DISP_PRIO_MENU, 0, 1 );
      ChatProc();               // ??????
      ChatBufferToFontBuffer(); // ??????????????????????
      ImeProc();                // ???????
      SortDispBuffer();         // 牙???????
      // ???????????????
      ClearBackSurface();
      // ????????????????
      PutBmp();
      // ??????????????????????
      lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL,
                               DDBLTFAST_WAIT);
      // ???????
      DispBuffer.DispCnt = 0;
      FontCnt = 0;
      // ????????
      NowTime = TimeGetTime();
      // ????????丙???阪?
      BackBufferDrawType = DRAW_BACK_PRODUCE;
      // ??
      DrawProduce(PRODUCE_4WAY_IN);
    } else {
      // ??????
      produceFlag = TRUE;
      SubProcNo++;
    }
    SubProcNo++;

    break;

  case 2: // ????????
    // ??
    if (DrawProduce(PRODUCE_4WAY_IN) == TRUE) {
      warpEffectStart = FALSE;
      warpEffectOk = FALSE;
      if (fastDrawTile) {
        SubProcNo = 20;
      } else {
        SubProcNo = 3;
      }
    }
    break;

  case 20:
    BackBufferDrawType = DRAW_BACK_NORMAL;
    repairMap();
    SubProcNo = 3;

  case 3:
    // ????????丙???阪?

    BackBufferDrawType = DRAW_BACK_NORMAL;
    paletteProc();

    initItemOverlapCheck(); // ????????????????

    if (!transmigrationEffectFlag) {
      drawGrid();  // ????????牙
      fieldProc(); // ?????????
      moveProc(); // ?去??
    }

    initCharPartsPrio();    //
    openServerWindowProc(); //
    drawFieldInfoWin();     //
    RunAction();
    StockTaskDispBuffer();
    mapEffectProc(); // ???????????．??
    drawMap2(); // ???牙

    if (!transmigrationEffectFlag) {
      drawField();
      ChatProc();
      ChatBufferToFontBuffer();
      FlashKeyboardCursor();
      MenuProc();
      ImeProc();
    }
    TimeZoneProc();

    // ?????????
    if (EncountFlag == TRUE) {
      resetPc();                                   // PC????
      resetCharObj();                              // ?????????????
      resetMap();                                  // ?????????
      clearPtActPartyParam();                      // ????????????????NULL???
      fieldInfoTime = 0;                           // ???????
      drawFieldInfoWin();                          // ???????
      resetFieldProc();                            // ????????????
      nowEncountPercentage = minEncountPercentage; // ????????????
      sendEnFlag = 0;
      encountNowFlag = 1;
      eventEnemySendFlag = 0;
      duelSendFlag = 0;
      jbSendFlag = 0;
      // ?????????????????
      if (MenuToggleFlag & JOY_CTRL_M)
        MapWmdFlagBak = TRUE;
      ResultWndTimer = RESULT_WND_CLOSE_TIME;
      InitMenu();          // ??????????
      BattleCmd[0] = NULL; // ??????????
      BattleStatus[0] = NULL; // ???┤???
      BattleTurnReceiveFlag = TRUE; // ????????
      play_se(215, 320, 240);
      sNowBgm = t_music_bgm_no;
      stop_bgm();
      ChangeProc2(PROC_GAME, SubProcNo + 1);
    }
    break;
  case 4:
    CopyBackBuffer();
    BackBufferDrawType = DRAW_BACK_PRODUCE;
    SubProcNo++;
    break;
  case 5:
    if (DrawProduce(PRODUCE_HAGARE_OCHI_OUT) == TRUE) {
#ifdef _HALLOWEEN_EFFECT
      initMapEffect(FALSE);
#endif
      ChangeProc(PROC_BATTLE);
    }
    MenuProc(); // ??????
    ImeProc();  // ???????
    break;
  }
}

void warpEffectProc(void) {
  oldMapEffectRainLevel = mapEffectRainLevel;
  oldMapEffectSnowLevel = mapEffectSnowLevel;
  DispBuffer.DispCnt = 0;
  FontCnt = 0;
  if (MenuToggleFlag & JOY_CTRL_M)
    MapWmdFlagBak = TRUE;
  InitMenu2();
  fieldProc();            //
  initCharPartsPrio();    //
  openServerWindowProc(); //
  fieldInfoTime = 0;  // ???????
  drawFieldInfoWin(); // ???????

  RunAction();
  StockTaskDispBuffer();
  mapEffectProc(); // ???????????．??
  redrawMap();
  drawMap(); // ???牙
  ChatProc();
  ChatBufferToFontBuffer();
  SortDispBuffer(); // 牙???????
  ClearBackSurface();
  PutBmp();
#ifndef __SKYISLAND
  // ??????????????????????
  lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
#endif
}

void repairMap(void) {
  float dx, dy;
#ifdef _MOVE_SCREEN
  if (pc.bMoveScreenMode) {
    dx = (float)(+(-nowGx) * (SURFACE_WIDTH >> 1) +
                 -nowGy * (SURFACE_WIDTH >> 1) + viewOffsetX);
    dy = (float)(-(-nowGx) * (SURFACE_HEIGHT >> 1) +
                 -nowGy * (SURFACE_HEIGHT >> 1) + viewOffsetY);
  } else
#endif
    //
    camMapToGamen(0.0, 0.0, &dx, &dy);

  nowXFastDraw = (int)(dx + .5);
  nowYFastDraw = (int)(dy + .5);

  nowXFastDraw2 = nowXFastDraw;
  nowYFastDraw2 = nowYFastDraw;

  if (gResolutionMode == 1) {
    nowXFastDraw = (int)(dx / 2 + .5);
    nowYFastDraw = (int)(dy / 2 + .5);
  }

  baseXFastDraw = nowXFastDraw;
  baseYFastDraw = nowYFastDraw;
  amountXFastDraw = nowXFastDraw - baseXFastDraw;
  amountYFastDraw = nowYFastDraw - baseYFastDraw;

  DispBuffer.DispCnt = 0;
  FontCnt = 0;
  drawTile();       //
  SortDispBuffer(); //
  // cary 2001 11 8
  ClearBackSurface();
  // ????????????????
  fastDrawTileFlag = 0;
  PutBmp();
  fastDrawTileFlag = 1;
#ifndef __SKYISLAND
  // ??????????????????????
  lpBattleSurface->BltFast(0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT);
#endif
  DispBuffer.DispCnt = 0;
  FontCnt = 0;
  // ????????
  NowTime = TimeGetTime();
}

void paletteProc(void) {
  if (palNo == -1) {
    // ????????
    // the second PaletteChange( SaTimeZoneNo, 0 );
    PaletteChange(SaTimeZoneNo, palTime);
    // ??┼????????????
    TimeZonePalChangeFlag = TRUE;
    palNo = -2;
  } else if (palNo >= 0) {
    // ?叉????　叉
    // the second PaletteChange( palNo, 0 );// ????????
    PaletteChange(palNo, palTime); // ????????
    // ??┼?????????????
    TimeZonePalChangeFlag = FALSE;
    palNo = -2;
  }
}

#ifdef _SURFACE_ANIM
extern ACTION *SPACT[MAX_ANIM];
void AniProc() {
  float mx, my;

  for (int i = 0; i < MAX_ANIM; i++) {
    if (SPACT[i] != NULL) {
      camMapToGamen(SPACT[i]->mx, SPACT[i]->my, &mx, &my);
      if (SPACT[i]->anim_chr_no > 102002) {
        SPACT[i]->x = (int)(mx - 5);
        SPACT[i]->y = (int)(my + 8);
      } else {
        SPACT[i]->x = (int)(mx + .5);
        SPACT[i]->y = (int)(my + .5);
      }
      pattern(SPACT[i], ANM_NOMAL_SPD, ANM_LOOP);
    }
  }
}
#endif
