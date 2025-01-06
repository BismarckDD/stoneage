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
    deleteWhiteSpace(line); /* remove whitespace    */
    if (line[0] == '#')
      continue; /* comment */
    if (line[0] == '\n')
      continue;  /* none    */
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
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

  // WON FIX
  for (i = 0; i < arraysizeof(LogConf); i++) {
    if (LogConf[i].f && LogConf[i].append) {
      printl(i, "server down(%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900,
             tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
      fclose(LogConf[i].f);
    }
  }
}

void printl(LOG_TYPE logtype, char *format, ...) {
  va_list arg;
  if (logtype < 0 || logtype >= LOG_TYPE_NUM)
    return;
  if (LogConf[logtype].append) {
    if (!LogConf[logtype].f)
      return;
    fputs(LogConf[logtype].label, LogConf[logtype].f);
    va_start(arg, format);
    vfprintf(LogConf[logtype].f, format, arg);
    va_end(arg);
    fputc('\n', LogConf[logtype].f);
  } else {
    FILE *f = fopen(LogConf[logtype].filename, "w");
    if (!f)
      return;
    fputs(LogConf[logtype].label, f);
    va_start(arg, format);
    vfprintf(f, format, arg);
    va_end(arg);
    fputc('\n', f);
    fclose(f);
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
    struct tm tm1;
    char buf[256];
    memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
    sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
            tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
    printl(LOG_FMPKRESULT, "\nFMPK: [%s]地点:%s %s(%d) 约战要求 %s(%d) time:%s",
           buf1, buf2, winner, winnerindex, loser, loserindex, buf);
  } break;
  case 2:
    printl(LOG_FMPKRESULT, "\nFMPK: Winner %s(%d)=>%d Loser %s(%d)=>%d time:%s",
           winner, winnerindex, num1, loser, loserindex, num2, date);
    break;
  }
}

#ifdef _NEW_MANOR_LAW
void LogFMPKGetMomey(char *szFMName, char *szID, char *szchar_name,
                     int iMomentum, int iGetMoney, int iDest) {
  struct tm tm1;
  char szDest[3][6] = {"身上", "银行", "错误"};

  if (iDest < 0 || iDest > 1)
    iDest = 2;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_FMPK_GETMONEY,
         "FMName:%s\tID:%s\tName:%s\tMomentum:%d\tGetMoney:%d\tAddTo:%s\t%s",
         szFMName, szID, szchar_name, iMomentum, iGetMoney, szDest[iDest], buf);
}
#endif

void LogFMFameShop(char *szFMName, char *szID, char *szchar_name, int iFame,
                   int iCostFame) {
  struct tm tm1;

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_FM_FAME_SHOP,
         "FMName:%s\tID:%s\tName:%s\tFame:%d\tCostFame:%d\t%s", szFMName, szID,
         szchar_name, iFame, iCostFame, buf);
}

void LogAcMess(int fd, char *type, char *mess) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  if (strstr(mess, "Broadcast") != NULL)
    return;
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_ACMESS, "%d %s [%s] %s", fd, type, mess, buf);
}

void LogItem(char *char_name, /* 平乓仿弁正   */
             char *char_id,   /* 平乓仿弁正ID */
             int ItemNo,     /* 失奶  丞  寞 */
             char *Key,      /* 平□伐□玉 */
             int floor,      /* 甄   */
             int x, int y,
             char *uniquecode, // shan 2001/12/14
             char *itemname, int itemID) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_ITEM, "%s\t%s\t%d(%s)=%s,(%d,%d,%d)%s,%s", char_name, char_id,
         itemID, itemname, Key, floor, x, y, buf, uniquecode);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    char buf[256];
    sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
            tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

    sprintf(token, "%s\t%s\t%d(%s)=%s,(%d,%d,%d)%s,%s", char_name, char_id,
            itemID, itemname, Key, floor, x, y, buf, uniquecode);

    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_ITEM].filename, token);
  }
