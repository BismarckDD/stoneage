/* 宠物店NPC */
#include "version.h"
//
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "gmsv_server.h"
#include "log.h"
#include "npc_petshop.h"
#include "npcutil.h"
#include "object.h"
#include "pet_skill.h"
#include "readmap.h"
#include <string.h>

// 这个ENUM是干什么的？

enum {
  NPC_PETSHOP_MSG_OVER,
  NPC_PETSHOP_MSG_MAIN,
  NPC_PETSHOP_MSG_REALY,
  NPC_PETSHOP_MSG_THANKS,
  NPC_PETSHOP_MSG_COST,
  NPC_PETSHOP_MSG_POOLTHANKS,
  NPC_PETSHOP_MSG_POOLFULL,
  NPC_PETSHOP_MSG_GETFULL,
};

typedef struct {
  char option[32];
  char defaultmsg[128];
} NPC_PETSHOP_MSG;

NPC_PETSHOP_MSG gShopMsg[] = {{"over_msg", "钱用完罗!没问题吗?"},
                             {"main_msg", "欢迎"},
                             {"realy_msg", "真的可以吗?"},
                             {"thanks_msg", "谢谢!"},
                             {"cost_msg", "钱不够喔!"},
                             {"pooltanks_msg", "谢谢!"},
                             {"poolfull_msg", "超过的无法处理喔"},
                             {"getfull_msg", "你不是有很多宠物吗"}

};

