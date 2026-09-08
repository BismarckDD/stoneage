#include "gmsv_server.h"
//
#include "autil.h"
#include "battle.h"
#include "char.h"
#include "config_file.h"
#include "descrypt.h"
#include "log.h" // for StoneAge
#include "npc_lua_interface.h"
#include "shop.h"
#include "workspace.h"
#include <zlib.h>
#include <net.h>

#define DME() print("<DME(%d)%d:%d>", fd, __LINE__, func)

extern int cliretfunc; // 定义于msignal，用于发生错误时记录环境

char charname[CHARNAMELEN];
char message[1 << 16]; // 64K长度，不应放在栈上
char buffer[1 << 16]; // 64K长度, 放在栈上
char raw[1 << 16];
int check_sum_recv = 0;
int check_sum_calc = 0;
int x, y, dir, dummy;
int request_index, item_index, pet_index, color;

int GmsvServer_ServerDispatchMessage(int fd, char *encoded) {
  /* No client RPC is valid while the account server is authenticating it. */
  if (CONNECT_getState(fd) == WHILEAUTH)
    return -1;
#ifdef _DEFEND_BIGBAO
  if (CONNECT_getState(fd) == NOTLOGIN) {
    if (strlen(encoded) > getBigBao()) {
      close(fd);
      return -1;
    }
  } else {
    if (strlen(encoded) > getBigBao2()) {
      close(fd);
      return -1;
    }
  }
#endif
  int func, fieldcount;
#ifdef _SERVER_DEF
  util_DiscardMessage();
#endif
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
#ifdef _NEW_FUNC_DECRYPT
  if (!util_DecodeMessageTea(raw, encoded)) {
    return -2;
  }
#else
  util_DecodeMessage(raw, encoded);
#endif
  if (!util_SplitMessage(raw, SEPARATOR)) {
    DME();
    return -2;
  }
  if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
    logHack(fd, HACK_GETFUNCFAIL);
    DME();
    return -1;
  }
  cliretfunc = func;
  // check_sum_calc = 0;
  if (func == LSSPROTO_W_RECV) {
    check_sum_calc = 0;
    check_sum_calc += util_deint(2, &x);        // s step
    check_sum_calc += util_deint(3, &y);        // y step
    check_sum_calc += util_destring(4, buffer); // direction
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
    GmsvServer_W_recv(fd, x, y, buffer);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_W2_RECV) {
    check_sum_calc = 0;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_destring(4, buffer);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_W2_RECV-x:%d,y:%d,direction:%s\n", x, y, direction);
#endif
    GmsvServer_W2_recv(fd, x, y, buffer);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_EV_RECV) {
    int event, seqno;
    check_sum_calc = util_deint(2, &event);
    check_sum_calc += util_deint(3, &seqno);
    check_sum_calc += util_deint(4, &x);
    check_sum_calc += util_deint(5, &y);
    check_sum_calc += util_deint(6, &dir);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_EV_RECV-event:%d,seqno:%d,x:%d,y:%d,dir:%d\n", event,
           seqno, x, y, dir);
#endif
    GmsvServer_EV_recv(fd, event, seqno, x, y, dir);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DU_RECV) {
    check_sum_calc = 0;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DU_RECV-x:%d,y:%d\n", x, y);
#endif
    GmsvServer_DU_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_EO_RECV) {
    check_sum_calc = util_deint(2, &dummy);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_EO_RECV-dummy:%d\n", dummy);
#endif
    GmsvServer_EO_recv(fd, dummy);
    util_DiscardMessage();
    return 0;
  }

  if (func == LSSPROTO_BU_RECV) {
    check_sum_calc = util_deint(2, &dummy);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_BU_RECV-dummy:%d\n", dummy);
#endif
    GmsvServer_BU_recv(fd, dummy);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_JB_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_JB_RECV-x:%d,y:%d\n", x, y);
#endif
    GmsvServer_JB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_LB_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_LB_RECV-x:%d,y:%d\n", x, y);
#endif
    GmsvServer_LB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_B_RECV) {
    check_sum_calc = util_destring(2, buffer); // buffer => command
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_B_RECV-command:%s\n", buffer);
#endif
    GmsvServer_B_recv(fd, buffer);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_SKD_RECV) {
    check_sum_calc = util_deint(2, &dir);
    check_sum_calc += util_deint(3, &request_index);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_SKD_RECV-dir:%d,index:%d\n", dir, request_index);
#endif
    GmsvServer_SKD_recv(fd, dir, request_index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_ID_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &item_index);
    check_sum_calc += util_deint(5, &request_index);
    util_deint(6, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_ID_RECV-x:%d,y:%d,item_index:%d,toindex:%d\n", x,
           y, item_index, toindex);
#endif
    GmsvServer_ID_recv(fd, x, y, item_index, request_index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_PI_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &dir);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PI_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
    GmsvServer_PI_recv(fd, x, y, dir);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DI_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &item_index);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DI_RECV-x:%d,y:%d,item_index:%d\n", x, y, item_index);
#endif
    GmsvServer_DI_recv(fd, x, y, item_index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DG_RECV) {
    int amount;
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &amount);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DG_RECV-x:%d,y:%d,amount:%d\n", x, y, amount);
#endif
    GmsvServer_DG_recv(fd, x, y, amount);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DP_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &pet_index);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DP_RECV-x:%d,y:%d,petindex:%d\n", x, y, petindex);
#endif
    GmsvServer_DP_recv(fd, x, y, pet_index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_MI_RECV) {
    check_sum_calc = 0;
    int fromindex;
    int toindex;
    check_sum_calc += util_deint(2, &fromindex);
    check_sum_calc += util_deint(3, &toindex);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_MI_RECV-fromindex:%d,toindex:%d\n", fromindex,
           toindex);
#endif
    GmsvServer_MI_recv(fd, fromindex, toindex);
    util_DiscardMessage();
    return 0;
  }
  // 邮件？
  if (func == LSSPROTO_MSG_RECV) {
    check_sum_calc = util_deint(2, &request_index);
    check_sum_calc += util_destring(3, message);
    check_sum_calc += util_deint(4, &color);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_MSG_RECV-index:%d,message:%s,color:%d\n", request_index,
           message, color);
#endif
    GmsvServer_MSG_recv(fd, request_index, message, color);
    util_DiscardMessage();
    return 0;
  }
  // 宠物邮件?
  if (func == LSSPROTO_PMSG_RECV) {
    check_sum_calc = util_deint(2, &request_index);
    check_sum_calc += util_deint(3, &pet_index);
    check_sum_calc += util_deint(4, &item_index);
    check_sum_calc += util_destring(5, message);
    check_sum_calc += util_deint(6, &color);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PMSG_RECV-index:%d,petindex:%d,item_index:%d,message:"
           "%s,color:%d\n",
           request_index, petindex, item_index, message, color);
