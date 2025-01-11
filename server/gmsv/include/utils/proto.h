#ifndef __UTILS_PROTO_H__
#define __UTILS_PROTO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *proto_cnv10to62(const int input, char *output, const int output_len);
int proto_cnv62to10(const char *input);
char *proto_ltoa(long l);
char *proto_utoa(unsigned long u);
void proto_strcpysafe(char *des, const char *src, int max_len);
void proto_strcatsafe(char *des, const char *src, int max_len);

typedef struct tagProto {
  int (*write_func)(int, char *, int); /* write function */
  unsigned workbufsize; /* size of work area */
  char *work;
  char *arraywork;
  char *escapework;
  char *val_str;
  char *ret_work; /* work areas which have same size  */
  char *cryptwork;
  char *jencodecopy;
  char *jencodeout;
  char *compresswork;  /* these work has bigger size (3times)*/
  char **token_list;   /* token list */
  unsigned message_id; /*= 1,  */ /* for generating message IDs */
} TagProto;

#endif // __UTILS_PROTO_H__
