#include "version.h"
#include <ctype.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "item.h"
#include "readmap.h"
#include "enemy.h"
#include "chatmagic.h"
#include "log.h"

#define EVFLG 118 // 58->白虎  118->青龙

void pet_make(int char_index, char* message);

BOOL NPC_PetMakerInit(int meindex)
{
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPEPETMAKER);
  CHAR_setFlg(meindex, CHAR_ISATTACKED, 0);
  return TRUE;
}

void NPC_PetMakerTalked(int meindex, int talker, char *msg, int color)
{
    char token[32], tmpbuf[256];
    int offset  = EVFLG / 32;
    int shift   = EVFLG % 32;
    int ckpoint = CHAR_getInt(talker, CHAR_ENDEVENT + offset);
    // 好像没啥用.
    const char *npcarg = CHAR_getChar(meindex, CHAR_NPCARGUMENT);
    getStringFromIndexWithDelim(npcarg, "|", 1, token, sizeof(token));
    const int msg_no = atoi(token);

    if (CHAR_getInt(talker, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)  
        return;

    if (NPC_Util_isFaceToFace(talker, meindex, 2) == FALSE) 
    {
      if (NPC_Util_isFaceToFace(talker, meindex, 1) == FALSE) 
          return;
    }
    
    // const int translv = CHAR_getInt(talker, CHAR_TRANSMIGRATION);    // 白虎
    
    int pets_on_char = 0;
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++)
      if (CHAR_getCharPet(talker, i) != -1) 
        ++pets_on_char;

    // 白虎
    /*if (translv < 5 || CHAR_getInt(talker, CHAR_LV) < 125) {
        sprintf(tmpbuf, "抱歉哦～等到转生５次及等级１２５级时再来找我吧！");
        CHAR_talkToCli(talker, meindex, tmpbuf, CHAR_COLORWHITE);
        return;
    }*/   
    // 青龙  
    if (CHAR_getInt(talker, CHAR_FAME) < 200000) {
        sprintf(tmpbuf, "抱歉哦～你目前个人声望 %d 点，请等到超过二千点时再来找我吧！", (CHAR_getInt(talker, CHAR_FAME)/100));
        CHAR_talkToCli(talker, meindex, tmpbuf, CHAR_COLORWHITE);
        return;
    }
    else if (ckpoint & (1 << shift)) {
        sprintf(tmpbuf, "哦～我记得我好像给过你了吧！");
        CHAR_talkToCli(talker, meindex, tmpbuf,CHAR_COLORWHITE);
        return;
    }
    else if (pets_on_char >= CHAR_MAXPETHAVE) {
        sprintf(tmpbuf, "抱歉～没多余的空间可放置宠物。");
        CHAR_talkToCli(talker, meindex, tmpbuf, CHAR_COLORWHITE);
        return;    
    }
    
    pet_make(talker, "1845");
    // 标识任务已经完成.
    ckpoint = ckpoint | (1 << shift);
    CHAR_setInt(talker, CHAR_ENDEVENT + offset, ckpoint);
}

void pet_make( int char_index, char* message )
{
  int  ret;
  char msgbuf[64];
  int  i;
  const int enemy_id  = atoi(message);
  const int enemy_num = ENEMY_getEnemyNum();
  for (i = 0; i < enemy_num; i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemy_id) {
      break;
    }
  }
  if (i == enemy_num) return;
  ret = ENEMY_createPetFromEnemyIndex( char_index, i);
  snprintf(msgbuf, sizeof(msgbuf), "哦～以你的能力足以配得上一只传说宠物，好吧！就是你了。");
  CHAR_talkToCli(char_index, -1, msgbuf, CHAR_COLORYELLOW);
              
  for( i=0; i<CHAR_MAXPETHAVE; i++ ){
      if( CHAR_getCharPet( char_index, i) == ret )break;
  }

  if( i==CHAR_MAXPETHAVE) i = 0;
  if( CHAR_CHECKINDEX( ret ) == TRUE ){
  CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
  }
  snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
  CHAR_sendStatusString( char_index, msgbuf );
                   
  snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
  CHAR_sendStatusString( char_index, msgbuf );

for( i=0; i<CHAR_MAXPETHAVE; i++ )
  if( CHAR_getCharPet(char_index, i) == ret )
    CHAR_send_K_StatusString(char_index, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);

LogPet(
  CHAR_getChar(char_index, CHAR_NAME ),
  CHAR_getChar(char_index, CHAR_CDKEY ), "凯恩", 1, "PetMaker",
  CHAR_getInt(char_index, CHAR_FLOOR),
  CHAR_getInt(char_index, CHAR_X ),
  CHAR_getInt(char_index, CHAR_Y ),
  CHAR_getChar(ret, CHAR_UNIQUECODE)
);
}
