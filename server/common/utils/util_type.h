#ifndef __COMMON_UTILS_UTIL_TYPE_H__
#define __COMMON_UTILS_UTIL_TYPE_H__

typedef struct tagSTRING32 {
  char string[32];
} STRING32;

typedef struct tagSTRING64 {
  char string[64];
} STRING64;

typedef struct tagSTRING128 {
  char string[128];
} STRING128;

/*
 * Win32 already owns RECT, POINT, CHAR, SHORT, INT and DOUBLE in the
 * ordinary C identifier namespace.  Keep the legacy source-level names on
 * Windows, but map them to project-prefixed declarations after windows.h has
 * been included by common.h.
 */
typedef struct tagSA_RECT {
  int x;
  int y;
  int width;
  int height;
} SA_RECT;

typedef struct tagSA_POINT {
  int x;
  int y;
} SA_POINT;

typedef enum {
  CTYPE_CHAR,
  CTYPE_SHORT,
  CTYPE_INT,
  CTYPE_DOUBLE
} CTYPE;

#ifdef _WIN32
#define RECT SA_RECT
#define POINT SA_POINT
#define CHAR CTYPE_CHAR
#define SHORT CTYPE_SHORT
#define INT CTYPE_INT
#define DOUBLE CTYPE_DOUBLE
#else
typedef SA_RECT RECT;
typedef SA_POINT POINT;
#define CHAR CTYPE_CHAR
#define SHORT CTYPE_SHORT
#define INT CTYPE_INT
#define DOUBLE CTYPE_DOUBLE
#endif

#endif
