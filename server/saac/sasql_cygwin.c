#define _SASQL_C_
#include "version.h"

#ifdef _SASQL // 新添加
#include "main.h"
#include <sql.h>
#include <sqlext.h>


HENV henv = SQL_NULL_HENV;
HDBC hdbc = SQL_NULL_HDBC;
HSTMT hstmt = SQL_NULL_HSTMT;
RETCODE rc;

SQLCHAR sid[16];
SQLCHAR sps[16];
SQLINTEGER err;

#define SASQL_PASSWORD_MAX 32

typedef struct tagConfig {
  char SQL_DSN[128];
  char SQL_USER[64];
  char SQL_INFOTABLE[64];
  char SQL_LOCK[64];
  char SQL_RegTime[16];
  char SQL_LoginTime[16];
  char SQL_OnlineName[16];
} Config;

Config config;

int AUTOREG;
int USEMSSQL;
void check_return(RETCODE rc, HENV henv, HDBC hdbc, HSTMT hstmt);

static BOOL sasql_ready(void) {
  return USEMSSQL != 0 && hstmt != SQL_NULL_HSTMT;
}

static BOOL sasql_valid_value(const char *value) {
  const unsigned char *p = (const unsigned char *)value;
  if (value == NULL)
    return FALSE;
  while (*p) {
    if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
          (*p >= '0' && *p <= '9') || *p == '.' || *p == '-' || *p == '_' ||
          *p == '|'))
      return FALSE;
    ++p;
  }
  return TRUE;
}

static BOOL sasql_exec(const char *sql) {
  if (!sasql_ready())
    return FALSE;
  SQLCancel(hstmt);
  rc = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  return rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO;
}

static int readConfig(char *path) {
  char buf[255];
  FILE *fp;

  fp = fopen(path, "r");
  if (fp == NULL) {
    return -2;
  }

  while (fgets(buf, sizeof(buf), fp)) {
    char command[255];
    char param[255];
    chop(buf);

    easyGetTokenFromString(buf, 1, command, sizeof(command));
    easyGetTokenFromString(buf, 2, param, sizeof(param));

    if (strcmp(command, "USEMSSQL") == 0) {
      USEMSSQL = atoi(param);
      if (USEMSSQL == 0)
        break;
    }
    if (strcmp(command, "SQL_DSN") == 0) {
      snprintf(config.SQL_DSN, sizeof(config.SQL_DSN), "%s", param);
      logErr("数据库 DSN：%s\n", config.SQL_DSN);
    } else if (strcmp(command, "SQL_INFOTABLE") == 0) {
      snprintf(config.SQL_INFOTABLE, sizeof(config.SQL_INFOTABLE), "%s", param);
      logErr("人物信息表：%s\n", config.SQL_INFOTABLE);
    } else if (strcmp(command, "SQL_LOCK") == 0) {
      snprintf(config.SQL_LOCK, sizeof(config.SQL_LOCK), "%s", param);
      logErr("锁定账号表：%s\n", config.SQL_LOCK);
    } else if (strcmp(command, "SQL_REGTIME") == 0) {
      snprintf(config.SQL_RegTime, sizeof(config.SQL_RegTime), "%s", param);
      logErr("注册时间：%s\n", config.SQL_RegTime);
    } else if (strcmp(command, "AUTOREG") == 0) {
      AUTOREG = atoi(param);
      if (AUTOREG) {
        logErr("开放自动注册：YES\n");
      } else {
        logErr("开放自动注册：NO\n");
      }
    }
  }
  fclose(fp);
  return 0;
}

