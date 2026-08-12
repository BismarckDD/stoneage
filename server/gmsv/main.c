#include "version.h"
//
#include "server_util.h"
//
#include "init.h"
#include "net.h"
#ifdef _EPOLL_ET_MODE
#include "epollnet.h"
#endif
#include "autil.h"
#include "battle.h"
#include "char.h"
#include "config_file.h"
#include "family.h"
#include "handletime.h"
#include "log.h"
#ifdef _SASQL
#include "longzoro/sasql.h"
#endif
#include "map_warppoint.h"
#include "npcgen.h"
#include "petmail.h"
#include "trade.h"
#ifdef _CHATROOMPROTOCOL
#include "chatroom.h"
#endif
#include "shop.h"

static void ShutdownProc(void);
void main_loop(void);
struct tm tmNow, tmOld;
void family_proc();

#ifdef _CHANNEL_MODIFY
extern int InitOccChannel(void);
#endif

#ifdef _GMSV_DEBUG
extern char *DebugMainFunction;
#endif

#ifdef _ANGEL_SUMMON
int AngelReady = 0;
time_t AngelNextTime;
void AngelReadyProc();
#endif

// 启动封包处理线程
void warplog_proc();


WorkSpace gSaacWorkSpace;
WorkSpace gGmsvWorkSpace;

int main(int argc, char **argv, char **env) {
#ifdef _WIN32
  if (sa_platform_init() != 0) {
    fprintf(stderr, "WinSock initialization failed: %d\n", errno);
    return 1;
  }
#endif
  setNewTime();
  ShopData_Init();
  EXIT_WITH_CODE_IF_FALSE(util_Init(), 1);
  LoadAnnounce(); // Arminius 7.12 loginannounce
  memcpy(&tmOld, localtime((time_t *)&NowTime.tv_sec), sizeof(tmNow));
  EXIT_WITH_CODE_IF_FALSE(init(argc, argv, env), 1);
  LoadPetTalk(); // Arminius 8.14 pet talk
#ifdef _GAMBLE_BANK
  Load_GambleBankItems();
#endif

#ifdef _CFREE_petskill
  Load_PetSkillCodes();
#endif

#ifdef _ITEM_PILEFORTRADE
  TRADE_InitTradeList();
#endif

#ifdef _SASQL
  if (sasql_init() == FALSE) {
    print("sasql init failed");
    exit(1);
  }
#endif

  main_loop();
  return 0;
}

void main_loop(void) {
  print("Init NPC......");
  NPC_generateLoop(1);
  print("succeed.\n");
  print("Init signal1...");
#ifndef _WIN32
  signal(SIGUSR1, sigusr1);
#endif
  print("succeed.\n");
  print("Init signal2...");
#ifndef _WIN32
  signal(SIGUSR2, sigusr2);
#endif
  print("succeed.\n");

#ifdef _MAP_WARP_POINT
  print("Init map warp point...");
  MAPPOINT_InitMapWarpPoint();
  print("succeed.\n");
  if (!MAPPOINT_loadMapWarpPoint()) {
    return;
  }
#endif

#ifdef _ASSESS_SYSEFFICACY
  Assess_InitSysEfficacy();
#endif
#ifdef _CHATROOMPROTOCOL
  print("Init chat room...");
  InitChatRoom();
  print("succeed.\n");
#endif
#ifdef _CHAR_PROFESSION
#ifdef _CHANNEL_MODIFY
  print("Init occupation channel...");
  if (!InitOccChannel())
    return;
  print("succeed.\n");
#endif
#endif
#ifdef _ANGEL_SUMMON
  print("Start to init ANGEL SUMMON......");
  AngelReady = 0;
#ifdef _ANGEL_TIME
  AngelNextTime = time(NULL) + getAngelPlayerTime();
#else
  AngelNextTime = time(NULL) + 1 * 60;
#endif
  print("succeed.\n");
#endif
#ifdef _JZ_NEWSCRIPT_LUA
  print("Init NPC lua file......");
  NPC_Lua_Init(getLuaFile());
  print("succeed.\n");
#endif

#ifdef _EPOLL_ET_MODE
  print("Init EPOLL ET MODE...");
  if (Start_PacketWrapper() != 0) {
    print("failed.\n");
    return;
  }
  print("succeed.\n");
  print("Strat epoll loop...");
  if (Start_Epoll_Loop() != 0) {
    print("failed.\n");
    return;
  }
  print("succeed.\n");
#endif

  while (1) {
#ifdef _ASSESS_SYSEFFICACY
    Assess_SysEfficacy(0);
#endif

    setNewTime();
    memcpy(&tmNow, localtime((time_t *)&NowTime.tv_sec), sizeof(tmNow));
    if (tmOld.tm_hour != getLogHour() && tmNow.tm_hour == getLogHour()) {

#ifdef _GMSV_DEBUG
      DebugMainFunction = "backupAllLogFile";
#endif
      backupAllLogFile(&tmOld);
    }

    setNewTime();
#ifdef _ASSESS_SYSEFFICACY_SUB

    Assess_SysEfficacy_sub(0, 1);
#ifdef _GMSV_DEBUG
    DebugMainFunction = "netloop_faster";
#endif

#ifdef _EPOLL_ET_MODE
    System_Loop();
#else
    netloop_faster();
#endif
    Assess_SysEfficacy_sub(1, 1);
    Assess_SysEfficacy_sub(0, 2);
#ifdef _GMSV_DEBUG
    DebugMainFunction = "NPC_generateLoop";
#endif
    NPC_generateLoop(0);
    Assess_SysEfficacy_sub(1, 2);
    Assess_SysEfficacy_sub(0, 3);
#ifdef _GMSV_DEBUG
    DebugMainFunction = "BATTLE_Loop";
#endif
    BATTLE_Loop();
    Assess_SysEfficacy_sub(1, 3);
    Assess_SysEfficacy_sub(0, 4);
#ifdef _GMSV_DEBUG
    DebugMainFunction = "CHAR_Loop";
#endif
    CHAR_Loop();
    Assess_SysEfficacy_sub(1, 4);

#ifdef _GMSV_DEBUG
    DebugMainFunction = "PETMAIL_proc";
#endif
    PETMAIL_proc();

#ifdef _GMSV_DEBUG
    DebugMainFunction = "family_proc";
#endif
    family_proc();

#ifdef _GMSV_DEBUG
    DebugMainFunction = "chardatasavecheck";
#endif
    chardatasavecheck();
#ifdef _ANGEL_SUMMON
#ifdef _GMSV_DEBUG
    DebugMainFunction = "AngelReadyProc";
#endif
    AngelReadyProc();
#endif
    tmOld = tmNow;
    if (tmOld.tm_sec != tmNow.tm_sec) {
#ifdef _GMSV_DEBUG
      DebugMainFunction = "CHAR_checkEffectLoop";
#endif
      CHAR_checkEffectLoop();
    }
    if (SERVSTATE_getShutdown() > 0) {
      ShutdownProc();
#ifdef _GMSV_DEBUG
      DebugMainFunction = "ShutdownProc";
#endif
    }
    tmOld = tmNow;
#ifdef _ASSESS_SYSEFFICACY
    Assess_SysEfficacy(1);
#endif
#endif
  }
}

