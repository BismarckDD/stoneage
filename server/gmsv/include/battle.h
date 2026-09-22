#ifndef __BATTLE_H__
#define __BATTLE_H__
#include "net.h"
#ifdef _TRADE_PK
#include "trade.h"
#endif
#ifdef _ALLBLUES_LUA_1_4
#include "lua.h"
#endif
#ifdef _MULTIPLAYER_
#define BATTLE_ENTRY_MAX 12
#define BATTLE_PLAYER_MAX 6
#define SIDE_OFFSET 12
#else
#define BATTLE_ENTRY_MAX 10
#define BATTLE_PLAYER_MAX 5
#define SIDE_OFFSET 10
#endif
#define BATTLE_STRING_MAX 4096
#define BATTLE_TIME_LIMIT (60 * 60)

#define DUELPOINT_RATE (0.1)

enum {
  BATTLE_MODE_NONE = 0,   // 无战斗状态
  BATTLE_MODE_INIT,       // 战斗初始化
  BATTLE_MODE_BATTLE,     // 战斗中
  BATTLE_MODE_FINISH,     // 战斗结束
  BATTLE_MODE_STOP,       // 战斗结束(未使用)
  BATTLE_MODE_WATCHBC,    // 观战初始化
  BATTLE_MODE_WATCHPRE,   // 观战(没作用)
  BATTLE_MODE_WATCHWAIT,  // 观战(没作用)
  BATTLE_MODE_WATCHMOVIE, // 观战(没作用)
  BATTLE_MODE_WATCHAFTER, // 观战(没作用)
  BATTLE_MODE_END
};

enum {
  BATTLE_TYPE_NONE = 0,
  BATTLE_TYPE_P_vs_E = 1,
  BATTLE_TYPE_P_vs_P = 2,
  BATTLE_TYPE_E_vs_E = 3,
  BATTLE_TYPE_WATCH = 4,
  BATTLE_TYPE_DP_BATTLE = 5,
  BATTLE_TYPE_BOSS_BATTLE = 6,
  BATTLE_TYPE_END
};

enum {
  BATTLE_CHARMODE_NONE = 0,  // 无战斗状态
  BATTLE_CHARMODE_INIT,      // 战斗初始化
  BATTLE_CHARMODE_C_WAIT,    // 等待战斗指令
  BATTLE_CHARMODE_C_OK,      // 已输入战斗指令
  BATTLE_CHARMODE_BATTLE,    // 未使用
  BATTLE_CHARMODE_RESCUE,    // 由help状态进入的
  BATTLE_CHARMODE_FINAL,     // 战斗结束
  BATTLE_CHARMODE_WATCHINIT, // 观战初始化
  BATTLE_CHARMODE_COMMAND,   // 未使用
  BATTLE_CHARMODE_END
};

typedef enum {
  BATTLE_ERR_NONE = 0,
  BATTLE_ERR_NOTASK,
  BATTLE_ERR_NOUSE,
  BATTLE_ERR_PARAM,
  BATTLE_ERR_ENTRYMAX,
  BATTLE_ERR_TYPE,
  BATTLE_ERR_CHARAINDEX,
  BATTLE_ERR_BATTLEINDEX,
  BATTLE_ERR_NOENEMY,
  BATTLE_ERR_ALREADYBATTLE,
  BATTLE_ERR_SAMEPARTY,
  BATTLE_ERR_END
} BATTLE_ERR;

enum { BATTLE_S_TYPE_PLAYER = 0, BATTLE_S_TYPE_ENEMY, BATTLE_S_TYPE_END };

