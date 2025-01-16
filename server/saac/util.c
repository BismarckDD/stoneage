#define __UTIL_C__

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include "main.h"
#include "util.h"

#define min(x, y)                                                              \
  ({                                                                           \
    typeof(x) __x = (x), __y = (y);                                            \
    (__x < __y) ? __x : __y;                                                   \
  })
#define max(x, y)                                                              \
  ({                                                                           \
    typeof(x) __x = (x), __y = (y);                                            \
    (__x < __y) ? __y : __x;                                                   \
  })
#define IS_2BYTEWORD(_a_) ((char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF))
#define PRIME 211

int hashpjw(const char *s) {
  char *p;
  unsigned int h = 0, g;
  for (p = s; *p; p++) {
    h = (h << 4) + (*p);
    if ((g = h & 0xF0000000) != 0) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % PRIME;
}

int getHash(const char *s) {
  int i;
  int h = 0;
  for (i = 0;; i++) {
    if (s[i] == 0) {
      break;
    }
    h += s[i];
  }
  return h;
}

void easyGetTokenFromString(char *src, int count, char *output, int len) {
  int i;
  int counter = 0;
  if (len <= 0)
    return;
#define ISSPACETAB(c) ((c) == ' ' || (c) == '\t')
  for (i = 0;; i++) {
    if (src[i] == '\0') {
      output[0] = '\0';
      return;
    }
    if (i > 0 && !ISSPACETAB(src[i - 1]) && !ISSPACETAB(src[i])) {
      continue;
    }
    if (!ISSPACETAB(src[i])) {
      counter++;
      if (counter == count) {
        int j;
        for (j = 0; j < len - 1; j++) {
          if (src[i + j] == '\0' || ISSPACETAB(src[i + j])) {
            break;
          }
          output[j] = src[i + j];
        }
        output[j] = '\0';
        return;
      }
    }
  }
}

void prepareDirectories(char *base) {
  int i;
  char dname[1024];

  for (i = 0; i < 256; i++) {
    int ret;
    snprintf(dname, sizeof(dname), "%s/0x%x", base, i);
    ret = mkdir(dname, 0755);
    if (ret < 0 && errno != EEXIST) {
      log("mkdir error:%d %s: %s\n", ret, strerror(errno), dname);
    }
    if (ret == 0)
      log(".");
  }
}

void makeDirFilename(char *out, const int out_len,
                     const char *base, int dir_char,
                     const char *child) {
  snprintf(out, out_len, "%s/0x%x/%s", base, dir_char & 0xFF, child);
}

int isFile(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return 0;
  } else {
    fclose(fp);
    return 1;
  }
}

int createFile(const char *filename, const char *line) {
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    return -1;
  } else {
    fprintf(fp, "%s", line);
    fclose(fp);
    return 0;
  }
}

#define BOOL int
#define FALSE 0
#define TRUE 1

typedef struct tagEscapeChar {
  char original_char;
  char escaped_char;
} EscapeChar;

static EscapeChar sEscapeCharMap[] = {
    {'\n', 'n'},
    {',', 'c'},
    {'|', 'z'},
    {'\\', 'y'},
};

// UTF-8编码下，没必要刻意处理2BYTEWORD.
char *makeStringFromEscaped(char *src) {
  int srclen = strlen(src);
  int ri/*read pointer*/, wi/*write pointer*/ = 0, j;
  int escape_len = arraysizeof(sEscapeCharMap);
  for (ri = 0; ri < srclen; ++ri) {
    if (src[ri] == '\\') {
      ++ri;
      for (j = 0; j < escape_len; ++j) {
        if (sEscapeCharMap[j].escaped_char == src[ri]) {
          src[wi++] = sEscapeCharMap[j].original_char;
          break;
        }
      }
      if (j == escape_len)
        src[wi++] = src[ri];
    } else {
      src[wi++] = src[ri];
    }
  }
  src[wi] = '\0';
  return src;
}

