#ifndef __TOOL_H__
#define __TOOL_H__

int getStringToken(char *src, char delim, int count, int maxlen, char *out);
int getIntegerToken(char *src, char delim, int count);
int a62toi(const char*);
int getInteger62Token(char *, char, int);

void jEncode(char *, int, int, char *, int *, int);
void jDecode(char *, int, int, char *, int *);

int strncmpi(char *s1, char *s2, int len);
// Important.
char *makeStringFromEscaped(char *src);
char *makeEscapeString(char *src, char *dest, int sizeofdest);

#endif /*__TOOL_H__*/
