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

#ifdef _ANNOUNCEMENT_

void announce(void);

EXTERN char gAnnouncementContent[512];
#ifdef __GAME_MAIN_CPP__
int gAnnouncementNum = 0;
int gAnnouncementColor = -1;
int gAnnouncementTime = 0;
#else
extern int gAnnouncementNum;
extern int gAnnouncementColor;
extern int gAnnouncementTime;
#endif

#endif


#endif /* __GAME_MAIN_H__ */