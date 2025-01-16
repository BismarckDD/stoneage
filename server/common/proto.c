#define __COMMON_PROTO_C__
#include "proto.h"

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

char *common_ltoa(long v) {
  char out[64];
  sprintf(out, "%ld", (long)v);
  return out;
}

char *common_utoa(unsigned long v) {
  char out[64];
  sprintf(out, "%lu", (unsigned long)v);
  return out;
}

void strcpysafe(char *des, const char *src, const int max_len) {
  int i;
  const int limit = max_len - 1;
  for (i = 0; i < limit; ++i) {
    des[i] = src[i];
    if (src[i] == 0)
      break;
  }
  des[i] = 0;
}

void strcatsafe(char *des, const char *src, const int max_len) {
  int i, j;
  const limit = max_len - 1;
  for (i = 0; i < limit; ++i) {
    if (des[i] == 0) {
      for (j = i; j < limit; j++) {
        des[j] = src[j - i];
        if (src[j - i] == 0)
          break;
      }
      des[j] = 0;
      break;
    }
  }
}

void GetMessageInfo(int *id, char *function_name, const int len,
                    const char **token_list) {
  if (token_list[0] == NULL || token_list[1] == NULL) {
    *id = 0;
    strcpysafe(function_name, "", len);
    return;
  }
  *id = strtoul(token_list[0], NULL, 10);
  strcpysafe(function_name, token_list[1], len);
  return;
}