char *makeEscapeString(const char *src, char *dst, const int dst_len) {
  int srclen = strlen(src);
  int ri, wi = 0, j;
  BOOL is_escape = FALSE;
  for (ri = 0; ri < srclen; ++ri) {
    is_escape = FALSE;
    char escaped_char = '\0';
    if (wi + 1 >= dst_len)
      break;
    // 确定src[ri]是否需要转义，以及对应的转义字符.
    for (j = 0; j < arraysizeof(sEscapeCharMap); ++j)
      if (src[ri] == sEscapeCharMap[j].original_char) {
        is_escape = TRUE;
        escaped_char = sEscapeCharMap[j].escaped_char;
        break;
      }
    if (is_escape == TRUE) {
      if (wi + 2 < dst_len) {
        dst[wi++] = '\\';
        dst[wi++] = escaped_char;
        continue;
      } else {
        break;
      }
    } else {
      dst[wi++] = src[ri];
    }
  }
  dst[wi] = '\0';
  return dst;
}

#undef BOOL
#undef FALSE
#undef TRUE

int strcatsafe(char *dst, const int max_len, const char *app) {
  int dst_len = strlen(dst);
  int app_len = strlen(app);
  if ((dst_len + app_len) >= max_len) {
    return -1;
  } else {
    strcat(dst, app);
  }
  return 0;
}

char *chop(char *s) {
  int l = strlen(s);
  if (l >= 1) {
    s[l - 1] = 0;
  }
  return s;
}

// 从src向dst拷贝字节，至多拷贝copy_bytes个字节;
char *strncpy2(char *dst, const char *src, const int copy_bytes) {
  if (copy_bytes > 0) {
    int i;
    for (i = 0; i < copy_bytes; i++) {
      if (*(dst + i) == 0) {
        *(dst + i) = '\0';
        break;
      }
      *(dst + i) = *(src + i);
      if (*(src + i) & 0x80) { // 现在都是UTF8编码, 双字节检测似乎没有意义.
        i++;
        if (i >= copy_bytes) {
          *(dst + i - 1) = '\0';
          break;
        } else {
          *(dst + i) = *(src + i);
        }
      }
    }
  }
  return dst;
}

void strncpysafe(char *dst, const int n, const char *src, const int length) {
  int len = min(strlen(src), length);
  if (n < len + 1) {
    strncpy2(dst, src, n - 1);
    dst[n - 1] = '\0';
  } else if (n <= 0) {
    return;
  } else {
    strncpy2(dst, src, len);
    dst[len] = '\0';
  }
}

void strcpysafe(char *dst, size_t n, const char *src) {
  if (!src) {
    *dst = '\0';
    return;
  }
  if (n <= 0)
    return;
  else if (n < strlen(src) + 1) {
    strncpy2(dst, src, n - 1);
    dst[n - 1] = '\0';
  } else
    strcpy(dst, src);
}

char *ScanOneByte(char *src, char delim) {
  if (!src)
    return NULL;
  for (; src[0] != '\0'; src++) {
    if (IS_2BYTEWORD(src[0])) {
      if (src[1] != 0) {
        src++;
      }
      continue;
    }
    if (src[0] == delim) {
      return src;
    }
  }
  return NULL;
}

int easyGetTokenFromBuf(char *src, char *delim, int count, char *output,
                        int len) {
  int i;
  int length = 0;
  int addlen = 0;
  int oneByteMode = 0;

  if (strlen(src) == 0)
    return 0;

  if (strlen(delim) == 1) {
    oneByteMode = 1;
  }
  for (i = 0; i < count; i++) {
    char *last;
    src += addlen;

    if (oneByteMode) {
      last = ScanOneByte(src, delim[0]);
    } else {
      last = strstr(src, delim);
    }
    if (last == NULL) {
      strcpysafe(output, len, src);

      if (i == count - 1)
        return 1;
      return 0;
    }
    length = last - src;
    addlen = length + strlen(delim);
  }
  strncpysafe(output, len, src, length);

  return 1;
}
