#include "version.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <setjmp.h>
#include "init.h"
#include "net.h"
#ifdef _EPOLL_ET_MODE
#include "epollnet.h"
#endif
#include "handletime.h"
#include "char.h"
#include "map_warppoint.h"
#include "npcgen.h"
#include "log.h"
#include "configfile.h"
#include "battle.h"
#include "petmail.h"
#include "autil.h"
#include "family.h"
#include "trade.h"
#include "longzoro/attestation.h"
#include "longzoro/sasql.h"
#ifdef _ALLBLUES_LUA
#include "mylua/ablua.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (���ɿ�) Syu ADD ������Ƶ��
#include "chatroom.h"
#endif
#include "shop.h"
jmp_buf mark;

static void ShutdownProc( void);
void mainloop( void );
struct tm tmNow, tmOld;
void family_proc();

#ifdef _CHANNEL_MODIFY
extern int InitOccChannel(void);
#endif

#ifdef _GMSV_DEBUG
extern char *DebugMainFunction;
#endif

#ifdef _ANGEL_SUMMON
#define ANGELTIMELIMIT	3*24*60*60 // �������ʱ��(��)
int AngelReady =0;
int AngelTimelimit = ANGELTIMELIMIT;
time_t AngelNextTime;
void AngelReadyProc();
#endif

void warplog_proc();

int main(int argc ,char** argv, char** env)
{
  setNewTime();
  ShopData_Init();
  EXITWITHEXITCODEIFFALSE(util_Init(), 1);

  LoadAnnounce();	// Arminius 7.12 loginannounce

	memcpy(&tmOld, localtime((time_t *)&NowTime.tv_sec), sizeof(tmNow));

  EXITWITHEXITCODEIFFALSE(init(argc, argv, env), 1);

  LoadPetTalk();	// Arminius 8.14 pet talk

#ifdef _GAMBLE_BANK
	Load_GambleBankItems();
#endif

#ifdef _CFREE_petskill
	Load_PetSkillCodes();
#endif

#ifdef _ITEM_PILEFORTRADE
	TRADE_InitTradeList();
#endif
		
#ifdef _SASQL
    if(sasql_init()==FALSE){
    	exit(1);
    }
#endif

    mainloop();
    
    return 0;
}

void mainloop(void)
{	
  print("��ʼ��NPC...");
  NPC_generateLoop( 1 );
  print("���\n");
  print("��ʼ��signal1...");
  signal(SIGUSR1,sigusr1);
  print("���\n");
  print("��ʼ��signal2...");
  signal(SIGUSR2,sigusr2);
  print("���\n");

#ifdef _MAP_WARPPOINT
	print("��ʼ����ͼ���͵�...");
	MAPPOINT_InitMapWarpPoint();
	print("���\n");
	if( !MAPPOINT_loadMapWarpPoint() ){
		return;
	}
#endif

#ifdef _ASSESS_SYSEFFICACY
	Assess_InitSysEfficacy();
#endif
#ifdef _CHATROOMPROTOCOL			// (���ɿ�) Syu ADD ������Ƶ��
	print("��ʼ��������Ƶ...");
	InitChatRoom();
	print("���\n");
#endif
#ifdef _CHAR_PROFESSION			// WON ADD ����ְҵ
#ifdef _CHANNEL_MODIFY
	print("��ʼ��ְҵƵ��...");
	if(!InitOccChannel()) return;			// ��ʼ��ְҵƵ��
	print("���\n");
#endif
#endif
#ifdef _ANGEL_SUMMON
	print("��ʼ��������ۼʱ��...");
	AngelReady = 0;
#ifdef _ANGEL_TIME
	AngelNextTime = time(NULL) + getAngelPlayerTime();
#else
	AngelNextTime = time(NULL) + 1*60;
#endif
	print("���\n");
#endif

//#ifdef _ALLDOMAN
//	print("��ʼ��Ӣ�۱���...");
//	InitHeroList();
//	print("���\n");
//#endif
#ifdef _JZ_NEWSCRIPT_LUA
	print("��ʼ��LNS����...");
	NPC_Lua_Init(getLuaFile());
	print("���\n");
#endif
#ifdef _ALLBLUES_LUA
	print("��ʼ��LUA�ű�...");
  LoadAllbluesLUA("data/ablua");
  print("���\n");
#endif

#ifdef _EPOLL_ET_MODE
	//������������߳�
  print("������������߳�...");
	if(Start_PacketWrapper() != 0 )
	{
		print("ʧ��\n");
		return;
	}
	print("���\n");

//	//�������ͷ���߳�
//  print("�������ͷ���߳�...");
//	if(Start_Epoll_SendLoop() != 0 )
//	{
//		print("ʧ��\n");
//		return;
//	}
//	print("���\n");

	//����epoll��ѭ���߳�
	print("����epoll��ѭ���߳�...");
	if( Start_Epoll_Loop() != 0 )
	{
		print("ʧ��\n");
		return;
	}
	print("���\n");
#endif

	while(1){
#ifdef _ASSESS_SYSEFFICACY
Assess_SysEfficacy( 0 );
#endif

    setNewTime();
    memcpy(&tmNow, localtime( (time_t *)&NowTime.tv_sec),
               sizeof( tmNow ) );
		if( tmOld.tm_hour != getLogHour( ) && tmNow.tm_hour == getLogHour( ) ){

#ifdef _GMSV_DEBUG
			DebugMainFunction="backupAllLogFile";
#endif
			backupAllLogFile( &tmOld );
		}
    
    setNewTime();
#ifdef _ASSESS_SYSEFFICACY_SUB //��ʾLOOPʱ��

Assess_SysEfficacy_sub( 0, 1);
#ifdef _GMSV_DEBUG
	  DebugMainFunction="netloop_faster";
#endif

#ifdef _EPOLL_ET_MODE
	  System_Loop();
#else
	  netloop_faster();
#endif

Assess_SysEfficacy_sub( 1, 1);
Assess_SysEfficacy_sub( 0, 2);
#ifdef _GMSV_DEBUG
    DebugMainFunction="NPC_generateLoop";
#endif
    NPC_generateLoop( 0 );
Assess_SysEfficacy_sub(1, 2);

Assess_SysEfficacy_sub(0, 3);
#ifdef _GMSV_DEBUG
    DebugMainFunction="BATTLE_Loop";
#endif
    BATTLE_Loop();
Assess_SysEfficacy_sub(1, 3);

Assess_SysEfficacy_sub(0, 4);
#ifdef _GMSV_DEBUG
   DebugMainFunction="CHAR_Loop";
#endif
   CHAR_Loop();
Assess_SysEfficacy_sub( 1, 4);

#ifdef _GMSV_DEBUG
   DebugMainFunction="PETMAIL_proc";
#endif
   PETMAIL_proc();

#ifdef _GMSV_DEBUG
   DebugMainFunction="family_proc";
#endif
   family_proc();

#ifdef _GMSV_DEBUG
   DebugMainFunction="chardatasavecheck";
#endif
   chardatasavecheck();
#ifdef _ANGEL_SUMMON
#ifdef _GMSV_DEBUG
		DebugMainFunction="AngelReadyProc";
#endif
		AngelReadyProc();
#endif
		tmOld = tmNow;
		if( tmOld.tm_sec != tmNow.tm_sec ) {
#ifdef _GMSV_DEBUG
			DebugMainFunction="CHAR_checkEffectLoop";
#endif
			CHAR_checkEffectLoop();
		}
    if( SERVSTATE_getShutdown()> 0 ) {
      ShutdownProc();
#ifdef _GMSV_DEBUG
      DebugMainFunction="ShutdownProc";
#endif
    }
		tmOld = tmNow;

#ifdef _ASSESS_SYSEFFICACY
Assess_SysEfficacy( 1 );
#endif
#endif
  }
}

