#include "tcp_buffer.h"
#include "tcp_transport.h"

#include <assert.h>
#include <string.h>

int main(void) {
  SaTcpPollItem item = {0};
  char storage[32] = {0};
  char line[16];
  int used = 0;

  item.readable = 1;
  item.writable = 1;
  assert(sa_tcp_take_readable(&item) == 1);
  assert(sa_tcp_take_readable(&item) == 0);
  assert(sa_tcp_take_writable(&item) == 1);
  assert(sa_tcp_take_writable(&item) == 0);

  assert(sa_tcp_buffer_append(storage, &used, sizeof(storage), "one\r\ntwo", 8) == 8);
  assert(sa_tcp_buffer_read_line(storage, &used, line, sizeof(line), 0) == 3);
  assert(strcmp(line, "one") == 0);
  assert(used == 3);
  assert(sa_tcp_buffer_read_line(storage, &used, line, sizeof(line), 1) == 0);
  assert(sa_tcp_buffer_append(storage, &used, sizeof(storage), "\n", 1) == 1);
  assert(sa_tcp_buffer_read_line(storage, &used, line, sizeof(line), 1) == 4);
  assert(strcmp(line, "two\n") == 0);
  assert(used == 0);
  assert(sa_tcp_buffer_append(storage, &used, sizeof(storage), storage, 33) == -1);
  return 0;
}