#endif
    GmsvServer_PMSG_recv(fd, request_index, pet_index, item_index, message, color);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_AB_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    util_deint(2, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_AB_RECV\n");
#endif
    GmsvServer_AB_recv(fd);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DAB_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int index;
    check_sum_calc += util_deint(2, &index);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DAB_RECV-index:%d\n", index);
#endif
    GmsvServer_DAB_recv(fd, index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_AAB_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int x;
    int y;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_AAB_RECV-x:%d,y:%d\n", x, y);
#endif
    GmsvServer_AAB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_L_RECV) {
    check_sum_calc = util_deint(2, &dir);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_L_RECV-dir:%d\n", dir);
#endif
    GmsvServer_L_recv(fd, dir);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_TK_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int x;
    int y;
    int color;
    int area;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_destring(4, message);
    check_sum_calc += util_deint(5, &color);
    check_sum_calc += util_deint(6, &area);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_TK_RECV-x:%d,y:%d,message:%s,color:%d,area:%d\n", x,
           y, message, color, area);
#endif
    GmsvServer_TK_recv(fd, x, y, message, color, area);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_M_RECV) {
    int fl, x1, y1, x2, y2;
    check_sum_calc = util_deint(2, &fl);
    check_sum_calc += util_deint(3, &x1);
    check_sum_calc += util_deint(4, &y1);
    check_sum_calc += util_deint(5, &x2);
    check_sum_calc += util_deint(6, &y2);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_M_RECV-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d\n", fl, x1, y1,
           x2, y2);
#endif
    GmsvServer_M_recv(fd, fl, x1, y1, x2, y2);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_C_RECV) {
    int index;
    check_sum_calc = util_deint(2, &index);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_C_RECV-index:%d\n", index);
#endif
    GmsvServer_C_recv(fd, index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_S_RECV) {
    check_sum_calc = util_destring(2, buffer); // buffer -> category
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_S_RECV-category:%s\n", category);
#endif
    GmsvServer_S_recv(fd, buffer);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_FS_RECV) {
    int flg;
    check_sum_calc = util_deint(2, &flg);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_FS_RECV-flg:%d\n", flg);
#endif
    GmsvServer_FS_recv(fd, flg);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_HL_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int flg;
    check_sum_calc += util_deint(2, &flg);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_HL_RECV-flg:%d\n", flg);
#endif
    GmsvServer_HL_recv(fd, flg);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_PR_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int x;
    int y;
    int request;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &request);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PR_RECV-x:%d,y:%d,request:%d\n", x, y, request);
#endif
    GmsvServer_PR_recv(fd, x, y, request);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_KS_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int petarray;
    check_sum_calc += util_deint(2, &petarray);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_KS_RECV-petarray:%d\n", petarray);
#endif
    GmsvServer_KS_recv(fd, petarray);
    util_DiscardMessage();
    return 0;
  }
#ifdef _STANDBYPET
  if (func == LSSPROTO_SPET_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int standbypet;
    check_sum_calc += util_deint(2, &standbypet);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_SPET_RECV-standbypet:%d\n", standbypet);
#endif
    GmsvServer_SPET_recv(fd, standbypet);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _RIGHTCLICK
  if (func == LSSPROTO_RCLICK_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int type;
    char data[1024];
    check_sum_calc += util_deint(2, &type);
    check_sum_calc += util_destring(3, data);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_RCLICK_RECV-type:%d,data:%s\n", type, data);
#endif
    GmsvServer_RCLICK_recv(fd, type, data);
    util_DiscardMessage();
    return 0;
  }
#endif
  if (func == LSSPROTO_AC_RECV) {
    check_sum_calc = 0;
    int actionno;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &actionno);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_AC_RECV-x:%d,y:%d,actionno:%d\n", x, y, actionno);
#endif
    GmsvServer_AC_recv(fd, x, y, actionno);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_MU_RECV) {
    check_sum_calc = 0;
    int array;
    int toindex;
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &array);
    check_sum_calc += util_deint(5, &toindex);
    util_deint(6, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_MU_RECV-x:%d,y:%d,array:%d,toindex:%d\n", x, y,
           array, toindex);
#endif
    GmsvServer_MU_recv(fd, x, y, array, toindex);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_PS_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int havepetindex;
    int havepetskill;
    int toindex;
    char data[1024 * 64];

    check_sum_calc += util_deint(2, &havepetindex);
    check_sum_calc += util_deint(3, &havepetskill);
    check_sum_calc += util_deint(4, &toindex);
    check_sum_calc += util_destring(5, data);
    util_deint(6, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PS_RECV-havepetindex:%d,havepetskill:%d,toindex:%d,"
           "data:%s\n",
           havepetindex, havepetskill, toindex, data);
#endif
    GmsvServer_PS_recv(fd, havepetindex, havepetskill, toindex, data);

    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_ST_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int titleindex;
    check_sum_calc += util_deint(2, &titleindex);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_ST_RECV-titleindex:%d\n", titleindex);
#endif
    GmsvServer_ST_recv(fd, titleindex);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_DT_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int titleindex;
    check_sum_calc += util_deint(2, &titleindex);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_DT_RECV-titleindex:%d\n", titleindex);
#endif
    GmsvServer_DT_recv(fd, titleindex);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_FT_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char data[1024 * 64];
    check_sum_calc += util_destring(2, data);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_FT_RECV-data:%s\n", data);
#endif
    GmsvServer_FT_recv(fd, data);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_SKUP_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int skillid;
    check_sum_calc += util_deint(2, &skillid);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_SKUP_RECV-skillid:%d\n", skillid);
#endif
    GmsvServer_SKUP_recv(fd, skillid);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_KN_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int havepetindex;
    char data[1024 * 64];
    check_sum_calc += util_deint(2, &havepetindex);
    check_sum_calc += util_destring(3, data);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_KN_RECV-havepetindex:%d,data:%s\n", havepetindex,
           data);
#endif
    GmsvServer_KN_recv(fd, havepetindex, data);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_WN_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int x;
    int y;
    int seqno;
    int objindex;
    int select;
    char data[1024 * 64];
    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &seqno);
    check_sum_calc += util_deint(5, &objindex);
    check_sum_calc += util_deint(6, &select);
    check_sum_calc += util_destring(7, data);

    util_deint(8, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_WN_RECV-x:%d,y:%d,seqno:%d,objindex:%d,select:%d,"
           "data:%s\n",
           x, y, seqno, objindex, select, data);
