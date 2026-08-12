#define __CONFIGFILE_C__
#include "version.h"
//
#include "gmsv_server.h"
#include "util.h"
// ttom start.
#include "config_file.h"
#include "net.h"
// ttom end
#include "npcutil.h"
// Arminius 7.12 login announce
#include "char.h"
#include "char_data.h"
// CoolFish: add
#include "autil.h"

#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

/* General Config of server. */
typedef struct tagServerConfig {
  char program_name[32];     /* program name. */
  char config_filename[32];  /* usually as setup.cf */
  unsigned char debug_level; /* usually as 3 */
  unsigned int memory_unit;
  unsigned int memory_unitnum;
  char account_server_name[32];       /* */
  unsigned short account_server_port; /* Account Server Port*/
  char account_server_password[32];   /* Account Server Password */
  char game_server_name[32];          /* Game Server Name*/
  // Arminius 7.24 manor pk
  char game_server_id[32];     // game server chinese id
  unsigned short allowmanorpk; // is this server allow manor pk
  unsigned short port; /* 扔□田及谨切熬仃禾□玄 */
  int servernumber;    /* 必□丞扔□田及  寞 */
  int reuse_addr;      /* Address already used... */
  int do_nodelay;      /* TCP_NODELAY 卞允月井升丹井 */
  int log_write_time;  /* */
  int log_io_time;     /* I/O */
  int log_game_time;   /* */
  int log_netloop_faster;    /* netloop_faster 及夫弘 */
  int saacwritenum;          /* */
  int saacreadnum;           /* */
  unsigned short fdnum;      /* fd num: < 65536, unsigned short is enough. */
  unsigned short charnum;    /* char num: < 65536? */
  unsigned int othercharnum; /* */
  unsigned int objnum;       /* obj num in Game. */
  unsigned int petcharnum;   /* pet num in Game. */
  unsigned int itemnum;      /* item num in Game. */
  unsigned int battlenum;    /* battle num in Game. */
#ifdef _GET_BATTLE_EXP
  unsigned int battleexp;
#endif
  char topdir[64];          /* usually as "." */
  char mapdir[64];          /* data/map. */
  char maptilefile[64];     /* data/map/ */
  char battlemapfile[64];   /* data/map/ */
  char itemfile[64];        /* data/itemfile.txt */
  char invfile[64];         /* */
  char appearfile[64];      /* 请蜇匏  涩烂白央奶伙  */
  char titlenamefile[64];   /* 惫寞白央奶伙   */
  char titleconfigfile[64]; /* 惫寞涩烂白央奶伙   */
  char encountfile[64];
  char enemybasefile[64]; /* pet base config file */
  char enemyfile[64];     /* pet base info file */
  char groupfile[64];     /* enemy group config file */
  char magicfile[64];     /* */
#ifdef _ATTACK_MAGIC
  char attmagicfile[64]; // 攻击性咒术
#endif
  char petskillfile[64]; /* 矢永玄  热诸涩烂白央奶伙   */
  char itematomfile[64]; /* 失奶  丞及笺    白央奶伙 */
  char effectfile[64];   /* 梢请涩烂白央奶伙  */
  char quizfile[64];     /* 弁奶术涩烂白央奶伙  */
  char lsgenlog[64]; /*扔□田及lsgen 失它玄皿永玄白央奶伙  */
  char storedir[64]; /*旦玄失犯奴伊弁玄伉    */
  char npcdir[64]; /*NPC及涩烂白央奶伙毛  仁犯奴伊弁玄伉   */
  char logdir[64];
  char logconfname[64];
  char chatmagicpasswd[64]; /**/
#ifdef _STORECHAR
  char storechar[64];
#endif
  unsigned int chatmagiccdkeycheck;  /* */
  unsigned int filesearchnum;        /* */
  unsigned int npctemplatenum;       /* */
  unsigned int npccreatenum;         /* */
  unsigned int walksendinterval;     /* */
  unsigned int CAsendinterval_ms;    /* What's CA? */
  unsigned int CDsendinterval_ms;    /* What's CD? */
  unsigned int Onelooptime_ms;       /* */
  unsigned int Petdeletetime;        /* */
  unsigned int Itemdeletetime;       /* */
  unsigned int CharSavesendinterval; /* */
  unsigned int addressbookoffmsgnum; /* */
  unsigned int protocolreadfrequency;
  unsigned int allowerrornum;
  unsigned int loghour;
  unsigned int battledebugmsg;
  // ttom add this because the second had this
  unsigned int encodekey;
  unsigned int acwbsize;
  unsigned int acwritesize;
  unsigned int ErrUserDownFlg;
  // ttom end
#ifdef _GMRELOAD
  char gmsetfile[64]; /* gmset.txt */
#endif
#ifdef _PROFESSION_SKILL
  char profession[32];
#endif
#ifdef _ITEM_QUITPARTY
  char itemquitparty[32];
#endif
#ifdef _DEL_DROP_GOLD
  unsigned int Golddeletetime;
#endif
#ifdef _NEW_PLAYER_CF
  int newplayertrans;
  int newplayerlv;
  int newplayergivepet[5];
  int newplayergiveitem[15];
  int newplayerpetlv;
  int newplayergivegold;
  int ridepetlevel;
#ifdef _VIP_SERVER
  int newplayerpetvip;
#endif
#endif
#ifdef _USER_EXP_CF
  char expfile[64];
#endif
#ifdef _UNLAW_WARP_FLOOR
  char unlawwarpfloor[512];
#endif
#ifdef _NO_JOIN_FLOOR
  char nojoinfloor[512];
#endif
#ifdef _WATCH_FLOOR
  int watchfloor[6];
#endif
#ifdef _BATTLE_FLOOR
  int battlefloor;
  char battlefloorcf[512];
#endif
#ifdef _UNREG_NEMA
  char unregname[5][16];
#endif
#ifdef _TRANS_LEVEL_CF
  int chartrans;
  int pettrans;
  int yblevel;
  int maxlevel;
#endif
#ifdef _POINT
  int point;
  int transpoint[8];
#endif
#ifdef _VIP_SERVER
  int vip_point;
#endif
#ifdef _PET_AND_ITEM_UP
  int petup;
  int itemup;
#endif
#ifdef _LOOP_ANNOUNCE
  char loopannouncepath[32];
  int loopannouncetime;
  char loopannounce[10][1024];
  int loopannouncemax;
#endif
#ifdef _SKILLUPPOINT_CF
  int skup;
#endif
#ifdef _RIDELEVEL
  int ridelevel;
  int ridetrans;
#endif
#ifdef _REVLEVEL
  int revlevel;
#endif
#ifdef _NEW_PLAYER_RIDE
  int npride;
#endif
#ifdef _FIX_CHARLOOPS
  int charloops;
#endif
#ifdef _PLAYER_ANNOUNCE
  int pannounce;
#endif
#ifdef _PLAYER_MOVE
  int pmove;
#endif
  int recvbuffer;
  int sendbuffer;
  int recvlowatbuffer;
  int runlevel;
#ifdef _SHOW_VIP_CF
  int showvip;
#endif
#ifdef _PLAYER_NUM
  int playernum;
#endif
#ifdef _BATTLE_GOLD
  int battlegold;
#endif
#ifdef _ANGEL_TIME
  int angelplayertime;
  int angelplayermun;
#endif
#ifdef _RIDEMODE_20
  int ridemode;
#endif
#ifdef _FM_POINT_PK
  int fmpointpk;
#endif
#ifdef _ENEMY_ACTION
  int enemyact;
#endif
#ifdef _FUSIONBEIT_TRANS
  char fusionbeittrans;
#endif
#ifdef _CHECK_PEPEAT
  int CheckRepeat;
#endif
  int cpuuse;
#ifdef _FM_JOINLIMIT
  int joinfamilytime;
#endif
#ifdef _MAP_HEALERALLHEAL
  char mapheal[256];
#endif
#ifdef _THE_WORLD_SEND
  int thewordtrans;
  int thewordlevel;
  int thewordsend;
#endif
#ifdef _LOGIN_DISPLAY
  int logindisplay;
#endif
#ifdef _VIP_POINT_PK
  char vippointpk[256];
  int vippointpkcost;
#endif
#ifdef _SPECIAL_MAP
  char specialmap[256];
#endif
#ifdef _NEW_AUTO_PK
  int autopk;
  int autopktrans;
  int autopklv;
#ifdef _FORMULATE_AUTO_PK
  int autopkpoint;
  int killpoint;
#endif
#endif
#ifdef _AUTO_DEL_PET
  char autodelpet[256];
#endif
#ifdef _AUTO_DEL_ITEM
  char autodelitem[256];
#endif
#ifdef _BT_PET
  int btpet;
#endif
#ifdef _BT_ITEM
  int btitem;
#endif
#ifdef _LUCK_STAR
  int luckstartime;
  int luckstarchances;
#endif
#ifdef _BATTLE_GETITEM_RATE
  char battlegetitemrate[256];
  int battlegetitemratemap;
#endif
#ifdef _BOUND_TIME_EXPANSION
  char unboundtimemap[64];
  int unboundtimelevel;
  int unboundtimetrans;
#endif

#ifdef _UNLAW_THIS_LOGOUT
  char unlawthislogout[256];
#endif

#ifdef _TRANS_POINT_UP
  char transpointup[256];
#endif
#ifdef _OPEN_STW_SEND
  int stwsendtype;
  int stwsendpoint;
#endif
#ifdef _POOL_ITEM_BUG
  int poolitembug;
  char poolitem[256];
#endif
#ifdef _NO_STW_ENEMY
  int nostwenemy;
  int nostwenemypoint;
#endif
#ifdef _NEW_STREET_VENDOR
  char streetvendorpoint[256];
#endif
#ifdef _ITEM_PET_LOCKED
  int itampetlocked;
#endif
#ifdef _TALK_SAVE
  int savefame;
#endif
#ifdef _TALK_CHECK
  int talkcheckmax;
  int talkcheckmin;
#endif
#ifdef _DISABLE_PROFESSION_SKILL
  char disableprofessionskill[256];
#endif
#ifdef _ALL_SERV_SEND
  int allservtrans;
  int allservlevel;
  int allservsend;
#endif
#ifdef _PET_TRANS_ABILITY
  int pettransability;
  int pettransability1;
  int pettransability2;
#endif
#ifdef _NEED_ITEM_ENEMY
  int delneeditem;
#endif
#ifdef _NOT_ESCAPE
  char notescape[256];
#endif
#ifdef _PLAYER_OVERLAP_PK
  char playeroverlappk[256];
#endif
#ifdef _FIMALY_PK_TIME
  int fimalypktime;
#endif
#ifdef _PETSKILL_SHOP_LUA
  char freepetskillshoppath[256];
#endif
#ifdef _CANCEL_ANGLE_TRANS
  int cancelanlgetrans;
#endif
#ifdef _VIP_BATTLE_EXP
  int vipbattleexp;
#endif
#ifdef _NO_HELP_MAP
  char nohelpmap[256];
#endif
#ifdef _BATTLE_TIME
  int battletime;
#endif
#ifdef _SAME_IP_ONLINE_NUM
  int sameiponlinenum;
#endif
#ifdef _STREET_VENDOR_TRANS
  int streetvendortrans;
#endif
#ifdef _CHECK_SEVER_IP
  char serverip[256];
#endif
#ifdef _DAMMAGE_CALC
  int dammagecalc;
#endif
#ifdef _PET_ENEMY_DEVELOP_UP
  int PetEnemyDevelopUp;
#endif
#ifdef _FIRST_LOCK_ITEM
  char FirstLockItem[256];
#endif
  int Connectnum;
#ifdef _PETSKILL_NEW_PASSIVE
  int autogetskill;
  int getskillpos;
  char skillinfolv[5][100];
  int skillfusion;
  char fusionrange[50];
  int skillcount;
#endif
#ifdef _SHARE_EXP
  int expshare;
#endif
#ifdef _DEFEND_BIGBAO
  int bigbao;
  int bigbao2;
#endif
#ifdef _MO_SHOW_DEBUG
  int isdebug;
#endif
#ifdef _CHAR_LOOP_TIME
  int charlooptime;
#endif
#ifdef _MO_RELOAD_NPC
  int reloadnpctime;
  int reloadnpctype;
#endif
#ifdef _JZ_NEWSCRIPT_LUA
  char luafile[256];
#endif
#ifdef _ITEM_LUA
  char itemluafile[256];
#endif
#ifdef _MO_LNS_NLSUOXU
  int mapstart;
#endif
#ifdef _ROOKIE_ITEM
  unsigned int rookieitem[5];
#endif
#ifdef _NO_TRANS_ITEM
  unsigned int notransitem;
#endif

#ifdef _MAX_MERGE_LEVEL
  unsigned int maxmergelevel;
#endif

#ifdef _NO_ATTACK
  int atttime;
  int attsafetime;
  int attcnt;
  int latetime;
  int attdmetime;
  int attdmecnt;
#endif
  char noattip[5][18];
#ifdef _NO_FULLPLAYER_ATT
  int nofullplayer;
  int nofull2player;
  int nocdkeyplayer;
  int nocdkeymode;
  int nocdkeytype;
  int nofulltime;
  int fengtype;
  int nofullendplayer;
  int nofullendtime;
  int manrennum;
#endif
  int locktype;
#ifdef _NEW_FUNC_DECRYPT
  int allowerrornum2;
#endif
#ifdef _MO_LOGIN_NO_KICK
  int loginnokick;
#endif
#ifdef _MO_ILLEGAL_NAME
  char illegalname[256];
#endif
#ifdef _NO_USE_PACKET_MAP
  char nousepacketmap[256];
#endif
#ifdef _NO_USE_MAGIC_MAP
  char nousemagicmap[256];
#endif
#ifdef _SOME_PETMAIL
  int petmailflg;
  char somepetmail[256];
#endif
#ifdef _CTRL_TRANS_DEVELOP
  int ctrltrans;
#endif
#ifdef _PETMAIL_TIME
  int petmailtime;
#endif

#ifdef _UP_BBPETPROB
  int upbbprob;
#endif
} ServerConfig;

ServerConfig gServerConfig;

#ifdef _USER_EXP_CF
int NeedLevelUpTbls[200];
#endif

