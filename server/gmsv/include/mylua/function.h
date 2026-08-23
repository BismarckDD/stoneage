
#ifndef __MYLUA_FUNCTION_H__
#define __MYLUA_FUNCTION_H__

BOOL RunCharTalkedEvent(int meindex, int toindex, char *messageeraseescape, int color, int channel);
BOOL RunCharLoopEvent(int meindex);
BOOL RunCharOverlapEvent( int meindex, int toindex);
BOOL RunCharBattleOverEvent( int meindex, int toindex, int iswin);
BOOL RunCharWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#ifdef _ALLBLUES_LUA_1_2 
BOOL RunItemUseEvent( int item_index, int char_index, int toindex, int haveitem_index );
BOOL RunItemDieReLifeEvent( int toindex, int item_index, int haveitem_index );
BOOL RunItemDetachEvent( int char_index, int item_index );
BOOL RunItemAttachEvent( int char_index, int item_index );
BOOL RunItemPickupEvent( int char_index, int item_index );
BOOL RunItemPostOverEvent( int item_index, int char_index );
BOOL RunItemPreOverEvent( int item_index, int char_index );
BOOL RunItemDropEvent( int char_index, int item_index );
BOOL RunUseChatMagic( int char_index, char *data, lua_State *lua);
#endif
#ifdef _PETSKILL_SHOP_LUA   
BOOL FreePetSkillShop( int talkerindex, int petindex, int oldSkillID, int newSkillID);
#endif
#ifdef _PETSKILL_SHOP_LUA   
BOOL OffLineCommand( int battleindex, int charindex, int side);
#endif
#ifdef _ALLBLUES_LUA_1_4
BOOL BattleFinish( int battleindex, int char_index);
BOOL BattleEscape( int battleindex, int char_index);
BOOL RunCharLogOutEvent( int char_index);
#endif
#ifdef _ALLBLUES_LUA_1_5
BOOL FreePartyJoin( int char_index, int toindex );
BOOL FreeVsPlayer( int char_index, int toindex );
BOOL FreeCharLogin( int char_index );
BOOL FreeCharCreate( int char_index );
BOOL NetLoopFunction( void );
#endif
#ifdef _ALLBLUES_LUA_1_6
BOOL FamilyRideFunction( int meindex, int petindex, int petid );
BOOL CharTalkFunction( int char_index, char *message, int color );
#endif
#ifdef _ALLBLUES_LUA_1_7
BOOL CharVsEnemyFunction( int char_index );
#endif
#ifdef _ALLBLUES_LUA_1_8
BOOL CaptureOkFunction( int attackindex, int defindex );
BOOL CaptureCheckFunction( int attackindex, int defindex );
#endif
#ifdef _ALLBLUES_LUA_1_9
BOOL SetBattleEnmeyFunction(int meindex, int enemy_index, int id );
BOOL BattleFinishFunction( int char_index, int battletime, int battleturn, int battletype );
void FreeCharExpSave( int charindex, int exp );
BOOL BattleCommand( int char_index, int battleindex );
#endif
#endif

#ifdef _RIDEQUERY_
void FreeRideQuery( int char_index);
#endif