#endif
    GmsvServer_WN_recv(fd, x, y, seqno, objindex, select, data);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_SP_RECV) {
    check_sum_calc = util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &dir);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_SP_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
    GmsvServer_SP_recv(fd, x, y, dir);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CLIENTLOGIN_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char cdkey[CDKEYLEN];
    char passwd[PASSWDLEN];
    char mac[512] = "";
    char ip[128];
    int servid = 0;
    strcpy(PersonalKey, _DEFAULT_PKEY);
    check_sum_calc += util_destring(2, cdkey);
    check_sum_calc += util_destring(3, passwd);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc == check_sum_recv) {
#ifdef _DEBUG_RET_CLI
      printf("[接收]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s\n", cdkey,
             passwd);
#endif
    } else {
      check_sum_calc += util_destring(4, mac);
      util_deint(5, &check_sum_recv);
      if (check_sum_calc == check_sum_recv) {
#ifdef _DEBUG_RET_CLI
        printf("[接收]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s,mac:%s\n",
               cdkey, passwd, mac);
#endif
      } else {
        check_sum_calc += util_deint(5, &servid);
        check_sum_calc += util_destring(6, ip);
        util_deint(7, &check_sum_recv);
        if (check_sum_calc != check_sum_recv) {
          util_DiscardMessage();
          logHack(fd, HACK_CHECKSUMERROR);
          DME();
          return -1;
        }
#ifdef _DEBUG_RET_CLI
        printf("[接收]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s,mac:%s,"
               "servid:%d\n",
               cdkey, passwd, mac, servid);
#endif
      }
    }

    if (strlen(cdkey) == 0 || strlen(passwd) == 0) {
      util_DiscardMessage();
      return -1;
    }
    GmsvServer_ClientLogin_recv(fd, cdkey, passwd, mac, servid, ip);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CREATENEWCHAR_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int dataplacenum;
    int imgno;
    int faceimgno;
    int vital;
    int str;
    int tgh;
    int dex;
    int earth;
    int water;
    int fire;
    int wind;
    int hometown;
    check_sum_calc += util_deint(2, &dataplacenum);
    check_sum_calc += util_destring(3, charname);
    check_sum_calc += util_deint(4, &imgno);
    check_sum_calc += util_deint(5, &faceimgno);
    check_sum_calc += util_deint(6, &vital);
    check_sum_calc += util_deint(7, &str);
    check_sum_calc += util_deint(8, &tgh);
    check_sum_calc += util_deint(9, &dex);
    check_sum_calc += util_deint(10, &earth);
    check_sum_calc += util_deint(11, &water);
    check_sum_calc += util_deint(12, &fire);
    check_sum_calc += util_deint(13, &wind);
    check_sum_calc += util_deint(14, &hometown);
    util_deint(15, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CREATENEWCHAR_RECV-dataplacenum:%d,charname:%s,"
           "imgno:%d,faceimgno:%d,vital:%d,str:%d,"
           "tgh:%d,dex:%d,earth:%d,water:%d,fire:%d,wind:%d,hometown:%d\n",
           dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex,
           earth, water, fire, wind, hometown);
#endif
    GmsvServer_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno,
                                vital, str, tgh, dex, earth, water, fire, wind,
                                hometown);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CHARDELETE_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char charname[CHARNAMELEN];
    char passwd[PASSWDLEN];
    check_sum_calc += util_destring(2, charname);
    check_sum_calc += util_destring(3, passwd);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CHARDELETE_RECV-charname:%s\n", charname);
