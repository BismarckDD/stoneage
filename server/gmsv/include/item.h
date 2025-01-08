#ifndef __ITEM_H__
#define __ITEM_H__

#ifdef _SIMPLIFY_ITEMSTRING
typedef struct ITEM_tagIntDataSetting {
  char *dumpchar;
  int defaults;
  int table;
  char *string;
} ITEM_intDataSetting;

typedef struct ITEM_tagCharDataSetting {
  char *dumpchar;
  char *defaults;
  int table;
  char *string;
} ITEM_charDataSetting;
#endif

typedef enum {
  ITEM_FIST = 0, // 
  ITEM_AXE,      // weapon -> hand.
  ITEM_CLUB,     // weapon -> hand.
  ITEM_SPEAR,    // weapon -> hand.
  ITEM_BOW,      // weapon -> hand.
  ITEM_SHIELD,   // shield -> hand.
  ITEM_HELM,     // helm   -> head.
  ITEM_ARMOUR,   // armor  -> body.
  ITEM_BRACELET = 8, //
  ITEM_MUSIC,
  ITEM_NECKLACE, // 
  ITEM_RING,     // 
  ITEM_BELT,     // ->
  ITEM_EARRING,  // 
  ITEM_NOSERING,
  ITEM_AMULET,
  /* ****** */
  ITEM_OTHER = 16,
  ITEM_BOOMERANG,  //
  ITEM_BOUNDTHROW, //
  ITEM_BREAKTHROW, //
  ITEM_DISH = 20,
#ifdef _ITEM_INSLAY
  ITEM_METAL,
  ITEM_JEWEL,
#endif
#ifdef _ITEM_CHECKWARES
  ITEM_WARES,   //
#endif
#ifdef _ITEM_EQUITSPACE
  ITEM_WBELT,   //
  ITEM_WSHIELD, //
  ITEM_WSHOES,  //
#endif
#ifdef _EQUIT_NEWGLOVE
  ITEM_WGLOVE, //
#endif
#ifdef _ALCHEMIST
  ITEM_ALCHEMIST = 30,
#endif

#ifdef _PET_ITEM
  ITEM_PET_HEAD,
  ITEM_PET_TOOTH,
  ITEM_PET_CLAW,
  ITEM_PET_BREAST,
  ITEM_PET_BACK,
  ITEM_PET_WING,
  ITEM_PET_FEET,
#endif

#ifdef _ANGEL_SUMMON
// ITEM_ANGELTOKEN,
// ITEM_HEROTOKEN,
#endif
  ITEM_CATEGORYNUM,
} ITEM_CATEGORY;

typedef enum {
  ITEM_FIELD_ALL,
  ITEM_FIELD_BATTLE,
  ITEM_FIELD_MAP,
} ITEM_FIELDTYPE;

typedef enum {
  ITEM_TARGET_MYSELF,
  ITEM_TARGET_OTHER,
  ITEM_TARGET_ALLMYSIDE,
  ITEM_TARGET_ALLOTHERSIDE,
  ITEM_TARGET_ALL,
} ITEM_TARGETTYPE;