typedef struct tagReadConf {
  char name[32];
  char *charvalue;
  size_t charsize;
  void *value;
  CTYPE valuetype;
} ReadConf;

ReadConf gReadConf[] = {
    {"debuglevel", NULL, 0, (void *)&gServerConfig.debug_level, CHAR},
    {"usememoryunit", NULL, 0, (void *)&gServerConfig.memory_unit, INT},
    {"usememoryunitnum", NULL, 0, (void *)&gServerConfig.memory_unitnum, INT},
    {"acserv", gServerConfig.account_server_name,
     sizeof(gServerConfig.account_server_name), NULL, 0},
    {"acservport", NULL, 0, (void *)&gServerConfig.account_server_port, SHORT},
    {"acpasswd", gServerConfig.account_server_password,
     sizeof(gServerConfig.account_server_password), NULL, 0},
    {"gameservname", gServerConfig.game_server_name,
     sizeof(gServerConfig.game_server_name), NULL, 0},
    // Arminius 7.24 manor pk
    {"gameservid", gServerConfig.game_server_id,
     sizeof(gServerConfig.game_server_id), NULL, 0},
    {"allowmanorpk", NULL, 0, (void *)&gServerConfig.allowmanorpk, SHORT},
    {"port", NULL, 0, (void *)&gServerConfig.port, SHORT},
    {"servernumber", NULL, 0, (void *)&gServerConfig.servernumber, INT},
    {"reuseaddr", NULL, 0, (void *)&gServerConfig.reuse_addr, INT},
    {"nodelay", NULL, 0, (void *)&gServerConfig.do_nodelay, INT},
    {"log_write_time", NULL, 0, (void *)&gServerConfig.log_write_time, INT},
    {"log_io_time", NULL, 0, (void *)&gServerConfig.log_io_time, INT},
    {"log_game_time", NULL, 0, (void *)&gServerConfig.log_game_time, INT},
    {"log_netloop_faster", NULL, 0, (void *)&gServerConfig.log_netloop_faster,
     INT},
    {"saacwritenum", NULL, 0, (void *)&gServerConfig.saacwritenum, INT},
    {"saacreadnum", NULL, 0, (void *)&gServerConfig.saacreadnum, INT},
    {"fdnum", NULL, 0, (void *)&gServerConfig.fdnum, SHORT},
    {"charnum", NULL, 0, (void *)&gServerConfig.charnum, SHORT},
    {"petnum", NULL, 0, (void *)&gServerConfig.petcharnum, INT},
    {"othercharnum", NULL, 0, (void *)&gServerConfig.othercharnum, INT},
    {"objnum", NULL, 0, (void *)&gServerConfig.objnum, INT},
    {"itemnum", NULL, 0, (void *)&gServerConfig.itemnum, INT},
    {"battlenum", NULL, 0, (void *)&gServerConfig.battlenum, INT},
#ifdef _GET_BATTLE_EXP
    {"battleexp", NULL, 0, (void *)&gServerConfig.battleexp, INT},
#endif
    {"topdir", gServerConfig.topdir, sizeof(gServerConfig.topdir), NULL, 0},
    {"mapdir", gServerConfig.mapdir, sizeof(gServerConfig.mapdir), NULL, 0},
    {"maptilefile", gServerConfig.maptilefile,
     sizeof(gServerConfig.maptilefile), NULL, 0},
    {"battlemapfile", gServerConfig.battlemapfile,
     sizeof(gServerConfig.battlemapfile), NULL, 0},
#ifdef _ITEMSET6_TXT
    {"itemset6file", gServerConfig.itemfile, sizeof(gServerConfig.invfile),
     NULL, 0},
#else
#ifdef _ITEMSET5_TXT
    {"itemset5file", gServerConfig.itemfile, sizeof(gServerConfig.invfile),
     NULL, 0},
#else
#ifdef _ITEMSET4_TXT
    {"itemset4file", gServerConfig.itemfile, sizeof(gServerConfig.invfile),
     NULL, 0},
#else
#ifdef _ITEMSET3_ITEM
    {"itemset3file", gServerConfig.itemfile, sizeof(gServerConfig.invfile),
     NULL, 0},
#endif
#endif
#endif
#endif
    {"invinciblefile", gServerConfig.invfile, sizeof(gServerConfig.invfile),
     NULL, 0},
    {"appearpositionfile", gServerConfig.appearfile,
     sizeof(gServerConfig.appearfile), NULL, 0},
    {"titlenamefile", gServerConfig.titlenamefile,
     sizeof(gServerConfig.titlenamefile), NULL, 0},
    {"titleconfigfile", gServerConfig.titleconfigfile,
     sizeof(gServerConfig.titleconfigfile), NULL, 0},
    {"encountfile", gServerConfig.encountfile,
     sizeof(gServerConfig.encountfile), NULL, 0},
    {"enemyfile", gServerConfig.enemyfile, sizeof(gServerConfig.enemyfile),
     NULL, 0},
    {"enemybasefile", gServerConfig.enemybasefile,
     sizeof(gServerConfig.enemybasefile), NULL, 0},
    {"groupfile", gServerConfig.groupfile, sizeof(gServerConfig.groupfile),
     NULL, 0},
    {"magicfile", gServerConfig.magicfile, sizeof(gServerConfig.magicfile),
     NULL, 0},
#ifdef _ATTACK_MAGIC
    {"attmagicfile", gServerConfig.attmagicfile,
     sizeof(gServerConfig.attmagicfile), NULL, 0},
#endif
#ifdef _PETSKILL2_TXT
    {"petskillfile2", gServerConfig.petskillfile,
     sizeof(gServerConfig.petskillfile), NULL, 0},
#else
    {"petskillfile1", gServerConfig.petskillfile,
     sizeof(gServerConfig.petskillfile), NULL, 0},
#endif
    {"itematomfile", gServerConfig.itematomfile,
     sizeof(gServerConfig.itematomfile), NULL, 0},
    {"effectfile", gServerConfig.effectfile, sizeof(gServerConfig.effectfile),
     NULL, 0},
    {"quizfile", gServerConfig.quizfile, sizeof(gServerConfig.quizfile), NULL,
     0},
    {"lsgenlogfilename", gServerConfig.lsgenlog, sizeof(gServerConfig.lsgenlog),
     NULL, 0},
#ifdef _GMRELOAD
    {"gmsetfile", gServerConfig.gmsetfile, sizeof(gServerConfig.gmsetfile),
     NULL, 0},
#endif
    {"storedir", gServerConfig.storedir, sizeof(gServerConfig.storedir), NULL,
     0},
    {"npcdir", gServerConfig.npcdir, sizeof(gServerConfig.npcdir), NULL, 0},
    {"logdir", gServerConfig.logdir, sizeof(gServerConfig.logdir), NULL, 0},
    {"logconfname", gServerConfig.logconfname,
     sizeof(gServerConfig.logconfname), NULL, 0},
    {"chatmagicpasswd", gServerConfig.chatmagicpasswd,
     sizeof(gServerConfig.chatmagicpasswd), NULL, 0},
#ifdef _STORECHAR
    {"storechar", gServerConfig.storechar, sizeof(gServerConfig.storechar),
     NULL, 0},
#endif
    {"chatmagiccdkeycheck", NULL, 0, &gServerConfig.chatmagiccdkeycheck, INT},
    {"filesearchnum", NULL, 0, &gServerConfig.filesearchnum, INT},
    {"npctemplatenum", NULL, 0, &gServerConfig.npctemplatenum, INT},
    {"npccreatenum", NULL, 0, &gServerConfig.npccreatenum, INT},
    {"walkinterval", NULL, 0, (void *)&gServerConfig.walksendinterval, INT},
    {"CAinterval", NULL, 0, (void *)&gServerConfig.CAsendinterval_ms, INT},
    {"CDinterval", NULL, 0, (void *)&gServerConfig.CDsendinterval_ms, INT},
    {"CharSaveinterval", NULL, 0, (void *)&gServerConfig.CharSavesendinterval,
     INT},
    {"Onelooptime", NULL, 0, (void *)&gServerConfig.Onelooptime_ms, INT},
    {"Petdeletetime", NULL, 0, (void *)&gServerConfig.Petdeletetime, INT},
    {"Itemdeletetime", NULL, 0, (void *)&gServerConfig.Itemdeletetime, INT},
    {"addressbookoffmesgnum", NULL, 0,
     (void *)&gServerConfig.addressbookoffmsgnum, INT},
    {"protocolreadfrequency", NULL, 0,
     (void *)&gServerConfig.protocolreadfrequency, INT},
    {"allowerrornum", NULL, 0, (void *)&gServerConfig.allowerrornum, INT},
    {"loghour", NULL, 0, (void *)&gServerConfig.loghour, INT},
    {"battledebugmsg", NULL, 0, (void *)&gServerConfig.battledebugmsg, INT},
    // ttom add because the second had
    {"encodekey", NULL, 0, (void *)&gServerConfig.encodekey, INT},
    {"acwritesize", NULL, 0, (void *)&gServerConfig.acwritesize, INT},
    {"acwbsize", NULL, 0, (void *)&gServerConfig.acwbsize, INT},
    {"erruser_down", NULL, 0, (void *)&gServerConfig.ErrUserDownFlg, INT},

#ifdef _PROFESSION_SKILL // WON ADD
    {"profession", gServerConfig.profession, sizeof(gServerConfig.profession),
     NULL, 0},
#endif

#ifdef _ITEM_QUITPARTY
    {"itemquitparty", gServerConfig.itemquitparty,
     sizeof(gServerConfig.itemquitparty), NULL, 0},
#endif

#ifdef _DEL_DROP_GOLD
    {"Golddeletetime", NULL, 0, (void *)&gServerConfig.Golddeletetime, INT},
#endif

#ifdef _NEW_PLAYER_CF
    {"TRANS", NULL, 0, (void *)&gServerConfig.newplayertrans, INT},
    {"LV", NULL, 0, (void *)&gServerConfig.newplayerlv, INT},
    {"PET1", NULL, 0, (void *)&gServerConfig.newplayergivepet[1], INT},
    {"PET2", NULL, 0, (void *)&gServerConfig.newplayergivepet[2], INT},
    {"PET3", NULL, 0, (void *)&gServerConfig.newplayergivepet[3], INT},
    {"PET4", NULL, 0, (void *)&gServerConfig.newplayergivepet[4], INT},
    {"ITEM1", NULL, 0, (void *)&gServerConfig.newplayergiveitem[0], INT},
    {"ITEM2", NULL, 0, (void *)&gServerConfig.newplayergiveitem[1], INT},
    {"ITEM3", NULL, 0, (void *)&gServerConfig.newplayergiveitem[2], INT},
    {"ITEM4", NULL, 0, (void *)&gServerConfig.newplayergiveitem[3], INT},
    {"ITEM5", NULL, 0, (void *)&gServerConfig.newplayergiveitem[4], INT},
    {"ITEM6", NULL, 0, (void *)&gServerConfig.newplayergiveitem[5], INT},
    {"ITEM7", NULL, 0, (void *)&gServerConfig.newplayergiveitem[6], INT},
    {"ITEM8", NULL, 0, (void *)&gServerConfig.newplayergiveitem[7], INT},
    {"ITEM9", NULL, 0, (void *)&gServerConfig.newplayergiveitem[8], INT},
    {"ITEM10", NULL, 0, (void *)&gServerConfig.newplayergiveitem[9], INT},
    {"ITEM11", NULL, 0, (void *)&gServerConfig.newplayergiveitem[10], INT},
    {"ITEM12", NULL, 0, (void *)&gServerConfig.newplayergiveitem[11], INT},
    {"ITEM13", NULL, 0, (void *)&gServerConfig.newplayergiveitem[12], INT},
    {"ITEM14", NULL, 0, (void *)&gServerConfig.newplayergiveitem[13], INT},
    {"ITEM15", NULL, 0, (void *)&gServerConfig.newplayergiveitem[14], INT},
    {"PETLV", NULL, 0, (void *)&gServerConfig.newplayerpetlv, INT},
    {"GOLD", NULL, 0, (void *)&gServerConfig.newplayergivegold, INT},
    {"RIDEPETLEVEL", NULL, 0, (void *)&gServerConfig.ridepetlevel, INT},
#ifdef _VIP_SERVER
    {"GIVEVIPPOINT", NULL, 0, (void *)&gServerConfig.newplayerpetvip, INT},
#endif
#endif

#ifdef _USER_EXP_CF
    {"USEREXP", gServerConfig.expfile, sizeof(gServerConfig.expfile), NULL, 0},
#endif

#ifdef _UNLAW_WARP_FLOOR
    {"UNLAWWARPFLOOR", gServerConfig.unlawwarpfloor,
     sizeof(gServerConfig.unlawwarpfloor), NULL, 0},
#endif

#ifdef _NO_JOIN_FLOOR
    {"NOJOINFLOOR", gServerConfig.nojoinfloor,
     sizeof(gServerConfig.nojoinfloor), NULL, 0},
#endif

#ifdef _WATCH_FLOOR
    {"WATCHFLOOR", NULL, 0, (void *)&gServerConfig.watchfloor[0], INT},
    {"WATCHFLOOR1", NULL, 0, (void *)&gServerConfig.watchfloor[1], INT},
    {"WATCHFLOOR2", NULL, 0, (void *)&gServerConfig.watchfloor[2], INT},
    {"WATCHFLOOR3", NULL, 0, (void *)&gServerConfig.watchfloor[3], INT},
    {"WATCHFLOOR4", NULL, 0, (void *)&gServerConfig.watchfloor[4], INT},
    {"WATCHFLOOR5", NULL, 0, (void *)&gServerConfig.watchfloor[5], INT},
#endif

#ifdef _BATTLE_FLOOR
    {"BATTLEFLOOR", NULL, 0, (void *)&gServerConfig.battlefloor, INT},
    {"BATTLEFLOORCF", gServerConfig.battlefloorcf,
     sizeof(gServerConfig.battlefloorcf), NULL, 0},
#endif

#ifdef _UNREG_NEMA
    {"NAME1", gServerConfig.unregname[0], sizeof(gServerConfig.unregname[0]),
     NULL, 0},
    {"NAME2", gServerConfig.unregname[1], sizeof(gServerConfig.unregname[1]),
     NULL, 0},
    {"NAME3", gServerConfig.unregname[2], sizeof(gServerConfig.unregname[2]),
     NULL, 0},
    {"NAME4", gServerConfig.unregname[3], sizeof(gServerConfig.unregname[3]),
     NULL, 0},
    {"NAME5", gServerConfig.unregname[4], sizeof(gServerConfig.unregname[4]),
     NULL, 0},
#endif
#ifdef _TRANS_LEVEL_CF
    {"CHARTRANS", NULL, 0, (void *)&gServerConfig.chartrans, INT},
    {"PETTRANS", NULL, 0, (void *)&gServerConfig.pettrans, INT},
    {"LEVEL", NULL, 0, (void *)&gServerConfig.yblevel, INT},
    {"MAXLEVEL", NULL, 0, (void *)&gServerConfig.maxlevel, INT},
#endif
#ifdef _POINT
    {"POINT", NULL, 0, (void *)&gServerConfig.point, INT},
    {"TRANS0", NULL, 0, (void *)&gServerConfig.transpoint[0], INT},
    {"TRANS1", NULL, 0, (void *)&gServerConfig.transpoint[1], INT},
    {"TRANS2", NULL, 0, (void *)&gServerConfig.transpoint[2], INT},
    {"TRANS3", NULL, 0, (void *)&gServerConfig.transpoint[3], INT},
    {"TRANS4", NULL, 0, (void *)&gServerConfig.transpoint[4], INT},
    {"TRANS5", NULL, 0, (void *)&gServerConfig.transpoint[5], INT},
    {"TRANS6", NULL, 0, (void *)&gServerConfig.transpoint[6], INT},
    {"TRANS7", NULL, 0, (void *)&gServerConfig.transpoint[7], INT},
#endif

#ifdef _PET_AND_ITEM_UP
    {"PETUP", NULL, 0, (void *)&gServerConfig.petup, INT},
    {"ITEMUP", NULL, 0, (void *)&gServerConfig.itemup, INT},
#endif
#ifdef _LOOP_ANNOUNCE
    {"ANNOUNCEPATH", gServerConfig.loopannouncepath,
     sizeof(gServerConfig.loopannouncepath), NULL, 0},
    {"ANNOUNCETIME", NULL, 0, (void *)&gServerConfig.loopannouncetime, INT},
#endif
#ifdef _SKILLUPPOINT_CF
    {"SKILLUPPOINT", NULL, 0, (void *)&gServerConfig.skup, INT},
#endif
#ifdef _RIDELEVEL
    {"RIDELEVEL", NULL, 0, (void *)&gServerConfig.ridelevel, INT},
    {"RIDETRANS", NULL, 0, (void *)&gServerConfig.ridetrans, INT},
#endif

#ifdef _REVLEVEL
    {"REVLEVEL", NULL, 0, (void *)&gServerConfig.revlevel, INT},
#endif
#ifdef _NEW_PLAYER_RIDE
    {"NPRIDE", NULL, 0, (void *)&gServerConfig.npride, INT},
#endif
#ifdef _FIX_CHARLOOPS
    {"CHARLOOPS", NULL, 0, (void *)&gServerConfig.charloops, INT},
#endif
#ifdef _PLAYER_ANNOUNCE
    {"PANNOUNCE", NULL, 0, (void *)&gServerConfig.pannounce, INT},
#endif
#ifdef _PLAYER_MOVE
    {"PMOVE", NULL, 0, (void *)&gServerConfig.pmove, INT},
#endif

    {"recvbuffer", NULL, 0, (void *)&gServerConfig.recvbuffer, INT},
    {"sendbuffer", NULL, 0, (void *)&gServerConfig.sendbuffer, INT},
    {"recvlowatbuffer", NULL, 0, (void *)&gServerConfig.recvlowatbuffer, INT},
    {"runlevel", NULL, 0, (void *)&gServerConfig.runlevel, INT},

#ifdef _SHOW_VIP_CF
    {"SHOWVIP", NULL, 0, (void *)&gServerConfig.showvip, INT},
#endif

#ifdef _PLAYER_NUM
    {"PLAYERNUM", NULL, 0, (void *)&gServerConfig.playernum, INT},
#endif

#ifdef _BATTLE_GOLD
    {"BATTLEGOLD", NULL, 0, (void *)&gServerConfig.battlegold, INT},
#endif
#ifdef _ANGEL_TIME
    {"ANGELPLAYERTIME", NULL, 0, (void *)&gServerConfig.angelplayertime, INT},
    {"ANGELPLAYERMUN", NULL, 0, (void *)&gServerConfig.angelplayermun, INT},
#endif
#ifdef _RIDEMODE_20
    {"RIDEMODE", NULL, 0, (void *)&gServerConfig.ridemode, INT},
#endif
#ifdef _FM_POINT_PK
    {"FMPOINTPK", NULL, 0, (void *)&gServerConfig.fmpointpk, INT},
#endif
#ifdef _ENEMY_ACTION
    {"ENEMYACTION", NULL, 0, (void *)&gServerConfig.enemyact, INT},
#endif
#ifdef _FUSIONBEIT_TRANS
    {"FUSIONBEIT", NULL, 0, (void *)&gServerConfig.fusionbeittrans, INT},
#endif
#ifdef _CHECK_PEPEAT
    {"CHECKPEPEAT", NULL, 0, (void *)&gServerConfig.CheckRepeat, INT},
#endif
    {"CPUUSE", NULL, 0, (void *)&gServerConfig.cpuuse, INT},
#ifdef _FM_JOINLIMIT
    {"JOINFAMILYTIME", NULL, 0, (void *)&gServerConfig.joinfamilytime, INT},
#endif
#ifdef _MAP_HEALERALLHEAL
    {"MAPHEAL", gServerConfig.mapheal, sizeof(gServerConfig.mapheal), NULL, 0},
#endif

#ifdef _THE_WORLD_SEND
    {"THEWORLDTRANS", NULL, 0, (void *)&gServerConfig.thewordtrans, INT},
    {"THEWORLDLEVEL", NULL, 0, (void *)&gServerConfig.thewordlevel, INT},
    {"THEWORLDSNED", NULL, 0, (void *)&gServerConfig.thewordsend, INT},
#endif
#ifdef _LOGIN_DISPLAY
    {"LOGINDISPLAY", NULL, 0, (void *)&gServerConfig.logindisplay, INT},
#endif
#ifdef _VIP_POINT_PK
    {"PKMAP", gServerConfig.vippointpk, sizeof(gServerConfig.vippointpk), NULL,
     0},
    {"PKMAPCOST", NULL, 0, (void *)&gServerConfig.vippointpkcost, INT},
#endif
#ifdef _SPECIAL_MAP
    {"SPECIALMAP", gServerConfig.specialmap, sizeof(gServerConfig.specialmap),
     NULL, 0},
#endif
#ifdef _NEW_AUTO_PK
    {"AUTOPK", NULL, 0, (void *)&gServerConfig.autopk, INT},
    {"AUTOPKTRANS", NULL, 0, (void *)&gServerConfig.autopktrans, INT},
    {"AUTOPKLV", NULL, 0, (void *)&gServerConfig.autopklv, INT},
#ifdef _FORMULATE_AUTO_PK
    {"AUTOPKPOINT", NULL, 0, (void *)&gServerConfig.autopkpoint, INT},
    {"KILLPOINT", NULL, 0, (void *)&gServerConfig.killpoint, INT},
#endif
#endif
#ifdef _AUTO_DEL_PET
    {"AUTODELPET", gServerConfig.autodelpet, sizeof(gServerConfig.autodelpet),
     NULL, 0},
#endif
#ifdef _AUTO_DEL_ITEM
    {"AUTODELITEM", gServerConfig.autodelitem,
     sizeof(gServerConfig.autodelitem), NULL, 0},
#endif
#ifdef _BT_PET
    {"BTPET", NULL, 0, (void *)&gServerConfig.btpet, INT},
#endif
#ifdef _BT_ITEM
    {"BTITEM", NULL, 0, (void *)&gServerConfig.btitem, INT},
#endif
#ifdef _LUCK_STAR
    {"LUCKSTARTIME", NULL, 0, (void *)&gServerConfig.luckstartime, INT},
    {"LUCKSTARCHANCES", NULL, 0, (void *)&gServerConfig.luckstarchances, INT},
#endif
#ifdef _BATTLE_GETITEM_RATE
    {"BATTLEGETITEMRATE", gServerConfig.battlegetitemrate,
     sizeof(gServerConfig.battlegetitemrate), NULL, 0},
    {"BATTLEGETITEMRATEMAP", NULL, 0,
     (void *)&gServerConfig.battlegetitemratemap, INT},
#endif
#ifdef _UNLAW_THIS_LOGOUT
    {"UNLAWTHISLOGOUT", gServerConfig.unlawthislogout,
     sizeof(gServerConfig.unlawthislogout), NULL, 0},
#endif
#ifdef _TRANS_POINT_UP
    {"TRANSPOINTUP", gServerConfig.transpointup,
     sizeof(gServerConfig.transpointup), NULL, 0},
#endif
#ifdef _OPEN_STW_SEND
    {"STWSENDTYPE", NULL, 0, (void *)&gServerConfig.stwsendtype, INT},
    {"STWSENDPOINT", NULL, 0, (void *)&gServerConfig.stwsendpoint, INT},
#endif
#ifdef _POOL_ITEM_BUG
    {"POOLITEMBUG", NULL, 0, (void *)&gServerConfig.poolitembug, INT},
    {"POOLITEM", gServerConfig.poolitem, sizeof(gServerConfig.poolitem), NULL,
     0},
#endif
#ifdef _NO_STW_ENEMY
    {"NOSTWENEMY", NULL, 0, (void *)&gServerConfig.nostwenemy, INT},
    {"NOSTWENEMYGOLD", NULL, 0, (void *)&gServerConfig.nostwenemypoint, INT},
#endif
#ifdef _NEW_STREET_VENDOR
    {"STREETVENDORPOINT", gServerConfig.streetvendorpoint,
     sizeof(gServerConfig.streetvendorpoint), NULL, 0},
#endif
#ifdef _ITEM_PET_LOCKED
    {"ITEMPETLOCKED", NULL, 0, (void *)&gServerConfig.itampetlocked, INT},
#endif
#ifdef _TALK_SAVE
    {"SAVEFAME", NULL, 0, (void *)&gServerConfig.savefame, INT},
#endif
#ifdef _TALK_CHECK
    {"TALKCHECKMAX", NULL, 0, (void *)&gServerConfig.talkcheckmax, INT},
    {"TALKCHECKMIN", NULL, 0, (void *)&gServerConfig.talkcheckmin, INT},
#endif
#ifdef _DISABLE_PROFESSION_SKILL
    {"DISABLEPROFESSION", gServerConfig.disableprofessionskill,
     sizeof(gServerConfig.disableprofessionskill), NULL, 0},
#endif
#ifdef _ALL_SERV_SEND
    {"ALLSERVTRANS", NULL, 0, (void *)&gServerConfig.allservtrans, INT},
    {"ALLSERVLEVEL", NULL, 0, (void *)&gServerConfig.allservlevel, INT},
    {"ALLSERVSNED", NULL, 0, (void *)&gServerConfig.allservsend, INT},
#endif
#ifdef _PET_TRANS_ABILITY
    {"PETTRANSABILITY", NULL, 0, (void *)&gServerConfig.pettransability, INT},
    {"PETTRANSABILITY1", NULL, 0, (void *)&gServerConfig.pettransability1, INT},
    {"PETTRANSABILITY2", NULL, 0, (void *)&gServerConfig.pettransability2, INT},
#endif
#ifdef _NEED_ITEM_ENEMY
    {"DELNEEDITEM", NULL, 0, (void *)&gServerConfig.delneeditem, INT},
#endif
#ifdef _NOT_ESCAPE
    {"NOTESCAPE", gServerConfig.notescape, sizeof(gServerConfig.notescape),
     NULL, 0},
#endif
#ifdef _PLAYER_OVERLAP_PK
    {"PLAYEROVERLAPPK", gServerConfig.playeroverlappk,
     sizeof(gServerConfig.playeroverlappk), NULL, 0},
#endif
#ifdef _FIMALY_PK_TIME
    {"FIMALYPKTIME", NULL, 0, (void *)&gServerConfig.fimalypktime, INT},
#endif
#ifdef _PETSKILL_SHOP_LUA
    {"PETSKILLSHOPPATH", gServerConfig.freepetskillshoppath,
     sizeof(gServerConfig.freepetskillshoppath), NULL, 0},
#endif
#ifdef _CANCEL_ANGLE_TRANS
    {"CANCELANGLETRANS", NULL, 0, (void *)&gServerConfig.cancelanlgetrans, INT},
#endif
#ifdef _VIP_BATTLE_EXP
    {"vipbattleexp", NULL, 0, (void *)&gServerConfig.vipbattleexp, INT},
#endif
#ifdef _NO_HELP_MAP
    {"nohelpmap", gServerConfig.nohelpmap, sizeof(gServerConfig.nohelpmap),
     NULL, 0},
#endif
#ifdef _BATTLE_TIME
    {"battletime", NULL, 0, (void *)&gServerConfig.battletime, INT},
#endif
#ifdef _SAME_IP_ONLINE_NUM
    {"sameiponlinenum", NULL, 0, (void *)&gServerConfig.sameiponlinenum, INT},
#endif
#ifdef _STREET_VENDOR_TRANS
    {"streetvendortrans", NULL, 0, (void *)&gServerConfig.streetvendortrans,
     INT},
#endif
#ifdef _CHECK_SEVER_IP
    {"serverip", gServerConfig.serverip, sizeof(gServerConfig.serverip), NULL,
     0},
#endif
#ifdef _DAMMAGE_CALC
    {"dammagecalc", NULL, 0, (void *)&gServerConfig.dammagecalc, INT},
#endif
#ifdef _PET_ENEMY_DEVELOP_UP
    {"PetEnemyDevelopUp", NULL, 0, (void *)&gServerConfig.PetEnemyDevelopUp,
     INT},
#endif
#ifdef _FIRST_LOCK_ITEM
    {"FirstLockItem", gServerConfig.FirstLockItem,
     sizeof(gServerConfig.FirstLockItem), NULL, 0},
#endif
    {"Connectnum", NULL, 0, (void *)&gServerConfig.Connectnum, INT},
#ifdef _PETSKILL_NEW_PASSIVE
    {"AUTUGETSKILL", NULL, 0, (void *)&gServerConfig.autogetskill, INT},
    {"GETSKILLPOS", NULL, 0, (void *)&gServerConfig.getskillpos, INT},
    {"SKILLINFOLV1", gServerConfig.skillinfolv[0],
     sizeof(gServerConfig.skillinfolv[0]), NULL, 0},
    {"SKILLINFOLV2", gServerConfig.skillinfolv[1],
     sizeof(gServerConfig.skillinfolv[1]), NULL, 0},
    {"SKILLINFOLV3", gServerConfig.skillinfolv[2],
     sizeof(gServerConfig.skillinfolv[2]), NULL, 0},
    {"SKILLINFOLV4", gServerConfig.skillinfolv[3],
     sizeof(gServerConfig.skillinfolv[3]), NULL, 0},
    {"SKILLINFOLV5", gServerConfig.skillinfolv[4],
     sizeof(gServerConfig.skillinfolv[4]), NULL, 0},
    {"FUSIONRANGE", gServerConfig.fusionrange,
     sizeof(gServerConfig.fusionrange), NULL, 0},
    {"SKILLFUSION", NULL, 0, (void *)&gServerConfig.skillfusion, INT},
    {"SKILLCOUNT", NULL, 0, (void *)&gServerConfig.skillcount, INT},
#endif
#ifdef _SHARE_EXP
    {"EXPSHARE", NULL, 0, (void *)&gServerConfig.expshare, INT},
#endif
#ifdef _DEFEND_BIGBAO
    {"BIGBAO", NULL, 0, (void *)&gServerConfig.bigbao, INT},
    {"BIGBAO2", NULL, 0, (void *)&gServerConfig.bigbao2, INT},
#endif
#ifdef _MO_SHOW_DEBUG
    {"ISDEBUG", NULL, 0, (void *)&gServerConfig.isdebug, INT},
#endif
#ifdef _CHAR_LOOP_TIME
    {"charlooptime", NULL, 0, (void *)&gServerConfig.charlooptime, INT},
#endif
#ifdef _MO_RELOAD_NPC
    {"RELOADNPCTIME", NULL, 0, (void *)&gServerConfig.reloadnpctime, INT},
    {"RELOADNPCTYPE", NULL, 0, (void *)&gServerConfig.reloadnpctype, INT},
#endif
#ifdef _JZ_NEWSCRIPT_LUA
    {"LUAFILE", gServerConfig.luafile, sizeof(gServerConfig.luafile), NULL, 0},
#endif
#ifdef _ITEM_LUA
    {"ITEMLUAFILE", gServerConfig.itemluafile,
     sizeof(gServerConfig.itemluafile), NULL, 0},
#endif
#ifdef _ROOKIE_ITEM
    {"ROOKIEITEM", NULL, 0, (void *)&gServerConfig.rookieitem[0], INT},
    {"ROOKIEITEM2", NULL, 0, (void *)&gServerConfig.rookieitem[1], INT},
    {"ROOKIEITEM3", NULL, 0, (void *)&gServerConfig.rookieitem[2], INT},
    {"ROOKIEITEM4", NULL, 0, (void *)&gServerConfig.rookieitem[3], INT},
    {"ROOKIEITEM5", NULL, 0, (void *)&gServerConfig.rookieitem[4], INT},
#endif
#ifdef _NO_TRANS_ITEM
    {"NOTRANSITEM", NULL, 0, (void *)&gServerConfig.notransitem, INT},
#endif
#ifdef _MAX_MERGE_LEVEL
    {"MAXMERGELEVEL", NULL, 0, (void *)&gServerConfig.maxmergelevel, INT},
#endif
#ifdef _NO_ATTACK
    {"ATTTIME", NULL, 0, (void *)&gServerConfig.atttime, INT},
    {"ATTSAFETIME", NULL, 0, (void *)&gServerConfig.attsafetime, INT},
    {"ATTCNT", NULL, 0, (void *)&gServerConfig.attcnt, INT},
    {"LATETIME", NULL, 0, (void *)&gServerConfig.latetime, INT},
    {"ATTDMETIME", NULL, 0, (void *)&gServerConfig.attdmetime, INT},
    {"ATTDMECNT", NULL, 0, (void *)&gServerConfig.attdmecnt, INT},
#endif
    {"NOATTIP1", gServerConfig.noattip[0], sizeof(gServerConfig.noattip[0]),
     NULL, 0},
    {"NOATTIP2", gServerConfig.noattip[1], sizeof(gServerConfig.noattip[1]),
     NULL, 0},
    {"NOATTIP3", gServerConfig.noattip[2], sizeof(gServerConfig.noattip[2]),
     NULL, 0},
    {"NOATTIP4", gServerConfig.noattip[3], sizeof(gServerConfig.noattip[3]),
     NULL, 0},
    {"NOATTIP5", gServerConfig.noattip[4], sizeof(gServerConfig.noattip[4]),
     NULL, 0},
#ifdef _NO_FULLPLAYER_ATT
    {"NOFULLPLAYER", NULL, 0, (void *)&gServerConfig.nofullplayer, INT},
    {"NOFULL2PLAYER", NULL, 0, (void *)&gServerConfig.nofull2player, INT},
    {"NOCDKEYPLAYER", NULL, 0, (void *)&gServerConfig.nocdkeyplayer, INT},
    {"NOCDKEYMODE", NULL, 0, (void *)&gServerConfig.nocdkeymode, INT},
    {"NOCDKEYTYPE", NULL, 0, (void *)&gServerConfig.nocdkeytype, INT},
    {"NOFULLTIME", NULL, 0, (void *)&gServerConfig.nofulltime, INT},
    {"FENGTYPE", NULL, 0, (void *)&gServerConfig.fengtype, INT},
    {"NOFULLENDPLAYER", NULL, 0, (void *)&gServerConfig.nofullendplayer, INT},
    {"NOFULLENDTIME", NULL, 0, (void *)&gServerConfig.nofullendtime, INT},
    {"MANRENNUM", NULL, 0, (void *)&gServerConfig.manrennum, INT},
#endif
    {"LOCKTYPE", NULL, 0, (void *)&gServerConfig.locktype, INT},
#ifdef _NEW_FUNC_DECRYPT
    {"ALLOWERRORNUM2", NULL, 0, (void *)&gServerConfig.allowerrornum2, INT},
#endif
#ifdef _MO_LOGIN_NO_KICK
    {"LOGINNOKICK", NULL, 0, (void *)&gServerConfig.loginnokick, INT},
#endif
#ifdef _MO_ILLEGAL_NAME
    {"ILLEGALNAME", gServerConfig.illegalname,
     sizeof(gServerConfig.illegalname), NULL, 0},
#endif
#ifdef _NO_USE_PACKET_MAP
    {"NOPACKETMAP", gServerConfig.nousepacketmap,
     sizeof(gServerConfig.nousepacketmap), NULL, 0},
#endif
#ifdef _NO_USE_MAGIC_MAP
    {"NOMAGICMAP", gServerConfig.nousemagicmap,
     sizeof(gServerConfig.nousemagicmap), NULL, 0},
#endif
#ifdef _SOME_PETMAIL
    {"PETMAILFLG", NULL, 0, (void *)&gServerConfig.petmailflg, INT},
    {"SOMEPETMAIL", gServerConfig.somepetmail,
     sizeof(gServerConfig.somepetmail), NULL, 0},
#endif
#ifdef _CTRL_TRANS_DEVELOP
    {"CTRLTRANS", NULL, 0, (void *)&gServerConfig.ctrltrans, INT},
#endif
#ifdef _PETMAIL_TIME
    {"PETMAILTIME", NULL, 0, (void *)&gServerConfig.petmailtime, INT},
#endif
#ifdef _UP_BBPETPROB
    {"UPBBPROB", NULL, 0, (void *)&gServerConfig.upbbprob, INT},
#endif

};