BOOL sasql_init(void) {
  memset(&config, 0, sizeof(config));
  AUTOREG = 0;
  USEMSSQL = 0;
  readConfig("acserv.cf");
  if (USEMSSQL == 0) {
    logErr("WARNING: MSSQL authentication is disabled; all player logins "
           "will be rejected.\n");
    return TRUE;
  }
  char buf[257];
  char ODBC[257];
  short buflen;
  //  printf ("Initialize the environment structure.\n");
  if (SQLAllocEnv(&henv) != SQL_SUCCESS)
    return FALSE;

  //  printf ("Initialize the connection structure.\n");
  if (SQLAllocConnect(henv, &hdbc) != SQL_SUCCESS)
    return FALSE;

  printf("连接ODBC驱动器...");
  sprintf(ODBC, "DSN=%s", config.SQL_DSN);

  rc = SQLDriverConnect(hdbc, 0, (SQLCHAR *)ODBC, SQL_NTS, (SQLCHAR *)buf,
                        sizeof(buf), &buflen, SQL_DRIVER_NOPROMPT);
  check_return(rc, henv, hdbc, hstmt);
  if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    return FALSE;
  printf("成功\n");
  if (SQLAllocStmt(hdbc, &hstmt) != SQL_SUCCESS)
    return FALSE;
  SQLBindCol(hstmt, 1, SQL_C_CHAR, &sid, 16, &err);
  SQLBindCol(hstmt, 2, SQL_C_CHAR, &sps, 16, &err);
  return TRUE;
}

void sasql_close(void) {
  if (USEMSSQL == 0)
    return;
  //  printf ("Free the statement handle.\n");
  SQLFreeStmt(hstmt, SQL_DROP);

  //  printf ("Disconnect from the data source.\n");
  SQLDisconnect(hdbc);

  //  printf ("Free the connection structure.\n");
  SQLFreeConnect(hdbc);

  //  printf ("Free the environment structure.\n");
  SQLFreeEnv(henv);
  hstmt = SQL_NULL_HSTMT;
  hdbc = SQL_NULL_HDBC;
  henv = SQL_NULL_HENV;
}

void check_return(RETCODE rc, HENV henv, HDBC hdbc, HSTMT hstmt) {
  UCHAR state_str[SQL_MAX_MESSAGE_LENGTH];
  SDWORD native_error;
  UCHAR error_msg[SQL_MAX_MESSAGE_LENGTH];
  SWORD error_msg_avail = SQL_MAX_MESSAGE_LENGTH - 1;
  SWORD error_msg_len;
  RETCODE local_rc;

  if (rc != SQL_ERROR && rc != SQL_SUCCESS_WITH_INFO) {
    return;
  }

  local_rc = SQLError(henv, hdbc, hstmt, state_str, &native_error, error_msg,
                      error_msg_avail, &error_msg_len);
  if (local_rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
    fprintf(stderr, "Uninterpretable error; exiting \n");
    exit(EXIT_FAILURE);
  }

  if (rc == SQL_SUCCESS_WITH_INFO) {
    fprintf(stderr, "Fatal Error %s: %s\n", state_str, error_msg);
    return;
  }

  fprintf(stderr, "Fatal Error %s: %s\n", state_str, error_msg);
  exit(EXIT_FAILURE);
}

int sasql_query(char *id, char *ps) {
  SQLCHAR sqlstr[160];
  SQLCHAR szPass[64] = {0};
  SQLLEN cbPass = 0;
  SQLLEN id_indicator = SQL_NTS;
  size_t id_len;
  size_t password_len;
  int query_result = 0;
  int sql_len;

  if (!sasql_ready() || id == NULL || ps == NULL)
    return 0;
  id_len = strlen(id);
  password_len = strlen(ps);
  if (id_len == 0 || id_len >= USERID_MAX || password_len == 0 ||
      password_len > SASQL_PASSWORD_MAX)
    return 0;

  sql_len = snprintf((char *)sqlstr, sizeof(sqlstr),
                     "select PassWord from %s where Name=?",
                     config.SQL_INFOTABLE);
  if (sql_len < 0 || (size_t)sql_len >= sizeof(sqlstr))
    return 0;

  SQLCancel(hstmt);
  SQLFreeStmt(hstmt, SQL_CLOSE);
  SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
  rc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                        (SQLULEN)id_len, 0, (SQLPOINTER)id,
                        (SQLLEN)(id_len + 1), &id_indicator);
  if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    goto cleanup;

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    rc = SQLFetch(hstmt);
    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
      rc = SQLGetData(hstmt, 1, SQL_C_CHAR, szPass, sizeof(szPass), &cbPass);
      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
        if (strcmp((char *)szPass, ps) == 0) {
          query_result = 1;
        } else {
          printf("账号%s密码错误!\n", id);
          query_result = 2;
        }
      }
    } else if (rc == SQL_NO_DATA) {
      printf("账号%s未注册!\n", id);
      query_result = 3;
    }
  }

