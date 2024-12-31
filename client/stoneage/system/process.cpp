﻿#include "version.h"
#include "systeminc/system.h"
#include "systeminc/map.h"
#include "systeminc/ime_sa.h"
#include "sdk/caryime.h"
#include "systeminc/menu.h"
#include "systeminc/pc.h"
#include "systeminc/character.h"
#include "systeminc/login.h"
#include "systeminc/netproc.h"
#include "systeminc/savedata.h"
#include "systeminc/testView.h"
#include "game/battle_proc.h"
#include "systeminc/produce.h"
#include "systeminc/lssproto_cli.h"
#include "systeminc/netmain.h"
#include "game/battle_menu.h"
#include "systeminc/t_music.h"
#include "systeminc/field.h"
#include "systeminc/handletime.h"

#ifdef _NEW_MUISC_
bool new_music = true;
#endif

/* ????吻? */
UINT ProcNo;
/* ??????吻? */
UINT SubProcNo;
/* ????吻?(?????????▔??) */
int ProcNo2;
int SubProcNo2;

int palNo;
int oldPalNo;
int palTime;

#ifdef _PET_TRACE_MOUSE
extern SCPlayPet PlayPet;
#endif

/* ?????? ********************************************************************/
void Process( void )
{
    if (ProcNo2 >= 0)
    {
        ProcNo = ProcNo2;
        ProcNo2 = -1;
        SubProcNo = SubProcNo2;
    }

    /* ????希? */
    switch (ProcNo)
    {
        case PROC_OPENNING:
            break;
        case PROC_INIT:     /* ??????? */
            InitProc();
            initMapEffect(TRUE);        // ???????????
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
            RunAction();            // ?????????
            StockTaskDispBuffer();    // ???牙????????????
            FlashKeyboardCursor();    // ?????????????
            ImeProc();                // ???????
#ifdef _LOGINKICK            
            extern DWORD StartTime;
            StartTime = -1 ; 
#endif

            break;
        // ????牙?????︻?
        case PROC_TITLE_MENU:
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            titleProc();
            TitleProduce();            // ?????
            RunAction();            // ?????????
            StockTaskDispBuffer();    // ???牙????????????

            break;
        // ???︻??
        case PROC_CHAR_SELECT:        // ???︻?
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            selectCharacterProc();
            break;
        // ????
        case PROC_CHAR_MAKE:
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            makeCharacterProc();
            break;
        // ︻???????????
        case PROC_CHAR_LOGIN_START:
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            // charLoginStart();???????????????????
            // ?????????????
            initMap();        // ??????
            initPcAll();    // PC??????????
            initCharObj();    // ????????????
            initOpenServerWindow();    // ?????牙???????????
            initFieldProc();        // ??????牙岌????
            initMapEffect(FALSE);        // ???????????
            EncountFlag = FALSE;
            logOutFlag = FALSE;
            InitOhtaParam();    // ??????????兣?﹐??
#ifdef __SKYISLAND
            extern void SkyIslandInit();
            SkyIslandInit();
#endif
            ChangeProc( PROC_CHAR_LOGIN );
            // ???????????
            fade_out_bgm();
            break;
        case PROC_CHAR_LOGIN:
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            characterLoginProc();
            break;
        // ?????
        case PROC_CHAR_LOGOUT:
            // ????????丙???阪?
//            BackBufferDrawType = DRAW_BACK_NORMAL; 
            characterLogoutProc();
            break;
        case PROC_GAME:     /* ??????? */
            GameProc();
            break;
        case PROC_DISCONNECT_SERVER:
            switch (SubProcNo)
            {
            case 0:
                // ???
                // PC????
                resetPc();
                // ?????????????
                initCharObj();
                // ??????
                DeathAllAction();
                // ?????
                ProduceInitFlag = TRUE;
                // ???????????? 
                CopyBackBuffer();
                // ????????丙???阪?
                BackBufferDrawType = DRAW_BACK_PRODUCE; 
                // ???????????
                fade_out_bgm();
                // ????????
                NowTime = TimeGetTime();
                SubProcNo++;
                break;
            case 1:
                // ??
                if (DrawProduce(PRODUCE_DOWN_ACCELE) == TRUE)
                {
                    BackBufferDrawType = DRAW_BACK_NORMAL; 
                    SubProcNo++;
                }
                break;
            case 2:
                // ????
                break;
            }
            // κ?????????
            if (disconnectServer())
            {
                // ?????????
                cleanupNetwork();
                // ????????????
                PaletteChange(DEF_PAL, 0);
                // ???????
                //cary
                ChangeProc( PROC_ID_PASSWORD );
                SubProcNo = 0;
                // ??????
                DeathAllAction();
                disconnectServerFlag = FALSE;
                oldDisconnectServerFlag = FALSE;
                break;
            }
            RunAction();            // ?????????
            StockTaskDispBuffer();    // ???牙????????????
            break;
        case PROC_BATTLE:     /* ??????? */
            BattleProc();
            break;
#ifdef _STONDEBUG_        
        //case PROC_OHTA_TEST:     /* 兣???????? */
        //    OhtaTestProc();
        //    break;
        //case PROC_TAKE_TEST:     /* ㄌ?????????? */
        //    TakeTestProc();
        //    break;
        //case PROC_DWAF_TEST:    // DWAF???
        //    dwafTestProc();
        //    break;
        case PROC_SPR_VIEW:        // ??????囝????
            SprViewProc();
            break;
        case PROC_ANIM_VIEW:    // ????????囝????
            AnimViewProc();
            break;
        case PROC_SE_TEST:    // ???囝????
            SeTestProc();
            break;
#endif
#ifdef _80_LOGIN_PLAY
        case PROC_80_LOGIN:
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            _80LoginProc();
            RunAction();
            StockTaskDispBuffer();
            break;
#endif
#ifdef _PK2007
        case PROC_PKSERVER_SELECT:
            BackBufferDrawType = DRAW_BACK_NORMAL; 
            selectpkProc();
            break;
#endif
        case PROC_ENDING:
            break;
    }
}