// Arminius 7.12 login announce
char announcetext[8192];
void AnnounceToPlayer(int char_index) {
  char *ptr, *qtr;

  ptr = announcetext;
  while ((qtr = strstr(ptr, "\n")) != NULL) {
    qtr[0] = '\0';
    //    printf("ptr=%s\n",ptr);
    CHAR_talkToCli(char_index, -1, ptr, CHAR_COLORYELLOW);
    qtr[0] = '\n';
    ptr = qtr + 1;
  }
  CHAR_talkToCli(char_index, -1, ptr, CHAR_COLORYELLOW);
}

// Robin 0720
void AnnounceToPlayerWN(int fd) {
  char buf[8192];
#ifdef _VIP_LOGOUT
  char token[8192];
  int char_index = CONNECT_getCharaindex(fd);
  long lastleavetime = CHAR_getInt(char_index, CHAR_LASTLEAVETIME);
  struct tm *p;
  p = localtime(&lastleavetime);

  sprintf(token, "您最后离线时间 %d年%d月%d日 %d:%d:%d\n\n%s", p->tm_year + 1900,
          p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,
          announcetext);
  GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_LOGINMESSAGE, WINDOW_BUTTONTYPE_OK,
                     -1, -1, makeEscapeString(token, buf, sizeof(buf)));