typedef enum {
  BATTLE_COM_NONE,
  BATTLE_COM_ATTACK,
  BATTLE_COM_GUARD,
  BATTLE_COM_CAPTURE,
  BATTLE_COM_ESCAPE,
  BATTLE_COM_PETIN,
  BATTLE_COM_PETOUT,
  BATTLE_COM_ITEM,
  BATTLE_COM_BOOMERANG,
  BATTLE_COM_COMBO,
  BATTLE_COM_COMBOEND,
  BATTLE_COM_WAIT,

  BATTLE_COM_SEKIBAN = 1000,
  BATTLE_COM_S_RENZOKU,
  BATTLE_COM_S_GBREAK,
  BATTLE_COM_S_GUARDIAN_ATTACK,
  BATTLE_COM_S_GUARDIAN_GUARD,
  BATTLE_COM_S_CHARGE,
  BATTLE_COM_S_MIGHTY,
  BATTLE_COM_S_POWERBALANCE,
  BATTLE_COM_S_STATUSCHANGE,
  BATTLE_COM_S_EARTHROUND0,
  BATTLE_COM_S_EARTHROUND1,
  BATTLE_COM_S_LOSTESCAPE,
  BATTLE_COM_S_ABDUCT,
  BATTLE_COM_S_STEAL,
  BATTLE_COM_S_NOGUARD,
  BATTLE_COM_S_CHARGE_OK,
  BATTLE_COM_JYUJYUTU = 2000,

  BATTLE_COM_COMPELESCAPE, // 强制离开

#ifdef _ATTACK_MAGIC
  BATTLE_COM_S_ATTACK_MAGIC, // 宠物魔法
#endif

#ifdef _PSKILL_FALLGROUND
  BATTLE_COM_S_FALLRIDE, // 落马术
#endif
#ifdef _PETSKILL_EXPLODE
  BATTLE_COM_S_EXPLODE,
#endif
#ifdef _PETSKILL_TIMID
  BATTLE_COM_S_TIMID,
#endif
#ifdef _PETSKILL_2TIMID
  BATTLE_COM_S_2TIMID,
#endif
#ifdef _PETSKILL_ANTINTER
  BATTLE_COM_S_ANTINTER,
#endif
#ifdef _PETSKILL_PROPERTY
  BATTLE_COM_S_PROPERTYSKILL,
#endif
#ifdef _PETSKILL_TEAR
  BATTLE_COM_S_PETSKILLTEAR,
#endif
#ifdef _BATTLE_LIGHTTAKE
  BATTLE_COM_S_LIGHTTAKE,
#endif
#ifdef _BATTLE_ATTCRAZED // ANDY 疯狂暴走
  BATTLE_COM_S_ATTCRAZED,
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
  BATTLE_COM_S_ATTSHOOT,
#endif
#ifdef _BATTLESTEAL_FIX
  BATTLE_COM_S_STEALMONEY,
#endif
#ifdef _PRO_BATTLEENEMYSKILL
  BATTLE_COM_S_ENEMYRELIFE, // NPC ENEMY 复活技能
  BATTLE_COM_S_ENEMYREHP,   // NPC ENEMY 补血技能
  BATTLE_COM_S_ENEMYHELP,   // NPC ENEMY 招人
#endif
#ifdef _SKILL_DAMAGETOHP
  BATTLE_COM_S_DAMAGETOHP, // 嗜血技
#endif
#ifdef _Skill_MPDAMAGE
  BATTLE_COM_S_MPDAMAGE, // MP伤害
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
  BATTLE_COM_S_WILDVIOLENTATTACK, // 狂暴攻击  vincent add 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
  BATTLE_COM_S_SPEEDYATTACK, // 疾速攻击  vincent add 2002/05/20
#endif
#ifdef _SKILL_GUARDBREAK2
  BATTLE_COM_S_GBREAK2, // 破除防御2 vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE
  BATTLE_COM_S_SACRIFICE, // 救援    vincent add 2002/05/30
#endif
#ifdef _SKILL_WEAKEN
  BATTLE_COM_S_WEAKEN, // 虚弱    vincent add 2002/07/11
#endif
#ifdef _SKILL_DEEPPOISON
  BATTLE_COM_S_DEEPPOISON, // 剧毒    vincent add 2002/07/16
#endif
#ifdef _SKILL_BARRIER
  BATTLE_COM_S_BARRIER, // 魔障    vincent add 2002/07/16
#endif
#ifdef _SKILL_NOCAST
  BATTLE_COM_S_NOCAST, // 沉默    vincent add 2002/07/16
#endif
#ifdef _SKILL_ROAR
  BATTLE_COM_S_ROAR, // 大吼    vincent add 2002/07/11
#endif
#ifdef _BATTLENPC_WARP_PLAYER
  BATTLE_COM_WARP, // npc warp player
#endif
#ifdef _SKILL_TOOTH
  BATTLE_COM_S_TOOTHCRUSHE,
#endif
#ifdef _PSKILL_MODIFY
  BATTLE_COM_S_MODIFYATT,
#endif
#ifdef _PSKILL_MDFYATTACK
  BATTLE_COM_S_MDFYATTACK,
#endif
#ifdef _MAGIC_SUPERWALL
  BATTLE_COM_S_SUPERWALL,
#endif
#ifdef _SKILL_REFRESH
  BATTLE_COM_S_REFRESH,
#endif
#ifdef _VARY_WOLF
  BATTLE_COM_S_VARY,
#endif
#ifdef _PETSKILL_SETDUCK
  BATTLE_COM_S_SETDUCK,
#endif
#ifdef _MAGICPET_SKILL
  BATTLE_COM_S_SETMAGICPET,
#endif
#ifdef _PROFESSION_SKILL         // WON ADD 人物职业技能
  BATTLE_COM_S_VOLCANO_SPRINGS,  // 火山泉
  BATTLE_COM_S_FIRE_BALL,        // 火星球
  BATTLE_COM_S_FIRE_SPEAR,       // 火龙枪
  BATTLE_COM_S_SUMMON_THUNDER,   // 召雷术
  BATTLE_COM_S_CURRENT,          // 电流术
  BATTLE_COM_S_STORM,            // 暴风雨
  BATTLE_COM_S_ICE_ARROW,        // 冰箭术
  BATTLE_COM_S_ICE_CRACK,        // 冰爆术
  BATTLE_COM_S_ICE_MIRROR,       // 冰镜术
  BATTLE_COM_S_DOOM,             // 世界末日
  BATTLE_COM_S_BLOOD,            // 嗜血成性
  BATTLE_COM_S_BLOOD_WORMS,      // 嗜血蛊
  BATTLE_COM_S_SIGN,             // 一针见血
  BATTLE_COM_S_FIRE_ENCLOSE,     // 火附体
  BATTLE_COM_S_ICE_ENCLOSE,      // 冰附体
  BATTLE_COM_S_THUNDER_ENCLOSE,  // 雷附体
  BATTLE_COM_S_ENCLOSE,          // 附身术
  BATTLE_COM_S_TRANSPOSE,        // 移形换位
                                 // 勇士
  BATTLE_COM_S_BRUST,            // 爆击
  BATTLE_COM_S_CHAIN_ATK,        // 连环攻击
  BATTLE_COM_S_AVOID,            // 回避
  BATTLE_COM_S_RECOVERY,         // 补血
  BATTLE_COM_S_WEAPON_FOCUS,     // 武器专精
  BATTLE_COM_S_REBACK,           // 状态回复
  BATTLE_COM_S_CHAIN_ATK_2,      // 双重攻击
  BATTLE_COM_S_SCAPEGOAT,        // 舍已为友
  BATTLE_COM_S_ENRAGE,           // 激化攻击
  BATTLE_COM_S_COLLECT,          // 能量聚集
  BATTLE_COM_S_FOCUS,            // 专注战斗
  BATTLE_COM_S_SHIELD_ATTACK,    // 盾击
  BATTLE_COM_S_DUAL_WEAPON,      // 二刀流
  BATTLE_COM_S_DEFLECT,          // 格档
  BATTLE_COM_S_THROUGH_ATTACK,   // 贯穿攻击
  BATTLE_COM_S_CAVALRY,          // 座骑攻击
  BATTLE_COM_S_DEAD_ATTACK,      // 濒死攻击
  BATTLE_COM_S_CONVOLUTE,        // 回旋攻击
  BATTLE_COM_S_CHAOS,            // 混乱攻击
                                 // 猎人
  BATTLE_COM_S_TRAP,             // 陷阱
  BATTLE_COM_S_TRACK,            // 追寻敌踪
  BATTLE_COM_S_DOCILE,           // 驯伏宠物
  BATTLE_COM_S_ENRAGE_PET,       // 激怒宠物
  BATTLE_COM_S_DRAGNET,          // 天罗地网
  BATTLE_COM_S_ENTWINE,          // 树根缠绕
  BATTLE_COM_S_AUTARKY,          // 自给自足
  BATTLE_COM_S_PLUNDER,          // 体掠夺
  BATTLE_COM_S_TOXIN_WEAPON,     // 毒素武器
  BATTLE_COM_S_RESIST_FIRE,      // 火抗性提升
  BATTLE_COM_S_RESIST_ICE,       // 冰抗性提升
  BATTLE_COM_S_RESIST_THUNDER,   // 雷抗性提升
  BATTLE_COM_S_G_RESIST_FIRE,    // 团体火抗性提升
  BATTLE_COM_S_G_RESIST_ICE,     // 团体冰抗性提升
  BATTLE_COM_S_G_RESIST_THUNDER, // 团体雷抗性提升
  BATTLE_COM_S_ATTACK_WEAK,      // 弱点攻击
  BATTLE_COM_S_INSTIGATE,        // 挑拨
  BATTLE_COM_S_OBLIVION,         // 遗忘
#ifdef _PROFESSION_ADDSKILL
  BATTLE_COM_S_RESIST_F_I_T, // 自然威能
  BATTLE_COM_S_CALL_NATURE,  // 号召自然
  BATTLE_COM_S_BOUNDARY,     // 四属性结界
#endif
#endif

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
  BATTLE_COM_S_SARS,
#endif
#ifdef _SONIC_ATTACK // WON ADD 音波攻击
  BATTLE_COM_S_SONIC,
  BATTLE_COM_S_SONIC2,
#endif
#ifdef _PETSKILL_REGRET
  BATTLE_COM_S_REGRET,
  BATTLE_COM_S_REGRET2,
#endif
#ifdef _PETSKILL_GYRATE
  BATTLE_COM_S_GYRATE,
#endif
#ifdef _PETSKILL_ACUPUNCTURE
  BATTLE_COM_S_ACUPUNCTURE,
#endif
#ifdef _PETSKILL_RETRACE
  BATTLE_COM_S_RETRACE,
#endif
#ifdef _PETSKILL_HECTOR
  BATTLE_COM_S_HECTOR,
#endif
#ifdef _PETSKILL_FIREKILL
  BATTLE_COM_S_FIREKILL,
#endif
#ifdef _PETSKILL_DAMAGETOHP
  BATTLE_COM_S_DAMAGETOHP2, // 暗月狂狼(嗜血技的变体)
#endif
#ifdef _PETSKILL_BECOMEFOX
  BATTLE_COM_S_BECOMEFOX,
#endif
#ifdef _PETSKILL_BECOMEPIG
  BATTLE_COM_S_BECOMEPIG,
#endif
#ifdef _PETSKILL_SHOWMERCY
  BATTLE_COM_S_SHOWMERCY,
#endif
#ifdef _PETSKILL_LER
  BATTLE_COM_S_BAT_FLY,       // 雷尔技 - 群蝠四窜
  BATTLE_COM_S_DIVIDE_ATTACK, // 雷尔技 - 分身地裂
#endif
#ifdef _PETSKILL_BATTLE_MODEL
  BATTLE_COM_S_BATTLE_MODEL, // 宠物技能战斗模组
#endif

#ifdef _MASSAGE_PETSKILL
  BATTLE_COM_S_MASSAGE, // 马杀鸡技能
#endif

#ifdef _STRENGTH_PETSKILL
  BATTLE_COM_S_STRENGTH, // 元气技能
#endif
#ifdef _RESURRECTION_PETSKILL
  BATTLE_COM_S_RESURRECTION,
#endif
#ifdef _LOSTLOST_PETSKILL
  BATTLE_COM_S_LOSTLOST,
#endif
#ifdef _GRAPPLING_PETSKILL
  BATTLE_COM_S_GRAPPLING,
#endif
#ifdef _PETOUT_PETSKILL
  BATTLE_COM_S_PETOUT,
#endif
#ifdef _OFFLINE_SYSTEM
  BATTLE_COM_S_OFFLINE_RECOVERY,
#endif
#ifdef _INVERSION_PETSKILL
  BATTLE_COM_S_INVERSION, // 反转技能
#endif
#ifdef _PETSKILL_NEW_PASSIVE
  BATTLE_COM_S_PASSIVE_PET_MATCH1, // 宠物单人组合被动技能
  BATTLE_COM_S_PASSIVE_PET_MATCH2, // 宠物双人组合被动技能
  BATTLE_COM_S_PASSIVE_PET_MATCH3, // 宠物三人组合被动技能
  BATTLE_COM_S_PASSIVE_PET_MATCH4, // 宠物四人组合被动技能
  BATTLE_COM_S_PASSIVE_PET_MATCH5, // 宠物五人组合被动技能
#endif
  BATTLE_COM_END
} BATTLE_COM;