#define PETCOST 10
int NPC_DepotPet_CheckRepeat(int char_index);
int NPC_DepotPet_CheckLevel(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_PetDate(int npc_index, int toindex, int shopMsgIdx, char *token);
void NPC_PetDate2(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_PetDate3(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_PetDel2(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_PetDel3(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_getDrawPet(int npc_index, int toindex, char *token, int *buttontype,
                    int shopMsgIdx);

int NPC_GetCost(int npc_index, int talker, int petindex);
void NPC_PetDel(int npc_index, int toindex, int shopMsgIdx, char *token);
void NPC_MaxGoldOver(int npc_index, int toindex, int shopMsgIdx, char *token);

#ifdef _NPC_DEPOTPET
void NPC_PoolPetShop_DepotPet_Menu(int npc_index, int talkerindex);
void NPC_DepotPet_List(int npc_index, int toindex, char *token, int *buttontype,
                       int shopMsgIdx);
void NPC_DepotPet_Add(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_DepotPet_Get(int npc_index, int talker, int shopMsgIdx, char *token);
void NPC_DepotPet_AskGet(int npc_index, int talker, int shopMsgIdx, char *token);
// void NPC_DepotPet_Pet_printWindow( int npc_index, int talkerindex);
// void NPC_DepotPet_Depot_printWindow( int npc_index, int talkerindex);
// BOOL NPC_DepotPet_InsertPet( int npc_index, int talkerindex, int num);
// BOOL NPC_DepotPet_getPet( int npc_index, int talkerindex, int num);
#endif
void NPC_DepotPet_CheckRepeat_del(int char_index, int petindex);

#define NPC_GETPOOLCOST(talker) (50 + CHAR_getInt(talker, CHAR_LV) * 4)
#define NPC_PETSHOP_POOLCOST_DEFAULT 200

enum {
  CHAR_WORK_NOMALRATE = CHAR_NPCWORKINT1,
  CHAR_WORK_SPECIALRATE = CHAR_NPCWORKINT2,
  CHAR_WORK_POOLCOST = CHAR_NPCWORKINT3,
  CHAR_WORK_POOLFLG = CHAR_NPCWORKINT4,
};

int petshophanlde;

static char buf[1024];
static char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

BOOL NPC_PetShopInit(int npc_index) {

  double rate;
  int intrate;
  CHAR_setInt(npc_index, CHAR_WHICHTYPE, CHAR_TYPESTONESHOP);
  if (NPC_Util_GetArgStr(npc_index, npcarg, sizeof(npcarg)) == NULL) {
    print("GetArgStrErr");
    return FALSE;
  }
  if (NPC_Util_GetStrFromStrWithDelim(npcarg, "nomal_rate", buf, sizeof(buf)) !=
      NULL) {
    rate = atof(buf);
    intrate = (int)(rate * 1000);
    CHAR_setWorkInt(npc_index, CHAR_WORK_NOMALRATE, intrate);
  } else {
    CHAR_setWorkInt(npc_index, CHAR_WORK_NOMALRATE, 1000);
  }

  if (NPC_Util_GetStrFromStrWithDelim(npcarg, "special_rate", buf,
                                      sizeof(buf)) != NULL) {
    rate = atof(buf);
    intrate = (int)(rate * 1000);
    CHAR_setWorkInt(npc_index, CHAR_WORK_SPECIALRATE, intrate);
  } else {
    CHAR_setWorkInt(npc_index, CHAR_WORK_SPECIALRATE, 1200);
  }

  int pool_flag = NPC_Util_GetNumFromStrWithDelim(npcarg, "pool_flg");
  if (pool_flag != 1)
    pool_flag = 0;
  CHAR_setWorkInt(npc_index, CHAR_WORK_POOLFLG, pool_flag);

  petshophanlde = npc_index;
  return TRUE;
}

void NPC_PetShopTalked(int npc_index, int talkerindex, char *szMes, int color) {
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  if (NPC_Util_isFaceToFace(talkerindex, npc_index, 2) == FALSE) {
    if (NPC_Util_CharDistance(talkerindex, npc_index) > 1)
      return;
  }
  CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
  NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
}

void NPC_PetShopLooked(int npc_index, int lookedindex) {
  if (CHAR_getInt(lookedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }
  if (NPC_Util_CharDistance(lookedindex, npc_index) > 2)
    return;
}

void NPC_PetShop_selectWindow(int npc_index, // npc_index
                              int toindex,   // chara_index
                              int page_no,   // interface no.
                              int shopMsgIdx)    // 选项附带上下文
{
  char token[1024];  // 发给客户端的窗口内容字符串缓冲区
  int buttontype = 0, windowtype = 0, windowno = 0;  // 按钮类型/窗口类型/窗口编号
  int fd = getfdFromCharaIndex(toindex);  // 根据玩家索引取得其连接 fd
  if (fd == -1) {  // fd == -1 表示玩家已掉线
    printEx("FD校验失败.\n");  // 打印 FD 校验失败
    return;  // 直接返回，不绘制窗口
  }  // 结束 fd 校验 if
  if (CHAR_getWorkInt(toindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {  // 玩家是否处于交易状态
    CHAR_talkToCli(toindex, -1, "交易中无法使用宠物商店!", CHAR_COLORRED);  // 红字提示交易中无法使用宠物商店
    return;  // 返回，禁止在交易中操作
  }  // 结束交易校验 if
  token[0] = '\0';  // 清空 token 缓冲区
  switch (page_no) {  // 按界面编号分发到对应 case
  case 0:  // case 0：主菜单（寄放/领取/卖掉/使用仓库/离开）
    NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_MAIN, buf);
#ifdef _NPC_DEPOTPET  // 编译开关：是否启用宠物寄养池 _NPC_DEPOTPET
    if (!CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG)) {  // 该 NPC 没有寄养池标志
      sprintf(token,  // 开始拼主菜单 token
              "3\n            ☆宠物商店☆"  // 标题行，首数字 3 = 告诉客户端可点起始行号
              "%s"  // 插入 buf（欢迎语）
              "\n"  // 换行
              "\n            《 卖掉宠物 》"  // 选项：卖掉宠物
              "\n"  // 换行
              "\n"  // 换行
              "\n             《 离开 》",  // 选项：离开
              buf);  // sprintf 的 buf 参数
    } else {  // 有寄养池分支
      sprintf(token,  // 开始拼含寄养池的主菜单 token
              "3\n            ☆宠物商店☆"  // 标题行
              "%s"  // buf 参数
              "\n            《 寄放宠物 》"  // 选项：寄放宠物
              "\n            《 领取宠物 》"  // 选项：领取宠物
              "\n            《 卖掉宠物 》"  // 选项：卖掉宠物
              "\n            《 使用仓库 》"  // 选项：使用仓库
              "\n              《 离开 》",  // 选项：离开
              buf);  // buf 参数
    }  // 结束 if/else
#else  // 未启用 _NPC_DEPOTPET 宏
// 2026.09.01, 第一个数字是告诉客户端: start_line
// 客户端会返回 data_num: i - start_line + 1;
    if (!CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG)) {  // 该 NPC 无寄养池
      sprintf(token,  // 开始拼主菜单 token
              "3\n           ☆宠物商店☆"  // 标题行（无池分支，空格数略有不同）
              "%s"  // buf 参数
              "\n"  // 换行
              "\n           《 卖掉宠物 》"  // 选项：卖掉宠物
              "\n"  // 换行
              "\n             《 离开 》",  // 选项：离开
              buf);  // buf 参数
    } else {  // 有池分支（未开 _NPC_DEPOTPET 时）
      sprintf(token,  // 开始拼主菜单：寄放/领取/卖掉/离开
              "3\n            ☆宠物商店☆"  // 标题行
              "%s"  // buf 参数
              "\n            《 寄放宠物 》"  // 选项：寄放宠物
              "\n            《 领取宠物 》"  // 选项：领取宠物
              "\n            《 卖掉宠物 》"  // 选项：卖掉宠物
              "\n              《 离开 》",  // 选项：离开
              buf);  // buf 参数
    }  // 结束 if/else
#endif  // 结束条件编译
    buttontype = WINDOW_BUTTONTYPE_NONE;  // 主菜单无按钮
    windowtype = WINDOW_MESSAGETYPE_SELECT;  // 窗口类型：选择菜单
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_START;  // 窗口编号：主菜单 START
    break;  // 跳出 switch
  case 1: // 寄放宠物
#ifdef _ITEM_PET_LOCKED  // 编译开关：宠物是否带安全锁 _ITEM_PET_LOCKED
    if (CHAR_getInt(toindex, CHAR_LOCKED) == 1) {  // 玩家宠物处于锁定状态
      char message[256];  // 提示语缓冲
      char buf[256];  // 转义用缓冲
      sprintf(message, "为了确保你的物品安全，请输入你的安全密码进行解锁！\n");  // 构造“请输入安全密码解锁”提示
  // （空行）
      GmsvServer_WN_send(  // 发送一个“带单行输入”的窗口让玩家输入密码
          getfdFromCharaIndex(toindex), WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,  // fd + 窗口类型 MESSAGEANDLINEINPUT
          WINDOW_BUTTONTYPE_OKCANCEL, CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,  // 按钮 OKCANCEL + 锁窗口编号 + -1
          makeEscapeString(message, buf, sizeof(buf)));  // 转义后的提示内容
  // （空行）
      return;  // 先返回，等玩家解锁后再继续寄放流程
    }  // 结束锁 if
#endif  // 结束锁宏
    buttontype = WINDOW_BUTTONTYPE_CANCEL;  // 寄放窗口提供“取消”按钮
    windowtype = WINDOW_MESSAGETYPE_PETSELECT;  // 窗口类型：宠物选择（列出身上宠物）
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT;  // 窗口编号：寄放用的宠物选择 PETSELECT
    break;  // 跳出 switch
  case 2: // 领取宠物
    NPC_PetDate(npc_index, toindex, shopMsgIdx, token);  // 取寄放池中第 shopMsgIdx 只宠物信息写入 token 作确认文案
    buttontype = WINDOW_BUTTONTYPE_YESNO;  // 提供 是/否 按钮
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 普通消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN;  // 窗口编号：领取确认 MAIN
    break;  // 跳出 switch
  case 3: // 卖掉宠物
    if ((CHAR_getInt(toindex, CHAR_GOLD) +  // 玩家当前金币
         CHAR_getWorkInt(toindex, CHAR_WORKSHOPRELEVANTSEC)) >  // 加上本次卖宠将获得的金币
        CHAR_getMaxHaveGold(toindex)) {  // 是否超过持有金币上限
      NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_OVER, token);  // 超上限：提示金币将溢出
      windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER;  // 窗口编号：金币溢出 GOLDOVER
      buttontype = WINDOW_BUTTONTYPE_YESNO;  // 是否继续
    } else {  // 未超上限
      NPC_PetDel(npc_index, toindex, shopMsgIdx, token);  // 执行卖出：扣宠物、加金币，写感谢语到 token
      buttontype = WINDOW_BUTTONTYPE_OK;  // 只用“确定”
      windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 窗口编号：结束 END
    }  // 结束 if/else
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口类型
    break;  // 跳出 switch
  case 4: // 离开
    NPC_PetDel(npc_index, toindex, shopMsgIdx, token);  // 复用卖出结束窗口（shopMsgIdx 实际未用）
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定按钮
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    break;  // 跳出 switch
  case 5: // 领取寄放宠列表
    if (CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG) != 1)  // 该 NPC 不是寄养池，直接返回
      return;  // 返回（不应出现）
    buttontype = WINDOW_BUTTONTYPE_CANCEL;  // 取消按钮
    windowtype = WINDOW_MESSAGETYPE_PETSELECT;  // 宠物选择窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2;  // 窗口编号：领取用的宠物选择2
    break;  // 跳出 switch
  case 6:  // case 6：寄放确认（费用）
    if (CHAR_getInt(toindex, CHAR_GOLD) < NPC_GETPOOLCOST(toindex)) {  // 玩家金币不足以支付寄放费
      NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_COST, token);  // 提示金币不足
      buttontype = WINDOW_BUTTONTYPE_OK;  // 只用“确定”
    } else {  // 金币足够
      NPC_PetDate2(npc_index, toindex, shopMsgIdx, token);  // 取寄放费用信息写入 token 确认文案
      buttontype = WINDOW_BUTTONTYPE_YESNO;  // 是否确认寄放
    }  // 结束 if/else
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2;  // 窗口编号：寄放确认 MAIN2
    break;  // 跳出 switch
  case 7:  // case 7：领取寄放宠（扣费）
    if (CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG) != 1)  // 非寄养池，返回
      return;  // 返回
    if (CHAR_getInt(toindex, CHAR_GOLD) < NPC_GETPOOLCOST(toindex)) {  // 金币不足支付领取费
      NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_COST, token);  // 提示不足
    } else {  // 金币足够
      NPC_PetDel2(npc_index, toindex, shopMsgIdx, token);  // 执行领取：从池移除宠物、扣费，写文案
    }
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    break;  // 跳出 switch
  case 8:  // case 8：寄养池已满
    NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_POOLFULL, token);  // 提示寄养池已满
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    break;  // 跳出 switch
  case 9: // 取出寄放宠列表
    if (CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG) != 1)  // 非寄养池，返回
      return;  // 返回
    NPC_getDrawPet(npc_index, toindex, token, &buttontype, shopMsgIdx);  // 取寄放池宠物列表（含分页）写入 token，并回写 buttontype
    windowtype = WINDOW_MESSAGETYPE_SELECT;  // 选择菜单窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT;  // 窗口编号：取出选择 DRAWSELECT
    break;  // 跳出 switch
  case 10:  // case 10：确认领回
    NPC_PetDate3(npc_index, toindex, shopMsgIdx, token);  // 取“领回第 shopMsgIdx 只”的确认文案
    buttontype = WINDOW_BUTTONTYPE_YESNO;  // 是否确认领回
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW;  // 窗口编号：确认领回 ASKDRAW
    break;  // 跳出 switch
  case 11:  // case 11：实际领回
    if (CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG) != 1)  // 非寄养池，返回
      return;  // 返回
    NPC_PetDel3(npc_index, toindex, shopMsgIdx, token);  // 执行实际领回（从池取出到身上），写文案
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    break;  // 跳出 switch
  case 12:  // case 12：身上宠物栏已满
    NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_GETFULL, token);  // 提示身上宠物栏已满，无法领回
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    break;  // 跳出 switch
#ifdef _NPC_DEPOTPET  // 仓库相关 case 仅在启用 _NPC_DEPOTPET 时编译
  case 13: // 存入仓库列表
    if (!CHAR_CheckDepotPet(toindex))  // 玩家无宠物仓库（未开通），返回
      return;  // 返回
    buttontype = WINDOW_BUTTONTYPE_CANCEL;  // 取消
    windowtype = WINDOW_MESSAGETYPE_PETSELECT;  // 宠物选择
    windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ADD;  // 窗口编号：存入仓库 ADD
    break;  // 跳出 switch
  case 14: // 取出仓库列表
    if (!CHAR_CheckDepotPet(toindex))  // 无宠物仓库，返回
      return;  // 返回
    NPC_DepotPet_List(npc_index, toindex, token, &buttontype, shopMsgIdx);  // 取仓库宠物列表（分页）写入 token 并回写 buttontype
    // buttontype=WINDOW_BUTTONTYPE_CANCEL;
    windowtype = WINDOW_MESSAGETYPE_SELECT;  // 选择菜单窗口
    windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_GET;  // 窗口编号：取出仓库 GET
    break;  // 跳出 switch
  case 15: // 询问确认存入仓库
    if (CHAR_getInt(toindex, CHAR_GOLD) < NPC_GETPOOLCOST(toindex)) {  // 金币不足支付存入费
      NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_COST, token);  // 提示不足
      buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    } else {  // 金币足够
      NPC_PetDate2(npc_index, toindex, shopMsgIdx, token);  // 取存入费用确认文案
      buttontype = WINDOW_BUTTONTYPE_YESNO;  // 是否确认
    }  // 结束 if/else
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKADD;  // 窗口编号：确认存入 ASKADD
    break;  // 跳出 switch
  case 16: // 询问确认取出仓库
    NPC_DepotPet_AskGet(npc_index, toindex, shopMsgIdx, token);  // 取“取出第 shopMsgIdx 只”的确认文案
    buttontype = WINDOW_BUTTONTYPE_YESNO;  // 是否确认取出
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET;  // 窗口编号：确认取出 ASKGET
    break;  // 跳出 switch
  case 17: // 确认存入仓库
    if (!CHAR_CheckDepotPet(toindex))  // 无宠物仓库，返回
      return;  // 返回
    if (CHAR_getInt(toindex, CHAR_GOLD) < NPC_GETPOOLCOST(toindex)) {  // 金币不足
      NPC_MaxGoldOver(npc_index, toindex, NPC_PETSHOP_MSG_COST, token);  // 提示不足
    } else {  // 金币足够
      NPC_DepotPet_Add(npc_index, toindex, shopMsgIdx, token);  // 执行存入：身上宠物移入仓库、扣费，写文案
    }  // 结束 if/else
  // （空行）
    buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
    windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    break;  // 跳出 switch
  // （空行）
  case 18: // 确认取出仓库
    if (CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG) != 1)  // 非寄养池，返回
      return;  // 返回
    if (NPC_DepotPet_CheckLevel(npc_index, toindex, shopMsgIdx, token) == TRUE) {  // 等级满足、可照顾该宠物？
      NPC_DepotPet_Get(npc_index, toindex, shopMsgIdx, token);  // 执行取出：仓库宠物移回身上，写文案
      buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
      windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    } else {  // 等级不足分支
      sprintf(token, "\n你无法照顾这只宠物。");  // 提示无法照顾
      buttontype = WINDOW_BUTTONTYPE_OK;  // 确定
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;  // 消息窗口
      windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;  // 结束窗口
    }  // 结束 if/else
    break;  // 跳出 switch
