#include "version.h"

#ifndef __AUTIL_H_
#define __AUTIL_H_

#define SLICE_MAX 20
#define SLICE_SIZE 1024 * 96

extern char *MesgSlice[SLICE_MAX];
extern int SliceCount; // count slices in MesgSlice

extern char PersonalKey[4096];

#define DEFAULTTABLE                                                           \
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{}"
#define DEFAULTFUNCBEGIN "&"
#define DEFAULTFUNCEND "#"

BOOL util_Init(void);

// void util_SplitMessage(char *source, char *separator);
BOOL util_SplitMessage(char *source, char *separator);
#ifdef _NEW_FUNC_DECRYPT
BOOL util_DecodeMessageTea(char *dst, char *sac);
#endif
void util_EncodeMessage(char *dst, char *src);
void util_DecodeMessage(char *dst, char *src);
int util_GetFunctionFromSlice(int *func, int *fieldcount);
void util_DiscardMessage(void);
#define util_SendMesg(fd, func, buffer)                                        \
  _util_SendMesg(__FILE__, __LINE__, fd, func, buffer)
void _util_SendMesg(char *file, int line, int fd, int func, char *buffer);

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

#ifdef _CRYPTO_DATA
void CryptoKey(char *usrkey);
void DecryptKey(char *usrkey);
void List(char *path);
#endif

#endif
