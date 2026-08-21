#include "version.h"
//
#include "util.h"
#include "gmsv_server.h"
#include "saac_client.h"
//
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "char_talk.h"
#include "config_file.h"
#include "encount.h"
#include "enemy.h"
#include "function.h"
#include "handletime.h"
#include "item.h"
#include "item_gen.h"
#include "log.h"
#include "magic_base.h"
#include "msignal.h"
#include "net.h"
#include "npc_quiz.h"
#include "object.h"
#include "pet_skill.h"
#include "petmail.h"
#include "readmap.h"
#include "readnpc.h"
#include "title.h"
#include "autil.h"
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#include "profession_skill.h"
#endif

#ifdef _ITEM_QUITPARTY
#include "init.h"
// int itemquitparty_num = 0;
// static DisappearItem *Disappear_Item;
#endif

#ifdef _EPOLL_ET_MODE
#include "epollnet.h"
#endif

#define MESSAGEFILE "hoge.txt"

#define OPTIONSTRING "d:f:hcl"
#define usage() print("Usage: %s [" OPTIONSTRING "]\n", getProgname());

void printUsage(void) {
  usage();
  print("          [-d debuglevel]        default value is 0\n");
  print("          [-f configfilename]    default value is setup.cf\n");
}

BOOL parseCommandLine(int argc, char **argv) {
  int c;
  extern char *optarg;
  while ((c = getopt(argc, argv, OPTIONSTRING)) != -1) {
    switch (c) {
    case 'd': {
      int debuglevel;
      if (!strtolchecknum(optarg, (int *)&debuglevel, 10, INT)) {
        print("Specify digit number\n");
        return FALSE;
      }
      setDebuglevel(debuglevel);
      break;
    }
    case 'f':
      setConfigfilename(optarg);
      break;
    case 'h':
      printUsage();
      exit(0);
      break;
    case 'c':
#ifdef _CRYPTO_DATA
      if (opendir("allblues") == NULL) {
        if (sa_mkdir("allblues", 0777) == 0) {
          printf("mkdir allblues\n");
        }
      }
      if (opendir("allblues/data") == NULL) {
        if (sa_mkdir("allblues/data", 0777) == 0) {
          printf("mkdir allblues/data\n");
        }
      }
      List("data");
      printf("init data allblues.\n");
      return FALSE;
#endif
      break;
#ifdef _CRYPTO_LUA
    case 'l': {
      int flg = 0, id = 0;
      printf("请问需要加密还是解密？(0为解密, 1为加密):");
      scanf("%d", &flg);
      printf("请问ID是多少:");
      scanf("%d", &id);
      CryptoAllbluesLUA("allblues", flg, id);
      if (flg == 0) {
        printf("已完成解密工作\n");
      } else {
        printf("已完成加密工作\n");
      }
      return FALSE;
    } break;
#endif
    default:
      printUsage();
      return FALSE;
      break;
    }
  }
  return TRUE;
}

BOOL parseEnvironment(char **env) {
  if (getDebuglevel() >= 3) {
    int index = 0;
    while (env[index] != NULL)
      print("%s ", env[index++]);
    print("\n");
  }
  return TRUE;
}

extern int backdoor;

