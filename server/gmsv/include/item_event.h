#ifndef __ITEM_EVENT_H__
#define __ITEM_EVENT_H__
#include "item.h"
int ITEM_eventDrop( int item_index, int char_index, int itemchar_index );
BOOL ITEM_MedicineInit( ITEM_Item* itm );
void ITEM_MedicineUsed( int char_index, int to_char_index, int item_index );
void ITEM_SandClockDetach( int charindex , int item_index );
void ITEM_chantMagicAttack( int char_index, int item_index, int toindex, float* damage );
void ITEM_addTitleAttach( int char_index, int item_index );
void ITEM_delTitleDetach( int char_index, int item_index );
void ITEM_DeleteByWatched( int myobjindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen );
void ITEM_DeleteTimeWatched( int myobjindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen );
void ITEM_useEffectTohelos( int char_index, int to_char_index, int haveitem_index);
void ITEM_useRecovery( int char_index, int toindex, int haveitem_index);
void ITEM_WarpDelErrorItem( int char_index );
void ITEM_useStatusChange( int char_index, int toindex, int haveitem_index);
void ITEM_useMagicDef( int char_index, int toindex, int haveitem_index);
void ITEM_useParamChange( int char_index, int toindex, int haveitem_index);
void ITEM_useFieldChange( int char_index, int toindex, int haveitem_index);
void ITEM_useAttReverse( int char_index, int toindex, int haveitem_index);
void ITEM_useStatusRecovery( int char_index, int toindex, int haveitem_index);
void ITEM_useRessurect( int char_index, int toindex, int haveitem_index);
void ITEM_useMic( int char_index, int to_char_index, int haveitem_index );
void ITEM_dropMic( int char_index, int item_index );
void ITEM_useCaptureUp( int char_index, int toindex, int haveitem_index);
void ITEM_useRenameItem( int char_index, int toindex, int haveitem_index);
void ITEM_useRenameItem_WindowResult( int char_index, int seqno, int select, char *data);
void ITEM_dropDice( int char_index, int item_index);
void ITEM_pickupDice( int char_index, int item_index);
void ITEM_useLottery( int char_index, int toindex, int haveitem_index);
BOOL ITEM_WarpForAny(int char_index, int ff, int fx, int fy,int flg);
void ITEM_useWarp( int char_index, int toindex, int haveitem_index );
void ITEM_petFollow( int char_index, int toindex, int haveitem_index );

#ifdef _PETFOLLOW_NEW_
void ITEM_petFollowNew( int char_index, int toindex, int haveitem_index );
#endif

BOOL ITEM_initLottery(ITEM_Item* itm);
void ITEM_useSkup( int char_index, int toindex, int haveitem_index ); // Nuke 0624
void ITEM_useNoenemy( int char_index, int toindex, int haveitem_index ); // Nuke 0626
void ITEM_equipNoenemy( int char_index, int item_index ); // Arminius 7.2: Ra's amulet
void ITEM_remNoenemy( int char_index, int item_index ); // Arminius 7.2: Ra's amulet