cleanup:
  SQLFreeStmt(hstmt, SQL_CLOSE);
  SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
  return query_result;
}

BOOL sasql_register(const char *id, const char *ps) {
  if (sasql_ready() && AUTOREG && sasql_valid_value(id) &&
      sasql_valid_value(ps)) {
    SQLCHAR sqlstr[128];
    SQLCancel(hstmt);
    sprintf(sqlstr, "select * from %s where Name='%s'", config.SQL_INFOTABLE,
            id);
    rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
    check_return(rc, henv, hdbc, hstmt);
    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
      while (SQL_SUCCESS == SQLFetch(hstmt))
        return FALSE;
      SQLCancel(hstmt);
      sprintf(sqlstr,
              "INSERT INTO %s (Name,PassWord,RegTime,Path) VALUES "
              "('%s','%s',CURRENT_TIMESTAMP,'char/0x%x')",
              config.SQL_INFOTABLE, id, ps, getHash(id) & 0xff);
      rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
      check_return(rc, henv, hdbc, hstmt);
      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
        printf("新账号%s注册成功!\n", id);
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL sasql_craete_lock(void) {
  if (!sasql_ready())
    return TRUE;
  char sqlstr[128];
  SQLCancel(hstmt);
  sprintf(sqlstr, "create table %s (Name varchar(16),PRIMARY KEY (Name));",
          config.SQL_LOCK);
  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    printf("锁定用户表建立成功!\n");
    return TRUE;
  }
  printf("锁定用户表建立失败!\n");
  return FALSE;
}

BOOL sasql_craete_userinfo(void) {
  if (!sasql_ready())
    return TRUE;
  char sqlstr[255];
  SQLCancel(hstmt);
  sprintf(sqlstr,
          "CREATE TABLE %s ("
          "Name varchar(16) NOT NULL,"
          "PassWord varchar(16) NOT NULL,"
          "IP varchar(16),"
          "RegTime datetime,"
          "LoginTime datetime,"
          "OnlineName varchar(32),"
          "Online int,"
          "Path varchar(10),"
          "PRIMARY KEY (Name));",
          config.SQL_INFOTABLE);
  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    printf("用户信息表建立成功!\n");
    return TRUE;
  }
  printf("用户信息表建立失败!\n");
  return FALSE;
}

BOOL sasql_check_lock(char *username_ip) {
  if (!sasql_ready() || !sasql_valid_value(username_ip))
    return FALSE;
  char sqlstr[128];
  SQLCancel(hstmt);
  sprintf(sqlstr, "select * from %s where Name='%s'", config.SQL_LOCK, username_ip);
  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    while (SQL_SUCCESS == SQLFetch(hstmt))
      return TRUE;
  }
  return FALSE;
}

BOOL sasql_add_lock(char *idip) {
  if (!sasql_ready() || !sasql_valid_value(idip))
    return FALSE;
  char sqlstr[128];
  SQLCancel(hstmt);
  sprintf(sqlstr, "INSERT INTO %s (Name) VALUES ('%s')", config.SQL_LOCK, idip);
  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    return TRUE;
  }
  return FALSE;
}

