#ifndef __TCP_TOOL_H__
#define __TCP_TOOL_H__

//
#include "version.h"
//
#include "common.h"

// 日志数量？？？
#define BACKLOGNUM 5

/* select 轮询阻塞超时(毫秒), 避免主循环空转烧满 CPU */
#ifndef SAAC_SELECT_TIMEOUT_MS
#define SAAC_SELECT_TIMEOUT_MS 50
#endif

int tcpstruct_write(int ti, const char *buf, int len);

/*
 * 初始化 TCP 连接
 * @param addr: 服务器地址
 * @param port: 服务器端口
 * @param timeout_ms: 超时时间
 * @param mem_use: 内存使用量
 * @param deb: 调试模式
 */
int tcpstruct_init(char *addr, int port, int timeout_ms, int mem_use, int deb);

/*
 * 接受一个连接
 * @return: 连接索引
 */
int tcpstruct_accept1(void);

/*
 * 接受多个连接
 * @param tis: 连接索引数组
 * @param ticount: 连接数
 */
int tcpstruct_accept(int *tis, int ticount);

/*
 * 关闭连接
 * @param ti: 连接索引
 * @return: 关闭连接成功
 */
int tcpstruct_close(const int ti);

/*
 * 读取一行数据
 * @param ti: tcp连接索引
 * @param buf: 数据缓冲区
 * @param len: 读取数据长度限制
 * @param kend_n: 是否去除'\n'
 * @param kend_r: 是否去除'\r'
 * @return: 读取到的数据长度
 */
int tcpstruct_readline(const int ti, char *buf, const int len, const int kend_n,
                       const int kend_r);

/*
 * 读取一行数据并去除行尾
 * @param ti: tcp连接索引
 * @param buf: 数据缓冲区
 * @param len: 数据长度限制
 * @return: 读取到的数据长度
 */
int tcpstruct_readline_chop(const int ti, char *buf, const int len);

/*
 * 计算membuf中有多少链接被使用
 * @return: membuf中被使用的链接数量.
 */
int tcpstruct_countmbuse(void);

/*
 * 连接服务器
 * @param addr: 服务器地址
 * @param port: 服务器端口
 * @return: 建立TCP链接.
 */
int tcpstruct_connect(const char *addr, const int port);

/*
 * 设置非阻塞
 * @param sock: 套接字
 */
void set_nodelay(int sock);

#define TCPSTRUCT_OK 0            /* 调用成功 */
#define TCPSTRUCT_ENOMEM -1       /* 内存分配失败 */
#define TCPSTRUCT_ESOCK -2        /* socket 创建失败 */
#define TCPSTRUCT_EBIND -3        /* bind 调用失败 */
#define TCPSTRUCT_ELISTEN -4      /* listen 调用失败 */
#define TCPSTRUCT_EBUG -6         /* 程序内部错误 */
#define TCPSTRUCT_EINVCIND -7     /* 无效的连接索引 */
#define TCPSTRUCT_EREADFIN -8     /* read finished, due to closed by remote */
#define TCPSTRUCT_EHOST -9        /* gethostbyname 撩   */
#define TCPSTRUCT_ECONNECT -10    /* connect 失败 */
#define TCPSTRUCT_ECFULL -11      /* 链接数达到最大值 */
#define TCPSTRUCT_ETOOLONG -12    /* 数据太长 */
#define TCPSTRUCT_EMBFULL -13     /* 内存缓冲区已满  */
#define TCPSTRUCT_ECLOSEAGAIN -14 /* 重复关闭链接 */

#endif /* __TCP_TOOL_H__ */