#endif  // 结束仓库宏
  // CoolFish: Add 2001/7/4
  default:
    break;
  }
  // 将 window_type, button_type, window_no 发给 npc_index
  GmsvServer_WN_send(fd, windowtype, buttontype, windowno,
                     CHAR_getWorkInt(npc_index, CHAR_WORKOBJINDEX), token);
}  // 函数结束

void NPC_PetShopWindowTalked(int npc_index, int talkerindex, int seqno,
                             int shopMsgIdx, char *data) {
  makeStringFromEscaped(data);
  int datanum = atoi(data);
  switch (seqno) {
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_START: {
    int poolflg = CHAR_getWorkInt(npc_index, CHAR_WORK_POOLFLG);
    if ((datanum == 3 && poolflg) ||  // 卖掉宠物
        (datanum == 2 && !poolflg)) { // 领取宠物
      NPC_PetShop_selectWindow(npc_index, talkerindex, 1, -1);
    } else if (datanum == 1 && poolflg) { // 寄放宠物
      if (CHAR_getCharPoolPetElement(talkerindex) != -1) {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 5, -1);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 8, -1);
      }
    } else if (datanum == 2 && poolflg) { // 领取宠物
      if (CHAR_getCharPetElement(talkerindex) != -1) {
        CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
        NPC_PetShop_selectWindow(npc_index, talkerindex, 9, 0);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 12, -1);
      }
    }