BOOL sasql_del_lock(char *idip) {
  if (!sasql_ready() || !sasql_valid_value(idip))
    return FALSE;
  char sqlstr[128];
  SQLCancel(hstmt);
  sprintf(sqlstr, "delete from %s where Name='%s'",
          config.SQL_LOCK, idip);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    return TRUE;
  }
  return FALSE;
}

#ifdef _SQL_BACKGROUND
BOOL sasql_online(char *ID, char *NM, char *IP, char *MAC, int flag) {
  (void)MAC;
  if (!sasql_ready())
    return TRUE;
  char sqlstr[256];
  SQLCancel(hstmt);
  if (flag == 0)
    sprintf(sqlstr, "update %s set Online=0 where Name='%s'",
            config.SQL_INFOTABLE, ID);
  else if (flag == 1)
    sprintf(sqlstr,
            "update %s set LoginTime=CURRENT_TIMESTAMP, IP='%s', Online=1 "
            "where Name='%s'",
            config.SQL_INFOTABLE, IP, ID);
  else if (flag == 2)
    sprintf(sqlstr,
            "update %s set LoginTime=CURRENT_TIMESTAMP, OnlineName='%s', "
            "Online=1 where "
            "Name='%s'",
            config.SQL_INFOTABLE, NM, ID);
  else if (flag == 3)
    sprintf(sqlstr, "update %s set Online=0", config.SQL_INFOTABLE);
  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
    return TRUE;

  printf("\n更新数据库失败！\n");
  return FALSE;
}
#endif

int sasql_onlinenum(char *IP) {
  if (!sasql_ready() || !sasql_valid_value(IP))
    return 0;
  char sqlstr[256];
  SQLCHAR szNum[64] = {0};
  SQLINTEGER cbNum;
  SQLCancel(hstmt);

  sprintf(sqlstr, "select count(*) from %s where Online=1 and IP='%s'",
          config.SQL_INFOTABLE, IP);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    if (SQL_SUCCESS == SQLFetch(hstmt)) {
      if (SQLGetData(hstmt, 1, SQL_C_CHAR, szNum, 50, &cbNum) == SQL_SUCCESS) {
        return atoi(szNum);
      }
    }
  }
  return 0;
}

#ifdef _NEW_VIP_SHOP
int sasql_query_point(char *name) {
  if (!sasql_ready() || !sasql_valid_value(name))
    return -1;
  char sqlstr[256];
  SQLCHAR szPoint[64] = {0};
  SQLINTEGER cbPoint;
  SQLCancel(hstmt);

  sprintf(sqlstr, "select VipPoint from %s where Name='%s'",
          config.SQL_INFOTABLE, name);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    if (SQL_SUCCESS == SQLFetch(hstmt)) {
      if (SQLGetData(hstmt, 1, SQL_C_CHAR, szPoint, 50, &cbPoint) ==
          SQL_SUCCESS) {
        return atoi(szPoint);
      }
    }
  }
  return -1;
}

BOOL sasql_add_vippoint(char *ID, int point) {
  if (!sasql_ready() || !sasql_valid_value(ID))
    return -1;
  char sqlstr[256];
  SQLCHAR szPoint[64] = {0};
  SQLINTEGER cbPoint;
  SQLCancel(hstmt);

  sprintf(sqlstr, "select VipPoint from %s where Name='%s'",
          config.SQL_INFOTABLE, ID);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    if (SQL_SUCCESS == SQLFetch(hstmt)) {
      if (SQLGetData(hstmt, 1, SQL_C_CHAR, szPoint, 50, &cbPoint) ==
          SQL_SUCCESS) {
        point += atoi(szPoint);
        if (point < 0) {
          printf("%s会员点数小于0\n", ID);
          return -1;
        }
        SQLCancel(hstmt);
        sprintf(sqlstr, "update %s set VipPoint=%d where NAME='%s'",
                config.SQL_INFOTABLE, point, ID);
        rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
        check_return(rc, henv, hdbc, hstmt);
        if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
          printf("用户%s当前会员点%d！\n", ID, point);
          return point;
        }
      }
    }
  }
  printf("修改%s会员点数失败！\n", ID);
  return -1;
}
#endif