#endif
}
void LogPkContend(char *teamname1, char *teamname2, int floor, int x, int y,
                  int flg) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  if (flg == 0) {
    printl(LOG_PKCONTEND, "[%32s 胜 %32s],(%5d,%4d,%4d)%s", teamname1,
           teamname2, floor, x, y, buf);
  } else {
    printl(LOG_PKCONTEND, "Msg:[%s],(%5d,%4d,%4d)%s", teamname1, floor, x, y,
           buf);
  }
}

void LogPetTrans(char *cdkey, char *uniwuecde, char *uniwuecde2, char *char_name,
                 int floor, int x, int y, int petID1, char *PetName1, int petLV,
                 int petrank, int vital1, int str1, int tgh1, int dex1,
                 int total1, int petID2, char *PetName2, int vital2, int str2,
                 int tgh2, int dex2, int total2, int work0, int work1,
                 int work2, int work3, int ans, int trans) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(PETTRANS,
         "\n*PETTRANS cdkey=%s unid=%s munid=%s %s %s  %d=%s LV:%d rand:%d "
         "trans:%d :[ %d, %d, %d, %d]=%d  %d=%s :[ %d, %d, %d, %d]=%d  [ %d, "
         "%d, %d, %d]=%d\n",
         cdkey, uniwuecde, uniwuecde2, char_name, buf, petID1, PetName1, petLV,
         petrank, trans, vital1, str1, tgh1, dex1, total1, petID2, PetName2,
         vital2, str2, tgh2, dex2, total2, work0, work1, work2, work3, ans);
}
/*------------------------------------------------------------
 *
 * 矢永玄夫弘毛潸月
 *
-------------------------------------------------------------*/
void LogPet(char *char_name, /* 平乓仿弁正   */
            char *char_id, char *PetName, int PetLv, char *Key, /* 平□伐□玉 */
            int floor,                                         /* 甄   */
            int x, int y,
            char *uniquecode // shan 2001/12/14
) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  // shan 2001/12/14
  // printl( LOG_PET, "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s" , char_name, char_id,
  //		PetName, PetLv,
  //		Key,
  //		floor, x, y, buf );

  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_PET, "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,%s", char_name, char_id, PetName,
         PetLv, Key, floor, x, y, buf, uniquecode);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,%s", char_name, char_id,
            PetName, PetLv, Key, floor, x, y, buf, uniquecode);

    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_PET].filename, token);
  }
#endif
}

#ifdef _STREET_VENDOR
void LogStreetVendor(char *SellName, char *SellID, char *BuyName, char *BuyID,
                     char *ItemPetName,
                     int PetLv, // 若是道具此值为 -1
                     int iPrice, char *Key, int Sfloor, int Sx, int Sy,
                     int Bfloor, int Bx, int By, char *uniquecode) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_STREET_VENDOR,
         "Sell:%s\t%s\tBuy:%s\t%s\tName=%s:Lv=%d|Price:%d,%s,SXY(%d,%d,%d)BXY(%"
         "d,%d,%d)%s,%s",
         SellName, SellID, BuyName, BuyID, ItemPetName, PetLv, iPrice, Key,
         Sfloor, Sx, Sy, Bfloor, Bx, By, buf, uniquecode);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token,
            "Sell:%s\t%s\tBuy:%s\t%s\tName=%s:Lv=%d|Price:%d,%s,SXY(%d,%d,%d)"
            "BXY(%d,%d,%d)%s,%s",
            SellName, SellID, BuyName, BuyID, ItemPetName, PetLv, iPrice, Key,
            Sfloor, Sx, Sy, Bfloor, Bx, By, buf, uniquecode);

    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_STREET_VENDOR].filename,
                                 token);
  }
#endif
}
#endif

void LogBankStone(char *char_name,        /* 平乓仿弁正   */
                  char *char_id,          /* 交□扒□ID */
                  int meindex, int Gold, /* 嗯喊 */
                  char *Key,             /* 平□伐□玉 */
                  int floor,             /* 甄   */
                  int x, int y, int my_gold, int my_personagold

) {
  struct tm tm1;

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_STONE, "%s:%s\ts:%d=%s,(%d,%d,%d)%s <<own=%d,bank=%d>>", char_id,
         char_name, Gold, Key, floor, x, y, buf, my_gold, my_personagold);
}