enum {
  BATTLE_RET_NORMAL,
  BATTLE_RET_CRITICAL,
  BATTLE_RET_MISS,
  BATTLE_RET_DODGE,
  BATTLE_RET_ALLGUARD,
#ifdef _EQUIT_ARRANGE
  BATTLE_RET_ARRANGE,
#endif
  BATTLE_RET_END
} BATTLE_RET;

#define BC_FLG_NEW (1 << 0)
#define BC_FLG_DEAD (1 << 1)
#define BC_FLG_PLAYER (1 << 2)
#define BC_FLG_POISON (1 << 3)
#define BC_FLG_PARALYSIS (1 << 4)
#define BC_FLG_SLEEP (1 << 5)
#define BC_FLG_STONE (1 << 6)
#define BC_FLG_DRUNK (1 << 7)
#define BC_FLG_CONFUSION (1 << 8)
#define BC_FLG_HIDE (1 << 9)
#define BC_FLG_REVERSE (1 << 10)
#ifdef _MAGIC_WEAKEN
#define BC_FLG_WEAKEN (1 << 11) // 虚弱
#endif
#ifdef _MAGIC_DEEPPOISON
#define BC_FLG_DEEPPOISON (1 << 12) // 剧毒
#endif
#ifdef _MAGIC_BARRIER
#define BC_FLG_BARRIER (1 << 13) // 魔障
#endif
#ifdef _MAGIC_NOCAST
#define BC_FLG_NOCAST (1 << 14) // 沉默
#endif

