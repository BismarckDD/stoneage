#ifndef __COMMON_COMMON_H__
#define __COMMON_COMMON_H__

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include "windows_compat.h"
#else
#include <error.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifndef _WIN32
#define sa_mkdir(path, mode) mkdir((path), (mode))
#define sa_chmod(path, mode) chmod((path), (mode))
#endif

#define RETURN_FALSE_IF_FALSE(x)                                               \
  {                                                                            \
    if (!x)                                                                    \
      return FALSE;                                                            \
  }
#define EXIT_WITH_CODE_IF_FALSE(x, code)                                       \
  {                                                                            \
    if (!x)                                                                    \
      exit(code);                                                              \
  }

// #define mj_sprintf(format, arg...)                                             \
//   ((sizeof(format) == 4) ? sprintf(format, ##arg)                              \
//                          : snprintf(format, sizeof(format), ##arg))

#define min(x, y)                                                              \
  ({                                                                           \
    typeof(x) __x = (x), __y = (y);                                            \
    (__x < __y) ? __x : __y;                                                   \
  })
#define max(x, y)                                                              \
  ({                                                                           \
    typeof(x) __x = (x), __y = (y);                                            \
    (__x < __y) ? __y : __x;                                                   \
  })
#define swap(x, y)                                                             \
  ({                                                                           \
    typeof(x) __x = (y);                                                       \
    (y) = (x);                                                                 \
    (x) = __x;                                                                 \
  })

#define STRINGBUFSIZ 16384 // 4096*4

#define debug(x, y) fprintf(stderr, #x " = %" #y "\n", x)
#define arraysizeof(x) (sizeof(x) / sizeof(x[0]))
#define errorprint                                                             \
  {                                                                            \
    fprint("%s\n", strerror(errno));                                           \
  }

#define logErr(format, args...) fprintf(stderr, format, ##args)
#define logOut(format, args...) fprintf(stdout, format, ##args)
#define logFile(filename, format, args...)                                     \
  do {                                                                         \
    FILE *fp;                                                                  \
    fp = fopen(filename, "a+");                                                \
    if (fp != NULL) {                                                          \
      if (sizeof(#args) == 0) {                                                \
        fwrite(format, strlen(format), 1, fp);                                 \
      } else {                                                                 \
        fprintf(fp, format, ##args);                                           \
      }                                                                        \
      fclose(fp);                                                              \
    } else {                                                                   \
      fprintf(stderr, "cannot open file %s. reason:%s\n", filename,            \
              strerror(errno));                                                \
    }                                                                          \
  } while (0);

// struct tm now;
// time_t timestamp; 长整型时间戳
#define logFileToday(format, args...)                                          \
  do {                                                                         \
    char filename[32];                                                         \
    struct tm today;                                                           \
    time_t timestamp;                                                          \
    time(&timestamp);                                                          \
    memcpy(&today, localtime(&timestamp), sizeof(today));                      \
    sprintf(filename, "%04d-%02d-%02d.log", today.tm_year + 1900,              \
            today.tm_mon + 1, today.tm_mday);                                  \
    logFile(filename, format, ##args);                                         \
  } while (0);
#ifdef __GNUC__
#define print(format, arg...) fprintf(stderr, format, ##arg)
#define fprint(format, arg...)                                                \
  fprintf(stderr, "%s:%d:", __FILE__, __LINE__),                            \
      fprintf(stderr, format, ##arg)
#define printErr(format, arg...) fprintf(stderr, format, ##arg)
#define printErrX(format, arg...)                                              \
  fprintf(stderr, "%s:%d:", format, __FILE__, __LINE__, ##arg)
#endif // __GUNC__

#define BACKSLASH '\\'
#define NEWLINE '\n'
#define TAB '\t'
#define SPACE ' '
//
#define SUCCESSFUL "successful"
#ifdef FAILED
#undef FAILED
#endif
#define FAILED "failed"
#define LOCK 1
#define UNLOCK 0
//
#define ON 1
#define OFF 0
#define BOOL int
#define FALSE 0
#define TRUE 1
#define INLINE inline
#define LINE_SIZE 1024

#define RAND(x, y)                                                             \
  ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))

#define GETLOWVALUE(x) ((x) & 0x0000FFFF)
#define GETHIGHVALUE(x) ((((x) & 0xFFFF0000) >> 16) & 0x0000FFFF)
#define SETLOWVALUE(x, y) ((x) = ((x) & 0xFFFF0000) | ((y) & 0x0000FFFF))
#define SETHIGHVALUE(x, y)                                                     \
  ((x) = ((x) & 0x0000FFFF) | (((y) << 16) & 0xFFFF0000))
#define MAKE2VALUE(x, y) ((((x) << 16) & 0xFFFF0000) | ((y) & 0x0000FFFF))

#define GETFIRSTVALUE(x) ((x) & 0x000000FF)
#define GETSECONDVALUE(x) ((((x) & 0x0000FF00) >> 8) & 0x000000FF)
#define GETTHIRDVALUE(x) ((((x) & 0x00FF0000) >> 16) & 0x000000FF)
#define GETFOURTHVALUE(x) ((((x) & 0xFF000000) >> 24) & 0x000000FF)

#define SETFIRSTVALUE(x, y) ((x) = (((x) & 0xFFFFFF00) | (y & 0x000000FF)))
#define SETSECONDVALUE(x, y)                                                   \
  ((x) = (((x) & 0xFFFF00FF) | (((y) << 8) & 0x0000FF00)))
#define SETTHIRDVALUE(x, y)                                                    \
  ((x) = (((x) & 0xFF00FFFF) | (((y) << 16) & 0x00FF0000)))
#define SETFOURTHVALUE(x, y)                                                   \
  ((x) = (((x) & 0x00FFFFFF) | (((y) << 24) & 0xFF000000)))

#define MAKE4VALUE(a, b, c, d)                                                 \
  ((((a) << 24) & 0xFF000000) | (((b) << 16) & 0x00FF0000) |                   \
   (((c) << 8) & 0x0000FF00) | ((d) & 0x000000FF))

#endif // __COMMON_COMMON_H__