typedef enum {
  ITEM_ID,
  ITEM_BASEIMAGENUMBER,
  ITEM_COST,
  ITEM_TYPE,
  ITEM_ABLEUSEFIELD,
  ITEM_TARGET,
  ITEM_LEVEL, /* LEVEL */
#ifdef _ITEM_MAXUSERNUM
  ITEM_DAMAGEBREAK, // DAMAGEBREAK?
#endif
#ifdef _ITEMSET4_TXT
  ITEM_USEPILENUMS, //
  ITEM_CANBEPILE,   //
  ITEM_NEEDSTR,
  ITEM_NEEDDEX,
  ITEM_NEEDTRANS,
  ITEM_NEEDPROFESSION,
#endif
  ITEM_DAMAGECRUSHE,
  ITEM_MAXDAMAGECRUSHE,
#ifdef _ADD_DEAMGEDEFC
  ITEM_OTHERDAMAGE,
  ITEM_OTHERDEFC,
#endif
#ifdef _SUIT_ITEM
  ITEM_SUITCODE,
#endif
  ITEM_ATTACKNUM_MIN,     /* min attack */
  ITEM_ATTACKNUM_MAX,     /* mix attack */
  ITEM_MODIFYATTACK,      /* + ATTACK   */
  ITEM_MODIFYDEFENCE,     /* + DEFENCE  */
  ITEM_MODIFYQUICK,       /* + QUICK    */
  ITEM_MODIFYHP,          /* + HP       */
  ITEM_MODIFYMP,          /* + MP       */
  ITEM_MODIFYLUCK,        /* + LUCK     */
  ITEM_MODIFYCHARM,       /* + CHARM   */
  ITEM_MODIFYAVOID,       /* + AVOID */
  ITEM_MODIFYATTRIB,      /* +          */
  ITEM_MODIFYATTRIBVALUE, /* */
  ITEM_MAGICID,           /*  magic id */
  ITEM_MAGICPROB,         /*  magic */
  ITEM_MAGICUSEMP,        /*  magic MP */
#ifdef _ITEMSET5_TXT
  ITEM_MODIFYARRANGE,
  ITEM_MODIFYSEQUENCE,
  ITEM_ATTACHPILE,
  ITEM_HITRIGHT,
#endif
#ifdef _ITEMSET6_TXT
  ITEM_NEGLECTGUARD,
//	ITEM_BEMERGE,
#endif
  ITEM_POISON,    /* sprit of poison */
  ITEM_PARALYSIS, /* sprit of */
  ITEM_SLEEP,     /* sprit of */
  ITEM_STONE,     /* sprit of stone. */
  ITEM_DRUNK,     /* sprit of drunk. */
  ITEM_CONFUSION, /* sprit of confustion. */
  ITEM_CRITICAL,  /* ? */

  ITEM_USEACTION,    /* �������ݼ�ʧ������� */
  ITEM_DROPATLOGOUT, /* drop this item when LOGOUT. */
  ITEM_VANISHATDROP, /* disappear when drp this item. */
  ITEM_ISOVERED,     /* */
  ITEM_CANPETMAIL,   /* e.g. Abu Water. */
  ITEM_CANMERGEFROM, /* be able to merge from. */
  ITEM_CANMERGETO,   /* ba able to merge to. */

  ITEM_INGVALUE0,    /* ����(5����) */
  ITEM_INGVALUE1,
  ITEM_INGVALUE2,
  ITEM_INGVALUE3,
  ITEM_INGVALUE4,

  ITEM_PUTTIME,   /*  ʧ��  ة��  ��ľ������ */
  ITEM_LEAKLEVEL, /*    ������ľ������ľ�׾�  */
  ITEM_MERGEFLG, /*  ������ľ��ʧ��  ة�������� */
  ITEM_CRUSHLEVEL, /*  çľպ���� 0  2 �߷�çľ��ئ�� 2����ç */

  ITEM_VAR1, /*    ���۽�       */
  ITEM_VAR2, /*    ���۽�       */
  ITEM_VAR3, /*    ���۽�       */
  ITEM_VAR4, /*    ���۽�       */

#ifdef _ITEM_COLOER
  ITEM_COLOER,
#endif

#ifdef _VERSION_GF
  ITEM_UNKNOWN1,
  ITEM_UNKNOWN2,
  ITEM_UNKNOWN3,
  ITEM_UNKNOWN4,
  ITEM_UNKNOWN5,
  ITEM_UNKNOWN6,
  ITEM_UNKNOWN7,
  ITEM_UNKNOWN8,
  ITEM_UNKNOWN9,
#endif
#ifdef _ITEM_USE_TIME
  ITEM_USETIME,
#endif
  ITEM_DATA_ENUM_MAX,
} ITEM_DATA_ENUM;

