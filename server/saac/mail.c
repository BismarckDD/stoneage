#define __MAIL_C__
#include "mail.h"
#include "main.h"
#include "saacproto_serv.h"
#include "util.h"

typedef enum {
  MS_NOUSE = 0,
  MS_NEWMESSAGE,
  MS_WAIT_ACK,
} MAILSTATE;

#define TEXT_MAX 1024
struct mail {
  int use;
  unsigned int id_char_name_hash;
  char id_to[USERID_MAX];
  char char_name_to[CHARNAME_MAX];
  char id_from[USERID_MAX];
  char char_name_from[CHARNAME_MAX];
  char text[TEXT_MAX];
  int option;
  unsigned int message_id;
  MAILSTATE state;
  time_t recv_time;
};

struct mail *mailbuf;
int mailbufsize = 0;

static unsigned getNextMessageID(void) {
  FILE *fp;
  unsigned i;
  char filename[1024];
  char line[1000];
  snprintf(filename, sizeof(filename), "%s/mail_id", maildir);
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fp = fopen(filename, "w");
    if (fp == NULL) {
      log("不能创建 %s ... 使用同样的邮件ID,"
          " saac 发送变得缓慢!(id:9999)\n",
          filename);
      return 9999;
    }
    fprintf(fp, "10000\n");
    fclose(fp);
    return 1000;
  }
  fgets(line, sizeof(line), fp);
  i = strtoul(line, NULL, 10);
  fclose(fp);

  fp = fopen(filename, "w");
  if (fp == NULL) {
    log("不能写入新的ID到 %s ... 使用同样的数字!\n", filename);
    return i;
  }
  fprintf(fp, "%u", i + 1);
  fclose(fp);

  log("新邮件ID:%u\n", i);
  return i;
}

static int reallocMailBuf(void) {
  struct mail *previous = mailbuf;
  struct mail *newbuf;
  int new_mailbufsize;
  if (mailbufsize == 0) {
    new_mailbufsize = 1;
  } else {
    new_mailbufsize = mailbufsize * 2;
  }

  newbuf = (struct mail *)calloc(1, new_mailbufsize * sizeof(struct mail));
  if (newbuf == NULL) {
    log("回复邮件缓冲: 内件不足!! 新邮件大小:%d\n", new_mailbufsize);
    return -1;
  }
  memset(newbuf, 0, new_mailbufsize * sizeof(struct mail));
  if (previous)
    memcpy((char *)newbuf, (char *)previous, mailbufsize * sizeof(struct mail));
  free(previous);
  mailbufsize = new_mailbufsize;
  mailbuf = newbuf;

  log("重新分配邮件缓冲: "
      "新邮件缓冲:%d 旧地址:%x 新地址:%x\n",
      new_mailbufsize, (unsigned int)previous, (unsigned int)newbuf);
  return 0;
}

static int mailbuf_finder = 0;
static int allocMail(int use_msgid, unsigned int msgid) {
  int i;
  for (i = 0; i < mailbufsize; i++) {
    mailbuf_finder++;
    if (mailbuf_finder == mailbufsize)
      mailbuf_finder = 0;
    if (mailbuf[mailbuf_finder].use == 0) {
      mailbuf[mailbuf_finder].use = 1;
      mailbuf[mailbuf_finder].text[0] = 0;
      mailbuf[mailbuf_finder].id_to[0] = 0;
      mailbuf[mailbuf_finder].char_name_to[0] = 0;
      mailbuf[mailbuf_finder].id_from[0] = 0;
      mailbuf[mailbuf_finder].char_name_from[0] = 0;
      if (use_msgid) {
        mailbuf[mailbuf_finder].message_id = msgid;
      } else {
        mailbuf[mailbuf_finder].message_id = getNextMessageID();
      }
      mailbuf[mailbuf_finder].state = MS_NEWMESSAGE;
      mailbuf[mailbuf_finder].recv_time = time(NULL);
      return mailbuf_finder;
    }
  }
  log("分配邮件: 邮件缓冲失败.正在进行分配...\n");
  if (reallocMailBuf() < 0) {
    log("分配邮件: 分配失败\n");
  } else {
    return allocMail(use_msgid, msgid);
  }

  return -1;
}

