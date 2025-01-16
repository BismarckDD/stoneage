#define _CHAR_C_
#include "char.h"
#include "main.h"
#include "saacproto_serv.h"
#include "util.h"

// CoolFish: Family 2001/6/12
#include "acfamily.h"

// Arminius 7.17 memory lock
#include "lock.h"
#include "recv.h"

static void getCharNameFromString(const char *in, char *output);
static void getCharOptionFromString(const char *in, char *output);
static void makeCharFileName(char *idstring, char *output, int outputlen, int num);
static void makeDeletCharFileName(char *id, char *output, int outputlen, int num);
// Nuke *1*1
static int makeSaveCharString(char *output, int outputlen, char *nm, char *opt,
                              char *info);
static int findBlankCharIndex(char *id);

static int unlinkCharFile(char *id, int num);
#ifdef _SLEEP_CHAR // 取得非活跃人物档名
static void makeSleepCharFileName(char *id, char *output, int outputlen, int num);
static void makeSleepCharPoolItemFileName(char *id, char *output, int outputlen);
static void makeSleepCharPoolPetFileName(char *id, char *output, int outputlen);
static void makeCharPoolPetFileName(char *id, char *output, int outputlen);
#endif
#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName(char *id, char *output, int outputlen);
#endif
#define SPACE '|'
#define SPACESTRING "|"