void LogPetPointChange(char *char_name, char *char_id, char *PetName,
                       int petindex, int errtype, int PetLv, char *Key,
                       int floor, int x, int y) {

  struct tm tm1;
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

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_PET,
         "%s\t%s\t%s:%d=%s,(%d,%d,%d)%s,err:%d "
         "%d<<%d,%d,%d,%d>>lvup<<%d,%d,%d,%d>>",
         char_name, char_id, PetName, PetLv, Key, floor, x, y, buf, errtype,
         pet_ID, vit, str, tgh, dex, l_vit, l_str, l_tgh, l_dex);
}

/*------------------------------------------------------------
 *
 * 鳖戏夫弘毛潸月
 *
-------------------------------------------------------------*/
void LogTensei(char *char_name,          /* 平乓仿弁正   */
               char *char_id, char *Key, /* 平□伐□玉 */
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
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_TENSEI,
         "%s\t%s\t%s=(%d,%d,%d,%d,%d,%d),(vi=%d->%d,str=%d->%d,tgh=%d->%d,dex=%"
         "d->%d),(%d,%d)",
         char_name, char_id, Key, level, transNum, quest, home, item, pet, vital,
         b_vital, str, b_str, tgh, b_tgh, dex, b_dex, buf);
}

// LOG_TALK
void LogTalk(char *char_name,          /* 平乓仿弁正   */
             char *char_id, int floor, /* 甄   */
             int x, int y, char *message) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_TALK, "%2d:%2d\t%s\t%s\t%d_%d_%d\tT=%s", buf,
         (char_id == NULL) ? "(null)" : char_id,
         (char_name == NULL) ? "(null)" : char_name, floor, x, y, message);
}

void backupAllLogFile(struct tm *ptm) {
  int i;
  char szBuffer[256];

  for (i = 0; i < arraysizeof(LogConf); i++) {
    /* append 匹卅中手及反仄卅中 */
    if (!LogConf[i].append)
      continue;

    sprintf(szBuffer, "%s.%4d%02d%02d", LogConf[i].filename,
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

    if (LogConf[i].f != NULL) {
      /* 左□皿件今木化中凶日弁夫□术 */
      fclose(LogConf[i].f);
      /* 伉生□丞 */
      rename(LogConf[i].filename, szBuffer);
      /* 疯太左□皿件 */
      LogConf[i].f = fopen(LogConf[i].filename, "a");

    } else {
      /* 伉生□丞 */
      rename(LogConf[i].filename, szBuffer);
      /* 疯太左□皿件 */
      LogConf[i].f = fopen(LogConf[i].filename, "a");
    }
  }
}
/*------------------------------------------------------------
*
* 云嗯毛胶丹
*
-------------------------------------------------------------*/
// Syu ADD 新增家族个人银行存取Log (不含家族银行)
void LogFamilyBankStone(char *char_name, char *char_id, int Gold, int MyGold,
                        char *Key, int floor, int x, int y, int banksum) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_BANKSTONELOG, "%s:%s\t%d=%s [%d] CHAR_GOLD(%d),(%d,%d,%d)%s",
         char_id, char_name, Gold, Key, banksum, MyGold, floor, x, y, buf);
  print("\n%s:%s\t%d=%s [%d] CHAR_GOLD(%d),(%d,%d,%d)%s\n", char_id, char_name,
        Gold, Key, banksum, MyGold, floor, x, y, buf);
}

void LogStone(int TotalGold, char *char_name, /* 平乓仿弁正   */
              char *char_id,                  /* 交□扒□ID */
              int Gold,                      /* 嗯喊 */
              int MyGold, char *Key,         /* 平□伐□玉 */
              int floor,                     /* 甄   */
              int x, int y) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  if (TotalGold == -1) {
    printl(LOG_STONE, "%s:%s\t%d=%s TOTAL_GOLD(%d),CHAR_GOLD(%d),(%d,%d,%d)%s",
           char_id, char_name, Gold, Key, TotalGold, MyGold, floor, x, y, buf);
  } else {
    printl(LOG_STONE, "%s:%s\t%d=%s CHAR_GOLD(%d),(%d,%d,%d)%s", char_id,
           char_name, Gold, Key, MyGold, floor, x, y, buf);
  }
}

