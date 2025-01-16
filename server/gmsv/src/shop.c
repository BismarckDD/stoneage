#include "version.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "buf.h"
#include "shop.h"

typedef struct tagShopData{
	char shopMd5[64];
	char *shopData;
} ShopData;

ShopData gShopData[5];

void ShopData_Init()
{
	memset(&gShopData,0,sizeof(ShopData)*5);
}

void WriteShopData(char *data,int id)
{
	char szDigest[17];
	int len=strlen(data)+1;
	if(gShopData[id].shopData){
		freeMemory(gShopData[id].shopData);
		gShopData[id].shopData=NULL;
	}
	if(*data!=0){
		gShopData[id].shopData = (char*)allocateMemory(len);
		memset(gShopData[id].shopData,0,len);
		memcpy(gShopData[id].shopData,data,len);
		// MD5Digest(gShopData[id].shopData,len-1,szDigest);
		szDigest[16]='\0';
		int i ;
		for(i=0;i<16;i++)
		{
			snprintf(gShopData[id].shopMd5+i*2,sizeof(gShopData[id].shopMd5+i*2),"%02X",(unsigned char)szDigest[i]);
		}
		gShopData[id].shopMd5[32]='\0';
	}else{
		gShopData[id].shopMd5[0]=0;
	}
}


void lssproto_upshopdata_recv(int fd,char shop[5][1024])
{
	int i=0;
	for(i;i<5;i++){
		if(*gShopData[i].shopMd5){
			if(strcmp(gShopData[i].shopMd5,shop[i]) || !shop[i]){
				if(gShopData[i].shopData){
					lssproto_UpShopData_send(fd,gShopData[i].shopData,gShopData[i].shopMd5,i);
				}
			}
		}
	}
	lssproto_ShopOK_send(fd);
}

