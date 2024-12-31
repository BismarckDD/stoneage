#include <cstring>

const int maxSize = 256;

char *sunday(char *str, char *subStr) {
  int next[maxSize];
  int strLen = strlen(str);
  int subLen = strlen(subStr);
  int i, j, pos;
  for (i = 0; i < maxSize; i++) {
    next[i] = subLen + 1;
  }
  for (i = 0; i < subLen; i++) {
    // 计算子串中的字符到字符串结尾的\0之间的距离
    next[(unsigned char)subStr[i]] = subLen - i;
  }
  pos = 0;
  while (pos <= (strLen - subLen)) {
    i = pos;
    for (j = 0; j < subLen; j++, i++) {
      if (str[i] != subStr[j]) {
        pos += next[(unsigned char)str[pos + subLen]]; // 向后移动
        break;
      }
    }
    if (j == subLen) {  // 找到字串，返回
      return str + pos;
    }
  }
  return NULL;
}