#ifdef _NPC_DEPOTPET
    else if (datanum == 4) { // 使用宠物仓库
      // print(" 使用宠物仓库 ");
      CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
      if (!CHAR_CheckDepotPet(talkerindex)) {
        CHAR_GetDepotPet(npc_index, talkerindex);
        CHAR_talkToCli(talkerindex, -1, "取得宠物，请稍後！", CHAR_COLORYELLOW);
      } else {
        NPC_PoolPetShop_DepotPet_Menu(npc_index, talkerindex);
      }
    }
#endif
  } break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_CANCEL) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);

    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 2, datanum);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_CANCEL) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 6, datanum);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 3, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 1, -1);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 7, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 5, -1);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      if (CHAR_getCharPetElement(talkerindex) != -1) {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 11, -1);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 12, -1);
      }
    } else {
      CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
      NPC_PetShop_selectWindow(npc_index, talkerindex, 9, 0);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 4, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    }
    break;
  case CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_PREV) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 9, -1);
    } else if (shopMsgIdx == WINDOW_BUTTONTYPE_NEXT) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 9, 1);
    } else if (shopMsgIdx != WINDOW_BUTTONTYPE_CANCEL) {
      if (CHAR_getCharPetElement(talkerindex) != -1) {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 10, datanum);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 12, -1);
      }
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    }
    break;

#ifdef _NPC_DEPOTPET
  case CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE:
    if (!CHAR_CheckDepotPet(talkerindex))
      return;
    switch (datanum) {
    case 1: // 放入
      if (CHAR_findEmptyDepotPet(talkerindex) == -1) {
        CHAR_talkToCli(talkerindex, -1, "宠物仓库已满！", CHAR_COLORYELLOW);
        return;
      }
      // NPC_DepotPet_Pet_printWindow( npc_index, talkerindex);
      NPC_PetShop_selectWindow(npc_index, talkerindex, 13, -1);
      break;
    case 2: // 取出
      if (CHAR_getCharPetElement(talkerindex) < 0) {
        CHAR_talkToCli(talkerindex, -1, "身上宠物栏位已满！", CHAR_COLORYELLOW);
        return;
      }
      NPC_PetShop_selectWindow(npc_index, talkerindex, 14, 0);
      break;
    case 0:
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
      break;
    }
    break;

  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ADD:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_CANCEL) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 15, datanum);
    }
    break;

  case CHAR_WINDOWTYPE_DEPOTPETSHOP_GET:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_PREV) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 14, -1);
    } else if (shopMsgIdx == WINDOW_BUTTONTYPE_NEXT) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 14, 1);
    } else if (shopMsgIdx != WINDOW_BUTTONTYPE_CANCEL) {
      if (CHAR_getCharPetElement(talkerindex) != -1) {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 16, datanum);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 12, -1);
      }
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    }

    break;
  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKADD: // 确认存入仓库
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 17, -1);
    } else {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 13, -1);
    }
    break;

  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET: // 确认取出仓库
    if (shopMsgIdx == WINDOW_BUTTONTYPE_YES) {
      if (CHAR_getCharPetElement(talkerindex) != -1) {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 18, -1);
      } else {
        NPC_PetShop_selectWindow(npc_index, talkerindex, 12, -1);
      }
    } else {
      CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
      NPC_PetShop_selectWindow(npc_index, talkerindex, 14, 0);
    }
    break;
#endif

  case CHAR_WINDOWTYPE_WINDOWPETSHOP_END:
    if (shopMsgIdx == WINDOW_BUTTONTYPE_OK) {
      NPC_PetShop_selectWindow(npc_index, talkerindex, 0, -1);
    }
    break;

  // CoolFish: Add 2001/7/4
  default:
    break;
  }
}

void NPC_PetDate(int npc_index, int talker, int shopMsgIdx, char *token) {
  int cost;
  char *petname;
  char buf[1024];
  CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, shopMsgIdx);
  int petindex = CHAR_getCharPet(talker, shopMsgIdx - 1);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_REALY, buf);
  cost = NPC_GetCost(npc_index, talker, petindex);
  petname = CHAR_getUseName(petindex);
  sprintf(token,
          "\n是[%s]没错吧？"
          "%s"
          "\n\n    买卖价格         %d STONE\n",
          petname, buf, cost);
}

void NPC_PetDate2(int npc_index, int talker, int shopMsgIdx, char *token) {

  int petindex;
  char *petname;
  CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, shopMsgIdx);

  petindex = CHAR_getCharPet(talker, shopMsgIdx - 1);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  petname = CHAR_getUseName(petindex);
  sprintf(token,
          "\n寄放[%s]"
          "\n\n    寄放价格            %d stone\n",
          petname, NPC_GETPOOLCOST(talker));
}

void NPC_PetDate3(int npc_index, int talker, int shopMsgIdx, char *token) {
  int poolindex;
  int petindex;
  char *petname;
  poolindex = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT) * 5 + shopMsgIdx - 1;
  CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, poolindex);
  petindex = CHAR_getCharPoolPet(talker, poolindex);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  petname = CHAR_getUseName(petindex);
  sprintf(token, "\n领回[%s]", petname);
}

int NPC_GetCost(int npc_index, int talker, int petindex) {
  int cost;
  int levelcost;
  int level;
  int getlevel;
  int graNo;
  int rare;
  int charm = 50;
  int petai;
  int i = 1;
  double rate = 1.0;
  int intrate;
  char buf[1024];
  char buff2[128];
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

  if (NPC_Util_GetArgStr(npc_index, npcarg, sizeof(npcarg)) == NULL) {
    print("GetArgStrErr");
    return FALSE;
  }

  getlevel = CHAR_getInt(petindex, CHAR_PETGETLV);
  level = CHAR_getInt(petindex, CHAR_LV);
  graNo = CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER);
  rare = CHAR_getInt(petindex, CHAR_RARE);

  if (getlevel == 0)
    getlevel = 1;

  /*--袄蟾毛菲户月--*/
  levelcost = (level * level) * 10;
  getlevel = (getlevel * getlevel) * 10;
