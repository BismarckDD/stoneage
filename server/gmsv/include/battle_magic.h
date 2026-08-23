#ifndef __BATTLE_MAGIC_H__
#define __BATTLE_MAGIC_H__

#define MAGIC_EFFECT_USER	100600
#define SPR_heal	100601
#define SPR_heal2	100602
#define SPR_heal3	100603
#define SPR_tyusya	100604
#define SPR_hoshi	100605

#define MAGIC_ID_RECOVERY	 		1
#define MAGIC_ID_STATUS_RECOVERY	3
#define MAGIC_ID_FIELD_CHANGE 		4
#define MAGIC_ID_BADSTATUS	 		5

enum{
	BFUKI_CAPTUREUP,
	BFUKI_END
};


enum{
	PC_KIND_NONE,
	PC_KIND_ATTACK,
	PC_KIND_DEFENSE,
	PC_KIND_QUICK,
	PC_KIND_CHARM,
	PC_KIND_CAPTURE,
	PC_KIND_END
};

int	MAGIC_Recovery_Battle(
	int char_index,
	int toindex,
	int magicindex,
	int mp
);

int	MAGIC_FieldAttChange_Battle(
	int char_index,
	int toindex,
	int marray,
	int mp
);

int	MAGIC_StatusChange_Battle(
	int char_index,
	int toindex,
	int marray,
	int mp
);

#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange_Battle2(
	int char_index,
	int toindex,
	int marray,
	int mp
);
#endif

#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange_Battle( int char_index, int toNo, int marray, int mp );

#endif

int	MAGIC_MagicDef_Battle(
	int char_index,
	int toNo,
	int marray,
	int mp
);

void BATTLE_MultiRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int per,
	int UseEffect,
	int RecevEffect

);

#ifdef _IMPRECATE_ITEM
void BATTLE_ImprecateRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int powers,
	int rounds,
	int UseEffect,
	int RecevEffect
);
#endif
void BATTLE_MultiStatusChange(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int turn,
	int UseEffect,
	int RecevEffect,
	int Success
);

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MultiMagicStatusChange(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int turn,
	int UseEffect,
	int RecevEffect,
	int nums
);
#endif

void BATTLE_MultiMagicDef(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int count,
	int UseEffect,
	int RecevEffect
);

void BATTLE_MultiParamChange(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int par,
	int UseEffect,
	int RecevEffect
);

int	MAGIC_FieldAttChange_Battle(
	int char_index,
	int toNo,
	int marray,
	int mp 
);

int	BATTLE_FieldAttChange(
	int char_index,
	char *pArg
);

void BATTLE_MultiAttReverse(
	int battleindex,
	int attackNo,
	int toNo,
	int UseEffect,
	int RecevEffect
);


void BATTLE_MultiStatusRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int UseEffect,
	int RecevEffect
);

int	MAGIC_StatusRecovery_Battle(
	int char_index,
	int toNo,
	int marray,
	int mp
);

void BATTLE_MultiRessurect(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int per,
	int UseEffect,
	int RecevEffect
);

void BATTLE_MultiCaptureUp(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int UseEffect,
	int RecevEffect
);

int	MAGIC_Ressurect_Battle(
	int char_index, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
);

int	MAGIC_AttReverse_Battle(
	int char_index, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
);

int	MAGIC_CaptureUp_Battle(
	int char_index, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
);

void BATTLE_MultiResAndDef( 
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int power,			// 湘  荚汊袄
	int per,			// ⊙井＂
	int kind, 			//   芊  豢及潘  
	int count, 			// 窒荚  什井
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
);
//
//***********************************************************
//****************************************************************
//
// 汊唾≈  芊  豢毛芨尹月热诸
//
int	MAGIC_ResAndDef_Battle(
	int char_index, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
);
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

#ifdef _ATTACK_MAGIC
int MAGIC_AttMagic_Battle( int char_index , int toNo , int marray , int mp );
#ifdef _FIX_MAGICDAMAGE
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int MagicLv);

#else
void BATTLE_MultiAttMagic( int battleindex , int attackNo , int toNo , int attIdx , int FieldAttr , int Power );
#endif
int MAGIC_AttMagic_Battle( int char_index , int toNo , int marray , int mp );

#endif



#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon_Battle( int char_index , int toNo , int marray , int mp );

void BATTLE_MultiToCallDragonMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int ImageNo);

#endif

#ifdef _Item_ReLifeAct
void BATTLE_MultiReLife( int battleindex, int attackNo, int toNo, int power, int RecevEffect );
#endif
#ifdef _MAGIC_WEAKEN
int	MAGIC_ParamChange_Turn_Battle(int char_index,int toNo,int marray,int mp);
void BATTLE_MultiParamChangeTurn( int battleindex,int attackNo,int toNo,int status,int UseEffect,int RecevEffect,int turn, int Success);
#endif
int BATTLE_MagicEffect(	int battleindex,int attackNo,int ToList[],int MyEffect,int ToEffect );


int BATTLE_AttrCalc(int My_Fire,   // 施法玩家火属性
                    int My_Water,  // 施法玩家水属性
                    int My_Earth,  // 施法玩家土属性
                    int My_Wind,   // 施法玩家风属性
                    int My_None,   // 施法玩家无属性
                    int Vs_Fire,   // 锹澎础
                    int Vs_Water,  // 目标玩家水属性
                    int Vs_Earth,  // 目标玩家土属性
                    int Vs_Wind,   // 目标玩家风属性
                    int Vs_None);



#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
void PROFESSION_MAGIC_ATTAIC( int battleindex, int attackNo, int toNo, int attIdx, int FieldAttr, int skill);
int PROFESSION_MAGIC_ATTAIC_Effect( int battleindex, int attackNo, int ToList[], int AttackMgcNo );
int analysis_profession_parameter( int attIdx, int skill, int toNo, int char_index );
void PROFESSION_MAGIC_GET_PRACTICE( float *hp_power, float *mp_power, float *dec_hp, float *dec_mp, int char_index );
void PROFESSION_MAGIC_TOLIST_SORT( int *list, int *listidx, int char_index );
void PROFESSION_MAGIC_CHANGE_STATUS( int char_index, int hp_power, float mp_power, float *add_hp, float *add_mp );
void PROFESSION_MAGIC_GET_IMG2( int toNo, int char_index, int attIdx, char *pszOption );
#ifdef _PROFESSION_ADDSKILL
int PROFESSION_MAGIC_GET_DAMAGE( int attackindex, int defindex, int magic_type, int power, int command );
#else
int PROFESSION_MAGIC_GET_DAMAGE( int attackindex, int defindex, int magic_type, int power );
#endif
int PROFESSION_MAGIC_DODGE( int atk_index, int def_index, int magic_type );
void PROFESSION_MAGIC_CHANG_IMG2( int img2, char *pszOption, int attIdx );
int PROFESSION_MAGIC_CHANG_STATUS(int command,int battleindex, int char_index, int charaidx, int power, int no);
int PROFESSION_MAGIC_GET_ICE_MIRROR_DAMAGE( int attackindex, int defindex, int command, int power );
#endif

#ifdef _PETSKILL_FIREKILL
void BATTLE_MultiAttMagic_Fire( int battleindex, int attackNo, int defNo,
						  int FieldAttr , int Power);
#endif

#ifdef _PROFESSION_ADDSKILL
unsigned int GET_PROFESSION_magic_uiSpriteNum(int idx);
#endif

#endif
