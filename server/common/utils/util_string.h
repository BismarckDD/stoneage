#ifndef __COMMON_UTILS_UTIL_STRING_H__
#define __COMMON_UTILS_UTIL_STRING_H__

#include "common.h"

void easyGetTokenFromString(const char *src, const int count, char *output,
                            const int len);
/* 1-based fields, split on an exact delimiter string. Empty fields are kept.
 * Truncation succeeds; out-of-range indices copy the last field and fail.
 * Invalid arguments fail without modifying buf. */
BOOL getDelimitedField(const char *src, const char *delim, int index,
                       char *buf, int buflen);
/* Initialize *cursor to the source; NULL marks exhaustion. Keeps trailing
 * empty fields. Rejects empty delimiters. Do not modify source storage or
 * overlap buf with it during iteration. Linear traversal for fixed delim. */
BOOL nextDelimitedField(const char **cursor, const char *delim,
                        char *buf, int buflen);

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
#endif /* __COMMON_UTILS_UTIL_STRING_H__ */