static void sendmsg_toall(char *msg) {
  int i;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      CHAR_talkToCli(i, -1, msg, CHAR_COLORYELLOW);
    }
  }
}

#define SYSINFO_SHUTDOWN_MSG "服务器即将在%d分钟后关闭, 请各位玩家及时保存游戏信息并登出。"
#define SYSINFO_SHUTDOWN_MSG_COMP "服务器即将关闭."

static void ShutdownProc(void) {
  int diff, hun;
  diff = NowTime.tv_sec - SERVSTATE_getShutdown();
  hun = SERVSTATE_getLimittime() - (diff / 60);
  if (hun != SERVSTATE_getDsptime()) {
    char buff[256];
    if (hun != 0) {
      snprintf(buff, sizeof(buff), SYSINFO_SHUTDOWN_MSG, hun);
    } else {
      strcpy(buff, SYSINFO_SHUTDOWN_MSG_COMP);
    }
    sendmsg_toall(buff);
    SERVSTATE_setDsptime(hun);
    if (hun == 1) {
      SERVSTATE_SetAcceptMore(0);
    }
  }
  /* closesallsockets */
  if (hun == 0) {
    closeAllConnectionandSaveData();
    SERVSTATE_setShutdown(0);
    SERVSTATE_setDsptime(0);
    SERVSTATE_setLimittime(0);
  }
}

void family_proc() {
  static unsigned get_time = 0;
  static unsigned check_time = 0;
  static unsigned proc_time = 0;

  unsigned int current_time = time(NULL);
  if (current_time < proc_time)
    return;
  proc_time = current_time + 5;
  if ((unsigned long)NowTime.tv_sec > get_time) {
    getNewFMList();
    get_time = (unsigned long)NowTime.tv_sec + 60 * 10;
  }

  if ((unsigned long)NowTime.tv_sec > check_time) {
    checkFamilyIndex();
    check_time = (unsigned long)NowTime.tv_sec + 60 * 30;
  }
}

void warplog_proc() {
  static unsigned long check_time = 0;
  if ((unsigned long)NowTime.tv_sec > check_time) {
    warplog_to_file();
    check_time = (unsigned long)NowTime.tv_sec + 300;
  }
}

#ifdef _ANGEL_SUMMON

extern int player_online;

void AngelReadyProc() {
  time_t nowTime;
  struct tm *temptime;
  char msg[1024];
  nowTime = time(NULL);
  if (nowTime < AngelNextTime)
    return;
#ifdef _ANGEL_TIME
  if (player_online <= getAngelPlayerMun())
#else
  if (player_online <= 10)
#endif
  {
    // print("\n当前在线人数=%d\n",	player_online);
    return;
  }
  AngelReady = 1;
  // AngelNextTime = min((int)(10000/player_online), 100)*60 + (unsigned long)nowTime;
#ifdef _ANGEL_TIME
  AngelNextTime = min((int)(getAngelPlayerTime() / player_online), 100) * 60 +
                  (unsigned long)nowTime;
#else
  AngelNextTime =
      min((int)(5000 / player_online), 100) * 60 + (unsigned long)nowTime;
#endif

  temptime = localtime(&AngelNextTime);
  sprintf(msg, "\n下一次出现天使召唤的时间=(%d::%d::%d::%d), 在线人数=%d\n",
          temptime->tm_mon + 1, temptime->tm_mday, temptime->tm_hour,
          temptime->tm_min, player_online);
  print(msg);
}

#endif
