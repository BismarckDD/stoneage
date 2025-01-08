#ifndef __LOG_H__
#define __LOG_H__

typedef enum {
  LOG_TALK,
  LOG_PROC,
  LOG_ITEM,
  LOG_STONE,
  LOG_PET,
  LOG_TENSEI,
  LOG_KILL,   // ttom 12/26/2000 kill the pet & items
  LOG_TRADE,  // CoolFish: 2001/4/19
  LOG_HACK,   // Arminius 2001/6/14
  LOG_SPEED,  // Nuke 0626
  LOG_FMPOP,  // CoolFish: 2001/9/12
  LOG_FAMILY, // Robin 10/02
  LOG_GM,     // Shan
#ifdef _GAMBLE_ROULETTE
  LOG_GAMBLE,
#endif
  LOG_LOGIN,
  PETTRANS,
  // Syu 增加庄园战胜负Log
  LOG_FMPKRESULT,
  // Syu ADD 新增家族个人银行存取Log (不含家族银行)
  LOG_BANKSTONELOG,
  LOG_ACMESS,
  LOG_PKCONTEND,
#ifdef _STREET_VENDOR
  LOG_STREET_VENDOR,
#endif

#ifdef _ANGEL_SUMMON
  LOG_ANGEL,
#endif

#ifdef _NEW_MANOR_LAW
  LOG_FMPK_GETMONEY,
#endif

  LOG_FM_FAME_SHOP,

#ifdef _AMPOINT_LOG
  LOG_AMPOINT,
#endif

#ifdef _SQL_VIPPOINT_LOG
  LOG_SQLVIPOINT,
#endif
#ifdef _NETLOG_
  LOG_LOGOUT,
#endif
  LOG_TYPE_NUM,
} LOG_TYPE;

void closeAllLogFile(void);
BOOL initLog(const char *filename);
void LogHelper(LOG_TYPE logtype, char *format, ...);

void LogAcMess(int fd, char *type, char *mess);

void LogItem(const char *char_name,
             const char *char_id,
             const int item_no,
             const char *cdkey,
             const int floor,
             const int x,
             const int y,
             const char *uniquecode,
             const char *item_name,
             const int item_id);
void LogPkContend(const char *team_name1,
                  const char *team_name2,
                  const int floor,
                  const int x, const int y,
                  const int flg);

void LogPetTrans(char *cdkey, char *uniwuecde, char *uniwuecde2, char *char_name,
                 int floor, int x, int y, int petID1, char *PetName1, int petLV,
                 int petrank, int vital1, int str1, int tgh1, int dex1,
                 int total1, int petID2, char *PetName2, int vital2, int str2,
                 int tgh2, int dex2, int total2, int work0, int work1,
                 int work2, int work3, int ans, int trans);

void LogPet(const char *char_name,
            const char *char_id,
            const char *pet_name,
            const int pet_lv,
            const char *cdkey,
            const int floor,
            const int x,
            const int y,
            const char *uniquecode);

#ifdef _STREET_VENDOR
void LogStreetVendor(char *sell_name,
                     char *SellID,
                     char *BuyName,char *BuyID,
                     char *ItemPetName,
                     int PetLv, // 若是道具此值为 -1
                     int iPrice, char *key, int Sfloor, int Sx, int Sy,
                     int Bfloor, int Bx, int By, char *uniquecode);
#endif

void LogTensei(char *char_name,          /* 平乓仿弁正   */
               char *CharID, char *key, /* 平□伐□玉 */
               int level,               // 伊矛伙
               int transNum,            // 鳖戏荚醒
               int quest,               // 弁巨旦玄醒
               int home,                // 请褥哗
               int item,                //  笛失奶  丞  井曰醒
               int pet,                 //  笛矢永玄  井曰醒
               int vital,               //  祭蟆Vital
               int b_vital,             //  祭  vital
               int str,                 //  祭蟆str
               int b_str,               //  祭  str
               int tgh,                 //  祭蟆
               int b_tgh,               //  祭
               int dex,                 //  祭蟆
               int b_dex                //  祭
);
// Syu ADD 新增家族个人银行存取Log (不含家族银行)
void LogFamilyBankStone(char *char_name, char *char_id, int Gold, int MyGold,
                        char *key, int floor, int x, int y, int banksum);

void LogStone(const int total_gold,
              const char *char_name,
              const char *char_id,
              const int gold,
              const int my_gold,
              const char *cdkey,
              const int floor,
              const int x, const int y);

