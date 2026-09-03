#include "systeminc/system.h"
#include "systeminc/netmain.h"
#include "sdk/zlib.h"
#include "systeminc/login.h"
#include "proto/lssproto_cli.h"
#include "proto/protocol.h"
#include "wgs/message.h"
#include "wgs/shop.h"
#include <winsock.h>
#pragma comment(lib, "zlib.lib")
// Nuke 0615: Avoid 7's lock
int isWGS7 = 0;

unsigned int sockfd; // SOCKET FD, 用于描述连接服务器的句柄
int init_net;        //
int server_choosed;  //
char *net_writebuf = NULL;
char *net_readbuf = NULL;
int net_readbuflen;
int net_writebuflen;

BOOL disconnectServerFlag = FALSE;
BOOL oldDisconnectServerFlag = FALSE;

int getServerInfo(int index, char *hostname, short *port) {
  if (index < 0 || index >= sizeof(gmsv) / sizeof(gmsv[0]))
    return -1;
  strcpy(hostname, gmsv[index].ipaddr);
  *port = atoi(gmsv[index].port);
  return 0;
}

char rpc_linebuffer[NETBUFSIZ];

void networkLoop(void) {
  if (init_net == FALSE)
    return;

  if (disconnectServerFlag && !oldDisconnectServerFlag) {
    ChangeProc(PROC_DISCONNECT_SERVER);
  }
  oldDisconnectServerFlag = disconnectServerFlag;
  if (disconnectServerFlag)
    return;

  if (server_choosed == 0)
    return;
  fd_set rfds, wfds, efds;

  struct timeval tm;
  tm.tv_sec = 0;
  tm.tv_usec = 0;

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);

  FD_SET(sockfd, &rfds);
  FD_SET(sockfd, &wfds);
  FD_SET(sockfd, &efds);

  int a = select(2, &rfds, &wfds, (fd_set *)NULL, &tm);
  int len = SOCKET_ERROR;
#ifdef __NEW_CLIENT_MEM
  BOOL ret;
  DWORD oldprotect;
#ifndef __NEW_CLIENT_ONLY_WRITE
  ret = VirtualProtect(net_readbuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);
#endif
#endif
  if (FD_ISSET(sockfd, &rfds)) {
    len = recv(sockfd, rpc_linebuffer, (NETBUFSIZ >> 1) - 1, 0);
    if (isWGS7) {
      if ((len > 1400) && (len <= 1460)) {
        Sleep(500);
        len += recv(sockfd, rpc_linebuffer + len, (NETBUFSIZ >> 1) - 1, 0);
      }
      isWGS7 = 0;
    }
    if (len == SOCKET_ERROR) {
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
        closesocket(sockfd);
        dwServer = NULL;
        // ??????????????
        disconnectServerFlag = TRUE;
      }
    } else
      appendReadBuf(rpc_linebuffer, len);
  }
  while (len != SOCKET_ERROR && net_readbuflen > 0) {
    // get line from read buffer
    if (GS == dwServer) {
      if (!getLineFromReadBuf(rpc_linebuffer, sizeof(rpc_linebuffer))) {

        if (bNewServer)
          SaDispatchMessage(sockfd, rpc_linebuffer);
        else
          lssproto_ClientDispatchMessage(sockfd, rpc_linebuffer);
      } else
        break;
    } else
      ReadWGSMessage(net_readbuf, net_readbuflen);
  }
#ifdef __NEW_CLIENT_MEM
#ifndef __NEW_CLIENT_ONLY_WRITE
  ret = VirtualProtect(net_readbuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
#endif
#endif
  static unsigned int writetime = TimeGetTime();
  if (FD_ISSET(sockfd, &wfds)) {
#ifdef __NEW_CLIENT_MEM
    ret = VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);
#endif
    len = 0;
    if (net_writebuflen)
      len = send(sockfd, net_writebuf, net_writebuflen, 0);
    if (len > 0)
      writetime = TimeGetTime();
    if (len == SOCKET_ERROR) {
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
        closesocket(sockfd);
        dwServer = NULL;
        // ??????????????
        disconnectServerFlag = TRUE;
      }
    } else {
      if (len)
        shiftWriteBuf(len);
    }
#ifdef __NEW_CLIENT_MEM
    ret = VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
#endif
  }
  if ((GS == dwServer) && (writetime + 30 * 1000 < TimeGetTime())) {
    if (init_net == TRUE) {
      if (bNewServer)
        lssproto_Echo_send(sockfd, "hoge");
      else
        old_lssproto_Echo_send(sockfd, "hoge");
    }
  }
}

BOOL initNet(void) {
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(1, 1), &wsadata) != 0)
    return FALSE;
  lssproto_InitClient(appendWriteBuf, NETBUFSIZ, sockfd);
  init_net = TRUE;
  disconnectServerFlag = FALSE;
  oldDisconnectServerFlag = FALSE;
  return TRUE;
}