// ttom 12/26/2000 print the kill log
void LogKill(char *char_name, char *char_id, char *CharPet_Item) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec, tm1.tm_sec);

  printl(LOG_KILL, "Name=%s:ID=%s\t%s %s", char_name, char_id, CharPet_Item, buf);
}
// ttom

// CoolFish: Trade 2001/4/19
void LogTrade(char *message) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_TRADE, "%s %s", message, buf);
#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, "%s %s", message, buf);

    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_TRADE].filename, token);
  }
#endif
}

// CoolFish: Family Popular 2001/9/12
void LogFMPOP(char *message) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min);

  printl(LOG_FMPOP, "%s %s", message, buf);
}

// Arminius 2001/6/14
char hackmsg[HACK_TYPE_NUM][4096] = {
    "??? 什麽事也没有发生", "无法取得通讯协定码", "收到无法辨识的通讯协定码",
    "检查码错误",           "人物的HP为负",
};

void logHack(int fd, int errcode) {
  struct tm tm1;
  char cdkey[4096];
  char charname[4096];
  unsigned long ip;
  char ipstr[4096];

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  CONNECT_getCdkey(fd, cdkey, 4096);
  CONNECT_getCharname(fd, charname, 4096);
  ip = CONNECT_get_userip(fd);
  sprintf(ipstr, "%d.%d.%d.%d", ((unsigned char *)&ip)[0],
          ((unsigned char *)&ip)[1], ((unsigned char *)&ip)[2],
          ((unsigned char *)&ip)[3]);
  if ((errcode < 0) || (errcode >= HACK_TYPE_NUM))
    errcode = HACK_NOTHING;

  printl(LOG_HACK, "%s %s ip=%s cdkey=%s charname=%s", buf, hackmsg[errcode],
         ipstr, cdkey, charname);
}

// Nuke 0626
void logSpeed(int fd) {
  struct tm tm1;
  char cdkey[4096];
  char charname[4096];
  unsigned long ip;
  char ipstr[4096];

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  CONNECT_getCdkey(fd, cdkey, 4096);
  CONNECT_getCharname(fd, charname, 4096);
  ip = CONNECT_get_userip(fd);
  sprintf(ipstr, "%d.%d.%d.%d", ((unsigned char *)&ip)[0],
          ((unsigned char *)&ip)[1], ((unsigned char *)&ip)[2],
          ((unsigned char *)&ip)[3]);
  printl(LOG_SPEED, "%s ip=%s cdkey=%s charname=%s", buf, ipstr, cdkey,
         charname);
}

// Shan
void LogGM(char *char_name, // 角色名称
           char *char_id,   // 玩家ID
           char *Message,  // 指令内容
           int floor, int x, int y) {
  struct tm tm1;

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_GM, "%s\t%s\t%s\t(%d,%d,%d)\t%s", char_name, char_id, Message, floor,
         x, y, buf);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, "%s\t%s\t%s\t(%d,%d,%d)\t%s", char_name, char_id, Message,
            floor, x, y, buf);

    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_GM].filename, token);
  }
#endif
}

void LogFamily(const char *family_name, const int family_index,
               const char *char_name, const char *char_id, const char *keyword,
               const char *data) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  printl(LOG_FAMILY, "%s\t%d\t%s\t%s\t= %s, %s %s", family_name, family_index,
         char_name, char_id, keyword, data, buf);
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
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  if (flg == 1) {
    printl(LOG_GAMBLE,
           "%s\t%s\t TYPE:%s  <<P_STONE:%9d,G_STONE:%9d,GET:%9d "
           ">>\t(%d,%d,%d)-%s GAMBLENUM=%d",
           char_name, char_id, key, player_stone, gamble_stone, get_stone, floor,
           x, y, buf, gamble_num);
  } else if (flg == 2) {
    printl(
        LOG_GAMBLE,
        "%s\tROULETTE MASTER\t TYPE:%s  <<MASTER_STONE:%24d >>\t(%d,%d,%d)-%s",
        char_name, key, player_stone, floor, x, y, buf);
  }
}