void receiveMail(const char *id_from, const char *char_name_from,
                 const char *id_to, const char *char_name_to,
                 const char *message, const int option,
                 const unsigned use_msgid, const unsigned msgid) {
  char id_char_name[1024];
  int h, mbindex;

  snprintf(id_char_name, sizeof(id_char_name), "%s_%s", id_to, char_name_to);
  h = hashpjw(id_char_name) & 0xff;
  if ((mbindex = allocMail(use_msgid, msgid)) < 0) {
    log("回复邮件: 获取新的邮件缓冲失败.\n");
    return;
  }
  mailbuf[mbindex].id_char_name_hash = h;
  snprintf(mailbuf[mbindex].text, sizeof(mailbuf[mbindex].text), "%s", message);
  snprintf(mailbuf[mbindex].id_to, sizeof(mailbuf[mbindex].id_to), "%s", id_to);
  snprintf(mailbuf[mbindex].char_name_to, sizeof(mailbuf[mbindex].char_name_to),
           "%s", char_name_to);
  snprintf(mailbuf[mbindex].id_from, sizeof(mailbuf[mbindex].id_from), "%s",
           id_from);
  snprintf(mailbuf[mbindex].char_name_from,
           sizeof(mailbuf[mbindex].char_name_from), "%s", char_name_from);
  mailbuf[mbindex].option = option;

  {
    char childname[1000];
    char savefile[1000];
    char char_name2[CHARNAME_MAX * 2 + 1];
    char text2[TEXT_MAX * 2 + 1];
    FILE *fp;

    snprintf(childname, sizeof(childname), "%u", mailbuf[mbindex].message_id);
    makeDirFilename(savefile, sizeof(savefile), maildir, h, childname);
    fp = fopen(savefile, "w");
    if (fp == NULL) {
      log("回复邮件 : 不能保存邮件文件: %s %s\n", savefile, strerror(errno));
      return;
    }
#define TO_ID_HEAD "ToID: "
#define TO_CHAR_HEAD "ToChar: "
#define FROM_ID_HEAD "FromID: "
#define FROM_CHAR_HEAD "FromChar: "
#define OPTION_HEAD "Option: "
#define TEXT_HEAD "Text: "

    fprintf(fp, TO_ID_HEAD "%s\n", id_to);
    snprintf(char_name2, sizeof(char_name2), "%s", char_name_to);
    fprintf(fp, TO_CHAR_HEAD "%s\n",
            makeEscapeString(char_name_to, char_name2, sizeof(char_name2)));
    fprintf(fp, FROM_ID_HEAD "%s\n", id_from);
    snprintf(char_name2, sizeof(char_name2), "%s", char_name_from);
    fprintf(fp, FROM_CHAR_HEAD "%s\n",
            makeEscapeString(char_name_from, char_name2, sizeof(char_name2)));
    fprintf(fp, OPTION_HEAD "%d\n", option);
    snprintf(text2, sizeof(text2), "%s", message);
    fprintf(fp, TEXT_HEAD "%s\n",
            makeEscapeString(message, text2, sizeof(text2)));
    fclose(fp);
  }
  {
    extern gmsv gs[MAXCONNECTION];
    int i;
    for (i = 0; i < MAXCONNECTION; i++) {
      if (gs[i].use && gs[i].name[0]) {
        saacproto_Message_send(i, id_from, char_name_from, id_to, char_name_to,
                               message, option, mailbuf[mbindex].message_id);
        mailbuf[mbindex].state = MS_WAIT_ACK;
      }
    }
  }
}

void receiveMailAck(const char *id, const char *char_name, int a, const unsigned msg_id) {
  int i;
  unsigned int h;
  char id_char_name[1024];
  snprintf(id_char_name, sizeof(id_char_name), "%s_%s", id, char_name);
  h = hashpjw(id_char_name) & 0xff;
  for (i = 0; i < mailbufsize; i++) {
    if (mailbuf[i].message_id == msg_id) {
      if (mailbuf[i].use && mailbuf[i].id_char_name_hash == h &&
          strcmp(mailbuf[i].id_to, id) == 0 &&
          strcmp(mailbuf[i].char_name_to, char_name) == 0 &&
          mailbuf[i].state == MS_WAIT_ACK) {
        char savefile[1024];
        char childname[1000];
        snprintf(childname, sizeof(childname), "%u", mailbuf[i].message_id);
        makeDirFilename(savefile, sizeof(savefile), maildir, h, childname);
        if (unlink(savefile) != 0) {
          log("failed to unlink %s: %s\n", savefile, strerror(errno));
        } else {
          log("回复邮件: 删除邮件 "
              "%u 从 %s(%s) to %s(%s)\n",
              mailbuf[i].message_id, mailbuf[i].id_from,
              mailbuf[i].char_name_from, mailbuf[i].id_to,
              mailbuf[i].char_name_to);
        }
        memset(&mailbuf[i], 0, sizeof(mailbuf[0]));
        return;
      } else {
        log("回复邮件: 无用信息或ID或名称或声明严重错误");
        log("use[%d] h[%d][%d] id[%s][%s] nm[%s][%s] st[%d]\n", mailbuf[i].use,
            mailbuf[i].id_char_name_hash, h, mailbuf[i].id_to, id,
            mailbuf[i].char_name_to, char_name, mailbuf[i].state);
      }
    }
  }
  log("回复邮件: 邮件 %u 不能从 %s(%s) 找到\n", msg_id, id, char_name);
}

