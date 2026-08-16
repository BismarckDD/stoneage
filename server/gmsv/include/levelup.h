#ifndef _LEVELUP_H
#define _LEVELUP_H

int GetEnemyExp( int level );

int BATTLE_GetLevelExp(
	int char_index,
	int level
);

int BATTLE_LevelUpCheck(
	int char_index
);

#endif

