#ifndef _SAACPROTOUTIL_H_
#define _SAACPROTOUTIL_H_
#include "version.h"
#include "proto.h"
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#endif

#define saacproto__NOENCRYPT
#define saacproto__NODEBUG
#ifdef _SAACPROTOUTIL_C_
TagProto saacproto = {
    NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1,
};
char **saacproto_stringwrapper;
char saacproto_readlogfilename[1024];
char saacproto_writelogfilename[1024];
#else
extern char **saacproto_stringwrapper;
extern TagProto saacproto;
extern char saacproto_readlogfilename[1024];
extern char saacproto_writelogfilename[1024];
#endif
void saacproto_splitString(char *src);
char *saacproto_wrapStringAddr(char *copy, int maxcopylen, char *src);
int saacproto_ClientRead(void);
unsigned int saacproto_GetNewMessageID(void);
void saacproto_CreateHeader(char *out, const char *fname);
void saacproto_CreateHeaderID(char *out, unsigned msg_id, const char *fname);
int saacproto_default_write_wrap(int fd, char *buf, int size);
void saacproto_Send(const int fd, const char *msg);
void saacproto_DebugSend(const int fd, const char *msg);

#endif
