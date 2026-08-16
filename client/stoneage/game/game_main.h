#ifndef __GAME_MAIN_H__
#define __GAME_MAIN_H__

#ifdef __GAME_MAIN_CPP__
#define EXTERN
#else
#define EXTERN extern 
#endif /* __GAME_MAIN_CPP__ */

enum {
  GAME_LOGIN,
  GAME_FIELD,
  GAME_FIELD_TO_ENCOUNT,
  GAME_ENCOUNT_TO_BATTLE,
  GAME_BATTLE
};

EXTERN int GameState;
EXTERN DWORD NowTime;

bool GameMain(void);
bool GameInit(void);
void GameEnd(void);

void DisplayFrameRate(void);


#endif /* __GAME_MAIN_H__ */