#endif
    GmsvServer_CharDelete_recv(fd, charname, passwd);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_UPSHOP_RECV) {
    int check_sum_recv;
    int check_sum_calc = 0;
    char shop[5][1024] = {0};

    check_sum_calc += util_destring(2, shop[0]);
    check_sum_calc += util_destring(3, shop[1]);
    check_sum_calc += util_destring(4, shop[2]);
    check_sum_calc += util_destring(5, shop[3]);
    check_sum_calc += util_destring(6, shop[4]);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      return -1;
    }
    GmsvServer_upshopdata_recv(fd, shop);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CHARLOGIN_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char charname[CHARNAMELEN];
    check_sum_calc += util_destring(2, charname);
    // 2026.08.24 查看收到的charname原始编码
    // for (size_t i = 0; i < strlen(charname); ++i)
    //   printf("%02X ", (unsigned char)charname[i]);
    // printf("\n");
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CHARLOGIN_RECV-charname:%s\n", charname);
#endif
    GmsvServer_CharLogin_recv(fd, charname);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CHARLIST_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    util_deint(2, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CHARLIST_RECV\n");
#endif
    GmsvServer_CharList_recv(fd);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_CHARLOGOUT_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int Flg = 1;
#ifdef _CHAR_NEWLOGOUT
    check_sum_calc += util_deint(2, &Flg);
    util_deint(3, &check_sum_recv);
#else
    util_deint(2, &check_sum_recv);
#endif
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CHARLOGOUT_RECV-Flg:%d\n", Flg);
#endif
    GmsvServer_CharLogout_recv(fd, Flg);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_PROCGET_RECV) {
    check_sum_calc = 0;
    strcpy(PersonalKey, _DEFAULT_PKEY);
    util_deint(2, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PROCGET_RECV\n");
#endif
    GmsvServer_ProcGet_recv(fd);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_PLAYERNUMGET_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    util_deint(2, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PLAYERNUMGET_RECV\n");
#endif
    GmsvServer_PlayerNumGet_recv(fd);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_ECHO_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char test[1024 * 64];
    check_sum_calc += util_destring(2, test);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_ECHO_RECV-test:%s\n", test);
#endif
    GmsvServer_Echo_recv(fd, test);
    util_DiscardMessage();
    return 0;
  }
  /*
          if (func==LSSPROTO_SHUTDOWN_RECV) {
                  int check_sum_calc=0, check_sum_recv;
                  char passwd[1024 * 64];
                  int min;
                  check_sum_calc += util_destring(2, passwd);
                  check_sum_calc += util_deint(3, &min);
                  util_deint(4, &check_sum_recv);
                  if (check_sum_calc!=check_sum_recv) {
                          util_DiscardMessage();
                          logHack(fd, HACK_CHECKSUMERROR);
                          DME(); return -1;
                  }
  #ifdef _DEBUG_RET_CLI
                  printf("[接收]LSSPROTO_SHUTDOWN_RECV-passwd:%s,min:%d\n",
  passwd,min); #endif
  //		GmsvServer_Shutdown_recv(fd, passwd, min);
                  util_DiscardMessage();
                  return 0;
          }
  */
  if (func == LSSPROTO_TD_RECV) {
    check_sum_calc = util_destring(2, message);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_TD_RECV-message:%s\n", message);
#endif
    GmsvServer_TD_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_FM_RECV) {
    check_sum_calc = util_destring(2, message);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_FM_RECV-message:%s\n", message);
#endif
    GmsvServer_FM_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }

  if (func == LSSPROTO_PETST_RECV) {
    int nPet;
    int sPet;
    check_sum_calc = util_deint(2, &nPet);
    check_sum_calc += util_deint(3, &sPet);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PETST_RECV-nPet:%d,sPet:%d\n", nPet, sPet);
#endif
    GmsvServer_PETST_recv(fd, nPet, sPet);
    util_DiscardMessage();
    return 0;
  }
#ifdef _CHECK_GAMESPEED
  if (func == LSSPROTO_CS_RECV) {
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CS_RECV\n");
#endif
    GmsvServer_CS_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _TEAM_KICKPARTY
  if (func == LSSPROTO_KTEAM_RECV) {
    int sindex;
    check_sum_calc = util_deint(2, &sindex);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_KTEAM_RECV-sindex:%d\n", sindex);
#endif
    GmsvServer_KTEAM_recv(fd, sindex);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _MIND_ICON
  if (func == LSSPROTO_MA_RECV) {
    int nMind;
    check_sum_calc = util_deint(2, &nMind);
    check_sum_calc += util_deint(3, &x);
    check_sum_calc += util_deint(4, &y);
    util_deint(5, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_MA_RECV-x:%d,y:%d,nMind:%d\n", x, y, nMind);
#endif
    GmsvServer_MA_recv(fd, x, y, nMind);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
  if (func == LSSPROTO_CHATROOM_RECV) {
    check_sum_calc = util_destring(2, buffer); // buffer -> test
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_CHATROOM_RECV-test:%s\n", buffer);
#endif
    GmsvServer_CHATROOM_recv(fd, buffer);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
  if (func == LSSPROTO_RESIST_RECV) {
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_RESIST_RECV\n");
#endif
    GmsvServer_RESIST_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
  if (func == LSSPROTO_BATTLESKILL_RECV) {
    int iNum;
    check_sum_calc = util_deint(2, &iNum);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_BATTLESKILL_RECV-iNum:%d\n", iNum);
#endif
    GmsvServer_BATTLESKILL_recv(fd, iNum);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _STREET_VENDOR
  if (func == LSSPROTO_STREET_VENDOR_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    check_sum_calc += util_destring(2, message);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_STREET_VENDOR_RECV-message:%s\n", message);
#endif
    GmsvServer_STREET_VENDOR_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _JOBDAILY
  if (func == LSSPROTO_JOBDAILY_RECV) {
    check_sum_calc = util_destring(2, buffer);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_JOBDAILY_RECV-buffer:%s\n", buffer);
#endif
    GmsvServer_JOBDAILY_recv(fd, buffer);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _TEACHER_SYSTEM
  if (func == LSSPROTO_TEACHER_SYSTEM_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    check_sum_calc += util_destring(2, message);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_TEACHER_SYSTEM_RECV-message:%s\n", message);
#endif
    GmsvServer_TEACHER_SYSTEM_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ADD_STATUS_2
  if (func == LSSPROTO_S2_RECV) {
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_S2_RECV\n");
#endif
    return 0;
  }
#endif
#ifdef _PET_ITEM
  if (func == LSSPROTO_PET_ITEM_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int petindex;
    int x, y;
    int fromindex;
    int toindex;

    check_sum_calc += util_deint(2, &x);
    check_sum_calc += util_deint(3, &y);
    check_sum_calc += util_deint(4, &petindex);
    check_sum_calc += util_deint(5, &fromindex);
    check_sum_calc += util_deint(6, &toindex);
    util_deint(7, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_PET_ITEM_RECV-x:%d,y:%d,petindex:%d,fromindex:%d,"
           "toindex:%d\n",
           x, y, petindex, fromindex, toindex);
#endif
    GmsvServer_PETITEM_recv(fd, x, y, petindex, fromindex, toindex);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ASSESS_ABILITY
  if (func == LSSPROTO_ASSESS_ABILITY_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    char data[256];

    check_sum_calc += util_destring(2, data);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_ASSESS_ABILITY_RECV-data:%s\n", data);
#endif
    // GmsvServer_ASSESS_ABILITY_send( fd,
    // "10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|10000|"
    // );
    GmsvServer_ASSESS_ABILITY_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ONLINE_SHOP
  if (func == LSSPROTO_VIP_SHOP_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int type, page;

    check_sum_calc += util_deint(2, &type);
    check_sum_calc += util_deint(3, &page);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_VIP_SHOP_RECV-x:%d,y:%d\n", type, page);
#endif
    GmsvServer_VIP_SHOP_recv(fd, type, page);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ONLINE_SHOP
  if (func == LSSPROTO_VIP_SHOP_BUY_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int type, page, id, num;

    check_sum_calc += util_deint(2, &type);
    check_sum_calc += util_deint(3, &page);
    check_sum_calc += util_deint(4, &id);
    check_sum_calc += util_deint(5, &num);
    util_deint(6, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_VIP_SHOP_RECV-type:%d,page:%d,id:%d,num:%d\n", type,
           page, id, num);
#endif
    GmsvServer_VIP_SHOP_buy_recv(fd, type, page, id, num);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ONLINE_SHOP
  if (func == LSSPROTO_VIP_SHOP_AMPOINT_BUY_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int type, page, id, num;

    check_sum_calc += util_deint(2, &type);
    check_sum_calc += util_deint(3, &page);
    check_sum_calc += util_deint(4, &id);
    check_sum_calc += util_deint(5, &num);
    util_deint(6, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_VIP_SHOP_RECV-type:%d,page:%d,id:%d,num:%d\n", type,
           page, id, num);
#endif
    GmsvServer_VIP_SHOP_buy_recv(fd, type, page, id, num);
    util_DiscardMessage();
    return 0;
  }
#endif
  // 2026.09.04: 开启/关闭原地遇敌
  if (func == LSSPROTO_SAMENU_RECV) {
    int check_sum_calc = 0, check_sum_recv, index;
    check_sum_calc += util_deint(2, &index); // 从workspace中解析int
    util_deint(3, &check_sum_recv);
    // TODO: 这里是否要校验check_sum_calc和check_sum_recv?
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_SAMENU_RECV. index:%d\n", index);
#endif
    GmsvServer_SaMenu_recv(fd, index);
    util_DiscardMessage();
    return 0;
  }
  if (func == LSSPROTO_SHOPOK_RECV) {
    int escindex, check_sum_recv;
    int check_sum_calc = 0;
    check_sum_calc += util_deint(2, &escindex);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      return -1;
    }
    // 2026.08.23, comment, maybe a error
    // NPC_Lua_NEWSHOP_Recv("ITRF_NEWSHOP", CONNECT_getCharaindex(fd));
    util_DiscardMessage();
    return 0;
  }
#ifdef _FAMILYBADGE_
  if (func == LSSPROTO_FAMILYBADGE_RECV) {
    int check_sum_calc = 0, check_sum_recv;
    int test;
    check_sum_calc += util_deint(2, &test);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[接收]LSSPROTO_FAMILYBADGE_RECV:%s\n", test);
#endif
    GmsvServer_FamilyBadge_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _NEW_TITLE
  if (func == LSSPROTO_CHARTITLE_RECV) {
    int chartitleindex, check_sum_recv;
    int check_sum_calc = 0;
    check_sum_calc += util_deint(2, &chartitleindex);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      DME();
      return -1;
    }
    if (chartitleindex == -1) {
      CHAR_CancelNewTitle(CONNECT_getCharaindex(fd));
    } else {
      CHAR_SetNewTitleUse(CONNECT_getCharaindex(fd), chartitleindex);
    }
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _VISUAL_BEATITUDE
  if (func == LSSPROTO_VB_RECV) {
    int check_sum_recv;
    int check_sum_calc = 0;
    int petindex = 0;
    int type = 0;
    check_sum_calc += util_deint(2, &petindex);
    check_sum_calc += util_deint(3, &type);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      DME();
      return -1;
    }
    FreeVisualBeatitude(CONNECT_getCharaindex(fd),
                        CHAR_getCharPet(CONNECT_getCharaindex(fd), petindex),
                        type);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _RIDEQUERY_
  if (func == LSSPROTO_RIDEQUERY_RECV) {
    int check_sum_recv;
    int check_sum_calc = 0;
    int id = 0;
    check_sum_calc += util_deint(2, &id);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv || id != 1458) {
      util_DiscardMessage();
      DME();
      return -1;
    }
    FreeRideQuery(CONNECT_getCharaindex(fd));
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _RED_MEMOY_
  if (func == LSSPROTO_REDMEMOY_RECV) {
    int check_sum_recv;
    int check_sum_calc = 0;
    int type;
    char str[512];
    check_sum_calc += util_deint(2, &type);
    check_sum_calc += util_destring(3, str);
    util_deint(4, &check_sum_recv);
    if (check_sum_calc != check_sum_recv) {
      util_DiscardMessage();
      DME();
      return -1;
    }
    extern void CHAR_RedMemoy_WindowSend(int charaindex);
    extern void CHAR_RedMemoy_Give(int charaindex, char *data);
    if (type == 1) { // 发送窗体
      CHAR_RedMemoy_WindowSend(CONNECT_getCharaindex(fd));
    } else if (type == 2) { // 领取红包
      CHAR_RedMemoy_Give(CONNECT_getCharaindex(fd), str);
    }
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _CHARSIGNDAY_
  if (func == LSSPROTO_SIGNDAY_RECV) {
    int check_sum_recv;
    int check_sum_calc = 0;
    int id = 0;
    check_sum_calc += util_deint(2, &id);
    util_deint(3, &check_sum_recv);
    if (check_sum_calc != check_sum_recv || id != 1458) {
      util_DiscardMessage();
      DME();
      return -1;
    }
    NPC_Lua_CharSignDay_Recv("CharSignDay", CONNECT_getCharaindex(fd));
    util_DiscardMessage();
    return 0;
  }
#endif
  printf("\n无法找到客户端接口=%d\n", func);
  util_DiscardMessage();
  logHack(fd, HACK_NOTDISPATCHED);
  DME();
  return -1;
}

void GmsvServer_XYD_send(int fd, int x, int y, int dir) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_XYD_SEND-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkint(buffer, x);
  check_sum_calc += util_mkint(buffer, y);
  check_sum_calc += util_mkint(buffer, dir);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}
void GmsvServer_EV_send(int fd, int seqno, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_EV_SEND-seqno:%d,result:%d\n", seqno, result);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkint(buffer, seqno);
  check_sum_calc += util_mkint(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}
void GmsvServer_EN_send(int fd, int result, int field) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_EN_SEND-result:%d,field:%d\n", result, field);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkint(buffer, result);
  check_sum_calc += util_mkint(buffer, field);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}
void GmsvServer_RS_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_RS_SEND-data:%s\n", data);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}
void GmsvServer_RD_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_RD_SEND-data:%s\n", data);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}
void GmsvServer_B_send(int fd, char *command) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  if (strlen(command) == 0) {
    sprintf(command, "FF|");
  }
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_B_SEND-command:%s\n", command);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkstring(buffer, command);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}
void GmsvServer_I_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_I_SEND-data:%s\n", data);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}
//
void GmsvServer_SI_send(int fd, int fromindex, int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_SI_SEND-fromindex:%d,toindex:%d\n", fromindex,
         toindex);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkint(buffer, fromindex);
  check_sum_calc += util_mkint(buffer, toindex);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}
void GmsvServer_MSG_send(int fd, int aindex, char *text, int color) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_MSG_send-aindex:%d,text:%s,color:%d\n", aindex, text,
         color);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, aindex);
  check_sum_calc += util_mkstring(buffer, text);
  check_sum_calc += util_mkint(buffer, color);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}

void GmsvServer_PME_send(int fd, int objindex, int graphicsno, int x, int y,
                       int dir, int flg, int no, char *cdata) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PME_SEND-objindex:%d,graphicsno:%d,x:%d,y:%d,dir:%d,"
         "flg:%d,no:%d,cdata:%s\n",
         objindex, graphicsno, x, y, dir, flg, no, cdata);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, objindex);
  check_sum_calc += util_mkint(buffer, graphicsno);
  check_sum_calc += util_mkint(buffer, x);
  check_sum_calc += util_mkint(buffer, y);
  check_sum_calc += util_mkint(buffer, dir);
  check_sum_calc += util_mkint(buffer, flg);
  check_sum_calc += util_mkint(buffer, no);
  check_sum_calc += util_mkstring(buffer, cdata);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}

void GmsvServer_AB_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_AB_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}
void GmsvServer_ABI_send(int fd, int num, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_ABI_SEND-num:%d,data:%s\n", num, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, num);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}
void GmsvServer_TK_send(int fd, int index, char *message, int color) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_TK_SEND-index:%d,message:%s,color:%d\n", index,
         message, color);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, index);
  check_sum_calc += util_mkstring(buffer, message);
  check_sum_calc += util_mkint(buffer, color);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}
void GmsvServer_MC_send(int fd, int fl, int x1, int y1, int x2, int y2,
                      int tilesum, int objsum, int eventsum, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_MC_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,tilesum:%d,"
         "objsum:%d,eventsum:%d,data:%s\n",
         fl, x1, y1, x2, y2, tilesum, objsum, eventsum, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, fl);
  check_sum_calc += util_mkint(buffer, x1);
  check_sum_calc += util_mkint(buffer, y1);
  check_sum_calc += util_mkint(buffer, x2);
  check_sum_calc += util_mkint(buffer, y2);
  check_sum_calc += util_mkint(buffer, tilesum);
  check_sum_calc += util_mkint(buffer, objsum);
  check_sum_calc += util_mkint(buffer, eventsum);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}

//
void GmsvServer_M_send(int fd, int fl, int x1, int y1, int x2, int y2,
                     char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_M_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,data:%s\n", fl,
         x1, y1, x2, y2, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, fl);
  check_sum_calc += util_mkint(buffer, x1);
  check_sum_calc += util_mkint(buffer, y1);
  check_sum_calc += util_mkint(buffer, x2);
  check_sum_calc += util_mkint(buffer, y2);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}
/* Inspect only the numeric header of the receiver's own C record. Never log
 * the full packet: later fields contain character names and other user data. */
static void LogSelfPlayerImage(int fd, const char *data) {
  int charindex = CONNECT_getCharaindex(fd);
  char objectid[32];
  const char *record;
#ifdef _OBJSEND_C
  const int idfield = 2, grafield = 6;
#else
  const int idfield = 1, grafield = 5;
#endif
  if (!data || !CHAR_CHECKINDEX(charindex))
    return;
  cnv10to62(CHAR_getWorkInt(charindex, CHAR_WORKOBJINDEX), objectid,
            sizeof(objectid));
  for (record = data; *record;) {
    const char *end = strchr(record, ',');
    const char *field = record;
    int n, self = 0;
    if (!end)
      end = record + strlen(record);
    for (n = 0; n <= grafield && field < end; ++n) {
      const char *sep = memchr(field, '|', (size_t)(end - field));
      size_t len;
      if (!sep)
        sep = end;
      len = (size_t)(sep - field);
      if (n == idfield)
        self = len == strlen(objectid) && !memcmp(field, objectid, len);
      if (n == grafield && self) {
        char token[32], *tail;
        long wireGra = 0;
        int valid = 0;
        if (len > 0 && len < sizeof(token)) {
          memcpy(token, field, len);
          token[len] = '\0';
          wireGra = strtol(token, &tail, 10);
          valid = tail != token && *tail == '\0';
        }
        print("[player-image] C-send fd=%d id=%d layout=%s floor=%d pos=(%d,%d) baseGra=%d originalGra=%d wireGra=%ld graTokenBytes=%u numeric=%d\n",
              fd, CHAR_getWorkInt(charindex, CHAR_WORKOBJINDEX),
#ifdef _OBJSEND_C
              "OBJSEND_C",
#else
              "legacy",
#endif
              CHAR_getInt(charindex, CHAR_FLOOR),
              CHAR_getInt(charindex, CHAR_X), CHAR_getInt(charindex, CHAR_Y),
              CHAR_getInt(charindex, CHAR_BASEIMAGENUMBER),
              CHAR_getInt(charindex, CHAR_BASEBASEIMAGENUMBER),
              wireGra, (unsigned)len, valid);
      }
      field = sep < end ? sep + 1 : end;
    }
    record = *end ? end + 1 : end;
  }
}

void GmsvServer_C_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  LogSelfPlayerImage(fd, data);
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_C_SEND-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

void GmsvServer_CA_send(const int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CA_SEND-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

void GmsvServer_CD_send(const int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CD_SEND-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

// R命令
void GmsvServer_R_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_R_SEND-data:%s\n", data);
#endif
  check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

// S命令
void GmsvServer_S_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_S_SEND-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

// D命令
void GmsvServer_D_send(int fd, int category, int dx, int dy, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_D_SEND-category:%d,dx:%d,dy:%d,data:%s\n", category,
         dx, dy, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, category);
  check_sum_calc += util_mkint(buffer, dx);
  check_sum_calc += util_mkint(buffer, dy);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

// FS命令
void GmsvServer_FS_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_FS_SEND-flg:%d\n", flg);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, flg);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}
void GmsvServer_HL_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_HL_SEND-flg:%d\n", flg);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, flg);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}
void GmsvServer_PR_send(int fd, int request, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PR_SEND-request:%d,result:%d\n", request, result);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, request);
  check_sum_calc += util_mkint(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}
#ifdef _PETS_SELECTCON
void GmsvServer_PETS_send(int fd, int petarray, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PETST_SEND-petarray:%d,result:%d\n", petarray, result);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, petarray);
  check_sum_calc += util_mkint(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

// KS命令
void GmsvServer_KS_send(int fd, int petarray, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_KS_SEND-petarray:%d,result:%d\n", petarray, result);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, petarray);
  check_sum_calc += util_mkint(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

// SPET命令
void GmsvServer_SPET_send(int fd, int standbypet, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_SPET_SEND-standbypet:%d,result:%d\n", standbypet,
         result);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, standbypet);
  check_sum_calc += util_mkint(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}
void GmsvServer_PS_send(int fd, int result, int havepetindex, int havepetskill,
                      int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PS_SEND-result:%d,havepetindex:%d,havepetskill:%d,"
         "toindex:%d\n",
         result, havepetindex, havepetskill, toindex);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, result);
  check_sum_calc += util_mkint(buffer, havepetindex);
  check_sum_calc += util_mkint(buffer, havepetskill);
  check_sum_calc += util_mkint(buffer, toindex);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

//
void GmsvServer_SKUP_send(int fd, int point) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_SKUP_SEND-point:%d\n", point);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, point);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}
void GmsvServer_WN_send(int fd, int windowtype, int buttontype, int seqno,
                      int objindex, char *data) {
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_WN_SEND-windowtype:%d,buttontype:%d,seqno:%d,objindex:"
         "%d,data:%s\n",
         windowtype, buttontype, seqno, objindex, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, windowtype);
  check_sum_calc += util_mkint(buffer, buttontype);
  check_sum_calc += util_mkint(buffer, seqno);
  check_sum_calc += util_mkint(buffer, objindex);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
}
void GmsvServer_EF_send(int fd, int effect, int level, char *option) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_EF_SEND-effect:%d,level:%d,char:%s\n", effect, level,
         option);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, effect);
  check_sum_calc += util_mkint(buffer, level);
  check_sum_calc += util_mkstring(buffer, option);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}
void GmsvServer_SE_send(int fd, int x, int y, int senumber, int sw) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_SE_SEND-x:%d,y:%d,senumber:%d,sw:%d\n", x, y, senumber,
         sw);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, x);
  check_sum_calc += util_mkint(buffer, y);
  check_sum_calc += util_mkint(buffer, senumber);
  check_sum_calc += util_mkint(buffer, sw);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}
void GmsvServer_ClientLogin_send(int fd, char *result) {
  if (CONNECT_checkfd(fd) == FALSE) {
    printf("[GMSV回复] fd=%d 无效连接\n", fd);
    return;
  }
  printf("[GMSV回复客户端] fd=%d result='%s'\n", fd, result);
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CLIENTLOGIN_SEND-result:%s\n", result);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
}
void _GmsvServer_CreateNewChar_send(int fd, char *result, char *data, char *file,
                                  int line) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CREATENEWCHAR_SEND-result:%s,data:%s\n", result, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

// CHAR_DELETE
void GmsvServer_CharDelete_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CHARDELETE_SEND-result:%s,data:%s\n", result, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}
void GmsvServer_CharLogin_send(int fd, char *result, char *data) {

  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CHARLOGIN_SEND-result:%s,data:%s\n", result, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

// CHAR LIST: 获取Char列表:2个
void GmsvServer_CharList_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CHARLIST_SEND-result:%s,data:%s\n", result, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

// CHAR LOGOUT
void GmsvServer_CharLogout_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CHARLOGOUT_SEND-result:%s,data:%s\n", result, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, result);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

//
void GmsvServer_ProcGet_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PROCGET_SEND-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

//
void GmsvServer_PlayerNumGet_send(int fd, int logincount, int player) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_PLAYERNUMGET_SEND-logincount:%d,player:%d\n",
         logincount, player);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, logincount);
  check_sum_calc += util_mkint(buffer, player);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

