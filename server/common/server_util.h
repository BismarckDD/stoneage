#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__

#include "workspace.h"

#ifdef __SERVER_UTIL_C__
#define EXTERN
#else
#define EXTERN extern
#endif

#define SAAC_SERVER_MAXLSRPCARGS 8
#define SAAC_CLIENT_MAXLSRPCARGS 8
#define GMSV_SERVER_MAXLSRPCARGS 14

void GetMessageInfo(int *id, char *function_name, const int max_len,
                    const char **token_list);
void SplitString(char *src, WorkSpace *ws);
unsigned GetNewMessageID(void);
void CreateHeader(char *out, const char *fname);
void CreateHeaderID(char *out, const unsigned msg_id, const char *fname);
void Send(WorkSpace *ws, const int fd, char *msg);
void DebugSend(WorkSpace *ws, const int fd, char *msg);

#endif // __SERVER_UTIL_H__