#define USERLOG(id, format, args...)                                           \
  {                                                                            \
    char body[1000];                                                           \
    char fn[1000];                                                             \
    if (id[0]) {                                                               \
      snprintf(body, sizeof(body), "%s.log.%d", id, get_rotate_count());       \
      makeDirFilename(fn, sizeof(fn), logdir, getHash(id), body);              \
      LOGBASE(fn, "%u ", (unsigned int)time(NULL));                            \
      LOGBASE(fn, format, ##args);                                             \
    }                                                                          \
  }

#ifdef _ANGEL_SUMMON
struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

void charLoadCallback(int ti, int auth, char *c0, char *c1, char *c2, char *c3,
                      char *c4, int i0, int i1) {
  // Spock deleted 2000/11/2
  // static int process_id = 0;
  // CoolFish: Init char_index 2001/10/16
#ifdef _NewSave
  int char_index = -1;
#else
  int char_index;
#endif
  char loadbuf[CHARDATASIZE];
  char infobuf[CHARDATASIZE];
  int lock = i0;
  char *process = c3;
  char *id = c0;
  char *passwd = c1;
  char *charname = c2;
  int mesgid = i1;

  char *deadline = c4;

  // Spock deleted 2000/11/2
  // process_id++;

  if (auth != 0) {
    char data[100];
    snprintf(data, sizeof(data), "%d", auth);
#ifdef _NewSave
    saacproto_ACCharLoad_send(ti, FAILED, data, mesgid, char_index);
#else
    saacproto_ACCharLoad_send(ti, FAILED, data, mesgid);
#endif
    return;
  }
  if (isLocked(id)) {
    int process = atoi(c3);
#ifdef _NewSave
    saacproto_ACCharLoad_send(ti, FAILED, "locked", mesgid, char_index);
#else
    saacproto_ACCharLoad_send(ti, FAILED, "locked", mesgid);
#endif
    DeleteMemLock(getHash(id) & 0xff, id, &process); // 如果AP无锁则AC解锁
    log("\n (%s) AC同一星系重覆登入，踢人!! ", id);
    saacproto_ACKick_recv(ti, id, 1, -1); // 踢人

    checkGSUCheck(id);
    return;
  }

  char_index = getCharIndexByName(id, charname);
#ifdef _NewSave
  // log("\n档案装载序号:%d 账号:%s 名字:%s\n", char_index, id, charname);
#endif

  if (char_index < 0) {
    /* 非法的char_index */
#ifdef _NewSave
    saacproto_ACCharLoad_send(ti, FAILED, "char nonexistent", mesgid,
                              char_index);
#else
    saacproto_ACCharLoad_send(ti, FAILED, "char nonexistent", mesgid);
#endif
    return;
  }

  if (loadCharOne(id, char_index, loadbuf, sizeof(loadbuf)) < 0) {
#ifdef _NewSave
    saacproto_ACCharLoad_send(ti, FAILED, "cannot load ( disk i/o error?)",
                              mesgid, char_index);
#else
    saacproto_ACCharLoad_send(ti, FAILED, "cannot load ( disk i/o error?)",
                              mesgid);
#endif
    return;
  } else {
#ifdef _CHAR_POOLITEM
    checkCharPoolItem(id);
#endif
#ifdef _CHAR_POOLPET
    checkCharPoolPet(id);
#endif
  }
  if (lock) {
    char result[100];
    char retdata[100];
#ifdef _LOCK_ADD_NAME
    if (lockUser(getGSName(ti), id, charname, passwd, 1, result, sizeof(result),
                 retdata, sizeof(retdata), process, deadline) < 0) {
#else
    // Spock 2000/11/2
    if (lockUser(getGSName(ti), id, passwd, 1, result, sizeof(result), retdata,
                 sizeof(retdata), process, deadline) < 0) {
#endif
      saacproto_ACCharLoad_send(ti, FAILED, "lock FAIL!!", mesgid, char_index);
      return;
    }
  }
  memset(infobuf, 0, sizeof(infobuf));
  getCharInfoFromString(loadbuf, infobuf);
  makeStringFromEscaped(infobuf);
#ifdef _NewSave
  saacproto_ACCharLoad_send(ti, SUCCESSFUL, infobuf, mesgid, char_index);
#else
  saacproto_ACCharLoad_send(ti, SUCCESSFUL, infobuf, mesgid);
#endif

#ifdef _WAEI_KICK
  saacproto_ACKick_recv(ti, id, 10, -1); // 踢其他星系
#endif
}

#ifdef _NewSave
int charSave(int ti, char *id, char *charname, char *opt, char *charinfo,
             int unlock, int mesgid, int char_index)
#else
int charSave(int ti, char *id, char *charname, char *opt, char *charinfo,
             int unlock, int mesgid)
#endif
{
#ifdef _NewSave
#else
  int char_index;
#endif
  char savebuf[CHARDATASIZE];
  int ret = -1;

  memset(savebuf, 0, sizeof(savebuf));

  // andy_log
  if (strstr(charinfo, "DATAEND=") == NULL) {
    FILE *fp;
    if ((fp = fopen("badpetstring.txt", "a+")) != NULL) {
      fprintf(fp, "%s\n", charinfo);
      fclose(fp);
    }
    log("err add batpetstring.txt:%s[%s] !\n", id, charname);
  }

  if (unlock) {
    char result[100];
    char retdata[100];
#ifdef _LOCK_ADD_NAME
    if ((ret = lockUser(getGSName(ti), id, "", "0", 0, result, sizeof(result),
                        retdata, sizeof(retdata), "0", "0")) < 0) {
#else
    if ((ret = lockUser(getGSName(ti), id, "0", 0, result, sizeof(result),
                        retdata, sizeof(retdata), "0", "0")) < 0) {
#endif
      log("解锁:%s 失败!!\n", id);
    }
  }
  // Nuke *1 add escape
  if (makeSaveCharString(savebuf, sizeof(savebuf), charname, opt, charinfo) <
      0) {
    log("\n AC存档:太长  ");
    saacproto_ACCharSave_send(ti, FAILED, "too long", mesgid);
    // Spock fixed 2000/11/1
    return ret;
  }
#ifdef _NewSave
  if (char_index == -1)
    char_index = getCharIndexByName(id, charname);
#else
  // Nuke *1-
  char_index = getCharIndexByName(id, charname);
#endif
  if (char_index < 0) {
    int blankind = findBlankCharIndex(id);
    if (blankind < 0) {
      log("\n ACCharSave:char full  ");
      saacproto_ACCharSave_send(ti, FAILED, "char full", mesgid);
      return ret;
    } else {
      char_index = blankind;
    }
  }

  log("账号:[%s] 人物:[%s]\n", id, charname);
  if (saveCharOne(id, char_index, savebuf) < 0) {
    log("\n ACCharSave:disk I/O error or a bug  ");
    saacproto_ACCharSave_send(ti, FAILED, "disk I/O error or a bug", mesgid);
    return ret;
  }

  saacproto_ACCharSave_send(ti, SUCCESSFUL, "", mesgid);
  return ret;
}

void charListCallback(int ti, int auth, char *c0, char *c1, char *c2, char *c3,
                      char *c4, int i0, int i1, int charlistflg) {
  char listbuf[CHARDATASIZE];
  char *id = c0;
  int mesgid = i0;
  if (auth != 0) {
    char data[100];
    snprintf(data, sizeof(data), "%d", auth);
    saacproto_ACCharList_send(ti, FAILED, "你的账号暂时无法登陆！", mesgid);
    total_ng_charlist++;
    return;
  }
  // 取消下列 unlock 动作
  if (isLocked(id)) {
    saacproto_ACCharList_send(ti, FAILED,
                              "你的账号处理锁定状态，请重新登陆，如果三次都不能"
                              "进入，请联系管理员为您解锁！",
                              mesgid);
    saacproto_ACKick_recv(ti, id, 1, -1);
    checkGSUCheck(id);
    total_ng_charlist++;
    return;
  }
#ifdef _SLEEP_CHAR // 如果无新档, 将旧档移至新档
  {
    char fn_old[256], fn_new[256];
    FILE *fp_old, *fp_new;
    int i;

    // 移人物档
    for (i = 0; i < MAXCHAR_PER_USER; i++) {
      makeCharFileName(id, fn_new, sizeof(fn_new), i);
      fp_new = fopen(fn_new, "r");
      if (fp_new == NULL) {
        makeSleepCharFileName(id, fn_old, sizeof(fn_old), i);
        fp_old = fopen(fn_old, "r");
        if (fp_old != NULL) {
          fclose(fp_old);
          rename(fn_old, fn_new); // 搬移
          // filecopy( fn_old, fn_new); // 复制
          log(" 移档_%s ", fn_new);
        }
      } else {
        fclose(fp_new);
      }
    }
#ifdef _CHAR_POOLITEM
    // 移仓物品库档
    makeCharPoolItemFileName(id, fn_new, sizeof(fn_new));
    fp_new = fopen(fn_new, "r");
    if (fp_new == NULL) {
      makeSleepCharPoolItemFileName(id, fn_old, sizeof(fn_old));
      fp_old = fopen(fn_old, "r");
      if (fp_old != NULL) {
        fclose(fp_old);
        rename(fn_old, fn_new); // 搬移
        // filecopy( fn_old, fn_new); // 复制
        log(" 移档_%s ", fn_new);
      }
    } else {
      fclose(fp_new);
    }
#endif
#ifdef _CHAR_POOLPET
    // 移仓宠物库档
    makeCharPoolPetFileName(id, fn_new, sizeof(fn_new));
    fp_new = fopen(fn_new, "r");
    if (fp_new == NULL) {
      makeSleepCharPoolPetFileName(id, fn_old, sizeof(fn_old));
      fp_old = fopen(fn_old, "r");
      if (fp_old != NULL) {
        fclose(fp_old);
        rename(fn_old, fn_new); // 搬移
        // filecopy( fn_old, fn_new); // 复制
        log(" 移档_%s ", fn_new);
      }
    } else {
      fclose(fp_new);
    }

#endif
  }
#endif

  loadCharNameAndOption(id, listbuf, sizeof(listbuf));
  // Arminius
  saacproto_ACCharList_send(ti, SUCCESSFUL, listbuf, mesgid);
  total_ok_charlist++;
}

void charDeleteCallback(int ti, int auth, char *c0, char *c1, char *c2,
                        char *c3, char *c4, int i0, int i1) {
  int char_index;
  char *id = c0;
  char *charname = c2;
  int mesgid = i0;
  struct tm *now;
  time_t timenow;
  char logfile[128];

  if (auth != 0) {
    char data[100];
    snprintf(data, sizeof(data), "%d", auth);
    saacproto_ACCharDelete_send(ti, FAILED, data, mesgid);
    return;
  }

  char_index = getCharIndexByName(id, charname);
  if (char_index < 0) {
    saacproto_ACCharDelete_send(ti, FAILED, "nochar", mesgid);
    return;
  }

  if (unlinkCharFile(id, char_index) < 0) {
    saacproto_ACCharDelete_send(ti, FAILED, "fileI/O", mesgid);
    return;
  }
#ifdef _FAMILY
  // CoolFish: Family 2001/6/12
  {
    int index = 0, fmindex = 0, fmchar_index = 0;
    char fmname[32];
    index = ACgetFMInfoFromChar(fmname, &fmindex, charname, id, &fmchar_index);
    if (index != -1) {
      if (fmchar_index == 0) {
#ifdef _FMVER21
        if (ChangeFMLeader(index, fmname, fmindex) >= 0) {
          if (ACDelFM(index, fmname, fmindex) >= 0)
            log("删除家族成员:%d 家族名:%s 家族索引:%d 人物:%s, 账号:%s, "
                "家族人物索引:%d\n",
                index, fmname, fmindex, charname, id, fmchar_index);
        }
        log("ChangeFMLeader_3 index:%d fmname:%s fmindex:%d\n", index, fmname,
            fmindex);
#else
        if (ACDelFM(index, fmname, fmindex) >= 0)
          log("删除家族成员:%d 家族名:%s 家族索引:%d 人物:%s, 账号:%s, "
              "家族人物索引:%d\n",
              index, fmname, fmindex, charname, id, fmchar_index);
#endif
      } else {
#ifdef _FMVER21
        if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, 0,
                            fmchar_index) >= 0)
          log("ACMemberLeaveFM index:%d 家族名:%s 家族索引:%d 人物:%s, "
              "账号:%s, 家族人物索引:%d\n",
              index, fmname, fmindex, charname, id, fmchar_index);
#else
        if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, fmchar_index) >=
            0)
          log("ACMemberLeaveFM index:%d 家族名:%s 家族索引:%d 人物:%s, "
              "账号:%s, 家族人物索引:%d\n",
              index, fmname, fmindex, charname, id, fmchar_index);
#endif
      }
    }
  }
