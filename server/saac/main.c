#define __MAIN_C__
//
#include "main.h"
// 版本信息
#include "copyright.h"
// 标准 C 语言头文件
#include <getopt.h>
// CoolFish: Family 2001/5/9
#include "acfamily.h"
#include "char.h"
#include "db.h"
#include "lock.h"
#include "mail.h"
#include "saac_config.h"
#include "saac_server.h"
#include "tcp_struct.h"
#include "util.h"
#include "utils/util_time.h"
#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
#include "recv.h"
#endif
#ifndef _WIN32
#include <execinfo.h>
#include <sys/resource.h>
#endif

#ifdef _SASQL
#include "sasql.h"
#endif

int worksockfd;

WorkSpace gSaacWorkSpace;
/*
 * CHARDATASIZE is 1 MiB while the Windows executable reserves a 2 MiB
 * stack.  Keeping this buffer automatic leaves too little stack for
 * request handlers such as ACCharLogin and the MariaDB client, causing
 * STATUS_STACK_OVERFLOW in ___chkstk_ms.  SAAC handles requests on this
 * single main loop, so one static receive buffer is sufficient.
 TCP读取缓冲区
 */
static char sTcpBuf[CHARDATASIZE];
static char sDebugFun[256];

#ifdef _SAVE_ZIP
int SAVEZIP = 0;
void savezipfile(void);
#endif
// Nuke start
char *chartime() {
  static char buf[80];
  time_t t;
  t = time(0);
  strcpy(buf, ctime(&t));
  buf[strlen(buf) - 1] = 0;
  return (buf);
}

#ifdef _ANGEL_SUMMON
int mission_table_init(void);
int mission_table_save(void);
void checkMissionTimelimit(void);
#endif

// Memory buffer allocation/release (defined later in this file)
int allocateMemBuf(void);
int releaseMemBuf(const int index);

// Arminius 7.20 memory unlock
// 处理用户自定义信号的方法
#ifndef _WIN32
void sigusr1(int a) {
  int i;
  FILE *f;
  char key[4096], buf[4096];
  signal(SIGUSR1, sigusr1);
  f = fopen("./unlock.arg", "r");

  if (f) {
    memset(key, 0, 4096);
    fread(key, 4096, 1, f);
    for (i = 0; i < strlen(key); i++)
      if (key[i] == '\n')
        key[i] = '\0';

    switch (key[0]) {
    case 'P': // unlock player
      if (DeleteMemLock(getHash(&key[1]) & 0xff, &key[1], &i)) {
        logErr("ADM: memunlock: %s success.\n", key);
      } else {
        logErr("ADM: memunlock: %s failed.\n", key);
      }
      break;
    case 'S': // unlock server
      DeleteMemLockServer(&key[1]);
      logErr("ADM: memunlock: %s\n", key);
      break;
    case 'C': // check player lock
      GetMemLockState(getHash(&key[1]) & 0xff, &key[1], buf);
      sprintf(key, "echo \"%s\" > ./sigusr1.result", buf);
      system(key);
      break;
#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
    case 'E':
      logErr("\nAC 向 GS 发送下雪特效!!\n");
      SendEffect(&key[1]);
      break;
#endif
    case 'L': // Robin 列出所有Server连线
      logErr("\nList All Server Conncet!!!!!\n");
      for (i = 0; i < MAXCONNECTION; i++)
        if (gs[i].use)
          logErr("\n gs[%d] fd:%d name:%s ", i, gs[i].fd, gs[i].name);
      break;
    }
    logErr(" sigusr1_over_1 ");
    fclose(f);
    logErr(" sigusr1_over_2 ");
  }
}
#endif

#if _ATTESTAION_ID == 1
int login_game_server(const int ti, const int id, const char *server_name,
                      const char *server_pass, char *result,
                      const int result_len, char *retdata,
                      const int ret_data_len)
#else
int login_game_server(const int ti, const char *server_name,
                      const char *server_pass, char *result,
                      const int result_len, char *retdata,
                      const int ret_data_len)
#endif
{
#if _ATTESTAION_ID == 1
  if (id != _ATTESTAION_ID) {
    logErr("服务端版本错误\n");
    snprintf(result, result_len, "失败");
    snprintf(retdata, ret_data_len, "版本错误");
    return 0;
  }
#endif
  // svpass 是一个外部变量
  if (strcmp(server_pass, g_saac_config.svpass) == 0) {
    logErr("服务器密码正确 %s\n", server_name);
  } else {
    logErr("服务器密码错误 %s\n", server_name);
    snprintf(result, result_len, "失败");
    snprintf(retdata, ret_data_len, "密码错误");
    return 0;
  }
  int i;
  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].use && strcmp(gs[i].name, server_name) == 0) {
      snprintf(result, result_len, "失败");
      snprintf(retdata, ret_data_len, "重复登陆");
      return 0;
    }
  }
  snprintf(gs[ti].name, sizeof(gs[ti].name), "%s", server_name);
  gs[ti].fd = ti;
  snprintf(result, result_len, SUCCESSFUL);
  snprintf(retdata, ret_data_len, "没有空间");
  DeleteMemLockServer(server_name); // Arminius 7.31 unlock server
  return 0;
}

