#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORKSPACE_TOKEN_CAPACITY 32

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

char *mkstr_int(WorkSpace *workspace, const int value);
char *mkstr_u_int(WorkSpace *workspace, const unsigned int value);
char *mkstr_long(WorkSpace *workspace, const long value);
char *mkstr_u_long(WorkSpace *workspace, const unsigned long value);
char *mkstr_short(WorkSpace *workspace, const short value);
char *mkstr_u_short(WorkSpace *workspace, const unsigned short value);
char *mkstr_char(WorkSpace *workspace, const char value);
char *mkstr_u_char(WorkSpace *workspace, const unsigned char value);
char *mkstr_string(WorkSpace *workspace, const char *value);
char *mkstr_float(WorkSpace *workspace, const float value);
char *mkstr_double(WorkSpace *workspace, const double value);
char *mkstr_int_array(WorkSpace *workspace, const int size,
                      const int *array);
char *mkstr_u_int_array(WorkSpace *workspace, const int size,
                        const unsigned int *array);
char *mkstr_short_array(WorkSpace *workspace, const int size,
                        const short *array);
char *mkstr_u_short_array(WorkSpace *workspace, const int size,
                          const unsigned short *array);
char *mkstr_char_array(WorkSpace *workspace, const int size,
                       const char *array);
char *mkstr_u_char_array(WorkSpace *workspace, const int size,
                         const unsigned char *array);
char *mkstr_float_array(WorkSpace *workspace, const int size,
                        const float *array);
char *mkstr_double_array(WorkSpace *workspace, const int size,
                         const double *array);
int demkstr_int(const char *value);
unsigned int demkstr_u_int(const char *value);
long demkstr_long(const char *value);
unsigned long demkstr_u_long(const char *value);
short demkstr_short(const char *value);
unsigned short demkstr_u_short(const char *value);
char demkstr_char(const char *value);
unsigned char demkstr_u_char(const char *value);
float demkstr_float(const char *value);
double demkstr_double(const char *value);
char *demkstr_string(WorkSpace *workspace, const char *value);

/* Generated SAAC protocol sources use their file-local `ws` implicitly. */
#ifndef __WORKSPACE_C__
#define mkstr_int(value) mkstr_int(ws, (value))
#define mkstr_u_int(value) mkstr_u_int(ws, (value))
#define mkstr_long(value) mkstr_long(ws, (value))
#define mkstr_u_long(value) mkstr_u_long(ws, (value))
#define mkstr_short(value) mkstr_short(ws, (value))
#define mkstr_u_short(value) mkstr_u_short(ws, (value))
#define mkstr_char(value) mkstr_char(ws, (value))
#define mkstr_u_char(value) mkstr_u_char(ws, (value))
#define mkstr_string(value) mkstr_string(ws, (value))
#define mkstr_float(value) mkstr_float(ws, (value))
#define mkstr_double(value) mkstr_double(ws, (value))
#define mkstr_int_array(size, array) mkstr_int_array(ws, (size), (array))
#define mkstr_u_int_array(size, array)                                        \
  mkstr_u_int_array(ws, (size), (array))
#define mkstr_short_array(size, array)                                        \
  mkstr_short_array(ws, (size), (array))
#define mkstr_u_short_array(size, array)                                      \
  mkstr_u_short_array(ws, (size), (array))
#define mkstr_char_array(size, array) mkstr_char_array(ws, (size), (array))
#define mkstr_u_char_array(size, array)                                       \
  mkstr_u_char_array(ws, (size), (array))
#define mkstr_float_array(size, array)                                        \
  mkstr_float_array(ws, (size), (array))
#define mkstr_double_array(size, array)                                       \
  mkstr_double_array(ws, (size), (array))
#define demkstr_string(value) demkstr_string(ws, (value))
#endif

char *escape_string(WorkSpace *ws, const char *a);
char *descape_string(WorkSpace *ws, const char *a);
int InitWorkSpace(WorkSpace *ws, int (*write_func)(int, char *, int),
              const int work_buf_size, const int string_buffer_size);
void FreeWorkSpace(WorkSpace *ws);

#endif // __WORKSPACE_H__
