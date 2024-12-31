#pragma once

enum {
  GAME_LOGIN,
  GAME_FIELD,
  GAME_FIELD_TO_ENCOUNT,
  GAME_ENCOUNT_TO_BATTLE,
  GAME_BATTLE
};


extern int   GameState;
extern DWORD NowTime;


bool GameMain( void );

bool GameInit( void );

void GameEnd( void );


#ifdef _STONDEBUG_
void InfoDisp( void );
#endif

void DisplayFrameRate( void );

