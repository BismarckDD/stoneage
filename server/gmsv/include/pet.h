#ifndef __PET_H__
#define __PET_H__

#include "version.h"
#include "char.h"

#define PETFEEDTIME	60*60	// 宠物蛋喂养时间(秒)

int PET_DEBUG_initPetOne( int char_index);
int PET_dropPet( int char_index, int havepetindex);
int PET_dropPetFollow( int char_index, int havepetindex, int fl, int x, int y);
int PET_dropPetFLXY( int char_index, int havepetindex, int fl, int x, int y);
int PET_dropPetAbsolute( int petindex, int floor, int x, int y,BOOL net);
int PET_createPetFromCharaIndex( int char_index, int enemy_index);
#define PET_createPetFromchar_index PET_createPetFromCharaIndex
BOOL PET_SelectBattleEntryPet( int char_index, int petarray);
int PET_initCharOneArray(Char *ch);
BOOL PET_isPutPoint( int fl,int x, int y);


BOOL PET_getBaseForAllocpoint( int toindex, int *work);
void PET_showEditBaseMsg( int char_index, int toindex, int item_index, int *work);

#ifdef _PET_EVOLUTION
BOOL PET_getBaseAndSkill( int char_index, int baseindex, int *base, int *skill, int flg);
#endif
BOOL CHAR_DelPetForIndex( int char_index, int petindex);

#endif