char *mkstr_int(int i) {
#define MKSTR_INT(v) common_ltoa((long)(v))
  strcpysafe(gProto.val_str, (char *)MKSTR_INT(i), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_u_int(unsigned int i) {
#define MKSTR_U_INT(v) common_utoa((unsigned long)(v))
  strcpysafe(gProto.val_str, MKSTR_U_INT(i), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_long(long l) {
#define MKSTR_LONG(v) common_ltoa(v)
  strcpysafe(gProto.val_str, MKSTR_LONG(l), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_u_long(unsigned long l) {
#define MKSTR_U_LONG(v) common_utoa(v)
  strcpysafe(gProto.val_str, MKSTR_U_LONG(l), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_short(short s) {
#define MKSTR_SHORT(v) common_ltoa((long)(v))
  strcpysafe(gProto.val_str, MKSTR_SHORT(s), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_u_short(unsigned short s) {
#define MKSTR_U_SHORT(v) common_utoa((unsigned long)(v)&0xFFFF)
  strcpysafe(gProto.val_str, MKSTR_U_SHORT(s), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_char(char c) {
#define MKSTR_CHAR(v) common_ltoa((long)(v))
  strcpysafe(gProto.val_str, MKSTR_CHAR(c), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_u_char(unsigned char c) {
#define MKSTR_U_CHAR(v) common_utoa((unsigned long)(v)&0xFF)
  strcpysafe(gProto.val_str, MKSTR_U_CHAR(c), gProto.work_buf_size);
  strcatsafe(gProto.val_str, " ", gProto.work_buf_size);
  return gProto.val_str;
}
char *mkstr_string(char *a) {
  char *ret = escape_string(a);
  strcatsafe(ret, " ", gProto.work_buf_size);
  return ret;
}
char *mkstr_float(float f) {
  sprintf(gProto.val_str, "%f ", f);
  return gProto.val_str;
}
char *mkstr_double(double d) {
  sprintf(gProto.val_str, "%f ", d);
  return gProto.val_str;
}

#define MKSTR_ARRAYMACRO(func)                                                 \
  {                                                                            \
    int i;                                                                     \
    gProto.array_work[0] = '\0';                                               \
    for (i = 0; i < size; i++) {                                               \
      strcatsafe(gProto.array_work, func(array[i]), gProto.work_buf_size);     \
    }                                                                          \
    return gProto.array_work;                                                  \
  }
char *mkstr_int_array(int size, int *array) { MKSTR_ARRAYMACRO(mkstr_int); }
char *mkstr_u_int_array(int size, unsigned int *array) {
  MKSTR_ARRAYMACRO(mkstr_u_int);
}
char *mkstr_short_array(int size, short *array) {
  MKSTR_ARRAYMACRO(mkstr_short);
}
char *mkstr_u_short_array(int size, unsigned short *array) {
  MKSTR_ARRAYMACRO(mkstr_u_short);
}
char *mkstr_char_array(int size, char *array) { MKSTR_ARRAYMACRO(mkstr_char); }
char *mkstr_u_char_array(int size, unsigned char *array) {
  MKSTR_ARRAYMACRO(mkstr_u_char);
}
char *mkstr_float_array(int size, float *array) {
  MKSTR_ARRAYMACRO(mkstr_float);
}
char *mkstr_double_array(int size, double *array) {
  MKSTR_ARRAYMACRO(mkstr_double);
}

int demkstr_int(const char *a) {
  if (a == (char *)NULL)
    return 0;
  return (int)strtol(a, NULL, 10);
}

unsigned int demkstr_u_int(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (unsigned int)strtoul(a, NULL, 10);
}
long demkstr_long(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (long)strtol(a, NULL, 10);
}
unsigned long demkstr_u_long(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (unsigned long)strtoul(a, NULL, 10);
}
short demkstr_short(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (short)strtol(a, NULL, 10);
}
unsigned short demkstr_u_short(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (unsigned short)strtoul(a, NULL, 10);
}
char demkstr_char(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (char)strtol(a, NULL, 10);
}
unsigned char demkstr_u_char(char *a) {
  if (a == (char *)NULL)
    return 0;
  return (unsigned char)strtoul(a, NULL, 10);
}
float demkstr_float(char *a) {
  if (a == (char *)NULL)
    return 0.0F;
  return (float)atof(a);
}
double demkstr_double(char *a) {
  if (a == (char *)NULL)
    return 0.0F;
  return (double)strtod(a, NULL);
}
char *demkstr_string(char *a) {
  if (a == (char *)NULL) {
    strcpysafe(gProto.escape_work, "", gProto.work_buf_size);
    return gProto.escape_work;
  }
  return descape_string(a);
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

char *escape_string(const char *a) {
  int i, c = 0;
  gProto.escape_work[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      gProto.escape_work[c++] = '\0';
      break;
    } else if (a[i] == '\\') {
      gProto.escape_work[c++] = '\\';
      gProto.escape_work[c++] = '\\';
    } else if (a[i] == ' ') {
      gProto.escape_work[c++] = '\\';
      gProto.escape_work[c++] = 'S';
    } else if (a[i] == '\n') {
      gProto.escape_work[c++] = '\\';
      gProto.escape_work[c++] = 'n';
    } else if (a[i] == '\r') {
      gProto.escape_work[c++] = '\\';
      gProto.escape_work[c++] = 'r';
    } else {
      gProto.escape_work[c++] = a[i];
    }
  }
  return gProto.escape_work;
}
char *descape_string(const char *a) {
  int i, c = 0;
  gProto.escape_work[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      gProto.escape_work[c++] = '\0';
      break;
    } else if (a[i] == '\\') {
      if (a[i + 1] == 0) { /* null */
        gProto.escape_work[c++] = a[i];
        continue;
      }
      if (a[i + 1] == 'S') { /* space */
        gProto.escape_work[c++] = ' ';
      } else if (a[i + 1] == 'n') {
        gProto.escape_work[c++] = '\n';
      } else if (a[i + 1] == 'r') {
        gProto.escape_work[c++] = '\r';
      } else if (a[i + 1] == '\\') {
        gProto.escape_work[c++] = '\\';
      } else {
        gProto.escape_work[c++] = a[i];
      }
      i++;
    } else {
      gProto.escape_work[c++] = a[i];
    }
  }
  return gProto.escape_work;
}

int InitProto(TagProto *proto, const int work_buf_size) {
  proto->work_buf_size = work_buf_size;
  proto->work = NULL;
  proto->array_work = NULL;
  proto->escape_work = NULL;
  proto->val_str = NULL;
  proto->token_list = NULL;
  proto->crypt_work = NULL;
  proto->jencode_copy = NULL;
  proto->jencode_out = NULL;
  proto->compress_work = NULL;
  proto->work = (char *)calloc(1, proto->work_buf_size);
  proto->array_work = (char *)calloc(1, proto->work_buf_size);
  proto->escape_work = (char *)calloc(1, proto->work_buf_size);
  proto->val_str = (char *)calloc(1, proto->work_buf_size);
  proto->token_list =
      (char **)calloc(1, proto->work_buf_size * sizeof(char **));
  proto->crypt_work = (char *)calloc(1, proto->work_buf_size * 3);
  proto->jencode_copy = (char *)calloc(1, proto->work_buf_size * 3);
  proto->jencode_out = (char *)calloc(1, proto->work_buf_size * 3);
  proto->compress_work = (char *)calloc(1, proto->work_buf_size * 3);
  memset(proto->work, 0, proto->work_buf_size);
  memset(proto->array_work, 0, proto->work_buf_size);
  memset(proto->escape_work, 0, proto->work_buf_size);
  memset(proto->val_str, 0, proto->work_buf_size);
  memset((char *)proto->token_list, 0, proto->work_buf_size * sizeof(char **));
  memset(proto->crypt_work, 0, proto->work_buf_size * 3);
  memset(proto->jencode_copy, 0, proto->work_buf_size * 3);
  memset(proto->jencode_out, 0, proto->work_buf_size * 3);
  memset(proto->compress_work, 0, proto->work_buf_size * 3);
  if (proto->work == NULL || proto->array_work == NULL ||
      proto->escape_work == NULL || proto->val_str == NULL ||
      proto->token_list == NULL || proto->crypt_work == NULL ||
      proto->jencode_copy == NULL || proto->jencode_out == NULL ||
      proto->compress_work == NULL) {
    free(proto->work);
    free(proto->val_str);
    free(proto->escape_work);
    free(proto->array_work);
    free(proto->token_list);
    free(proto->crypt_work);
    free(proto->jencode_copy);
    free(proto->jencode_out);
    free(proto->compress_work);
    return -1;
  }
  return 0;
}