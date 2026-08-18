#pragma once

#define BATTLE_MAP_FILES 220

#ifdef __BATTLE_MAP_CPP__
int BattleMapNo = 0;
#else
extern int BattleMapNo;
#endif

BOOL ReadBattleMap(const int no);

void DrawBattleMap(void);
