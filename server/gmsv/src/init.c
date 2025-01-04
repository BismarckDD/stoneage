#include "version.h"
#include "linux_platform.h"
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "char_talk.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"
#include "function.h"
#include "handletime.h"
#include "item.h"
#include "item_gen.h"
#include "log.h"
#include "lssproto_serv.h"
#include "magic_base.h"
#include "msignal.h"
#include "net.h"
#include "npc_quiz.h"
#include "object.h"
#include "pet_skill.h"
#include "petmail.h"
#include "readmap.h"
#include "readnpc.h"
#include "saacproto_cli.h"
#include "title.h"
#include "util.h"
#ifdef _TALK_MOVE_FLOOR
#include "longzoro/move.h"
#endif
#ifdef _LUCK_STAR
#include "longzoro/luckstar.h"
#endif
#include "autil.h"
#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
#include "profession_skill.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/mylua.h"
#endif
#ifdef _ONLINE_SHOP
#include "longzoro/newshop.h"
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
  /*print( "Usage: %s ["OPTIONSTRING"]\n", progname );*/
  print("          [-d debuglevel]        default value is 0\n");
  print("          [-f configfilename]    default value is setup.cf\n");
}

/*
 *
 * ¦��
 * ߯Ի��
 *      TRUE(1)     ����ئ��Ѩ������̼�¦�ѷ�������
 *      FALSE(0)    ����ئ��Ѩ������̼�¦�ѷ�������
 */