/*
Params:
  argument (const char*): one line data joint by "|".
  entry (const char*): 
  val (char *): 
  val_len (const int): 
Return Values:

*/
BOOL ITEM_getArgument(const char* argument, const char* entry, char* value, const int value_len);	
void ITEM_useEncounter( int char_index, int toindex, int haveitem_index); // Arminius 7.31 cursed stone
#ifdef _ITEM_METAMO
void ITEM_metamo( int char_index, int toindex, int haveitem_index );
void ITEM_ColorMetamo( int char_index, int toindex, int haveitem_index);
void ITEM_CharaMetamo( int char_index, int toindex, int haveitem_index);
void ITEM_SexMetamo( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_CRACKER
void ITEM_Cracker(int char_index,int toindex,int haveitem_index);
#endif
#ifdef _DEL_DROP_GOLD
void GOLD_DeleteTimeCheckLoop( void);
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
void ITEM_Addexp(int char_index,int toindex,int haveitem_index);
#endif
#ifdef _ITEM_REFRESH //vincent 解除异常状态道具
void ITEM_Refresh(int char_index,int toindex,int haveitem_index);
#endif
//Terry 2001/12/21
#ifdef _ITEM_FIRECRACKER
void ITEM_firecracker(int char_index,int toindex,int haveitem_index);
#endif
//Terry end

#ifdef _PET_LIMITLEVEL
void ITEM_useOtherEditBase( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_EDITBASES
void ITEM_useFusionEditBase( int char_index, int toindex, int haveitem_index);
#endif

void ITEM_WearEquip( int char_index, int item_index);
void ITEM_ReWearEquip( int char_index, int item_index);


#ifdef _ITEM_CONSTITUTION
void ITEM_Constitution( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _Item_ReLifeAct
void ITEM_DIErelife( int char_index, int item_index, int eqw );
#endif

#ifdef _Item_MoonAct
void ITEM_randEnemyEquipOne( int char_index, int toindex, int haveitem_index);
void ITEM_randEnemyEquip( int char_index, int item_index);
void ITEM_RerandEnemyEquip( int char_index, int item_index);
#endif

#ifdef _ITEM_ORNAMENTS
void ITEM_PutOrnaments( int char_index, int item_index);
#endif

#ifdef _CHIKULA_STONE
void ITEM_ChikulaStone( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _SUIT_ITEM
void ITEM_suitEquip( int char_index, int item_index);
void ITEM_ResuitEquip( int char_index, int item_index);
#endif

#ifdef _EQUIT_DEFMAGIC
void ITEM_MagicEquitWear( int char_index, int item_index);
void ITEM_MagicEquitReWear( int char_index, int item_index);
#endif

#ifdef _EQUIT_RESIST
void ITEM_MagicResist( int char_index, int item_index);
void ITEM_MagicReResist( int char_index, int item_index);
#endif

#ifdef _MAGIC_RESIST_EQUIT			// WON ADD 职业抗性装备
void ITEM_P_MagicEquitWear( int char_index, int item_index );
void ITEM_P_MagicEquitReWear( int char_index, int item_index );
#endif


#ifdef _Item_DeathAct
void ITEM_UseDeathCounter( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _FEV_ADD_NEW_ITEM			// FEV ADD 增加复活守精
void ITEM_ResAndDef( int char_index, int toindex, int haveitem_index );
#endif

#ifdef _CHRISTMAS_REDSOCKS
void ITEM_useMaxRedSocks( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _CHRISTMAS_REDSOCKS_NEW
void ITEM_useMaxRedSocksNew( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _PETSKILL_CANNEDFOOD
void ITEM_useSkillCanned( int char_index, int toindex, int itemNo);
#endif

#ifdef _NEW_RIDEPETS
void ITEM_useLearnRideCode( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_WARP_FIX_BI
void recoverbi(int index);
#endif

#ifdef _ITEM_TIME_LIMIT
void ITEM_TimeLimit( int char_index);
#endif

#ifdef _USEWARP_FORNUM
void ITEM_useWarpForNum( int char_index, int toindex, int haveitem_index );
#endif

#ifdef _IMPRECATE_ITEM
void ITEM_useImprecate( int char_index, int toNo, int haveitem_index );
#endif

#ifdef _THROWITEM_ITEMS
void ITEM_ThrowItemBox( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_WATERWORDSTATUS
void ITEM_WaterWordStatus( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_LOVERPARTY
void ITEM_LoverSelectUser( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic( int char_index, int toindex, int haveitem_index);
#endif
 
#ifdef _PETSKILL_CANNEDFOOD
void ITEM_usePetSkillCanned_WindowResult( int char_index, int seqno, int select, char *data);
#endif

void ITEM_AddPRSkillPoint(int char_index,int toindex,int haveitem_index);
void ITEM_AddPRSkillPercent( int char_index,int toindex,int haveitem_index);

#ifdef _ANGEL_SUMMON
void ITEM_AngelToken( int char_index, int toindex, int haveitem_index );
void ITEM_HeroToken( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _HALLOWEEN_EFFECT
void ITEM_MapEffect(int char_index,int toindex,int haveitem_index);
#endif
void ITEM_changePetOwner( int char_index, int toindex, int haveitem_index);

#ifdef _TIME_TICKET
void ITEM_timeticketEx( int char_index, int toindex, int haveitem_index, int flag);
void ITEM_timeticket( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _ITEM_SETLOVER
void ITEM_SetLoverUser( int char_index, int toindex, int haveitem_index);
void ITEM_LoverWarp( int char_index, int toindex, int haveitem_index);
void ITEM_LoverUnmarry( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _GM_ITEM
void ITEM_GMFUNCTION( int char_index, int toindex, int haveitem_index);
#endif

#ifdef _VIP_SERVER
void ITEM_AddMemberPoint( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _VIP_RIDE
void ITEM_VipRide( int char_index, int toindex, int haveitem_index);
#ifdef _NEW_NAME      //自定义称号
void ITEM_NewName( int char_index, int toindex, int haveitem_index);
#endif
#endif
#ifdef _FM_ITEM
void ITEM_AddFame( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _LUCK_ITEM
void ITEM_Luck( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_METAMO_TIME
void ITEM_MetamoTime( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _ITEM_GOLD
void ITEM_Gold( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _MYSTERIOUS_GIFT
void ITEM_MysteriousGift( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _BATTLE_PK
void ITEM_BattlePK( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _SILENTZERO
void ITEM_SetSilentZero( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _PET_LEVEL_ITEM
void ITEM_PetLevelItem( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_EFMETAMO
void ITEM_efMetamo( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _PET_BEATITUDE
void PET_BEATITUDE( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _GET_MULTI_ITEM
void ITEM_GetMultiItem( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _SUPER_FLOOR_MIC
void ITEM_useSuperMic( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_PET_LOCKED
void ITEM_ItemPetLocked( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ONLINE_COST
void ITEM_OnlineCost( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _VIPPOINT_OLD_TO_NEW
void ITEM_OldToNew( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _BOUND_TIME
void ITEM_BoundTime( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _TALK_SIZE_ITEM
void ITEM_TalkSize( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _FORMULATE_AUTO_PK
void ITEM_PointToSQLPkPoint( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _SUPER_MAN_ITEM
void ITEM_SuperManItem( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _COST_ITEM
void ITEM_CostItem( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_EXP
void ITEM_EXP( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _MEMORY_ITEM
void ITEM_WarpItem( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _NULL_CHECK_ITEM
void ITEM_NullCheck( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _SPECIAL_SUIT
void ITEM_SpecialSuitEquip( int char_index, int item_index);
void ITEM_SpecialResuitEquip( int char_index, int item_index);
#endif
#ifdef _MANOR_EQUIP
void ITEM_ManorEquip( int char_index, int item_index);
void ITEM_ReManorEquip( int char_index, int item_index);
#endif
#ifdef _FIND_TREASURES
void ITEM_FindTreasures( int char_index, int toindex, int haveitem_index );
BOOL FindTreasures_init();
#endif
#ifdef _ITEM_POOLITEM
void ITEM_PoolItem( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _ITEM_POOLPET
void ITEM_PoolPet( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _NEW_GM_ITEM
void ITEM_NewGMItem( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _SHOW_PET_ABL
void ITEM_ShowPetAbl( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _NEWEVENT_ITEM
void ITEM_NeweventItem( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _NEW_PET_BEATITUDE
void ITEM_NewPetBeatitude( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _OLYMPIC_TORCH
void ITEM_OlympicTorck( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _PLAYER_DIY_MAP
void ITEM_PlayerDiyMapObj( int char_index, int toindex, int haveitem_index );
void ITEM_PlayerDiyMapTile( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _SHOW_ITEM
void ITEM_ShowItem( int char_index, int toindex, int haveitem_index );
#endif
#ifdef _ITEM_LUA
void ITEM_Lua(int char_index, int toindex, int haveitem_index);
#endif
enum{
	BD_KIND_HP,
	BD_KIND_MP,
	BD_KIND_CHARM,
	BD_KIND_AI,
	BD_KIND_CURSE,
	BD_KIND_BESTOW,
	BD_KIND_WISHES,
#ifdef _ITEM_UNBECOMEPIG
	BD_KIND_UNBECOMEPIG,
#endif
#ifdef _ITEM_PROPERTY
    BD_KIND_PROPERTY,
#endif
	BD_KIND_END
};

#endif