// Syu ADD 修改卖转生宠金钱会变负的bug
#ifdef _PET_2TRANS
  if (CHAR_getInt(petindex, CHAR_TRANSMIGRATION) > 0)
#else
  if (CHAR_getInt(petindex, CHAR_TRANSMIGRATION) == 1)
#endif
    getlevel = 10;

  if (rare == 0) {
    rare = 1;
  } else if (rare == 1) {
    rare = 5;
  } else if (rare == 2) {
    rare = 8;
  }

  cost = ((levelcost - getlevel) + (level * PETCOST)) * rare;
  intrate = CHAR_getWorkInt(npc_index, CHAR_WORK_NOMALRATE);
  rate = (double)intrate / 1000;

  NPC_Util_GetStrFromStrWithDelim(npcarg, "special_pet", buf, sizeof(buf));

  i = 1;
  while (getStringFromIndexWithDelim(buf, ",", i, buff2, sizeof(buff2)) !=
         FALSE) {
    if (strstr(buff2, "-") == NULL) {
      if (graNo == atoi(buff2)) {
        intrate = CHAR_getWorkInt(npc_index, CHAR_WORK_SPECIALRATE);
        rate = (double)intrate / 1000;
        break;
      }
    } else {
      int start;
      int end;
      int tmp;
      int graflg = 0;
      char token2[128];
      getStringFromIndexWithDelim(buff2, "-", 1, token2, sizeof(token2));
      start = atoi(token2);
      getStringFromIndexWithDelim(buff2, "-", 2, token2, sizeof(token2));
      end = atoi(token2);
      if (start > end) {
        tmp = start;
        start = end;
        end = tmp;
      }
      end++;
      if (start <= graNo && graNo < end) {
        intrate = CHAR_getWorkInt(npc_index, CHAR_WORK_SPECIALRATE);
        rate = (double)intrate / 1000;
        graflg = 1;
        break;
      }

      if (graflg == 1)
        break;
    }
    i++;
  }

  petai = CHAR_getWorkInt(petindex, CHAR_WORKFIXAI);
  charm = CHAR_getWorkInt(talker, CHAR_WORKFIXCHARM);
  charm = charm + petai;
  if (charm < 20) {
    charm = 20;
  }
  charm = charm / 2;
  cost = (int)cost * rate;
  rate = (double)charm / 100;
  cost = cost * rate;
  if (cost > 1000000)
    cost = 1000000;
  CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, cost);
  return cost;
}

#if 1

