#ifndef __ADDRESSBOOK_H__
#define __ADDRESSBOOK_H__

#include "common.h"
#include "util.h"
#include "net.h"

#define ADDRESSBOOK_MAX 80

typedef struct
{
  int use;
  BOOL online;               // is online?
  int level;                 // level
  int duelpoint;             //
  int graphicsno;            // Profile Graph.
  char cdkey[CDKEYLEN];      // CD-KEY
  char charname[CHARNAMELEN];// 游戏名称.
  int transmigration;        // 转生.
} ADDRESSBOOK_entry;

char *ADDRESSBOOK_makeAddressbookString( ADDRESSBOOK_entry *a );
BOOL ADDRESSBOOK_makeAddressbookEntry( char *in , ADDRESSBOOK_entry *a );
BOOL ADDRESSBOOK_deleteEntry(int meindex, int index);
BOOL ADDRESSBOOK_addEntry(int meindex);
BOOL ADDRESSBOOK_sendAddressbookTable(int cindex);
BOOL ADDRESSBOOK_sendAddressbookTableOne(int cindex, int num);
BOOL ADDRESSBOOK_sendMessage(int cindex, int aindex, char *text,
                             int color );
BOOL ADDRESSBOOK_sendMessage_FromOther(char *fromcdkey, char *fromcharaname,
     char *tocdkey, char *tocharaname, char* text , int color );

void ADDRESSBOOK_notifyLoginLogout(int cindex, int flg);
void ADDRESSBOOK_addAddressBook( int meindex, int toindex);
void ADDRESSBOOK_DispatchMessage( char *cd, char *nm, char *value, int mode);
int ADDRESSBOOK_getIndexInAddressbook(int cindex , char *cdkey, char *charname);
BOOL ADDRESSBOOK_AutoaddAddressBook( int meindex, int toindex);

#endif
