#ifndef __COMMON_UTILS_UTIL_STRING_H__
#define __COMMON_UTILS_UTIL_STRING_H__

#include "common.h"

void easyGetTokenFromString(const char *src, const int count, char *output,
                            const int len);
BOOL GeneralSplitImpl(const char *src, const char *delim, const int index,
                      char *buf, const int buflen, const char *file,
                      const int line);
#define getStringFromIndexWithDelim(src, delim, index, buf, buflen)            \
  GeneralSplitImpl(src, delim, index, buf, buflen, __FILE__, __LINE__)

char *makeStringFromEscaped(char *src);
char *makeEscapeString(const char *src, char *dst, const int dst_len);
int getHash(const char *s);

char *chop(char *src);
// src的尾字符是否在del_str中，若是，则删除
char *dchop(char *src, const char *del_str);
/* 删除文本行尾。兼容 LF、CRLF，以及旧数据包中常见的 CRCRLF。 */
#define chomp(src) chompex(src)
/* 从第一个 CR/LF 起截断字符串。 */
char *chompex(char *src);
// src的头字符是否在del_str中，若是，则删除
char *pohcd(char *src, const char *del_str);

int hashpjw(const char *s);
int hashpjwEx(const char *s, const int prime);

int easyGetTokenFromBuf(const char *src, const char *delim, const int count,
                        char *output, const int len);

void util_strncpysafe1(char *dst, const int dst_len, const char *src,
                       const int copy_bytes);
void util_strncpysafe2(char *dst, const int dst_len, const char *src);

#endif /* __COMMON_UTILS_UTIL_STRING_H__ */