void NPC_PetDel(int npc_index, int talker, int shopMsgIdx, char *token) {
  int petindex;
  int petsel;
  char szPet[128];
  int cost;
  int defpet;
  int fd = getfdFromCharaIndex(talker);

  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_THANKS, token);
  petsel = (CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT) - 1);
  petindex = CHAR_getCharPet(talker, petsel);

  if (!CHAR_CHECKINDEX(petindex))
    return;

  // CoolFish: Family 2001/6/14
  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
    sprintf(token, "\n\n很抱歉～守护兽无法贩卖喔！\n请自己好好照顾！");
    return;
  }

  // Robin 0725
  if (CHAR_getInt(talker, CHAR_RIDEPET) == petsel) {
    sprintf(token, "\n\n骑乘中的宠物无法贩卖喔！");
    return;
  }

  if (CHAR_getWorkInt(CONNECT_getCharaindex(fd), CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  defpet = CHAR_getInt(talker, CHAR_DEFAULTPET);
  if (defpet == petsel) {
    CHAR_setInt(talker, CHAR_DEFAULTPET, -1);
    GmsvServer_KS_send(fd, -1, TRUE);
  }

#ifdef _PET_ITEM
  CHAR_sendPetItemEmpty(talker, petsel);
#endif

  CHAR_setCharPet(talker, petsel, -1);
  LogPet(CHAR_getChar(talker, CHAR_NAME),
         CHAR_getChar(talker, CHAR_CDKEY), CHAR_getChar(petindex, CHAR_NAME),
         CHAR_getInt(petindex, CHAR_LV), "sell(卖宠)",
         CHAR_getInt(talker, CHAR_FLOOR), CHAR_getInt(talker, CHAR_X),
         CHAR_getInt(talker, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );

  snprintf(szPet, sizeof(szPet), "K%d", petsel);
  CHAR_sendStatusString(talker, szPet);
  cost = NPC_GetCost(npc_index, talker, petindex);
  CHAR_AddGold(talker, cost);
  CHAR_endCharOneArray(petindex);
}
#endif
void NPC_PetDel2(int npc_index, int talker, int shopMsgIdx, char *token) {
  int petindex;
  int selectpet;
  int emptyindex;
  char szPet[128];
  int fd = getfdFromCharaIndex(talker);

  if (fd == -1) {
    printEx("err\n");
    return;
  }
  selectpet = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT) - 1;
  petindex = CHAR_getCharPet(talker, selectpet);

  if (!CHAR_CHECKINDEX(petindex))
    return;
  // Robin 0725
  if (CHAR_getInt(talker, CHAR_RIDEPET) == selectpet) {
    sprintf(token, "\n\n骑乘中的宠物无法寄放喔！");
    return;
  }

  emptyindex = CHAR_getCharPoolPetElement(talker);
  if (emptyindex == -1) {
    printEx("err\n");
    return;
  }

  // 爵  赏  迕矢永玄卅日］公木及瓒  毛壅允［
  if (CHAR_getInt(talker, CHAR_DEFAULTPET) == selectpet) {
    CHAR_setInt(talker, CHAR_DEFAULTPET, -1);
    GmsvServer_KS_send(fd, -1, TRUE);
  }
  CHAR_setCharPoolPet(talker, emptyindex, petindex);
  // 矢永玄  井日壅允
#ifdef _PET_ITEM
  CHAR_sendPetItemEmpty(talker, selectpet);
#endif
  CHAR_setCharPet(talker, selectpet, -1);

  // 丢永本□斥潸
  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
  // 云嗯蛹日允

  CHAR_DelGold(talker, NPC_GETPOOLCOST(talker));

  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  snprintf(szPet, sizeof(szPet), "K%d", selectpet);
  CHAR_sendStatusString(talker, szPet);
  LogPet(CHAR_getChar(talker, CHAR_NAME), /* 平乓仿   */
         CHAR_getChar(talker, CHAR_CDKEY), CHAR_getChar(petindex, CHAR_NAME),
         CHAR_getInt(petindex, CHAR_LV), "pool(寄放宠物店)",
         CHAR_getInt(talker, CHAR_FLOOR), CHAR_getInt(talker, CHAR_X),
         CHAR_getInt(talker, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );

#ifdef _SAMETHING_SAVEPOINT
  if (CHAR_charSaveFromConnect(talker, FALSE)) {
    CHAR_talkToCli(talker, -1, "系统自动为您存档!", CHAR_COLORRED);
  }
#endif
}

void NPC_PetDel3(int npc_index, int talker, int shopMsgIdx, char *token) {
  int emptyindex;
  char szPet[128];
  int workindex[CHAR_MAXPOOLPETHAVE];
  int i;
  int cnt;
  int selectpet = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT);
  int petindex = CHAR_getCharPoolPet(talker, selectpet);

  if (!CHAR_CHECKINDEX(petindex))
    return;
  emptyindex = CHAR_getCharPetElement(talker);
  if (emptyindex == -1) {
    printEx("err\n");
    return;
  }

  // 如果是守护兽改为普通兽
  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1 &&
      CHAR_getInt(talker, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) {
    CHAR_setInt(petindex, CHAR_PETFAMILY, 0);
    CHAR_talkToCli(talker, -1, "守护兽改为普通兽", CHAR_COLORYELLOW);
  }

  CHAR_setCharPet(talker, emptyindex, petindex);
  CHAR_setCharPoolPet(talker, selectpet, -1);
  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
#ifdef _PET_ITEM
  CHAR_sendPetItemData(talker, emptyindex);
#endif
  snprintf(szPet, sizeof(szPet), "K%d", emptyindex);
  CHAR_sendStatusString(talker, szPet);
  snprintf(szPet, sizeof(szPet), "W%d", emptyindex);
  CHAR_sendStatusString(talker, szPet);
  for (i = 0; i < CHAR_MAXPOOLPETHAVE; i++) {
    workindex[i] = -1;
  }
  cnt = 0;
  for (i = 0; i < CHAR_MAXPOOLPETHAVE; i++) {
    int petindex = CHAR_getCharPoolPet(talker, i);
    if (CHAR_CHECKINDEX(petindex)) {
      workindex[cnt++] = petindex;
    }
  }
  for (i = 0; i < CHAR_MAXPOOLPETHAVE; i++) {
    CHAR_setCharPoolPet(talker, i, workindex[i]);
  }
  LogPet(CHAR_getChar(talker, CHAR_NAME), CHAR_getChar(talker, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "draw(领取宠物店)", CHAR_getInt(talker, CHAR_FLOOR),
         CHAR_getInt(talker, CHAR_X), CHAR_getInt(talker, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );
#ifdef _PET_EVOLUTION
  if (CHAR_getInt(petindex, CHAR_FUSIONBEIT) == 1 &&
      CHAR_getInt(petindex, CHAR_FUSIONRAISE) > 0) { // 检查是否为融合宠
    CHAR_setInt(petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL));
  }
#endif
#ifdef _SAMETHING_SAVEPOINT
  if (CHAR_charSaveFromConnect(talker, FALSE)) {
    CHAR_talkToCli(talker, -1, "系统自动为您存档!", CHAR_COLORRED);
  }
#endif
}

void NPC_getDrawPet(int npc_index, int toindex, char *token, int *buttontype,
                    int shopMsgIdx) {
#define LINENUM 5
  char buf[1024];
  int i;
  int petindex = -1;
  int page = CHAR_getWorkInt(toindex, CHAR_WORKSHOPRELEVANT);
  // andy_reEdit 2003/09/18
  int limit = (CHAR_getInt(toindex, CHAR_TRANSMIGRATION) * 2) + 5;
  int endnum = (page + shopMsgIdx + 1) * LINENUM;
  int startnum = endnum - LINENUM;
  *buttontype = WINDOW_BUTTONTYPE_CANCEL;
  if (endnum >= limit) {
    endnum = limit;
    if (page + shopMsgIdx != 0)
      *buttontype |= WINDOW_BUTTONTYPE_PREV;
  } else if (endnum <= LINENUM) {
    endnum = LINENUM;
    *buttontype |= WINDOW_BUTTONTYPE_NEXT;
  } else {
    *buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
  }
  CHAR_setWorkInt(toindex, CHAR_WORKSHOPRELEVANT, page + shopMsgIdx);

  strcpy(token, "2\n    请选择宠物    \n\n");
  for (i = startnum; i < endnum; i++) {
    petindex = CHAR_getCharPoolPet(toindex, i);
    if (CHAR_CHECKINDEX(petindex)) {
      snprintf(buf, sizeof(buf), "LV.%3d MaxHP %3d %s\n",
               CHAR_getInt(petindex, CHAR_LV),
               CHAR_getWorkInt(petindex, CHAR_WORKMAXHP),
               CHAR_getUseName(petindex));
    } else {
      strcpy(buf, "\n");
    }
    strcat(token, buf);
  }
  if (i != limit) {
    petindex = CHAR_getCharPoolPet(toindex, i);
    if (!CHAR_CHECKINDEX(petindex)) {
      *buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
    }
  }
}

/* 2026.09.01 这个是干啥的？ */
void NPC_MaxGoldOver(int npc_index, int toindex, int shopMsgIdx, char *token) {
  // The caller may pass the global buf as token. Never use buf as both
  // snprintf's destination and its %s source: overlapping buffers are UB.
  char message[sizeof(buf) - 2];
  char args[NPC_UTIL_GETARGSTR_BUFSIZE];
  token[0] = '\0';
  if (shopMsgIdx < 0 || shopMsgIdx >= arraysizeof(gShopMsg))
    return;

  if (NPC_Util_GetArgStr(npc_index, args, sizeof(args)) == NULL ||
      NPC_Util_GetStrFromStrWithDelim(args,
        gShopMsg[shopMsgIdx].option, message, sizeof(message)) == NULL) {
    strcpy(message, gShopMsg[shopMsgIdx].defaultmsg);
  }
  snprintf(token, sizeof(buf), "\n\n%s", message);
}

#ifdef _NPC_DEPOTPET
void NPC_PoolPetShop_DepotPet_Menu(int npc_index, int talkerindex) {
  int fd;

  // print(" NPC_PoolPetShop_DepotPet_Menu ");

  fd = getfdFromCharaIndex(talkerindex);
  if (fd != -1) {
    char message[1024];
    char buf[2048];

    strcpy(message, "3\n"
                    "              使用宠物仓库\n"
                    "◆特别注意：请勿将贵重宠物存入公共仓库\n"
                    "　　　　　　『宠物公共仓库』\n"
                    "          ＜＜＜存放宠物＞＞＞\n"
                    "          ＜＜＜取回宠物＞＞＞\n");

    GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE,
                       CHAR_getWorkInt(npc_index, CHAR_WORKOBJINDEX),
                       makeEscapeString(message, buf, sizeof(buf)));
  }
}

void NPC_DepotPet_List(int npc_index, int toindex, char *token, int *buttontype,
                       int shopMsgIdx) {
// ㄠ矢□斥丐凶曰及垫醒
#define LINENUM 5
  char buf[1024];
  int i;
  int startnum, endnum;
  int petindex = -1;
  // 蜇箕及矢□斥
  int page = CHAR_getWorkInt(toindex, CHAR_WORKSHOPRELEVANT);
  int limit;

  limit = CHAR_MAXDEPOTPETHAVE;

  endnum = (page + shopMsgIdx + 1) * LINENUM;
  startnum = endnum - LINENUM;
  *buttontype = WINDOW_BUTTONTYPE_CANCEL;
  // 羹矢□斥譬帮［
  if (endnum >= limit) {
    endnum = limit;
    if (page + shopMsgIdx != 0)
      *buttontype |= WINDOW_BUTTONTYPE_PREV;
  } else if (endnum <= LINENUM) {
    endnum = LINENUM;
    *buttontype |= WINDOW_BUTTONTYPE_NEXT;
  } else {
    *buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
  }
  CHAR_setWorkInt(toindex, CHAR_WORKSHOPRELEVANT, page + shopMsgIdx);

  strcpy(token, "2\n　　　　请选择要从仓库取出的宠物　　　　\n\n");
  for (i = startnum; i < endnum; i++) {
    petindex = CHAR_getDepotPetIndex(toindex, i);
    if (CHAR_CHECKINDEX(petindex)) {
      snprintf(buf, sizeof(buf), "LV.%3d MaxHP %3d %s\n",
               CHAR_getInt(petindex, CHAR_LV),
               CHAR_getWorkInt(petindex, CHAR_WORKMAXHP),
               CHAR_getUseName(petindex));
    } else {
      strcpy(buf, "\n");
    }
    strcat(token, buf);
  }
  // 戚及域蜊毛譬屯化］  井匀凶日戚尺示正件毛潸月
  if (i != limit) {
    petindex = CHAR_getDepotPetIndex(toindex, i);
    if (!CHAR_CHECKINDEX(petindex)) {
      *buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
    }
  }
  // print("\n DepotPet_List:%s ", token);
}

void NPC_DepotPet_Add(int npc_index, int talker, int shopMsgIdx, char *token) {

  int petindex;
  int selectpet;
  int emptyindex;
  char szPet[128];
  int fd = getfdFromCharaIndex(talker);

  if (fd == -1) {
    printEx("err\n");
    return;
  }
  selectpet = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT) - 1;
  // print("\n selectpet:%d ", selectpet);
  petindex = CHAR_getCharPet(talker, selectpet);
  if (!CHAR_CHECKINDEX(petindex))
    return;

  // Robin 0725
  if (CHAR_getInt(talker, CHAR_RIDEPET) == selectpet) {
    sprintf(token, "\n\n骑乘中的宠物无法存入仓库喔！");
    return;
  }

  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1) {
    sprintf(token, "\n\n家族守护兽无法存入仓库喔！");
    return;
  }

