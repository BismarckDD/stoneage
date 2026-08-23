#define __WORKSPACE_C__

#include "workspace.h"

char *cnv10to62(int src_input, char *output, int output_len) {
  int i, j;
  char base[] = {
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  int tmp[64];
  int src;
  int minus;
  int baselen = sizeof(base) - 1;
  if (src_input < 0) {
    minus = 1;
    src_input *= -1;
  } else {
    minus = 0;
  }
  if (src_input < baselen) {
    if (minus) {
      *(output) = '-';
      *(output + 1) = base[src_input];
      *(output + 2) = '\0';
      return (output);
    } else {
      *output = base[src_input];
      *(output + 1) = '\0';
      return (output);
    }
  }
  src = src_input;
  for (i = 0; src >= baselen; i++) {
    tmp[i] = src % baselen;
    src /= baselen;
  }
  i--;
  if (minus) {
    *output = '-';
    *(output + 1) = base[src];
    for (j = 2; i >= 0; i--, j++) {
      if (j > output_len - 2)
        return NULL;
      *(output + j) = base[tmp[i]];
    }
  } else {
    *output = base[src];
    for (j = 1; i >= 0; i--, j++) {
      if (j > output_len - 2)
        return NULL;
      *(output + j) = base[tmp[i]];
    }
  }
  *(output + j) = '\0';
  return (output);
}

int cnv62to10(const char *input) {
  int ret = 0;
  int minus;
  if (input[0] == '-') {
    minus = -1;
    input++;
  } else {
    minus = 1;
  }
  while (*input != '\0') {
    ret *= 62;
    if ('0' <= (*input) && (*input) <= '9')
      ret += (*input) - '0';
    else if ('a' <= (*input) && (*input) <= 'z')
      ret += (*input) - 'a' + 10;
    else if ('A' <= (*input) && (*input) <= 'Z')
      ret += (*input) - 'A' + 36;
    else
      return 0;
    input++;
  }
  return ret * minus;
}

char *common_ltoa(const long v) {
  static char out[64];
  sprintf(out, "%ld", v);
  return out;
}

char *common_utoa(const unsigned long v) {
  static char out[64];
  sprintf(out, "%lu", v);
  return out;
}

// strncpy并不保证以'\0'结尾，因此需要strncpysafe
char *strncpysafe(char *des, const int max_len, const char *src) {
  int i;
  const int limit = max_len - 1;
  for (i = 0; i < limit; ++i) {
    des[i] = src[i];
    if (src[i] == '\0')
      break;
  }
  des[i] = '\0';
  return des;
}

char *strncpysafe2(char *des, const int des_len, const char *first, const char *last) {
  char *des_last = des + des_len - 1;
  while (first < last && des < des_last) {
    *des++ = *first++;
  }
  *des = '\0';
  return des;
}
// strncat中的第三个参数，代表是可以连接的长度，而不是des的总容量
char *strncatsafe(char *des, const char *src, const int des_len) {
  int i, j;
  const int idx_upper = des_len - 1;
  for (i = 0; i < idx_upper; ++i)
    if (des[i] == '\0')
      break;

  for (j = i; j < idx_upper; j++) {
    if (src[j - i] == '\0')
      break;
    des[j] = src[j - i];
  }
  des[j] = '\0';
  return des;
}

char *mkstr_int(WorkSpace *ws, const int i) {
#define MKSTR_INT(v) common_ltoa((const long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, (char *)MKSTR_INT(i));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_u_int(WorkSpace *ws, const unsigned int i) {
#define MKSTR_U_INT(v) common_utoa((const unsigned long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_U_INT(i));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_long(WorkSpace *ws, const long l) {
#define MKSTR_LONG(v) common_ltoa(v)
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_LONG(l));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_u_long(WorkSpace *ws, const unsigned long l) {
#define MKSTR_U_LONG(v) common_utoa(v)
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_U_LONG(l));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_short(WorkSpace *ws, const short s) {
#define MKSTR_SHORT(v) common_ltoa((const long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_SHORT(s));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_u_short(WorkSpace *ws, const unsigned short s) {
#define MKSTR_U_SHORT(v) common_utoa((const unsigned long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_U_SHORT(s));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_char(WorkSpace *ws, const char c) {
#define MKSTR_CHAR(v) common_ltoa((long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_CHAR(c));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_u_char(WorkSpace *ws, const unsigned char c) {
#define MKSTR_U_CHAR(v) common_utoa((const unsigned long)(v))
  strncpysafe(ws->val_str, ws->work_buf_size, MKSTR_U_CHAR(c));
  strncatsafe(ws->val_str, " ", ws->work_buf_size);
  return ws->val_str;
}
char *mkstr_string(WorkSpace *ws, const char *a) {
  char *ret = escape_string(ws, a);
  strncatsafe(ret, " ", ws->work_buf_size);
  return ret;
}
char *mkstr_float(WorkSpace *ws, const float f) {
  sprintf(ws->val_str, "%f", f);
  return ws->val_str;
}
char *mkstr_double(WorkSpace *ws, const double d) {
  sprintf(ws->val_str, "%f", d);
  return ws->val_str;
}

#define MKSTR_ARRAYMACRO(func)                                                 \
  {                                                                            \
    int i;                                                                     \
    ws->array_work[0] = '\0';                                                  \
    for (i = 0; i < size; i++) {                                               \
      strncatsafe(ws->array_work, func(ws, array[i]), ws->work_buf_size);       \
    }                                                                          \
    return ws->array_work;                                                     \
  }
char *mkstr_int_array(WorkSpace *ws, const int size, const int *array) {
  MKSTR_ARRAYMACRO(mkstr_int);
}
char *mkstr_u_int_array(WorkSpace *ws, const int size,
                        const unsigned int *array) {
  MKSTR_ARRAYMACRO(mkstr_u_int);
}
char *mkstr_short_array(WorkSpace *ws, const int size, const short *array) {
  MKSTR_ARRAYMACRO(mkstr_short);
}
char *mkstr_u_short_array(WorkSpace *ws, const int size,
                          const unsigned short *array) {
  MKSTR_ARRAYMACRO(mkstr_u_short);
}
char *mkstr_char_array(WorkSpace *ws, const int size, const char *array) {
  MKSTR_ARRAYMACRO(mkstr_char);
}
char *mkstr_u_char_array(WorkSpace *ws, const int size,
                         const unsigned char *array) {
  MKSTR_ARRAYMACRO(mkstr_u_char);
}
char *mkstr_float_array(WorkSpace *ws, const int size, const float *array) {
  MKSTR_ARRAYMACRO(mkstr_float);
}
char *mkstr_double_array(WorkSpace *ws, const int size, const double *array) {
  MKSTR_ARRAYMACRO(mkstr_double);
}
int demkstr_int(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (int)strtol(a, NULL, 10);
}
unsigned int demkstr_u_int(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (unsigned int)strtoul(a, NULL, 10);
}
long demkstr_long(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (long)strtol(a, NULL, 10);
}
unsigned long demkstr_u_long(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (unsigned long)strtoul(a, NULL, 10);
}
short demkstr_short(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (short)strtol(a, NULL, 10);
}
unsigned short demkstr_u_short(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (unsigned short)strtoul(a, NULL, 10);
}
char demkstr_char(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (char)strtol(a, NULL, 10);
}
unsigned char demkstr_u_char(const char *a) {
  if (a == (const char *)NULL)
    return 0;
  return (unsigned char)strtoul(a, NULL, 10);
}
float demkstr_float(const char *a) {
  if (a == (const char *)NULL)
    return 0.0F;
  return (float)atof(a);
}
double demkstr_double(const char *a) {
  if (a == (const char *)NULL)
    return 0.0F;
  return (double)strtod(a, NULL);
}
char *demkstr_string(WorkSpace *ws, const char *a) {
  if (a == (const char *)NULL) {
    strncpysafe(ws->escape_work, ws->work_buf_size, "");
    return ws->escape_work;
  }
  return descape_string(ws, a);
}

#define DEMKSTR_ARRAYMACRO(func, defaultvalue)                                 \
  {                                                                            \
    int i;                                                                     \
    for (i = start; i < (start + size); i++) {                                 \
      if (token_list[i] == NULL) {                                             \
        array[i - start] = defaultvalue;                                       \
      } else {                                                                 \
        array[i - start] = func(token_list[i]);                                \
      }                                                                        \
    }                                                                          \
    return array;                                                              \
  }
int *demkstr_int_array(const char **token_list, int *array, int start,
                       int size) {
  DEMKSTR_ARRAYMACRO(demkstr_int, 0);
}
unsigned int *demkstr_u_int_array(const char **token_list, unsigned int *array,
                                  int start, int size) {
  DEMKSTR_ARRAYMACRO(demkstr_u_int, 0);
}
long *demkstr_long_array(const char **token_list, long *array, int start,
                         int size) {
  DEMKSTR_ARRAYMACRO(demkstr_long, 0);
}
unsigned long *demkstr_u_long_array(const char **token_list,
                                    unsigned long *array, int start, int size) {
  DEMKSTR_ARRAYMACRO(demkstr_u_long, 0);
}
short *demkstr_short_array(const char **token_list, short *array, int start,
                           int size) {
  DEMKSTR_ARRAYMACRO(demkstr_short, 0);
}
unsigned short *demkstr_u_short_array(const char **token_list,
                                      unsigned short *array, int start,
                                      int size) {
  DEMKSTR_ARRAYMACRO(demkstr_u_short, 0);
}
char *demkstr_char_array(const char **token_list, char *array, int start,
                         int size) {
  DEMKSTR_ARRAYMACRO(demkstr_char, 0);
}
unsigned char *demkstr_u_char_array(const char **token_list,
                                    unsigned char *array, int start, int size) {
  DEMKSTR_ARRAYMACRO(demkstr_u_char, 0);
}
float *demkstr_float_array(const char **token_list, float *array, int start,
                           int size) {
  DEMKSTR_ARRAYMACRO(demkstr_float, (float)0.0);
}
double *demkstr_u_double_array(const char **token_list, double *array,
                               int start, int size) {
  DEMKSTR_ARRAYMACRO(demkstr_double, (double)0.0);
}

char *escape_string(WorkSpace *ws, const char *a) {
  int i, c = 0;
  ws->escape_work[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      ws->escape_work[c++] = '\0';
      break;
    } else if (a[i] == '\\') {
      ws->escape_work[c++] = '\\';
      ws->escape_work[c++] = '\\';
    } else if (a[i] == ' ') {
      ws->escape_work[c++] = '\\';
      ws->escape_work[c++] = 'S';
    } else if (a[i] == '\n') {
      ws->escape_work[c++] = '\\';
      ws->escape_work[c++] = 'n';
    } else if (a[i] == '\r') {
      ws->escape_work[c++] = '\\';
      ws->escape_work[c++] = 'r';
    } else {
      ws->escape_work[c++] = a[i];
    }
  }
  return ws->escape_work;
}
char *descape_string(WorkSpace *ws, const char *a) {
  int i, c = 0;
  ws->escape_work[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      ws->escape_work[c++] = '\0';
      break;
    } else if (a[i] == '\\') {
      if (a[i + 1] == 0) { /* null */
        ws->escape_work[c++] = a[i];
        continue;
      }
      if (a[i + 1] == 'S') { /* space */
        ws->escape_work[c++] = ' ';
      } else if (a[i + 1] == 'n') {
        ws->escape_work[c++] = '\n';
      } else if (a[i + 1] == 'r') {
        ws->escape_work[c++] = '\r';
      } else if (a[i + 1] == '\\') {
        ws->escape_work[c++] = '\\';
      } else {
        ws->escape_work[c++] = a[i];
      }
      i++;
    } else {
      ws->escape_work[c++] = a[i];
    }
  }
  return ws->escape_work;
}

