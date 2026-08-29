#define __TCP_TOOL_C__

#include "tcp_struct.h"
#include "main.h"
#include "tcp_transport.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

// Forward declaration (defined in main.c)
extern int releaseMemBuf(const int index);
static int mem_buffer_has_data(int top);

static void saac_session_attach(SaacSession *session, int fd,
                                const struct sockaddr_in *peer) {
  session->fd = fd;
  session->closed_by_remote = 0;
  if (peer != NULL)
    memcpy(&session->remoteaddr, peer, sizeof(*peer));
}

static void saac_session_mark_remote_closed(SaacSession *session) {
  session->closed_by_remote = 1;
}

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
  g_con = (SaacSession *)calloc(1, MAXCONNECTION * sizeof(SaacSession));
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
  sa_tcp_set_reuseaddr(g_main_sock_fd);
  if (sa_tcp_set_nonblocking(g_main_sock_fd) < 0)
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
  SaTcpPollItem items[MAXCONNECTION + 1];
  int i, k;
  int accepted = 0;
  int poll_result;

  memset(items, 0, sizeof(items));
  items[0].fd = g_main_sock_fd;
  items[0].want_read = 1;

  for (i = 0; i < MAXCONNECTION; i++) {
    items[i + 1].fd = -1;
    if (g_con[i].use && g_con[i].fd >= 0 && g_con[i].closed_by_remote == 0) {
      items[i + 1].fd = g_con[i].fd;
      items[i + 1].want_read = 1;
      items[i + 1].want_write = mem_buffer_has_data(g_con[i].mbtop_wi);
    }
  }

  poll_result = sa_tcp_poll(items, MAXCONNECTION + 1, 0);
  if (poll_result < 0)
    return sa_tcp_error_is_interrupted() ? 0 : TCPSTRUCT_EBUG;

  for (i = 0; i < MAXCONNECTION; i++) {
    SaTcpPollItem *item = &items[i + 1];
    int send_rounds = 1;
    if (item->fd < 0)
      continue;
    if (item->error) {
      saac_session_mark_remote_closed(&g_con[i]);
      continue;
    }
    if (item->readable) {
      int fr = getFreeMem();
      int rr, readsize;
      if (fr <= 1) {
        fprintf(stderr, "连接%d内存不足, 标记远程关闭(fd=%d)\n", i,
                g_con[i].fd);
        saac_session_mark_remote_closed(&g_con[i]);
      } else {
        readsize = fr > (int)sizeof(g_temp_buffer) ? (int)sizeof(g_temp_buffer)
                                                   : fr - 1;
        rr = sa_tcp_read(g_con[i].fd, g_temp_buffer, readsize);
        if (rr == 0 || (rr < 0 && !sa_tcp_error_is_would_block() &&
                       !sa_tcp_error_is_interrupted())) {
          saac_session_mark_remote_closed(&g_con[i]);
        } else if (rr > 0 && appendReadBuffer(i, g_temp_buffer, rr) < 0) {
          saac_session_mark_remote_closed(&g_con[i]);
        }
      }
    }
    if (!item->writable || g_con[i].closed_by_remote)
      continue;
    if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) > 0.50)
      send_rounds = 2;
    else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) > 0.40)
      send_rounds = 3;
    else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) > 0.30)
      send_rounds = 4;
    else if ((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION) > 0.20)
      send_rounds = 5;
    for (k = 0; k < send_rounds; k++) {
      char send_buf[4096];
      int length = consumeMemBufList(g_con[i].mbtop_wi, send_buf,
                                     sizeof(send_buf), 0, 1);
      int written;
      if (length <= 0)
        break;
      written = sa_tcp_write(g_con[i].fd, send_buf, length);
      if (written > 0) {
        consumeMemBufList(g_con[i].mbtop_wi, NULL, written, 1, 0);
      } else if (written < 0 && !sa_tcp_error_is_would_block() &&
                 !sa_tcp_error_is_interrupted()) {
        saac_session_mark_remote_closed(&g_con[i]);
        break;
      } else {
        break;
      }
    }
  }

  if (items[0].error)
    return TCPSTRUCT_ESOCK;
  if (items[0].readable) {
    for (i = 0; i < ticount; i++) {
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
        tcpstruct_close(newcon);
        break;
      }
      set_nodelay(newsockfd);
      sa_tcp_set_nonblocking(newsockfd);
      saac_session_attach(&g_con[newcon], newsockfd, &c);
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
  sa_tcp_close(g_con[ti].fd);
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

static int mem_buffer_has_data(int top) {
  int visited = 0;
  while (top >= 0 && top < g_mem_buffer_size && visited < g_mem_buffer_size) {
    if (g_mem_buffer[top].len > 0)
      return 1;
    top = g_mem_buffer[top].next;
    visited++;
  }
  return 0;
}

int tcpstruct_idle_wait(const int timeout_ms) {
  SaTcpPollItem items[MAXCONNECTION + 1];
  int i;

  if (timeout_ms <= 0)
    return 0;

  /* Buffered input must be dispatched before sleeping. */
  for (i = 0; i < MAXCONNECTION; i++) {
    if (!g_con[i].use)
      continue;
    // 读缓冲区有数据，就不再WAIT
    if (mem_buffer_has_data(g_con[i].mbtop_ri))
      return 0;
  }

  memset(items, 0, sizeof(items));
  items[0].fd = g_main_sock_fd;
  items[0].want_read = 1;
  for (i = 0; i < MAXCONNECTION; i++) {
    items[i + 1].fd = -1;
    if (!g_con[i].use || g_con[i].fd < 0 || g_con[i].closed_by_remote)
      continue;
    items[i + 1].fd = g_con[i].fd;
    items[i + 1].want_read = 1;
    /* A writable socket wakes immediately, so reconnect initialization data
     * is flushed at full speed. Backpressure still blocks without spinning. */
    if (mem_buffer_has_data(g_con[i].mbtop_wi))
      items[i + 1].want_write = 1;
  }
  return sa_tcp_poll(items, MAXCONNECTION + 1, timeout_ms);
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
      sa_tcp_close(s);
      return TCPSTRUCT_EHOST;
    }
    memcpy(&svaddr.sin_addr.s_addr, he->h_addr, sizeof(struct in_addr));
  }
  r = connect(s, (struct sockaddr *)&svaddr, sizeof(svaddr));
  if (r < 0) {
    sa_tcp_close(s);
    return TCPSTRUCT_ECONNECT;
  }
  set_nodelay(s);
  sa_tcp_set_nonblocking(s);
  newti = findregBlankCon();
  if (newti < 0) {
    fprintf(stderr, "连接失败: newti:%d\n", newti);
    sa_tcp_close(s);
    return TCPSTRUCT_ECFULL;
  }
  saac_session_attach(&g_con[newti], s, &svaddr);
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
