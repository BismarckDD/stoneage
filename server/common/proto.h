#ifndef __COMMON_PROTO_H__
#define __COMMON_PROTO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *cnv10to62(const int input, char *output, const int output_len);
int cnv62to10(const char *input);
char *common_ltoa(long l);
char *common_utoa(unsigned long u);
void strcpysafe(char *des, const char *src, const int max_len);
void strcatsafe(char *des, const char *src, const int max_len);
void GetMessageInfo(int *id, char *function_name, const int max_len, const char **token_list);

typedef struct tagProto {
  int (*write_func)(int, char *, int); /* write function */
  unsigned work_buf_size; /* size of work area */
  char *work;
  char *array_work;
  char *escape_work;
  char *val_str;
  char *ret_work; /* work areas which have same size  */
  char *crypt_work;
  char *jencode_copy;
  char *jencode_out;
  char *compress_work;  /* these work has bigger size (3times) */
  char **token_list;   /* token list */
  unsigned message_id; /* = 1, for generating message IDs */
} TagProto;

#ifdef __COMMON_PROTO_C__
TagProto gProto;
#else
extern TagProto gProto;
#endif

char *escape_string(const char *a);
char *descape_string(const char *a);
InitProto(TagProto *proto, const int work_buf_size);

#endif // __UTILS_PROTO_H__