#ifdef _ITEM_PET_LOCKED
char *sasql_ItemPetLocked(char *id, char *safepasswd) {
  if (!sasql_ready())
    return "安全锁数据库功能未启用。";
  if (!sasql_valid_value(id) || !sasql_valid_value(safepasswd))
    return "安全锁参数无效。";
  char sqlstr[256];
  SQLCHAR szSafePasswd[32] = {0};
  SQLINTEGER cbSafePasswd;
  SQLCancel(hstmt);

  sprintf(sqlstr, "select SafePasswd from %s where Name='%s'",
          config.SQL_INFOTABLE, id);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    if (SQL_SUCCESS == SQLFetch(hstmt)) {
      if (SQLGetData(hstmt, 1, SQL_C_CHAR, szSafePasswd, 50, &cbSafePasswd) ==
          SQL_SUCCESS) {
        if (strlen(szSafePasswd) > 0) {
          if (strcmp(safepasswd, szSafePasswd) == 0) {
            return "安全锁已经成功解锁！";
          } else {
            return "密码错误，安全锁无法解开！";
          }
        } else {
          return "您还未设置安全锁解锁密码，为了确保安全，请输入一次六位以上密"
                 "码做为安全锁密码并劳劳记住！";
        }
      }
    }
  }
  return "无法解锁，请与本服管理员联系！";
}

char *sasql_ItemPetLocked_Passwd(char *id, char *safepasswd) {
  if (!sasql_ready())
    return "安全锁数据库功能未启用。";
  if (!sasql_valid_value(id) || !sasql_valid_value(safepasswd))
    return "安全锁参数无效。";
  char sqlstr[256];
  SQLCancel(hstmt);
  sprintf(sqlstr, "update %s set SafePasswd='%s' where Name='%s'",
          config.SQL_INFOTABLE, safepasswd, id);

  rc = SQLExecDirect(hstmt, sqlstr, SQL_NTS);
  check_return(rc, henv, hdbc, hstmt);
  if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
    return "安全密码修改成功，请妥善保管你的安全密码！";
  }
  return "安全密码修改失败，请与本服管理员联系！";
}

BOOL sasql_ItemPetLocked_Char(char *id, char *safepasswd) {
  char sqlstr[256];
  SQLCHAR stored[64] = {0};
  SQLINTEGER stored_len = 0;

  if (!sasql_ready())
    return 0;
  if (!sasql_valid_value(id) || !sasql_valid_value(safepasswd))
    return -1;

  snprintf(sqlstr, sizeof(sqlstr),
           "select SafePasswd from %s where Name='%s'",
           config.SQL_INFOTABLE, id);
  if (!sasql_exec(sqlstr) || SQLFetch(hstmt) != SQL_SUCCESS)
    return -1;
  if (SQLGetData(hstmt, 1, SQL_C_CHAR, stored, sizeof(stored), &stored_len) !=
      SQL_SUCCESS)
    return -1;
  if (stored_len <= 0 || stored[0] == '\0')
    return 0;
  return strcmp((char *)stored, safepasswd) == 0 ? 1 : -1;
}
#endif

