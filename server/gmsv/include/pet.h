#ifndef __PET_H__
#define __PET_H__

#include "version.h"
#include "char.h"

#define PETFEEDTIME	60*60	// 宠物蛋喂养时间(秒)

int PET_DEBUG_initPetOne( int charaindex);
int PET_dropPet( int charaindex, int havepetindex);
int PET_dropPetFollow( int charaindex, int havepetindex, int fl, int x, int y);
int PET_dropPetFLXY( int charaindex, int havepetindex, int fl, int x, int y);
int PET_dropPetAbsolute( int petindex, int floor, int x, int y,BOOL net);
int PET_createPetFromCharaIndex( int charaindex, int enemyindex);
BOOL PET_SelectBattleEntryPet( int charaindex, int petarray);
int PET_initCharOneArray(Char *ch);
BOOL PET_isPutPoint( int fl,int x, int y);


BOOL PET_getBaseForAllocpoint( int toindex, int *work);
void PET_showEditBaseMsg( int charaindex, int toindex, int itemindex, int *work);

#ifdef _PET_EVOLUTION
BOOL PET_getBaseAndSkill( int charaindex, int baseindex, int *base, int *skill, int flg);
#endif
BOOL CHAR_DelPetForIndex( int charaindex, int petindex);

#endif

