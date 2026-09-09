#include "common.h"
#include "tcp_transport.h"

int main(void) {
  struct sockaddr_in address;
  int fd;

  if (sa_platform_init() != 0)
    return 1;
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return 2;
  if (sa_tcp_set_reuseaddr(fd) < 0 || sa_tcp_set_nonblocking(fd) < 0) {
    sa_tcp_close(fd);
    return 3;
  }

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = 0;
  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    sa_tcp_close(fd);
    return 4;
  }
  if (listen(fd, 1) < 0) {
    sa_tcp_close(fd);
    return 5;
  }
  return sa_tcp_close(fd) == 0 ? 0 : 6;
}