// ti代表一个链接的标识
int logout_game_server(const int ti) {
  gs[ti].use = 0;
  gs[ti].fd = -1;
  gs[ti].name[0] = '\0';
  tcpstruct_close(ti);
  printf("内存剩余: %f\n",
         (float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION));
  return 0;
}

// 判断ti链接是否已经登录, 这种判定是不是合理？
int is_game_server_login(const int ti) {
  if (strlen(gs[ti].name) == 0) {
    return 0;
  } else {
    return gs[ti].use;
  }
}

int servid;

static void parse_opts(int argc, char **argv) {
  int c, option_index;
  int date;
  while (TRUE) {
    static struct option long_options[] = {
        {"nice", 1, 0, 'n'},     {"buy", 0, 0, 'b'},
        {"cost", 0, 0, 'c'},     {"help", 0, 0, 'h'},
        {"userinfo", 0, 0, 'i'}, {"lockuser", 0, 0, 'l'},
        {"transmd5", 0, 0, 'm'}, {"del", 0, 0, 'd'},
        {"trans", 0, 0, 't'}};
    c = getopt_long(argc, argv, "n:bchilmdt", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 'h':
      fprintf(stderr, "使用方法: saac [-h] [-w port] [-w port] ... \n"
                      "-h : 显示saac的帮助\n"
                      "-w port : 添加一个工作站进程端口\n"
                      "-l : 测试sql连接是否正常, 测试完成后服务退出\n"
                      "-i : 自定义测试, 测试完成后服务退出\n"
                      "@Copyright Franklin.2025.\n");
      exit(0);
    case 'i': {
      int i;
      char buf[256] = "heihei";
      int len = strlen(buf);
      char crypto_key[] = {"aBcDeFgHiJkLmNoPqRsTuVwXyZ"};
      for (i = 0; i < len; i++) {
        buf[i] = buf[i] ^ crypto_key[26 - i % 26];
      }
      FILE *fpw = fopen("temp", "w+");
      fputs(buf, fpw);
      fclose(fpw);
      exit(0);
    }
    case 'l':
#ifdef _SASQL
      sasql_init();
      sasql_craete_lock();
      sasql_close();
#endif
      exit(0);
      break;
    case 'b':
#ifdef _SQL_BUY_FUNC
    {
      int type = 0, num = 0;
      char coststr[64];
      printf("请输入您要制作提货卡的类型(0为宠物、1为道具、2为石币)\n");
      scanf("%d", &type);
      if (type <= 0) {
        int id, vital, str, tough, dex;
        printf("请输入您要制作宠物的ID：");
        scanf("%d", &id);
        printf("请输入您要制作宠物的体力：");
        scanf("%d", &vital);
        printf("请输入您要制作宠物的腕力：");
        scanf("%d", &str);
        printf("请输入您要制作宠物的耐力：");
        scanf("%d", &tough);
        printf("请输入您要制作宠物的敏捷：");
        scanf("%d", &dex);
        sprintf(coststr, "%d|%d|%d|%d|%d", id, vital, str, tough, dex);
      } else if (type == 1) {
        int id;
        printf("请输入您要制作物品的ID：");
        scanf("%d", &id);
        sprintf(coststr, "%d", id);
      } else if (type >= 2) {
        int gold;
        printf("请输入您要制作石币的面值：");
        scanf("%d", &gold);
        sprintf(coststr, "%d", gold);
      }
      printf("请输入您要制作的充值卡数量：");
      scanf("%d", &num);
      sasql_init();
      sasql_OnlineBuy_add(coststr, type, num);
      sasql_close();
    }
#endif
      exit(0);
      break;
    case 'c':
#ifdef _ONLINE_COST
    {
      int cost = 0, num = 0, point = 0;
      printf("请输入您要制作的充值卡面值：");
      scanf("%d", &cost);
      printf("请输入您要制作的充值卡数量：");
      scanf("%d", &num);
      printf("请输入您要制作的充值卡积分：");
      scanf("%d", &point);
      sasql_init();
      sasql_OnlineCost_add(cost, num, point);
      sasql_close();
    }
#endif
      exit(0);
    case 'm':
#ifdef _OLDPS_TO_MD5PS
      sasql_init();
      sasql_OldpsToMd5ps();
      sasql_close();
#endif
      exit(0);
    case 't':
      sasql_init();
      sasql_TransOnlineCost();
      sasql_close();
      exit(0);
    case 'd':
      printf("输入时间:");
      scanf("%d", &date);
      sasql_init();
      if (date >= 0) {
        sasql_CleanCdkey(date);
      } else {
        sasql_CleanLockCdkey();
      }
      sasql_close();
      exit(0);
    case 'n':
#ifdef _WIN32
      sa_set_process_priority(atoi(optarg));
#else
      nice(atoi(optarg));
#endif
      break;
    default:
      logErr("不能读懂选项 %c\n", c);
      exit(0);
    }
  }
}

