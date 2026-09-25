#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0
char *strncpysafe(char *, int, const char *);
char *strncpysafe2(char *, int, const char *, const char *);
BOOL getDelimitedField(const char *, const char *, int, char *, int);
BOOL nextDelimitedField(const char **, const char *, char *, int);

/* Reference the old positive-index behavior, including the output on failure. */
static BOOL legacyField(const char *src, const char *delim, int index,
                        char *buf, int size) {
  const char *end = NULL;
  size_t step = strlen(delim);
  for (int i = 0; i < index; ++i) {
    end = strstr(src, delim);
    if (!end) {
      strncpysafe(buf, size, src);
      return i == index - 1;
    }
    if (i + 1 < index)
      src = end + step;
  }
  strncpysafe2(buf, size, src, end);
  return TRUE;
}

int main(void) {
  const char *delims[] = {"|", "||", "ab", "", "OVER"};
  unsigned cases = 0;
  /* Exhaustively exercise short strings with overlapping delimiters. */
  for (unsigned code = 0; code < 4096; ++code) {
    char src[7];
    unsigned value = code;
    for (int i = 0; i < 6; ++i) {
      src[i] = "a|b\0"[value % 4];
      value /= 4;
    }
    src[6] = '\0';
    for (unsigned d = 0; d < sizeof(delims) / sizeof(delims[0]); ++d) {
      for (int size = 1; size <= 8; ++size) {
        const char *cursor = src;
        for (int index = 1; index <= 9; ++index) {
          char old[9], current[9], sequential[9];
          memset(old, 0x5a, sizeof(old));
          memset(current, 0x5a, sizeof(current));
          BOOL expected = legacyField(src, delims[d], index, old, size);
          assert(getDelimitedField(src, delims[d], index, current, size) == expected);
          assert(memcmp(old, current, sizeof(old)) == 0);
          if (*delims[d]) {
            memset(sequential, 0x5a, sizeof(sequential));
            assert(nextDelimitedField(&cursor, delims[d], sequential, size) == expected);
            if (expected)
              assert(memcmp(old, sequential, sizeof(old)) == 0);
            else
              assert(cursor == NULL && sequential[0] == 0x5a);
          }
          ++cases;
        }
      }
    }
  }
  char buf[16] = "unchanged";
  const char *cursor = "abc";
  assert(!getDelimitedField(NULL, "|", 1, buf, sizeof(buf)));
  assert(!getDelimitedField("abc", NULL, 1, buf, sizeof(buf)));
  assert(!getDelimitedField("abc", "|", 0, buf, sizeof(buf)));
  assert(!getDelimitedField("abc", "|", -1, buf, sizeof(buf)));
  assert(!getDelimitedField("abc", "|", 1, NULL, sizeof(buf)));
  assert(!getDelimitedField("abc", "|", 1, buf, 0));
  assert(!nextDelimitedField(&cursor, "", buf, sizeof(buf)));
  assert(!nextDelimitedField(NULL, "|", buf, sizeof(buf)));
  assert(!nextDelimitedField(&cursor, NULL, buf, sizeof(buf)));
  assert(!nextDelimitedField(&cursor, "|", NULL, sizeof(buf)));
  assert(!nextDelimitedField(&cursor, "|", buf, 0));
  assert(strcmp(buf, "unchanged") == 0 && strcmp(cursor, "abc") == 0);
  printf("Passed %u compatibility cases and invalid-argument checks.\n", cases);
  return 0;
}