/* ????阪??? ***********************************************************/
void ChangeProc( int procNo )
{
#ifdef _NEW_MUISC_
    if(PROC_ID_PASSWORD==procNo){
        new_music = true;
        //t_music_se_volume = t_music_bgm_volume = 15;
    }
#endif
    ProcNo = procNo;
    // ??????吻?????
    SubProcNo = 0;


}
/* ????阪??? ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
    // ????吻????
    ProcNo = procNo;
    // ??????吻?????
    SubProcNo = subProcNo;
}
/* ????阪???????????????▔???**************************/
void ChangeProc2( int procNo )
{
    // ????吻????
    ProcNo2 = procNo;
    SubProcNo2 = 0;
}
/* ????阪??? ***********************************************************/
void ChangeProc2( int procNo, int subProcNo )
{
    // ????吻????
    ProcNo2 = procNo;
    // ??????吻?????
    SubProcNo2 = subProcNo;
}

void GameProc( void )
{
    static int now_bgm;
    static BOOL produceFlag;
    switch( SubProcNo ){
        case 0:
            //cary 滅砦婬棒腎奀腔弝敦紹隱
            extern short helpBtn, actBtn;
            helpBtn = 0;
            actBtn = 0;
            //end cary
#ifdef _DEBUG__
            if( offlineFlag ){
                initMap();
            }
#endif
            //InitIme();
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
            GetKeyInputFocus( &MyChatBuffer );
            SubProcNo++;
        case 101:
            if( loginFlag ){
                BackBufferDrawType = DRAW_BACK_NORMAL; 
                break;
            }
            SubProcNo++;
        case 102:
            paletteProc();

            DispBuffer.DispCnt = 0;
            FontCnt = 0;
            initCharPartsPrio();    // ?????????！???叉??????
            RunAction();            // ?????????
            StockTaskDispBuffer();    // ???牙????????????
            stop_bgm();                //???于
            drawMap();                //    ???牙
            MenuProc();                // ??????
            ChatProc();                // ??????
            ChatBufferToFontBuffer(); // ??????????????????????
            ImeProc();                // ???????
            SortDispBuffer();     // 牙???????
            // ???????????????
            ClearBackSurface();    
            // ????????????????
            PutBmp();    
            // ??????????????????????
            lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
            // ???????
            DispBuffer.DispCnt = 0;
            FontCnt = 0;

            play_bgm( map_bgm_no );

            // ????????
            NowTime = TimeGetTime();
            // ????????丙???阪?
//            BackBufferDrawType = DRAW_BACK_NORMAL; 
            BackBufferDrawType = DRAW_BACK_PRODUCE; 
            // ??
            ProduceInitFlag = TRUE;
            DrawProduce( PRODUCE_BRAN_BIG );
/*#ifdef __SKYISLAND
            ClearBackSurface();
#endif*/
            SubProcNo++;
            break;

        case 103:
            // ??
            BackBufferDrawType = DRAW_BACK_PRODUCE; 
            if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
            {
                produceFlag = TRUE;
                fieldInfoTime = TimeGetTime();
                if( fastDrawTile )
                {
                    SubProcNo = 20;
                }
                else
                {
                    SubProcNo = 3;
                }
            }
            break;

        // ????
        case 200:
            ProduceInitFlag = TRUE;

            // ???????
            DispBuffer.DispCnt = 0;
            FontCnt = 0;

            // ????????
            NowTime = TimeGetTime();

            // S??????C? warpEffectProc();?????
            // ?????????????????
            SubProcNo++;

        case 201:
            // ??
            BackBufferDrawType = DRAW_BACK_PRODUCE; 
            if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
            {
                SubProcNo++;
            }
            drawField();    // ????????????牙
            MenuProc();    // ??????
            ImeProc();    // ???????
            break;

        case 202:
            if( !warpEffectStart || !warpEffectOk )
            {
                BackBufferDrawType = DRAW_BACK_NORMAL; 
                drawField();    // ????????????牙
                MenuProc();    // ??????
                ImeProc();    // ???????
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
            initCharPartsPrio();    // ?????????！???叉??????
            RunAction();            // ?????????
            StockTaskDispBuffer();    // ???牙????????????
            redrawMap();
            drawMap();                //    ???牙

            // ?????????
            if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 ) 
                || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 )
                )
                initMapEffect(FALSE);
            ChatProc();                // ??????
            ChatBufferToFontBuffer(); // ??????????????????????
            SortDispBuffer();     // 牙???????
            // ???????????????
            ClearBackSurface();    
            if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
             || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
                mapEffectProc2( 80 );        // ???????????．??
            // ????????????????
            PutBmp();    
            // ??????????????????????
            lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
            // ???????
            DispBuffer.DispCnt = 0;
            FontCnt = 0;

