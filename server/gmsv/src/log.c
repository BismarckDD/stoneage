#include "version.h"
#include "log.h"
#include "char_base.h"
#include "handletime.h"
#include "net.h"
#include "util.h"

struct tagLogconf {
  char *label;
  char *entry;
  char filename[256];
  FILE *f;
  BOOL append;
} LogConf[LOG_TYPE_NUM] = {
    {"TALK: ", "talklog", "", NULL, TRUE},
    {"PROC: ", "proc", "", NULL, FALSE},
    {"ITEM: ", "itemlog", "", NULL, TRUE},
    {"STONE: ", "stonelog", "", NULL, TRUE},
    {"PET: ", "petlog", "", NULL, TRUE},
    {"TENSEI: ", "tenseilog", "", NULL, TRUE},
    {"KILL: ", "killlog", "", NULL, TRUE},
    // CoolFish: 2001/4/19
    {"TRADE: ", "tradelog", "", NULL, TRUE},
    // Arminius: 2001/6/14
    {"HACK: ", "hacklog", "", NULL, TRUE},
    // Nuke: 0626 Speed
    {"SPEED: ", "speedlog", "", NULL, TRUE},
    // CoolFish: FMPopular 2001/9/12
    {"FMPOP: ", "fmpoplog", "", NULL, TRUE},
    // Robin 10/02
    {"FAMILY: ", "familylog", "", NULL, TRUE},
    // Shan 11/02
    {"GM: ", "gmlog", "", NULL, TRUE},
#ifdef _GAMBLE_ROULETTE
    {"", "gamblelog", "", NULL, TRUE},
#endif
    {"LOGIN: ", "loginlog", "", NULL, TRUE},
    {"", "pettranslog", "", NULL, TRUE},
    // Syu 增加庄园战胜负Log
    {"FMPKRESULT: ", "fmpkresultlog", "", NULL, TRUE},
    // Syu ADD 新增家族个人银行存取Log (不含家族银行)
    {"BANKSTONELOG: ", "bankstonelog", "", NULL, TRUE},
    {"ACMESSAGE: ", "acmessagelog", "", NULL, TRUE},
    {"PKCONTEND:", "pkcontendlog", "", NULL, TRUE},
#ifdef _STREET_VENDOR
    {"STREETVENDOR: ", "StreetVendorlog", "", NULL, TRUE},
#endif
#ifdef _ANGEL_SUMMON
    {"ANGEL: ", "angellog", "", NULL, TRUE},
#endif
#ifdef _NEW_MANOR_LAW
    {"FMPKGETMONEY: ", "FMPKGetMoneylog", "", NULL, TRUE},
#endif
    {"FMFAMESHOP: ", "FMFameShoplog", "", NULL, TRUE},
#ifdef _AMPOINT_LOG
    {"AMPOINT: ", "ampoint", "", NULL, TRUE},
#endif
#ifdef _SQL_VIPPOINT_LOG
    {"SQLVIPOINT: ", "sqlvippoint", "", NULL, TRUE},
#endif
#ifdef _NETLOG_
    {"LOGOUT: ", "Tplaylog", "", NULL, TRUE},
#endif
};

tagWarplog warplog[MAXMAPNUM];
tagWarpCount warpCount[MAXMAPLINK];

char local_time[256];
#define GET_LOCAL_TIME() \
struct tm s_tm;\
memcpy(&s_tm, localtime((time_t *)&NowTime.tv_sec), sizeof(s_tm));\
sprintf(local_time, " (%d-%d-%d %d:%d:%d) ", s_tm.tm_year + 1900, s_tm.tm_mon + 1,\
        s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min, s_tm.tm_sec);

void LogSaacHelper(LOG_TYPE log_type, const char *format, ...) {
#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, format, __VA_ARGS_);
    saacproto_OtherSaacLink_send(osfd, LogConf[log_type].filename, token);
  }
#endif
}

