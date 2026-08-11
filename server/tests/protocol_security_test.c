#include "server_util.h"

#include <assert.h>

WorkSpace *ws;

static int discard_write(int fd, char *data, int len) {
  (void)fd;
  (void)data;
  return len;
}

int main(void) {
  WorkSpace workspace;
  char normal[] = "1 ACServerLogin server password";
  char oversized[256];
  size_t offset = 0;
  int i;

  memset(&workspace, 0, sizeof(workspace));
  assert(InitWorkSpace(&workspace, discard_write, 4096, 16) == 0);
  ws = &workspace;

  assert(SplitString(normal, ws) == 4);
  assert(strcmp(ws->token_list[0], "1") == 0);
  assert(strcmp(ws->token_list[1], "ACServerLogin") == 0);
  assert(strcmp(ws->token_list[3], "password") == 0);

  for (i = 0; i < WORKSPACE_TOKEN_CAPACITY + 1; ++i) {
    const int written = snprintf(oversized + offset, sizeof(oversized) - offset,
                                 "%s%d", i == 0 ? "" : " ", i);
    assert(written > 0);
    offset += (size_t)written;
    assert(offset < sizeof(oversized));
  }
  assert(SplitString(oversized, ws) == -1);
  assert(ws->token_list[0] == NULL);

  assert(strcmp(mkstr_int(42), "42 ") == 0);
  assert(strcmp(demkstr_string("account"), "account") == 0);

  FreeWorkSpace(&workspace);
  return 0;
}
