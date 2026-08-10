#ifndef __SASQL_H__
#define __SASQL_H__

#include "version.h"
//
#include "common.h"

#ifdef _SASQL

void sasql_close(void);
BOOL sasql_init(void);
int sasql_query(char *nm, char *pas);
BOOL sasql_update(char *nm, char *path);
BOOL sasql_logindate(char *nm);
BOOL sasql_online(char *ID, char *NM, char *MAC, char *IP, int flag);
BOOL sasql_register(const char *id, const char *ps);
BOOL sasql_craete_userinfo(void);
// 创建锁表
BOOL sasql_craete_lock(void);
// 检查锁
BOOL sasql_check_lock(char *idip);
// 添加锁
BOOL sasql_add_lock(char *idip);
// 删除锁
BOOL sasql_del_lock(char *idip);
// 在线人数
int sasql_onlinenum(char *MAC);
#ifdef _NEW_VIP_SHOP
// 添加vip点数
BOOL sasql_add_vippoint(char *ID, int point);
// 查询vip点数
int sasql_query_point(char *name);
#endif
#ifdef _ITEM_PET_LOCKED
// 查询宠物密码
char *sasql_ItemPetLocked(char *id, char *safepasswd);
// 修改宠物密码
char *sasql_ItemPetLocked_Passwd(char *id, char *safepasswd);
// 修改宠物密码
BOOL sasql_ItemPetLocked_Char(char *id, char *safepasswd);
#endif
#ifdef _ONLINE_COST
char *sasql_OnlineCost(char *id, char *costpasswd, int fmindex, char *fmname);
char *sasql_TransOnlineCost(void);
void sasql_OnlineCost_add(int cost, int num, int point);
#endif
#ifdef _SQL_BUY_FUNC
char *sasql_OnlineBuy(char *id, char *costpasswd);
void sasql_OnlineBuy_add(char *coststr, int type, int num);
#endif
#ifdef _FORMULATE_AUTO_PK
BOOL sasql_add_FormulateAutoPk(char *ID, int point);
#endif
#ifdef _OLDPS_TO_MD5PS
void sasql_OldpsToMd5ps(void);
#endif

void sasql_CleanCdkey(int date);
void sasql_CleanLockCdkey(void);

#endif // _SASQL

#endif // __SASQL_H__