#else
  GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_LOGINMESSAGE, WINDOW_BUTTONTYPE_OK,
                     -1, -1, makeEscapeString(announcetext, buf, sizeof(buf)));
#endif
}

void LoadAnnounce(void) {
  FILE *f;

  memset(announcetext, 0, sizeof(announcetext));
  if ((f = fopen("./announce.txt", "r")) != NULL) {
    fread(announcetext, sizeof(announcetext), 1, f);
    announcetext[sizeof(announcetext) - 1] = '\0';
    fclose(f);
  }
}
#ifdef _PET_TALKPRO
PTALK pettalktext[PETTALK_MAXID];

void LoadPetTalk(void) {
  FILE *fp;
  char fn[256];
  char line[4096];
  char talkmem[4096];
  int maxid = 0;
  char buf1[256], buf2[256], buf3[256];
  int talkNO = -1, mark = -1, i;
  int len = sizeof(talkmem);

  memset(talkmem, 0, sizeof(talkmem));
  sprintf(fn, "%s/pettalk/pettalk.menu", getNpcdir());

  for (i = 0; i < PETTALK_MAXID; i++) {
    pettalktext[i].ID = -1;
    strcpy(pettalktext[i].DATA, "\0");
  }

  print("\n装载宠物对话文件:%s...", fn);

#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", fn);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(fn, "r");
  }
  if (fp != NULL) {
    while (fgets(line, sizeof(line), fp)) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(line);
      }
#endif
      if (strlen(talkmem) != 0) {
        if (talkmem[strlen(talkmem) - 1] != '|') {
          util_strcatsafe(talkmem, len, "|");
        }
      }
      chompex(line);
      util_strcatsafe(talkmem, len, line);
    }
    fclose(fp);
  } else {
    print("错误:找不到文件!");
  }

  talkNO = 1;
  while (getStringFromIndexWithDelim(talkmem, "END", talkNO, buf1,
                                     sizeof(buf1)) != FALSE) {
    talkNO++;
    if (NPC_Util_GetStrFromStrWithDelim(buf1, "PETTEMPNO", buf2,
                                        sizeof(buf2)) == NULL)
      continue;
    mark = 1;
    strcpy(fn, "\0");

    if (getStringFromIndexWithDelim(buf2, ",", mark + 1, buf3, sizeof(buf3)) !=
        FALSE) {
      pettalktext[maxid].ID = atoi(buf3);
      if (getStringFromIndexWithDelim(buf2, ",", mark, buf3, sizeof(buf3)) !=
          FALSE) {
        sprintf(fn, "%s/pettalk/%s", getNpcdir(), buf3);
        // print("\n ...file:%s", fn);
#ifdef _CRYPTO_DATA
        char realopfile[256];
        BOOL crypto = FALSE;
        sprintf(realopfile, "%s.allblues", fn);
        fp = fopen(realopfile, "r");
        if (fp != NULL) {
          crypto = TRUE;
        } else
#endif
        {
          fp = fopen(fn, "r");
        }
        if (fp != NULL) {
          char line[4096];
          while (fgets(line, sizeof(line), fp)) {
#ifdef _CRYPTO_DATA
            if (crypto == TRUE) {
              DecryptKey(line);
            }
#endif
            if (strlen(pettalktext[maxid].DATA) != 0) {
              if (pettalktext[maxid]
                      .DATA[strlen(pettalktext[maxid].DATA) - 1] != '|') {
                util_strcatsafe(pettalktext[maxid].DATA,
                                sizeof(pettalktext[maxid].DATA), "|");
              }
            }
            chompex(line);
            util_strcatsafe(pettalktext[maxid].DATA,
                            sizeof(pettalktext[maxid].DATA), line);
          }
          maxid++;
          fclose(fp);
        } else {
          print("错误:[%s] 找不到!", fn);
          pettalktext[maxid].ID = -1;
        }
      } else {
        pettalktext[maxid].ID = -1;
      }
    }
    print(".");
    if (maxid >= PETTALK_MAXID)
      break;
  }
  print("最大ID=%d...", maxid);
  {
    int haveid = 0;
    for (i = 0; i < PETTALK_MAXID; i++) {
      if (pettalktext[i].ID >= 0) {
        haveid++;
      }
    }
    print("载入总数=%d", haveid);
  }
}

#else
char pettalktext[4096];
void LoadPetTalk(void) {
  FILE *fp;
  char fn[256];
  char line[4096];
  int len = sizeof(pettalktext);

  memset(pettalktext, 0, sizeof(pettalktext));
  sprintf(fn, "%s/pettalk/pettalk.mem", getNpcdir());
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", fn);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(fn, "r");
  }
  if (fp != NULL) {
    print("\n\n pettalk.mem");
    while (fgets(line, sizeof(line), fp)) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(line);
      }