#ifdef _PET_SKILL_SARS        // WON ADD 毒煞蔓延
#define BC_FLG_SARS (1 << 15) // 毒煞
#endif

#ifdef _PROFESSION_SKILL            // WON ADD 人物职业技能
#define BC_FLG_DIZZY (1 << 16)      // 晕眩
#define BC_FLG_ENTWINE (1 << 17)    // 树根缠绕
#define BC_FLG_DRAGNET (1 << 18)    // 天罗地网
#define BC_FLG_ICECRACK (1 << 19)   // 冰爆术
#define BC_FLG_OBLIVION (1 << 20)   // 遗忘
#define BC_FLG_ICEARROW (1 << 21)   // 冰箭
#define BC_FLG_BLOODWORMS (1 << 22) // 嗜血蛊
#define BC_FLG_SIGN (1 << 23)       // 一针见血
#define BC_FLG_CARY (1 << 24)       // 挑拨
#define BC_FLG_F_ENCLOSE (1 << 25)  // 火附体
#define BC_FLG_I_ENCLOSE (1 << 26)  // 冰附体
#define BC_FLG_T_ENCLOSE (1 << 27)  // 雷附体
#ifdef _PROFESSION_ADDSKILL
#define BC_FLG_WATER (1 << 28) // 水附体
#define BC_FLG_FEAR (1 << 29)  // 恐惧
#endif
#ifdef _PETSKILL_LER
#define BC_FLG_CHANGE (1 << 30) // 雷尔变身
#endif
#endif

