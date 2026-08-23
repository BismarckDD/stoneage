#define __TCP_TOOL_C__

#include "tcp_struct.h"
#include "main.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

// Forward declaration (defined in main.c)
extern int releaseMemBuf(const int index);

int tcpstruct_init(char *addr, int p, int timeout_ms, int mem_use, int db) {

  // 初始化 g_mem_buffer
  g_mem_buffer_size = mem_use / sizeof(MemBuffer);
  g_mem_buffer_used = 0;
  g_mem_buffer_finder = 0;
  g_mem_buffer = (MemBuffer *)calloc(1, g_mem_buffer_size * sizeof(MemBuffer));
  if (g_mem_buffer == NULL)
    return TCPSTRUCT_ENOMEM;
  memset(g_mem_buffer, 0, g_mem_buffer_size * sizeof(MemBuffer));
  // 初始化 g_con, 和每个用户的连接
  g_mem_buffer_size = mem_use / sizeof(MemBuffer);
  g_con = (Connection *)calloc(1, MAXCONNECTION * sizeof(Connection));
  if (g_con == NULL) {
    free(g_mem_buffer);
    return TCPSTRUCT_ENOMEM;
  }
  int i;
  for (i = 0; i < MAXCONNECTION; i++) {
    g_con[i].use = 0;
    g_con[i].fd = -1;
  }
  select_timeout.tv_sec = timeout_ms / 1000;
  select_timeout.tv_usec = (timeout_ms - (timeout_ms / 1000) * 1000) * 1000;

  /* socket */
  g_main_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (g_main_sock_fd < 0)
    return TCPSTRUCT_ESOCK;

  /* bind */
  memset(&g_local_addr, 0, sizeof(g_local_addr));
  g_local_addr.sin_family = AF_INET;
  g_local_addr.sin_port = htons(p);

  if (addr) {
    g_local_addr.sin_addr.s_addr = inet_addr(addr);
  } else {
    g_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  if (bind(g_main_sock_fd, (struct sockaddr *)&g_local_addr, sizeof(g_local_addr)) < 0)
    return TCPSTRUCT_EBIND;

  /* listen */
  if (listen(g_main_sock_fd, BACKLOGNUM) < 0)
    return TCPSTRUCT_ELISTEN;

  return TCPSTRUCT_OK;
}

int tcpstruct_accept1(void) {
  int tis[BACKLOGNUM];
  int ret = tcpstruct_accept(tis, 1);
  if (ret < 0) {
    return ret;
  } else if (ret == 1) {
    return tis[0];
  } else {
    return TCPSTRUCT_EBUG;
  }
}

int tcpstruct_accept(int *tis, int ticount) {
  int i, k, num = 0;
  int sret = 0;
  int accepted = 0;
  struct timeval t;
  fd_set rfds, wfds, efds;
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);

  for (i = 0; i < MAXCONNECTION; i++) {
    if (g_con[i].use && g_con[i].fd >= 0 && g_con[i].closed_by_remote == 0) {
      FD_SET(g_con[i].fd, &rfds);
      FD_SET(g_con[i].fd, &wfds);
      FD_SET(g_con[i].fd, &efds);

      int j = 1, k;

      t = select_timeout;
      sret = select(g_con[i].fd + 1, &rfds, (fd_set *)NULL, &efds, &t);
      if (sret > 0) {
        if ((g_con[i].fd >= 0) && FD_ISSET(g_con[i].fd, &rfds)) {
          int fr = getFreeMem();
          int rr, readsize;
          if (fr <= 0) {
            fprintf(stderr, "连接%d内存不足, 标记远程关闭(fd=%d)\n", i, g_con[i].fd);
            g_con[i].closed_by_remote = 1;
          } else {
            memset(g_temp_buffer, 0, sizeof(g_temp_buffer));
            if (fr > sizeof(g_temp_buffer)) {
              readsize = sizeof(g_temp_buffer);
            } else {
              readsize = fr - 1;
            }
            rr = read(g_con[i].fd, g_temp_buffer, readsize);
            if (rr <= 0) {
              fprintf(stderr, "连接%d读取返回%d, 标记远程关闭(fd=%d)\n", i, rr, g_con[i].fd);
              g_con[i].closed_by_remote = 1;
            } else {
              appendReadBuffer(i, g_temp_buffer, rr);
            }
          }
        }
      }

      if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) > 0.50) {
        j = 2;
      } else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) >
                 0.40) {
        j = 3;
      } else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) >
                 0.30) {
        j = 4;
      } else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) >
                 0.20) {
        j = 5;
      }

      for (k = 0; k < j; k++) {
        t = select_timeout;
        sret = select(g_con[i].fd + 1, (fd_set *)NULL, &wfds, (fd_set *)NULL, &t);
        if (sret > 0) {
          if ((g_con[i].fd >= 0) && FD_ISSET(g_con[i].fd, &wfds)) {
            char send_buf[4096];
            memset(send_buf, 0, sizeof(send_buf));
            int l = consumeMemBufList(g_con[i].mbtop_wi, send_buf,
                                      sizeof(send_buf), 0, 1);
            if (l > 0) {
              int rr = write(g_con[i].fd, send_buf, l);
              if (rr < 0) {
                g_con[i].closed_by_remote = 1;
              } else {
#ifdef _DEBUG
                printf("发送内容:%s\n", send_buf);
#endif
                consumeMemBufList(g_con[i].mbtop_wi, send_buf, l, 1, 0);
              }
            }
          }
        }
      }
    }
  }

  for (i = 0; i < ticount; i++) {
    int asret;
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    FD_SET(g_main_sock_fd, &rfds);
    FD_SET(g_main_sock_fd, &wfds);
    FD_SET(g_main_sock_fd, &efds);
    asret = select(g_main_sock_fd + 1, &rfds, &wfds, &efds, &t);
    // Nuke 20040610: add asret>0 to avoid signal interrupt in select
    if ((asret > 0) && FD_ISSET(g_main_sock_fd, &rfds)) {
      struct sockaddr_in c;
      int len, newsockfd;
      int newcon;
      memset(&c, 0, sizeof(c));
      len = sizeof(c);
      fprintf(stderr, "i can accept ");
      newcon = findregBlankCon();
      if (newcon < 0)
        continue;
      newsockfd = accept(g_main_sock_fd, (struct sockaddr *)&c, &len);
      if (newsockfd < 0) {
        releaseMemBuf(newcon);
        continue;
      }
      set_nodelay(newsockfd);
      g_con[newcon].fd = newsockfd;
      memcpy(&g_con[newcon].remoteaddr, &c, sizeof(c));
      tis[accepted] = newcon;
      accepted++;
    }
  }
  return accepted;
}