#ifdef _ONLINE_COST
char *sasql_OnlineCost(char *id, char *costpasswd, int fmindex, char *fmname) {
  char sqlstr[256];
  SQLCHAR szCostVal[32] = {0};
  SQLCHAR szPayVal[32] = {0};
  SQLCHAR szTemp[32] = {0};
  SQLINTEGER cbCostVal;
  SQLINTEGER cbPayVal;
  SQLINTEGER cbTemp;
  static char token[256];

  (void)fmindex;
  (void)fmname;
  if (!sasql_ready())
    return "充值数据库功能未启用。";
  if (!sasql_valid_value(id) || !sasql_valid_value(costpasswd))
    return "充值失败，卡号格式不正确。";

  snprintf(sqlstr, sizeof(sqlstr),
           "select CostVal, PayVal, [check] from OnlineCost "
           "where CostPasswd='%s'",
           costpasswd);

  if (sasql_exec(sqlstr)) {
    if (SQL_SUCCESS == SQLFetch(hstmt)) {
      if (SQLGetData(hstmt, 1, SQL_C_CHAR, szCostVal, sizeof(szCostVal),
                     &cbCostVal) ==
              SQL_SUCCESS &&
          SQLGetData(hstmt, 2, SQL_C_CHAR, szPayVal, sizeof(szPayVal),
                     &cbPayVal) == SQL_SUCCESS &&
          SQLGetData(hstmt, 3, SQL_C_CHAR, szTemp, sizeof(szTemp), &cbTemp) ==
              SQL_SUCCESS) {
        if (atoi(szTemp) == 1) {
          int costval = atoi(szCostVal);
          int payval = atoi(szPayVal);
          int vippoint = sasql_add_vippoint(id, costval);
          snprintf(sqlstr, sizeof(sqlstr),
                   "update OnlineCost set cdkey='%s', "
                   "CostTime=CURRENT_TIMESTAMP, [check]=0 "
                   "where CostPasswd='%s'",
                   id, costpasswd);

          if (sasql_exec(sqlstr)) {
            logErr("充值卡号%s已充值！\n", costpasswd);
          }
          snprintf(token, sizeof(token),
                   "充值成功：会员点增加%d，充值积分为%d，当前会员点为%d。",
                   costval, payval, vippoint);
          return token;
        } else {
          return "该充值卡已使用过，请勿重复使用！";
        }
      } else {
        return "充值失败，该充值卡密码不正确！";
      }
    } else {
      return "查找不到该卡号，请与本服管理员联系！";
    }
  }
  return "充值失败，请与本服管理员联系！";
}

char *sasql_TransOnlineCost(void) {
  char sqlstr[512];
  static char result[128];

  if (!sasql_ready())
    return "充值数据库功能未启用。";
  snprintf(sqlstr, sizeof(sqlstr),
           "UPDATE u SET u.PayPoint=COALESCE(u.PayPoint,0)+o.PayVal "
           "FROM %s AS u INNER JOIN OnlineCost AS o ON u.Name=o.cdkey "
           "WHERE o.[check]=0",
           config.SQL_INFOTABLE);
  if (sasql_exec(sqlstr)) {
    snprintf(result, sizeof(result), "充值积分转换完成。");
  } else {
    snprintf(result, sizeof(result), "充值积分转换失败。");
  }
  return result;
}

void sasql_OnlineCost_add(int cost, int num, int point) {
  char sqlstr[256];
  char CostPasswd[32];
  int i, j, added = 0;

  if (!sasql_ready() || num <= 0)
    return;
  for (i = 0; i < num; i++) {
    memset(CostPasswd, 0, sizeof(CostPasswd));
    for (j = 0; j < 16; j++) {
      CostPasswd[j] = rand() % 2 ? (rand() % 26) + 'A' : (rand() % 10) + '0';
    }
    snprintf(sqlstr, sizeof(sqlstr),
             "INSERT INTO OnlineCost (CostPasswd, CostVal, PayVal) "
             "VALUES ('%s', %d, %d)",
             CostPasswd, cost, point);
    if (sasql_exec(sqlstr))
      ++added;
  }
  printf("成功添加%d条面值%d的充值卡\n", added, cost);
}
#endif