//            if( map_bgm_no != now_bgm )
//            {
//                stop_bgm();                //???于
//                play_bgm( map_bgm_no );
//            }

            // ????????
            NowTime = TimeGetTime();

            ProduceInitFlag = TRUE;

            SubProcNo++;

        case 204:
            // ??
            BackBufferDrawType = DRAW_BACK_PRODUCE; 
            if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
            {
                fieldInfoTime = TimeGetTime();
                if( fastDrawTile )
                {
                    SubProcNo = 20;
                }
                else
                {
                    SubProcNo = 3;
                }
        #ifdef _NPC_DANCE
                if( nowFloor == 7027 )
                    play_bgm ( 24 ) ;
        #endif
            }
            drawField();    // ????????????牙
            MenuProc();    // ??????
            ImeProc();    // ???????
            break;


        case 1:
            initPc();                // ?????????
            resetFieldProc();        // ????????????
            restorePtActCharObjAll();    // ?????????????????
            initOpenServerWindow();    // ?????牙???????????
            InitMenu();        // ??????????
//            BattleStatusBak[ 0 ] = NULL;// ???┤???
//            BattleStatusReadPointer = BattleStatusWritePointer =0;
            // 向??????台
            GetKeyInputFocus( &MyChatBuffer );

            // ????
            if( produceFlag == TRUE ){
            // ·卯????????????
            //if( BattleResultWndFlag == TRUE ){

                // ?????
                // ???????
                DispBuffer.DispCnt = 0;
                FontCnt = 0;
                initCharPartsPrio();    // ?????????！???叉??????
                RunAction();            // ?????????
                StockTaskDispBuffer();    // ???牙????????????
                stop_bgm();                //???于
                updateMapArea();
                redrawMap();
                drawMap();                //    ???牙
                //????????
                if(!draw_map_bgm_flg){
                    //????????
                    play_bgm(map_bgm_no = now_bgm);
                    draw_map_bgm_flg = 1;
                }
                MenuProc();                // ??????
                // ???????主???????
                //StockBoxDispBuffer( 0, 456, lpDraw->xSize, lpDraw->ySize, DISP_PRIO_MENU, 0, 1 );
                ChatProc();                // ??????
                ChatBufferToFontBuffer(); // ??????????????????????
                ImeProc();                // ???????
                SortDispBuffer();     // 牙???????
                // ???????????????
                ClearBackSurface();    
                // ????????????????
                PutBmp();    
                // ??????????????????????
                lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
                // ???????
                DispBuffer.DispCnt = 0;
                FontCnt = 0;
                // ????????
                NowTime = TimeGetTime();
                // ????????丙???阪?
                BackBufferDrawType = DRAW_BACK_PRODUCE; 
                // ??
                DrawProduce( PRODUCE_4WAY_IN );
            }else{
                // ??????
                produceFlag = TRUE;
                SubProcNo++;
            }
            SubProcNo++;
            
            break;
        
        case 2:    // ????????
            // ??
            if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
                warpEffectStart = FALSE;
                warpEffectOk = FALSE;
                if( fastDrawTile )
                {
                    SubProcNo = 20;
                }
                else
                {
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

            initItemOverlapCheck();    // ????????????????

            if( !transmigrationEffectFlag ){
                drawGrid();                // ????????牙
                fieldProc();            // ?????????
                // Nuke 0407
                if( bNewServer){
                    moveProc();                // ?去??
                }else
                    moveProc();                // ?去??
            }

            initCharPartsPrio();    // ?????????！???叉??????
            //addressBookProc();        // ?????????
            openServerWindowProc();    // ?????????
            drawFieldInfoWin();        // ???????
            
            /* ????????? */
            RunAction();
            // ???牙????????????
            // 叉吳?????????
            StockTaskDispBuffer();

            mapEffectProc();        // ???????????．??

            drawMap2();        // ???牙

            if( !transmigrationEffectFlag ){
                drawField();    // ????????????牙

            /* ?????? */
                ChatProc();
            // ??????????????????????
                ChatBufferToFontBuffer(); 
            // ?????????????
                FlashKeyboardCursor();
            // ??????
                 MenuProc();
            // ???????
                ImeProc();        
            }
            // ??┼?????????????
            TimeZoneProc();
            
/*cary
#ifdef _STONDEBUG_
            // ?????????
            if( joy_trg[ 0 ] & JOY_P_DOWN ){
                if( pc.ptAct != NULL ){
                    resetMap();                    // ?????????
                    // ???????
                    if( bNewServer)
                        lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
                    else
                        old_lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
                    eventEnemyFlag = 0;
                }
            }
#endif
*/
            // ?????????
            if( EncountFlag == TRUE ){
                resetPc();                // PC????
                resetCharObj();            // ?????????????
                resetMap();                // ?????????
                clearPtActPartyParam();    // ????????????????NULL???
                fieldInfoTime = 0;        // ???????
                drawFieldInfoWin();        // ???????
                resetFieldProc();        // ????????????
                nowEncountPercentage = minEncountPercentage;// ????????????
                sendEnFlag = 0;
                encountNowFlag = 1;
                eventEnemySendFlag = 0;
                duelSendFlag = 0;
                jbSendFlag = 0;
                // ?????????????????
                if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
                // ???????????
                ResultWndTimer = RESULT_WND_CLOSE_TIME;
                InitMenu();            // ??????????
                BattleCmd[ 0 ] = NULL;        // ??????????
//                BattleCmdBak[ 0 ] = NULL;    // ??????????
//                BattleCmdReadPointer = BattleCmdWritePointer =0;
                BattleStatus[ 0 ] = NULL;    // ???┤???
                //BattleStatusBak[ 0 ] = NULL;// ???┤???
//                BattleStatusReadPointer = BattleStatusWritePointer =0;
                BattleTurnReceiveFlag = TRUE;    // ????????
                // ???????
                play_se( 215, 320, 240 );
                // ??????卒㎏
                now_bgm = t_music_bgm_no;
                // ???于
                stop_bgm();
                //SubProcNo++;
                ChangeProc2( PROC_GAME, SubProcNo+1 );
            //    printf("\n桵須啎揭燴硒俴俇救 SubProcNo=%d",SubProcNo+1);
            }
            break;
            
        case 4:    // ????????????????????
            // ???????????? 
            CopyBackBuffer();
            // ????????丙???阪?
            BackBufferDrawType = DRAW_BACK_PRODUCE; 
            SubProcNo++;
        //    printf("\nDRAW_BACK_PRODUCE硒俴俇救 SubProcNo=%d",SubProcNo);
            break;
            
        case 5:    // ???????
        
            // ??
            if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
            //if( DrawProduce( PRODUCE_RIGHT_ACCELE ) == TRUE ){
            //if( DrawProduce( PRODUCE_LEFT_RIGHT_ACCELE ) == TRUE ){
            //if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
                // ????????
#ifdef _HALLOWEEN_EFFECT
                initMapEffect(FALSE);    
#endif
                ChangeProc( PROC_BATTLE );
        //        printf("\nPROC_BATTLE載遙俇救 SubProcNo=%d",SubProcNo);
            }
            
            MenuProc();    // ??????
        //    printf("\nMenuProc硒俴俇救");
            ImeProc();    // ???????
        //    printf("\nImeProc硒俴俇救");
            break;
    }
}


