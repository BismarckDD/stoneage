/* Combined with the actual validator definitions by test-check-string-err.ps1. */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define print(...) ((void)0)

BOOL checkStringErr(char *text);

int main(void) {
  const char *valid[] = {
      "", "Pet123_", "|/\\", /* Preserve existing ASCII policy. */
      "\xe4\xbd\xa9\xe9\x9c\xb2\xe5\xa4\x8f", /* Peiluxia */
      "\xe5\xa5\x87\xe9\xad\x94\xe7\x8c\xab", /* Qimomao */
      "\xe7\x99\xbd\xe8\x99\x8e", /* White tiger */
      "A\xe4\xbd\xa9" "9", "\xc2\x80", "\xdf\xbf",
      "\xe0\xa0\x80", "\xed\x9f\xbf", "\xef\xbf\xbf",
      "\xf0\x90\x80\x80", "\xf4\x8f\xbf\xbf"};
  const char *invalid[] = {
      NULL, " ", "A B", "\t", "\r", "\n", "\x01", "\x7f",
      "a'b", "a;b", "\x80", "\xff", "\xfe", "\xc0\xaf",
      "\xc1\xbf", "\xe0\x80\xaf", "\xed\xa0\x80",
      "\xf0\x80\x80\xaf", "\xf4\x90\x80\x80",
      "\xf5\x80\x80\x80", "\xc2", "\xe4", "\xe4\xbd",
      "\xf0\x90\x80", "\xe4" "A", "\xe4\xbd" "A",
      "\xe4\xbd\xa9;", "\xe4\xbd\xa9\x80"};
  size_t i;
  for (i = 0; i < sizeof(valid) / sizeof(valid[0]); ++i)
    assert(checkStringErr((char *)valid[i]) == FALSE);
  for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
    assert(checkStringErr((char *)invalid[i]) == TRUE);
  puts("checkStringErr: 42 cases passed");
  return 0;
}