#endif
  // Spock 2000/12/6
  time(&timenow);
  now = localtime(&timenow);
  sprintf(logfile, "log/chardel/%04d%02d%02d.log", now->tm_year + 1900,
          now->tm_mon + 1, now->tm_mday);
  LOGBASE(logfile,
          "%04d/%02d/%02d %02d:%02d:%02d id:[%s] char:[%s] index:[%d]\n",
          now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour,
          now->tm_min, now->tm_sec, id, charname, char_index);
  // Spock end
  USERLOG(id, "账号删除: 成功\n");
  saacproto_ACCharDelete_send(ti, SUCCESSFUL, "ok", mesgid);
}

void dummyCallback(int ti, int auth, char *c0, char *c1, char *c2, char *c3,
                   char *c4, int i0, int i1) {
  return;
}

static void getCharNameFromString(const char *input, char *output) {
  int i;
  output[0] = '\0';
  for (i = 0;; ++i) {
    output[i] = input[i];
    if (input[i] == '\0')
      break;
    if (input[i] == SPACE) {
      output[i] = '\0';
      break;
    }
  }
}

static void getCharOptionFromString(const char *input, char *output) {
  int ri, wi = 0;
  output[0] = '\0';
  for (ri = 0;; ++ri) {
    if (input[ri] == '\0')
      break;
    if (input[ri] == SPACE) {
      ++ri;
      for (;; ++ri) {
        output[wi] = input[ri];
        if (input[ri] == '\0')
          break;
        if (input[ri] == SPACE) {
          output[wi] = '\0';
          break;
        }
        wi++;
      }
      return;
    }
  }
}