BOOL parseCommandLine(int argc, char **argv) {
  int c;               /* getopt ƥ���� */
  extern char *optarg; /* getopt ƥ���� */

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
      return FALSE;
      break;
    case 'c':
#ifdef _CRYPTO_DATA
      if (opendir("allblues") == NULL) {
        if (mkdir("allblues", 0777) == 0) {
          printf("�����ļ��� allblues\n");
        }
      }
      if (opendir("allblues/data") == NULL) {
        if (mkdir("allblues/data", 0777) == 0) {
          printf("�����ļ��� data\n");
        }
      }
      List("data");
      printf("data��������ɣ������allblues�ļ���\n");
      return FALSE;
#endif
      break;
#ifdef _CRYPTO_LUA
    case 'l': {
      int flg = 0, id = 0;
      printf("������Ҫ���ܻ��ǽ��ܣ�(0Ϊ����, 1Ϊ����):");
      scanf("%d", &flg);
      printf("����ID�Ƕ���:");
      scanf("%d", &id);
      CryptoAllbluesLUA("allblues", flg, id);
      if (flg == 0) {
        printf("����ɽ��ܹ���\n");
      } else {
        printf("����ɼ��ܹ���\n");
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

/*
 * ¦��
 *
 * �ἰ��������ئ��
 */
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

#define GOTORETURNFALSEIFFALSE(x)                                              \
  if (!(x))                                                                    \
  goto RETURNFALSE
/*
 * �����������
 * ¦��
 *      argc    argv����
 *      argv    ��Ѩ������̼�¦��
 * ߯Ի��
 */
BOOL init(int argc, char **argv, char **env) {
#ifdef _ITEM_QUITPARTY
  FILE *f;
  int i;
  char line[256];
#endif
  srand(getpid());
  print("This Program is compiled at %s %s by gcc %s\n", __DATE__, __TIME__,
        __VERSION__);

  defaultConfig(argv[0]);
  GOTORETURNFALSEIFFALSE(parseCommandLine(argc, argv));
  GOTORETURNFALSEIFFALSE(parseEnvironment(env));

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

  print("�����ļ�: %s\n", getConfigfilename());

  GOTORETURNFALSEIFFALSE(readconfigfile(getConfigfilename()));

  nice(getrunlevel());
  // ttom start
  {
    int iWork = setEncodeKey();
    if (iWork == 0) {
      // �޼������ƽ��ëɬ��
      printf("----------------------------------------\n");
      printf("-------------[����] �޷����� %s\n",
             getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      // �޼������ƽ��ëɬ��
      printf("���� = %d\n", iWork);
    }
  }
  // AcWBuffëɬ��
  {
    int iWork = setAcWBSize();
    if (iWork == 0) {
      printf("----------------------------------------\n");
      printf("-------------[AC����] �޷����� %s\n",
             getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      printf("AC���� = %d\n", iWork);
    }
  }
  // ttom end

  if (getDebuglevel() >= 1) {
    //		print("ServerType: %d\n", getServerType() );
    print("���Եȼ�: %d\n", getDebuglevel());
    print("���еȼ�: %d\n", getrunlevel());
    print("���ջ���: %d\n", getrecvbuffer() * 1024);
    print("���ͻ���: %d\n", getsendbuffer() * 1024);
    print("���ջ�������: %d\n", getrecvlowatbuffer());
    print("�ڴ浥Ԫ��С: %d\n", getMemoryunit());
    print("�ڴ浥Ԫ����: %d\n", getMemoryunitnum());

    print("�˺ŷ�������ַ: %s\n", getAccountservername());
    print("�˺ŷ������˿�: %d\n", getAccountserverport());
    print("��½����������: %s\n", getGameservername());
    print("��½����������: %s\n", getAccountserverpasswd());

    print("�ȴ����Ӷ˿�: %d\n", getPortnumber());

    print("��������к�: %d\n", getServernumber());

    print("�ظ���ַʹ��: %d\n", getReuseaddr());

    print("�����������: %d\n", getFdnum());
    print("������߳���: %d\n", getPetcharnum());
    print("���������Ŀ: %d\n", getOtherscharnum());
    print("��������Ŀ: %d\n", getObjnum());
    print("�����Ʒ��Ŀ: %d\n", getItemnum());
    print("���ս����Ŀ: %d\n", getBattlenum());
    print("�����ļ�Ŀ¼: %s\n", getTopdir());
    print("��ͼ�ļ�Ŀ¼: %s\n", getMapdir());
    print("��ͼ��ʶ�ļ�: %s\n", getMaptilefile());
    print("��Ʒ�����ļ�: %s\n", getItemfile());
    print("����ս���ļ�: %s\n", getInvfile());
    print("��ʾλ���ļ�: %s\n", getAppearfile());
    print("���������ļ�: %s\n", getEffectfile());
    print("ͷ�������ļ�: %s\n", getTitleNamefile());
    print("ͷ�������ļ�: %s\n", getTitleConfigfile());
    print("���������ļ�: %s\n", getEncountfile());
    print("������Ⱥ�ļ�: %s\n", getGroupfile());
    print("��������ļ�: %s\n", getEnemyBasefile());
    print("���������ļ�: %s\n", getEnemyfile());
    print("����ħ���ļ�: %s\n", getMagicfile());

#ifdef _ATTACK_MAGIC
    print("����ħ���ļ�: %s\n", getAttMagicfileName());
#endif

    print("���＼���ļ�: %s\n", getPetskillfile());

#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
    print("ְҵ�����ļ�: %s\n", getProfession());
#endif

    print("��Ʒ�ɷ��ļ�: %s\n", getItematomfile());
    print("���������ļ�: %s\n", getQuizfile());
#ifdef _GMRELOAD
    print("G M �����ļ�: %s\n", getGMSetfile());
#endif
    print("��־��¼�ļ�: %s\n", getLsgenlogfilename());
    print("��ԭ����Ŀ¼: %s\n", getStoredir());
    print("NPC ����Ŀ¼: %s\n", getNpcdir());
    print("��־�����ļ�: %s\n", getLogdir());
    print("��־�����ļ�: %s\n", getLogconffile());
    print("GM��ָ������: %s\n", getChatMagicPasswd());
    print("ʹ��GM��Ȩ��: %d\n", getChatMagicCDKeyCheck());

    print("NPC ģ����Ŀ: %d\n", getNpctemplatenum());
    print("NPC �����Ŀ: %d\n", getNpccreatenum());

    print("��·ʱ����: %d\n", getWalksendinterval());
    print("������м��: %d\n", getCAsendinterval_ms());
    print("���Ŀ����: %d\n", getCDsendinterval_ms());
    print("ִ��һ��ʱ��: %d\n", getOnelooptime_ms());
    print("�������ʱ��: %d\n", getPetdeletetime());
    print("�������ʱ��: %d\n", getItemdeletetime());
#ifdef _DEL_DROP_GOLD
    print("ʯ�����ʱ��: %d\n", getGolddeletetime());
#endif
    print("���ݱ�����: %d\n", getCharSavesendinterval());

    print("��Ƭ�����Ŀ: %d\n", getAddressbookoffmsgnum());
    print("��ȡƵ��Э��: %d\n", getProtocolreadfrequency());

    print("���Ӵ�������: %d\n", getAllowerrornum());
#ifdef _GET_BATTLE_EXP
    print("ս�����鱶��: %d��\n", getBattleexp());
#endif
#ifdef _NEW_PLAYER_CF
    print("��������ת��: %dת\n", getNewplayertrans());
    print("��������ȼ�: %d��\n", getNewplayerlv());
    print("���������Ǯ: %d S\n", getNewplayergivegold());
    print("��������ȼ�: %d��\n", getNewplayerpetlv());
#ifdef _VIP_SERVER
    print("����ӵ�е���: %d��\n", getNewplayergivevip());
#endif
    print("��������ȼ�: %d\n", getRidePetLevel());
#ifdef _NEW_PLAYER_RIDE
    print("�����������: %s\n", getPlayerRide());
#endif
    print("����ӵ�г���: NO1:%d NO2:%d NO3:%d NO4:%d NO5:%d\n",
          getNewplayergivepet(0), getNewplayergivepet(1),
          getNewplayergivepet(2), getNewplayergivepet(3),
          getNewplayergivepet(4));
    print("����ӵ����Ʒ: ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
          "��������������ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
          "��������������ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n",
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
    print("��ֹ��������: ����1:%s ����2:%s ����3:%s ����4:%s ����5:%s\n",
          getUnregname(0), getUnregname(1), getUnregname(2), getUnregname(3),
          getUnregname(4));
#endif

#ifdef _WATCH_FLOOR
    print("�Ƿ�ȫͼ��ս: %s\n", getWatchFloorCF());
    if (strcmp(getWatchFloorCF(), "��"))
      print("������ս��ͼ: ��ͼ1:%d ��ͼ2:%d ��ͼ3:%d ��ͼ4:%d ��ͼ5:%d\n",
            getWatchFloor(1), getWatchFloor(2), getWatchFloor(3),
            getWatchFloor(4), getWatchFloor(5));
#endif

#ifdef _BATTLE_FLOOR
    print("�Ƿ�ǿ��ս��: %s\n", getBattleFloorCF());
    if (strcmp(getBattleFloorCF(), "��"))
      print("ǿ��ս����ͼ: ��ͼ1:%d ��ͼ2:%d ��ͼ3:%d ��ͼ4:%d ��ͼ5:%d\n",
            getBattleFloor(1), getBattleFloor(2), getBattleFloor(3),
            getBattleFloor(4), getBattleFloor(5));
#endif

#ifdef _TRANS_LEVEL_CF
    print("����ȼ�ת��: %d��\n", getChartrans());
    print("����ȼ�ת��: %d��\n", getPettrans());
#endif

#ifdef _POINT
    print("��ֹ��������: %s\n", getPoint());
    if (strcmp(getPoint(), "��"))
      print("ÿת��������: 0ת:%d 1ת:%d 2ת:%d 3ת:%d 4ת:%d 5ת:%d 6ת:%d\n",
            getTransPoint(0), getTransPoint(1), getTransPoint(2),
            getTransPoint(3), getTransPoint(4), getTransPoint(5),
            getTransPoint(6));
#endif

#ifdef _PET_AND_ITEM_UP
    print("�����ܷ���: %s\n", getPetup());
    print("�����ܷ���: %s\n", getItemup());
#endif
#ifdef _LOOP_ANNOUNCE
    print("ѭ������·��: %s\n", getLoopAnnouncePath());
    print("ѭ��ʱ����: %d����\n", getLoopAnnounceTime());
#endif
#ifdef _SKILLUPPOINT_CF
    print("ÿ����������: %d\n", getSkup());
#endif

#ifdef _RIDELEVEL
    print("���ȼ����: %d��\n", getRideLevel());
#endif
#ifdef _REVLEVEL
    print("��ԭ���޵ȼ�: %s��\n", getRevLevel());
#endif
#ifdef _TRANS_LEVEL_CF
    print("һ��ȼ�����: %d��\n", getYBLevel());
    print("��ߵȼ�����: %d��\n", getMaxLevel());
#endif
#ifdef _FIX_CHARLOOPS
    print("��ħʱ�䱶��: %d��\n", getCharloops());
#endif
#ifdef _PLAYER_ANNOUNCE
    if (getPAnnounce() == -1)
      print("�������ĵ���: �ر�ʹ��\n");
    else
      print("�������ĵ���: %d��\n", getPAnnounce());
#endif
#ifdef _PLAYER_MOVE
    if (getPMove() == -1)
      print("˳�����ĵ���: �ر�ʹ��\n");
    else
      print("˳�����ĵ���: %d��\n", getPMove());
#endif
#ifdef _BATTLE_GOLD
    print("ս����ý�Ǯ: %d\n", getBattleGold());
#endif
#ifdef _ANGEL_TIME
    print("�����ٻ�ʱ��: (%d��/��������)��\n", getAngelPlayerTime());
    print("�����ٻ�����: %d��\n", getAngelPlayerMun());
#endif
#ifdef _RIDEMODE_20
    print("2.0 ���ģʽ: %d\n", getRideMode());
#endif
#ifdef _FM_POINT_PK
    print("ׯ԰����ģʽ: %s\n", getFmPointPK());
#endif
  }

  { // andy_add 2003/05/05 check GameServer Name
    char *GameServerName;
    GameServerName = getGameserverID();
    if (GameServerName == NULL || strlen(GameServerName) <= 0)
      return FALSE;
    print("\n��Ϸ������ID: %s\n", GameServerName);
  }

  print("��ʼ��ʼ��\n");

  // #define DEBUG1( arg... ) if( getDebuglevel()>1 ){##arg}
  print("�����ڴ�ռ�...");
  GOTORETURNFALSEIFFALSE(configmem(getMemoryunit(), getMemoryunitnum()));
  GOTORETURNFALSEIFFALSE(memInit());
  print("���\n");

  print("ʼ�ջ����ӿռ�...");
  if (!initConnect(getFdnum()))
    goto MEMEND;
  print("���\n");
  while (1) {
    print("���԰󶨱��ض˿� %d... ", getPortnumber());
#ifdef _EPOLL_ET_MODE
    bindedfd = epoll_bind(getPortnumber());
#else
    bindedfd = bindlocalhost(getPortnumber());
#endif
    if (bindedfd == -1)
      sleep(10);
    else
      break;
  }
  print("���\n");
  print("��������...");
  if (!initObjectArray(getObjnum()))
    goto CLOSEBIND;
  print("���\n");

  print("��������...");
#ifdef _OFFLINE_SYSTEM
  if (!CHAR_initCharArray(getPlayercharnum(), getPetcharnum(),
                          getOtherscharnum()))
#else
  if (!CHAR_initCharArray(getFdnum(), getPetcharnum(), getOtherscharnum()))
#endif
    goto CLOSEBIND;
  print("���\n");
  print("������Ʒ...");
  if (!ITEM_readItemConfFile(getItemfile()))
    goto CLOSEBIND;
  if (!ITEM_initExistItemsArray(getItemnum()))
    goto CLOSEBIND;
  print("���\n");

  print("����ս��...");
  if (!BATTLE_initBattleArray(getBattlenum()))
    goto CLOSEBIND;
  print("���\n");

  print("��������ģ��...");
  if (!initFunctionTable())
    goto CLOSEBIND;
  print("���\n");

  print("��ʼ���ʼ�...");
  if (!PETMAIL_initOffmsgBuffer(getAddressbookoffmsgnum()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ����ս���ļ�...");
  if (!CHAR_initInvinciblePlace(getInvfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ��ʾλ���ļ�...");
  if (!CHAR_initAppearPosition(getAppearfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡͷ�������ļ�...");
  if (!TITLE_initTitleName(getTitleNamefile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡͷ�������ļ�...");
  if (!TITLE_initTitleConfig(getTitleConfigfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ���������ļ�...");
  if (!ENCOUNT_initEncount(getEncountfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ��������ļ�...");
  if (!ENEMYTEMP_initEnemy(getEnemyBasefile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ���������ļ�...");
  if (!ENEMY_initEnemy(getEnemyfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ������Ⱥ�ļ�...");
  if (!GROUP_initGroup(getGroupfile()))
    goto CLOSEBIND;
  print("���\n");
  print("��ȡħ���ļ�...");
  if (!MAGIC_initMagic(getMagicfile()))
    goto CLOSEBIND;
  print("���\n");

#ifdef _ATTACK_MAGIC

  print("��ȡħ�������ļ�...");

  if (!ATTMAGIC_initMagic(getAttMagicfileName()))
    //		if( !ATTMAGIC_initMagic( getMagicfile() ) )
    goto CLOSEBIND;

  print("ħ�������ļ� -->%s...", getAttMagicfileName());
  print("���\n");

#endif

  print("��ȡ���＼���ļ�...");
  if (!PETSKILL_initPetskill(getPetskillfile()))
    goto CLOSEBIND;
  print("���\n");

#ifdef _PROFESSION_SKILL // WON ADD ����ְҵ����
  print("��ȡְҵ�����ļ�...");
  if (!PROFESSION_initSkill(getProfession())) {
    goto CLOSEBIND;
  }
  print("���\n");
#endif

  /* ʧ��  ة����    ë  �� */
  print("��ȡ��Ʒ�ɷ��ļ�...");
  if (!ITEM_initItemAtom(getItematomfile()))
    goto CLOSEBIND;
  print("���\n");

  print("��ʼ�������ϳ���Ʒ...");
  if (!ITEM_initItemIngCache())
    goto CLOSEBIND;
  print("���\n");

  print("��ʼ�����ϳ�����趨...");
  if (!ITEM_initRandTable())
    goto CLOSEBIND;
  print("���\n");

  print("��ȡ���������ļ�...");
  if (!CHAR_initEffectSetting(getEffectfile()))
    goto CLOSEBIND;
  print("���\n");
  print("��ȡ���������ļ�...");
  if (!QUIZ_initQuiz(getQuizfile()))
    goto CLOSEBIND;
  print("���\n");
#ifdef _GMRELOAD
  print("��ȡGM�����ļ�...");
  if (!LoadGMSet(getGMSetfile()))
    goto CLOSEBIND;
  print("���\n");
#endif

#ifdef _USER_EXP_CF
  print("��ȡ���������ļ�...");
  if (!LoadEXP(getEXPfile()))
    goto CLOSEBIND;
  print("��ߵȼ�: %d...", getMaxLevel());
  print("һ��ȼ�: %d...", getYBLevel());
  print("���\n");
#endif

#ifdef _ANGEL_SUMMON
  print("��ȡ�����ٻ������б��ļ�...");
  if (!LoadMissionList())
    goto CLOSEBIND;
  print("���\n");
#endif

#ifdef _JOBDAILY
  print("��ȡ������־�ļ�...");
  if (!LoadJobdailyfile())
    print("...ʧ��\n");
  else
    print("���\n");
#endif

#ifdef _LOOP_ANNOUNCE
  print("��ȡѭ�������ļ�...");
  if (!loadLoopAnnounce())
    print("...ʧ��\n");
  else
    print("���\n");
#endif
#ifdef _RIDE_CF
  print("��ȡ�Զ�������ļ�...");
  if (!CHAR_Ride_CF_init())
    print("...ʧ��\n");
  print("���\n");
#endif
#ifdef _FM_LEADER_RIDE
  print("��ȡׯ԰�峤ר������ļ�...");
  if (!CHAR_FmLeaderRide_init())
    print("...ʧ��\n");
  print("���\n");
#endif
#ifdef _RE_GM_COMMAND
  print("�Զ���GM�����������ļ�...");
  if (!re_gm_command())
    print("...ʧ��\n");
  print("���\n");
#endif

#ifdef _FIND_TREASURES
  print("��ȡѰ����Ʒ�����ļ�...");
  if (!FindTreasures_init())
    print("...ʧ��\n");
  print("���\n");
#endif
  print("������ͼ...");
  if (!MAP_initReadMap(getMaptilefile(), getMapdir()))
    goto CLOSEBIND;
  print("���\n");
  print("��ȡNPC�ļ�...");
  if (!NPC_readNPCSettingFiles(getNpcdir(), getNpctemplatenum(),
                               getNpccreatenum()))
    goto CLOSEBIND;
  print("���\n");

#ifdef _TALK_MOVE_FLOOR
  print("��ȡ˵���ƶ���ͼ�ļ�...");
  if (!MoveMap_init())
    print("...ʧ��\n");
  print("���\n");
#endif

#ifdef _LUCK_STAR
  print("��ȡ�������ļ�...");
  if (!LuckStar_init())
    print("...ʧ��\n");
  print("���\n");
#endif

#ifdef _ONLINE_SHOP
  print("��ȡ�����̳������ļ�...");
  if (!OnlineShop_init())
    print("...ʧ��\n");
  print("���\n");
#endif

#ifdef _PLAYER_DIY_MAP
  print("��ʼ�����DIY��ͼ...");
  if (!MAP_intPlayerMap())
    print("...ʧ��\n");
  print("���\n");
#endif

#ifdef _FILTER_TALK
  print("��ȡ˵�������ļ�...");
  if (!ReadFilterTalk())
    print("...ʧ��\n");
  print("���\n");
#endif

  print("��ʼ�� NPC ������... ");
  if (lssproto_InitServer(lsrpcClientWriteFunc, LSGENWORKINGBUFFER) < 0)
    goto CLOSEBIND;
  print("���\n");
  print("���������˺ŷ�����... ");
  acfd = connectHost(getAccountservername(), getAccountserverport());
  if (acfd == -1)
    goto CLOSEBIND;

#ifdef _EPOLL_ET_MODE
  if (epoll_add_acfd(acfd) == -1)
    goto CLOSEBIND;
#endif

  /*
          {
                  int errorcode;
                  int errorcodelen;
                  int qs;

                  errorcodelen = sizeof(errorcode);
                  qs = getsockopt( acfd, SOL_SOCKET, SO_RCVBUF , &errorcode,
     &errorcodelen);
                  //andy_log
                  print("\n\n GETSOCKOPT SO_RCVBUF: [ %d, %d, %d] \n", qs,
     errorcode, errorcodelen);
          }
  */

  print("���\n");
  initConnectOne(acfd, NULL, 0);
  if (!CONNECT_acfdInitRB(acfd))
    goto CLOSEAC;
  if (!CONNECT_acfdInitWB(acfd))
    goto CLOSEAC;
  CONNECT_setCtype(acfd, AC);

  print("��ʼ�� �˺� �ͻ��� ... ");
  if (saacproto_InitClient(lsrpcClientWriteFunc, LSGENWORKINGBUFFER, acfd) < 0)
    goto CLOSEAC;
  print("���\n");

  print("���˺ŷ��������͵�½����... ");
  /*  ����̼�ۢ��ë����  */
  {
#if _ATTESTAION_ID == 1
    saacproto_ACServerLogin_send(acfd, _ATTESTAION_ID, getGameservername(),
                                 getAccountserverpasswd());
#else
    saacproto_ACServerLogin_send(acfd, getGameservername(),
                                 getAccountserverpasswd());
#endif
  }
  print("���\n");
#ifdef _OTHER_SAAC_LINK
  OtherSaacConnect();
#endif

  if (isExistFile(getLsgenlogfilename())) {
    lssproto_SetServerLogFiles(getLsgenlogfilename(), getLsgenlogfilename());
    saacproto_SetClientLogFiles(getLsgenlogfilename(), getLsgenlogfilename());
  }

  print("��ʼ�������\n");

#ifdef _LOTTERY_SYSTEM
  saacproto_LotterySystem_send();
#endif
  print("��ʼ��Ѱ�µ���־ ... ");
  {
    char logconffile[512];
    snprintf(logconffile, sizeof(logconffile), "%s/%s", getLogdir(),
             getLogconffile());
    if (!initLog(logconffile))
      goto CLOSEAC;
  }
  print("���\n");
#ifdef _PET_ITEM
  restoreObjects(getStoredir());
#endif
#ifdef _ITEM_QUITPARTY
  print("��ȡ�����ɢ��Ʒ��ʧ�ļ�...");

  // ��ȡ����
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", getitemquitparty());
  f = fopen(realopfile, "r");
  if (f != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    f = fopen(getitemquitparty(), "r");
  }
  if (f != NULL) {
    while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(line);
      }
#endif
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      itemquitparty_num++;
    }
    if (fseek(f, 0, SEEK_SET) == -1) {
      print("��Ʒ¼�Ҵ���\n");
      fclose(f);
      goto CLOSEAC;
    }
    // �������
    Disappear_Item =
        allocateMemory(sizeof(struct tagDisappearItem) * itemquitparty_num);
    if (Disappear_Item == NULL) {
      print("�޷������ڴ� %d\n",
            sizeof(struct tagDisappearItem) * itemquitparty_num);
      fclose(f);
      goto CLOSEAC;
    }

    i = 0;
    // �����߱�Ŵ��� Disappear_Item.string
    while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(line);
      }
#endif
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      sprintf(Disappear_Item[i].string, "%s", line);
      print("\n���߱��:%s", Disappear_Item[i].string);
      i++;
    }
    fclose(f);
  }
#endif

  DEBUG_ADJUSTTIME = 0;
  print("\n");
  return TRUE;

CLOSEAC:
  close(acfd);
CLOSEBIND:
  close(bindedfd);
  endConnect();
MEMEND:
  memEnd();
RETURNFALSE:
  return FALSE;
}