//
void GmsvServer_Echo_send(int fd, char *test) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_ECHO_SEND-test:%s\n", test);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, test);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}
// CoolFish: Trade 2001/4/18
void GmsvServer_TD_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_TD_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}


#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
void GmsvServer_CHATROOM_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_CHATROOM_SEND-message:%s\n", message);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer);
}
#endif
#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
void GmsvServer_RESIST_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_RESIST_SEND-message:%s\n", message);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif
#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
void GmsvServer_BATTLESKILL_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_BATTLESKILL_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer);
}
#endif
#ifdef _FAMILYBADGE_
void GmsvServer_CHAREFFECT_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_BATTLESKILL_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHAREFFECT_SEND, buffer);
}
#endif

//
void GmsvServer_NU_send(int fd, int nu) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_NU_SEND-nu:%d\n", nu);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, nu);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}
void GmsvServer_FM_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_FM_SEND-message:%s\n", message);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}
void GmsvServer_WO_send(int fd, int effect) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_WO_SEND-effect:%d\n", effect);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, effect);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void GmsvServer_IC_send(int fd, int x, int y) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_IC_send-x:%d,y:%d\n", x, y);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, x);
  check_sum_calc += util_mkint(buffer, y);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST // 精灵:沉默
void GmsvServer_NC_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_NC_SEND-flg:%d\n", flg);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, flg);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif
#ifdef _STREET_VENDOR
void GmsvServer_STREET_VENDOR_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSSTREET_VENDOR_SEND-message:%s\n", message);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, message);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_STREET_VENDOR_SEND, buffer);
}
#endif
#ifdef _RIGHTCLICK
void GmsvServer_RCLICK_send(int fd, int type, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_RCLICK_SEND-type:%d,data:%s\n", type, data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  print("\n RCLICK_send( type=%d data=%s) ", type, data);
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, type);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_RCLICK_SEND, buffer);
}
#endif
#ifdef _JOBDAILY
void GmsvServer_JOBDAILY_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_JOBDAILY_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_JOBDAILY_SEND, buffer);
}
#endif
#ifdef _TEACHER_SYSTEM
void GmsvServer_TEACHER_SYSTEM_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_TEACHER_SYSTEM_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_TEACHER_SYSTEM_SEND, buffer);
}
#endif
#ifdef _ADD_STATUS_2
void GmsvServer_S2_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_S2_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_S2_SEND, buffer);
}
#endif
#ifdef _ONLINE_SHOP
void GmsvServer_VIP_SHOP_send(int fd, int num, int BJ, int type, int shoppage,
                            int page, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_VIP_SHOP_send-num:%d,BJ:%d,type:%d,shoppage:%d,page:%"
         "d,data:%s\n",
         num, BJ, type, shoppage, page, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, num);      // 有效数目
  check_sum_calc += util_mkint(buffer, BJ);       // 贝币数目
  check_sum_calc += util_mkint(buffer, shoppage); // 显示页面
  check_sum_calc += util_mkint(buffer, page);     // 总共页数
  check_sum_calc += util_mkint(buffer, type);     // 显示类别
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_VIP_SHOP_SEND, buffer);
}
#endif
#ifdef _ASSESS_ABILITY
void GmsvServer_ASSESS_ABILITY_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_ASSESS_ABILITY_send-data:%s\n", data);
#endif
  char buffer[65500];
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_ASSESS_ABILITY_SEND, buffer);
}
#endif

