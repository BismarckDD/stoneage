#ifndef __PET_SKILL_H__
#define __PET_SKILL_H__

#include "util.h"

typedef enum
{
	PETSKILL_FIELD_ALL,			/* 允屯化及桦赭匹银尹月 */
	PETSKILL_FIELD_BATTLE,				/* 爵    及心 */
	PETSKILL_FIELD_MAP,				/* 骚橘穴永皿晓及心 */

}PETSKILL_FIELDTYPE;

typedef enum
{
	PETSKILL_TARGET_MYSELF,		/* 愤坌及心 */
	PETSKILL_TARGET_OTHER,			/* 职及谛  愤坌殖戈) */
	PETSKILL_TARGET_ALLMYSIDE,		/*     蝈   */
	PETSKILL_TARGET_ALLOTHERSIDE,	/* 锹澎础蝈   */
	PETSKILL_TARGET_ALL,			/* 蝈化 */
	PETSKILL_TARGET_NONE,			/* 簿手蓟  请  卅中［  豢支凶户及凛 */
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,/* 职及谛  愤坌殖引卅中) */
	PETSKILL_TARGET_WITHOUTMYSELFANDPET,  /* 愤坌午矢永玄动陆 */
}PETSKILL_TARGETTYPE;

typedef enum
{
	PETSKILL_ID,
	PETSKILL_FIELD,
	PETSKILL_TARGET,
#ifdef _PETSKILL2_TXT
	PETSKILL_USETYPE,
#endif
	PETSKILL_COST,
	PETSKILL_ILLEGAL,
	PETSKILL_DATAINTNUM,
}PETSKILL_DATAINT;

typedef enum
{
	PETSKILL_NAME,					/*      */
	PETSKILL_COMMENT,				/* 戊丢件玄*/
	PETSKILL_FUNCNAME,				/* 楮醒   */
	PETSKILL_OPTION,				/* 左皿扑亦件 */
#ifdef _CFREE_petskill
	PETSKILL_FREE,					/*条件*/
	PETSKILL_KINDCODE,				/*种类码*/
#endif

	PETSKILL_DATACHARNUM,
}PETSKILL_DATACHAR;

typedef struct tagPetskill
{
	int			data[PETSKILL_DATAINTNUM];
	STRING64	string[PETSKILL_DATACHARNUM];

}Petskill;

typedef int (*PETSKILL_CALLFUNC)( int, int, int, char * );

INLINE BOOL PETSKILL_CHECKINDEX( int index );
INLINE int PETSKILL_getInt( int index, PETSKILL_DATAINT element);
INLINE int PETSKILL_setInt( int index, PETSKILL_DATAINT element, int data);
INLINE char* PETSKILL_getChar( int index, PETSKILL_DATACHAR element);
INLINE BOOL PETSKILL_setChar( int index ,PETSKILL_DATACHAR element, char* new );
int PETSKILL_getPetskillNum( void);

#define		PETSKILL_GetArray( char_index, havepetskill)	_PETSKILL_GetArray( __FILE__, __LINE__, char_index, havepetskill)
int _PETSKILL_GetArray( char *file, int line, int char_index, int havepetskill );

BOOL PETSKILL_initPetskill( char *filename);
BOOL PETSKILL_reinitPetskill( void );
int PETSKILL_getPetskillArray( int petskillid);
PETSKILL_CALLFUNC PETSKILL_getPetskillFuncPointer(char* name);

int PETSKILL_Use(
	int char_index,
	int toindex,
	int array,
	char *data
	//BOOL isCLI	// Robin 2001/02/26 if owner is player
);

int PETSKILL_ContinuationAttack(
	int char_index,
	int toindex,
	int array,
	char *data
);
int PETSKILL_ChargeAttack(
	int char_index,
	int toindex,
	int array,
	char *data
);


#define PETSKILL_ID_GBREAK		0	// 布□玉皮伊奶弁
#define PETSKILL_ID_RENZOKU		1	//   粮  猾
#define PETSKILL_ID_GUARDIAN	2	// 镝舅甩民跤
#define PETSKILL_ID_CHARGE		3	// 民乓□斥  猾
#define PETSKILL_ID_ICHIGEKI	100	// 域猾  诮
#define PETSKILL_ID_POWERBALANCE 110	//     及讽