#ifdef _SQL_BUY_FUNC
char *sasql_OnlineBuy(char *id, char *costpasswd) {
  char sqlstr[512];
  SQLCHAR coststr[256] = {0};
  SQLCHAR used[16] = {0};
  SQLINTEGER coststr_len = 0;
  SQLINTEGER used_len = 0;
  static char token[320];

  if (!sasql_ready())
    return "提货数据库功能未启用。";
  if (!sasql_valid_value(id) || !sasql_valid_value(costpasswd))
    return "提货失败，卡号格式不正确。";

  snprintf(sqlstr, sizeof(sqlstr),
           "select CostStr, [check] from OnlineBuy where CostPasswd='%s'",
           costpasswd);
  if (!sasql_exec(sqlstr) || SQLFetch(hstmt) != SQL_SUCCESS)
    return "提货失败，找不到该提货卡。";
  if (SQLGetData(hstmt, 1, SQL_C_CHAR, coststr, sizeof(coststr), &coststr_len) !=
          SQL_SUCCESS ||
      SQLGetData(hstmt, 2, SQL_C_CHAR, used, sizeof(used), &used_len) !=
          SQL_SUCCESS)
    return "提货失败，无法读取提货卡。";
  if (atoi((char *)used) != 1)
    return "该提货卡已使用，请勿重复使用。";

#ifdef _SQL_BUY_FUNC_ONE_CDKEY
  if (costpasswd[0] == '1') {
    SQLCHAR count[32] = {0};
    SQLINTEGER count_len = 0;
    snprintf(sqlstr, sizeof(sqlstr),
             "select count(*) from OnlineBuy where CostStr='%s' and cdkey='%s'",
             (char *)coststr, id);
    if (sasql_exec(sqlstr) && SQLFetch(hstmt) == SQL_SUCCESS &&
        SQLGetData(hstmt, 1, SQL_C_CHAR, count, sizeof(count), &count_len) ==
            SQL_SUCCESS &&
        atoi((char *)count) > 0)
      return "相同类型的道具卡每个账号只能领取一次。";
  }
#endif

  snprintf(sqlstr, sizeof(sqlstr),
           "update OnlineBuy set cdkey='%s', CostTime=CURRENT_TIMESTAMP, "
           "[check]=0 where CostPasswd='%s'",
           id, costpasswd);
  if (!sasql_exec(sqlstr))
    return "提货失败，请与管理员联系。";
  snprintf(token, sizeof(token), "%c|%s", costpasswd[0], (char *)coststr);
  return token;
}

void sasql_OnlineBuy_add(char *coststr, int type, int num) {
  char sqlstr[512];
  char password[32];
  int i, j, added = 0;

  if (!sasql_ready() || !sasql_valid_value(coststr) || num <= 0)
    return;
  for (i = 0; i < num; ++i) {
    memset(password, 0, sizeof(password));
    password[0] = (char)('0' + type);
    for (j = 1; j < 16; ++j)
      password[j] = rand() % 2 ? (rand() % 26) + 'A' : (rand() % 10) + '0';
    snprintf(sqlstr, sizeof(sqlstr),
             "INSERT INTO OnlineBuy (CostPasswd, CostStr) VALUES ('%s','%s')",
             password, coststr);
    if (sasql_exec(sqlstr))
      ++added;
  }
  printf("成功添加%d条提货卡\n", added);
}
#endif

#ifdef _OLDPS_TO_MD5PS
void sasql_OldpsToMd5ps(void) {
  if (!sasql_ready())
    return;
  logErr("Windows ODBC 模式不支持批量旧密码转换；请使用数据库维护工具执行。\n");
}
#endif

void sasql_CleanCdkey(int date) {
  (void)date;
  if (sasql_ready())
    logErr("Windows ODBC 模式不执行账号文件批量删除。\n");
}

void sasql_CleanLockCdkey(void) {
  if (sasql_ready())
    logErr("Windows ODBC 模式不执行锁定账号文件批量删除。\n");
}

#endif