int tcpstruct_close(int ti) {

  if (ti < 0 || ti >= MAXCONNECTION)
    return TCPSTRUCT_EINVCIND;
  if (g_con[ti].use == 0) {
    return TCPSTRUCT_ECLOSEAGAIN;
  }
  close(g_con[ti].fd);
  g_con[ti].use = 0;
  g_con[ti].fd = -1;

  consumeMemBufList(g_con[ti].mbtop_ri, NULL,
                    g_mem_buffer_size * sizeof(g_mem_buffer[0].buf), 1, 0);
  consumeMemBufList(g_con[ti].mbtop_wi, NULL,
                    g_mem_buffer_size * sizeof(g_mem_buffer[0].buf), 1, 0);

  releaseMemBuf(g_con[ti].mbtop_ri);
  releaseMemBuf(g_con[ti].mbtop_wi);
  g_con[ti].mbtop_ri = -1;
  g_con[ti].mbtop_wi = -1;
  return TCPSTRUCT_OK;
}

int tcpstruct_read(const int ti, char *buf, const int len) {
  if (ti < 0 || ti >= MAXCONNECTION || g_con[ti].use == 0)
    return TCPSTRUCT_EINVCIND;
  int l = consumeMemBufList(g_con[ti].mbtop_ri, buf, len, 1, 1);
  if (l == 0 && g_con[ti].closed_by_remote)
    return TCPSTRUCT_EREADFIN;
  return l;
}

int tcpstruct_readline(const int ti,  // Connection fd.
                       char *buf,     // read_buf
                       const int len, // read_buf_len
                       const int kend,
                       const int kend_r) {
  int l;
  int minus = 0;  // 最终要剥离几个字符
  if (ti < 0 || ti >= MAXCONNECTION || g_con[ti].use == 0)
    return TCPSTRUCT_EINVCIND;
  l = getLineReadBuffer(ti, buf, len);
  if (l == 0) {
    if (g_con[ti].closed_by_remote) {
      return TCPSTRUCT_EREADFIN;
    } else {
      return 0;
    }
  }

  if (kend) {
    if (l >= 1 && buf[l - 1] == '\n') {
      buf[l - 1] = 0;
      minus = -1;
    }
  }
  if (kend_r) {
    if (l + minus >= 1 && buf[l + minus - 1] == '\r') {
      buf[l + minus - 1] = 0;
      minus -= 1;
    }
  }
  return l + minus;
}

int tcpstruct_readline_chop(const int ti, char *buf, const int len) {
  return tcpstruct_readline(ti, buf, len, 1, 1);
}

int tcpstruct_write(const int ti, const char *buf, const int len) {
  if (ti < 0 || ti >= MAXCONNECTION || g_con[ti].use == 0)
    return TCPSTRUCT_EINVCIND;
  return appendWriteBuffer(ti, buf, len);
}

int tcpstruct_connect(const char *addr, const int port) {
  int newti;
  int s, r;
  struct sockaddr_in svaddr;
  struct hostent *he;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    return -2;

  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sin_family = AF_INET;
  svaddr.sin_port = htons(port);

  if (inet_aton(addr, &svaddr.sin_addr) == 0) {
    he = gethostbyname(addr);
    if (he == NULL) {
      return TCPSTRUCT_EHOST;
    }
    memcpy(&svaddr.sin_addr.s_addr, he->h_addr, sizeof(struct in_addr));
  }
  r = connect(s, (struct sockaddr *)&svaddr, sizeof(svaddr));
  if (r < 0) {
    return TCPSTRUCT_ECONNECT;
  }
  set_nodelay(s);
  newti = findregBlankCon();
  if (newti < 0) {
    fprintf(stderr, "连接失败: newti:%d\n", newti);
    return TCPSTRUCT_ECFULL;
  }
  g_con[newti].fd = s;
  memcpy(&g_con[newti].remoteaddr, &svaddr, sizeof(struct sockaddr_in));
  return newti;
}

int tcpstruct_countmbuse(void) {
  int i, c = 0;
  for (i = 0; i < g_mem_buffer_size; i++) {
    if (g_mem_buffer[i].use)
      c++;
  }
  return c;
}
