// Arminius' protocol utilities ver 0.1
//
// Any questions and bugs, mailto: arminius@mail.hwaei.com.tw

#ifndef __AUTIL_H__
#define __AUTIL_H__
#include "version.h"
#define SLICE_MAX 20
#define SLICE_SIZE 65500
extern char **MesgSlice; // store message slices
extern int SliceCount;   // count slices in MesgSlice
extern char PersonalKey[];

#define DEFAULTTABLE                                                           \
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{}"
#define DEFAULTFUNCBEGIN "&"
#define DEFAULTFUNCEND "#"

void util_Init(void);
void util_Release(void);
void util_SplitMessage(char *source, char *separator);
void util_EncodeMessage(char *dst, char *src);
void util_DecodeMessage(char *dst, char *src);
int util_GetFunctionFromSlice(int *func, int *fieldcount);
void util_DiscardMessage(void);
void util_SendMesg(int fd, int func, char *buffer);

// -------------------------------------------------------------------
// Encoding function units.  Use in Encrypting functions.
int util_256to64(char *dst, char *src, int len, char *table);
int util_64to256(char *dst, char *src, char *table);
int util_256to64_shr(char *dst, char *src, int len, char *table, char *key);
int util_shl_64to256(char *dst, char *src, char *table, char *key);
int util_256to64_shl(char *dst, char *src, int len, char *table, char *key);
int util_shr_64to256(char *dst, char *src, char *table, char *key);

void util_swapint(int *dst, int *src, char *rule);
void util_xorstring(char *dst, char *src);
void util_shrstring(char *dst, char *src, int offs);
void util_shlstring(char *dst, char *src, int offs);
// -------------------------------------------------------------------
// Encrypting functions
int util_deint(int sliceno, int *value);
int util_mkint(char *buffer, int value);
int util_destring(int sliceno, char *value);
int util_mkstring(char *buffer, char *value);

int strcmptail(char *s1, char *s2);

#ifdef _FONT_STYLE_
struct WM_STR_STYLE {
  int x;
  int size;
  int color;
  char str[128];
};

struct WM_STR {
  int flg;
  WM_STR_STYLE style[30];
};

struct TITLE_STR_STYLE {
  int x;
  int size;
  int color;
  char str[32];
};

struct TITLE_STR {
  int flg;
  int len;
  TITLE_STR_STYLE style[10];
};

void getCharTitleSplit(char *str, TITLE_STR *title);
extern WM_STR wmstr[25];
void getStrSplitNew(char str[][256]);
extern char *sunday(char *str, char *subStr);
void PutWinText(int x, int y, char fontPrio, int color, char *str, BOOL hitFlag,
                int index);

#endif // _FONT

#endif //

BOOL getStringFromIndexWithDelim_body(char *src, char *delim, int index,
                                      char *buf, int buflen);

int getUtf8CharNum(const char *);
int getUtf8SequenceLength(const char *text, size_t remainingBytes);
BOOL isValidUtf8(const char *text);
size_t getUtf8PrefixBytes(const char *text, int maxDisplayWidth);
BOOL copyUtf8WithLimit(char *destination, size_t destinationSize,
                       const char *source, int maxDisplayWidth);
