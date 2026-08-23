#define __SAAC_CONFIG_C__
#include "saac_config.h"
#include "utils/util_string.h"


static char buf[2048];

int saac_read_config(const char *path) {
  FILE *fp;
  fp = fopen(path, "r");
  if (fp == NULL) {
    return -2;
  }
  while (fgets(buf, sizeof(buf), fp)) {
    char command[128];
    char param[128];
    chop(buf);
    easyGetTokenFromString(buf, 1, command, sizeof(command));
    easyGetTokenFromString(buf, 2, param, sizeof(param));
    if (strcmp(command, "port") == 0) {
      g_saac_config.port = atoi(param);
      logErr("端口:%d\n", g_saac_config.port);
    } else if (strcmp(command, "logdir") == 0) {
      snprintf(g_saac_config.logdir, sizeof(g_saac_config.logdir), param);
      logErr("日志目录:%s\n", g_saac_config.logdir);
    } else if (strcmp(command, "chardir") == 0) {
      snprintf(g_saac_config.chardir, sizeof(g_saac_config.chardir), param);
      logErr("档案目录:%s\n", g_saac_config.chardir);
#ifdef _SLEEP_CHAR
      snprintf(g_saac_config.sleepchardir, sizeof(g_saac_config.sleepchardir),
               "%s_sleep", g_saac_config.chardir);
      logErr("睡眠目录:%s\n", g_saac_config.sleepchardir);
#endif
    }
    else if (strcmp(command, "pass") == 0) {
      snprintf(g_saac_config.svpass, sizeof(g_saac_config.svpass), param);
      logErr("密码:%s\n", param);
    } else if (strcmp(command, "dbdir") == 0) {
      snprintf(g_saac_config.dbdir, sizeof(g_saac_config.dbdir), param);
      logErr("数据目录:%s\n", g_saac_config.dbdir);
    } else if (strcmp(command, "rotate_interval") == 0) {
      g_saac_config.log_rotate_interval = atoi(param);
      logErr("日志循环间隔:%d\n", g_saac_config.log_rotate_interval);
    } else if (strcmp(command, "maildir") == 0) {
      snprintf(g_saac_config.maildir, sizeof(g_saac_config.maildir), param);
      logErr("邮件目录:%s\n", g_saac_config.maildir);
    }
#ifdef _FAMILY
    else if (strcmp(command, "familydir") == 0) {
      snprintf(g_saac_config.familydir, sizeof(g_saac_config.familydir), param);
      logErr("家族目录:%s\n", g_saac_config.familydir);
    } else if (strcmp(command, "fmpointdir") == 0) {
      snprintf(g_saac_config.fmpointdir, sizeof(g_saac_config.fmpointdir), param);
      logErr("庄园表列:%s\n", g_saac_config.fmpointdir);
    } else if (strcmp(command, "fmsmemodir") == 0) {
      snprintf(g_saac_config.fmsmemodir, sizeof(g_saac_config.fmsmemodir), param);
      logErr("家族备份:%s\n", g_saac_config.fmsmemodir);
    }
#endif
    else if (strcmp(command, "Total_Charlist") == 0) {
      g_saac_config.Total_Charlist = atoi(param);
      logErr("更新人物点数间隔:%d秒\n", g_saac_config.Total_Charlist);
    } else if (strcmp(command, "Expired_mail") == 0) {
      g_saac_config.Expired_mail = atoi(param);
      logErr("更新过期邮件间隔:%d秒\n", g_saac_config.Expired_mail);
    } else if (strcmp(command, "Del_Family_or_Member") == 0) {
      g_saac_config.Del_Family_or_Member = atoi(param);
      logErr("删除家族成员间隔:%d秒\n", g_saac_config.Del_Family_or_Member);
    } else if (strcmp(command, "Write_Family") == 0) {
      g_saac_config.Write_Family = atoi(param);
      logErr("更新家族信息间隔:%d秒\n", g_saac_config.Write_Family);
    } else if (strcmp(command, "SameIpMun") == 0) {
      g_saac_config.sameipmun = atoi(param);
      if (g_saac_config.sameipmun > 0) {
        logErr("同IP允许同时登陆:%d次\n", g_saac_config.sameipmun);
      } else {
        logErr("同IP允许同时登陆:无限制\n");
      }
    }
#ifdef _AUTO_BACKUP
    else if (strcmp(command, "AUTOBACKUPDAY") == 0) {
      g_saac_config.autobackupday = atoi(param);
      logErr("每隔%d天备份一次数据！\n", g_saac_config.autobackupday);
    } else if (strcmp(command, "AUTOBACKUPHOUR") == 0) {
      g_saac_config.autobackuphour = atoi(param);
      logErr("每次在%d点进行备份！\n", g_saac_config.autobackuphour);
    }
#endif
#ifdef _LOTTERY_SYSTEM
    else if (strcmp(command, "LOTTERYSYSTEM") == 0) {
      g_saac_config.lotterysystem = atoi(param);
      logErr("每隔%d天开一次奖！\n", g_saac_config.lotterysystem);
    }
#endif
#ifdef _SAVE_ZIP
    else if (strcmp(command, "savezip") == 0) {
      g_saac_config.savezip = atoi(param);
      logErr("是否自动备份:%s\n", g_saac_config.savezip > 0 ? "是" : "否");
    }
#endif
    else if (strcmp(command, "servid") == 0) {
      g_saac_config.servid = atoi(param);
      logErr("服务器ID:%d\n", g_saac_config.servid);
    }
  }

  fclose(fp);
  return 0;
}