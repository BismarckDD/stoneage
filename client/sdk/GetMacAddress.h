#ifndef _GET_MAC_ADDRESS
#define _GET_MAC_ADDRESS

typedef struct MAC_ADDR_t
{
  unsigned char addr[6];
} MAC_ADDR;

extern int GetMACaddress(MAC_ADDR *MacAddr, int MaxGetCount);
extern void PrintMACaddress(unsigned char MACData[]);

#endif
