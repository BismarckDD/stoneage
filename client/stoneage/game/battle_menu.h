/************************/
/*    battleMenu.h        */
/************************/
#ifndef __BATTLE_MENU_H__
#define __BATTLE_MENU_H__

#define AI_NONE 0
#define AI_ATTACK 1
#define AI_GUARD 2
#define AI_SELECT 3
#define AI_ESCAPE 4


// BattleBpFlag
#define BATTLE_BP_JOIN (1 << 0)             // 
#define BATTLE_BP_PLAYER_MENU_NON (1 << 1)  //
#define BATTLE_BP_BOOMERANG (1 << 2)        //
#define BATTLE_BP_PET_MENU_NON (1 << 3)     // ?????????
#define BATTLE_BP_ENEMY_SURPRISAL (1 << 4)  // ??自?▄?????自???????
#define BATTLE_BP_PLAYER_SURPRISAL (1 << 5) // ??????自?▄?????

//
#define BATTLE_BUF_SIZE 4
#define BATTLE_COMMAND_SIZE 4096


#ifdef __BATTLE_MENU_CPP__
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int AI;
EXTERN DWORD PauseAI;
EXTERN char BattleCmd[BATTLE_COMMAND_SIZE];
EXTERN char BattleCmdBak[BATTLE_BUF_SIZE][BATTLE_COMMAND_SIZE];
EXTERN int BattleCmdReadPointer;
EXTERN int BattleCmdWritePointer;
EXTERN char BattleStatus[BATTLE_COMMAND_SIZE];
EXTERN char BattleStatusBak[BATTLE_BUF_SIZE][BATTLE_COMMAND_SIZE];
EXTERN int BattleStatusReadPointer;
EXTERN int BattleStatusWritePointer;
EXTERN int battleMenuFlag;
EXTERN int battleMenuFlag2;
EXTERN int BattleMyNo;
EXTERN int BattleMyMp;
EXTERN int BattleEscFlag;
EXTERN int BattlePetStMenCnt;
// 战斗宠物是否有后备？有后备才可以换上.
EXTERN int battlePetNoBak;
// 战斗宠物是否有后备？
EXTERN int battlePetNoBak2;
// 战斗中是否有物品
EXTERN int BattleItemNo;
EXTERN int BattleBpFlag;
EXTERN int BattleAnimFlag;
EXTERN BOOL BattleTurnReceiveFlag;
#ifdef PK_SYSTEM_TIMER_BY_ZHU
EXTERN BOOL BattleCntDownRest;
EXTERN BOOL SendToServer;
EXTERN DWORD BattleCntDown;
#define BATTLE_CNT_DOWN_TIME 30000
#endif
EXTERN int BattleCliTurnNo;
EXTERN int BattleSvTurnNo;
EXTERN BOOL BattleResultWndFlag;
#ifdef _HUNDRED_KILL
EXTERN BOOL BattleHundredFlag;
#endif
#undef EXTERN

void BattleMenuProc(void);
void ClearBattleButton(void);
void InitBattleMenu(void);
void CheckBattleAnimFlag(void);
int CheckBattle1P2P(void);
void HpMeterDisp(int no);
void BattleNameDisp(void);
#ifdef _BATTLESKILL // (祥褫羲) Syu ADD 桵須撮夔賡醱
void BattleSetWazaHitBox(int no, int typeflag);
#endif
#ifdef PK_SYSTEM_TIMER_BY_ZHU
void BattleDown();
#endif
void battleMenuFix(void); // 党葩桵須AI蛌遙腔BUG

#endif // __BATTLE_MENU_H__
