#ifndef __UTIL_H__
#define __UTIL_H__

#define arraysizeof(x) (sizeof(x) / sizeof(x[0]))

int strcatsafe(char *dst, const int max_len, const char *app);
void prepareDirectories(char *base);
void easyGetTokenFromString( char *src,int count,char*output,int len );
int isFile(const char *fn);
int createFile(const char *fn, const char *line);
void makeDirFilename(char *out, const int outlen,
                     const char *base, const int dirchar, const char *child);

char* makeStringFromEscaped(char* src);
char* makeEscapeString(const char* src, char* dst, const int dst_len);
int getHash (const char* s );
char * chop( char *s );
int hashpjw (const char* s );

int easyGetTokenFromBuf(char* src, char* delim, int count, char* output, int len);

#endif /* ifndef _UTIL_H_ */