#define BP_FLG_JOIN (1 << 0)
#define BP_FLG_PLAYER_MENU_OFF (1 << 1)
#define BP_FLG_BOOMERANG (1 << 2)
#define BP_FLG_PET_MENU_OFF (1 << 3)
#define BP_FLG_ENEMY_SURPRISAL (1 << 4)
#define BP_FLG_PLAYER_SURPRISAL (1 << 5)

#define CHAR_BATTLEFLG_ULTIMATE (1 << 0)
#define CHAR_BATTLEFLG_AIBAD (1 << 1)
#define CHAR_BATTLEFLG_REVERSE (1 << 2)
#define CHAR_BATTLEFLG_GUARDIAN (1 << 3)
#define CHAR_BATTLEFLG_NORETURN (1 << 4)
#define CHAR_BATTLEFLG_RECOVERY (1 << 5)
#define CHAR_BATTLEFLG_ABIO (1 << 6)
#define CHAR_BATTLEFLG_NODUCK (1 << 7)

#define GETITEM_MAX 3

typedef struct {
  int attacker;
} INVADER;

typedef struct {
  int char_index;
  int bid;
  int escape;
  int flg;
  int guardian;
  int duelpoint;
  int getitem[GETITEM_MAX];
} BATTLE_ENTRY;

#define BENT_FLG_ULTIMATE (1 << 0)

typedef struct {
  int type;
  int flg;
  int common_dp;
  BATTLE_ENTRY Entry[BATTLE_ENTRY_MAX];
} BATTLE_SIDE;

#define BSIDE_FLG_SURPRISE (1 << 0)
#define BSIDE_FLG_HELP_OK (1 << 1)

#ifdef _ALLBLUES_LUA_1_4
typedef enum {
  BATTLE_FINISH,
  BATTLE_ESCAPE,
  BATTLE_FUNCTABLENUM,
} BATTLE_FUNCTABLE;
#endif

