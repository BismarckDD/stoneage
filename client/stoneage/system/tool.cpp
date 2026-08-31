#include "../systeminc/system.h"
#define WIN32_LEAN_AND_MEAN
#include "../systeminc/tool.h"

#ifdef _TIME_GET_TIME
LARGE_INTEGER tickCount;
LARGE_INTEGER CurrentTick;
#endif

void Delay(ULONG ulMicroSeconds) {
  LARGE_INTEGER timeStop;
  LARGE_INTEGER timeStart;
  LARGE_INTEGER Freq;
  ULONG ulTimeToWait;

  if (!QueryPerformanceFrequency(&Freq))
    return;

  ulTimeToWait = Freq.QuadPart * ulMicroSeconds / 1000 / 1000;

  QueryPerformanceCounter(&timeStart);

  timeStop = timeStart;

  while (timeStop.QuadPart - timeStart.QuadPart < ulTimeToWait) {
    QueryPerformanceCounter(&timeStop);
  }
}

BOOL isXP() {
  OSVERSIONINFOEX os;
  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  if (GetVersionEx((OSVERSIONINFO *)&os)) {
    switch (os.dwMajorVersion) {
    case 4:
      return FALSE;
    case 5:
      return TRUE;
    case 6:
      return FALSE;
    default:
      return FALSE;
    }
  } else {
    return FALSE;
  }
}

unsigned int TimeGetTime(void) {
#ifdef _TIME_GET_TIME
  static __int64 time;
  QueryPerformanceCounter(&CurrentTick);
  return (unsigned int)(time = CurrentTick.QuadPart / tickCount.QuadPart);
  // return GetTickCount();
#else
  return GetTickCount();
  // return timeGetTime();
#endif
}

static int copyStringUntilDelim(unsigned char *, char delim, int,
                                unsigned char *);

inline unsigned char *searchDelimPoint(unsigned char *src,
                                       unsigned char delim) {
  // ASCII delimiters cannot occur inside a UTF-8 multi-byte sequence.
  // The old GBK pair-skip could swallow the separator after a UTF-8 character.
  for (unsigned char *pt = src; *pt != '\0'; ++pt) {
    if (*pt == delim)
      return pt + 1;
  }
  return NULL;
}

/*
   char *src : ???更??
   char delim : ????
   int count : ????????????????
   int maxlen : out ???????更?????
   char *out : ?

   */
int getStringToken(char *src, char delim, int count, int maxlen, char *out) {
  int c = 1;
  int i;
  unsigned char *pt;

  pt = (unsigned char *)src;
  for (i = 0; i < count - 1; i++) {
    if (pt == (unsigned char *)0)
      break;

    pt = searchDelimPoint(pt, delim);
  }

  if (pt == (unsigned char *)0) {
    out[0] = '\0';
    return 1;
  }

  return copyStringUntilDelim(pt, delim, maxlen, (unsigned char *)out);
}

/*
  char *src : ??更??
  char delim : ????
  int maxlen : ????????
  char *out : ?

*/
static int copyStringUntilDelim(unsigned char *src, char delim, int maxlen,
                                unsigned char *out) {
  int i;
  for (i = 0; i < maxlen; ++i) {
    if (src[i] == (unsigned char)delim) {
      out[i] = '\0';
      return 0;
    }
    out[i] = src[i];
    if (out[i] == '\0')
      return 1;
  }
  out[i] = '\0';
  return 1;
}

/*
  ???????intπ??????

  char *src : ???更??
  char delim: ???????更??
  int count :????????? ??????????

  return value : π

  */
int getIntegerToken(char *src, char delim, int count) {
  char s[128];

  getStringToken(src, delim, count, sizeof(s) - 1, s);

  if (s[0] == '\0')
    return -1;

  return atoi(s);
}

// 0-9,a-z(10-35),A-Z(36-61)
int a62toi(const char *a) {
  int ret = 0;
  int fugo = 1;
  while (*a != NULL) {
    ret *= 62;
    if ('0' <= (*a) && (*a) <= '9')
      ret += (*a) - '0';
    else if ('a' <= (*a) && (*a) <= 'z')
      ret += (*a) - 'a' + 10;
    else if ('A' <= (*a) && (*a) <= 'Z')
      ret += (*a) - 'A' + 36;
    else if (*a == '-')
      fugo = -1;
    else
      return 0;
    a++;
  }
  return ret * fugo;
}

/*
  char *src : ???更??
  char delim: ???????更??
  int count :????????? ??????????
  return value : π
*/
int getInteger62Token(char *src, char delim, int count) {
  char s[128];
  getStringToken(src, delim, count, sizeof(s) - 1, s);
  if (s[0] == '\0')
    return -1;
  return a62toi(s);
}

/*
  更???更??????更???坊????

  char *s1 : 更???
  char *s2 : 更???
  int len : ??坊?更???


  */
int strncmpi(char *s1, char *s2, int len) {
  int c1, c2;
  for (int i = 0; i < len; i++) {
    if (s1[i] == '\0' || s2[i] == '\0')
      return 0;
    c1 = tolower(s1[i]);
    c2 = tolower(s2[i]);
    if (c1 != c2)
      return 1;
  }
  return 0;
}