#endif
      if (strlen(pettalktext) != 0) {
        if (pettalktext[strlen(pettalktext) - 1] != '|') {
          util_strcatsafe(pettalktext, len, "|");
        }
      }
      chompex(line);
      util_strcatsafe(pettalktext, len, line);
    }
    fclose(fp);
    print("\n %s", pettalktext);
  } else {
    print("\n 不能找到 pettalk.mem");
  }
}
#endif

#ifdef _GAMBLE_BANK
GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems(void) {

  FILE *fp;
  char filename[256];
  char buf1[256];
  char name[128];
  int num, ID, type;
  int i = 0;
  sprintf(filename, "./data/gambleitems.txt");
  print("\n加载赌博物品文件 %s ...", filename);
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(filename, "r");
  }
  if (fp != NULL) {
    while (fgets(buf1, sizeof(buf1), fp) != NULL) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(buf1);
      }
#endif
      if (strstr(buf1, "#") != 0)
        continue;
      sscanf(buf1, "%s %d %d %d", name, &ID, &num, &type);
      strcpy(GB_ITEMS[i].name, name);
      GB_ITEMS[i].Gnum = num;
      GB_ITEMS[i].ItemId = ID;
      GB_ITEMS[i].type = type;
      i++;
    }
    print("最大ID: %d ", i);
    fclose(fp);
  } else {
    print("错误 找不到文件 %s", filename);
  }
}
#endif

#ifdef _CFREE_petskill
PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes(void) {
  FILE *fp;
  char filename[256];
  char buf1[256];
  char name[128];
  char type[256];
  int num, ID;
  int i = 0;
  sprintf(filename, "./data/skillcode.txt");
  print("\n加载宠物技能编码文件:%s...", filename);
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(filename, "r");
  }
  if (fp != NULL) {
    while (fgets(buf1, sizeof(buf1), fp) != NULL) {
#ifdef _CRYPTO_DATA
      if (crypto == TRUE) {
        DecryptKey(buf1);
      }
#endif
      sscanf(buf1, "%s %d %d %s", name, &num, &ID, type);
      strcpy(Code_skill[i].name, name);
      Code_skill[i].TempNo = num;
      Code_skill[i].PetId = ID;
      strcpy(Code_skill[i].Code, type);
      i++;
      if (i >= PETSKILL_CODE)
        break;
    }
    fclose(fp);
  } else {
    print("打不到文件 %s", filename);
  }
  print("完成\n");
}
#endif

#ifdef _GMRELOAD
BOOL LoadGMSet(char *filename) {
  FILE *fp;
  int i = 0, gm_num = 0;

#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(filename, "r");
  }
  if (fp == NULL) {
    print("无法打开文件\n");
    return FALSE;
  }
  for (i = 0; i < GMMAXNUM; i++) {
    strcpy(gminfo[i].cdkey, "");
    gminfo[i].level = 0;
  }
  while (1) {
    char line[64], cdkey[64], level[64];
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    chop(line);
    if (line[0] == '#')
      continue;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }
    gm_num = gm_num + 1;
    if (gm_num > GMMAXNUM)
      break;
    easyGetTokenFromString(line, 1, cdkey, sizeof(cdkey));
    if (strcmp(cdkey, "") == 0)
      break;
    strncpy(gminfo[gm_num].cdkey, cdkey, sizeof(gminfo[gm_num].cdkey));
    easyGetTokenFromString(line, 2, level, sizeof(level));
    if (strcmp(level, "") == 0)
      break;
    gminfo[gm_num].level = atoi(level);
  }
  fclose(fp);
  return TRUE;
}
#endif

char *getProgramName(void) { return gServerConfig.program_name; }

char *getConfigFilename(void) { return gServerConfig.config_filename; }

void setConfigFilename(const char *config_filename) {
  strcpysafe(gServerConfig.config_filename,
             sizeof(gServerConfig.config_filename), config_filename);
}

unsigned getDebugLevel(void) { return gServerConfig.debug_level; }

unsigned setDebugLevel(unsigned debug_level) {
  unsigned old_level = gServerConfig.debug_level;
  gServerConfig.debug_level = debug_level;
  return old_level;
}

unsigned getMemoryUnitSize(void) { return gServerConfig.memory_unit; }

unsigned getMemoryUnitNum(void) { return gServerConfig.memory_unitnum; }

char *getAccountServerName(void) { return gServerConfig.account_server_name; }

unsigned short getAccountServerPort(void) {
  return gServerConfig.account_server_port;
}

char *getAccountServerPassword(void) {
  return gServerConfig.account_server_password;
}

char *getGameServerName(void) { return gServerConfig.game_server_name; }

char *getGameServerID(void) {
  const int idx = strlen(gServerConfig.game_server_id) - 1;
  if (gServerConfig.game_server_id[idx] == '\n')
    gServerConfig.game_server_id[idx] = '\0';
  return gServerConfig.game_server_id;
}

unsigned short getAllowManorPK(void) { return gServerConfig.allowmanorpk; }

unsigned short getPortNumber(void) { return gServerConfig.port; }

int getServernumber(void) { return gServerConfig.servernumber; }

int getReuseAddr(void) { return gServerConfig.reuse_addr; }

int getNodelay(void) { return gServerConfig.do_nodelay; }
int getLogWriteTime(void) { return gServerConfig.log_write_time; }
int getLogIOTime(void) { return gServerConfig.log_io_time; }
int getLogGameTime(void) { return gServerConfig.log_game_time; }
int getLogNetloopFaster(void) { return gServerConfig.log_netloop_faster; }

int getSaacwritenum(void) { return gServerConfig.saacwritenum; }

void setSaacwritenum(int num) { gServerConfig.saacwritenum = num; }

int getSaacreadnum(void) { return gServerConfig.saacreadnum; }

void setSaacreadnum(int num) { gServerConfig.saacreadnum = num; }

unsigned int getFdnum(void) { return gServerConfig.fdnum; }

unsigned int getPlayercharnum(void) { return gServerConfig.charnum; }

unsigned int getPetcharnum(void) { return gServerConfig.petcharnum; }

unsigned int getOtherscharnum(void) { return gServerConfig.othercharnum; }

unsigned int getObjnum(void) { return gServerConfig.objnum; }

unsigned int getItemnum(void) { return gServerConfig.itemnum; }

unsigned int getBattlenum(void) { return gServerConfig.battlenum; }

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp(void) { return gServerConfig.battleexp; }
void setBattleexp(int exp) {
  gServerConfig.battleexp = exp;
  return;
}
#else
unsigned int getBattleexp(void) { return 1; }
#endif

char *getTopdir(void) { return gServerConfig.topdir; }

char *getMapdir(void) { return gServerConfig.mapdir; }

char *getMaptilefile(void) { return gServerConfig.maptilefile; }

char *getBattleMapfile(void) { return gServerConfig.battlemapfile; }

char *getItemfile(void) { return gServerConfig.itemfile; }

char *getInvfile(void) { return gServerConfig.invfile; }

char *getAppearfile(void) { return gServerConfig.appearfile; }

char *getEffectfile(void) { return gServerConfig.effectfile; }

char *getTitleNamefile(void) { return gServerConfig.titlenamefile; }

char *getTitleConfigfile(void) { return gServerConfig.titleconfigfile; }

char *getEncountfile(void) { return gServerConfig.encountfile; }

char *getEnemyfile(void) { return gServerConfig.enemyfile; }

char *getEnemyBasefile(void) { return gServerConfig.enemybasefile; }

char *getGroupfile(void) { return gServerConfig.groupfile; }

char *getMagicfile(void) { return gServerConfig.magicfile; }

#ifdef _ATTACK_MAGIC

char *getAttMagicfileName(void) { return gServerConfig.attmagicfile; }

#endif

char *getPetskillfile(void) { return gServerConfig.petskillfile; }

#ifdef _PROFESSION_SKILL // WON ADD
char *getProfession(void) { return gServerConfig.profession; }
#endif

#ifdef _ITEM_QUITPARTY
char *getitemquitparty(void) { return gServerConfig.itemquitparty; }
#endif

char *getItematomfile(void) { return gServerConfig.itematomfile; }

char *getQuizfile(void) { return gServerConfig.quizfile; }

char *getLsgenlogfilename(void) { return gServerConfig.lsgenlog; }

#ifdef _GMRELOAD
char *getGMSetfile(void) { return gServerConfig.gmsetfile; }
#endif

char *getStoredir(void) { return gServerConfig.storedir; }

#ifdef _STORECHAR
char *getStorechar(void) { return gServerConfig.storechar; }
#endif

char *getNpcdir(void) { return gServerConfig.npcdir; }

char *getLogdir(void) { return gServerConfig.logdir; }

char *getLogconffile(void) { return gServerConfig.logconfname; }

char *getChatMagicPasswd(void) { return gServerConfig.chatmagicpasswd; }

void setChatMagicPasswd(void) { sprintf(gServerConfig.chatmagicpasswd, "gm"); }

unsigned getChatMagicCDKeyCheck(void) {
  return gServerConfig.chatmagiccdkeycheck;
}

void setChatMagicCDKeyCheck(void) { gServerConfig.chatmagiccdkeycheck = 0; }

unsigned int getFilesearchnum(void) { return gServerConfig.filesearchnum; }

unsigned int getNpctemplatenum(void) { return gServerConfig.npctemplatenum; }

unsigned int getNpccreatenum(void) { return gServerConfig.npccreatenum; }

unsigned int getWalksendinterval(void) {
  return gServerConfig.walksendinterval;
}

void setWalksendinterval(unsigned int interval) {
  gServerConfig.walksendinterval = interval;
}

unsigned int getCAsendinterval_ms(void) {
  return gServerConfig.CAsendinterval_ms;
}

void setCAsendinterval_ms(unsigned int interval_ms) {
  gServerConfig.CAsendinterval_ms = interval_ms;
}

unsigned int getCDsendinterval_ms(void) {
  return gServerConfig.CDsendinterval_ms;
}

void setCDsendinterval_ms(unsigned int interval_ms) {
  gServerConfig.CDsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * Onelooptime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOnelooptime_ms(void) { return gServerConfig.Onelooptime_ms; }

void setOnelooptime_ms(unsigned int interval_ms) {
  gServerConfig.Onelooptime_ms = interval_ms;
}
unsigned int getPetdeletetime(void) { return gServerConfig.Petdeletetime; }

void setPetdeletetime(unsigned int interval) {
  gServerConfig.Petdeletetime = interval;
}

unsigned int getItemdeletetime(void) { return gServerConfig.Itemdeletetime; }

void setItemdeletetime(unsigned int interval) {
  gServerConfig.Itemdeletetime = interval;
}

unsigned int getCharSavesendinterval(void) {
  return gServerConfig.CharSavesendinterval;
}

void setCharSavesendinterval(unsigned int interval) {
  gServerConfig.CharSavesendinterval = interval;
}

unsigned int getAddressbookoffmsgnum(void) {
  return gServerConfig.addressbookoffmsgnum;
}

unsigned int getProtocolreadfrequency(void) {
  return gServerConfig.protocolreadfrequency;
}

unsigned int getAllowerrornum(void) { return gServerConfig.allowerrornum; }

unsigned int getLogHour(void) { return gServerConfig.loghour; }

unsigned int getBattleDebugMsg(void) { return gServerConfig.battledebugmsg; }

void setBattleDebugMsg(unsigned int num) { gServerConfig.battledebugmsg = num; }

void defaultConfig(char *argv0) {
  char *program = rindex(argv0, '/');
  char *windows_program = rindex(argv0, '\\');
  if (windows_program != NULL &&
      (program == NULL || windows_program > program))
    program = windows_program;
  if (program == NULL)
    program = argv0;
  else
    program++;
  strcpysafe(gServerConfig.program_name, sizeof(gServerConfig.program_name),
             program);
  strcpysafe(gServerConfig.config_filename,
             sizeof(gServerConfig.config_filename), "setup.cf");
}

void lastConfig(void) {
  char entry[256];
  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.mapdir);
  strcpysafe(gServerConfig.mapdir, sizeof(gServerConfig.mapdir), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.maptilefile);
  strcpysafe(gServerConfig.maptilefile, sizeof(gServerConfig.maptilefile),
             entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.battlemapfile);
  strcpysafe(gServerConfig.battlemapfile, sizeof(gServerConfig.battlemapfile),
             entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.itemfile);
  strcpysafe(gServerConfig.itemfile, sizeof(gServerConfig.itemfile), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.invfile);
  strcpysafe(gServerConfig.invfile, sizeof(gServerConfig.invfile), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.appearfile);
  strcpysafe(gServerConfig.appearfile, sizeof(gServerConfig.appearfile), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.effectfile);
  strcpysafe(gServerConfig.effectfile, sizeof(gServerConfig.effectfile), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.quizfile);
  strcpysafe(gServerConfig.quizfile, sizeof(gServerConfig.quizfile), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.titlenamefile);
  strcpysafe(gServerConfig.titlenamefile, sizeof(gServerConfig.titlenamefile),
             entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.lsgenlog);
  strcpysafe(gServerConfig.lsgenlog, sizeof(gServerConfig.lsgenlog), entry);

  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.npcdir);
  strcpysafe(gServerConfig.npcdir, sizeof(gServerConfig.npcdir), entry);

#ifdef _STORECHAR
  snprintf(entry, sizeof(entry), "%s/%s", gServerConfig.topdir,
           gServerConfig.storechar);
  strcpysafe(gServerConfig.storechar, sizeof(gServerConfig.storechar), entry);
#endif
}

void substitutePointerFromType(void *to, CTYPE type, double value) {
  switch (type) {
  case CHAR:
    *(char *)to = (char)value;
    break;
  case SHORT:
    *(short *)to = (short)value;
    break;
  case INT:
    *(int *)to = (int)value;
    break;
  case DOUBLE:
    *(double *)to = (double)value;
    break;
  }
}