typedef enum {
  ITEM_NAME,         /* */
  ITEM_SECRETNAME,   /* */
  ITEM_EFFECTSTRING, /* */
  ITEM_ARGUMENT,     /* */
#ifdef _ITEM_INSLAY
  ITEM_TYPECODE,
  ITEM_INLAYCODE,
#endif
  ITEM_CDKEY, /* */
#ifdef _ITEM_FORUSERNAMES
  ITEM_FORUSERNAME,
  ITEM_FORUSERCDKEY,
#endif
// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
  ITEM_UNIQUECODE, /* ��Ʒ���� */
#endif
  ITEM_INGNAME0, /*  ���м�  �(5����) */
  ITEM_INGNAME1,
  ITEM_INGNAME2,
  ITEM_INGNAME3,
  ITEM_INGNAME4,
  ITEM_INITFUNC, /* */
  ITEM_FIRSTFUNCTION = ITEM_INITFUNC,
  ITEM_PREOVERFUNC,  /* CHAR_PREOVERFUNC ë��� */
  ITEM_POSTOVERFUNC, /* CHAR_POSTOVERFUNC ë���*/
  ITEM_WATCHFUNC,    /* CHAR_WATCHFUNC ë��� */
  ITEM_USEFUNC,      /* ¦�ѷ���
                      * int charaindex ƽ�ҷ��̼������͵�
                      * int charitemindex ���м�
                      *              ʧ��  ة    ����
                      *              ë�����׾�
                      */
  ITEM_ATTACHFUNC,   /* ¦�ѷ���
                      * int charaindex ƽ�ҷ��̼������͵�
                      * int itemindex  ʧ��  ة�̼������͵�
                      *      ƽ�ҷ�������  �Ȼ�����ʧ��  ة
                      *      ��ʧ��  ة  ƥ���̼������͵�
                      *      ƥ��ئ���Ǳ�������
                      */
  ITEM_DETACHFUNC,   /* ¦�ѷ���
                      * int charaindex ƽ�ҷ��̼������͵�
                      * int itemindex  ʧ��  ة�̼������͵�
                      *      ƽ�ҷ�������  �Ȼ�����ʧ��  ة
                      *      ��ʧ��  ة  ƥ���̼������͵�
                      *      ƥ��ئ���Ǳ�������
                      */
  ITEM_DROPFUNC,     /*   ����������
                      * ¦�ѷ�
                      *  int charaindex   ������ƽ�ҷ�
                      *  int itemindex ʧ��  ة�̼������͵�
                      */
  ITEM_PICKUPFUNC,   /* ʧ��  ةë��������
                      * ¦�ѷ�
                      *  int charaindex  ������ƽ�ҷ�index
                      *  int itemindex ʧ��  ة�̼������͵�
                      */
#ifdef _Item_ReLifeAct
  ITEM_DIERELIFEFUNC,
#endif
  ITEM_LASTFUNCTION,
  ITEM_DATACHARNUM = ITEM_LASTFUNCTION,
#ifdef _ANGEL_SUMMON
  ITEM_ANGELMISSION = ITEM_INGNAME0,
  ITEM_ANGELINFO = ITEM_INGNAME1,
  ITEM_HEROINFO = ITEM_INGNAME2,
#endif
#ifdef _VERSION_GF
  ITEM_UNKNOWN10,
#endif
} ITEM_DATACHAR;

typedef enum {
  ITEM_WORKOBJINDEX,
  ITEM_WORKCHARAINDEX,
#ifdef _ITEM_ORNAMENTS
  ITEM_CANPICKUP,
#endif
#ifdef _ITEM_TIME_LIMIT
  ITEM_WORKTIMELIMIT,
#endif

#ifdef _ITEM_TALK_CHECK
  ITEM_WORKTALKCHECK,
#endif
#ifdef _PET_AND_ITEM_UP
  ITEM_WORKCDKEY,
#endif

  ITEM_WORKDATAINTNUM,
} ITEM_WORKDATAINT;

typedef struct tagItem {
  int data[ITEM_DATA_ENUM_MAX];
  STRING64 string[ITEM_DATACHARNUM];
  int workint[ITEM_WORKDATAINTNUM];

  void *functable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
#ifdef _ALLBLUES_LUA_1_2
  lua_State *lua[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
  char *luafunctable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
#endif
#ifdef _JZ_NEWSCRIPT_LUA
  STRING32 lua_charfunctable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];

  void *sur_functable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
  STRING32 sur_charfunctable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
#endif
} ITEM_Item;

typedef struct tagITEM_table {
  int use;
  ITEM_Item itm;
  int randomdata[ITEM_DATA_ENUM_MAX];
} ITEM_table;

typedef struct tagITEM_index {
  BOOL use;
  int index;
} ITEM_index;

typedef struct tagITEM_exists {
  BOOL use;
  ITEM_Item itm;
} ITEM_exists;

#define ITEM_CHECKINDEX(index) _ITEM_CHECKINDEX(__FILE__, __LINE__, index)
INLINE BOOL _ITEM_CHECKINDEX(char *file, int line, int index);

BOOL ITEM_initExistItemsArray(int num);
BOOL ITEM_endExistItemsArray(ITEM_table *ITEM_item);
#define ITEM_initExistItemsOne(itm)                                            \
  _ITEM_initExistItemsOne(__FILE__, __LINE__, itm)
int _ITEM_initExistItemsOne(char *file, int line, ITEM_Item *itm);

#define ITEM_endExistItemsOne(index)                                           \
  _ITEM_endExistItemsOne(index, __FILE__, __LINE__)

void _ITEM_endExistItemsOne(int index, char *file, int line);

#define ITEM_getInt(Index, item_data_enum)                                            \
  _ITEM_getInt(__FILE__, __LINE__, Index, item_data_enum)
INLINE int _ITEM_getInt(char *file, int line, int index, ITEM_DATA_ENUM item_data_enum);

#define ITEM_setInt(Index, item_data_enum, data)                                      \
  _ITEM_setInt(__FILE__, __LINE__, Index, item_data_enum, data)