void LogHelper(LOG_TYPE log_type, char *format, ...) {
  va_list arg;
  if (log_type < 0 || log_type >= LOG_TYPE_NUM)
    return;
  if (LogConf[log_type].append) {
    if (!LogConf[log_type].f)
      return;
    fputs(LogConf[log_type].label, LogConf[log_type].f);
    va_start(arg, format);
    vfprintf(LogConf[log_type].f, format, arg);
    va_end(arg);
    fputc('\n', LogConf[log_type].f);
  } else {
    FILE *f = fopen(LogConf[log_type].filename, "w");
    if (!f)
      return;
    fputs(LogConf[log_type].label, f);
    va_start(arg, format);
    vfprintf(f, format, arg);
    va_end(arg);
    fputc('\n', f);
    fclose(f);
  }
}

static BOOL readLogConfFile(const char *filename) {
  FILE *f;
  char line[256];
  char basedir[256];
  int linenum = 0;
  {
    char *r;
    r = rindex(filename, '/');
    if (r == NULL)
      snprintf(basedir, sizeof(basedir), ".");
    else {
      memcpy(basedir, filename, r - filename);
      basedir[r - filename] = '\0';
    }
  }
  f = fopen(filename, "r");
  if (f == NULL) {
    print("Can't open %s\n", filename);
    return FALSE;
  }
  while (fgets(line, sizeof(line), f)) {
    char firstToken[256];
    int i;
    BOOL ret;
    linenum++;
    deleteWhiteSpace(line);
    if (line[0] == '#' || line[0] == '\n')
      continue; /* comment */
    chomp(line); /* remove tail newline  */
    ret = getStringFromIndexWithDelim(line, "=", 1, firstToken,
                                      sizeof(firstToken));
    if (ret == FALSE) {
      print("Find error at %s in line %d. Ignore\n", filename, linenum);
      continue;
    }
    for (i = 0; i < arraysizeof(LogConf); i++) {
      if (strcmp(LogConf[i].entry, firstToken) == 0) {
        char secondToken[256];
        ret = getStringFromIndexWithDelim(line, "=", 2, secondToken,
                                          sizeof(secondToken));
        if (ret == FALSE) {
          print("Find error at %s in line %d. Ignore\n", filename, linenum);
          continue;
        }

        char dirname[256];
        snprintf(dirname, sizeof(dirname), "%s/%d", basedir, getServernumber());
        if (opendir(dirname) == NULL) {
          if (mkdir(dirname, 0777) == 0) {
            printf("建立文件夹 %s\n", dirname);
          }
        }
        snprintf(LogConf[i].filename, sizeof(LogConf[i].filename), "%s/%d/%s",
                 basedir, getServernumber(), secondToken);
      }
    }
  }
  fclose(f);
  return TRUE;
}

int openAllLogFile(void) {
  int i;
  int opencount = 0;
  for (i = 0; i < arraysizeof(LogConf); i++) {
    if (!LogConf[i].append)
      continue;
    LogConf[i].f = fopen(LogConf[i].filename, "a");
    if (LogConf[i].f != NULL)
      opencount++;
  }
  return opencount;
}

