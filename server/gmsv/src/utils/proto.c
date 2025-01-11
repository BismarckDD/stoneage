#include "utils/proto.h"

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
  /* special case */
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

char *proto_ltoa(long v) {
  char out[64];
  cnv10to62((int)v, out, sizeof(out));
  return out;
}

char *proto_utoa(unsigned long v) {
  char out[64];
  sprintf(out, "%u", (unsigned int)v);
  return out;
}

void proto_strcpysafe(char *des, const char *src, const int max_len) {
  int i;
  const int limit = max_len - 1;
  for (i = 0; i < limit; ++i) {
    des[i] = src[i];
    if (src[i] == 0)
      break;
  }
  des[i] = 0;
}
void proto_strcatsafe(char *des, const char *src, const int max_len) {
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

char *lssproto_escapeString(char *a) {
  int i, c = 0;
  lssproto.escapework[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      lssproto.escapework[c++] = '\0';
      break;
    } else if ((char)0x80 <= a[i] && a[i] <= (char)0xFF) {
      // for 2 Byte Word
      lssproto.escapework[c++] = a[i++];
      lssproto.escapework[c++] = a[i];
    } else if (a[i] == '\\') {
      lssproto.escapework[c++] = '\\';
      lssproto.escapework[c++] = '\\';
    } else if (a[i] == ' ') {
      lssproto.escapework[c++] = '\\';
      lssproto.escapework[c++] = 'S';
    } else if (a[i] == '\n') {
      lssproto.escapework[c++] = '\\';
      lssproto.escapework[c++] = 'n';
    } else if (a[i] == '\r') {
      lssproto.escapework[c++] = '\\';
      lssproto.escapework[c++] = 'r';
    } else {
      lssproto.escapework[c++] = a[i];
    }
  }
  return lssproto.escapework;
}
char *lssproto_descapeString(char *a) {
  int i, c = 0;
  lssproto.escapework[0] = '\0';
  for (i = 0;; i++) {
    if (a[i] == '\0') {
      lssproto.escapework[c++] = '\0';
      break;
    } else if ((char)0x80 <= a[i] && a[i] <= (char)0xFF) {
      lssproto.escapework[c++] = a[i++];
      lssproto.escapework[c++] = a[i];
    } else if (a[i] == '\\') {
      if (a[i + 1] == 0) {
        lssproto.escapework[c++] = a[i];
        continue;
      }
      if (a[i + 1] == 'S') {
        lssproto.escapework[c++] = ' ';
      } else if (a[i + 1] == 'n') {
        lssproto.escapework[c++] = '\n';
      } else if (a[i + 1] == 'r') {
        lssproto.escapework[c++] = '\r';
      } else if (a[i + 1] == '\\') {
        lssproto.escapework[c++] = '\\';
      } else {
        lssproto.escapework[c++] = a[i];
      }
      i++;
    } else {
      lssproto.escapework[c++] = a[i];
    }
  }
  return lssproto.escapework;
}