// 地牢相关的请求
void GmsvServer_DENGON_send(int fd, char *data, int color, int num) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_DENGON_send-data:%s,%d,%d\n", data, color, num);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  check_sum_calc += util_mkint(buffer, color);
  check_sum_calc += util_mkint(buffer, num);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_DENGON_SEND, buffer);
}

//
void GmsvServer_SAMENU_send(int fd, int index, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_SAMENU_send-data:%d,%s\n", index, data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkint(buffer, index);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SAMENU_SEND, buffer);
}

void GmsvServer_UpShopData_send(int fd, char *data, char *md5, int id) {
  unsigned long len = strlen(data);
  unsigned long comprLen = 1024 * 100 - 55;
  char compr[1024 * 100] = {0};
  compress(compr + 55, &comprLen, data, len);
  snprintf(compr, sizeof(compr), "商城%d|%d|%d|%s", len, comprLen, id, md5);
  fd_set rfds, wfds, efds;
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);
  FD_SET(fd, &rfds);
  FD_SET(fd, &wfds);
  FD_SET(fd, &efds);
  struct timeval tmv;
  tmv.tv_sec = tmv.tv_usec = 0;
  int ret = select(fd + 1, &rfds, &wfds, &efds, &tmv);
  if (ret > 0 && FD_ISSET(fd, &wfds)) {
    ret = send(fd, compr, comprLen + 55, 0);
    if (ret == -1 && errno != EINTR) {
#ifdef _NETLOG_
      char cdkey[16];
      char charname[32];
      CONNECT_getCharname(CONNECT_getCharaindex(fd), charname, 32);
      CONNECT_getCdkey(CONNECT_getCharaindex(fd), cdkey, 16);
      char token[128];
      sprintf(token, "商城 send T人 ret=%d  errno=%s", ret, strerror(errno));
      LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif
      CONNECT_endOne_debug(fd);
    }
  } else if (ret < 0 && errno != EINTR) {
#ifdef _NETLOG_
    char cdkey[16];
    char charname[32];
    CONNECT_getCharname(CONNECT_getCharaindex(fd), charname, 32);
    CONNECT_getCdkey(CONNECT_getCharaindex(fd), cdkey, 16);
    char token[128];
    sprintf(token, "商城 select T人 ret=%d  errno=%s", ret, strerror(errno));
    LogCharOut(charname, cdkey, __FILE__, __FUNCTION__, __LINE__, token);
#endif
    CONNECT_endOne_debug(fd);
  }
}
void GmsvServer_ShopOK_send(int fd) {
  strcpy(buffer, "");
  int check_sum_calc = 0;
  check_sum_calc += util_mkint(buffer, 0);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_SHOPOK_SEND, buffer);
}