void getCharInfoFromString(char *input, char *output) {
  int ri = 0, wi = 0;
  int space_counter = 0;

  output[wi] = '\0';
  for (ri = 0;; ++ri) {
    if (input[ri] == '\0')
      break;
    if (input[ri] == SPACE) {
      space_counter++;
    }
    if (space_counter == 2 && input[ri] == SPACE) {
      ++ri;
      for (;; ++ri) {
        output[wi] = input[ri];
        if (input[ri] == '\0')
          break;
        if (input[ri] == SPACE) {
          output[wi] = '\0';
          break;
        }
        ++wi;
      }
      return;
    }
  }
}

static void makeDeletCharFileName(char *id, char *output, int outputlen, int num) {
  char body[1024];
  if (strlen(id) < 1)
    return;
  struct tm tm1;
  time_t timep;
  time(&timep);
  memcpy(&tm1, localtime(&timep), sizeof(tm1));
  snprintf(body, sizeof(body), "%s.%d.char.%d.%d.%d.%d.%d.%d", id, num,
           tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour,
           tm1.tm_min, tm1.tm_sec);
  makeDirFilename(output, outputlen, "char_delet", getHash(id), body);
}

static void makeCharFileName(char *id, char *output, int outputlen, int num) {
  char body[1024];
  if (strlen(id) < 1)
    return;
  snprintf(body, sizeof(body), "%s.%d.char", id, num);
  makeDirFilename(output, outputlen, chardir, getHash(id), body);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物档名
static void makeSleepCharFileName(char *id, char *output, int outputlen, int num) {
  char body[1024];
  if (strlen(id) < 1)
    return;

  snprintf(body, sizeof(body), "%s.%d.char", id, num);
  makeDirFilename(output, outputlen, sleepchardir, getHash(id), body);
}
#endif

int loadCharNameAndOption(char *id, char *output, int outputlen) {
  char char_buf[MAXCHAR_PER_USER][CHARDATASIZE];
  char name_work[CHARDATASIZE];
  char option_work[CHARDATASIZE];
  char nm_work[CHARDATASIZE];
  int i, count = 0;
  int so_far_bytes = 0;
  output[0] = 0;

  for (i = 0; i < MAXCHAR_PER_USER; i++) {
    int l;
    char delim[2];
    char_buf[i][0] = 0;
    loadCharOne(id, i, char_buf[i], sizeof(char_buf[i]));
    getCharNameFromString(char_buf[i], name_work);
    getCharOptionFromString(char_buf[i], option_work);
    if (char_buf[i][0] == 0)
      continue;
    if (strlen(output) > 0) {
      delim[0] = SPACE;
      delim[1] = 0;
    } else {
      delim[0] = 0;
    }
    snprintf(nm_work, sizeof(nm_work), "%s%s|%s", delim, name_work, option_work);
    l = strlen(nm_work);
    if ((so_far_bytes + l) < outputlen) {
      strcat(output, nm_work);
      so_far_bytes += l;
      count++;
    }
  }
  return count;
}

int loadCharOne(char *id, int num, char *output, int outputlen) {
  char fn[1024];
  FILE *fp;
  char c_temp, *c_ptr;
  c_ptr = output;

  makeCharFileName(id, fn, sizeof(fn), num);
  fp = fopen(fn, "r");
  if (fp == NULL) {
    return -1;
  }

  do {
    c_temp = fgetc(fp);
    *c_ptr = c_temp;
    c_ptr++;
  } while (c_temp != EOF);
  *c_ptr = '\0';

  if (output[0] == '|' && output[1] == '|') {
    return -1;
  }

#ifdef _FIX_CAHR_VERSION
  {
    char *strp;
    while ((strp = strstr(output, "\\a")) != NULL) {
      *(strp + 1) = 'n';
    }
  }
#endif

  fclose(fp);
  return 0;
}

int saveCharOne(char *id, int num, char *input) {
  char fn[1024];
  FILE *fp;
  char *strp;
  char *strp1; // Won 修正 hp 为负的人

  makeCharFileName(id, fn, sizeof(fn), num);
  fp = fopen(fn, "w");
  if (fp == NULL) {
    log("save error 01: %d", errno); // Change add errno
    return -1;
  }
  // Won 修正 hp 为负的人
  if ((strp = strstr(input, "\\nhp=-")) != NULL) {
    strp1 = strstr(input, "\\nmp=");
    *(strp + 5) = '1';
    strcpy((strp + 6), strp1);
  }

  // Arminius
  if ((strp = strstr(input, "\\nhp=0\\n")) != NULL)
    *(strp + 5) = '1';

#ifdef _FIX_CAHR_VERSION
  {
    char *strp;
    while ((strp = strstr(input, "\\n")) != NULL) {
      *(strp + 1) = 'a';
    }
  }
#endif
  fprintf(fp, "%s", input);
  fclose(fp);
  log("写入 %s 档案文件:%s\n", id, fn);
  chmod(fn, 0777);
  return 0;
}

static int makeSaveCharString(char *output, int outputlen, char *nm, char *opt,
                              char *info) {
  char nmwork[CHARDATASIZE];
  char optwork[CHARDATASIZE];
  char infowork[CHARDATASIZE];
  char outputwork[CHARDATASIZE];
  char *nmwork_p, *optwork_p, *infowork_p;
  int l;

  strncpy(nmwork, nm, sizeof(nmwork));
  nmwork[strlen(nm)] = 0;

  strncpy(optwork, opt, sizeof(optwork));
  optwork[strlen(opt)] = 0;

  strncpy(infowork, info, sizeof(infowork));
  infowork[strlen(info)] = 0;

  nmwork_p = makeEscapeString(nm, nmwork, sizeof(nmwork));
  optwork_p = makeEscapeString(opt, optwork, sizeof(optwork));
  infowork_p = makeEscapeString(info, infowork, sizeof(infowork));

  snprintf(outputwork, sizeof(outputwork), "%s" SPACESTRING "%s" SPACESTRING "%s",
           nmwork_p, optwork_p, infowork_p);

  l = strlen(outputwork);
  if (l >= (outputlen - 1)) {
    return -1;
  }
  memcpy(output, outputwork, l + 1);
  return 0;
}

int getCharIndexByName(char *id, char *charname) {
  int i;
  for (i = 0; i < MAXCHAR_PER_USER; i++) {
    char output[CHARDATASIZE];
    if (loadCharOne(id, i, output, sizeof(output)) < 0) {
      continue;
    } else {
      char cn[CHARDATASIZE];
      getCharNameFromString(output, cn);
      if (strcmp(charname, makeStringFromEscaped(cn)) == 0) {
        return i;
      } else {
      }
    }
  }
  return -1;
}

static int findBlankCharIndex(char *id) {
  int i;
  char output[CHARDATASIZE];
  for (i = 0; i < MAXCHAR_PER_USER; i++) {
    if (loadCharOne(id, i, output, sizeof(output)) < 0) {
      return i;
    }
  }
  return -1;
}

static int unlinkCharFile(char *id, int num) {
  char fname[1024];
  char dname[1024];
#ifdef _SLEEP_CHAR // 删除人物时旧档也删
  makeSleepCharFileName(id, fname, sizeof(fname), num);
  unlink(fname);
#endif
  makeCharFileName(id, fname, sizeof(fname), num);
  makeDeletCharFileName(id, dname, sizeof(dname), num);

  return rename(fname, dname);
}

#ifdef _LOCK_ADD_NAME
int lockUser(char *gmsvname, char *id, char *name, char *passwd, int lock,
             char *result, int resultlen, char *retdata, int retdatalen,
             char *process, char *deadline)
#else
int lockUser(char *gmsvname, char *id, char *passwd, int lock, char *result,
             int resultlen, char *retdata, int retdatalen, char *process,
             char *deadline)
#endif
{
  int ret = -1;

  if (!id[0]) {
    snprintf(result, resultlen, FAILED);
    snprintf(retdata, retdatalen, "bad id");
    return -1;
  }
  retdata[0] = 0;
  if (lock) {
    if (isLocked(id)) {
      snprintf(result, resultlen, FAILED);
      snprintf(retdata, retdatalen, "already locked");
      log("写入内存信息: 用户:%x/%s 已经同意锁定 !!\n", getHash(id), id);
      return -1;
    } else {
#ifdef _LOCK_ADD_NAME
      if (InsertMemLock(getHash(id) & 0xff, id, name, passwd, gmsvname,
                        atoi(process), deadline))
#else
      if (InsertMemLock(getHash(id) & 0xff, id, passwd, gmsvname, atoi(process),
                        deadline))
#endif
        return 0;
      else
        return -1;
    }
  } else {
    if (!isLocked(id)) {
      log("删除内存信息: 用户:%x/%s 没有锁定!!\n", getHash(id), id);
    }
    if (DeleteMemLock(getHash(id) & 0xff, id, &ret)) {
      snprintf(result, resultlen, SUCCESSFUL);
      snprintf(retdata, retdatalen, "removed");
      return ret;
    } else {
      snprintf(result, resultlen, FAILED);
      snprintf(retdata, retdatalen, "不能移除锁定");

      log("不能解锁 %x:%s !\n", getHash(id), id);
      return ret;
    }
  }
}

int isLocked(char *id) {
  if (!id[0])
    return 1; // invalid id: lock it
  return isMemLocked(getHash(id) & 0xff, id);
}

int checkValidCharName(char *a) {
  int i;
  for (i = 0;; i++) {
    if (a[i] == 0)
      break;
    if (a[i] == SPACE)
      return -1;
  }
  return 0;
}

#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName(char *id, char *output, int outputlen) {
  char poolitem[256];
  if (strlen(id) < 1)
    return;
  snprintf(poolitem, sizeof(poolitem), "%s.item", id);
  makeDirFilename(output, outputlen, chardir, getHash(id), poolitem);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物仓库档名
static void makeSleepCharPoolItemFileName(char *id, char *output, int outputlen) {
  char poolitem[256];
  if (strlen(id) < 1)
    return;
  snprintf(poolitem, sizeof(poolitem), "%s.item", id);
  makeDirFilename(output, outputlen, sleepchardir, getHash(id), poolitem);
}
#endif

int InsertCharPoolItem(char *id, char *input, int sizes) {
  char fn[256];
  FILE *fp;

  if (sizes <= 0)
    return sizes;
  makeCharPoolItemFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "a+");
  if (fp == NULL) {
    fp = fopen(fn, "w");
    if (fp == NULL) {
      log("保存 %s 错误.\n", fn);
      return -1;
    }
  }

  fprintf(fp, "%s", input);
  fclose(fp);

  log("添加 %s 物品仓库文件:%s\n", id, fn);
  return sizes;
}

