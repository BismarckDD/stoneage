#include "db.h"

#include <stdio.h>
#include <string.h>

int main(void) {
  int value = 0;
  char text[256];

  if (dbUpdateEntryInt("duelpoint", "aaa", 10, "first") != 0 ||
      dbGetEntryInt("duelpoint", "aaa", &value) != 0 || value != 10)
    return 1;
  if (dbUpdateEntryInt("duelpoint", "bbb", 20, "second") != 0 ||
      dbUpdateEntryInt("duelpoint", "aaa", 30, "updated") != 0 ||
      dbGetEntryInt("duelpoint", "aaa", &value) != 0 || value != 30)
    return 2;
  if (dbUpdateEntryString("string-data", "aaa", "payload") != 0 ||
      dbGetEntryString("string-data", "aaa", text, sizeof(text)) != 0 ||
      strcmp(text, "payload") != 0)
    return 3;

  puts("SAAC database first-allocation and update test passed");
  return 0;
}
