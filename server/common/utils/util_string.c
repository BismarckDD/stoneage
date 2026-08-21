#define __COMMON_UTILS_UTIL_STRING_C__

#include "util_string.h"

#define PRIME 211

int hashpjw(const char *s) {
  return hashpjwEx(s, PRIME);
}

int hashpjwEx(const char *src, const int prime) {
  char *p;
  unsigned int h = 0, g;
  for (p = src; *p; p++) {
    h = (h << 4) + (*p);
    if ((g = h & 0xF0000000) != 0) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % prime;
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

#define ISSPACETAB(c) ((c) == ' ' || (c) == '\t')
void easyGetTokenFromString(const char *src, const int count, char *output,
                            const int len) {
  int i;
  int counter = 0;
  if (len <= 0)
    return;
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

int CreateDir(const char *dirname, int mode) {
  // ret == 0 目录创建成功
  // ret == -1 且不是目录已存在, 则返回错误.
  int ret = sa_mkdir(dirname, mode);
  if (ret < 0 && errno != EEXIST) {
    printf("mkdir error:%d %s: %s\n", ret, strerror(errno), dirname);
    return -1;
  }
  return 0;
}

void PrepareDirectories(const char *base_dirname) {
  int i;
  char dirname[1024];
  for (i = 0; i < 256; i++) {
    snprintf(dirname, sizeof(dirname), "%s/0x%x", base_dirname, i);
    CreateDir(dirname, 0755);
  }
}

void makeDirFilename(char *out, const int out_len, const char *base,
                     const int dir_char, const char *child) {
  snprintf(out, out_len, "%s/0x%x/%s", base, dir_char & 0xFF, child);
}

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

//  移除串中的最后一个字符.
char *chop(char *src) {
  int len = strlen(src);
  if (len >= 1) {
    src[len - 1] = 0;
  }
  return src;
}

char *chompex(char *src) {
  return sa_normalize_text_line(src);
}

char *dchop(char *src, const char *del_str) {
  int i, flag = 0;
  const int src_len = strlen(src);
  const int del_len = strlen(del_str);
  if (src_len == 0 || del_len == 0)
    return src;
  for (i = 0; i < del_len; i++) {
    if (src[src_len - 1] == del_str[i]) {
      flag = 1;
      break;
    }
  }
  if (flag)
    src[src_len - 1] = '\0';
  return src;
}

char *pohcd(char *src, const char *del_str) {
  int i, flag = 0;
  const int src_len = strlen(src);
  const int del_len = strlen(del_str);
  if (src_len == 0 || del_len == 0)
    return src;
  for (i = 0; i < del_len; i++) {
    if (src[0] == del_str[i]) {
      flag = 1;
      break;
    }
  }
  if (flag)
    for (i = 0; i < src_len; i++)
      src[i] = src[i + 1];
  return src;
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
      strncpysafe(output, output_len, first);
      // 做一下校验是否是到最后的这种情况.
      if (i == count - 1)
        return 1;
      return 0;
    }
    length = last - first;
    delta = length + delim_len;
  }
  // 针对找到了delim,要复制到delime的case
  // 2026.08.20 有修改
  strncpysafe2(output, output_len, first, last);
  return 1;
}


BOOL GeneralSplitImpl(const char *src, const char *delim, const int index,
                      char *buf, const int buflen, const char *file,
                      const int line) {
  int i, length = 0, offset = 0;
  const int delim_len = strlen(delim);
  char *last; // delim开始字节所在的位置
  for (i = 0; i < index; i++) {
    src += offset;
    if (delim_len == 1) {
      last = strstr_onebyte(src, delim[0]);
    } else {
      last = strstr(src, delim);
    }
    if (last == NULL) {
      strncpysafe(buf, buflen, src);
      // printf("%s,%s,%d\n", buf, src, buflen);
      return i == index - 1 ? TRUE : FALSE;
    }
    length = last - src;
    offset = length + delim_len;
  }
  // 2026.08.20 有修改
  strncpysafe2(buf, buflen, src, last);
  // printf("%s,%s,%d\n", buf, src, buflen);
  return TRUE;
}