BOOL init(int argc, char **argv, char **env) {
#ifdef _ITEM_QUITPARTY
  FILE *f;
  int i;
  char line[256];
#endif
  srand(
#ifdef _WIN32
      _getpid()
#else
      getpid()
#endif
  );
  print("This Program is compiled at %s %s by gcc %s\n", __DATE__, __TIME__,
        __VERSION__);
  defaultConfig(argv[0]);
  RETURN_FALSE_IF_FALSE(parseCommandLine(argc, argv));
  RETURN_FALSE_IF_FALSE(parseEnvironment(env));

  signalset();

  {
    Char aho;
    debug(sizeof(aho), d);
    debug(sizeof(aho.data), d);
    debug(sizeof(aho.string), d);
    debug(sizeof(aho.flg), d);
    debug(sizeof(aho.indexOfExistItems), d);
    debug(sizeof(aho.haveSkill), d);
    debug(sizeof(aho.indexOfHaveTitle), d);
    debug(sizeof(aho.addressBook), d);
    debug(sizeof(aho.workint), d);
    debug(sizeof(aho.workchar), d);
  }
  print("Current Config File name: %s.\n", getConfigfilename());
  RETURN_FALSE_IF_FALSE(readgServerConfigfile(getConfigfilename()));
#ifdef _WIN32
  sa_set_process_priority(getrunlevel());
#else
  nice(getrunlevel());
#endif
  {
    int iWork = setEncodeKey();
    if (iWork == 0) {
      printf("----------------------------------------\n");
      printf("-------------[EncodeKey] EncodeKey %s\n", getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      printf("Encode Key = %d.\n", iWork);
    }
  }
  {
    int iWork = setAcWBSize();
    if (iWork == 0) {
      printf("----------------------------------------\n");
      printf("-------------[AC缓冲] 无法设置 %s\n",
             getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      printf("AC WB Size = %d.\n", iWork);
    }
  }

  if (getDebuglevel() >= 1) {
    print("Current Debug Level: %d\n", getDebuglevel());
    print("Current Run Level: %d\n", getrunlevel());
    print("Recv Buf Size: %d\n", getrecvbuffer() * 1024);
    print("Send Buf Size: %d\n", getsendbuffer() * 1024);
    print("Recv Lowat Buf Size: %d\n", getrecvlowatbuffer());
    print("Memory Unit size: %d\n", getMemoryunit());
    print("Memory Unit Num: %d\n", getMemoryunitnum());
    print("Account Server Name: %s\n", getAccountservername()); // SAAC -> StoneAge Account
    print("Account Server Port: %d\n", getAccountserverport());
    print("Account Server Password: %s\n", getAccountserverpasswd());
    print("Game Server Name: %s\n", getGameservername()); // GMSV -> Game Server
    print("Game Server Port: %d\n", getPortNumber());
    print("Game Server Num: %d\n", getServernumber());
    print("If reuse address: %d\n", getReuseaddr());
    print("FD number: %d\n", getFdnum());
    print("PetChar Number: %d\n", getPetcharnum());
    print("OtherChar Number: %d\n", getOtherscharnum());
    print("Object Number: %d\n", getObjnum());
    print("Item Number: %d\n", getItemnum());
    print("Battle Num: %d\n", getBattlenum());
    print("TopDir: %s\n", getTopdir());
    print("MapDir: %s\n", getMapdir());
    print("MapTilePath: %s\n", getMaptilefile());
    print("ItemFilePath: %s\n", getItemfile());
    print("InvinciblePlaceDefinitionFilePath: %s\n", getInvfile());
    print("AppearFilePath: %s\n", getAppearfile());
    print("EffectFilePath: %s\n", getEffectfile());
    print("TitleNameFilePath: %s\n", getTitleNamefile());
    print("TitleConfigFilePath: %s\n", getTitleConfigfile());
    print("EncountFilePath: %s\n", getEncountfile());
    print("Group(EnemyGroup)FilePath: %s\n", getGroupfile());
    print("EnemyBaseFilePath: %s\n", getEnemyBasefile());
    print("EnemyConfigFilePath: %s\n", getEnemyfile());
    print("MagicFilePath: %s\n", getMagicfile());
#ifdef _ATTACK_MAGIC
    print("AttackMagicFilepath: %s\n", getAttMagicfileName());
#endif
    print("PetSkillFilePath: %s\n", getPetskillfile());
#ifdef _PROFESSION_SKILL
    print("ProfessionSkillFilePath: %s\n", getProfession());
#endif
    print("ItemAtomFilePath: %s\n", getItematomfile());
    print("QuizFilePath: %s\n", getQuizfile());
#ifdef _GMRELOAD
    print("GM SetFile: %s\n", getGMSetfile());
#endif
    print("LsGenLogFile: %s\n", getLsgenlogfilename());
    print("Store Dir: %s\n", getStoredir());
    print("NPC Dir: %s\n", getNpcdir());
    print("Log Dir: %s\n", getLogdir());
    print("LogConfigFilePath: %s\n", getLogconffile());
    print("GM Password: %s\n", getChatMagicPasswd());
    print("GM CDKEY Check: %d\n", getChatMagicCDKeyCheck());
    print("NPC Template Num: %d\n", getNpctemplatenum());
    print("NPC Create Num: %d\n", getNpccreatenum());
    print("Walk Send Interval: %d\n", getWalksendinterval());
    print("CA Send Interval: %d\n", getCAsendinterval_ms());
    print("CD Send Interval: %d\n", getCDsendinterval_ms());
    print("ִOne Loop Time: %d\n", getOnelooptime_ms());
    print("宠物清除时间: %d\n", getPetdeletetime());
    print("道具清除时间: %d\n", getItemdeletetime());
#ifdef _DEL_DROP_GOLD
    print("石器清除时间: %d\n", getGolddeletetime());
#endif
    print("数据保存间隔: %d\n", getCharSavesendinterval());
    print("名片最大数目: %d\n", getAddressbookoffmsgnum());
    print("读取频率协议: %d\n", getProtocolreadfrequency());
    print("连接错误上限: %d\n", getAllowerrornum());
#ifdef _GET_BATTLE_EXP
    print("BattleEXP?: %d\n", getBattleexp());
#endif
#ifdef _NEW_PLAYER_CF
    print("NewPlayerTrans: %d\n", getNewplayertrans());
    print("NewPlayerLevel: %d\n", getNewplayerlv());
    print("NewPlayerGold: %d(stone).\n", getNewplayergivegold());
    print("NewPlayerPetLevel: %d\n", getNewplayerpetlv());
#ifdef _VIP_SERVER
    print("IF new player give VIP?: %d.\n", getNewplayergivevip());
#endif
    print("RidePetLevel: %d\n", getRidePetLevel());
#ifdef _NEW_PLAYER_RIDE
    print("NewPlayerRide: %s\n", getPlayerRide());
#endif
    print("NewPlayerGivePet: NO1:%d NO2:%d NO3:%d NO4:%d NO5:%d\n",
          getNewplayergivepet(0), getNewplayergivepet(1),
          getNewplayergivepet(2), getNewplayergivepet(3),
          getNewplayergivepet(4));
    print("NewPlayerGiveItem(1): ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
          "NewPlayerGiveItem(2): ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
          "NewPlayerGiveItem(3): ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n",
          getNewplayergiveitem(0), getNewplayergiveitem(1),
          getNewplayergiveitem(2), getNewplayergiveitem(3),
          getNewplayergiveitem(4), getNewplayergiveitem(5),
          getNewplayergiveitem(6), getNewplayergiveitem(7),
          getNewplayergiveitem(8), getNewplayergiveitem(9),
          getNewplayergiveitem(10), getNewplayergiveitem(11),
          getNewplayergiveitem(12), getNewplayergiveitem(13),
          getNewplayergiveitem(14));
#endif
#ifdef _UNREG_NEMA
    print("禁止人物名称: 名字1:%s 名字2:%s 名字3:%s 名字4:%s 名字5:%s\n",
          getUnregname(0), getUnregname(1), getUnregname(2), getUnregname(3),
          getUnregname(4));
#endif

#ifdef _WATCH_FLOOR
    print("Watch Floor CF: %s\n", getWatchFloorCF());
    if (strcmp(getWatchFloorCF(), "0"))
      print("WatchFloor: No.1:%d No.2:%d No.3:%d No.4:%d No.5:%d\n",
            getWatchFloor(1), getWatchFloor(2), getWatchFloor(3),
            getWatchFloor(4), getWatchFloor(5));
#endif
#ifdef _BATTLE_FLOOR
    print("BattleFloorCF: %s\n", getBattleFloorCF());
    if (strcmp(getBattleFloorCF(), "0"))
      print("BattleFloor: No.1:%d No.2:%d No.3:%d No.4:%d No.5:%d\n",
            getBattleFloor(1), getBattleFloor(2), getBattleFloor(3),
            getBattleFloor(4), getBattleFloor(5));
#endif
#ifdef _TRANS_LEVEL_CF
    print("Char Trans Num: %d\n", getChartrans());
    print("Pet Trans Num: %d\n", getPettrans());
#endif
#ifdef _POINT
    print("禁止点数上限: %s\n", getPoint());
    if (strcmp(getPoint(), "是"))
      print("每转点数上限: 0转:%d 1转:%d 2转:%d 3转:%d 4转:%d 5转:%d 6转:%d\n",
            getTransPoint(0), getTransPoint(1), getTransPoint(2),
            getTransPoint(3), getTransPoint(4), getTransPoint(5),
            getTransPoint(6));
#endif
#ifdef _PET_AND_ITEM_UP
    print("宠物能否捡获: %s\n", getPetup());
    print("道具能否捡获: %s\n", getItemup());
#endif
#ifdef _LOOP_ANNOUNCE
    print("循环公告的文件路径: %s.\n", getLoopAnnouncePath());
    print("循环公告的间隔时间: %d.\n", getLoopAnnounceTime());
#endif
#ifdef _SKILLUPPOINT_CF
    print("每级升级点数: %d\n", getSkup());
#endif

#ifdef _RIDELEVEL
    print("RIDE LEVEL: %d\n", getRideLevel());
#endif
#ifdef _REVLEVEL
    print("REV LEVEL: %s\n", getRevLevel());
#endif
#ifdef _TRANS_LEVEL_CF
    print("Player YB Level: %d\n", getYBLevel());
    print("Player Max Level: %d\n", getMaxLevel());
#endif
#ifdef _FIX_CHARLOOPS
    print("Char Loops: %d\n", getCharloops());
#endif
#ifdef _PLAYER_ANNOUNCE
    if (getPAnnounce() == -1)
      print("Player Announce: Disable\n");
    else
      print("Player Announce: %d\n", getPAnnounce());
#endif
#ifdef _PLAYER_MOVE
    if (getPMove() == -1)
      print("Player Move: Disable\n");
    else
      print("Player Move: %d\n", getPMove());
#endif
#ifdef _BATTLE_GOLD
    print("Battle Gold: %d\n", getBattleGold());
#endif
#ifdef _ANGEL_TIME
    print("Angel Player Time: (%d)\n", getAngelPlayerTime());
    print("Angel Player Mun: %d.\n", getAngelPlayerMun());
#endif
#ifdef _RIDEMODE_20
    print("2.0 RIDE MODE: %d\n", getRideMode());
#endif
#ifdef _FM_POINT_PK
    print("Family Point PK: %s\n", getFmPointPK());
#endif
  }
  { // andy_add 2003/05/05 check GameServer Name
    char *GameServerName = getGameserverID();
    if (GameServerName == NULL || strlen(GameServerName) <= 0) {
      return FALSE;
    }
    print("\nGameServerName: %s\n", GameServerName);
  }
  print("开始初始化内存设置:");
  if (!configmem(getMemoryunit(), getMemoryunitnum())) {
    print("\n内存配置无效：usememoryunit=%u, usememoryunitnum=%u.\n",
          getMemoryunit(), getMemoryunitnum());
    return FALSE;
  }
  if (!memInit()) {
    print("\n内存池初始化失败。请检查可用内存及 usememoryunit 配置。\n");
    return FALSE;
  }
  print("内存初始化完毕.\n");

  print("开始初始化网络设置:");
  if (!initConnect(getFdnum())) {
    print("网络配置初始化失败.\n");
    goto MEMEND;
  }
  while (1) {
    print("开始监听端口: %d......", getPortNumber());
#ifdef _EPOLL_ET_MODE
    bindedfd = epoll_bind(getPortNumber());
#else
    bindedfd = bindLocalhost(getPortNumber());
#endif
    if (bindedfd == -1)
#ifdef _WIN32
      sa_sleep(10);
#else
      sleep(10);
#endif
    else
      break;
  }
  print("成功.\n");
  print("开始初始化地图可交互对象(包括玩家、NPC、宠物、物品)列表:");
  if (!initObjectArray(getObjnum()))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化角色列表(服务器所有的玩家、NPC、敌人):");
  if (!CHAR_initCharArray(getFdnum(), getPetcharnum(), getOtherscharnum()))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化物品列表(服务器可以同时存在的所有物品):");
  if (!ITEM_readItemConfFile(getItemfile())) {
    print("物品配置文件读取失败，服务退出.\n");
    goto CLOSEBIND;
  }
  // 整个服务器所有的物品
  if (!ITEM_initExistItemsArray(getItemnum())) {
    print("初始化物品列表失败，服务退出.\n");
    goto CLOSEBIND;
  }
  print("成功.\n");
  print("开始初始化战斗列表(服务器可以同时容纳的所有战斗):");
  if (!BATTLE_initBattleArray(getBattlenum())) {
    print("初始化物品列表失败，服务退出.\n");
    goto CLOSEBIND;
  }
  print("成功.\n");
  print("Start to init function table......");
  if (!initFunctionTable())
    goto CLOSEBIND;
  print("成功.\n");
  print("Start to init address book related......");
  if (!PETMAIL_initOffmsgBuffer(getAddressbookoffmsgnum()))
    goto CLOSEBIND;
  print("成功.\n");
  print("Start to init invincible place......");
  if (!CHAR_initInvinciblePlace(getInvfile()))
    goto CLOSEBIND;
  print("成功.\n");
  print("Start to init appear position......");
  if (!CHAR_initAppearPosition(getAppearfile()))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化头衔名称......");
  if (!TITLE_initTitleName(getTitleNamefile()))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化头衔配置......");
  if (!TITLE_initTitleConfig(getTitleConfigfile()))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化遭遇敌人坐标配置......");
  if (!ENCOUNT_initEncount(getEncountfile()))
    goto CLOSEBIND;
  print("成功.\n");
  const char *enemyBaseFile = getEnemyBasefile();
  print("开始初始化宠物模板(一个宠物模板可能生成多个宠物实例):%s,", enemyBaseFile);
  if (!ENEMYTEMP_initEnemy(enemyBaseFile))
    goto CLOSEBIND;
  print("成功.\n");
  const char *enemyFile = getEnemyfile();
  print("开始初始化宠物实例(一个宠物模板可能生成多个宠物实例):%s,", enemyFile);
  if (!ENEMY_initEnemy(enemyFile))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化敌人群组......");
  if (!GROUP_initGroup(getGroupfile()))
    goto CLOSEBIND;
  print("成功.\n");
  const char *magicFile = getMagicfile();
  print("开始初始化魔法配置(精灵):%s,", magicFile);
  if (!MAGIC_initMagic(magicFile))
    goto CLOSEBIND;
  print("成功.\n");
#ifdef _ATTACK_MAGIC
  const char *attMagicFile = getAttMagicfileName();
  print("开始初始化魔法配置(攻击性魔法):%s,", attMagicFile);
  if (!ATTMAGIC_initMagic(attMagicFile))
    goto CLOSEBIND;
  print("成功.\n");
#endif
  print("开始初始化宠物技能......");
  if (!PETSKILL_initPetskill(getPetskillfile()))
    goto CLOSEBIND;
  print("成功.\n");
#ifdef _PROFESSION_SKILL // WON ADD
  print("Start to init profession skill.....");
  if (!PROFESSION_initSkill(getProfession()))
    goto CLOSEBIND;
  print("succeed.\n");
#endif
  const char *itemAtomFile = getItematomfile();
  print("开始初始化合成材料:%s,", itemAtomFile);
  if (!ITEM_initItemAtom(itemAtomFile))
    goto CLOSEBIND;
  print("成功.\n");
  print("开始初始化合成配方,");
  if (!ITEM_initItemIngCache())
    goto CLOSEBIND;
  print("成功.\n");
  print("Start to init rand table.");
  if (!ITEM_initRandTable())
    goto CLOSEBIND;
  print("succeed.\n");
  print("开始初始化特效......");
  if (!CHAR_initEffectSetting(getEffectfile()))
    goto CLOSEBIND;
  print("成功.\n");
  print("这是啥......");
  if (!QUIZ_initQuiz(getQuizfile()))
    goto CLOSEBIND;
  print("成功.\n");
#ifdef _GMRELOAD
  print("开始加载GM设置......");
  if (!LoadGMSet(getGMSetfile()))
    goto CLOSEBIND;
  print("成功.\n");
#endif

#ifdef _USER_EXP_CF
  print("Start to init user exp cf......");
  if (!LoadEXP(getEXPfile()))
    goto CLOSEBIND;
  print("User Max Level: %d......", getMaxLevel());
  print("User YB Level: %d......", getYBLevel());
  print("succeed.\n");
#endif

#ifdef _ANGEL_SUMMON
  print("Start to init angel summon expansion......");
  if (!LoadMissionList())
    goto CLOSEBIND;
  print("succeed.\n");
#endif

#ifdef _JOBDAILY
  print("Start to init job daily......");
  if (!LoadJobdailyfile())
    print("......failed.\n");
  else
    print("succeed.\n");
#endif

#ifdef _LOOP_ANNOUNCE
  print("Start to load init loop announce......");
  if (!loadLoopAnnounce())
    print("......failed.\n");
  else
    print("succeed.\n");
#endif
#ifdef _RIDE_CF
  print("Start to init Ride CF......");
  if (!CHAR_Ride_CF_init())
    print("......failed.\n");
  else
    print("succeed.\n");
#endif
#ifdef _FM_LEADER_RIDE
  print("Start to init FM Leader Ride......");
  if (!CHAR_FmLeaderRide_init())
    print("......failed.\n");
  print("succeed.\n");
#endif
#ifdef _RE_GM_COMMAND
  print("......");
  if (!re_gm_command())
    print("......failed.\n");
  print("succeed.\n");
#endif

#ifdef _FIND_TREASURES
  print("Start to init find treasure......");
  if (!FindTreasures_init())
    print("......failed.\n");
  else
    print("succeed.\n");
#endif
  print("开始初始化地图......");
  if (!MAP_initReadMap(getMaptilefile(), getMapdir()))
    goto CLOSEBIND;
  print("成功.\n");
  print("Start to init npc setting......");
  if (!NPC_readNPCSettingFiles(getNpcdir(), getNpctemplatenum(),
                               getNpccreatenum()))
    goto CLOSEBIND;
  print("succeed.\n");

#ifdef _PLAYER_DIY_MAP
  print("Start to init player diy map......");
  if (!MAP_intPlayerMap())
    print("......failed.\n");
  print("succeed.\n");
#endif

#ifdef _FILTER_TALK
  print("Start to init filter talk......");
  if (!ReadFilterTalk())
    print("......failed.\n");
  else
    print("succeed.\n");
#endif
  print("Start to connect host...... ");
  acfd = connectHost(getAccountservername(), getAccountserverport());
  if (acfd == -1) {
    print("连接 SAAC 失败：%s:%u。请确认 SAAC 已启动且端口配置正确。\n",
          getAccountservername(), getAccountserverport());
    goto CLOSEBIND;
  }
#ifdef _EPOLL_ET_MODE
  if (epoll_add_acfd(acfd) == -1)
    goto CLOSEBIND;
#endif
  print("succeed.\n");
  initConnectOne(acfd, NULL, 0);
  if (!CONNECT_acfdInitRB(acfd))
    goto CLOSEAC;
  if (!CONNECT_acfdInitWB(acfd))
    goto CLOSEAC;
  CONNECT_setCtype(acfd, AC);
  print("开始初始化SAAC客户端......");
  if (SaacClient_InitClient(lsrpcClientWriteFunc, LSGENWORKINGBUFFER, acfd) < 0)
    goto CLOSEAC;
  print("成功.\n");
  print("向SAAC发送登录的服务器名和密码......");
  {
#if _ATTESTAION_ID == 1
    // 2026.08.21 SaacClient是 GMSV用的
    SaacClient_ACServerLogin_send(acfd, _ATTESTAION_ID, getGameservername(),
                                 getAccountserverpasswd());
#else
    SaacClient_ACServerLogin_send(acfd, getGameservername(),
                                 getAccountserverpasswd());
#endif
  }
  print("成功.\n");
#ifdef _OTHER_SAAC_LINK
  OtherSaacConnect();
#endif
  if (IsFileExist(getLsgenlogfilename())) {
    extern WorkSpace gSaacWorkSpace;
    SetLogFiles(&gSaacWorkSpace, getLsgenlogfilename(),
                getLsgenlogfilename());
  }
  print("为工作空间设置对应的日志文件:%s\n",getLsgenlogfilename());
#ifdef _LOTTERY_SYSTEM
  SaacClient_LotterySystem_send();
#endif
  char logConfFile[512];
  snprintf(logConfFile, sizeof(logConfFile), "%s/%s", getLogdir(),
          getLogconffile());
  print("Start to load log conf file: %s......", logConfFile);
  if (!initLog(logConfFile))
    goto CLOSEAC;
  print("succeed.\n");
#ifdef _PET_ITEM
  restoreObjects(getStoredir());
#endif
#ifdef _ITEM_QUITPARTY
  print("Start to init item quit party......");
  f = fopen(getitemquitparty(), "r");
  if (f != NULL) {
    while (fgets(line, sizeof(line), f)) {
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      itemquitparty_num++;
    }
    if (fseek(f, 0, SEEK_SET) == -1) {
      print("Item Quit Party file is not open proper.\n");
      fclose(f);
      goto CLOSEAC;
    }
    Disappear_Item = allocateMemory(sizeof(struct tagDisappearItem) * itemquitparty_num);
    if (Disappear_Item == NULL) {
      print("Disappear_Item is too big to allocate, %d\n",
            sizeof(struct tagDisappearItem) * itemquitparty_num);
      fclose(f);
      goto CLOSEAC;
    }

    i = 0;
    // Disappear_Item.string
    while (fgets(line, sizeof(line), f)) {
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      sprintf(Disappear_Item[i].string, "%s", line);
      print("DisapperItem:%s\n", Disappear_Item[i].string);
      i++;
    }
    fclose(f);
  }
#endif
  DEBUG_ADJUSTTIME = 0;
  return TRUE;
CLOSEAC:
  logOut("Close AC.\n");
  close(acfd);
CLOSEBIND:
  logOut("关闭绑定的文件描述符.\n");
  close(bindedfd);
  endConnect();
MEMEND:
  logOut("释放内存.\n");
  memEnd();
  return FALSE;
}
