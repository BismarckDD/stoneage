#include "tcp_transport.h"

int sa_tcp_poll(SaTcpPollItem *items, int count, int timeout_ms) {
  fd_set rfds, wfds, efds;
  struct timeval timeout;
  int i, maxfd = -1, result;

  if (items == NULL || count < 0)
    return -1;

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);
  for (i = 0; i < count; i++) {
    items[i].readable = 0;
    items[i].writable = 0;
    items[i].error = 0;
    if (items[i].fd < 0)
      continue;
    if (items[i].want_read)
      FD_SET(items[i].fd, &rfds);
    if (items[i].want_write)
      FD_SET(items[i].fd, &wfds);
    FD_SET(items[i].fd, &efds);
    if (items[i].fd > maxfd)
      maxfd = items[i].fd;
  }

  if (maxfd < 0)
    return 0;
  if (timeout_ms < 0)
    timeout_ms = 0;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;
  result = select(maxfd + 1, &rfds, &wfds, &efds, &timeout);
  if (result <= 0)
    return result;

  for (i = 0; i < count; i++) {
    if (items[i].fd < 0)
      continue;
    items[i].readable = FD_ISSET(items[i].fd, &rfds) ? 1 : 0;
    items[i].writable = FD_ISSET(items[i].fd, &wfds) ? 1 : 0;
    items[i].error = FD_ISSET(items[i].fd, &efds) ? 1 : 0;
  }
  return result;
}

int sa_tcp_set_nonblocking(int fd) {
#ifdef _WIN32
  u_long enabled = 1;
  return ioctlsocket(fd, FIONBIO, &enabled);
#else
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0)
    return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

int sa_tcp_set_nodelay(int fd) {
  int enabled = 1;
  return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&enabled,
                    sizeof(enabled));
}

int sa_tcp_set_reuseaddr(int fd) {
  int enabled = 1;
  return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&enabled,
                    sizeof(enabled));
}

int sa_tcp_configure_connected(int fd) {
  if (fd < 0 || sa_tcp_set_nonblocking(fd) < 0)
    return -1;
  if (sa_tcp_set_nodelay(fd) < 0)
    return -1;
  return 0;
}

int sa_tcp_read(int fd, void *buffer, int length) {
#ifdef _WIN32
  return recv(fd, (char *)buffer, length, 0);
#else
  return (int)read(fd, buffer, (size_t)length);
#endif
}

int sa_tcp_write(int fd, const void *buffer, int length) {
#ifdef _WIN32
  return send(fd, (const char *)buffer, length, 0);
#else
  return (int)write(fd, buffer, (size_t)length);
#endif
}

int sa_tcp_close(int fd) {
#ifdef _WIN32
  return closesocket(fd);
#else
  return close(fd);
#endif
}

int sa_tcp_error_is_interrupted(void) {
#ifdef _WIN32
  return WSAGetLastError() == WSAEINTR;
#else
  return errno == EINTR;
#endif
}

int sa_tcp_error_is_would_block(void) {
#ifdef _WIN32
  int error = WSAGetLastError();
  return error == WSAEWOULDBLOCK;
#else
  return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}
