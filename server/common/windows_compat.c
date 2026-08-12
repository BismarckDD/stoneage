#define SA_WINDOWS_COMPAT_IMPLEMENTATION
#include "windows_compat.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SA_MAX_WINDOWS_SOCKETS FD_SETSIZE

typedef struct tagSaSocketEntry {
  SOCKET socket;
  int used;
  int nonblocking;
} SaSocketEntry;

static SaSocketEntry g_socket_table[SA_MAX_WINDOWS_SOCKETS];
static SRWLOCK g_socket_table_lock = SRWLOCK_INIT;
static INIT_ONCE g_winsock_once = INIT_ONCE_STATIC_INIT;
static int g_winsock_error = WSANOTINITIALISED;
static void (*g_console_handler)(int) = NULL;

char *index(const char *text, int character) {
  return strchr(text, character);
}

char *rindex(const char *text, int character) {
  return strrchr(text, character);
}

long random(void) {
  return rand();
}

static BOOL WINAPI sa_console_control(DWORD control_type) {
  int signal_number;
  if (g_console_handler == NULL)
    return FALSE;
  switch (control_type) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
    signal_number = 2;
    break;
  case CTRL_CLOSE_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    signal_number = 0;
    break;
  default:
    return FALSE;
  }
  g_console_handler(signal_number);
  return TRUE;
}

static void sa_set_socket_errno(int error_code) {
  switch (error_code) {
  case WSAEINTR:
    errno = EINTR;
    break;
  case WSAEWOULDBLOCK:
    /* The legacy read paths consistently treat EAGAIN as non-fatal. */
    errno = EAGAIN;
    break;
  case WSAECONNRESET:
    errno = ECONNRESET;
    break;
  case WSAECONNABORTED:
    errno = ECONNABORTED;
    break;
  case WSAETIMEDOUT:
    errno = ETIMEDOUT;
    break;
  case WSAEADDRINUSE:
    errno = EADDRINUSE;
    break;
  case WSAECONNREFUSED:
    errno = ECONNREFUSED;
    break;
  case WSAENOTSOCK:
    errno = EBADF;
    break;
  case WSAEINVAL:
    errno = EINVAL;
    break;
  default:
    errno = EIO;
    break;
  }
}

static BOOL CALLBACK sa_start_winsock(PINIT_ONCE once, PVOID parameter,
                                      PVOID *context) {
  WSADATA data;
  (void)once;
  (void)parameter;
  (void)context;
  g_winsock_error = WSAStartup(MAKEWORD(2, 2), &data);
  if (g_winsock_error == 0)
    atexit(sa_platform_cleanup);
  return TRUE;
}

int sa_platform_init(void) {
  /* Source strings are UTF-8; make the Windows console decode them as UTF-8. */
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  InitOnceExecuteOnce(&g_winsock_once, sa_start_winsock, NULL, NULL);
  if (g_winsock_error != 0) {
    sa_set_socket_errno(g_winsock_error);
    return -1;
  }
  return 0;
}

static SOCKET sa_native_socket(int fd) {
  SOCKET result = INVALID_SOCKET;
  if (fd <= 0 || fd >= SA_MAX_WINDOWS_SOCKETS) {
    errno = EBADF;
    return INVALID_SOCKET;
  }
  AcquireSRWLockShared(&g_socket_table_lock);
  if (g_socket_table[fd].used)
    result = g_socket_table[fd].socket;
  ReleaseSRWLockShared(&g_socket_table_lock);
  if (result == INVALID_SOCKET)
    errno = EBADF;
  return result;
}

static int sa_register_socket(SOCKET native_socket) {
  int fd;
  AcquireSRWLockExclusive(&g_socket_table_lock);
  for (fd = 1; fd < SA_MAX_WINDOWS_SOCKETS; ++fd) {
    if (!g_socket_table[fd].used) {
      g_socket_table[fd].socket = native_socket;
      g_socket_table[fd].used = 1;
      g_socket_table[fd].nonblocking = 0;
      break;
    }
  }
  ReleaseSRWLockExclusive(&g_socket_table_lock);
  if (fd == SA_MAX_WINDOWS_SOCKETS) {
    errno = EMFILE;
    return -1;
  }
  return fd;
}

