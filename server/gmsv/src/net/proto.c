#include "net/proto.h"

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