#ifdef _LOCK_PET_ITEM
  char *arg = CHAR_getChar(petindex, CHAR_NAME);
  if (arg[0] == '*') {
    sprintf(token, "\n\n绑定的宠物不可存入共同仓库！");
    CHAR_talkToCli(talker, -1, "!", CHAR_COLORYELLOW);
    return;
  }
  if (FreeLockPetSave(talker, petindex) == 1) {
    return;
  }
#endif
  // 坞五毛茧允［蟆卞茧仄化民尼永弁仄化中月及匹］仇仇匹坞中化中卅中及反云井仄中
  emptyindex = CHAR_getCharDepotPetElement(talker);
  if (emptyindex == -1) {
    printEx("err\n");
    return;
  }

  // 爵  赏  迕矢永玄卅日］公木及瓒  毛壅允［
  if (CHAR_getInt(talker, CHAR_DEFAULTPET) == selectpet) {
    CHAR_setInt(talker, CHAR_DEFAULTPET, -1);
    GmsvServer_KS_send(fd, -1, TRUE);
  }
  //   仃  木  卞］矢永玄及奶件犯永弁旦毛戊疋□
  CHAR_setDepotPetIndex(talker, emptyindex, petindex);
  // 矢永玄  井日壅允
  CHAR_setCharPet(talker, selectpet, -1);

  // 丢永本□斥潸
  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
  // 云嗯蛹日允

  CHAR_DelGold(talker, NPC_GETPOOLCOST(talker));

  // 云嗯犯□正霜月
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  snprintf(szPet, sizeof(szPet), "K%d", selectpet);
  // 蕙仄中矢永玄旦  □正旦霜曰勾仃月
  CHAR_sendStatusString(talker, szPet);
  // 矢永玄毛  仃凶夫弘
#ifdef _CHECK_PEPEAT
  if (getCheckRepeat() == 1 ||
      NPC_DepotPet_CheckRepeat(talker) == 0) // 检查编码重复宠
#else
  if (NPC_DepotPet_CheckRepeat(talker) == 0) // 检查编码重复宠
#endif
  {
    LogPet(CHAR_getChar(talker, CHAR_NAME), /* 平乓仿   */
           CHAR_getChar(talker, CHAR_CDKEY), CHAR_getChar(petindex, CHAR_NAME),
           CHAR_getInt(petindex, CHAR_LV), "pool(存入宠物仓库)",
           CHAR_getInt(talker, CHAR_FLOOR), CHAR_getInt(talker, CHAR_X),
           CHAR_getInt(talker, CHAR_Y),
           CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
    );
  } else {
    CHAR_talkToCli(talker, -1, "这是非法的宠物喔!!", CHAR_COLORYELLOW);
  }

#ifdef _SAMETHING_SAVEPOINT
  if (CHAR_charSaveFromConnect(talker, FALSE)) {
    CHAR_talkToCli(talker, -1, "系统自动为您存档!", CHAR_COLORRED);
  }
#endif
}

void NPC_DepotPet_Get(int npc_index, int talker, int shopMsgIdx, char *token) {

  int petindex;
  int selectpet;
  int emptyindex;
  char szPet[128];
  int workindex[CHAR_MAXPOOLPETHAVE];
  int i;
  int cnt;

  selectpet = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT);
  petindex = CHAR_getDepotPetIndex(talker, selectpet);
  if (!CHAR_CHECKINDEX(petindex))
    return;
#ifdef _LOCK_PET_ITEM
  char *arg = CHAR_getChar(petindex, CHAR_NAME);
  if (arg[0] == '*') {
    CHAR_talkToCli(talker, -1, "绑定的宠物不可存入共同仓库!", CHAR_COLORYELLOW);
    return;
  }
  if (FreeLockPetSave(talker, petindex) == 1) {
    return;
  }