typedef struct _Battle {
  BOOL use;         /* 呼び出しているかどうか  (是否正在调用) */
  int battle_index; /* バトル番号  (战斗编号) */
  int mode;         /* バトルモード  (战斗模式) */
  int type;         /* 战斗类型 (0:PVE)(1:DUEL)(2:PVP) */
  int dpbattle;     /* DPバトルか?  (是否DP战斗) */
  int norisk;       /* 死んでもリスクのないバトルか?  (死亡也无风险的战斗) */
  int turn;         /* ターン数  (回合数) */
  int timer;        /* ウェイト時間に使うタイマー  (等待时间用计时器) */
  int leaderindex;  /* エンカウントを起こしたキャラのインデックス  (触发遭遇的角色索引) */
#ifdef _AUTO_PK
  char leadercdkey[CDKEYLEN];
  char leadername[CHARNAMELEN];
  int rivalindex;
  char rivalcdkey[CDKEYLEN];
  char rivalname[CHARNAMELEN];
#endif

#ifdef _TRADE_PK
  STradeList TradeList[2];
#endif
  int BattleFloor; /* 战场楼层 (CHAR_FLOOR) */
  int winside;     /* 勝ったサイド  (获胜方) */
  int field_att;   /* 战场属性 */
  int att_count;   /* フィールドの属性変化のタイマー  (场地属性变化计时器) */
  int att_pow;     /* フィールドの属性変化のパワー  (场地属性变化强度) */
  int field_no;
  int flg; /* 各種フラグ  (各种标志) */
  BATTLE_SIDE Side[2];
#ifdef _BATTLE_TIMESPEED
  unsigned int CreateTime;
  unsigned int EndTime;
  unsigned int PartTime;
  int flgTime;
#endif

#ifdef _PROFESSION_ADDSKILL
  // 冰爆术存放
  int ice_count;     // 暂存数量
  int ice_bout[20];  // 回合计数
  BOOL ice_use[20];  // 是否使用
  int ice_toNo[20];  // 对象
  int ice_level[20]; // 攻击力
  int ice_array[20];
  int ice_char_index[20];
  int ice_attackNo[20];
#endif

  int iEntryBack[BATTLE_ENTRY_MAX * 2];  // 前ターンに記録されていたナンバー  (上一回合记录的编号)
  int iEntryBack2[BATTLE_ENTRY_MAX * 2]; // 前ターンに記録されていたナンバー  (上一回合记录的编号)
  int createindex; /* この戦いを仕掛けたキャラインデックス(NPCなど)  (发起战斗的角色索引(NPC等)) */
  int (*WinFunc)(int battle_index, int char_index);
#ifdef _ALLBLUES_LUA_1_4
  lua_State *lua[BATTLE_FUNCTABLENUM];
  char *luafunctable[BATTLE_FUNCTABLENUM];
#endif
  struct _Battle *pNext;   // 下一个行动
  struct _Battle *pBefore; // 上一个行动
#ifdef _BATTLE_TIME
  unsigned int tv_sec;
  unsigned int tv_usec;
#endif
#ifdef _JZ_NEWSCRIPT_LUA
  int (*BakFunc)(int battle_index, int char_index);
  int (*EndFunc)(int battle_index);
  char BakLuaFuncName[32];
  char EndLuaFuncName[32];
  int EndLuaIndex;
  int floor;
  int battletime;
#endif
} BATTLE;

#define BATTLE_FLG_FREEDP (1 << 0)    //
#define BATTLE_FLG_CHARALOST (1 << 1) //

enum {
  BATTLE_ATTR_NONE = 0, // 战场属性：无
  BATTLE_ATTR_EARTH,    // 战场属性：地
  BATTLE_ATTR_WATER,    // 战场属性：水
  BATTLE_ATTR_FIRE,     // 战场属性：火
  BATTLE_ATTR_WIND,     // 战场属性：风
  BATTLE_ATTR_END
};

// krynn 2001/12/28
enum {
  BATTLE_SIDE_RIGHT, // 战斗中右侧  right side of battle
  BATTLE_SIDE_LEFT,  // 战斗中左侧  left side of battle
  BATTLE_SIDE_WATCH, // 观战      watch battle player
};
// krynn end

#define TARGET_SIDE_0 20 // 右下
#define TARGET_SIDE_1 21 // 左上
#define TARGET_ALL 22    // 全体

#ifdef _ATTACK_MAGIC

#define TARGET_SIDE_0_B_ROW 26 // 右下後一列
#define TARGET_SIDE_0_F_ROW 25 // 右下前一列
#define TARGET_SIDE_1_F_ROW 24 // 左上前一列
#define TARGET_SIDE_1_B_ROW 23 // 左上後一列

// won add
#define TARGER_THROUGH 27

#endif

#ifdef __BATTLE_C__
#define EXTERN
int gItemCrushRate = 400000;
#else
#define EXTERN extern
extern int gItemCrushRate;                        // 破壊デフォルト  (破坏默认值)
#endif
EXTERN BATTLE *BattleArray;                       /* 戦タスク  (战斗任务)     */
EXTERN int BATTLE_battlenum;                      /* 戦闘の数  (战斗数量) */
EXTERN char szAllBattleString[BATTLE_STRING_MAX]; /* 戦闘に使用する文字列データ  (战斗用字符串数据)   */
EXTERN char *pszBattleTop, *pszBattleLast;        /* 戦闘に使用するデータの先頭  (战斗数据开头)   */
EXTERN char szBadStatusString[1024];              // ステータス異常用データ  (状态异常用数据)
EXTERN int gWeponType;                            // 現在の 武器の種  (当前武器种类)
EXTERN float gDamageDiv;                          // ダメージ倍率  (伤害倍率)