typedef struct tagEscapeChar {
  char escapechar;
  char escapedchar;
} EscapeChar;

static EscapeChar escapeChar[] = {
    {'\n', 'n'},
    {',', 'c'},
    {'|', 'z'},
    {'\\', 'y'},
};

char makeCharFromEscaped(char c) {
  for (int i = 0; i < sizeof(escapeChar) / sizeof(escapeChar[0]); i++) {
    if (escapeChar[i].escapedchar == c) {
      c = escapeChar[i].escapechar;
      break;
    }
  }
  return c;
}

/*----------------------------------------
 * makeEscapeString?????更???????
 * ??
 *  src             char*       ????更????????▔??
 * 刺?π
 *  src    ?刺??(???向??????????)
 ----------------------------------------*/
char *makeStringFromEscaped(char *src) {
  int srclen = strlen(src);
  int searchindex = 0;
  for (int i = 0; i < srclen; i++) {
    {
      if (src[i] == '\\') {
        int j;
        i++;
        for (j = 0; j < sizeof(escapeChar) / sizeof(escapeChar[0]); j++)
          if (escapeChar[j].escapedchar == src[i]) {
            src[searchindex++] = escapeChar[j].escapechar;
            goto NEXT;
          }
        src[searchindex++] = src[i];
      } else
        src[searchindex++] = src[i];
    }
  NEXT:;
  }
  src[searchindex] = '\0';
  return src;
}

/*----------------------------------------
 * ????????
 * ??
 *  src             char*       ????更??
 *  dest            char*       ????????更??
 *  sizeofdest      int         dest ? ???
 * 刺?π
 *  dest    ?刺??(???向??????????)
 ----------------------------------------*/
char *makeEscapeString(char *src, char *dest, int sizeofdest) {
  int srclen = strlen(src);
  int destindex = 0;
  for (int i = 0; i < srclen; i++) {
    if (destindex + 1 >= sizeofdest)
      break;
    {
      BOOL dirty = FALSE;
      char escapechar = '\0';
      for (int j = 0; j < sizeof(escapeChar) / sizeof(escapeChar[0]); j++)
        if (src[i] == escapeChar[j].escapechar) {
          dirty = TRUE;
          escapechar = escapeChar[j].escapedchar;
          break;
        }
      if (dirty == TRUE) {
        if (destindex + 2 < sizeofdest) {
          dest[destindex++] = '\\';
          dest[destindex++] = escapechar;
        } else
          break;
      } else
        dest[destindex++] = src[i];
    }
  }
  dest[destindex] = '\0';
  return dest;
}

/*
char *src    ????adress
int srclen    ????src?????
int keystring    ????????????
char *encoded    ????????更????宇??????????
int *encodedlen    ????????更?????????????
int maxencodedlen ????????????
*/
void jEncode(char *src, int srclen, int key, char *encoded, int *encodedlen,
             int maxencodedlen) {
  char sum = 0;
  int i;

  if (srclen + 1 > maxencodedlen) {
    // ?????????? 1 ????
    // ????????????????Μ????????
    *encodedlen = maxencodedlen;
    for (i = 0; i < (*encodedlen); i++) {
      encoded[i] = src[i];
    }
  }

  if (srclen + 1 <= maxencodedlen) {
    // ??????????
    *encodedlen = srclen + 1;
    for (i = 0; i < srclen; i++) {
      sum = sum + src[i];
      // ！卡??7??????2?????????????劫?
      if (((key % 7) == (i % 5)) || ((key % 2) == (i % 2))) {
        src[i] = ~src[i];
      }
    }
    for (i = 0; i < (*encodedlen); i++) {
      if (abs((key % srclen)) > i) {
        encoded[i] = src[i] + sum * ((i * i) % 3); // ?????????
      } else
        // key%srclen??ㄅ?「??????????ㄅ?
        if (abs((key % srclen)) == i) {
          encoded[i] = sum;
        } else if (abs((key % srclen)) < i) {
          encoded[i] = src[i - 1] + sum * ((i * i) % 7); // ?????????
        }
    }
  }
}

/*
char *src    ?????????
int srclen    ????????????????
int key        ????????可???
char *decoded    ???????更????
int *decodedlen    ???????更?????
*/
void jDecode(char *src, int srclen, int key, char *decoded, int *decodedlen) {
  char sum;
  int i;

  *decodedlen = srclen - 1;
  sum = src[abs(key % (*decodedlen))];

  for (i = 0; i < srclen; i++) {
    if (abs((key % (*decodedlen))) > i) {
      decoded[i] = src[i] - sum * ((i * i) % 3);
    }

    if (abs((key % (*decodedlen))) < i) {
      decoded[i - 1] = src[i] - sum * ((i * i) % 7);
    }
  }
  for (i = 0; i < (*decodedlen); i++) {
    if (((key % 7) == (i % 5)) || ((key % 2) == (i % 2))) {
      decoded[i] = ~decoded[i];
    }
  }
}