void cleanupNetwork(void) {
  if (init_net == FALSE)
    return;
  init_net = FALSE;
  server_choosed = 0;
  disconnectServerFlag = FALSE;
  oldDisconnectServerFlag = FALSE;
  closesocket(sockfd);
  dwServer = NULL;
  WSACleanup();
  lssproto_CleanupClient();
}

int appendReadBuf(char *buf, int size) {
#ifdef _NEWSHOP_
  BOOL flg = TRUE;
  while (*(DWORD *)buf ==
         0xC7B3CCC9) { // buf[0]==0xFFFFFFC9 && buf[1]==0xFFFFFFCC &&
                       // buf[2]==0xFFFFFFB3 && buf[3]==0xFFFFFFC7
    ShopClass shop_file[5];
    char patch[256] = {0};
    int id = -1;
    buf += 4;
    char *临时单个道具数据[4] = {0};
    unsigned int 总大小, 压缩大小;
    字符串切割(临时单个道具数据, (char *)buf, "|");
    总大小 = atoi(临时单个道具数据[0]);
    压缩大小 = atoi(临时单个道具数据[1]);
    id = atoi(临时单个道具数据[2]);
    memcpy(shop_file[id].MD5码, 临时单个道具数据[3], 32);
    sprintf_s(patch, "./data/shop%d.bin", id + 1);
    if (shop_file[id].商城数据 != NULL)
      free(shop_file[id].商城数据);
    shop_file[id].商城数据 = (char *)malloc(总大小 + 1);
    memset(shop_file[id].商城数据, 0, 总大小 + 1);
    uLongf lendata = 总大小 - 1;
    uncompress((Bytef *)shop_file[id].商城数据, &lendata, (Bytef *)(buf + 51),
               (unsigned long)压缩大小);
    ShopClasssave(patch, &shop_file[id]);
    memset(buf - 4, 0, 压缩大小 + 55);
    size -= 压缩大小 + 55;
    buf += 压缩大小 + 51;
    flg = FALSE;
  }
  if (!flg)
    return -1;
#endif
  if (server_choosed == 0)
    return -100;

  if ((net_readbuflen + size) > NETBUFSIZ)
    return -1;

  memcpy(net_readbuf + net_readbuflen, buf, size);
  net_readbuflen += size;
  return 0;
}

int appendWriteBuf(int index, char *buf, int size) {
  if (server_choosed == 0)
    return -100;
#ifndef __NEW_CLIENT_MEM
  if ((net_writebuflen + size) > NETBUFSIZ)
    return -1;
  memcpy(net_writebuf + net_writebuflen, buf, size);
  net_writebuflen += size;
#else
  DWORD oldprotect;
  BOOL ret;
  ret = VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_READWRITE, &oldprotect);

  if ((net_writebuflen + size) > NETBUFSIZ)
    return -1;
  memcpy(net_writebuf + net_writebuflen, buf, size);
  net_writebuflen += size;

  ret = VirtualProtect(net_writebuf, NETBUFSIZ, PAGE_NOACCESS, &oldprotect);
#endif
  return 0;
}

int shiftReadBuf(int size) {
  if (server_choosed == 0)
    return -100;
  if (size > net_readbuflen)
    return -1;
  for (int i = size; i < net_readbuflen; i++) {
    net_readbuf[i - size] = net_readbuf[i];
  }
  net_readbuflen -= size;
  return 0;
}

int shiftWriteBuf(int size) {
  int i;

  if (server_choosed == 0)
    return -100;
  if (size > net_writebuflen)
    return -1;
  for (i = size; i < net_writebuflen; i++) {
    net_writebuf[i - size] = net_writebuf[i];
  }
  net_writebuflen -= size;
  return 0;
}

int getLineFromReadBuf(char *output, int maxlen) {
  int i;

  if (server_choosed == 0)
    return -100;
  int j;
  for (i = 0; i < net_readbuflen && i < (maxlen - 1); i++) {
    if (net_readbuf[i] == '\n') {
      memcpy(output, net_readbuf, i);
      output[i] = '\0';
      // ?????????????? 0x0d??????
      for (j = i + 1; j >= 0; j--) {
        if (output[j] == 0x0d) {
          output[j] = '\0';
          break;
        }
      }
      shiftReadBuf(i + 1);
      net_readbuf[net_readbuflen] = '\0';
      return 0;
    }
  }
  return -1;
}

int sendn(SOCKET s, char *buffer, int len) {
  int total = 0;
  int r;
  if (server_choosed == 0)
    return -100;
  while (TRUE) {
    r = send(s, buffer, len, 0);
    if (r == SOCKET_ERROR)
      return SOCKET_ERROR;
    total += r;
    if (total == len)
      return total;
  }
}
