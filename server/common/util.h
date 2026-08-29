#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__

#include "common.h"
#include "utils/util_file.h"
#include "utils/util_string.h"
#include "utils/util_type.h"
#include "utils/util_time.h"
#include "utils/md5.h"

#define MAX_DIRPATH_LEN 256
#define MAX_FILEPATH_LEN 288
BOOL strtolchecknum(char *text, void *number, int base, CTYPE type);
int strcmptail(const char *text, const char *suffix);
int textToInt(const char *text);
void deleteCharFromString(char *text, const char *characters);
void deleteCharFromStringNoEscape(char *text, const char *characters);
#define deleteWhiteSpace(text) deleteCharFromStringNoEscape((text), " \t")
char *replaceString(char *text, char old_character, char new_character);
void getFourIntsFromString(char *text, int *value1, int *value2, int *value3,
                           int *value4);
void deleteSequentChar(char *text, char *characters);

int bindLocalhost(unsigned short port);
int connectHost(char *hostname, unsigned short port);
int charInclude(const char *text, const char *characters);
int rgetFileName(const char *directory, STRING64 *files, int capacity);
BOOL checkStringsUnique(char **strings, int count, int verbose);

BOOL PointInRect(RECT *rect, POINT *point);
BOOL CoordinateInRect(RECT *rect, int x, int y);
int clipRect(RECT *rect1, RECT *rect2, RECT *result);
BOOL isstring1or0(char *text);
float linearDiv(float value1, float value2, float delta);
void makeRandomString(char *candidates, char *output, int length);
BOOL checkRedundancy(const int *array, int array_size);
unsigned short CheckCRC(unsigned char *data, int size);

#define ABS(value) ((value) >= 0 ? (value) : -(value))
#define VALIDATEDIR(direction)                                                \
  do {                                                                        \
    (direction) %= 8;                                                         \
    (direction) = ((direction) + 8) % 8;                                      \
  } while (0)

#endif // __COMMON_UTIL_H__