void flushMail(const int fd, const char *id, const char *char_name) {
  int i, c = 0;
  unsigned int h;
  char id_char_name[1000];
#define MAX_FLUSH_MAIL 1024
  int flush_index[MAX_FLUSH_MAIL];
  int flush_i = 0;
  snprintf(id_char_name, sizeof(id_char_name), "%s_%s", id, char_name);
  h = hashpjw(id_char_name);
  // Nuke +1
  log("邮件缓冲大小:%d (%s)\n", mailbufsize, chartime());
  // Nuke *1
  for (i = 0; (i < mailbufsize) && (i < MAX_FLUSH_MAIL); i++) {
    if (mailbuf[i].id_char_name_hash == h && mailbuf[i].use &&
        strcmp(mailbuf[i].id_to, id) == 0 &&
        strcmp(mailbuf[i].char_name_to, char_name) == 0) {
      log("消息ID:%u\n", mailbuf[i].message_id);
      flush_index[flush_i++] = i;
      c++;
    }
  }

  for (i = 0; i < flush_i; i++) {
    int j;
    for (j = 0; j < flush_i - 1; j++) {
      if (mailbuf[flush_index[j]].message_id >
          mailbuf[flush_index[j + 1]].message_id) {
        int sw = flush_index[j];
        flush_index[j] = flush_index[j + 1];
        flush_index[j + 1] = sw;
        log("inverted %d and %d in %d \n", flush_index[j], flush_index[j + 1],
            j);
      }
    }
  }
  for (i = 0; i < flush_i; i++) {
    saacproto_Message_send(
        fd, mailbuf[flush_index[i]].id_from,
        mailbuf[flush_index[i]].char_name_from, mailbuf[flush_index[i]].id_to,
        mailbuf[flush_index[i]].char_name_to, mailbuf[flush_index[i]].text,
        mailbuf[flush_index[i]].option, mailbuf[flush_index[i]].message_id);
    mailbuf[flush_index[i]].state = MS_WAIT_ACK;
    log("分类邮件ID:%u\n", mailbuf[flush_index[i]].message_id);
  }
  // Nuke *1
  log("邮件: 发送 %d 封邮件到 %s(%s)(%s)\n", c, id, char_name, chartime());
}

// Nuke start: To expire undelivered mail
#define MAIL_EXPIRE_TIME 3600
#define MAX_FLUSH_MAIL 1024
void expireMail() {
  int i, c = 0;
  unsigned int h;
  char id_char_name[1000];
  time_t now = time(NULL);
  int flush_index[MAX_FLUSH_MAIL];
  int flush_i = 0;
  log("邮件缓冲大小:%d (%s)\n", mailbufsize, chartime());
  // Nuke *1
  for (i = 0; (i < mailbufsize) && (i < MAX_FLUSH_MAIL); i++) {
    if (mailbuf[i].use && (now - mailbuf[i].recv_time >= MAIL_EXPIRE_TIME)) {
      log("消息ID:%u 已过期\n", mailbuf[i].message_id);
      flush_index[flush_i++] = i;
      c++;
    }
  }

  for (i = 0; i < flush_i; i++) {

    snprintf(id_char_name, sizeof(id_char_name), "%s_%s",
             mailbuf[flush_index[i]].id_to,
             mailbuf[flush_index[i]].char_name_to);
    h = hashpjw(id_char_name) & 0xff;
    {
      char savefile[1024];
      char childname[1000];
      snprintf(childname, sizeof(childname), "%u",
               mailbuf[flush_index[i]].message_id);
      makeDirFilename(savefile, sizeof(savefile), maildir, h, childname);
      if (unlink(savefile) != 0) {
        log("failed to unlink %s: %s\n", savefile, strerror(errno));
      } else {
        log("过期邮件: 删除游戏 "
            "%u 从 %s(%s) 到 %s(%s)\n",
            mailbuf[flush_index[i]].message_id, mailbuf[flush_index[i]].id_from,
            mailbuf[flush_index[i]].char_name_from,
            mailbuf[flush_index[i]].id_to, mailbuf[flush_index[i]].char_name_to);
      }
      memset(&mailbuf[flush_index[i]], 0, sizeof(mailbuf[0]));
    }
  }
  // Nuke *1
  log("过期邮件: 过期 %d 消息 (%s)\n", c, chartime());
}