void LogTalk(const char *char_name,
             const char *char_id,
             const int floor,
             const int x, const int y,
             const char *message);
// ttom 12/26/2000 kill pet & items
void LogKill(char *char_name, char *char_id, char *CharPet_Item);
// ttom

// CoolFish: 2001/4/19
void LogTrade(const char *message);
// CoolFish: 2001/9/12
void LogFMPOP(const char *message);

// Arminius 2001/6/14
enum {
  HACK_NOTHING,
  HACK_GETFUNCFAIL,
  HACK_NOTDISPATCHED,
  HACK_CHECKSUMERROR,
  HACK_HP,
  HACK_TYPE_NUM,
} HACK_TYPE;
void logHack(int fd, int errcode);
// Nuke 0626
void logSpeed(int fd);

void closeAllLogFile(void);
int openAllLogFile(void);

// Robin 10/02
void LogFamily(const char *family_name,
               const int family_index,
               const char *char_name,
               const char *char_id,
               const char *keyword,
               const char *data);

// Shan 11/02
void LogGM(const char *char_name,    // 角色名称
           const char *char_id,      // 玩家ID
           const char *instruction,  // 指令内容
           const int floor,
           const int x, const int y);

void LogLogin(const char *char_id,   // 玩家ID
              const char *char_name, // 角色名称
              const int save_index,
              const char *ipadress
#ifdef _NEWCLISETMAC
              ,
              char *mac
#endif
);

void LogCreatFUPet(char *PetName, int petid, int lv, int hp, int vital, int str,
                   int tgh, int dex, int fixstr, int fixtgh, int fixdex,
                   int trans, int flg);

#ifdef _GAMBLE_ROULETTE

void LogGamble(const char *char_name, // 角色名称
               const char *char_id,   // 玩家ID
               const char *cdkey,      // 说明
               const int floor,
               const int x, const int y,
               const int player_stone, // 所拥有金钱
               const int gamble_stone, // 下注本金
               const int get_stone,    // 获得
               const int gamble_num,
               const int flg); // flg = 1 玩家 2 庄家
#endif

void LogBankStone(const char *char_name,
                  const char *char_id,
                  const int meindex, int Gold,
                  const char *cdkey,
                  const int floor,
                  const int x, const int y,
                  const int my_gold,
                  const int my_personal_gold);

// Syu 增加庄园战胜负Log
void Logfmpk(char *winner, int winnerindex, int num1, char *loser,
             int loserindex, int num2, char *date, char *buf1, char *buf2,
             int flg);

#ifdef _NEW_MANOR_LAW
void LogFMPKGetMomey(char *szFMName, char *szID, char *szchar_name,
                     int iMomentum, int iGetMoney, int iDest);
#endif

void LogFMFameShop(char *szFMName, char *szID, char *szchar_name, int iFame,
                   int iCostFame);

void backupAllLogFile(struct tm *ptm);

void LogPetPointChange(char *char_name, char *CharID, char *PetName,
                       int petindex, int errtype, int PetLv, char *key,
                       int floor, int x, int y);

void LogPetFeed(char *char_name, char *CharID, char *PetName, int petindex,
                int PetLv, char *key, int floor, int x, int y, char *ucode);

#ifdef _ANGEL_SUMMON
void LogAngel(char *msg);
#endif

void warplog_to_file(void);
void warplog_from_file(void);

typedef struct {
  int floor;
  int incount;
  int outcount;
} tagWarplog;
#define MAXMAPNUM 700
extern tagWarplog warplog[MAXMAPNUM];

typedef struct {
  int floor1;
  int floor2;
  int count;
} tagWarpCount;
#define MAXMAPLINK 1000
extern tagWarpCount warpCount[MAXMAPLINK];

#ifdef _AMPOINT_LOG
void LogAmPoint(const char *char_name,
                const char *char_id,
                const int gold,
                const int my_am_point,
                const char *key,
                const int floor,
                const int x, const int y);
#endif

#ifdef _SQL_VIPPOINT_LOG
void LogSqlVipPoint(const char *char_name,
                    const char *char_id,
                    const char *cdkey,
                    const int vip_point,
                    const int floor,
                    const int x, const int y);
#endif

#endif
#ifdef _NETLOG_
void LogCharOut(const char *char_name,
                const char *char_id,
                const char *file,
                const char *function,
                const int line,
                const char *reason);

#endif