static void sendmsg_toall( char *msg )
{
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
		}
	}
}
static void ShutdownProc( void)
{
#define		SYSINFO_SHUTDOWN_MSG		"%d ���Ӻ�ʼ����ϵͳͣ��ά��, ������ǰ�����������ݶ�ʧ��"
#define		SYSINFO_SHUTDOWN_MSG_COMP	"�������ѹرա�"
	int diff,hun;

	diff = NowTime.tv_sec - SERVSTATE_getShutdown();
	hun = SERVSTATE_getLimittime() - (diff/60);
	if( hun != SERVSTATE_getDsptime() ){
		char	buff[256];
		if( hun != 0 ) {
			snprintf( buff, sizeof(buff), SYSINFO_SHUTDOWN_MSG, hun);
		}
		else {
			strcpy( buff, SYSINFO_SHUTDOWN_MSG_COMP);
		}
		sendmsg_toall( buff);
		SERVSTATE_setDsptime(hun);
		if( hun == 1 ) {
		    SERVSTATE_SetAcceptMore( 0 );
		}
	}
	/* closesallsockets */
	if( hun == 0) {
    	closeAllConnectionandSaveData();
		SERVSTATE_setShutdown(0);
		SERVSTATE_setDsptime(0);
		SERVSTATE_setLimittime(0);
	}
	
}

void family_proc()
{
	static	unsigned long gettime = 0;
	static  unsigned long checktime = 0;
	static  unsigned long proctime = 0;

	if( time(NULL) < proctime ) return;
	proctime = time(NULL)+5;

	if( (unsigned long)NowTime.tv_sec > gettime ){
		getNewFMList();
		gettime = (unsigned long)NowTime.tv_sec + 60*10;
	}

	if( (unsigned long)NowTime.tv_sec > checktime ){
		checkFamilyIndex();
		checktime = (unsigned long)NowTime.tv_sec + 60*30;
	}
}

void warplog_proc()
{
	static  unsigned long checktime = 0;
	
	if( (unsigned long)NowTime.tv_sec > checktime ){
		warplog_to_file();
		checktime = (unsigned long)NowTime.tv_sec + 300;
	}
}

#ifdef _ANGEL_SUMMON

extern int player_online;

void AngelReadyProc()
{
	//static time_t lastCreateTime = time(NULL);
	time_t nowTime;
	//static unsigned long AngelNextTime = 30*60;
	struct tm *temptime;
	char msg[1024];

	nowTime = time(NULL);

	if( nowTime < AngelNextTime )
		return;
#ifdef _ANGEL_TIME
	if( player_online <= getAngelPlayerMun() )
#else
	if( player_online <= 10 )
#endif
	{
//		print("\n�����ٻ�:������������=%d\n",	player_online );
		return;
	}

	AngelReady = 1;
	//AngelNextTime = min( (int)(10000/player_online), 100)*60 + (unsigned long)nowTime;
#ifdef _ANGEL_TIME
	AngelNextTime = min( (int)(getAngelPlayerTime()/player_online), 100)*60 + (unsigned long)nowTime;
#else
	AngelNextTime = min( (int)(5000/player_online), 100)*60 + (unsigned long)nowTime;
#endif

	temptime = localtime( &AngelNextTime );
	sprintf( msg, "\n�����ٻ�:����һλȱ��  �´β���ʱ��=(%d/%d %d:%d) Ŀǰ����=%d\n",
		temptime->tm_mon+1, temptime->tm_mday, temptime->tm_hour, temptime->tm_min, player_online );
	print( msg);
	//LogAngel( msg);
}

#endif