#endif
#ifdef _AUTO_DEL_PET
  {
    int j, petid;
    petid = CHAR_getInt(petindex, CHAR_PETID);
    for (j = 0; j < AUTODELITEMNUM; j++) {
      if (petid != -1 && petid == getAutoDelPet(j)) {
        CHAR_setDepotPetIndex(talker, selectpet, -1); // 清除不能当宠物的角色
        LogPet(
            CHAR_getChar(talker, CHAR_NAME), CHAR_getChar(talker, CHAR_CDKEY),
            CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
            "petshop(清除CF设置的宠物)", CHAR_getInt(talker, CHAR_FLOOR),
            CHAR_getInt(talker, CHAR_X), CHAR_getInt(talker, CHAR_Y),
            CHAR_getChar(petindex, CHAR_UNIQUECODE));
        CHAR_talkToCli(talker, -1, "系统清除你仓库的宠物", CHAR_COLORRED);
        return;
      }
    }
  }
#endif

  emptyindex = CHAR_getCharPetElement(talker);
  if (emptyindex == -1) {
    printEx("err\n");
    return;
  }

  CHAR_setCharPet(talker, emptyindex, petindex);
  CHAR_setChar(petindex, CHAR_OWNERCHARANAME, CHAR_getChar(talker, CHAR_NAME));
  CHAR_setDepotPetIndex(talker, selectpet, -1);
  NPC_MaxGoldOver(npc_index, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
  snprintf(szPet, sizeof(szPet), "K%d", emptyindex);
  CHAR_sendStatusString(talker, szPet);
  snprintf(szPet, sizeof(szPet), "W%d", emptyindex);
  CHAR_sendStatusString(talker, szPet);
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    workindex[i] = -1;
  }
  cnt = 0;
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    int petindex = CHAR_getDepotPetIndex(talker, i);
    if (CHAR_CHECKINDEX(petindex)) {
      workindex[cnt++] = petindex;
    }
  }
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    CHAR_setDepotPetIndex(talker, i, workindex[i]);
  }
  LogPet(CHAR_getChar(talker, CHAR_NAME), CHAR_getChar(talker, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "draw(领取仓库宠物)", CHAR_getInt(talker, CHAR_FLOOR),
         CHAR_getInt(talker, CHAR_X), CHAR_getInt(talker, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );
#ifdef _PET_EVOLUTION
  if (CHAR_getInt(petindex, CHAR_FUSIONBEIT) == 1 &&
      CHAR_getInt(petindex, CHAR_FUSIONRAISE) > 0) { // 检查是否为融合宠
    CHAR_setInt(petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL));
  }
#endif
#ifdef _SAMETHING_SAVEPOINT
  if (CHAR_charSaveFromConnect(talker, FALSE)) {
    CHAR_talkToCli(talker, -1, "系统自动为您存档!", CHAR_COLORRED);
  }
#endif
}

void NPC_DepotPet_AskGet(int npc_index, int talker, int shopMsgIdx, char *token) {
  int depotindex;
  int petindex;
  char *petname;
  depotindex = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT) * 5 + shopMsgIdx - 1;
  CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, depotindex);
  petindex = CHAR_getDepotPetIndex(talker, depotindex);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  petname = CHAR_getUseName(petindex);
  sprintf(token, "\n领回[%s]", petname);
}

int NPC_DepotPet_CheckRepeat(int char_index) {
  int petindex1, petindex2;
  int i, j, find, ret = FALSE;
  int workindex[CHAR_MAXDEPOTPETHAVE], cnt = 0;

  for (i = 0; i < (CHAR_MAXDEPOTPETHAVE - 1); i++) {
    petindex1 = CHAR_getDepotPetIndex(char_index, i);
    if (!CHAR_CHECKINDEX(petindex1))
      continue;
    find = 0;
    for (j = (i + 1); j < CHAR_MAXDEPOTPETHAVE; j++) {
      petindex2 = CHAR_getDepotPetIndex(char_index, j);
      if (!CHAR_CHECKINDEX(petindex2))
        continue;
      if (petindex1 == petindex2)
        continue;
      if (!strcmp(CHAR_getChar(petindex1, CHAR_UNIQUECODE),
                  CHAR_getChar(petindex2, CHAR_UNIQUECODE))) {
        print(" 在仓库找到非法宠了 ");
        CHAR_setDepotPetIndex(char_index, j, -1);
        NPC_DepotPet_CheckRepeat_del(char_index, petindex2);
        find++;
        ret = TRUE;
      }
    }
    if (find > 0) {
      CHAR_setDepotPetIndex(char_index, i, -1);
      NPC_DepotPet_CheckRepeat_del(char_index, petindex1);
    }
  }

  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    workindex[i] = -1;
  }
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    int pindex = CHAR_getDepotPetIndex(char_index, i);
    if (CHAR_CHECKINDEX(pindex)) {
      workindex[cnt++] = pindex;
    }
  }
  for (i = 0; i < CHAR_MAXDEPOTPETHAVE; i++) {
    CHAR_setDepotPetIndex(char_index, i, workindex[i]);
  }

  return ret;
}

void NPC_DepotPet_CheckRepeat_del(int char_index, int petindex) {
  LogPet(CHAR_getChar(char_index, CHAR_NAME), /* 平乓仿   */
         CHAR_getChar(char_index, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "del(清除仓库洗宠)", CHAR_getInt(char_index, CHAR_FLOOR),
         CHAR_getInt(char_index, CHAR_X), CHAR_getInt(char_index, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE) // shan 2001/12/14
  );
  CHAR_endCharOneArray(petindex);
}

// 检查等级是否足够
int NPC_DepotPet_CheckLevel(int npc_index, int talker, int shopMsgIdx, char *token) {
  int petindex;
  int selectpet;

  if (!CHAR_CheckDepotPet(talker))
    return FALSE;

  selectpet = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANT);
  petindex = CHAR_getDepotPetIndex(talker, selectpet);
  if (!CHAR_CHECKINDEX(petindex))
    return FALSE;

  // print("\n trans:%d char_lv:%d pet_lv:%d ",
  //	CHAR_getInt( talker, CHAR_TRANSMIGRATION),
  //	CHAR_getInt( talker, CHAR_LV),
  //	CHAR_getInt( petindex, CHAR_LV) );

  // 如果是守护兽改为普通兽
  if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1 &&
      CHAR_getInt(talker, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER) {
    CHAR_setInt(petindex, CHAR_PETFAMILY, 0);
  }

  if (CHAR_getWorkInt(talker, CHAR_PickAllPet) == TRUE) { // 装备驯兽戒指的话...
    return TRUE;
  }

  if (CHAR_getInt(talker, CHAR_TRANSMIGRATION) > 0 ||
      CHAR_getInt(talker, CHAR_LV) + 20 >= CHAR_getInt(petindex, CHAR_LV)) {
    return TRUE;
  }

  return FALSE;
}

#endif