int readMail(const char *dir) {
  int i, read_count = 0;
  for (i = 0; i < 256; i++) {
    char dirname[1000];
    DIR *d;

    snprintf(dirname, sizeof(dirname), "%s/0x%x", dir, i);
    d = opendir(dirname);
    if (d == NULL) {
      mkdir(dirname, 0755);
      log("创建 %s\n", dirname);
      continue;
    }
    while (1) {
      struct dirent *de;
      de = readdir(d);
      if (de == NULL)
        break;
      if (de->d_name[0] != '.') {
        char filename[1000];
        FILE *fp;
        struct stat s;
        snprintf(filename, sizeof(filename), "%s/%s", dirname, de->d_name);
        if (stat(filename, &s) < 0) {
          continue;
        }
        if (!(s.st_mode & S_IFREG))
          continue;
        fp = fopen(filename, "r");
        if (fp == NULL) {
          log("不能打开文件 %s %s\n", filename, strerror(errno));
          continue;
        }
        {
          char line[16384];
          char toid[1000], fromid[1000];
          char tochar[CHARNAME_MAX * 2 + 1];
          char fromchar[CHARNAME_MAX * 2 + 1];
          char text[TEXT_MAX * 2 + 1];
          int opt = 0;
          toid[0] = fromid[0] = tochar[0] = fromchar[0] = text[0] = 0;
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(TO_ID_HEAD, line, strlen(TO_ID_HEAD)) == 0) {
            snprintf(toid, sizeof(toid), "%s", line + strlen(TO_ID_HEAD));
          }
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(TO_CHAR_HEAD, line, strlen(TO_CHAR_HEAD)) == 0) {
            snprintf(tochar, sizeof(tochar), "%s", line + strlen(TO_CHAR_HEAD));
            makeStringFromEscaped(tochar);
          }
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(FROM_ID_HEAD, line, strlen(FROM_ID_HEAD)) == 0) {
            snprintf(fromid, sizeof(fromid), "%s", line + strlen(FROM_ID_HEAD));
          }
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(FROM_CHAR_HEAD, line, strlen(FROM_CHAR_HEAD)) == 0) {
            snprintf(fromchar, sizeof(fromchar), line + strlen(FROM_CHAR_HEAD));
            makeStringFromEscaped(fromchar);
          }
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(OPTION_HEAD, line, strlen(OPTION_HEAD)) == 0) {
            opt = atoi(line + strlen(OPTION_HEAD));
          }
          fgets(line, sizeof(line), fp);
          chop(line);
          if (strncmp(TEXT_HEAD, line, strlen(TEXT_HEAD)) == 0) {
            snprintf(text, sizeof(text), "%s", line + strlen(TEXT_HEAD));
            makeStringFromEscaped(text);
          }
          if (toid[0] == 0 || fromid[0] == 0 || tochar[0] == 0 ||
              fromchar[0] == 0 || text[0] == 0) {
            log("有问题邮件! %s 接收ID[%c] 接收名字[%c]"
                " 发送ID[%c] 发送名字[%c] 文本[%c]\n",
                filename, toid[0], tochar[0], fromid[0], fromchar[0], text[0]);
            // Nuke +1 1027: Close for safe
            fclose(fp);
            continue;
          }
          receiveMail(fromid, fromchar, toid, tochar, text, opt, 1,
                      strtoul(de->d_name, NULL, 10));
          read_count++;
        }
        fclose(fp);
      }
    }
    closedir(d);
  }
  log("读取邮件: 在'%s'目录里读取到 %d 封邮件 \n", dir, read_count);
  return 0;
}
