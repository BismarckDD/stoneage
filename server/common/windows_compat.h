#ifndef STONEAGE_WINDOWS_COMPAT_H
#define STONEAGE_WINDOWS_COMPAT_H

/*
 * Native Windows compatibility for the legacy server.
 *
 * The game server historically uses a POSIX socket descriptor both as a
 * socket handle and as an index into its connection array.  A WinSock SOCKET
 * is pointer-sized and cannot safely be used that way, so this layer exposes
 * small virtual descriptors and translates them to WinSock handles.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef FD_SETSIZE
#define FD_SETSIZE 4096
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/*
 * WinSock's timeval stores seconds in a 32-bit long, while 64-bit MinGW uses
 * a 64-bit time_t.  The legacy server frequently passes &tv_sec to localtime,
 * so retaining WinSock's layout makes those calls read past the field.  Use a
 * project timeval with a time_t-sized seconds field and translate it only at
 * the WinSock select boundary.
 */
struct sa_timeval {
  time_t tv_sec;
  long tv_usec;
};
#define timeval sa_timeval

#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x0004
#endif
#ifndef W_OK
#define W_OK 2
#endif

/* MinGW exposes dirent, while the legacy sources use glibc's 64-bit aliases. */
#ifndef dirent64
#define dirent64 dirent
#endif
#ifndef readdir64
#define readdir64 readdir
#endif

/* The non-epoll Windows build only needs mutexes. SRWLOCK is zero-initializable. */
typedef SRWLOCK pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER SRWLOCK_INIT
static inline int pthread_mutex_init(pthread_mutex_t *mutex,
                                     const void *attributes) {
  (void)attributes;
  InitializeSRWLock(mutex);
  return 0;
}
static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) {
  (void)mutex;
  return 0;
}
static inline int pthread_mutex_lock(pthread_mutex_t *mutex) {
  AcquireSRWLockExclusive(mutex);
  return 0;
}
static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  ReleaseSRWLockExclusive(mutex);
  return 0;
}

int sa_platform_init(void);
void sa_platform_cleanup(void);
void sa_sleep(unsigned int seconds);
int sa_mkdir(const char *path, int mode);
int sa_chmod(const char *path, int mode);
int sa_set_process_priority(int nice_value);
int sa_gettimeofday(struct timeval *value, void *timezone_value);
int sa_inet_aton(const char *address, struct in_addr *result);
int sa_install_console_handler(void (*handler)(int));
int sa_gbk_to_utf8(const char *source, char *destination,
                   size_t destination_size);
char *index(const char *text, int character);
char *rindex(const char *text, int character);
long random(void);

int sa_socket_open(int domain, int type, int protocol);
int sa_socket_accept(int fd, struct sockaddr *address, int *address_length);
int sa_socket_connect(int fd, const struct sockaddr *address,
                      int address_length);
int sa_socket_bind(int fd, const struct sockaddr *address, int address_length);
int sa_socket_listen(int fd, int backlog);
int sa_socket_close(int fd);
int sa_socket_read(int fd, void *buffer, int length);
int sa_socket_write(int fd, const void *buffer, int length);
int sa_socket_send(int fd, const char *buffer, int length, int flags);
int sa_socket_recv(int fd, char *buffer, int length, int flags);
int sa_socket_select(int ignored_nfds, fd_set *read_set, fd_set *write_set,
                     fd_set *error_set, const struct timeval *timeout);
int sa_socket_setsockopt(int fd, int level, int option, const char *value,
                         int value_length);
int sa_socket_fcntl(int fd, int command, ...);

#ifndef SA_WINDOWS_COMPAT_IMPLEMENTATION
#define socket sa_socket_open
#define accept sa_socket_accept
#define connect sa_socket_connect
#define bind sa_socket_bind
#define listen sa_socket_listen
#define close sa_socket_close
#define read sa_socket_read
#define write sa_socket_write
#define send sa_socket_send
#define recv sa_socket_recv
#define select sa_socket_select
#define setsockopt sa_socket_setsockopt
#define fcntl sa_socket_fcntl
#define gettimeofday sa_gettimeofday
#define inet_aton sa_inet_aton
#define access _access
#define unlink _unlink
#define fileno _fileno
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#endif /* STONEAGE_WINDOWS_COMPAT_H */