BOOL luareadgServerConfigfile(char *data) {
  char firstToken[256];
  int ret =
      getStringFromIndexWithDelim(data, "=", 1, firstToken, sizeof(firstToken));
  if (ret == FALSE) {
    return FALSE;
  }
  int i;
  for (i = 0; i < arraysizeof(gReadConf); i++) {
    if (strcmp(gReadConf[i].name, firstToken) == 0) {
      char secondToken[256]; /*2    及  侬  */
      ret = getStringFromIndexWithDelim(data, "=", 2, secondToken,
                                        sizeof(secondToken));
      if (ret == FALSE) {
        break;
      }

      if (gReadConf[i].charvalue != NULL)
        strcpysafe(gReadConf[i].charvalue, gReadConf[i].charsize, secondToken);
      if (gReadConf[i].value != NULL) {
        if (strcmp("ON", secondToken) == 0) {

          substitutePointerFromType(gReadConf[i].value, gReadConf[i].valuetype,
                                    1.0);

        } else if (strcmp("OFF", secondToken) == 0) {

          substitutePointerFromType(gReadConf[i].value, gReadConf[i].valuetype,
                                    1.0);
        } else {
          strtolchecknum(secondToken, (int *)gReadConf[i].value, 10,
                         gReadConf[i].valuetype);
        }
      }
      break;
    }
  }
  return TRUE;
}

BOOL readgServerConfigfile(char *filename) {
  FILE *f = NULL;
  char linebuf[256];
  int linenum = 0;
  char realopenfilename[256];
  char hostname[128];
  if (gethostname(hostname, sizeof(hostname)) != -1) {
    char *initdot;
    initdot = index(hostname, '.');
    if (initdot != NULL)
      *initdot = '\0';
    snprintf(realopenfilename, sizeof(realopenfilename), "%s.%s", filename,
             hostname);
    f = fopen(realopenfilename, "r");
  }
  if (f == NULL) {
    f = fopen(filename, "r");
    if (f == NULL) {
      print("Can't open %s\n", filename);
      return FALSE;
    }
  }

  while (fgets(linebuf, sizeof(linebuf), f)) {
    char firstToken[256];
    int i;
    int ret;
    linenum++;
    deleteWhiteSpace(linebuf);
    if (linebuf[0] == '#' || linebuf[0] == '\n')
      continue;     /* comment or blank line. */
    chomp(linebuf); /* remove tail newline */
    ret = getStringFromIndexWithDelim(linebuf, "=", 1, firstToken,
                                      sizeof(firstToken));
    if (ret == FALSE) {
      print("Find error at %s in line %d. Ignore\n", filename, linenum);
      continue;
    }
    for (i = 0; i < arraysizeof(gReadConf); i++) {
      if (strcmp(gReadConf[i].name, firstToken) == 0) {
        char secondToken[256];
        ret = getStringFromIndexWithDelim(linebuf, "=", 2, secondToken,
                                          sizeof(secondToken));
        if (ret == FALSE) {
          print("Find error at %s in line %d. Ignore", filename, linenum);
          break;
        }
        if (gReadConf[i].charvalue != NULL)
          strcpysafe(gReadConf[i].charvalue, gReadConf[i].charsize,
                     secondToken);
        if (gReadConf[i].value != NULL) {
          if (strcmp("ON", secondToken) == 0) {
            substitutePointerFromType(gReadConf[i].value,
                                      gReadConf[i].valuetype, 1.0);
          } else if (strcmp("OFF", secondToken) == 0) {
            substitutePointerFromType(gReadConf[i].value,
                                      gReadConf[i].valuetype, 1.0);
          } else {
            strtolchecknum(secondToken, (int *)gReadConf[i].value, 10,
                           gReadConf[i].valuetype);
          }
        }
        break;
      }
    }
  }
  fclose(f);
  lastConfig();
  return TRUE;
}

extern int JENCODE_KEY;
unsigned int setEncodeKey(void) {
  JENCODE_KEY = gServerConfig.encodekey;
  return JENCODE_KEY;
}
unsigned int setAcWBSize(void) {
  AC_WBSIZE = gServerConfig.acwbsize;
  return AC_WBSIZE;
}
unsigned int getAcwriteSize(void) { return gServerConfig.acwritesize; }
unsigned int getErrUserDownFlg(void) { return gServerConfig.ErrUserDownFlg; }

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime(void) {
  if (gServerConfig.Golddeletetime > 0)
    return gServerConfig.Golddeletetime;
  else
    return gServerConfig.Itemdeletetime;
}
void setIGolddeletetime(unsigned int interval) {
  gServerConfig.Golddeletetime = interval;
}
#endif

#ifdef _NEW_PLAYER_CF
int getNewplayertrans(void) {
  if (gServerConfig.newplayertrans > 7)
    return 7;
  else if (gServerConfig.newplayertrans >= 0)
    return gServerConfig.newplayertrans;
  else
    return 0;
}
int getNewplayerlv(void) {
  if (gServerConfig.newplayerlv > 160)
    return 160;
  else if (gServerConfig.newplayerlv > 0)
    return gServerConfig.newplayerlv;
  else
    return 0;
}
int getNewplayerpetlv(void) {
  if (gServerConfig.newplayerpetlv > 160)
    return 160;
  else if (gServerConfig.newplayerpetlv > 0)
    return gServerConfig.newplayerpetlv;
  else
    return 0;
}

int getNewplayergivepet(int index) {
  if (gServerConfig.newplayergivepet[index] > 0)
    return gServerConfig.newplayergivepet[index];
  else
    return -1;
}

int getNewplayergiveitem(int index) {
  if (gServerConfig.newplayergiveitem[index] > 0)
    return gServerConfig.newplayergiveitem[index];
  else
    return -1;
}

void setNewplayergivepet(unsigned int index, unsigned int interval) {
  gServerConfig.newplayergivepet[index] = interval;
}

int getNewplayergivegold(void) {
  if (gServerConfig.newplayergivegold > 1000000)
    return 1000000;
  else if (gServerConfig.newplayergivegold < 0)
    return 0;
  else
    return gServerConfig.newplayergivegold;
}
int getRidePetLevel(void) {
  if (gServerConfig.ridepetlevel > 0)
    return gServerConfig.ridepetlevel;
  else
    return -1;
}
#ifdef _VIP_SERVER
int getNewplayergivevip(void) {
  return gServerConfig.newplayerpetvip < 0 ? 0 : gServerConfig.newplayerpetvip;
}
#endif
#endif

#ifdef _UNLAW_WARP_FLOOR
int getUnlawwarpfloor(unsigned int index) {
  char unlawwarpfloor[256];
  getStringFromIndexWithDelim(gServerConfig.unlawwarpfloor, ",", index + 1,
                              unlawwarpfloor, sizeof(unlawwarpfloor));
  return atoi(unlawwarpfloor);
}
#endif

#ifdef _NO_JOIN_FLOOR
int getNoJoinFloor(unsigned int index) {
  char nojoinfloor[256];
  getStringFromIndexWithDelim(gServerConfig.nojoinfloor, ",", index + 1,
                              nojoinfloor, sizeof(nojoinfloor));
  return atoi(nojoinfloor);
}
#endif

#ifdef _WATCH_FLOOR
int getWatchFloor(unsigned int index) {
  if (gServerConfig.watchfloor[index] > 0)
    return gServerConfig.watchfloor[index];
  else
    return -1;
}
char *getWatchFloorCF(void) {
  return (gServerConfig.watchfloor[0] > 0) ? "是" : "否";
}
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor(unsigned int index) {

  char battlefloor[256];
  if (getStringFromIndexWithDelim(gServerConfig.battlefloorcf, ",", index + 1,
                                  battlefloor, sizeof(battlefloor)) == TRUE)
    return atoi(battlefloor);
  else
    return -1;
}
char *getBattleFloorCF(void) {
  return (gServerConfig.battlefloor > 0) ? "是" : "否";
}
#endif

#ifdef _ANGEL_SUMMON

extern int mission_num;

BOOL LoadMissionList(void) {
  FILE *fp;
  int i = 0;

  mission_num = 0;
#ifdef _CRYPTO_DATA
  BOOL crypto = FALSE;
  fp = fopen("./data/mission.txt.allblues", "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen("./data/mission.txt", "r");
  }
  if (fp == NULL) {
    print("任务文件打开错误\n");
    return FALSE;
  }

  memset(missionlist, 0, sizeof(missionlist));

  while (1) {
    char line[1024];
    char token[1024];
    int mindex;
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    chop(line);
    // 以#为注解*******
    if (line[0] == '#')
      continue;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }
    //*************************************

    // 格式 #任务编号,必要等级,任务说明,奖品ID,限制时间(小时)

    getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    mindex = atoi(token);

    if (mindex <= 0 || mindex >= MAXMISSION)
      break;

    missionlist[mindex].id = mindex;

    getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    missionlist[mindex].level = atoi(token);

    getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(missionlist[mindex].eventflag, token);

    getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(missionlist[mindex].detail, token);
    getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    missionlist[mindex].limittime = atoi(token);
    mission_num++;
  }
  fclose(fp);
  return TRUE;
}

BOOL LoadMissionCleanList() {
  FILE *fp;
  int listindex = 0;
  int i = 0;

  memset(missiontable, 0, sizeof(missiontable));

#ifdef _CRYPTO_DATA
  BOOL crypto = FALSE;
  fp = fopen("./data/missionclean.txt.allblues", "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen("./data/missionclean.txt", "r");
  }
  if (fp == NULL) {
    print("清除任务文件打开错误\n");
    return FALSE;
  }
  while (1) {
    char line[1024];
    char token[1024];

    if (fgets(line, sizeof(line), fp) == NULL)
      break;
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    chop(line);
    if (line[0] == '#')
      continue;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }
    getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(missiontable[listindex].angelinfo, token);

    getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(missiontable[listindex].heroinfo, token);

    getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    missiontable[listindex].mission = atoi(token);

    getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
    // if (strcmp(token, "") == 0)  break;
    missiontable[listindex].flag = atoi(token);

    getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    missiontable[listindex].time = atoi(token);

    print("\nMISSIONCLEAN[%d] %s %s %d %d %d", listindex,
          missiontable[listindex].angelinfo, missiontable[listindex].heroinfo,
          missiontable[listindex].mission, missiontable[listindex].flag,
          missiontable[listindex].time);

    listindex++;
    if (listindex >= MAXMISSIONTABLE)
      break;
  }
  fclose(fp);
  return TRUE;
}

#endif

#ifdef _JOBDAILY
extern DailyFileType dailyfile[MAXDAILYLIST];
BOOL LoadJobdailyfile(void) {
  char line[20000];
  char token[16384];
  int listindex = 0;
  int i;
  FILE *fp;
#ifdef _CRYPTO_DATA
  BOOL crypto = FALSE;
  fp = fopen("./data/jobdaily.txt.allblues", "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen("./data/jobdaily.txt", "r");
  }
  if (fp == NULL) {
    print("日常工作文件打开错误\n");
    return FALSE;
  }

  memset(dailyfile, 0, sizeof(dailyfile));

  while (1) {
    line[0] = '\0';
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    // print("\n %s ", line);
    chop(line);

    // #为注解
    if (line[0] == '#')
      continue;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }

    getStringFromIndexWithDelim(line, "|", 1, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    dailyfile[listindex].jobid = atoi(token);

    getStringFromIndexWithDelim(line, "|", 2, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(dailyfile[listindex].rule, token);

    getStringFromIndexWithDelim(line, "|", 3, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    if (strlen(token) > 64) {
      print("\n任务说明过长:%d\n", strlen(token));
      return FALSE;
    }
    strcpy(dailyfile[listindex].explain, token);

    getStringFromIndexWithDelim(line, "|", 4, token, sizeof(token));
    if (strcmp(token, "") == 0)
      break;
    strcpy(dailyfile[listindex].state, token);

    listindex++;
    if (listindex >= MAXDAILYLIST)
      break;
  }
  fclose(fp);
  return TRUE;
}
#endif

#ifdef _USER_EXP_CF
BOOL LoadEXP(char *filename) {
  FILE *fp;
  int i = 0;
  int MaxLevel = 0;

#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  fp = fopen(realopfile, "r");
  if (fp != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    fp = fopen(filename, "r");
  }
  if (fp == NULL) {
    print("无法打开文件\n");
    return FALSE;
  }

  while (1) {
    char line[64], exp[64];
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    chop(line);

    if (line[0] == '#')
      continue;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }
    MaxLevel = MaxLevel + 1;
    if (MaxLevel >= 200)
      break;
    easyGetTokenFromString(line, 2, exp, sizeof(exp));
    NeedLevelUpTbls[MaxLevel] = atoi(exp);
  }
  fclose(fp);
  return TRUE;
}

char *getEXPfile(void) { return gServerConfig.expfile; }

int getNeedLevelUpTbls(int level) { return NeedLevelUpTbls[level]; }

#endif

#ifdef _UNREG_NEMA
char *getUnregname(int index) { return gServerConfig.unregname[index]; }
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans(void) {
  if (gServerConfig.chartrans > 6)
    gServerConfig.chartrans = 5;
  return gServerConfig.chartrans;
}
int getPettrans(void) {
  if (gServerConfig.pettrans > 2)
    return 2;
  else if (gServerConfig.pettrans < -1)
    return -1;
  return gServerConfig.pettrans;
}
int getYBLevel(void) {
  if (gServerConfig.yblevel > gServerConfig.maxlevel)
    gServerConfig.yblevel = gServerConfig.maxlevel;
  return gServerConfig.yblevel;
}
int getMaxLevel(void) { return gServerConfig.maxlevel; }
#endif

#ifdef _POINT
char *getPoint(void) { return (gServerConfig.point > 0) ? "是" : "否"; }
int getTransPoint(int index) { return gServerConfig.transpoint[index]; }
#endif

#ifdef _PET_AND_ITEM_UP
char *getPetup(void) { return (gServerConfig.petup > 0) ? "是" : "否"; }
char *getItemup(void) { return (gServerConfig.itemup > 0) ? "是" : "否"; }
#endif
#ifdef _LOOP_ANNOUNCE
char *getLoopAnnouncePath(void) { return gServerConfig.loopannouncepath; }
int loadLoopAnnounce(void) {
  FILE *fp;
  int i = 0;
  gServerConfig.loopannouncemax = 0;
  fp = fopen(gServerConfig.loopannouncepath, "r");
  if (fp == NULL) {
    print("无法打开文件\n");
    return FALSE;
  }
  while (1) {
    char line[1024];
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
    chop(line);

    if (line[0] == '#')
      continue;
    for (i = 0; i < 10; i++) {
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }
    }
    //*************************************
    strcpy(gServerConfig.loopannounce[gServerConfig.loopannouncemax], line);
    gServerConfig.loopannouncemax++;
  }
  fclose(fp);
  return TRUE;
}
int getLoopAnnounceTime(void) {
  return (gServerConfig.loopannouncetime < 0) ? -1
                                              : gServerConfig.loopannouncetime;
}
int getLoopAnnounceMax(void) {
  return (gServerConfig.loopannouncemax > 0) ? gServerConfig.loopannouncemax
                                             : 0;
}
char *getLoopAnnounce(int index) { return gServerConfig.loopannounce[index]; }
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup(void) { return (gServerConfig.skup > 0) ? gServerConfig.skup : 0; }
#endif
#ifdef _RIDELEVEL
int getRideLevel(void) { return gServerConfig.ridelevel; }
int getRideTrans(void) { return gServerConfig.ridetrans; }
#else
int getRideTrans(void) { return 0; }
#endif
#ifdef _REVLEVEL
char *getRevLevel(void) { return (gServerConfig.revlevel > 0) ? "是" : "否"; }
#endif
#ifdef _NEW_PLAYER_RIDE
char *getPlayerRide(void) {
  if (gServerConfig.npride > 2)
    return "配套送虎加雷";
  else if (gServerConfig.npride == 2)
    return "配套送雷";
  else if (gServerConfig.npride == 1)
    return "配套送虎";
  else
    return "不送配套骑宠";
}
#endif