char *mkstr_int(int i) {
#define MKSTR_INT(v) proto_ltoa((long)(v))
  proto_strcpysafe(proto.val_str, (char *)MKSTR_INT(i), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_u_int(unsigned int i) {
#define MKSTR_U_INT(v) proto_utoa((unsigned long)(v))
  proto_strcpysafe(proto.val_str, MKSTR_U_INT(i), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_long(long l) {
#define MKSTR_LONG(v) proto_ltoa(v)
  proto_strcpysafe(proto.val_str, MKSTR_LONG(l), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_u_long(unsigned long l) {
#define MKSTR_U_LONG(v) proto_utoa(v)
  proto_strcpysafe(proto.val_str, MKSTR_U_LONG(l), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_short(short s) {
#define MKSTR_SHORT(v) proto_ltoa((long)(v))
  proto_strcpysafe(proto.val_str, MKSTR_SHORT(s), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_u_short(unsigned short s) {
#define MKSTR_U_SHORT(v) proto_utoa((unsigned long)(v)&0xFFFF)
  proto_strcpysafe(proto.val_str, MKSTR_U_SHORT(s), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_char(char c) {
#define MKSTR_CHAR(v) proto_ltoa((long)(v))
  proto_strcpysafe(proto.val_str, MKSTR_CHAR(c), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_u_char(unsigned char c) {
#define MKSTR_U_CHAR(v) proto_utoa((unsigned long)(v)&0xFF)
  proto_strcpysafe(proto.val_str, MKSTR_U_CHAR(c), proto.workbufsize);
  proto_strcatsafe(proto.val_str, " ", proto.workbufsize);
  return proto.val_str;
}
char *mkstr_string(char *a) {
  char *ret = proto_escapeString(a);
  proto_strcatsafe(ret, " ", proto.workbufsize);
  return ret;
}
char *mkstr_float(float f) {
  sprintf(proto.val_str, "%f ", f);
  return proto.val_str;
}
char *mkstr_double(double d) {
  sprintf(proto.val_str, "%f ", d);
  return proto.val_str;
}

#define MKSTR_ARRAY_MACRO(func)                                                \
  {                                                                            \
    int i;                                                                     \
    proto.arraywork[0] = '\0';                                                 \
    for (i = 0; i < size; i++) {                                               \
      proto_strcatsafe(proto.arraywork, func(array[i]), proto.workbufsize);    \
    }                                                                          \
    return proto.arraywork;                                                    \
  }

char *mkstr_int_array(int size, int *array) { MKSTR_ARRAY_MACRO(mkstr_int); }
char *mkstr_u_int_array(int size, unsigned int *array) {
  MKSTR_ARRAY_MACRO(mkstr_u_int);
}
char *mkstr_short_array(int size, short *array) {
  MKSTR_ARRAY_MACRO(mkstr_short);
}
char *mkstr_u_short_array(int size, unsigned short *array) {
  MKSTR_ARRAY_MACRO(mkstr_u_short);
}
char *mkstr_char_array(int size, char *array) { MKSTR_ARRAY_MACRO(mkstr_char); }
char *mkstr_u_char_array(int size, unsigned char *array) {
  MKSTR_ARRAY_MACRO(mkstr_u_char);
}
char *mkstr_float_array(int size, float *array) {
  MKSTR_ARRAY_MACRO(mkstr_float);
}
char *mkstr_double_array(int size, double *array) {
  MKSTR_ARRAY_MACRO(mkstr_double);
}
int demkstr_int(char *a) {
  if (a == (char *)NULL)
    return 0;
  return cnv62to10(a);
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
char demkstr_char(const char *a) {
  if (a == NULL)
    return 0;
  return (char)strtol(a, NULL, 10);
}
unsigned char demkstr_u_char(const unsigned char *a) {
  if (a == NULL)
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
    proto_strcpysafe(proto.escapework, "", proto.workbufsize);
    return proto.escapework;
  }
  return proto_descapeString(a);
}
#define DEMKSTR_ARRAY_MACRO(func, defaultvalue)                                \
  {                                                                            \
    int i;                                                                     \
    for (i = start; i < (start + size); i++) {                                 \
      if (tk[i] == NULL) {                                                     \
        buf[i - start] = defaultvalue;                                         \
      } else {                                                                 \
        buf[i - start] = func(tk[i]);                                          \
      }                                                                        \
    }                                                                          \
    return buf;                                                                \
  }
int *demkstr_int_array(char **tk, int *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_int, 0);
}
int *demkstr_u_int_array(char **tk, int *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_u_int, 0);
}
unsigned int *demkstr_long_array(char **tk, unsigned int *buf, int start,
                                 int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_long, 0);
}
unsigned long *demkstr_u_long_array(char **tk, unsigned long *buf, int start,
                                    int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_u_long, 0);
}
short *demkstr_short_array(char **tk, short *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_short, 0);
}
unsigned short *demkstr_u_short_array(char **tk, unsigned short *buf, int start,
                                      int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_u_short, 0);
}
char *demkstr_char_array(char **tk, char *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_u_char, 0);
}
unsigned char *demkstr_u_char_array(char **tk, unsigned char *buf, int start,
                                    int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_u_char, 0);
}
float *demkstr_float_array(char **tk, float *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_float, (float)0.0);
}
double *demkstr_u_double_array(char **tk, double *buf, int start, int size) {
  DEMKSTR_ARRAY_MACRO(demkstr_double, (double)0.0);
}