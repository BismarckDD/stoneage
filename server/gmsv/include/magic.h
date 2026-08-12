#ifndef __MAGIC_H__
#define __MAGIC_H__

/* 热诸 */
int MAGIC_Use( int char_index, int haveitem_index, int toindex);
#ifdef _MAGIC_FEATHERS
int MAGIC_Feathers( int char_index, int toindex, int marray, int mp );
#endif

int MAGIC_Recovery( int char_index, int toindex,int marray, int mp );
int MAGIC_OtherRecovery( int char_index, int toindex, int marray, int mp );
int	MAGIC_FieldAttChange( int char_index, int toindex, int marray, int mp );
int	MAGIC_StatusChange( int char_index, int toindex, int marray, int mp );
#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange2( int char_index, int toindex, int marray, int mp );
#endif
int	MAGIC_StatusRecovery( int char_index, int toindex, int marray, int mp );
int	MAGIC_MagicDef( int char_index, int toindex, int marray, int mp );
int	MAGIC_Ressurect( int char_index, int toindex, int marray, int mp );
int	MAGIC_AttReverse( int char_index, int toindex, int marray, int mp );
int	MAGIC_ResAndDef( int char_index, int toindex, int marray, int mp );
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _ATTACK_MAGIC
int     MAGIC_AttMagic( int char_index , int toindex , int marray , int mp );
#endif
#ifdef _ITEM_METAMO
int MAGIC_Metamo( int char_index, int toindex,int marray, int mp );
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill( int char_index, int toindex,int marray, int mp );
#endif
#ifdef _MAGIC_WEAKEN// vincent  精灵:虚弱
int	MAGIC_Weaken( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_BARRIER// vincent  精灵:魔障
int	MAGIC_Barrier( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_NOCAST// vincent  精灵:沉默
int	MAGIC_Nocast( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int char_index, int toindex,int marray, int mp );
#endif

//----------------------------------------------------------------------
// 失奶  丞及匏  井日热诸  寞毛忒允
//
int MAGIC_GetArrayNo(
	int char_index, 	// 平乓仿弁正奶件犯永弁旦
	int haveitem_index   //    匀化月失奶  丞及匏  
);
//
//----------------------------------------------------------------------
//-------------------------------------------------------------------
//
//  热诸毛  涛银丹
//
int MAGIC_DirectUse(
	int char_index, // 银丹平乓仿及奶件犯永弁旦
	int marray, 	// 银丹热诸及奶件犯永弁旦
	int toindex, 	// 簿卞银丹＂
	int itemnum
);
//
//-------------------------------------------------------------------

#endif 