// ?????
void warpEffectProc( void )
{
    oldMapEffectRainLevel = mapEffectRainLevel;
    oldMapEffectSnowLevel = mapEffectSnowLevel;

    DispBuffer.DispCnt = 0;
    FontCnt = 0;

    // ?????????????????
    if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
    InitMenu2();

    fieldProc();            // ?????????
    initCharPartsPrio();    // ?????????！???叉??????
    openServerWindowProc();    // ?????????

    fieldInfoTime = 0;        // ???????
    drawFieldInfoWin();        // ???????


    /* ????????? */
    RunAction();
    // ???牙????????????
    StockTaskDispBuffer();

    mapEffectProc();        // ???????????．??

    redrawMap();
    drawMap();        // ???牙
    /* ?????? */
    ChatProc();
    // ??????????????????????
    ChatBufferToFontBuffer(); 
    // ??┼?????????????
//    TimeZoneProc();
    SortDispBuffer();     // 牙???????
    // ???????????????
    ClearBackSurface();    
    // ????????????????
    PutBmp();    

#ifndef __SKYISLAND
    // ??????????????????????
    lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
}


void repairMap( void )
{
    float dx, dy;
#ifdef _MOVE_SCREEN
    if (pc.bMoveScreenMode)
    {
        dx = (float)(+ (-nowGx) * (SURFACE_WIDTH >> 1) + -nowGy * (SURFACE_WIDTH >> 1) + viewOffsetX);
        dy = (float)(- (-nowGx) * (SURFACE_HEIGHT >> 1) + -nowGy * (SURFACE_HEIGHT >> 1) + viewOffsetY);
    }
    else
#endif
    //
        camMapToGamen( 0.0, 0.0, &dx, &dy );

    nowXFastDraw = (int)(dx+.5);
    nowYFastDraw = (int)(dy+.5);

    nowXFastDraw2 = nowXFastDraw;
    nowYFastDraw2 = nowYFastDraw;

    //???????????????
    if( ResoMode == 1 ){
        nowXFastDraw = (int)(dx/2+.5);
        nowYFastDraw = (int)(dy/2+.5);
    }

    baseXFastDraw = nowXFastDraw;
    baseYFastDraw = nowYFastDraw;
    amountXFastDraw = nowXFastDraw - baseXFastDraw;
    amountYFastDraw = nowYFastDraw - baseYFastDraw;

    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    drawTile();        // ???牙
    SortDispBuffer();     // 牙???????
    // ???????????????
    //cary 2001 11 8
    ClearBackSurface();    

    // ????????????????
    fastDrawTileFlag = 0;
    PutBmp();    
    fastDrawTileFlag = 1;
#ifndef __SKYISLAND
    // ??????????????????????
    lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
    DispBuffer.DispCnt = 0;
    FontCnt = 0;
    // ????????
    NowTime = TimeGetTime();
}