int saveCharPoolItem(char *id, char *input, int sizes) {
  char fn[256];
  FILE *fp;

  if (sizes < 0)
    return sizes;
  if (input == NULL)
    return -1;
  makeCharPoolItemFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "w");
  if (fp == NULL) {
    log("保存 %s 错误.\n", fn);
    return -1;
  }

  fprintf(fp, "%s", input);
  fclose(fp);

  log("写入 %s 道具档案文件:%s\n", id, fn);
  return sizes;
}

int loadCharPoolItemOne(char *id, char *output, int outputlen) {
  char fn[256];
  FILE *fp;
  char c_temp, *c_ptr;
  int sizes = 0;
  c_ptr = output;

  makeCharPoolItemFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "r");
  if (fp == NULL) {
    if ((fp = fopen(fn, "w")) == NULL)
      return -1;
    return 0;
  }

  do {
    c_temp = fgetc(fp);
    *c_ptr = c_temp;
    c_ptr++;
    sizes++;
    if (sizes >= (outputlen - 1))
      break;
  } while (c_temp != EOF);
  *c_ptr = '\0';
  fclose(fp);
  return sizes;
}

int checkCharPoolItem(char *id) {
  char fn[256];
  FILE *fp;

  makeCharPoolItemFileName(id, fn, sizeof(fn));
  fp = fopen(fn, "r");
  if (fp == NULL) {
    if (saveCharPoolItem(id, "", 0) < 0) {
      log("\n 无法建立档案:%s 物品仓库文件!!", id);
      return -1;
    }
  } else {
    fclose(fp);
  }

  return 1;
}
#endif