#undef EXTERN

BOOL BATTLE_CHECKINDEX(int battle_index);
#define BATTLE_CHECKSIDE(a) (((a) >= 2 || (a) < 0) ? (FALSE) : (TRUE))
#define BATTLE_CHECKNO(a) (((a) >= 20 || (a) < 0) ? (FALSE) : (TRUE))
#define BATTLE_CHECKADDRESS(a)                                                 \
  ((&BattleArray[0]) <= (a) && (a) <= (&BattleArray[BATTLE_battlenum - 1])     \
       ? (TRUE)                                                                \
       : (FALSE))

#define IsBATTLING(a)                                                          \
  (CHAR_getWorkInt((a), CHAR_WORKBATTLEMODE) ? (TRUE) : (FALSE))

#define STRCPY_TAIL(_pszTop, _pszLast, _szBuffer)                              \
  {                                                                            \
    int _len = strlen(_szBuffer);                                              \
    (_pszTop + _len < (_pszLast) - 1)                                          \
        ? (memcpy(_pszTop, _szBuffer, _len), _pszTop += _len, _pszTop[0] = 0)  \
        : (0);                                                                 \
  }

#define BATTLESTR_ADD(_szBuffer) _BATTLESTR_ADD(_szBuffer, __FILE__, __LINE__)
#define _BATTLESTR_ADD(_szBuffer, file, line)                                  \
  { /*printf("文件=%s 行数=%d 内容=%s\n",file,line,_szBuffer);*/               \
    int _len = strlen(_szBuffer);                                              \
    (pszBattleTop + _len < (pszBattleLast) - 1)                                \
        ? (memcpy(pszBattleTop, _szBuffer, _len), pszBattleTop += _len,        \
           pszBattleTop[0] = 0)                                                \
        : (0);                                                                 \
  }

#define BATTLE_MAP_MAX 219

#define CH_FIX_PLAYERLEVELUP (+2) // プレイヤーのレベルがアップ  (玩家升级)
#define CH_FIX_PLAYERDEAD (-2)    // プレイヤーが通常死  (玩家正常死亡)
#define CH_FIX_PLAYEULTIMATE (-4) // プレイヤーがアルティメット死  (玩家被终极技杀死)
#define CH_FIX_PETESCAPE (-1)     // ペットが逃げた  (宠物逃跑)

#define AI_FIX_PETLEVELUP (+5 * 100)   // ペットがレベルアップ  (宠物升级)
#define AI_FIX_PETWIN (+1)             // ペットが相手を倒した  (宠物击败对手)
#define AI_FIX_PETGOLDWIN (+2 * 10)    // ペットがレベルの高い相手を倒した  (宠物击败高等级对手)
#define AI_FIX_PETRECOVERY (+10)       // 戦闘に回復してもらった  (战斗中被回复)
#define AI_FIX_PETRESSURECT (+3 * 100) // 戦闘に復活してもらった  (战斗中被复活)
// #define AI_FIX_PETRECOVERY  (+50)    // 戦闘に回復してもらった  (战斗中被回复)

#define AI_FIX_SEKKAN (-2 * 100)          // 自分のペットを撃  (击打自己的宠物)
#define AI_FIX_PLAYERULTIMATE (-10 * 100) // 自分のマジックがアルティメット死  (自己的魔法终极技)
#define AI_FIX_PETULTIMATE (-10 * 100)    // ペットがアルティメット死  (宠物被终极技杀死)
#define AI_FIX_PLAYERDEAD (-1 * 100)      // 自分のマジックが気絶  (自己的魔法气绝)
#define AI_FIX_PETDEAD (-5 * 100)         // ペットが気絶  (宠物气绝)

#ifdef _Item_ReLifeAct
int BATTLE_getBattleDieIndex(int battle_index, int bid);
#endif
int BATTLE_AddProfit(int battle_index, int *pBidList);
int BATTLE_No2Index(int battle_index, int No);

int BATTLE_Index2No(int battle_index, int char_index);

BOOL BATTLE_initBattleArray(int battlenum);

int BATTLE_CreateBattle(void);
int BATTLE_DeleteBattle(int battle_index);

int BATTLE_NewEntry(int char_index, int battle_index, int side);

#define BATTLE_Exit(char_index, battle_index)                                   \
  _BATTLE_Exit(__FILE__, __LINE__, char_index, battle_index)
INLINE int _BATTLE_Exit(char *file, int line, int char_index, int battle_index);

#define BATTLE_ExitAll(battle_index)                                            \
  _BATTLE_ExitAll(__FILE__, __LINE__, battle_index)
