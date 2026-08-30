/************************/
/*    battleMenu.h        */
/************************/
#ifndef __BATTLE_MENU_H__
#define __BATTLE_MENU_H__

// 战斗自动操作模式。
#define AI_NONE 0   // 关闭自动战斗，由玩家手动操作。
#define AI_ATTACK 1 // 自动执行普通攻击。
#define AI_GUARD 2  // 自动执行防御。
#define AI_SELECT 3 // 使用玩家在 AI 设置窗口中配置的策略。
#define AI_ESCAPE 4 // 自动尝试逃跑（保留模式，当前菜单循环通常只切换到 AI_SELECT）。


// BattleBpFlag：服务器通过 BP 指令下发的本回合战斗条件位。
#define BATTLE_BP_JOIN (1 << 0)             // 中途加入战斗；用于加入者的特殊入场处理。
#define BATTLE_BP_PLAYER_MENU_NON (1 << 1)  // 本回合禁止玩家操作，跳过人物指令菜单并自动回复 N。
#define BATTLE_BP_BOOMERANG (1 << 2)        // 人物使用回力镖攻击，目标选择允许同列多目标。
#define BATTLE_BP_PET_MENU_NON (1 << 3)     // 本回合禁止宠物操作，跳过宠物指令菜单。
#define BATTLE_BP_ENEMY_SURPRISAL (1 << 4)  // 敌方偷袭，显示“遭敌偷袭”并应用偷袭流程。
#define BATTLE_BP_PLAYER_SURPRISAL (1 << 5) // 玩家先制攻击，显示“出其不意的攻击”并应用先制流程。

// 战斗协议采用环形队列缓存，容量必须保持为 2 的幂，以便使用位与运算回绕。
#define BATTLE_BUF_SIZE 4       // 战斗状态/动画命令队列的槽位数。
#define BATTLE_COMMAND_SIZE 4096 // 单条战斗协议命令的最大缓冲区长度。


#ifdef __BATTLE_MENU_CPP__
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int AI;       // 当前自动战斗模式，取值为 AI_*。
EXTERN DWORD PauseAI; // AI 暂停状态：0=运行，1=请求暂停，2=已暂停/等待恢复。
EXTERN char BattleCmd[BATTLE_COMMAND_SIZE]; // 当前正在解析和播放的战斗动画命令。
EXTERN char BattleCmdBak[BATTLE_BUF_SIZE][BATTLE_COMMAND_SIZE]; // 待处理战斗动画命令环形队列。
EXTERN int BattleCmdReadPointer;  // BattleCmdBak 下一条待读取槽位。
EXTERN int BattleCmdWritePointer; // BattleCmdBak 下一条待写入槽位。
EXTERN char BattleStatus[BATTLE_COMMAND_SIZE]; // 当前正在解析的战场单位状态命令。
EXTERN char BattleStatusBak[BATTLE_BUF_SIZE][BATTLE_COMMAND_SIZE]; // 待处理战场状态命令环形队列。
EXTERN int BattleStatusReadPointer;  // BattleStatusBak 下一条待读取槽位。
EXTERN int BattleStatusWritePointer; // BattleStatusBak 下一条待写入槽位。
EXTERN int battleMenuFlag;  // 当前已打开的战斗菜单位集合（人物菜单/宠物菜单）。
EXTERN int battleMenuFlag2; // 新回合菜单初始化请求；BattleMenuProc 消费后清为 FALSE。
EXTERN int BattleMyNo; // 本机人物在 p_party 战斗阵列中的位置编号；20 以上表示观战者。
EXTERN int BattleMyMp; // 服务器 BP 指令下发的本机人物当前 MP，用于回合开始时同步显示。
EXTERN int BattleEscFlag; // 收到服务器 BU 指令后置位，通知战斗流程退出战斗场景。
EXTERN int BattlePetStMenCnt; // 已设置为可参战的宠物数量，限制最多选择四只后备宠物。
EXTERN int battlePetNoBak; // 当前回合使用的宠物栏位快照；-1 表示没有参战宠物，-2 表示尚未初始化。
EXTERN int battlePetNoBak2; // 动画处理使用的宠物栏位快照，避免换宠后错误更新另一只宠物。
EXTERN int BattleItemNo; // 人物在本回合选择使用的道具栏位编号。
EXTERN int BattleBpFlag;   // 当前回合的 BATTLE_BP_* 条件位集合，由服务器 BP 指令更新。
EXTERN int BattleAnimFlag; // 服务器 BA 指令携带的参战者动画完成位；每一位对应一个战斗单位。
EXTERN BOOL BattleTurnReceiveFlag; // 等待首个 BA 回合号同步；同步完成后立即清除。
#ifdef PK_SYSTEM_TIMER_BY_ZHU
EXTERN BOOL BattleCntDownRest; // 服务器要求暂停/保留当前回合倒计时。
EXTERN BOOL SendToServer;      // 本回合倒计时结束指令是否已经发送，防止重复发送。
EXTERN DWORD BattleCntDown;    // 当前回合操作截止时间（TimeGetTime 毫秒值）。
#define BATTLE_CNT_DOWN_TIME 30000 // 每回合允许玩家操作的默认时间，单位为毫秒。
#endif
EXTERN int BattleCliTurnNo; // 客户端当前处理/显示的回合编号。
EXTERN int BattleSvTurnNo;  // 最近一次服务器 BA 指令携带的回合编号。
EXTERN BOOL BattleResultWndFlag; // 战斗结果窗口状态；非零时菜单系统保留/显示结果窗口。
#ifdef _HUNDRED_KILL
EXTERN BOOL BattleHundredFlag; // 百人道场/百人斩战斗标志，控制对应的战斗表现与结算。
#endif
#undef EXTERN

void BattleMenuProc(void);       // 每帧处理人物/宠物战斗菜单、目标选择与回合倒计时。
void ClearBattleButton(void);    // 清除所有战斗按钮的按下/选中状态。
void InitBattleMenu(void);       // 进入战斗时初始化菜单、按钮、宠物快照和结果窗口状态。
void CheckBattleAnimFlag(void);  // 根据服务器动画完成位标记各战斗单位的本回合命令结束状态。
int CheckBattle1P2P(void);       // 统计有效战斗成员，返回当前是否属于多人战斗布局。
void HpMeterDisp(int no);        // 绘制指定 p_party 位置单位的 HP/MP 等战斗状态条。
void BattleNameDisp(void);       // 绘制战场中各单位名称，并刷新相关命中/显示信息。
#ifdef _BATTLESKILL // (祥褫羲) Syu ADD 桵須撮夔賡醱
void BattleSetWazaHitBox(int no, int typeflag); // 按职业/宠物技能目标类型设置可点击的战斗目标区域。
#endif
#ifdef PK_SYSTEM_TIMER_BY_ZHU
void BattleDown(); // 处理服务器强制结束本回合操作，并提交尚未完成的默认指令。
#endif
void battleMenuFix(void); // 切换手动/自动战斗时关闭旧菜单并重置目标选择状态。

#endif // __BATTLE_MENU_H__
