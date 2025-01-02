#pragma once

void 显示道具数据(short x, short y, UCHAR 层次, char *内容, unsigned int 形像,
                  char *名字);


#ifdef _NEWSHOP_
struct ShopClass {
  char MD5码[64];
  char *商城数据;
};

void ShopClasssave(char *路径, ShopClass *商城);

int 字符串切割(char *arr[], char *str, const char *del);
#endif