void sa_platform_cleanup(void) {
  int fd;
  if (g_winsock_error != 0)
    return;
  AcquireSRWLockExclusive(&g_socket_table_lock);
  for (fd = 1; fd < SA_MAX_WINDOWS_SOCKETS; ++fd) {
    if (g_socket_table[fd].used) {
      closesocket(g_socket_table[fd].socket);
      g_socket_table[fd].used = 0;
      g_socket_table[fd].socket = INVALID_SOCKET;
    }
  }
  ReleaseSRWLockExclusive(&g_socket_table_lock);
  WSACleanup();
  g_winsock_error = WSANOTINITIALISED;
}

void sa_sleep(unsigned int seconds) { Sleep(seconds * 1000U); }

int sa_mkdir(const char *path, int mode) {
  (void)mode;
  return _mkdir(path);
}

int sa_chmod(const char *path, int mode) { return _chmod(path, mode); }

int sa_set_process_priority(int nice_value) {
  DWORD priority = NORMAL_PRIORITY_CLASS;
  if (nice_value > 0)
    priority = BELOW_NORMAL_PRIORITY_CLASS;
  else if (nice_value < 0)
    priority = ABOVE_NORMAL_PRIORITY_CLASS;
  if (!SetPriorityClass(GetCurrentProcess(), priority)) {
    errno = EACCES;
    return -1;
  }
  return 0;
}

int sa_install_console_handler(void (*handler)(int)) {
  g_console_handler = handler;
  if (!SetConsoleCtrlHandler(sa_console_control, TRUE)) {
    errno = EIO;
    return -1;
  }
  return 0;
}

int sa_gettimeofday(struct timeval *value, void *timezone_value) {
  FILETIME file_time;
  ULARGE_INTEGER ticks;
  const uint64_t epoch_delta = UINT64_C(116444736000000000);
  (void)timezone_value;
  if (value == NULL) {
    errno = EINVAL;
    return -1;
  }
  GetSystemTimeAsFileTime(&file_time);
  ticks.LowPart = file_time.dwLowDateTime;
  ticks.HighPart = file_time.dwHighDateTime;
  ticks.QuadPart -= epoch_delta;
  value->tv_sec = (long)(ticks.QuadPart / UINT64_C(10000000));
  value->tv_usec = (long)((ticks.QuadPart % UINT64_C(10000000)) / 10);
  return 0;
}

int sa_inet_aton(const char *address, struct in_addr *result) {
  return InetPtonA(AF_INET, address, result) == 1 ? 1 : 0;
}

int sa_socket_open(int domain, int type, int protocol) {
  SOCKET native_socket;
  int fd;
  if (sa_platform_init() != 0)
    return -1;
  native_socket = socket(domain, type, protocol);
  if (native_socket == INVALID_SOCKET) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  fd = sa_register_socket(native_socket);
  if (fd < 0)
    closesocket(native_socket);
  return fd;
}

int sa_socket_accept(int fd, struct sockaddr *address, int *address_length) {
  SOCKET native_socket = sa_native_socket(fd);
  SOCKET accepted_socket;
  int accepted_fd;
  if (native_socket == INVALID_SOCKET)
    return -1;
  accepted_socket = accept(native_socket, address, address_length);
  if (accepted_socket == INVALID_SOCKET) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  accepted_fd = sa_register_socket(accepted_socket);
  if (accepted_fd < 0)
    closesocket(accepted_socket);
  return accepted_fd;
}

int sa_socket_connect(int fd, const struct sockaddr *address,
                      int address_length) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = connect(native_socket, address, address_length);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

int sa_socket_bind(int fd, const struct sockaddr *address, int address_length) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = bind(native_socket, address, address_length);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

int sa_socket_listen(int fd, int backlog) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = listen(native_socket, backlog);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

int sa_socket_close(int fd) {
  SOCKET native_socket;
  int result;
  if (fd <= 0 || fd >= SA_MAX_WINDOWS_SOCKETS) {
    errno = EBADF;
    return -1;
  }
  AcquireSRWLockExclusive(&g_socket_table_lock);
  if (!g_socket_table[fd].used) {
    ReleaseSRWLockExclusive(&g_socket_table_lock);
    errno = EBADF;
    return -1;
  }
  native_socket = g_socket_table[fd].socket;
  g_socket_table[fd].used = 0;
  g_socket_table[fd].socket = INVALID_SOCKET;
  ReleaseSRWLockExclusive(&g_socket_table_lock);
  result = closesocket(native_socket);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return 0;
}

int sa_socket_read(int fd, void *buffer, int length) {
  return sa_socket_recv(fd, (char *)buffer, length, 0);
}