#endif

void LogLogin(char *char_id,   // 玩家ID
              char *char_name, // 角色名称
              int saveIndex, char *ipadress
#ifdef _NEWCLISETMAC
              ,
              char *mac
#endif
) {
  struct tm tm1;

  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

#ifdef _NEWCLISETMAC
  printl(LOG_LOGIN, "%s\t%s\ti=%d\t%s\t%s\t%s\t%s", char_id, char_name, saveIndex,
         getGameservername(), ipadress, mac, buf);
#else
  printl(LOG_LOGIN, "%s\t%s\ti=%d\t%s\t%s\t%s", char_id, char_name, saveIndex,
         getGameservername(), ipadress, buf);
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

void LogPetFeed(char *char_name, char *char_id, char *PetName, int petindex,
                int PetLv, char *Key, int floor, int x, int y, char *ucode) {

  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  printl(LOG_PET, "%s\t%s\t%s:%d 喂蛋=%s (%d,%d,%d)%s %s ", char_name, char_id,
         PetName, PetLv, Key, floor, x, y, buf, ucode);
}

#ifdef _ANGEL_SUMMON
void LogAngel(char *msg) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  printl(LOG_ANGEL, "%s %s ", msg, buf);
}
#endif

#ifdef _AMPOINT_LOG
void LogAmPoint(char *char_name,           /* 平乓仿弁正   */
                char *char_id,             /* 交□扒□ID */
                int AmPoint,              /* 嗯喊 */
                int MyAmPoint, char *Key, /* 平□伐□玉 */
                int floor,                /* 甄   */
                int x, int y) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_AMPOINT, "%s:%s\t%d=%s CHAR_AMPOINT(%d),(%d,%d,%d)%s", char_id,
         char_name, AmPoint, Key, MyAmPoint, floor, x, y, buf);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, "%s:%s\t%d=%s CHAR_AMPOINT(%d),(%d,%d,%d)%s", char_id,
            char_name, AmPoint, Key, MyAmPoint, floor, x, y, buf);
    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_AMPOINT].filename, token);
  }
#endif
}
#endif

#ifdef _SQL_VIPPOINT_LOG
void LogSqlVipPoint(char *char_name, /* 平乓仿弁正   */
                    char *char_id,   /* 交□扒□ID */
                    char *Key,      /* 平□伐□玉 */
                    int VipPoint,   /* 嗯喊 */
                    int floor,      /* 甄   */
                    int x, int y) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[256];
  sprintf(buf, " (%d-%d-%d %d:%d:%d) ", tm1.tm_year + 1900, tm1.tm_mon + 1,
          tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

  printl(LOG_SQLVIPOINT, "%s:%s\t%s:%d,(%d,%d,%d)%s", char_id, char_name, Key,
         VipPoint, floor, x, y, buf);

#ifdef _OTHER_SAAC_LINK
  if (osfd == -1) {
    OtherSaacConnect();
  } else {
    char token[1024];
    sprintf(token, "%s:%s\t%s:%d,(%d,%d,%d)%s", char_id, char_name, Key, VipPoint,
            floor, x, y, buf);
    saacproto_OtherSaacLink_send(osfd, LogConf[LOG_SQLVIPOINT].filename, token);
  }
#endif
}
#endif
#ifdef _NETLOG_
void LogCharOut(char *char_name, char *char_id, char *file, char *fun, int ilne,
                char *yuanyin) {
  struct tm tm1;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
  char buf[512];
  snprintf(
      buf, sizeof(buf),
      "(%d年%d月%d日 %d:%d) 帐号:%s 游戏名:%s 文件:%s 函数:%s 行数:%d 原因:%s",
      tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
      char_id, char_name, file, fun, ilne, yuanyin);
  printl(LOG_LOGOUT, buf);
}
#endif