void closeAllLogFile(void) {
  int i;
  struct tm local_time;
  memcpy(&local_time, localtime((time_t *)&NowTime.tv_sec), sizeof(local_time));

  // WON FIX
  for (i = 0; i < arraysizeof(LogConf); i++) {
    if (LogConf[i].f && LogConf[i].append) {
      LogHelper(i, "server down(%d-%d-%d %d:%d:%d) ", local_time.tm_year + 1900,
             local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
      fclose(LogConf[i].f);
    }
  }
}

BOOL initLog(const char *filename) {
  if (readLogConfFile(filename) == FALSE)
    return FALSE;
  openAllLogFile();
  return TRUE;
}

// Syu 增加庄园战胜负Log
void Logfmpk(char *winner, int winnerindex, int num1, char *loser,
             int loserindex, int num2, char *date, char *buf1, char *buf2,
             int flg) {
  switch (flg) {
  case 1: {
    GET_LOCAL_TIME();
    LogHelper(LOG_FMPKRESULT, "\nFMPK: [%s]地点:%s %s(%d) 约战要求 %s(%d) time:%s",
           buf1, buf2, winner, winnerindex, loser, loserindex, local_time);
  } break;
  case 2:
    LogHelper(LOG_FMPKRESULT, "\nFMPK: Winner %s(%d)=>%d Loser %s(%d)=>%d time:%s",
           winner, winnerindex, num1, loser, loserindex, num2, date);
    break;
  }
}

#ifdef _NEW_MANOR_LAW
void LogFMPKGetMomey(char *szFMName, char *szID, char *szchar_name,
                     int iMomentum, int iGetMoney, int iDest) {
  char szDest[3][6] = {"身上", "银行", "错误"};
  if (iDest < 0 || iDest > 1) iDest = 2;
  GET_LOCAL_TIME();
  LogHelper(LOG_FMPK_GETMONEY,
         "FMName:%s\tID:%s\tName:%s\tMomentum:%d\tGetMoney:%d\tAddTo:%s\t%s",
         szFMName, szID, szchar_name, iMomentum, iGetMoney, szDest[iDest], local_time);
}
#endif

void LogFMFameShop(char *szFMName, char *szID, char *szchar_name, int iFame,
                   int iCostFame) {
  GET_LOCAL_TIME();
  LogHelper(LOG_FM_FAME_SHOP,
         "FMName:%s\tID:%s\tName:%s\tFame:%d\tCostFame:%d\t%s", szFMName, szID,
         szchar_name, iFame, iCostFame, local_time);
}

void LogAcMess(int fd, char *type, char *mess) {
  if (strstr(mess, "Broadcast") != NULL)
    return;
  GET_LOCAL_TIME();
  LogHelper(LOG_ACMESS, "%d %s [%s] %s", fd, type, mess, local_time);
}

void LogItem(const char *char_name,
             const char *char_id,
             const int item_no,
             const char *cdkey,
             const int floor,
             const int x, const int y,
             const char *uniquecode,
             const char *item_name,
             const int item_id) {
  GET_LOCAL_TIME();
  LogHelper(LOG_ITEM, "%s\t%s\t%d(%s)=%s,(%d,%d,%d)%s,%s", char_name, char_id,
            item_id, item_name, cdkey, floor, x, y, local_time, uniquecode);
  LogSaacHelper(LOG_ITEM, "%s\t%s\t%d(%s)=%s,(%d,%d,%d)%s,%s", char_name, char_id,
                item_id, item_name, cdkey, floor, x, y, local_time, uniquecode);
}

void LogPkContend(const char *team_name1,
                  const char *team_name2,
                  const int floor,
                  const int x, const int y,
                  const int flg) {
  GET_LOCAL_TIME();
  if (flg == 0) {
    LogHelper(LOG_PKCONTEND, "[%32s 胜 %32s],(%5d,%4d,%4d)%s", team_name1,
           team_name2, floor, x, y, local_time);
  } else {
    LogHelper(LOG_PKCONTEND, "Msg:[%s],(%5d,%4d,%4d)%s", team_name1, floor, x, y,
           local_time);
  }
}

void LogPetTrans(char *cdkey, char *uniwuecde, char *uniwuecde2, char *char_name,
                 int floor, int x, int y, int pet_id1, char *pet_name1, int petLV,
                 int petrank, int vital1, int str1, int tgh1, int dex1,
                 int total1, int pet_id2, char *pet_name2, int vital2, int str2,
                 int tgh2, int dex2, int total2, int work0, int work1,
                 int work2, int work3, int ans, int trans) {
  GET_LOCAL_TIME();
  LogHelper(PETTRANS,
         "\n*PETTRANS cdkey=%s unid=%s munid=%s %s %s  %d=%s LV:%d rand:%d "
         "trans:%d :[ %d, %d, %d, %d]=%d  %d=%s :[ %d, %d, %d, %d]=%d  [ %d, "
         "%d, %d, %d]=%d\n",
         cdkey, uniwuecde, uniwuecde2, char_name, local_time, pet_id1, pet_name1, petLV,
         petrank, trans, vital1, str1, tgh1, dex1, total1, pet_id2, pet_name2,
         vital2, str2, tgh2, dex2, total2, work0, work1, work2, work3, ans);
}

void LogPet(const char *char_name,
            const char *char_id,
            const char *pet_name,
            const int pet_lv,
            const char *cdkey,
            const int floor,
            const int x, const int y,
            const char *uniquecode) {
  GET_LOCAL_TIME();
  LogHelper(LOG_PET, "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,%s", char_name, char_id, pet_name,
            pet_lv, cdkey, floor, x, y, local_time, uniquecode);
  LogSaacHelper(LOG_PET, "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,%s", char_name, char_id,
                pet_name, pet_lv, cdkey, floor, x, y, local_time, uniquecode);
}

#ifdef _STREET_VENDOR
void LogStreetVendor(char *SellName, char *SellID, char *BuyName, char *BuyID,
                     char *Itempet_name,
                     int PetLv, // 若是道具此值为 -1
                     int iPrice, char *cdkey, int Sfloor, int Sx, int Sy,
                     int Bfloor, int Bx, int By, char *uniquecode) {
  GET_LOCAL_TIME();
  LogHelper(LOG_STREET_VENDOR,
            "Sell:%s\t%s\tBuy:%s\t%s\tName=%s:Lv=%d|Price:%d,%s,SXY(%d,%d,%d)BXY(%"
            "d,%d,%d)%s,%s",
            SellName, SellID, BuyName, BuyID, Itempet_name, PetLv, iPrice, cdkey,
            Sfloor, Sx, Sy, Bfloor, Bx, By, local_time, uniquecode);
  LogSaacHelper(LOG_STREET_VENDOR,
                "Sell:%s\t%s\tBuy:%s\t%s\tName=%s:Lv=%d|Price:%d,%s,SXY(%d,%d,%d)"
                "BXY(%d,%d,%d)%s,%s",
                SellName, SellID, BuyName, BuyID, Itempet_name, PetLv, iPrice, cdkey,
                Sfloor, Sx, Sy, Bfloor, Bx, By, local_time, uniquecode);
}
#endif

void LogBankStone(const char *char_name,
                  const char *char_id,
                  const int meindex,
                  const int gold,
                  const char *cdkey,
                  const int floor,
                  const int x, int y,
                  const int my_gold,
                  const int my_personagold) {
  GET_LOCAL_TIME();
  LogHelper(LOG_STONE, "%s:%s\ts:%d=%s,(%d,%d,%d)%s <<own=%d,bank=%d>>", char_id,
         char_name, gold, cdkey, floor, x, y, local_time, my_gold, my_personagold);
}

void LogPetPointChange(char *char_name, char *char_id, char *pet_name,
                       int petindex, int errtype, int PetLv, char *cdkey,
                       int floor, int x, int y) {

  struct tm local_time;
  int vit, str, tgh, dex;
  int l_vit, l_str, l_tgh, l_dex;
  int pet_ID, levellvup;

  pet_ID = CHAR_getInt(petindex, CHAR_PETID);
  vit = CHAR_getInt(petindex, CHAR_VITAL);
  str = CHAR_getInt(petindex, CHAR_STR);
  tgh = CHAR_getInt(petindex, CHAR_TOUGH);
  dex = CHAR_getInt(petindex, CHAR_DEX);
  levellvup = CHAR_getInt(petindex, CHAR_ALLOCPOINT);

  l_vit = (levellvup >> 24);
  l_str = (levellvup >> 16) & 0xff;
  l_tgh = (levellvup >> 8) & 0xff;
  l_dex = (levellvup >> 0) & 0xff;

  memcpy(&local_time, localtime((time_t *)&NowTime.tv_sec), sizeof(local_time));

  LogHelper(LOG_PET,
         "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,err:%d "
         "%d<<%d,%d,%d,%d>>lvup<<%d,%d,%d,%d>>",
         char_name, char_id, pet_name, PetLv, cdkey, floor, x, y, local_time, errtype,
         pet_ID, vit, str, tgh, dex, l_vit, l_str, l_tgh, l_dex);
}

void LogTensei(char *char_name,          /* 平乓仿弁正   */
               char *char_id, char *cdkey, /* 平□伐□玉 */
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
) {
	GET_LOCAL_TIME();
  LogHelper(LOG_TENSEI,
         "%s\t%s\t%s=(%d,%d,%d,%d,%d,%d),(vi=%d->%d,str=%d->%d,tgh=%d->%d,dex=%d->%d),(%d,%d)",
         char_name, char_id, cdkey, level, transNum, quest, home, item, pet, vital,
         b_vital, str, b_str, tgh, b_tgh, dex, b_dex, local_time);
}

// LOG_TALK
void LogTalk(const char *char_name,
             const char *char_id,
             const int floor,
             const int x,
             const int y,
             const char *message) {
  GET_LOCAL_TIME();
  LogHelper(LOG_TALK, "%2d:%2d\t%s\t%s\t%d_%d_%d\tT=%s", local_time,
         (char_id == NULL) ? "(null)" : char_id,
         (char_name == NULL) ? "(null)" : char_name, floor, x, y, message);
}

void backupAllLogFile(struct tm *ptm) {
  int i;
  char szBuffer[256];
  for (i = 0; i < arraysizeof(LogConf); i++) {
    if (!LogConf[i].append)
      continue;
    sprintf(szBuffer, "%s.%4d%02d%02d", LogConf[i].filename,
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    if (LogConf[i].f != NULL) {
      fclose(LogConf[i].f);
      rename(LogConf[i].filename, szBuffer);
      LogConf[i].f = fopen(LogConf[i].filename, "a");
    } else {
      rename(LogConf[i].filename, szBuffer);
      LogConf[i].f = fopen(LogConf[i].filename, "a");
    }
  }
}
// Syu ADD 新增家族个人银行存取Log (不含家族银行)
void LogFamilyBankStone(char *char_name, char *char_id, int gold, int my_gold,
                        char *cdkey, int floor, int x, int y, int banksum) {
  GET_LOCAL_TIME();
  LogHelper(LOG_BANKSTONELOG, "%s:%s\t%d=%s [%d] CHAR_GOLD(%d),(%d,%d,%d)%s",
         char_id, char_name, gold, cdkey, banksum, my_gold, floor, x, y, local_time);
  print("\n%s:%s\t%d=%s [%d] CHAR_GOLD(%d),(%d,%d,%d)%s\n", char_id, char_name,
        gold, cdkey, banksum, my_gold, floor, x, y, local_time);
}

void LogStone(const int total_gold,
              const char *char_name,
              const char *char_id,
              const int gold,
              const int my_gold,
							const char *cdkey,
              const int floor,
              const int x, const int y) {
	GET_LOCAL_TIME();
  if (total_gold == -1) {
    LogHelper(LOG_STONE, "%s:%s\t%d=%s TOTAL_GOLD(%d),CHAR_GOLD(%d),(%d,%d,%d)%s",
           char_id, char_name, gold, cdkey, total_gold, my_gold, floor, x, y, local_time);
  } else {
    LogHelper(LOG_STONE, "%s:%s\t%d=%s CHAR_GOLD(%d),(%d,%d,%d)%s", char_id,
           char_name, gold, cdkey, my_gold, floor, x, y, local_time);
  }
}

// ttom 12/26/2000 print the kill log
void LogKill(char *char_name, char *char_id, char *CharPet_Item) {
	GET_LOCAL_TIME();
  LogHelper(LOG_KILL, "Name=%s:ID=%s\t%s %s", char_name, char_id, CharPet_Item, local_time);
}

// CoolFish: Trade 2001/4/19
void LogTrade(const char *message) {
  GET_LOCAL_TIME();
  LogHelper(LOG_TRADE, "%s %s", message, local_time);
  LogSaacHelper(LOG_TRADE, "%s %s", message, local_time);
}

// CoolFish: Family Popular 2001/9/12
void LogFMPOP(const char *message) {
  GET_LOCAL_TIME();
  LogHelper(LOG_FMPOP, "%s %s", message, local_time);
}

// Arminius 2001/6/14
const char hackmsg[HACK_TYPE_NUM][1024] = {
    "??? 什麽事也没有发生",
    "无法取得通讯协定码",
    "收到无法辨识的通讯协定码",
    "检查码错误",
    "人物的HP为负",
};

void logHack(int fd, int errcode) {
  char cdkey[4096];
  char charname[4096];
  unsigned int ip;
  char ipstr[4096];
  GET_LOCAL_TIME();
  CONNECT_getCdkey(fd, cdkey, 4096);
  CONNECT_getCharname(fd, charname, 4096);
  ip = CONNECT_get_userip(fd);
  sprintf(ipstr, "%d.%d.%d.%d", ((unsigned char *)&ip)[0],
          ((unsigned char *)&ip)[1], ((unsigned char *)&ip)[2],
          ((unsigned char *)&ip)[3]);
  if ((errcode < 0) || (errcode >= HACK_TYPE_NUM))
    errcode = HACK_NOTHING;
  LogHelper(LOG_HACK, "%s %s ip=%s cdkey=%s charname=%s", local_time, hackmsg[errcode],
         ipstr, cdkey, charname);
}

// Nuke 0626
void logSpeed(int fd) {
  char cdkey[4096];
  char charname[4096];
  unsigned long ip;
  char ipstr[4096];
  GET_LOCAL_TIME();
  CONNECT_getCdkey(fd, cdkey, 4096);
  CONNECT_getCharname(fd, charname, 4096);
  ip = CONNECT_get_userip(fd);
  sprintf(ipstr, "%d.%d.%d.%d", ((unsigned char *)&ip)[0],
          ((unsigned char *)&ip)[1], ((unsigned char *)&ip)[2],
          ((unsigned char *)&ip)[3]);
  LogHelper(LOG_SPEED, "%s ip=%s cdkey=%s charname=%s", local_time, ipstr, cdkey,
         charname);
}

// Shan
void LogGM(const char *char_name, // 角色名称
           const char *char_id,   // 玩家ID
           const char *instruction,  // 指令内容
           const int floor,
           const int x, const int y) {
  GET_LOCAL_TIME();
  LogHelper(LOG_GM, "%s\t%s\t%s\t(%d,%d,%d)\t%s", char_name, char_id, instruction, floor,
         x, y, local_time);
  LogSaacHelper(LOG_GM, "%s\t%s\t%s\t(%d,%d,%d)\t%s", char_name, char_id, instruction, floor,
         x, y, local_time);
}

void LogFamily(const char *family_name, const int family_index,
               const char *char_name, const char *char_id, const char *keyword,
               const char *data) {
  GET_LOCAL_TIME();
  LogHelper(LOG_FAMILY, "%s\t%d\t%s\t%s\t= %s, %s %s", family_name, family_index,
         char_name, char_id, keyword, data, local_time);
}

#ifdef _GAMBLE_ROULETTE
void LogGamble(const char *char_name, // 角色名称
               const char *char_id,   // 玩家ID
               const char *key,      // 说明
               int floor, int x, int y,
               int player_stone, // 所拥有金钱
               int gamble_stone, // 下注本金
               int get_stone,    // 获得
               int gamble_num,
               int flg // flg = 1 玩家 2 庄家
) {
	GET_LOCAL_TIME();
  if (flg == 1) {
    LogHelper(LOG_GAMBLE,
           "%s\t%s\t TYPE:%s  <<P_STONE:%9d,G_STONE:%9d,GET:%9d "
           ">>\t(%d,%d,%d)-%s GAMBLENUM=%d",
           char_name, char_id, key, player_stone, gamble_stone, get_stone, floor,
           x, y, local_time, gamble_num);
  } else if (flg == 2) {
    LogHelper(
        LOG_GAMBLE,
        "%s\tROULETTE MASTER\t TYPE:%s  <<MASTER_STONE:%24d >>\t(%d,%d,%d)-%s",
        char_name, key, player_stone, floor, x, y, local_time);
  }
}

#endif

void LogLogin(const char *char_id,   // 玩家ID
              const char *char_name, // 角色名称
              const int save_index,
              const char *ipadress
#ifdef _NEWCLISETMAC
              , char *mac
#endif
) {
  GET_LOCAL_TIME();
#ifdef _NEWCLISETMAC
  LogHelper(LOG_LOGIN, "%s\t%s\ti=%d\t%s\t%s\t%s\t%s", char_id, char_name, save_index,
         getGameservername(), ipadress, mac, local_time);
#else
  LogHelper(LOG_LOGIN, "%s\t%s\ti=%d\t%s\t%s\t%s", char_id, char_name, save_index,
         getGameservername(), ipadress, local_time);
#endif
}

void warplog_to_file() {
  int i = 0;
  char outbuf[128];
  FILE *f;
  f = fopen("log/warp1.log", "w");
  if (!f)
    return;

  for (i = 0; i < MAXMAPNUM; i++) {
    if (warplog[i].floor <= 0)
      continue;
    sprintf(outbuf, "%6d,%10d,%10d\n", warplog[i].floor, warplog[i].incount,
            warplog[i].outcount);
    fputs(outbuf, f);
  }
  fclose(f);

  f = fopen("log/warp2.log", "w");
  if (!f)
    return;

  for (i = 0; i < MAXMAPLINK; i++) {
    if (warpCount[i].floor1 <= 0)
      continue;
    sprintf(outbuf, "%6d,%6d,%10d\n", warpCount[i].floor1, warpCount[i].floor2,
            warpCount[i].count);
    fputs(outbuf, f);
  }
  fclose(f);
}

void warplog_from_file() {
  int i = 0;
  char outbuf[128];
  FILE *f;
  print("warplog_from_file ");
  f = fopen("log/warp1.log", "r");
  if (!f)
    return;
  while (fgets(outbuf, sizeof(outbuf), f) && i < MAXMAPNUM) {
    if (!sscanf(outbuf, "%d,%d,%d", &warplog[i].floor, &warplog[i].incount,
                &warplog[i].outcount)) {
      continue;
    }
    // print(" %d", warplog[i].floor);
    i++;
  }
  print(" read_count:%d\n", i);
  fclose(f);
  f = fopen("log/warp2.log", "r");
  if (!f)
    return;
  i = 0;
  while (fgets(outbuf, sizeof(outbuf), f) && i < MAXMAPLINK) {
    if (!sscanf(outbuf, "%d,%d,%d", &warpCount[i].floor1, &warpCount[i].floor2,
                &warpCount[i].count)) {
      continue;
    }
    i++;
  }
  print(" read_count2:%d\n", i);
  fclose(f);
}

void LogPetFeed(char *char_name, char *char_id, char *pet_name, int petindex,
                int PetLv, char *cdkey, int floor, int x, int y, char *ucode) {

  GET_LOCAL_TIME();
  LogHelper(LOG_PET, "%s\t%s\t%s:%d 喂蛋=%s (%d,%d,%d)%s %s ", char_name, char_id,
         pet_name, PetLv, cdkey, floor, x, y, local_time, ucode);
}

#ifdef _ANGEL_SUMMON
void LogAngel(char *msg) {
	GET_LOCAL_TIME();
  LogHelper(LOG_ANGEL, "%s %s ", msg, local_time);
}
#endif

#ifdef _AMPOINT_LOG
void LogAmPoint(const char *char_name,
                const char *char_id,
                const int am_point,
                const int my_am_point,
                const char *cdkey,
                const int floor,
                const int x, const int y) {
  GET_LOCAL_TIME();
  LogHelper(LOG_AMPOINT, "%s:%s\t%d=%s CHAR_AMPOINT(%d),(%d,%d,%d)%s", char_id,
         char_name, am_point, cdkey, my_am_point, floor, x, y, local_time);
  LogSaacHelper(LOG_AMPOINT, "%s:%s\t%d=%s CHAR_AMPOINT(%d),(%d,%d,%d)%s", char_id,
           char_name, am_point, cdkey, my_am_point, floor, x, y, local_time);
}
#endif

#ifdef _SQL_VIPPOINT_LOG
void LogSqlVipPoint(const char *char_name,
                    const char *char_id,
                    const char *cdkey,
                    const int vip_point,
                    const int floor,
                    const int x, const int y) {
  GET_LOCAL_TIME();
  LogHelper(LOG_SQLVIPOINT, "%s:%s\t%s:%d,(%d,%d,%d)%s", char_id, char_name, cdkey,
         vip_point, floor, x, y, local_time);
  LogSaacHelper(LOG_SQLVIPOINT, "%s:%s\t%s:%d,(%d,%d,%d)%s", char_id, char_name, cdkey,
           vip_point, floor, x, y, local_time);
}
#endif
#ifdef _NETLOG_
void LogCharOut(const char *char_name,
                const char *char_id,
                const char *file,
                const char *function,
                const int line,
                const char *reason) {
  GET_LOCAL_TIME();
  LogHelper(LOG_LOGOUT, "%s: 帐号:%s 玩家名:%s 文件:%s 函数:%s 行数:%d 原因:%s",
         local_time, char_id, char_name, file, function, line, reason);
}
#endif