// 家族勋章
#ifdef _FAMILYBADGE_
void GmsvServer_FamilyBadge_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_FamilyBadge_send-data:%s\n", data);
#endif
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_FAMILYBADGE_SEND, buffer);
}
#endif

// 头衔
#ifdef _NEW_TITLE
void GmsvServer_CharTitle_send(int fd, char *data) // 发送新数据
{
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_CHARTITLE_SEND, buffer);
  char msg[256];
  int fd_charaindex = CONNECT_getCharaindex(fd);
  sprintf(msg, "4|%d", CHAR_getInt(fd_charaindex, CHAR_TITLE_DEFAULT));
  GmsvServer_CHAREFFECT_send(getfdFromCharaIndex(fd_charaindex), msg);
}
#endif

// 虚拟
#ifdef _VISUAL_BEATITUDE
void GmsvServer_VisualBeatitude_send(int fd, char *data) {
  int check_sum_calc = 0;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_VB_SEND, buffer);
}
#endif

//
#ifdef _RED_MEMOY_
void GmsvServer_RedMemoy_send(int fd, int type, int time, int vip, char *name,
                            int index) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  check_sum_calc = util_mkint(buffer, type);
  check_sum_calc += util_mkint(buffer, time);
  check_sum_calc += util_mkint(buffer, vip);
  check_sum_calc += util_mkstring(buffer, name);
  check_sum_calc += util_mkint(buffer, index);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_REDMEMOY_SEND, buffer);
}
#endif