//**********************************************************************
//
//  檗  -- 职谛毛潮曰卅互日  猾
//
int PETSKILL_Guardian(
	int char_index,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************

int PETSKILL_PowerBalance(
	int char_index,
	int toindex,
	int array,
	char *data
);

//**********************************************************************
//
//  檗  -- 雄  分互荚  今木月割  互嫖中
//
int PETSKILL_Mighty(
	int char_index,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************


//**********************************************************************
//
//  檗  -- 旦  □正旦唱橘  猾
//
int PETSKILL_StatusChange(
	int char_index,
	int toindex,
	int array,
	char *data

);
//
//**********************************************************************


//*******************************************************
//
// 檗  -- 骚橘  猾
//
int PETSKILL_NormalAttack(
	int char_index,
	int toindex,
	int array,
	char *data

);


#ifdef _INVERSION_PETSKILL
int PETSKILL_Inversion(
	int char_index,
	int toindex,
	int array,
	char *data

);
#endif

#ifdef _MASSAGE_PETSKILL
int PETSKILL_Massage(
	int char_index,
	int toindex,
	int array,
	char *data

);
#endif

#ifdef _PETOUT_PETSKILL
int PETSKILL_Petout(
	int char_index,
	int toindex,
	int array,
	char *data

);
#endif

//
//*******************************************************

//*******************************************************
//
// 檗  -- 骚橘  豢
//
int PETSKILL_NormalGuard(
	int char_index,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 檗  -- 谨窗(窒手仄卅中)
//
int PETSKILL_None(
	int char_index,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 檗  -- 哗萄域葱(怅木化ㄠ正□件  卞锹澎及    井日  猾)
//
int PETSKILL_EarthRound(
	int char_index,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 檗  -- 布□玉皮伊奶弁
//
int PETSKILL_GuardBreak(
	int char_index,
	int toNo,
	int array,
	char *data

);
///////////////////////////////////////////
#ifdef _SKILL_GUARDBREAK2//破除防御2 vincent add 2002/05/20
int PETSKILL_GuardBreak2(
	int char_index,
	int toNo,
	int array,
	char *data

);
#endif
//
//*******************************************************
//*******************************************************
// 檗  --     木
//
int PETSKILL_Abduct(
	int char_index,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************
//*******************************************************
// 檗  --   戈
int PETSKILL_Steal(
	int char_index,
	int toNo,
	int array,
	char *data

);
//*******************************************************
#ifdef _BATTLESTEAL_FIX
int PETSKILL_StealMoney(
	int char_index,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _ITEM_INSLAY
int PETSKILL_Inslay(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_Fixitem(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

// 檗  -- 失奶  丞宁岳
//
int PETSKILL_Merge(
	int char_index,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 檗  -- 用□布□玉
//
int PETSKILL_NoGuard(
	int char_index,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************



// Terry add 2001/11/05
#ifdef _ATTACK_MAGIC
int PETSKILL_AttackMagic(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _VARY_WOLF
int PETSKILL_Vary( int cindex, int tindex, int id, char* data);
#endif

#ifdef _SKILL_WILDVIOLENT_ATT
//vincent add 2002/05/16
int PETSKILL_WildViolentAttack(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SPEEDY_ATT
//vincent add 2002/05/16
int PETSKILL_SpeedyAttack(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SACRIFICE
//vincent add 2002/05/30
int PETSKILL_Sacrifice(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_REFRESH
//vincent add 2002/08/08
int PETSKILL_Refresh(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_WEAKEN  //vincent宠技:虚弱
int PETSKILL_Weaken(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_DEEPPOISON  //vincent宠技:剧毒
int PETSKILL_Deeppoison(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_BARRIER  //vincent宠技:魔障
int PETSKILL_Barrier(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_NOCAST  //vincent宠技:沉默
int PETSKILL_Nocast(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_ROAR //vincent宠技:大吼
int PETSKILL_Roar(
	int char_index,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _PSKILL_FALLGROUND
int PETSKILL_FallGround( int char_index, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_EXPLODE
int PETSKILL_Explode( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int ENEMYSKILL_ReLife( int enemy_index, int toNo, int array, char *data );
int ENEMYSKILL_ReHP( int enemy_index, int toNo, int array, char *data );

int ENEMYSKILL_EnemyHelp( int enemy_index, int toNo, int array, char *data );
#endif

#ifdef _SKILL_DAMAGETOHP
int PETSKILL_DamageToHp( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TIMID
int PETSKILL_BattleTimid( int char_index, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_2TIMID
int PETSKILL_2BattleTimid( int char_index, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_ANTINTER
int PETSKILL_AntInter(int char_index, int toindex, int array, char* data);		// 宠物技能战斗模组
#endif

#ifdef _RESURRECTION_PETSKILL
int PETSKILL_Resurrection( int char_index, int toNo, int array, char* data);
#endif
#ifdef _LOSTLOST_PETSKILL
int PETSKILL_Lostlost( int char_index, int toNo, int array, char* data);
#endif


#ifdef _PETSKILL_PROPERTY
int PETSKILL_BattleProperty( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TEAR
int PETSKILL_BattleTearDamage( int char_index, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_LIGHTTAKE
int PETSKILL_Lighttakeed( int char_index, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_ATTCRAZED
int PETSKILL_AttackCrazed( int char_index, int toNo, int array, char *data );
#endif

#ifdef _SHOOTCHESTNUT	// Syu ADD 宠技：丢栗子
int PETSKILL_AttackShoot( int char_index, int toNo, int array, char *data );
#endif

#ifdef _Skill_MPDAMAGE
int PETSKILL_MpDamage( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_SETDUCK
int PETSKILL_SetDuck( int char_index, int toNo, int array, char *data );
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet( int char_index, int toNo, int array, char *data );
#endif

#ifdef _SKILL_TOOTH
int PETSKILL_ToothCrushe( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MODIFY
int PETSKILL_Modifyattack( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MDFYATTACK
int PETSKILL_Mdfyattack( int char_index, int toNo, int array, char *data );
#endif


#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange( int char_index, int toindex, int array, char *data );
#endif

#ifdef _PET_SKILL_SARS				// WON ADD 毒煞蔓延
int PETSKILL_Sars( int char_index, int toNo, int array, char *data );
#endif

#ifdef _SONIC_ATTACK				// WON ADD 音波攻击
int PETSKILL_Sonic( int char_index, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_REGRET
int PETSKILL_Regret( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_GYRATE
int PETSKILL_Gyrate( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_ACUPUNCTURE //针刺外皮
int PETSKILL_Acupuncture( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_RETRACE
int PETSKILL_Retrace( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_HECTOR
int PETSKILL_Hector( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_FIREKILL
int PETSKILL_Firekill( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_DAMAGETOHP   //暗月狂狼(嗜血技的变体)
int PETSKILL_DamageToHp2( int char_index, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_BECOMEFOX
int PETSKILL_BecomeFox( int char_index, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_BECOMEPIG
int PETSKILL_BecomePig( int char_index, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_SHOWMERCY
int PETSKILL_ShowMercy(int char_index, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_COMBINED
int PETSKILL_Combined(int char_index, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_LER
int PETSKILL_BatFly(int char_index, int toNo, int array, char* data);					// 雷尔技 - 群蝠四窜
int PETSKILL_DivideAttack(int char_index, int toNo, int array, char* data);		// 雷尔技 - 分身地裂
#endif

#ifdef _PETSKILL_BATTLE_MODEL
int PETSKILL_BattleModel(int char_index, int toindex, int array, char* data);		// 宠物技能战斗模组
#endif

#ifdef _STRENGTH_PETSKILL
int PETSKILL_Strength( int char_index, int toNo, int array, char *data );
#endif
#ifdef _GRAPPLING_PETSKILL
int PETSKILL_Grappling( int char_index, int toNo, int array, char* data);
#endif
#ifdef _PETSKILL_PASSIVE_PET_MATCH1		//宠物单人组合被动技能
int PETSKILL_PASSIVE_PET_MATCH1( int char_index, int toNo, int array, char *data );
#endif

#endif