#ifdef _CHAR_POOLPET
static void makeCharPoolPetFileName(char *id, char *output, int outputlen) {
  char poolpet[256];
  if (strlen(id) < 1)
    return;
  snprintf(poolpet, sizeof(poolpet), "%s.pet", id);
  makeDirFilename(output, outputlen, chardir, getHash(id), poolpet);
}

#ifdef _SLEEP_CHAR // 取得非活跃人物仓库档名
static void makeSleepCharPoolPetFileName(char *id, char *output, int outputlen) {
  char poolpet[256];
  if (strlen(id) < 1)
    return;
  snprintf(poolpet, sizeof(poolpet), "%s.pet", id);
  makeDirFilename(output, outputlen, sleepchardir, getHash(id), poolpet);
}
#endif

int InsertCharPoolPet(char *id, char *input, int sizes) {
  char fn[256];
  FILE *fp;

  if (sizes <= 0)
    return sizes;
  makeCharPoolPetFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "a+");
  if (fp == NULL) {
    fp = fopen(fn, "w");
    if (fp == NULL) {
      log("保存 %s 错误.\n", fn);
      return -1;
    }
  }

  fprintf(fp, "%s", input);
  fclose(fp);

  log("添加 %s 宠物仓库文件:%s\n", id, fn);
  return sizes;
}