void dump_error() {
#ifdef _WIN32
  void *array[10];
  USHORT size = CaptureStackBackTrace(0, arraysizeof(array), array, NULL);
  USHORT i;
  logOut("Obtained %u stack frames.\n", (unsigned)size);
  for (i = 0; i < size; ++i)
    logFileToday("  frame[%u] = %p\n", (unsigned)i, array[i]);
#else
  void *array[10];
  size_t size;
  char **strings;
  size_t i;
  size = backtrace(array, 10);
  strings = backtrace_symbols(array, size);
  logOut("Obtained %zd stack frames.\n", size);
  for (i = 0; i < size; i++) {
    logFileToday(strings[i]);
  }
  free(strings);
#endif
}

#ifndef _WIN32
static void enable_core_dump(void) {
  struct rlimit limit;
  if (getrlimit(RLIMIT_CORE, &limit) != 0) {
    perror("getrlimit(RLIMIT_CORE)");
    return;
  }
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit(RLIMIT_CORE, &limit) != 0)
    perror("setrlimit(RLIMIT_CORE)");
}

static int is_fatal_signal(int number) {
  return number == SIGSEGV || number == SIGBUS || number == SIGILL ||
         number == SIGFPE || number == SIGABRT;
}
#endif

void signal_shutdown(const int number) {
#ifndef _WIN32
  if (is_fatal_signal(number)) {
    static const char message[] =
        "SAAC fatal signal received; generating core dump.\n";
    write(STDERR_FILENO, message, sizeof(message) - 1);
    signal(number, SIG_DFL);
    raise(number);
    _exit(128 + number);
  }
#endif
  if (number == 0) {
    logErr("SAAC正常关闭\n");
  } else if (number == 2) {
    logErr("SAAC被CTRL+C手动中断\n");
  } else {
    // 只有异常的时候, 才写文件
    logFileToday("SAAC收到异常信号. DUMP!\n");
    dump_error();
  }
  // 确认结束后，忽略之后的信号SIG_IGN
  signal(SIGINT, SIG_IGN);
#ifndef _WIN32
  signal(SIGQUIT, SIG_IGN);
  signal(SIGILL, SIG_IGN);
  signal(SIGTRAP, SIG_IGN);
  signal(SIGIOT, SIG_IGN);
  signal(SIGBUS, SIG_IGN);
  signal(SIGFPE, SIG_IGN);
  signal(SIGKILL, SIG_IGN);
  signal(SIGSEGV, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
#endif
  signal(SIGTERM, SIG_IGN);
  logErr("收到一个信号! 异常中断......\n");
  writeFamily(g_saac_config.familydir);
  writeFMPoint(g_saac_config.fmpointdir);
  writeFMSMemo(g_saac_config.fmsmemodir);
#ifdef _ANGEL_SUMMON
  mission_table_save();
#endif
  exit(1);
}

// 定义SAAC要处理的信号, 之行后SAAC收到信号时会执行对应的方法.
void signal_set(void) {
  // CoolFish: Test Signal 2001/10/26
  printf("\n开始获取信号..\n");
  printf("SIGINT:%d\n", SIGINT);
#ifndef _WIN32
  printf("SIGQUIT:%d\n", SIGQUIT);
  printf("SIGFPE:%d\n", SIGILL);
  printf("SIGTRAP:%d\n", SIGTRAP);
  printf("SIGIOT:%d\n", SIGIOT);
  printf("SIGBUS:%d\n", SIGBUS);
  printf("SIGFPE:%d\n", SIGFPE);
  printf("SIGKILL:%d\n", SIGKILL);
  printf("SIGSEGV:%d\n", SIGSEGV);
  printf("SIGPIPE:%d\n", SIGPIPE);
#endif
  printf("SIGTERM:%d\n", SIGTERM);
  // 测试中：可以使用 kill -[SIGNAL_NAME] [PROCESS_ID] 向进程发送信号
  signal(SIGINT, signal_shutdown);
#ifndef _WIN32
  signal(SIGQUIT, signal_shutdown);
  signal(SIGILL, signal_shutdown);
  signal(SIGTRAP, signal_shutdown);
  signal(SIGIOT, signal_shutdown);
  signal(SIGBUS, signal_shutdown);
  signal(SIGFPE, signal_shutdown);
  signal(SIGABRT, signal_shutdown);
  signal(SIGSEGV, signal_shutdown);
  signal(SIGPIPE, SIG_IGN);
#else
  sa_install_console_handler(signal_shutdown);
#endif
  signal(SIGTERM, signal_shutdown);
}

int main(int argc, char **argv) {
#ifndef _WIN32
  enable_core_dump();
#endif
#ifdef _WIN32
  if (sa_platform_init() != 0) {
    fprintf(stderr, "WinSock initialization failed: %d\n", errno);
    return 1;
  }
#endif
  // 先解析参数, 如果有参数, 则进入测试流程
  parse_opts(argc, argv);
  // 给出一个随机数种子. 产生随机数: 用于Lottery
  srand((int)time(0));
  // 如果没有参数，进入正常工作流程
  signal_set();
  // Nuke +1 1012: Loop counter
  int counter1 = 0;
  int counter2 = 0;
  int counter3 = 0;
  int counter4 = 0;
  int counter5 = 0;
  int counter6 = 0;
  g_saac_config.log_rotate_interval = 3600 * 24 * 7;

  Lock_Init(); // Arminius 7.17 memory lock
  UNlockM_Init();

  // 从acserv.cf文件中读取配置
  if (saac_read_config(DEFAULT_CONFIG_FILE) < 0) {
    logErr("读取配置失败，可能是无法找到配置文件(acserv.cf).\n");
    exit(1);
  }

#ifdef _SASQL
  if (sasql_init() == FALSE) {
    logErr("无法初始化数据库和sqllib.\n");
    exit(1);
  }
#ifdef _SQL_BACKGROUND
  sasql_online(NULL, NULL, NULL, NULL, 3);
#endif
#endif
  logErr("读取数据目录\n");
  dbRead(g_saac_config.dbdir);
#ifdef _FAMILY
  logErr("读取家族庄园\n");
  readFMSMemo(g_saac_config.fmsmemodir);
  logErr("读取家族留言\n");
  readFMPoint(g_saac_config.fmpointdir);
  logErr("读取家族目录\n");
  readFamily(g_saac_config.familydir);
#endif
  logErr("准备档案目录\n");
  PrepareDirectories(g_saac_config.chardir);
  logErr("准备日志目录\n");
  PrepareDirectories(g_saac_config.logdir);
  logErr("准备邮件目录\n");
  PrepareDirectories(g_saac_config.maildir);
#ifdef _SLEEP_CHAR
  PrepareDirectories(g_saac_config.sleepchardir);
  logErr("准备睡眠档案目录\n");
#endif
  if (Mail_read(g_saac_config.maildir) < 0) {
    logErr("不能初始化邮件\n");
    exit(1);
  }
  /* TCPSTRUCT */
  do {
    int tcpr;
    if ((tcpr = tcpstruct_init(NULL, g_saac_config.port, 0,
                               CHARDATASIZE * 16 * MAXCONNECTION,
                               1 /* DEBUG */)) == 0) {
      // break 是TCP INIT 成功
      break;
    }
    logErr("监听TCP端口失败, 错误代码: %d, 1s后重新尝试...\n", tcpr);
#ifdef _WIN32
    sa_sleep(1);
#else
    sleep(1);
#endif
  } while (TRUE);
  printf("TCP连接建立成功: %d %d\n", CHARDATASIZE,
         SAAC_SERVER_MAXLSRPCARGS);
  InitWorkSpace(&gSaacWorkSpace, tcpstruct_write, CHARDATASIZE,
                SAAC_SERVER_MAXLSRPCARGS);

#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
  logErr("初始化家族PK相关......");
  load_fm_pk_list();
  logErr("成功.\n");
#endif

#ifdef _ACFMPK_LIST
  logErr("加载家族PK......");
  FMPK_LoadList();
  logErr("成功.\n");
#endif
#ifdef _ALLDOMAN
  logErr("Load HeroList...");
  LOAD_herolist(); // Syu ADD 排行榜NPC
  logErr("Succeed.\n");
#endif

#ifdef _ANGEL_SUMMON
  logErr("Load Mission Table...");
  mission_table_init();
  logErr("Succeed.\n");
#endif
  logErr("\n服务端版本: <%s>\n", SERVER_VERSION);
  logErr("\n开始工作.....\n");

  int itime = 0;
  int new_ti, i, j;
  static BOOL lottery = FALSE;
  static time_t main_loop_time;
  while (TRUE) {
    sys_time = time(NULL);
#ifdef _LOTTERY_SYSTEM
    // 周期性的抽奖活动
    char todayaward[256] = "-1,-1,-1,-1,-1,-1,-1";
    {
      if (g_saac_config.lotterysystem > 0) {
        struct tm *p = localtime(&sys_time); /*取得当地时间*/
        if (lottery == FALSE) {
          if ((p->tm_mday % g_saac_config.lotterysystem) == 0) {
            if (p->tm_hour == 0) {
              int award[7];
              for (i = 0; i < 7; i++) {
                award[i] = rand() % 36 + 1;
                for (j = 0; j < i; j++) {
                  if (award[i] == award[j]) {
                    award[i] = rand() % 36 + 1;
                    j = 0;
                  }
                }
              }
              sprintf(todayaward, "%d,%d,%d,%d,%d,%d,%d", award[0], award[1],
                      award[2], award[3], award[4], award[5], award[6]);
              FILE *f1 = fopen("todayaward.txt", "w+");
              fputs(todayaward, f1);
              fclose(f1);
              for (i = 0; i < MAXCONNECTION; i++) {
                if (gs[i].use && gs[i].name[0]) {
                  SaacServer_LotterySystem_send(i, todayaward);
                }
              }
              lottery = TRUE;
            }
          }
        } else {
          if (p->tm_hour != 0) {
            lottery = FALSE;
          }
        }
      }
    }
#endif

    if (main_loop_time != sys_time) {
      main_loop_time = time(NULL);
      counter1++; // 档案刷新间隔
      counter2++; // 邮件过期时间
      counter3++; // 家族数据刷新间隔
      counter4++;
      counter5++; // 家族数据持久化间隔
      counter6++; // 读取SAAC配置文件时间间隔. 600s
      if (counter6 > 600) {
        saac_read_config(DEFAULT_CONFIG_FILE);
        counter6 = 0;
      }
      // andy add 2002/06/20
      UNlockM_UnlockPlayer();

#ifdef _ANGEL_SUMMON
      checkMissionTimelimit();
#endif
      // Nuke *1 1012
      if (counter1 > g_saac_config.Total_Charlist) {
        counter1 = 0;
        char *c = ctime(&main_loop_time);
        if (c) {
          struct timeval st, et;
          logErr("\nTIME:%s\n", c);
          gettimeofday(&st, NULL);
          dbFlush(g_saac_config.dbdir);
          gettimeofday(&et, NULL);
          logErr("持久化数据, %fsecs.\n", time_diff(et, st));
          logErr("档案表列总数:%d NG:%d\n", total_ok_charlist,
                 total_ng_charlist);
        }
      }
      // Nuke **1 1012
      // if( ( counter % 600 ) == 0 ){
      if (counter2 > g_saac_config.Expired_mail) {
        counter2 = 0;
        struct timeval st, et;
        gettimeofday(&st, NULL);
        Mail_expire();
        gettimeofday(&et, NULL);
        logErr("处理过期邮件, %fsecs.\n", time_diff(et, st));
      }
#ifdef _FAMILY
#ifdef _DEATH_FAMILY_LOGIN_CHECK
      // if ((counter % 300) == 0) // 300( -> 60)
      if ((counter4 % 1800) == 0) // 3hr( -> 1min)
      {
        counter4 = 0;
        struct timeval st, et;
        time_t t1;
        gettimeofday(&st, NULL);
        time(&t1);
        delovertimeFMMem(t1);
        gettimeofday(&et, NULL);
        logErr("删除过期家族或家族成员, %fsecs.\n", time_diff(et, st));
      }
#endif
      if (counter5 > g_saac_config.Write_Family) // 300( -> 60)
      {
        counter5 = 0;
        struct timeval st, et;
        gettimeofday(&st, NULL);
        writeFamily(g_saac_config.familydir);
        writeFMPoint(g_saac_config.fmpointdir);
        writeFMSMemo(g_saac_config.fmsmemodir);
        gettimeofday(&et, NULL);
        logErr("持久化家族数据, %fsecs.\n", time_diff(et, st));
      }
#endif
    }

    // 单线程处理新进入的连接
    new_ti = tcpstruct_accept1();
    if (new_ti >= 0) {
      logErr("建立连接: %d\n", new_ti);
      gs[new_ti].use = 1;
    }

    for (i = 0; i < MAXCONNECTION; i++) {
      if (!gs[i].use)
        continue;
      // 不断再循环这个过程
      // print("%d\n", i);
      const int read_bytes = tcpstruct_readline_chop(i, sTcpBuf, sizeof(sTcpBuf) - 1);
      if (read_bytes > 0) {
        sTcpBuf[read_bytes] = 0;
        int rc = SaacServer_ServerDispatchMessage(i, sTcpBuf, sDebugFun);
        // print("rc: %d\n", rc);
        if (rc < 0) {
          logOut("line :%s;%d\n", sTcpBuf, strlen(sTcpBuf));
          char token[256];
          struct tm now;
          time_t timep;
          time(&timep);
          memcpy(&now, localtime(&timep), sizeof(now));
          sprintf(token, "[%s]GMSV(%s)消息:%02d:%02d:%02d\n",
            gs[i].name, sDebugFun,
            now.tm_hour, now.tm_min, now.tm_sec);
          logFileToday(token);
        }
        // logErr("成功处理请求: %d, 服务器名:%s, readbytes: %d, msg:%s, debug:%s\n",
        //   i, gs[i].name, read_bytes, sTcpBuf, sDebugFun);
      } else if (read_bytes == TCPSTRUCT_ETOOLONG) {
        logFileToday(
            "连接%d, 接收到的数据长度超过预期, 服务器名:%s, 链接登出.\n", i,
            gs[i].name);
        logout_game_server(i);
      } else if (read_bytes < 0) {
        logErr("关闭连接: %d, 服务器名:%s, read_bytes:%d\n", i, gs[i].name, read_bytes);
        logout_game_server(i);
      } else if (read_bytes == 0) {
      }
    } // while 
  }

  return 0;
}

int get_rotate_count(void) {
  unsigned int t = (unsigned int) time(NULL);
  return (int) (t / g_saac_config.log_rotate_interval) *
          g_saac_config.log_rotate_interval;
}

int appendReadBuffer(int index, char *data, int len) {
  int top = g_con[index].mbtop_ri;
  for (;;) {
    int nextind = g_mem_buffer[top].next;
    if (nextind == -1)
      break;
    top = nextind;
  }
  return appendMemBufList(top, data, len);
}

int appendWriteBuffer(int index, char *data, int len) {
  int top = g_con[index].mbtop_wi;
  for (;;) {
    int nextind = g_mem_buffer[top].next;
    if (nextind == -1)
      break;
    top = nextind;
  }
  return appendMemBufList(top, data, len);
}

// 
int appendMemBufList(int top, char *data, int len) {
  int fr = getFreeMem();
  int rest = len;
  int data_topaddr = 0;

  if (len >= fr) {
    logErr("appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
    return -1;
  }
  data[len] = 0;
  for (;;) {
    int blanksize = sizeof(g_mem_buffer[0].buf) - g_mem_buffer[top].len;
    int cpsize = (rest <= blanksize) ? rest : blanksize;
    memcpy(g_mem_buffer[top].buf + g_mem_buffer[top].len, data + data_topaddr,
           cpsize);
    g_mem_buffer[top].len += cpsize;
    if (rest <= blanksize) {
      return len;
    } else {
      int newmb;
      rest -= cpsize;
      data_topaddr += cpsize;
      if ((newmb = allocateMemBuf()) == TCPSTRUCT_EMBFULL) {
        FILE *fp;
        if ((fp = fopen("badsysinfo.txt", "a+")) != NULL) {
          fprintf(fp, "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
          fclose(fp);
        }
        logErr("appendMemBufList: 内存缓冲区已满, 丢弃 %d 字节数据\n", rest);
        return -1;
      }
      g_mem_buffer[top].next = newmb;
      top = g_mem_buffer[top].next;
    }
  }
  return TCPSTRUCT_EBUG;
}

int consumeMemBufList(int top,                  // top: 链表头节点索引(mbtop_ri, mbtop_wi)
                      char *output_buffer,      // 输出缓冲区
                      const int max_consumed_bytes,   // 最多读取多少个字节
                      int is_consume,           // 是否消费数据
                      int is_copy) {            // 是否拷贝到输出缓冲区
  int consumed_bytes = 0, cpsize;
  int head = top, prev;
  int mem_buffer_size = g_mem_buffer_size;
  while (top != -1 && consumed_bytes < max_consumed_bytes) {
    if (top < -1 || top >= g_mem_buffer_size || --mem_buffer_size < 0) {
      // top == -1 是正常的
      logErr("[consumeMemBufList]:链表遍历异常,"
        "top=%d, mem_buffer_size=%d, 可能存在环或损坏.\n", top, mem_buffer_size);
      break;
    }
    cpsize = (g_mem_buffer[top].len <= (max_consumed_bytes - consumed_bytes)) ?
      g_mem_buffer[top].len : (max_consumed_bytes - consumed_bytes);
    // 拷贝数据
    if (is_copy)
      memcpy(output_buffer + consumed_bytes, g_mem_buffer[top].buf, cpsize);
    // 无论是否拷贝，consumed_bytes都要加
    consumed_bytes += cpsize;

    if (is_consume) {
      g_mem_buffer[top].len -= cpsize;
      if (g_mem_buffer[top].len > 0) {
        memmove(g_mem_buffer[top].buf, g_mem_buffer[top].buf + cpsize,
                sizeof(g_mem_buffer[top].buf) - cpsize);
      }
    }
    top = g_mem_buffer[top].next;
  }

  if (is_consume && head >= 0) {
    top = g_mem_buffer[head].next;
    mem_buffer_size = g_mem_buffer_size;
    while (top != -1) {
      if (top < 0 || top >= g_mem_buffer_size || --mem_buffer_size <= 0) {
        logErr("[consumeMemBufList]:链表清理异常,"
          "top=%d, mem_buffer_size=%d, 可能存在环或损坏.\n", top, mem_buffer_size);
        break;
      }
      if (g_mem_buffer[top].len == 0) {
        g_mem_buffer[head].next = g_mem_buffer[top].next;
        prev = top;
        top = g_mem_buffer[top].next;
        releaseMemBuf(prev);
      } else {
        top = g_mem_buffer[top].next;
      }
    }
  }
  return consumed_bytes;
}

// 从ti所在的链接中读取之多max_len个字节
int getLineReadBuffer(int ti, char *buf, int max_len) {
  int top = g_con[ti].mbtop_ri;
  int len = 0, breakflag = 0;
  int flag = TRUE;
  while (flag) {
    // 2026.08.21 这里有一个修复
    // 如果在这里break出来, 直接返回0就行嘛
    if (top < 0 || top >= g_mem_buffer_size)
      return 0;
    int l = g_mem_buffer[top].len, i;
    for (i = 0; i < l; ++i) {
      if (g_mem_buffer[top].buf[i] == '\n') {
        flag = FALSE;
        break;
      }
      ++len;
    }
    top = g_mem_buffer[top].next;
  }
  if (len > max_len) {
    return TCPSTRUCT_ETOOLONG;
  }

  // 消费并拷贝ti + 1 个字节到 buf
  return consumeMemBufList(g_con[ti].mbtop_ri, buf, len + 1, 1, 1);
}

int getFreeMem(void) {
  return (g_mem_buffer_size - g_mem_buffer_used) * sizeof(g_mem_buffer[0].buf);
}

int allocateMemBuf(void) {
  int i;
  for (i = 0; i < g_mem_buffer_size; i++) {
    g_mem_buffer_finder++;
    if (g_mem_buffer_finder >= g_mem_buffer_size || g_mem_buffer_finder < 0)
      g_mem_buffer_finder = 0;
    if (g_mem_buffer[g_mem_buffer_finder].use == 0) {
      g_mem_buffer[g_mem_buffer_finder].use = 1;
      g_mem_buffer[g_mem_buffer_finder].len = 0;
      g_mem_buffer[g_mem_buffer_finder].next = -1;
      g_mem_buffer_used++;
      return g_mem_buffer_finder;
    }
  }
  return TCPSTRUCT_EMBFULL;
}

int releaseMemBuf(const int index) {
  g_mem_buffer[index].use = 0;
  g_mem_buffer[index].next = -1;
  g_mem_buffer[index].len = 0;
  --g_mem_buffer_used;
  return TCPSTRUCT_OK;
}

int findregBlankCon(void) {
  int i;
  // Nuke changed 0->1
  for (i = 1; i < MAXCONNECTION; i++) {
    if (g_con[i].use == 0) {
      g_con[i].use = 1;
      g_con[i].fd = -1;
      g_con[i].mbtop_ri = allocateMemBuf();
      if (g_con[i].mbtop_ri < 0) {
        g_con[i].use = 0;
        fprintf(stderr, "EMBFULL\n");
        return TCPSTRUCT_EMBFULL;
      }
      g_con[i].mbtop_wi = allocateMemBuf();
      if (g_con[i].mbtop_wi < 0) {
        releaseMemBuf(g_con[i].mbtop_ri);
        g_con[i].use = 0;
        g_con[i].mbtop_ri = -1;
        fprintf(stderr, "EMBFULL\n");
        return TCPSTRUCT_EMBFULL;
      }
      memset(&g_con[i].remoteaddr, 0, sizeof(struct sockaddr_in));
      g_con[i].closed_by_remote = 0;
      return i;
    }
  }
  return TCPSTRUCT_ECFULL;
}

void checkGSUCheck(char *id) {
  int i;
  char gname[256];
  if (!id[0])
    return;
  memset(gname, 0, sizeof(gname));
  if (LockNode_getGname((getHash(id) & 0xff), id, gname) <= 0) {
    logErr("无法从游戏中找到账号:%x/%s !!\n", getHash(id), id);
    return;
  }
  for (i = 0; i < MAXCONNECTION; i++) {
    if (gs[i].name[0] && strcmp(gs[i].name, gname) == 0) {
      logErr("发送解锁检查[%s] 到 %d.%x/%s 服务器:%d !!\n", id, i, getHash(id),
             gname, gs[i].fd);
      SaacServer_ACUCheck_send(gs[i].fd, id);
      return;
    }
  }

  int ret = -1;
  if (!isLocked(id)) {
    logErr("删除内存信息: 用户:%x/%s 没有锁定!!\n", getHash(id), id);
  }
  if (DeleteMemLock(getHash(id) & 0xff, id, &ret)) {
    // ???
  } else {
    logErr("不能解锁 %x:%s !\n", getHash(id), id);
  }
}

void set_nodelay(int sock) {
  int flag = 1;
  int result =
      setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
  if (result < 0) {
    logErr("不能设置延迟.\n");
  } else {
    logErr("设置延迟: fd:%d\n", sock);
  }
}

void gmsvBroadcast(int fd, char *p1, char *p2, char *p3, int flag) {
  int i, c = 0;
  for (i = 0; i < MAXCONNECTION; i++) {
    if ((flag == 1) && (i == fd))
      continue;
    if (gs[i].use && gs[i].name[0]) {
      SaacServer_Broadcast_send(i, p1, p2, p3);
      c++;
    }
  }
}

#ifdef _ANGEL_SUMMON

#define MISSIONFILE "db/mission_table.txt"

int mission_table_init(void) {
  FILE *fp;
  char onedata[1024];
  char buf[1024];
  int index = 0;
  memset(g_mission_table, 0, sizeof(g_mission_table));
  fp = fopen(MISSIONFILE, "r");
  if (!fp) {
    logErr("\n读取精灵召唤mission_table文件错误\n");
    return FALSE;
  }
  logErr("\n加载精灵召唤数据...");
  while (1) {
    //
    if (fgets(onedata, sizeof(onedata), fp) == NULL)
      break;
    if (onedata[0] == '\0' || onedata[0] == '#')
      continue;
    easyGetTokenFromBuf(onedata, ",", 1, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    strcpy(g_mission_table[index].angelinfo, buf);
    easyGetTokenFromBuf(onedata, ",", 2, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    strcpy(g_mission_table[index].heroinfo, buf);
    easyGetTokenFromBuf(onedata, ",", 3, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    g_mission_table[index].mission = atoi(buf);
    easyGetTokenFromBuf(onedata, ",", 4, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    g_mission_table[index].flag = atoi(buf);
    easyGetTokenFromBuf(onedata, ",", 5, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    g_mission_table[index].time = atoi(buf);
    easyGetTokenFromBuf(onedata, ",", 6, buf, sizeof(buf));
    if (buf[0] == '\0')
      continue;
    g_mission_table[index].limittime = atoi(buf);
    logErr("%d=%s,%s,%d,%d,%d,%d \n", index, g_mission_table[index].angelinfo,
           g_mission_table[index].heroinfo, g_mission_table[index].mission,
           g_mission_table[index].flag, g_mission_table[index].time,
           g_mission_table[index].limittime);
    index++;
    if (index >= MISSTION_TABLE_SIZE)
      break;
  }
  fclose(fp);
  logErr("...成功!\n");
  return TRUE;
}

int mission_table_save(void) {
  FILE *fp;
  char onedata[1024];
  int index = 0;

  fp = fopen(MISSIONFILE, "w");
  if (!fp) {
    logErr("\n打开精灵召唤错误!!!! \n");
    return FALSE;
  }
  logErr("\n保存精灵召唤...");
  for (index = 0; index < MISSTION_TABLE_SIZE; index++) {

    if (g_mission_table[index].angelinfo[0] == '\0')
      continue;
    sprintf(onedata, "%s,%s,%d,%d,%d,%d\n", g_mission_table[index].angelinfo,
            g_mission_table[index].heroinfo, g_mission_table[index].mission,
            g_mission_table[index].flag, g_mission_table[index].time,
            g_mission_table[index].limittime);
    fputs(onedata, fp);
  }
  fclose(fp);
  logErr("..成功! \n");
  return TRUE;
}

void mission_table_delete(int index) {
  int gi;

  logErr("\n删除精灵召唤:%d:%s:%s \n", index, g_mission_table[index].angelinfo,
         g_mission_table[index].heroinfo);

  if (index < 0 || index >= MISSTION_TABLE_SIZE)
    return;
  strcpy(g_mission_table[index].angelinfo, "");
  strcpy(g_mission_table[index].heroinfo, "");
  g_mission_table[index].mission = 0;
  g_mission_table[index].flag = MISSION_NONE;
  g_mission_table[index].time = 0;
  g_mission_table[index].limittime = 0;

  for (gi = 0; gi < MAXCONNECTION; gi++) {
    if (gs[gi].use && gs[gi].name[0]) {
      SaacServer_ACMissionTable_send(gi, index, 3, "", "");
    }
  }
}

#define ANSWERTIME 1 // 等待回答时间(小时)
// #define DOINGTIME 3*24 // 任务时间(小时)
#define BOUNDSTIME 1 * 24 // 保留时间(小时)

void checkMissionTimelimit(void) {
  int index;
  static time_t lastcheck = 0;

  if (sys_time < lastcheck + 5 * 60)
    return;

  logErr("\n检查精灵召唤时间限制:%d \n", (int)sys_time);
  for (index = 0; index < MISSTION_TABLE_SIZE; index++) {
    if (g_mission_table[index].flag == MISSION_NONE) {
      continue;
    }
    // 等待使者回应1小时
    else if (g_mission_table[index].flag == MISSION_WAIT_ANSWER &&
             sys_time > g_mission_table[index].time + ANSWERTIME * 60 * 60) {

      mission_table_delete(index); // 删
    }
    // 等待领奖完成 limittime小时
    else if ((g_mission_table[index].flag == MISSION_DOING ||
              g_mission_table[index].flag == MISSION_HERO_COMPLETE) &&
             (sys_time > (g_mission_table[index].time +
                          g_mission_table[index].limittime * 60 * 60))) {

      char buf[1024];
      int gi;
      // 改TIMEOVER
      logErr("精灵召唤及领奖时间过:%d.", index);
      g_mission_table[index].flag = MISSION_TIMEOVER;
      g_mission_table[index].time = time(NULL);
      g_mission_table[index].limittime = BOUNDSTIME;

      sprintf(buf, "%d|%s|%s|%d|%d|%d|%d ", index,
              g_mission_table[index].angelinfo, g_mission_table[index].heroinfo,
              g_mission_table[index].mission, g_mission_table[index].flag,
              g_mission_table[index].time, g_mission_table[index].limittime);
      for (gi = 0; gi < MAXCONNECTION; gi++) {
        if (gs[gi].use && gs[gi].name[0]) {
          SaacServer_ACMissionTable_send(gi, 1, 1, buf, "");
        }
      }

      continue;
    }
    // 资料保留时间(BOUNDSTIME小时)
    else if (g_mission_table[index].flag == MISSION_TIMEOVER &&
             sys_time > g_mission_table[index].time + BOUNDSTIME * 60 * 60) {
      logErr("保留时间过:%d.", index);
      mission_table_delete(index); // 删
    }
  }
  mission_table_save();
#ifdef _SAVE_ZIP
  if (SAVEZIP > 0)
    savezipfile();
#endif
  lastcheck = sys_time;
}

#endif

#ifdef _SAVE_ZIP
void savezipfile(void) {
  time_t timep;
  time(&timep);
  struct tm *ptm;
  int y, m, d;
  timep = time(NULL);
  ptm = localtime(&timep);
  y = ptm->tm_year + 1900;
  m = ptm->tm_mon + 1;
  d = ptm->tm_mday;
  char command[768];
  sprintf(command, "%d-%d-%d.zip", y, m, d);
  if (access(command, W_OK) == 0)
    return; // 文件存在
#ifdef _WIN32
  snprintf(command, sizeof(command),
           "powershell.exe -NoProfile -NonInteractive -Command \""
           "$p=@('char','char_sleep','data','db','lock','log','mail','pklist',"
           "'race') | Where-Object { Test-Path $_ }; "
           "Compress-Archive -Path $p -DestinationPath '%d-%d-%d.zip' "
           "-Force\"",
           y, m, d);
#else
  snprintf(command, sizeof(command),
           "zip -q -r %d-%d-%d.zip char char_sleep data db "
           "lock log mail pklist race&",
           y, m, d);
#endif
  logErr("备份档案......");
  system(command); // 执行shell命令.
  logErr("成功!\n");
  return;
}
#endif
