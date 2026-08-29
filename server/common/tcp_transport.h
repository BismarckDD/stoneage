#ifndef STONEAGE_COMMON_TCP_TRANSPORT_H
#define STONEAGE_COMMON_TCP_TRANSPORT_H

#include "common.h"

typedef struct SaTcpPollItem {
  int fd;
  unsigned char want_read;
  unsigned char want_write;
  unsigned char readable;
  unsigned char writable;
  unsigned char error;
} SaTcpPollItem;

/* Polls all supplied sockets as one readiness set. The result flags belong to
 * this call only and are always cleared, including on failure. */
int sa_tcp_poll(SaTcpPollItem *items, int count, int timeout_ms);

/* Readiness is a snapshot. Taking a flag clears it so priority scheduling
 * cannot issue a second blocking I/O operation from the same event. */
static inline int sa_tcp_take_readable(SaTcpPollItem *item) {
  int ready = item != NULL && item->readable;
  if (item != NULL)
    item->readable = 0;
  return ready;
}

static inline int sa_tcp_take_writable(SaTcpPollItem *item) {
  int ready = item != NULL && item->writable;
  if (item != NULL)
    item->writable = 0;
  return ready;
}

int sa_tcp_set_nonblocking(int fd);
int sa_tcp_set_nodelay(int fd);
int sa_tcp_set_reuseaddr(int fd);
int sa_tcp_configure_connected(int fd);
int sa_tcp_read(int fd, void *buffer, int length);
int sa_tcp_write(int fd, const void *buffer, int length);
int sa_tcp_close(int fd);
int sa_tcp_error_is_interrupted(void);
int sa_tcp_error_is_would_block(void);

#endif