int saveCharPoolPet(char *id, char *input, int sizes) {
  char fn[256];
  FILE *fp;

  if (sizes < 0)
    return sizes;
  if (input == NULL)
    return -1;
  makeCharPoolPetFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "w");
  if (fp == NULL) {
    log("保存 %s 错误.\n", fn);
    return -1;
  }

  fprintf(fp, "%s", input);
  fclose(fp);

  log("写入 %s 宠物档案文件:%s\n", id, fn);
  return sizes;
}

int loadCharPoolPetOne(char *id, char *output, int outputlen) {
  char fn[256];
  FILE *fp;

  char c_temp, *c_ptr;
  int sizes = 0;
  c_ptr = output;

  makeCharPoolPetFileName(id, fn, sizeof(fn));

  fp = fopen(fn, "r");
  if (fp == NULL) {
    if ((fp = fopen(fn, "w")) == NULL)
      return -1;
    return 0;
  }

  do {
    c_temp = fgetc(fp);
    *c_ptr = c_temp;
    c_ptr++;
    sizes++;
    if (sizes >= (outputlen - 1))
      break;
  } while (c_temp != EOF);
  *c_ptr = '\0';
  fclose(fp);
  return sizes;
}

int checkCharPoolPet(char *id) {
  char fn[256];
  FILE *fp;

  makeCharPoolPetFileName(id, fn, sizeof(fn));
  fp = fopen(fn, "r");
  if (fp == NULL) {
    if (saveCharPoolPet(id, "", 0) < 0) {
      log("\n 无法建立档案:%s 宠物仓库文件!!", id);
      return -1;
    }
  } else {
    fclose(fp);
  }

  return 1;
}
#endif

int filecopy(char *oldfilename, char *newfilename) {
  FILE *oldfile;
  FILE *newfile;
  char c_temp;

  if ((oldfile = fopen(oldfilename, "r")) == NULL) {
    return -1;
  }
  if ((newfile = fopen(newfilename, "w")) == NULL) {
    return -1;
  }

  while (1) {
    c_temp = fgetc(oldfile);
    if (c_temp == EOF)
      break;
    fputc(c_temp, newfile);
  };

  fclose(oldfile);
  fclose(newfile);

  return 0;
}
