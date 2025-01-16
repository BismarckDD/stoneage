#define __UTIL_C__

#include "util.h"

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
      printf("mkdir error:%d %s: %s\n", ret, strerror(errno), dname);
    }
    if (ret == 0)
      printf(".");
  }
}

void makeDirFilename(char *out, const int out_len, const char *base,
                     int dir_char, const char *child) {
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
  int ri /*read pointer*/, wi /*write pointer*/ = 0, j;
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

int util_strcatsafe(char *dst, const int max_len, const char *app) {
  int dst_len = strlen(dst);
  int app_len = strlen(app);
  if ((dst_len + app_len) >= max_len) {
    return -1;
  } else {
    strcat(dst, app);
  }
  return 0;
}

//  移除串中的最后一个字符.
char *chop(char *src) {
  int len = strlen(src);
  if (len >= 1) {
    src[len - 1] = 0;
  }
  return src;
}

void util_strncpysafe1(char *dst, const int dst_len, const char *src,
                       const int copy_bytes) {
  if (dst_len <= 0 || copy_bytes <= 0)
    return;
  if (!dst || !src)
    return;
  int src_len = strlen(src);
  if (copy_bytes < src_len)
    src_len = copy_bytes;
  if (dst_len < src_len + 1) {
    strncpy(dst, src, dst_len - 1);
    dst[dst_len - 1] = '\0';
  } else {
    strncpy(dst, src, src_len);
    dst[src_len] = '\0';
  }
}

void util_strncpysafe2(char *dst, const int dst_len, const char *src) {
  if (dst_len <= 0)
    return;
  if (!dst || !src)
    return;
  int src_len = strlen(src);
  if (dst_len < src_len + 1) {
    strncpy(dst, src, dst_len - 1);
    dst[dst_len - 1] = '\0';
  } else {
    strncpy(dst, src, src_len);
    dst[src_len] = '\0';
  }
}

char *strstr_onebyte(char *src, char delim) {
  if (!src)
    return NULL;
  while (*src != '\0') {
    if (*src == delim) {
      return src;
    }
    ++src;
  }
  return NULL;
}

int easyGetTokenFromBuf(const char *src, const char *delim, const int count,
                        char *output, const int output_len) {
  int i, length = 0, delta = 0;
  const char *first = src;
  const char *last;
  if (strlen(src) == 0)
    return 0;
  int delim_len = strlen(delim);
  for (i = 0; i < count; i++) {
    first += delta;
    if (delim_len == 1) {
      last = strstr_onebyte(first, delim[0]);
    } else {
      last = strstr(first, delim);
    }
    // 针对没有找到delim,要复制到最后的case
    if (last == NULL) {
      util_strncpysafe2(output, output_len, first);
      // 做一下校验是否是到最后的这种情况.
      if (i == count - 1)
        return 1;
      return 0;
    }
    length = last - first;
    delta = length + delim_len;
  }
  // 针对找到了delim,要复制到delime的case
  util_strncpysafe1(output, output_len, first, length);
  return 1;
}