#define FREE(x) if (x != NULL) free(x);

int InitWorkSpace(WorkSpace *ws, int (*write_func)(int, const char*, int),
                  const int work_buf_size, const int string_buffer_size) {

  ws->work_buf_size = work_buf_size;
  ws->string_buffer_size = string_buffer_size;
  ws->write_func = write_func;

  int i, j;
  // Init String Buffer.
  ws->string_buffer = (char **)calloc(1, sizeof(char **) * ws->string_buffer_size);
  if (ws->string_buffer == NULL)
    return -1;
  for (i = 0; i < ws->string_buffer_size; ++i) {
    ws->string_buffer[i] = (char *)calloc(1, ws->work_buf_size);
    if (ws->string_buffer[i] == NULL) {
      for (j = 0; j < i; ++j) {
        FREE(ws->string_buffer[j]);
      }
      return -1;
    }
    memset(ws->string_buffer[i], 0, ws->work_buf_size);
  }
  /* One additional slot is reserved for the terminating NULL pointer. */
  ws->token_list =
      (char **)calloc(WORKSPACE_TOKEN_CAPACITY + 1, sizeof(*ws->token_list));
  if (ws->token_list == NULL)
    return -1;
  ws->work = (char *)calloc(1, ws->work_buf_size);
  ws->array_work = (char *)calloc(1, ws->work_buf_size);
  ws->escape_work = (char *)calloc(1, ws->work_buf_size);
  ws->val_str = (char *)calloc(1, ws->work_buf_size);
  ws->crypt_work = (char *)calloc(1, ws->work_buf_size * 3);
  ws->jencode_copy = (char *)calloc(1, ws->work_buf_size * 3);
  ws->jencode_out = (char *)calloc(1, ws->work_buf_size * 3);
  ws->compress_work = (char *)calloc(1, ws->work_buf_size * 3);
  ws->ret_work = (char *)calloc(1, ws->work_buf_size);
  if (ws->work == NULL || ws->array_work == NULL || ws->escape_work == NULL ||
      ws->val_str == NULL || ws->crypt_work == NULL ||
      ws->jencode_copy == NULL || ws->jencode_out == NULL ||
      ws->compress_work == NULL || ws->ret_work == NULL) {
    printf("Memory Alloc Failed.\n");
    FreeWorkSpace(ws);
    return -1;
  }
  memset(ws->work, 0, ws->work_buf_size);
  memset(ws->array_work, 0, ws->work_buf_size);
  memset(ws->escape_work, 0, ws->work_buf_size);
  memset(ws->val_str, 0, ws->work_buf_size);
  memset(ws->crypt_work, 0, ws->work_buf_size * 3);
  memset(ws->jencode_copy, 0, ws->work_buf_size * 3);
  memset(ws->jencode_out, 0, ws->work_buf_size * 3);
  memset(ws->compress_work, 0, ws->work_buf_size * 3);
  // 内存越界写会导致fopen无法查明原因的阻塞，这个还是很有意思的.
  memset(ws->ret_work, 0, ws->work_buf_size);
  return 0;
}

void FreeWorkSpace(WorkSpace *ws) {
  int i;
  for (i = 0; i < ws->string_buffer_size; i++) {
    FREE(ws->string_buffer[i]);
  }
  FREE(ws->string_buffer);
  FREE(ws->work);
  FREE(ws->array_work);
  FREE(ws->escape_work);
  FREE(ws->val_str);
  FREE(ws->token_list);
  FREE(ws->crypt_work);
  FREE(ws->jencode_copy);
  FREE(ws->jencode_out);
  FREE(ws->compress_work);
  FREE(ws->ret_work);
}

void SetLogFiles(WorkSpace *ws, const char *r_log, const char *w_log) {
  strncpysafe(ws->w_log, sizeof(ws->w_log), w_log);
  strncpysafe(ws->r_log, sizeof(ws->r_log), r_log);
}