INLINE int _ITEM_setInt(char *file, int line, int index, ITEM_DATA_ENUM item_data_enum,
                        int data);

INLINE char *ITEM_getChar(int index, ITEM_DATACHAR item_data_enum);
INLINE BOOL ITEM_setChar(int index, ITEM_DATACHAR item_data_enum, char *new);

INLINE int ITEM_getWorkInt(int index, ITEM_WORKDATAINT item_data_enum);
INLINE int ITEM_setWorkInt(int index, ITEM_WORKDATAINT item_data_enum, int data);
INLINE int ITEM_getITEM_itemnum(void);
INLINE int ITEM_getITEM_UseItemnum(void);
INLINE BOOL ITEM_getITEM_use(int index);
void ITEM_constructFunctable(int itemindex);
#define ITEM_getFunctionPointer(itemindex, functype)                           \
  _ITEM_getFunctionPointer(itemindex, functype, __FILE__, __LINE__)
void *_ITEM_getFunctionPointer(int itemindex, int functype, char *file,
                               int line);
#ifdef _ALLBLUES_LUA_1_2
typedef struct tagITEM_LuaFunc {
  lua_State *lua;
  char luafuncname[128];
  char luafunctable[128];
  struct tagITEM_LuaFunc *next;
} ITEM_LuaFunc;

INLINE BOOL ITEM_setLUAFunction(int itemindex, int functype,
                                const char *luafuncname);
INLINE lua_State *ITEM_getLUAFunction(int itemindex, int functype);

BOOL ITEM_addLUAListFunction(lua_State *L, const char *luafuncname,
                             const char *luafunctable);
#endif
INLINE ITEM_Item *ITEM_getItemPointer(int index);
int ITEM_getItemMaxIdNum(void);

char *ITEM_makeStringFromItemData(ITEM_Item *one, int mode);
char *ITEM_makeStringFromItemIndex(int index, int mode);

BOOL ITEM_makeExistItemsFromStringToArg(char *src, ITEM_Item *item, int mode);
void ITEM_getDefaultItemSetting(ITEM_Item *itm);

INLINE BOOL ITEM_CHECKITEMTABLE(int number);
BOOL ITEM_readItemConfFile(char *filename);

CHAR_EquipPlace ITEM_getEquipPlace(int charaindex, int itmid);

char *ITEM_makeItemStatusString(int haveitemindex, int itemindex);
char *ITEM_makeItemFalseString(void);
char *ITEM_makeItemFalseStringWithNum(int haveitemindex);

BOOL ITEM_makeItem(ITEM_Item *itm, int number);
int ITEM_makeItemAndRegist(int number);

void ITEM_equipEffect(int index);

void Other_DefcharWorkInt(int index);

char *ITEM_getAppropriateName(int itemindex);
char *ITEM_getEffectString(int itemindex);

int ITEM_getcostFromITEMtabl(int item_id);

#define ITEM_getNameFromNumber(id)                                             \
  _ITEM_getNameFromNumber(__FILE__, __LINE__, id)
INLINE char *_ITEM_getNameFromNumber(char *file, int line, int item_id);

#define ITEM_getArgumentString(id)                                             \
  _ITEM_getArgumentString(__FILE__, __LINE__, id)
INLINE char *_ITEM_getArgumentString(char *file, int line, int item_id);

int ITEM_getlevelFromITEMtabl(int item_id);
int ITEM_getgraNoFromITEMtabl(int item_id);
char *ITEM_getItemInfoFromNumber(int item_id);

int ITEM_getdropatlogoutFromITEMtabl(int item_id);
int ITEM_getvanishatdropFromITEMtabl(int item_id);
int ITEM_getcanpetmailFromITEMtabl(int item_id);
int ITEM_getmergeItemFromFromITEMtabl(int item_id);

#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares(int charaindex, int flg);
#endif

BOOL ITEM_canuseMagic(int itemindex);
// Nuke +1 08/23 : For checking the validity of item target
int ITEM_isTargetValid(int charaindex, int itemindex, int toindex);

int ITEMTBL_getInt(int ItemID, ITEM_DATA_ENUM datatype);
char *ITEMTBL_getChar(int ItemID, ITEM_DATACHAR datatype);

int ITEM_getItemDamageCrusheED(int itemindex);
void ITEM_RsetEquit(int charaindex);
void ITEM_reChangeItemToPile(int itemindex);
void ITEM_reChangeItemName(int itemindex);

#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData(int itemID, ITEM_Item *itm);
#endif

#ifdef _PET_ITEM
char *ITEM_petmakeItemStatusString(int petindex, int item_id);
#endif

#endif