void paletteProc( void )
{
    if( palNo == -1 )
    {
        // ????????
        // the second PaletteChange( SaTimeZoneNo, 0 );
        PaletteChange( SaTimeZoneNo, palTime );
        // ??┼????????????
        TimeZonePalChangeFlag = TRUE;
        palNo = -2;
    }
    else
    if( palNo >= 0 )
    {
        // ?叉????　叉
        // the second PaletteChange( palNo, 0 );// ????????
        PaletteChange( palNo, palTime );// ????????
        // ??┼?????????????
        TimeZonePalChangeFlag = FALSE;
        palNo = -2;
    }
}

#ifdef _SURFACE_ANIM
extern ACTION *SPACT[MAX_ANIM];
void AniProc()
{
    float mx,my;

    for (int i = 0; i < MAX_ANIM; i++)
    {
        if(SPACT[i] != NULL)
        {
            camMapToGamen(SPACT[i]->mx, SPACT[i]->my, &mx, &my);
            if(SPACT[i]->anim_chr_no > 102002){
                SPACT[i]->x = (int)(mx - 5);
                SPACT[i]->y = (int)(my + 8);
            }else{
                SPACT[i]->x = (int)(mx +.5);
                SPACT[i]->y = (int)(my +.5);
            }
            pattern(SPACT[i], ANM_NOMAL_SPD, ANM_LOOP);
        }
    }
}
#endif
