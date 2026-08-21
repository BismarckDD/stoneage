#ifndef __SAAC_COMMON_H__ 
#define __SAAC_COMMON_H__

//
#include "version.h"
//
#include "common.h"

//
#define USERID_MAX 32 // 用户ID的最大长度
#define CHARNAME_MAX 32 // 角色名称的最大长度
#define TEXT_MAX 1024 // MAIL文本的最大长度

#define MAXCONNECTION 20
#define MAIL_EXPIRE_TIME 3600
#define MAX_FLUSH_MAIL 1024

#define MAX_FAMILY 1000 // 1000
#define MAX_MEMBERNUM FAMILY_MAX_MEMBER
#define MIN_MEMBERNUM 5 // 10
#ifdef _FIX_10_FMPOINT  // WON ADD 九大庄园
#define MAX_FMPOINT 2
#else
#define MAX_FMPOINT 4 // 4
#endif

#define PREDEL_TIME 60 * 60 * 24 * 7        // 七天 60*60*24*3
#define PREDELMEMBER_TIME 60 * 60 * 24 * 60 // 30天 60*60*24*30
#define FAMILY_MEMONUM 35                   // 家族留言板最大留言数量
#define FAMILY_MEMOLEN 256
#define FMSMEMOINDEX 10000 // 家族之间留言板 index
#define FMSMEMONUM 140
#define TOPFMLISTNUM 10 // 家族排行榜数量
#define MINFMPOPNUM -20000000
// shan 20011207
#define MAXFMPOPNUM 100000000
#define MAXFMMOMENTUM 10000000

// #define MAXFMPOPNUM 5000000
#define MINAPPLYPNUM 10 // 申请家族最低人数
#define FMMEMBERINDEX "fmmember"
#define FMMEMOLIST "memo"
#define FMELDERNUM 5         // 长老数量
#define FMMAXGOLD 1000000000 // 家族基金上限

// shan 20011207
#define MAXRECVPOP 5000000 // 家族声望转移上限
// CoolFish: 2001/11/26
// #define MAXRECVPOP			2500000		// 家族声望转移上限
#endif // __SAAC_COMMON_H__