INLINE void _BATTLE_ExitAll(char *file, int line, int battle_index);

int BATTLE_CreateVsPlayer(int char_index0, int char_index1);

int BATTLE_CreateVsEnemy(int char_index, int mode, int npcindex);
int BATTLE_CreateVsEnemyLvNew(int char_index, int npcindex, int *table,
                              int *lvtable);
int BATTLE_CreateVsEnemyNew(int char_index, int npcindex, int *enemytable);

int BATTLE_CountEntry(int battle_index, int side);

int BATTLE_Loop(void);

int BATTLE_FinishSet(int battle_index);
int BATTLE_StopSet(int battle_index);
int BATTLE_RescueEntry(int char_index, int toindex);

int BATTLE_PetDefaultExit(int char_index, int battle_index);

int BATTLE_PetDefaultEntry(int char_index,  // ペットを持っているプレイヤーの  (持有宠物的玩家的)
                           int battle_index, // バトルインデックス  (战斗索引)
                           int side);

BOOL BATTLE_RescueTry(int char_index);

BOOL BATTLE_RescueParentTry(int char_index, int pindex);

int BATTLE_DefaultAttacker(int battle_index, int side);

BOOL BATTLE_IsThrowWepon(int item_index);

void BATTLE_BadStatusString(int defNo, int status);
int BATTLE_MultiList(int battle_index, int toNo, int ToList[]);
BOOL BATTLE_IsCharge(int com);
BOOL BATTLE_CanMoveCheck(int char_index);
int BATTLE_TargetCheck(int battle_index, int defNo);
char *BATTLE_CharTitle(int char_index);
void BATTLE_EscapeDpSend(int battle_index, int char_index);
int BATTLE_GetDuelPoint(int battle_index, int side, int num);
int BATTLE_TargetCheckDead(int battle_index, int defNo);

void BATTLE_MultiListDead(int battle_index, int toNo, int ToList[]);
BOOL BATTLE_WatchTry(int char_index);
int BATTLE_WatchEntry(int char_index, int toindex);
void BATTLE_WatchStop(int char_index);
int BATTLE_WatchUnLink(int battle_index);
void BATTLE_BpSendToWatch(BATTLE *pBattle, char *pszBcString);

int BATTLE_GetWepon(int char_index);

#ifdef _ITEM_EQUITSPACE
int BATTLE_GetEqShield(int char_index);
#endif

int BATTLE_GetAttackCount(int char_index);
int DoujyouRandomWeponSet(int char_index);
void BATTLE_AttReverse(int char_index);
void BATTLE_BadStatusAllClr(int char_index);
#define CHAR_GETWORKINT_HIGH(index, pos) (CHAR_getWorkInt((index), (pos)) >> 16)
#define CHAR_SETWORKINT_HIGH(index, pos, set)                                  \
  {                                                                            \
    int iTmp = CHAR_getWorkInt((index), (pos)) & 0xFFFF, work = (set);         \
    CHAR_setWorkInt((index), (pos), (work << 16) | iTmp);                      \
  }
#define CHAR_GETWORKINT_LOW(index, pos)                                        \
  (CHAR_getWorkInt((index), (pos)) & 0xFFFF)
#define CHAR_SETWORKINT_LOW(index, pos, set)                                   \
  {                                                                            \
    int iTmp = CHAR_getWorkInt(index, pos) & 0xFFFF0000, work = (set);         \
    CHAR_setWorkInt((index), (pos), (work & 0x0000FFFF) | iTmp);               \
  }

int Battle_getTotalBattleNum();

#ifdef _TYPE_TOXICATION
void CHAR_ComToxicationHp(int char_index);
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
void BATTLE_ProfessionStatus_init(int battle_index, int char_index);
void BATTLE_ProfessionStatusSeq(int battle_index, int char_index);
#endif
#ifdef _TRADE_PK
int BATTLE_CreateVsPlayerForTrade(STradeList TradeList1, STradeList TradeList2);
#endif

#ifdef _ALLBLUES_LUA_1_4
INLINE BOOL BATTLE_setLUAFunction(int battle_index, int functype, lua_State *L,
                                  const char *luafunctable);
INLINE lua_State *BATTLE_getLUAFunction(int battle_index, int functype);
#endif

#ifdef _PETSKILL_NEW_PASSIVE
void BATTLE_PassiveSkill(int char_index);
#endif

#endif

int BATTLE_getBattleFloor(int battle_index);
int BATTLE_getCreateIime(int battle_index);
int BATTLE_getType(int battle_index);
int BATTLE_CreateForWatcher(int char_index, int battle_index);
