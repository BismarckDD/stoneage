#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *cnv10to62(const int input, char *output, const int output_len);
int cnv62to10(const char *input);
char *common_ltoa(const long l);
char *common_utoa(const unsigned long u);
char *strcpysafe(char *des, const int max_len, const char *src);
char *strcpysafe2(char *des, const int max_len, const char *src);
char *strncpysafe2(char *des, const int max_len, const char *src);
char *strcatsafe(char *des, const char *src, const int max_len);

typedef struct tagWorkSpace {
  int (*write_func)(int, char *, int); /* write function */
  int work_buf_size;              /* size of work area */
  int string_buffer_size;
  char *work;
  char *array_work;
  char *escape_work;
  char *val_str;
  char *ret_work; /* work areas which have same size  */
  char *crypt_work;
  char *jencode_copy;
  char *jencode_out;
  char *compress_work; /* these work has bigger size (3times) */
  char **token_list;   /* token list */
  char **string_buffer;
  char r_log[1024];
  char w_log[1024];
} WorkSpace;

char *escape_string(WorkSpace *ws, const char *a);
char *descape_string(WorkSpace *ws, const char *a);
int InitWorkSpace(WorkSpace *ws, int (*write_func)(int, char *, int),
              const int work_buf_size, const int string_buffer_size);
void FreeWorkSpace(WorkSpace *ws);

#endif // __WORKSPACE_H__
