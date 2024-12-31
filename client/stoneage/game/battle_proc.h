﻿/************************/
/*    battleProc.h        */
/************************/
#ifndef _BATTLE_PROC_H_
#define _BATTLE_PROC_H_

// ??????? ??????吻?
enum{
    BATTLE_SUBPROC_INIT,                // ?ㄩ???
    BATTLE_SUBPROC_IN_PRODUCE,            // ?ㄩ??????
    BATTLE_SUBPROC_RECEIVE_BC,            // ?ㄩ??┴?
    BATTLE_SUBPROC_CHAR_IN,                // ?ㄩ仕?
    BATTLE_SUBPROC_CMD_INPUT,            // ?ㄩ????向?
    BATTLE_SUBPROC_RECEIVE_MOVIE,        // ?ㄩ????┴?
    BATTLE_SUBPROC_MOVIE,                // ?ㄩ??????
    BATTLE_SUBPROC_OUT_PRODUCE_INIT,    // ?ㄩ?????
    BATTLE_SUBPROC_OUT_PRODUCE,            // ?ㄩ??
};

// ?????????
extern BOOL EncountFlag;
// ???????
extern BOOL NoHelpFlag;
#ifdef _MAGIC_NOCAST//麥蘇
extern BOOL NoCastFlag;
#endif
// ???????????
extern BOOL EncountOffFlag;
// ???????
extern BOOL DuelFlag;

// ??????????
extern int BattleDebTurnNo;
// ??????? 
void BattleProc( void );

#endif