// 
#ifdef _MOVE_SCREEN
void GmsvServer_MoveScreen_send(int fd, BOOL bMoveScreenMove, int iXY) {
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_MOVESCREEN_SEND-fd:%d,bMoveScreenMove:%d,iXY:%d\n", fd,
         bMoveScreenMove, iXY);
#endif
  buffer[0] = '\0'; // 2026.09.08 豆包推荐这种写法
  CONNECT_getCdkey(fd, PersonalKey, 4096);
#ifdef _NEW_ENCRYT
  CONNECT_catRunKey(fd, PersonalKey);
#else
  strcat(PersonalKey, _RUNNING_KEY);
#endif
  icheck_sum_calc += util_mkint(buffer, bMoveScreenMove);
  icheck_sum_calc += util_mkint(buffer, iXY);
  util_mkint(buffer, icheck_sum_calc);
  util_SendMesg(fd, LSSPROTO_MOVE_SCREEN_SEND, buffer);
}
#endif
#ifdef _THEATER
void GmsvServer_TheaterData_send(int fd, char *pData) {
  int icheck_sum_calc = 0;
  char szBuffer[65500];
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_TheaterData_SEND-fd:%d,pData:%s\n", fd, pData);
#endif
  strcpy(szBuffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
#ifdef _NEW_ENCRYT
  CONNECT_catRunKey(fd, PersonalKey);
#else
  strcat(PersonalKey, _RUNNING_KEY);
#endif
  icheck_sum_calc += util_mkstring(szBuffer, pData);
  util_mkint(szBuffer, icheck_sum_calc);
  util_SendMesg(fd, LSSPROTO_THEATER_DATA_SEND, szBuffer);
}
#endif
#ifdef _NPC_MAGICCARD
void GmsvServer_MagiccardAction_send(int fd, char *data) {
  char buffer[65500];
  int check_sum_calc = 0;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTO_MagiccardAction_SEND-fd:%d,data:%s\n", fd, data);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
#ifdef _NEW_ENCRYT
  CONNECT_catRunKey(fd, PersonalKey);
#else
  strcat(PersonalKey, _RUNNING_KEY);
#endif
  check_sum_calc += util_mkstring(buffer, data);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_MAGICCARD_ACTION_SEND, buffer);
}
void GmsvServer_MagiccardDamage_send(int fd, int position, int damage,
                                   int offsetx, int offsety) {
#ifdef _DEBUG_SEND_CLI
  printf("[发送]LSSPROTOMagiccardDamage_SEND-fd:%d,position:%d,damage:%d,"
         "offsetx:%d,offsety:%d\n",
         fd, position, damage, offsetx, offsety);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
#ifdef _NEW_ENCRYT
  CONNECT_catRunKey(fd, PersonalKey);
#else
  strcat(PersonalKey, _RUNNING_KEY);
#endif
  check_sum_calc = util_mkint(buffer, position);
  check_sum_calc += util_mkint(buffer, damage);
  check_sum_calc += util_mkint(buffer, offsetx);
  check_sum_calc += util_mkint(buffer, offsety);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_MAGICCARD_DAMAGE_SEND, buffer);
}
#endif
#ifdef _NPC_DANCE
void GmsvServer_DancemanOption_send(int fd, int option) {
  int check_sum_calc = 0;
#ifdef _DEBUG_SEND_CLI
  printf("[发送]GmsvServer_DancemanOption_send-fd:%d,option:%d\n", fd, option);
#endif
  strcpy(buffer, "");
  CONNECT_getCdkey(fd, PersonalKey, 4096);
#ifdef _NEW_ENCRYT
  CONNECT_catRunKey(fd, PersonalKey);
#else
  strcat(PersonalKey, _RUNNING_KEY);
#endif
  check_sum_calc += util_mkint(buffer, option);
  util_mkint(buffer, check_sum_calc);
  util_SendMesg(fd, LSSPROTO_DANCEMAN_OPTION_SEND, buffer);
}
#endif