#ifdef _FIX_CHARLOOPS
int getCharloops(void) { return gServerConfig.charloops - 1; }
#endif

#ifdef _PLAYER_ANNOUNCE
int getPAnnounce(void) { return max(-1, gServerConfig.pannounce); }
#endif
#ifdef _PLAYER_MOVE
int getPMove(void) { return max(-1, gServerConfig.pmove); }
#endif

int getrecvbuffer(void) {
  if (gServerConfig.recvbuffer < 0)
    return 0;
  else if (gServerConfig.recvbuffer > 128)
    return 128;
  else
    return gServerConfig.recvbuffer;
}

int getsendbuffer(void) {
  if (gServerConfig.sendbuffer < 0)
    return 0;
  else if (gServerConfig.sendbuffer > 128)
    return 128;
  else
    return gServerConfig.sendbuffer;
}

int getrecvlowatbuffer(void) {
  if (gServerConfig.recvlowatbuffer < 0)
    return 0;
  else if (gServerConfig.recvlowatbuffer > 1024)
    return 1024;
  else
    return gServerConfig.recvlowatbuffer;
}

int getrunlevel(void) {
  if (gServerConfig.runlevel < -20)
    return -20;
  else if (gServerConfig.runlevel > 19)
    return 19;
  else
    return gServerConfig.runlevel;
}

#ifdef _SHOW_VIP_CF
int getShowVip(void) {
  if (gServerConfig.showvip > 2)
    return 2;
  else if (gServerConfig.showvip < 0)
    return 0;
  else
    return gServerConfig.showvip;
}
#endif

#ifdef _PLAYER_NUM
int getPlayerNum(void) { return gServerConfig.playernum; }
void setPlayerNum(int num) { gServerConfig.playernum = num; }
#endif

#ifdef _BATTLE_GOLD
int getBattleGold(void) {
  if (gServerConfig.battlegold < 0)
    return 0;
  else if (gServerConfig.battlegold > 100)
    return 100;
  else
    return gServerConfig.battlegold;
}
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime(void) {
  return (gServerConfig.angelplayertime > 1) ? gServerConfig.angelplayertime
                                             : 1;
}
int getAngelPlayerMun(void) {
  return (gServerConfig.angelplayermun > 2) ? gServerConfig.angelplayermun : 2;
}
#endif

#ifdef _RIDEMODE_20
int getRideMode(void) {
  if (gServerConfig.ridemode < 0)
    gServerConfig.ridemode = 0;
  return gServerConfig.ridemode;
}
#else
int getRideMode(void) { return 0; }
#endif
#ifdef _FM_POINT_PK
char *getFmPointPK(void) { return (gServerConfig.fmpointpk > 0) ? "是" : "否"; }
#endif
#ifdef _ENEMY_ACTION
int getEnemyAction(void) {
  if (gServerConfig.enemyact > 100)
    return 100;
  else if (gServerConfig.enemyact < 1)
    return 1;
  else
    return gServerConfig.enemyact;
}
#endif

#ifdef _FUSIONBEIT_TRANS
int getFusionbeitTrans(void) {
  if (gServerConfig.fusionbeittrans > 2)
    return 2;
  else if (gServerConfig.fusionbeittrans < 0)
    return 0;
  else
    return gServerConfig.fusionbeittrans;
}
#endif
int getCpuUse(void) { return gServerConfig.cpuuse; }
#ifdef _CHECK_PEPEAT
int getCheckRepeat(void) { return (gServerConfig.CheckRepeat > 0) ? 1 : 0; }
#endif

#ifdef _FM_JOINLIMIT
int getJoinFamilyTime(void) { return gServerConfig.joinfamilytime; }
#endif

#ifdef _MAP_HEALERALLHEAL
int getMapHeal(int index) {
  char mapheal[256];
  getStringFromIndexWithDelim(gServerConfig.mapheal, ",", index + 1, mapheal,
                              sizeof(mapheal));
  return atoi(mapheal);
}
#endif

#ifdef _THE_WORLD_SEND
int getTheWorldTrans() { return gServerConfig.thewordtrans; }
int getTheWorldLevel() { return gServerConfig.thewordlevel; }
int getTheWorldSend() { return gServerConfig.thewordsend; }
#endif

#ifdef _LOGIN_DISPLAY
int getLoginDisplay() { return gServerConfig.logindisplay; }
#endif

#ifdef _VIP_POINT_PK
int getVipPointPK(int index) {
  char vippointpk[256];
  if (getStringFromIndexWithDelim(gServerConfig.vippointpk, ",", index + 1,
                                  vippointpk, sizeof(vippointpk)) == TRUE) {
    return atoi(vippointpk);
  } else {
    return -1;
  }
}
float getVipPointPKCost(void) { return gServerConfig.vippointpkcost / 100.00; }
#endif

#ifdef _SPECIAL_MAP
int getSpecialMap(int index) {
  char specialmap[256];
  if (getStringFromIndexWithDelim(gServerConfig.specialmap, ",", index + 1,
                                  specialmap, sizeof(specialmap)) == TRUE) {
    return atoi(specialmap);
  } else {
    return -1;
  }
}
#endif

#ifdef _NEW_AUTO_PK
int getAutoPkTime() {
  if (gServerConfig.autopk > 23)
    gServerConfig.autopk = 23;
  if (gServerConfig.autopk < 0)
    gServerConfig.autopk = -1;
  return gServerConfig.autopk;
}
int getAutoPkTrans() { return gServerConfig.autopktrans; }

int getAutoPkLv() { return gServerConfig.autopklv; }
#ifdef _FORMULATE_AUTO_PK
int getAutoPkPoint() { return gServerConfig.autopkpoint; }
int getKillPoint() { return gServerConfig.killpoint; }
#endif
#endif

#ifdef _AUTO_DEL_PET
int getAutoDelPet(int index) {
  char autodelpet[256];
  getStringFromIndexWithDelim(gServerConfig.autodelpet, ",", index + 1,
                              autodelpet, sizeof(autodelpet));
  return atoi(autodelpet);
}
#endif

#ifdef _AUTO_DEL_ITEM
int getAutoDelItem(int index) {
  char autodelitem[256];
  getStringFromIndexWithDelim(gServerConfig.autodelitem, ",", index + 1,
                              autodelitem, sizeof(autodelitem));
  return atoi(autodelitem);
}
#endif

#ifdef _BT_PET
int getBtPet() { return gServerConfig.btpet; }
#endif

#ifdef _BT_ITEM
int getBtItem() { return gServerConfig.btitem; }
#endif

#ifdef _LUCK_STAR
int getLuckStarTime() {
  if (gServerConfig.luckstartime < 1)
    gServerConfig.luckstartime = 1;
  return gServerConfig.luckstartime;
}
int getLuckStarChances() { return gServerConfig.luckstarchances; }
#endif

#ifdef _BATTLE_GETITEM_RATE
int getBattleGetItemRate(int index) {
  char battlegetitemrate[256];
  getStringFromIndexWithDelim(gServerConfig.battlegetitemrate, ",", index + 1,
                              battlegetitemrate, sizeof(battlegetitemrate));
  return atoi(battlegetitemrate);
}
int getBattleGetItemRateMap() { return gServerConfig.battlegetitemratemap; }
#endif

#ifdef _UNLAW_THIS_LOGOUT
int getUnlawThisLogout(int index) {
  char unlawthislogout[256];
  getStringFromIndexWithDelim(gServerConfig.unlawthislogout, ",", index + 1,
                              unlawthislogout, sizeof(unlawthislogout));
  return atoi(unlawthislogout);
}
#endif

#ifdef _TRANS_POINT_UP
int getTransPoinUP(int index) {
  char transpointup[256];
  getStringFromIndexWithDelim(gServerConfig.transpointup, ",", index + 1,
                              transpointup, sizeof(transpointup));
  return atoi(transpointup);
}
#endif

#ifdef _OPEN_STW_SEND
int getOpenStwSendType(void) {
  if (gServerConfig.stwsendtype < -1) {
    gServerConfig.stwsendtype = -1;
  } else if (gServerConfig.stwsendtype > 5) {
    gServerConfig.stwsendtype = 5;
  }
  return gServerConfig.stwsendtype;
}
int getOpenStwSendPoint(void) { return max(1, gServerConfig.stwsendpoint); }

#endif

#ifdef _POOL_ITEM_BUG
int getPoolItemBug(void) { return gServerConfig.poolitembug; }
int getPoolItem(int id) {
  if (id < 0)
    id = 0;
  char poolitem[256];
  if (getStringFromIndexWithDelim(gServerConfig.poolitem, ",", id + 1, poolitem,
                                  sizeof(poolitem)) == TRUE) {
    return atoi(poolitem);
  } else {
    return -1;
  }
}

#endif

#ifdef _NO_STW_ENEMY
int getNoSTWNenemy(void) { return gServerConfig.nostwenemy; }

int getNoSTWNenemyPoint(void) { return gServerConfig.nostwenemypoint; }
#endif

#ifdef _NEW_STREET_VENDOR
int getStreetVendor(int id) {
  if (id < 0)
    id = 0;
  char streetvendorpoint[256];
  if (getStringFromIndexWithDelim(gServerConfig.streetvendorpoint, ",", id + 1,
                                  streetvendorpoint,
                                  sizeof(streetvendorpoint)) == TRUE) {
    return atoi(streetvendorpoint);
  } else {
    return -1;
  }
}

#endif

#ifdef _ITEM_PET_LOCKED
int getItemPetLocked(void) { return gServerConfig.itampetlocked; }
#endif

#ifdef _TALK_SAVE
int getSaveFame(void) { return max(0, gServerConfig.savefame * 100); }
#endif

#ifdef _TALK_CHECK
int getTalkCheck(void) {
  return (rand() % max(10, gServerConfig.talkcheckmax)) +
         max(10, gServerConfig.talkcheckmin);
}
#endif

#ifdef _DISABLE_PROFESSION_SKILL
BOOL getDisableProfessionSkill(int floor) {
  int i = 1;
  char buff[64];
  while (getStringFromIndexWithDelim(gServerConfig.disableprofessionskill, ",",
                                     i, buff, sizeof(buff))) {
    if (atoi(buff) == floor)
      return TRUE;
    i++;
  }
  return FALSE;
}
#endif
#ifdef _ALL_SERV_SEND
int getAllServTrans() { return gServerConfig.allservtrans; }
int getAllServLevel() { return gServerConfig.allservlevel; }
int getAllServSend() { return gServerConfig.allservsend; }
#endif
#ifdef _PET_TRANS_ABILITY
int getPetTransAbility() {
  if (gServerConfig.pettransability <= 0)
    gServerConfig.pettransability = 0;
  return gServerConfig.pettransability;
}
int getPetTransAbility1() {
  if (gServerConfig.pettransability1 <= 0)
    gServerConfig.pettransability1 = 150;
  return gServerConfig.pettransability1;
}
int getPetTransAbility2() {
  if (gServerConfig.pettransability2 <= 0)
    gServerConfig.pettransability2 = 200;
  return gServerConfig.pettransability2;
}
#endif

#ifdef _NEED_ITEM_ENEMY
int getDelNeedItem() { return gServerConfig.delneeditem; }
#endif

#ifdef _NOT_ESCAPE
int getNotEscape(int index) {
  char notescape[256];
  if (getStringFromIndexWithDelim(gServerConfig.notescape, ",", index + 1,
                                  notescape, sizeof(notescape)) == TRUE) {
    return atoi(notescape);
  } else {
    return -1;
  }
}
#endif

#ifdef _PLAYER_OVERLAP_PK
int getPlayerOverlapPk(int index) {
  char playeroverlappk[256];
  if (getStringFromIndexWithDelim(gServerConfig.playeroverlappk, ",", index + 1,
                                  playeroverlappk,
                                  sizeof(playeroverlappk)) == TRUE) {
    return atoi(playeroverlappk);
  } else {
    return -1;
  }
}
#endif

#ifdef _FIMALY_PK_TIME
int getFimalyPkTime() { return gServerConfig.fimalypktime; }
#endif

#ifdef _PETSKILL_SHOP_LUA
char *getFreePetSkillShopPath() { return gServerConfig.freepetskillshoppath; }
#endif

#ifdef _CANCEL_ANGLE_TRANS
int getCancelAngleTrans() { return gServerConfig.cancelanlgetrans; }
#endif

#ifdef _VIP_BATTLE_EXP
int getVipBattleexp() { return gServerConfig.vipbattleexp; }
#endif

#ifdef _NO_HELP_MAP
int getNoHelpMap(int index) {
  char nohelpmap[256];
  if (getStringFromIndexWithDelim(gServerConfig.nohelpmap, ",", index + 1,
                                  nohelpmap, sizeof(nohelpmap)) == TRUE) {
    return atoi(nohelpmap);
  } else {
    return -1;
  }
}
#endif