int sa_socket_write(int fd, const void *buffer, int length) {
  return sa_socket_send(fd, (const char *)buffer, length, 0);
}

int sa_socket_send(int fd, const char *buffer, int length, int flags) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = send(native_socket, buffer, length, flags);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

int sa_socket_recv(int fd, char *buffer, int length, int flags) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = recv(native_socket, buffer, length, flags);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

static void sa_make_native_set(const fd_set *virtual_set, fd_set *native_set) {
  u_int index;
  FD_ZERO(native_set);
  if (virtual_set == NULL)
    return;
  for (index = 0; index < virtual_set->fd_count; ++index) {
    int fd = (int)virtual_set->fd_array[index];
    SOCKET native_socket = sa_native_socket(fd);
    if (native_socket != INVALID_SOCKET)
      FD_SET(native_socket, native_set);
  }
}

static void sa_copy_ready_set(fd_set *virtual_set, const fd_set *native_set) {
  SOCKET virtual_fds[FD_SETSIZE];
  u_int old_count;
  u_int input_index;
  u_int output_count = 0;
  if (virtual_set == NULL)
    return;
  old_count = virtual_set->fd_count;
  memcpy(virtual_fds, virtual_set->fd_array,
         old_count * sizeof(virtual_set->fd_array[0]));
  for (input_index = 0; input_index < old_count; ++input_index) {
    int fd = (int)virtual_fds[input_index];
    SOCKET native_socket = sa_native_socket(fd);
    if (native_socket != INVALID_SOCKET && FD_ISSET(native_socket, native_set))
      virtual_set->fd_array[output_count++] = (SOCKET)fd;
  }
  virtual_set->fd_count = output_count;
}

int sa_socket_select(int ignored_nfds, fd_set *read_set, fd_set *write_set,
                     fd_set *error_set, const struct timeval *timeout) {
  fd_set native_read;
  fd_set native_write;
  fd_set native_error;
  TIMEVAL native_timeout;
  const TIMEVAL *native_timeout_pointer = NULL;
  int result;
  (void)ignored_nfds;
  if (timeout != NULL) {
    native_timeout.tv_sec =
        timeout->tv_sec > LONG_MAX ? LONG_MAX : (long)timeout->tv_sec;
    native_timeout.tv_usec = timeout->tv_usec;
    native_timeout_pointer = &native_timeout;
  }
  sa_make_native_set(read_set, &native_read);
  sa_make_native_set(write_set, &native_write);
  sa_make_native_set(error_set, &native_error);
  result = select(0, read_set ? &native_read : NULL,
                  write_set ? &native_write : NULL,
                  error_set ? &native_error : NULL, native_timeout_pointer);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  sa_copy_ready_set(read_set, &native_read);
  sa_copy_ready_set(write_set, &native_write);
  sa_copy_ready_set(error_set, &native_error);
  return result;
}

int sa_socket_setsockopt(int fd, int level, int option, const char *value,
                         int value_length) {
  SOCKET native_socket = sa_native_socket(fd);
  int result;
  if (native_socket == INVALID_SOCKET)
    return -1;
  result = setsockopt(native_socket, level, option, value, value_length);
  if (result == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  return result;
}

int sa_socket_fcntl(int fd, int command, ...) {
  SOCKET native_socket = sa_native_socket(fd);
  int flags = 0;
  va_list arguments;
  u_long nonblocking;
  if (native_socket == INVALID_SOCKET)
    return -1;
  if (command == F_GETFL) {
    AcquireSRWLockShared(&g_socket_table_lock);
    flags = g_socket_table[fd].nonblocking ? O_NONBLOCK : 0;
    ReleaseSRWLockShared(&g_socket_table_lock);
    return flags;
  }
  if (command != F_SETFL) {
    errno = EINVAL;
    return -1;
  }
  va_start(arguments, command);
  flags = va_arg(arguments, int);
  va_end(arguments);
  nonblocking = (flags & O_NONBLOCK) ? 1UL : 0UL;
  if (ioctlsocket(native_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) {
    sa_set_socket_errno(WSAGetLastError());
    return -1;
  }
  AcquireSRWLockExclusive(&g_socket_table_lock);
  if (g_socket_table[fd].used)
    g_socket_table[fd].nonblocking = nonblocking != 0;
  ReleaseSRWLockExclusive(&g_socket_table_lock);
  return 0;
}