#ifdef _BATTLE_TIME
int getBattleTime() { return gServerConfig.battletime; }
#endif

#ifdef _SAME_IP_ONLINE_NUM
int getSameIpOnlineNum() { return gServerConfig.sameiponlinenum; }
#endif

#ifdef _STREET_VENDOR_TRANS
int getStreetVendorTrans() { return gServerConfig.streetvendortrans; }
#endif

#ifdef _CHECK_SEVER_IP
int checkServerIp(unsigned int ip) {
  char serverip[64];
  char cliip[64];
  int i = 1;

  int a, b, c, d;

  a = (ip % 0x100);
  ip = ip / 0x100;
  b = (ip % 0x100);
  ip = ip / 0x100;
  c = (ip % 0x100);
  ip = ip / 0x100;
  d = (ip % 0x100);

  sprintf(cliip, "%d.%d.%d.%d", a, b, c, d);

  while (getStringFromIndexWithDelim(gServerConfig.serverip, ",", i++, serverip,
                                     sizeof(serverip)) == TRUE) {
    if (strcmp(cliip, serverip) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}
#endif

#ifdef _DAMMAGE_CALC
int getDamageCalc() {
  if (gServerConfig.dammagecalc > 0) {
    return gServerConfig.dammagecalc;
  } else {
    return 70;
  }
}
#endif

#ifdef _PET_ENEMY_DEVELOP_UP
int getPetEnemyDevelopUp() { return gServerConfig.PetEnemyDevelopUp; }
#endif

#ifdef _FIRST_LOCK_ITEM
int getFirstLockItem(int index) {
  char FirstLockItem[256];
  if (getStringFromIndexWithDelim(gServerConfig.FirstLockItem, ",", index + 1,
                                  FirstLockItem,
                                  sizeof(FirstLockItem)) == TRUE) {
    return atoi(FirstLockItem);
  } else {
    return -1;
  }
}
#endif
int getConnectnum() { return gServerConfig.Connectnum; }

#ifdef _PETSKILL_NEW_PASSIVE
int getAutoGetSkill(void) {
  if (gServerConfig.autogetskill == 1)
    return 1;

  return 0;
}

int getSkillFusion(void) { return gServerConfig.skillfusion; }

int getSkillCount(void) {
  if (gServerConfig.skillcount < 1 || gServerConfig.skillcount > 6)
    return 1;
  return gServerConfig.skillcount;
}

// 获得技能生成的位置
int getSkillPos(void) {
  if (gServerConfig.getskillpos < 1 || gServerConfig.getskillpos > 7)
    return 6;
  return gServerConfig.getskillpos - 1;
}

int getSkillProb(int skillLevel) {
  char prob[20];
  if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[skillLevel - 1],
                                  "|", 1, prob, sizeof(prob)) == FALSE)
    return 0;
  return atoi(prob);
}

int getSkillRange(int skillLevel, int start) {
  char index[20];
  if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[skillLevel - 1],
                                  "|", start + 1, index,
                                  sizeof(index)) == FALSE)
    return -1;
  return atoi(index);
}

int getFusionRange(int start) {
  char index[20];
  if (getStringFromIndexWithDelim(gServerConfig.fusionrange, "|", start, index,
                                  sizeof(index)) == FALSE)
    return -1;
  return atoi(index);
}

int isFusionPet(int petId) {
  if ((petId >= getFusionRange(1) && petId <= getFusionRange(2)) ||
      petId == 1045)
    return 1;
  return 0;
}

int isPassiveSkill(int skillId) {
  if (skillId < 0)
    return -1;
  char start[20];
  char end[20];
  int i;
  for (i = 0; i < 5; i++) {
    if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[i], "|", 2, start,
                                    sizeof(start)) == FALSE)
      return -1;
    if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[i], "|", 3, end,
                                    sizeof(end)) == FALSE)
      return -1;
    if (skillId >= atoi(start) && skillId <= atoi(end))
      return 1;
  }
  return -1;
}

int getSkillLevel(int skillId) {
  if (skillId < 0)
    return -1;
  char start[20];
  char end[20];
  int i;
  for (i = 0; i < 5; i++) {
    if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[i], "|", 2, start,
                                    sizeof(start)) == FALSE)
      return -1;
    if (getStringFromIndexWithDelim(gServerConfig.skillinfolv[i], "|", 3, end,
                                    sizeof(end)) == FALSE)
      return -1;
    if (skillId >= atoi(start) && skillId <= atoi(end))
      return i + 1;
  }
  return -1;
}

int getRandSkillByLevel(int skillLevel) {
  if (skillLevel < 0 || skillLevel > 5)
    return -1;
  return RAND(getSkillRange(skillLevel, 1), getSkillRange(skillLevel, 2));
}

int getRandSkill(void) {
  int i, k;
  int prob[5];
  for (i = 1; i <= 5; i++) {
    prob[i - 1] = getSkillProb(i);
  }
  k = RAND(0, 10000);
  if (k <= prob[0]) {
    return getRandSkillByLevel(1);
  } else if (k <= (prob[0] + prob[1])) {
    return getRandSkillByLevel(2);
  } else if (k <= (prob[0] + prob[1] + prob[2])) {
    return getRandSkillByLevel(3);
  } else if (k <= (prob[0] + prob[1] + prob[2] + prob[3])) {
    return getRandSkillByLevel(4);
  } else if (k <= (prob[0] + prob[1] + prob[2] + prob[3] + prob[4])) {
    return getRandSkillByLevel(5);
  } else {
    return -1;
  }
}
#endif
#ifdef _SHARE_EXP
int getExpShare(void) {
  if (gServerConfig.expshare == 1)
    return 1;
  else
    return 0;
}
#endif
#ifdef _DEFEND_BIGBAO
int getBigBao(void) {
  if (gServerConfig.bigbao <= 0)
    return 1024;
  return gServerConfig.bigbao;
}
int getBigBao2(void) {
  if (gServerConfig.bigbao2 <= 0)
    return 1024;
  return gServerConfig.bigbao2;
}
#endif

#ifdef _MO_SHOW_DEBUG
int isDebug(void) { return gServerConfig.isdebug; }
#endif
#ifdef _CHAR_LOOP_TIME
int getCharLoopTime() { return gServerConfig.charlooptime; }
#endif
#ifdef _MO_RELOAD_NPC
int getReloadNpcTime(void) {
  if (gServerConfig.reloadnpctime <= 0)
    return 0;
  return gServerConfig.reloadnpctime;
}

int getReloadNpcType(void) {
  if (gServerConfig.reloadnpctype <= 0)
    return 0;
  return 1;
}
#endif
#ifdef _JZ_NEWSCRIPT_LUA
char *getLuaFile(void) { return gServerConfig.luafile; }
BOOL ITEM_LuaInit(const char *_FileName) {
  if (strlen(_FileName) != 0)
    return TRUE;
  return FALSE;
}
#endif

#ifdef _ITEM_LUA
const char *getitemluafile(void) { return gServerConfig.itemluafile; }
#endif
#ifdef _MO_LNS_NLSUOXU
int GetConfigLineType(char *TM_ConfigName) {
  int i;
  for (i = 0; i < arraysizeof(gReadConf); i++) {
    if (strcmp(gReadConf[i].name, TM_ConfigName) == 0)
      break;
  }
  if (i == arraysizeof(gReadConf))
    return -1;
  switch (gReadConf[i].valuetype) {
  case CHAR:
    return 0;
    break;
  case SHORT:
    return 1;
    break;
  case INT:
    return 2;
    break;
  case DOUBLE:
    return 3;
    break;
  }
}

char *GetConfigLineVal(char *TM_ConfigName) {
  const char *filename = getConfigFilename();
  FILE *f = NULL;
  char linebuf[256];
  int linenum = 0;
  char realopenfilename[256];
  char secondToken[256];
  char hostname[128];

  if (f == NULL) {
    f = fopen(filename, "r");
    if (f == NULL) {
      print("Can't open %s\n", filename);
      return FALSE;
    }
  }
  while (fgets(linebuf, sizeof(linebuf), f)) {
    char firstToken[256];
    int i;
    int ret;
    linenum++;
    deleteWhiteSpace(linebuf); /* remove whitespace    */
    if (linebuf[0] == '#')
      continue; /* comment */
    if (linebuf[0] == '\n')
      continue;     /* none    */
    chomp(linebuf); /* remove tail newline  */
    ret = getStringFromIndexWithDelim(linebuf, "=", 1, firstToken,
                                      sizeof(firstToken));
    if (ret == FALSE) {
      print("Find error at %s in line %d. Ignore\n", filename, linenum);
      continue;
    }
    if (strcmp(firstToken, TM_ConfigName) != 0)
      continue;
    for (i = 0; i < arraysizeof(gReadConf); i++) {
      if (strcmp(gReadConf[i].name, firstToken) == 0) {
        ret = getStringFromIndexWithDelim(linebuf, "=", 2, secondToken,
                                          sizeof(secondToken));
        if (ret == FALSE) {
          print("Find error at %s in line %d. Ignore", filename, linenum);
          break;
        }
        return secondToken;
        break;
      }
    }
    break;
  }
  fclose(f);
  lastConfig();
  return NULL;
}

int getCopymapstartingID(void) { return gServerConfig.mapstart; }
#endif
#ifdef _ROOKIE_ITEM
unsigned int getRookieItem(int index) {
  if (gServerConfig.rookieitem[index] > 0)
    return gServerConfig.rookieitem[index];
  return 0;
}
#endif
#ifdef _NO_TRANS_ITEM
unsigned int getNoTransItem(void) { return gServerConfig.notransitem; }
#endif
#ifdef _MAX_MERGE_LEVEL
unsigned int getMaxMergeLevel(void) { return gServerConfig.maxmergelevel; }
#endif
#ifdef _NO_ATTACK
int getAttTime(void) { return gServerConfig.atttime; }
int getAttSafeTime(void) { return gServerConfig.attsafetime; }
int getAttCnt(void) { return gServerConfig.attcnt; }
int getLateTime(void) { return gServerConfig.latetime; }
int getAttDmeTime(void) { return gServerConfig.attdmetime; }
int getAttDmeCnt(void) { return gServerConfig.attdmecnt; }
#endif
char *getNoAttIp(int num) {
  if (num < 0)
    num = 0;
  else if (num > 4)
    num = 4;
  return gServerConfig.noattip[num];
}
#ifdef _NO_FULLPLAYER_ATT
int getNoFullPlayer(void) { return gServerConfig.nofullplayer; }
int getNoFull2Player(void) { return gServerConfig.nofull2player; }
int getNoCdkeyPlayer(void) {
  if (gServerConfig.nocdkeyplayer < 0 || gServerConfig.nocdkeyplayer > 2000)
    return 0;
  return gServerConfig.nocdkeyplayer;
}
int getNoCdkeyMode(void) {
  if (gServerConfig.nocdkeymode < 0 || gServerConfig.nocdkeymode > 2)
    return 0;
  return gServerConfig.nocdkeymode;
}
int getNoCdkeyType(void) {
  if (gServerConfig.nocdkeytype < 0 || gServerConfig.nocdkeytype > 2)
    return 0;
  return gServerConfig.nocdkeytype;
}
int getNoFullTime(void) { return gServerConfig.nofulltime; }
int getFengType(void) { return gServerConfig.fengtype; }
int getNoFullEndPlayer(void) { return gServerConfig.nofullendplayer; }
int getNoFullEndTime(void) { return gServerConfig.nofullendtime; }
int getManRenNum(void) { return gServerConfig.manrennum; }
#endif

int getLockType(void) {
  if (gServerConfig.locktype <= 0)
    return 0;
  return gServerConfig.locktype;
}

#ifdef _NEW_FUNC_DECRYPT
unsigned int getAllowerrornum2(void) { return gServerConfig.allowerrornum2; }
#endif
#ifdef _MO_LOGIN_NO_KICK
int getLoginNoKick(void) {
  if (gServerConfig.loginnokick == 1)
    return 1;
  return 0;
}
#endif

#ifdef _MO_ILLEGAL_NAME
char *getIllegalName(int index) {
  char illegalname[256];
  if (getStringFromIndexWithDelim(gServerConfig.illegalname, ",", index + 1,
                                  illegalname, sizeof(illegalname)) == TRUE) {
    return illegalname;
  } else {
    return "\0";
  }
}
#endif

#ifdef _NO_USE_PACKET_MAP
int getNoPacketMap(int index) {
  char NoPacketMap[256];
  if (getStringFromIndexWithDelim(gServerConfig.nousepacketmap, ",", index + 1,
                                  NoPacketMap, sizeof(NoPacketMap)) == TRUE) {
    return atoi(NoPacketMap);
  } else {
    return -1;
  }
}
#endif
#ifdef _NO_USE_MAGIC_MAP
int getNoMagicMap(int index) {
  char NoMagicMap[256];
  if (getStringFromIndexWithDelim(gServerConfig.nousemagicmap, ",", index + 1,
                                  NoMagicMap, sizeof(NoMagicMap)) == TRUE) {
    return atoi(NoMagicMap);
  } else {
    return -1;
  }
}
#endif

#ifdef _SOME_PETMAIL
int getPetMailFlg(void) {
  if (gServerConfig.petmailflg == 1) {
    return gServerConfig.petmailflg;
  }
  return 0;
}
int getPetMailPetid(unsigned int index) {
  char petid[256];
  if (getStringFromIndexWithDelim(gServerConfig.somepetmail, ",", index + 1,
                                  petid, sizeof(petid)) == TRUE) {
    return atoi(petid);
  } else {
    return -1;
  }
}
#endif
#ifdef _CTRL_TRANS_DEVELOP
int getCtrlTrans(void) {
  if (gServerConfig.ctrltrans == 1) {
    return gServerConfig.ctrltrans;
  }
  return 0;
}
#endif

#ifdef _PETMAIL_TIME
int getPetMailTime(void) {
  if (gServerConfig.petmailtime <= 0)
    return 0;
  return gServerConfig.petmailtime;
}
#endif
#ifdef _UP_BBPETPROB
int getUpBBProb(void) {
  if (gServerConfig.upbbprob == 1)
    return 1;
  return 0;
}
#endif
