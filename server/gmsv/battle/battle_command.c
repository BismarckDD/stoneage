#include "version.h"
//
#include "gmsv_server.h"
//
#include "battle.h"
#include "battle_command.h"
#include "battle_event.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "handletime.h"
#include "magic.h"
#include "magic_base.h"
#include "pet_skill.h"
#ifdef _PROFESSION_SKILL
#include "profession_skill.h"
#endif
int checkErrorStatus(int char_index);
int NowBattlerFd;

int BATTLE_MpDown(int char_index, int down) { return 0; }

void BattleCommandDispach(int fd, char *command) {
  int endFlg = 0, battleindex;
  int EscapeFree = BATTLE_FLG_FREEDP;
  int char_index = CONNECT_getCharaindex(fd);
  char szBuffer[256] = "";
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
  int i, j, pindex;
  BATTLE_ENTRY *pEntry;
#endif

  NowBattlerFd = fd;

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return;

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    // printf("BattleCommandDispach:%s %d\n", __FILE__, __LINE__);
    return;
  }

  battleindex = CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX);

  if (BATTLE_CHECKINDEX(battleindex) == FALSE ||
      CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_NONE) {
    // printf("BattleCommandDispach:%s %d\n", __FILE__, __LINE__);
    return;
  }

  if (BattleArray[battleindex].flg & BATTLE_FLG_CHARALOST) {
    EscapeFree = 0;
  }

  if (strncmp(command, "U", 1) == 0) {
    // shan 2001/12/25 begin
    EscapeFree = 0;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEWATCH) != TRUE) {
      CHAR_talkToCli(char_index, -1,
                     "华义大魔王使出定身法，定定定定定定把你定在战斗里。"
                     "",
                     CHAR_COLORYELLOW);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      // 历史注释的原始编码已损坏，无法可靠恢复。
      // );
      endFlg = 1;
      BATTLE_MpDown(char_index, 2);
    } else {
      BATTLE_WatchStop(char_index);
    }
    // shan end
  } else if (strncmp(command, "E", 1) == 0) {

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }
#ifdef _NOT_ESCAPE

    int i;
    for (i = 0; i < 32; i++) {
      if (getNotEscape(i) == CHAR_getInt(char_index, CHAR_FLOOR)) {
        CHAR_talkToCli(char_index, -1,
                       "大魔王使出定身法，定定定定定定把你定在战斗里。"
                       "",
                       CHAR_COLORYELLOW);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);
        break;
      }
    }
    if (i == 32)
#endif
    {
      CHAR_setWorkInt(char_index, CHAR_WORKDBATTLEESCAPE, 1);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ESCAPE);
    }
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    // 历史注释的原始编码已损坏，无法可靠恢复。
    endFlg = 1;

    BATTLE_MpDown(char_index, 20);

  } else if (strncmp(command, "H|", 2) == 0) {
    int iNum = -1, At_SoubiIndex;
    if (sscanf(command + 2, "%X", &iNum) != 1 || (iNum < 0 || iNum >= 20)) {
      iNum = -1;
    }

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, iNum);

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);

    At_SoubiIndex = CHAR_getItemIndex(char_index, CHAR_ARM);
    if (ITEM_CHECKINDEX(At_SoubiIndex) == FALSE) {

    } else {

      if (ITEM_getInt(At_SoubiIndex, ITEM_TYPE) == ITEM_BOOMERANG) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG);
        // print("ITEM_TYPE=%d iNum=%d",At_SoubiIndex,iNum);
#ifdef _PETSKILL_BECOMEFOX
        if (CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER) == 101749 ||
            CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸不可以使用回力标
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
#endif
#ifdef _PETSKILL_BECOMEPIG
        if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > -1) { //变成乌力了
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
        }
#endif
      }
    }

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

    CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3, 1);
    // 历史注释的原始编码已损坏，无法可靠恢复。

    endFlg = 1;

    BATTLE_MpDown(char_index, 5);

  } else if (strncmp(command, "G", 1) == 0) {

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

    sprintf(szBuffer, "Command(%s)(防御)", CHAR_getUseName(char_index));
    endFlg = 1;

    BATTLE_MpDown(char_index, 2);

  } else if (strncmp(command, "N", 1) == 0) {

    EscapeFree = 0;

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    //		sprintf( szBuffer, "Command(%s)(server托管)", CHAR_getUseName(
    // 历史注释的原始编码已损坏，无法可靠恢复。
    // char_index ) );
    endFlg = 1;

  } else if (strncmp(command, "T|", 2) == 0) {
    int iNum = -1;

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;

    if (sscanf(command + 2, "%X", &iNum) != 1 || (iNum < 0 || iNum >= 20)) {
      iNum = -1;
    }

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, iNum);

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_CAPTURE);

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

    // 历史注释的原始编码已损坏，无法可靠恢复。
    endFlg = 1;

    BATTLE_MpDown(char_index, 20);

  } else if (strncmp(command, "S|", 2) == 0) {
    int iNum = -1;

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;
    // shan 2002/01/08 begin
    if (sscanf(command + 2, "%d", &iNum) != 1 ||
        (iNum < -1 || iNum >= CHAR_MAXPETHAVE) ||
        (CHAR_getInt(char_index, CHAR_RIDEPET) == iNum)) {
      iNum = -1;
    }
    int petindex = CHAR_getCharPet(char_index, iNum);
#ifdef _PET_VALIDITY
    if (CHAR_getInt(petindex, CHAR_PETVALIDITY) > 0 &&
        CHAR_getInt(petindex, CHAR_PETVALIDITY) < time(NULL)) {
      CHAR_talkToCli(char_index, -1, "该宠物已经失效了！", CHAR_COLORYELLOW);
      iNum = -1;
    }
#endif

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, iNum);
    if (iNum < 0) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_PETIN);
    } else {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_PETOUT);
    }
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

    endFlg = 1;

    BATTLE_MpDown(char_index, 10);

  } else if (strncmp(command, "W|", 2) == 0) {
    int iNum = -1, petskillindex = -1, ToNo = -1, petnum, petindex;

    if (sscanf(command + 2, "%X|%X", &iNum, &ToNo) < 1) {
      iNum = -1;
      ToNo = -1;
    }
    if (iNum == 255)
      iNum = -1;
    if (ToNo == 255)
      ToNo = -1;

    petnum = CHAR_getInt(char_index, CHAR_DEFAULTPET);
    petindex = CHAR_getCharPet(char_index, petnum);
    if (CHAR_CHECKINDEX(petindex) == FALSE) {
    } else {
#ifdef _FIX_PETSKILL_BUG
      if (CHAR_getWorkInt(petindex, CHAR_WORKBATTLEMODE) ==
              BATTLE_CHARMODE_C_OK ||
          CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
              BATTLE_CHARMODE_C_OK) {
        CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      } else
#endif
          if (iNum < 0 || iNum >= CHAR_MAXPETSKILLHAVE
#ifdef _PETSKILLBUG // Add By Syu 2002.0805
              || (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) ||
              (CHAR_getInt(char_index, CHAR_HP) <= 0)
#endif
              || checkErrorStatus(petindex)) {
        // sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
        CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
#ifdef _PETSKILLBUG // Add By Syu 2002.0805
      } else if ((CHAR_getInt(petindex, CHAR_TRANSMIGRATION) < 1) &&
                 (iNum >= CHAR_getInt(petindex, CHAR_SLOT))) {
        // sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
        CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
#endif
      } else {
        petskillindex = PETSKILL_GetArray(petindex, iNum);
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
        if ((CHAR_getPetSkill(petindex, iNum) == 600) && (ToNo == -1)) {
          for (j = 0; j < 2; j++) {
            pEntry = BattleArray[battleindex].Side[j].Entry;
            for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
              pindex = pEntry[i].char_index;
              if (pindex == petindex) {
                ToNo = i + (j * 10);
                break;
              }
            }
          }
        }
#endif
        if (petskillindex >= 0 &&
            PETSKILL_Use(petindex, iNum, ToNo, NULL) == TRUE) {
          EscapeFree = 0;
        } else {
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
        }
      }
    }
    endFlg = 1;

    BATTLE_MpDown(char_index, 10);

  } else if (strncmp(command, "J|", 1) == 0) {
    int iNum = -1, magicindex = -1, ToNo = -1;
    int valid = -1;
    int item_index = -1;

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;

    if (sscanf(command + 2, "%X|%X", &iNum, &ToNo) != 2) {
      iNum = -1;
      ToNo = -1;
    }

    if (iNum < CHAR_EQUIPPLACENUM && iNum >= CHAR_HEAD) {
      magicindex = MAGIC_GetArrayNo(char_index, iNum);
      item_index = CHAR_getItemIndex(char_index, iNum);
      valid = MAGIC_isTargetValid(magicindex, ToNo);

      if ((valid < 0) || (magicindex < 0) || (item_index < 0)) {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      } else {
#ifdef _ITEM_ATTSKILLMAGIC
        char *magicarg;
        int magic_array = -1;
        magic_array = MAGIC_getMagicArray(magicindex);
        magicarg = MAGIC_getChar(magic_array, MAGIC_OPTION);
        if (magicarg != "\0" && strstr(magicarg, "SKILL") != NULL) {
          int mp = ITEM_getInt(item_index, ITEM_MAGICUSEMP);
          if (MAGIC_AttSkill(char_index, ToNo, magic_array, mp) == FALSE) {
            CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
            CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE,
                            BATTLE_CHARMODE_C_OK);
          }
        } else {
#endif
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, ToNo);
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU);
          CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3, magicindex);
          CHAR_SETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3, iNum);
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE,
                          BATTLE_CHARMODE_C_OK);
#ifdef _ITEM_ATTSKILLMAGIC
        }
#endif
      }
    } else {
      EscapeFree = 0;
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    }
    endFlg = 1;
  } else if (strncmp(command, "I|", 1) == 0) {
    int iNum = -1, ToNo = -1;
    int valid = 0, item_index = -1;

    if (checkErrorStatus(char_index)) {
      BattleCommandDispach(fd, "N");
      return;
    }

    EscapeFree = 0;

    if (sscanf(command + 2, "%X|%X", &iNum, &ToNo) != 2) {
      iNum = -1;
      ToNo = -1;
    }
    item_index = CHAR_getItemIndex(char_index, iNum);
    if (!ITEM_CHECKINDEX(item_index))
      valid = -1;
    if (valid >= 0)
      valid = ITEM_isTargetValid(char_index, item_index, ToNo);
    if (valid < 0) {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    } else {
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, ToNo);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_ITEM);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM3, iNum);
      CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      endFlg = 1;
    }
  } else if (strncmp(command, "@", 1) == 0) {
    // 历史注释的原始编码已损坏，无法可靠恢复。

    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    endFlg = 1;

    CHAR_setInt(char_index, CHAR_MP,
                min(100, CHAR_getInt(char_index, CHAR_MP) + 30));

  } else
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    if (strncmp(command, "P", 1) == 0) {
      int iNum = -1, ToNo = -1;

      if (checkErrorStatus(char_index)) {
        BattleCommandDispach(fd, "N");
        return;
      }

      EscapeFree = 0;

      if (sscanf(command + 2, "%X|%X", &iNum, &ToNo) < 1) {
        iNum = -1;
        ToNo = -1;
      }

      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_NONE) {
#ifdef _PROSKILL_ERR_KICK
        CONNECT_setCloseRequest(fd, 1);
#endif
        return;
      }
#ifndef _PROSKILL_OPTIMUM // Robin fix cancel.
      char_pskill = CHAR_getInt(char_index, PROFESSION_CLASS);
      skillindex = PROFESSION_SKILL_GetArray(char_index, iNum);
      Pskillid = PROFESSION_SKILL_getskillArray(skillindex);
      profession_skill =
          PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

      if ((char_pskill > 0) && (char_pskill == profession_skill)) {
#else
      if (1) {
#endif
        if (PROFESSION_SKILL_Use(char_index, iNum, ToNo, NULL) == 1) {
          endFlg = 1;
        } else {
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
          CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE,
                          BATTLE_CHARMODE_C_OK);
#ifdef _PROSKILL_ERR_KICK
          CONNECT_setCloseRequest(fd, 1);
#endif
        }
      } else {
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT);
        CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
#ifdef _PROSKILL_ERR_KICK
        CONNECT_setCloseRequest_debug(fd, 1);
#endif
      }

      EscapeFree = 0;
    } else
#endif
    {
      endFlg = 2;
    }
  if (endFlg) {
    BATTLE_ActSettingSend(battleindex);
    /*
                    if( endFlg==1 ){
                            int fd;
                            if( CHAR_getInt( char_index,
       CHAR_WHICHTYPE)==CHAR_TYPEPLAYER ){ fd = CHAR_getWorkInt( char_index,
       CHAR_WORKFD); CONNECT_SetBattleRecvTime( fd, &NowTime);
                            }
                    }
    */
  }
  {
    // if( BATTLE_CHECKINDEX( battleindex ) == TRUE ){
    //	BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORWHITE ) ;
    // }
  }

  if (BattleArray[battleindex].turn == 0) {
    BattleArray[battleindex].flg |= EscapeFree;
  } else {
    BattleArray[battleindex].flg &= EscapeFree;
  }
}

void BATTLE_ActSettingSend(int battleindex) {
  int pindex, k = 0, i, j, char_index;
  int endBit = 0;
  char szBA[256];
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pindex = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(pindex)) {
        // 戊穴件玉    匹五化中凶日
        if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_C_OK) {
          endBit |= 1 << k; // 申永玄  化月
        } else
          // 衬分匀凶日濮覆  井允
          if (CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY
#ifdef _PLAYER_NPC
              || CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
              CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC
#endif
          ) {
            endBit |= 1 << k; // 申永玄  化月
          }
      }
      k++;
    }
  }
  sprintf(szBA, "BA|%X|%X|", endBit, BattleArray[battleindex].turn);
  //	print( "%s\n", szBA );
  // 今日卞蝈够伙□皿仄化霜月
  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pindex = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      // 辅爵仄凶化及平乓仿卞反霜日卅中
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE)
        continue;
      // 皿伊奶乩□分匀凶日
      if (CHAR_getInt(pindex, CHAR_WHICHTYPE) ==
          CHAR_TYPEPLAYER) { // 戊穴件玉霜月
        BATTLE_CommandSend(pindex, szBA);
      }
    }
  }

  // 棋爵仄化中月谛卞手霜月
  pBattle = BattleArray[battleindex].pNext;
  for (; pBattle; pBattle = pBattle->pNext) {

    // 仇及失玉伊旦互恳仄中井民尼永弁
    if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
      fprint("err:观战battle address错误(%p)\n", pBattle);
      break;
    }
    // 蝈够卞霜耨
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = pBattle->Side[0].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
        continue;
      // 戊穴件玉霜月
      BATTLE_CommandSend(pindex, szBA);
    }
  }
}

BOOL BATTLE_IsHide(int char_index) {
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLECOM1) ==
      BATTLE_COM_S_EARTHROUND0) {
    return TRUE;
  }
  return FALSE;
}

BOOL _BATTLE_CommandSend(int char_index, char *pszCommand, char *file,
                         int line) {
  //	printf("封包内容=%s\n",pszCommand);
  //	printf("封包路径%s  行=%d\n",file,line);
  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return FALSE;
  if (getfdFromCharaIndex(char_index) < 0)
    return FALSE;
  GmsvServer_B_send(getfdFromCharaIndex(char_index), pszCommand);
  return TRUE;
}

BOOL BATTLE_MakeCharaString(int battleindex, char *pszCommand, int size) {
  int i, j, iOffset, char_index, flg;
  BATTLE_ENTRY *pEntry;
  char szBuffer[4096] = "", *pszTop, *pszLast;
  char szEscapeName[128], szEscapeTitle[128];
  char szEscapePetName[128];
  int rideflg = 0, petindex = -1, petlevel, pethp, petmaxhp;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return FALSE;
  pszTop = pszCommand;
  pszLast = pszCommand + size - 1;
  sprintf(szBuffer, "BC|%X|", BattleArray[battleindex].field_att);
  STRCPY_TAIL(pszTop, pszLast, szBuffer);

  for (j = 0; j < 2; j++) {
    if (j == 1) {
      iOffset = SIDE_OFFSET;
    } else {
      iOffset = 0;
    }
    pEntry = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = pEntry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
          CHAR_BATTLEFLG_ULTIMATE)
        continue;
      flg = 0;
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_INIT)
        flg |= BC_FLG_NEW;
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE)
        flg |= BC_FLG_NEW;
      if (CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
        flg |= BC_FLG_DEAD;
      } else if (CHAR_getWorkInt(char_index, CHAR_WORKPOISON) > 0) {
        flg |= BC_FLG_POISON;
      } else
#ifdef _MAGIC_WEAKEN //   虚弱
        if (CHAR_getWorkInt(char_index, CHAR_WORKWEAKEN) > 0) {
          flg |= BC_FLG_WEAKEN;
        } else
#endif
#ifdef _MAGIC_DEEPPOISON //   剧毒
          if (CHAR_getWorkInt(char_index, CHAR_WORKDEEPPOISON) > 0) {
            flg |= BC_FLG_DEEPPOISON;
          } else
#endif
#ifdef _MAGIC_NOCAST
            if (CHAR_getWorkInt(char_index, CHAR_WORKNOCAST) > 0) {
              flg |= BC_FLG_NOCAST;
            } else
#endif
#ifdef _MAGIC_BARRIER
              if (CHAR_getWorkInt(char_index, CHAR_WORKBARRIER) > 0) {
                flg |= BC_FLG_BARRIER;
              } else
#endif
                  if (CHAR_getWorkInt(char_index, CHAR_WORKPARALYSIS) > 0) {
                flg |= BC_FLG_PARALYSIS;
              } else if (CHAR_getWorkInt(char_index, CHAR_WORKSLEEP) > 0) {
                flg |= BC_FLG_SLEEP;
              } else if (CHAR_getWorkInt(char_index, CHAR_WORKSTONE) > 0) {
                flg |= BC_FLG_STONE;
              } else if (CHAR_getWorkInt(char_index, CHAR_WORKDRUNK) > 0) {
                flg |= BC_FLG_DRUNK;
              } else if (CHAR_getWorkInt(char_index, CHAR_WORKCONFUSION) > 0) {
                flg |= BC_FLG_CONFUSION;
              }
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        flg |= BC_FLG_PLAYER;
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEFLG) &
          CHAR_BATTLEFLG_REVERSE) {
        flg |= BC_FLG_REVERSE;
      }
      if (BATTLE_IsHide(char_index) == TRUE) {
        flg |= BC_FLG_HIDE;
      }
#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
      if (CHAR_getWorkInt(char_index, CHAR_WORKSARS) > 0) {
        flg |= BC_FLG_SARS;
      }
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
      if (CHAR_getWorkInt(char_index, CHAR_WORKDIZZY) > 0) {
        flg |= BC_FLG_DIZZY; // 晕眩
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKENTWINE) > 0) {
        flg |= BC_FLG_ENTWINE; // 树根缠绕
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKDRAGNET) > 0) {
        flg |= BC_FLG_DRAGNET; // 天罗地网
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKICECRACK) > 0) {
        //	flg |= BC_FLG_ICECRACK;			// 冰爆术
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKOBLIVION) > 0) {
        flg |= BC_FLG_OBLIVION; // 遗忘
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKICEARROW) > 0) {
        flg |= BC_FLG_ICEARROW; // 冰箭
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKBLOODWORMS) > 0) {
        flg |= BC_FLG_BLOODWORMS; // 嗜血蛊
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKSIGN) > 0) {
        flg |= BC_FLG_SIGN; // 一针见血
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKINSTIGATE) > 0) {
        flg |= BC_FLG_CARY; // 挑拨
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORK_F_ENCLOSE) > 0) {
        flg |= BC_FLG_F_ENCLOSE; // 火附体
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORK_I_ENCLOSE) > 0) {
        flg |= BC_FLG_I_ENCLOSE; // 冰附体
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORK_T_ENCLOSE) > 0) {
        flg |= BC_FLG_T_ENCLOSE; // 雷附体
      }
#endif
#ifdef _PROFESSION_ADDSKILL
      if (CHAR_getWorkInt(char_index, CHAR_WORKANNEX) > 0) {
        flg |= BC_FLG_T_ENCLOSE; // 附身术
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKWATER) > 0) {
        flg |= BC_FLG_WATER; // 水附体
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKFEAR) > 0) {
        flg |= BC_FLG_FEAR; // 恐惧
      }
#endif
#ifdef _PETSKILL_LER
      if (CHAR_getWorkInt(char_index, CHAR_WORK_RELIFE) > 0) {
        flg |= BC_FLG_CHANGE; // 雷尔变身
        CHAR_setWorkInt(char_index, CHAR_WORK_RELIFE, 0);
      }
#endif
      makeEscapeString(CHAR_getUseName(char_index), szEscapeName,
                       sizeof(szEscapeName));
      makeEscapeString(BATTLE_CharTitle(char_index), szEscapeTitle,
                       sizeof(szEscapeTitle));
      // Robin 0728 ride Pet
      petindex = BATTLE_getRidePet(char_index);
      if (petindex != -1) {
        rideflg = 1;
        makeEscapeString(CHAR_getUseName(petindex), szEscapePetName,
                         sizeof(szEscapePetName));
        petlevel = CHAR_getInt(petindex, CHAR_LV);
        pethp = CHAR_getInt(petindex, CHAR_HP);
        petmaxhp = CHAR_getWorkInt(petindex, CHAR_WORKMAXHP);

      } else {
        if (CHAR_getWorkInt(char_index, CHAR_WORKPETFALL) == 1) {
          rideflg = -1;
#ifdef _PETSKILL_BECOMEFOX // 若中了媚惑术而落马时将讯息传给 Client
          if (CHAR_getWorkInt(char_index, CHAR_WORKFOXROUND) != -1)
            rideflg = -2; // 历史注释的原始编码已损坏，无法可靠恢复。
                          // 历史注释的原始编码已损坏，无法可靠恢复。
#endif
#ifdef _PETSKILL_BECOMEPIG // 若中了乌力化而落马时将讯息传给 Client
          if (CHAR_getInt(char_index, CHAR_BECOMEPIG) > 120) //变成乌力了
            rideflg = -3; // client
                          // 历史注释的原始编码已损坏，无法可靠恢复。
#endif
          CHAR_setWorkInt(char_index, CHAR_WORKPETFALL, 0);
        } else
          rideflg = 0;
        szEscapePetName[0] = 0;
        petlevel = 0;
        pethp = 0;
        petmaxhp = 0;
      }

      sprintf(szBuffer, "%X|%s|%s|%X|%X|%X|%X|%X|%X|%s|%X|%X|%X|",
              pEntry[i].bid, szEscapeName, szEscapeTitle,
              CHAR_getInt(char_index, CHAR_BASEIMAGENUMBER),
              CHAR_getInt(char_index, CHAR_LV),
              max(CHAR_getInt(char_index, CHAR_HP), 0),
              CHAR_getWorkInt(char_index, CHAR_WORKMAXHP), flg, rideflg,
              szEscapePetName, petlevel, pethp, petmaxhp);
      STRCPY_TAIL(pszTop, pszLast, szBuffer);
      if (pszTop >= pszLast)
        return FALSE;
    }
  }
  return TRUE;
}

void BATTLE_BpSendToWatch(BATTLE *pBattle,  //
                          char *pszBcString) { // BC
  char szBp[256];
  int flg = 0, i, char_index;
  if (pBattle == NULL)
    return;
  if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
    logErr("err:battle address:(%p)\n", pBattle);
    return;
  }

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    char_index = pBattle->Side[0].Entry[i].char_index;
    if (CHAR_CHECKINDEX(char_index) == FALSE)
      continue;
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_WATCHINIT) {
      flg |= BP_FLG_JOIN; // 蕙筋辅笛
    }
    flg |= BP_FLG_PLAYER_MENU_OFF; // 皿伊奶乩□丢瓦亘□请今卅中
    // 皿伊奶乩□  寞霜月
    sprintf(szBp, "BP|%X|%X|%X", 20, flg, 0);
    BATTLE_CommandSend(char_index, szBp);
    // 蝈够隋骚树  霜月
    BATTLE_CommandSend(char_index, pszBcString);
  }
  // 戊穴件玉谨切尺
  pBattle->mode = BATTLE_MODE_WATCHPRE;
  //	}
}

void BATTLE_CharSendAll(int battleindex) {
  int i, j, char_index, flg, pet, At_SoubiIndex, pindex;
  char szBp[256];
  BATTLE *pBattle;

  BATTLE_MakeCharaString(battleindex, szAllBattleString,
                         sizeof(szAllBattleString));

  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = BattleArray[battleindex].Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
        continue;
      flg = 0;
      if (BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_ENEMY_SURPRISAL;
      } else if (BattleArray[battleindex].Side[1 - j].flg &
                 BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_PLAYER_SURPRISAL;
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
        flg |= BP_FLG_JOIN; // 辅爵
      }
      At_SoubiIndex = CHAR_getItemIndex(char_index, CHAR_ARM);
      if (ITEM_CHECKINDEX(At_SoubiIndex) == FALSE) {
      } else {
        if (ITEM_getInt(At_SoubiIndex, ITEM_TYPE) == ITEM_BOOMERANG) {
          flg |= BP_FLG_BOOMERANG;
        }
      }

      pet = CHAR_getInt(char_index, CHAR_DEFAULTPET);
      if (pet >= 0) {
        snprintf(szBp, sizeof(szBp), "K%d", pet);
        CHAR_sendStatusString(char_index, szBp);

        pindex = CHAR_getCharPet(char_index, pet);

#ifdef _PETSKILL_DAMAGETOHP
        {
          /* 历史注释或停用代码的原始编码已损坏，无法可靠恢复。 */
          char msg[32] = {0};
          // print("\n宠物id:%d",CHAR_getInt( pindex, CHAR_PETID));
          // print("\n宠物名:%s",CHAR_getChar( pindex, CHAR_NAME));
          // if( CHAR_getInt( pindex, CHAR_PETID) == 777 ){//宠物ID 水双头狼
          //	  || CHAR_getInt( pindex, CHAR_PETID) == 146 ) //火双头狼
          //	&& CHAR_getInt( pindex, CHAR_HP) ){
          sprintf(msg, "o%d", pet);
          CHAR_sendStatusString(char_index, msg);
        }
#endif

#ifdef _PETSKILL_BECOMEFOX // 限制中了媚惑术的宠物的技能
        // if( CHAR_getInt( pindex, CHAR_BASEIMAGENUMBER) == 101749 ){
        if (CHAR_getWorkInt(pindex, CHAR_WORKFOXROUND) != -1) { // 若是变成小狐狸
          char msg[32] = {0};
          sprintf(msg, "a%d", pet);
          CHAR_sendStatusString(char_index, msg);
        }
#endif

        if (CHAR_CHECKINDEX(pindex) == TRUE) {
          if (BATTLE_IsCharge(pindex) == TRUE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }
          if (BATTLE_CanMoveCheck(pindex) == FALSE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }

          if (CHAR_getFlg(pindex, CHAR_ISDIE) == TRUE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }
        }
      } else {
        flg |= BP_FLG_PET_MENU_OFF;
      }

      if (BATTLE_CanMoveCheck(char_index) == FALSE ||
          CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
        flg |= BP_FLG_PLAYER_MENU_OFF;
      }

      if (BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_PLAYER_MENU_OFF;
        flg |= BP_FLG_PET_MENU_OFF;
      }
      sprintf(szBp, "BP|%X|%X|%X", i + SIDE_OFFSET * j, flg,
              CHAR_getInt(char_index, CHAR_MP));
      BATTLE_CommandSend(char_index, szBp);
      BATTLE_CommandSend(char_index, szAllBattleString);
    }
  }
  pBattle = BattleArray[battleindex].pNext;
  for (; pBattle; pBattle = pBattle->pNext) {
    // 仇及失玉伊旦互恳仄中井民尼永弁
    if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
      fprint("err:观战battle address错误(%p)\n", pBattle);
      break;
    }

    BATTLE_BpSendToWatch(pBattle, szAllBattleString);
  }
}

void BattleEncountOut(int char_index) {

  if (CHAR_CHECKINDEX(char_index) == FALSE)
    return;
  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_FINAL) {
    return;
  }

  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEINDEX, -1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_MODE_NONE);

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));
  CHAR_sendArroundCharaData(char_index);

  if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEWATCH) == 0) {
    CHAR_sendBattleEffect(char_index, OFF);
  } else {
    CHAR_sendBattleWatch(CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX), OFF);
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLEWATCH, 0);
  }

  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
    int pmode /*, battleindex*/;
    int pindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    pmode = CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE);
    if (pmode > 0 && pmode != BATTLE_CHARMODE_FINAL) {
      BATTLE_RescueParentTry(char_index, pindex);
      //				print( "较慢参加战斗(%s)\n",
      // CHAR_getUseName( char_index ) );
    }
  }
}

BOOL BATTLE_PetDefaultCommand(int petindex)
{
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return FALSE;
  // 锹澎及隙烂
  CHAR_setWorkInt(petindex, CHAR_WORKBATTLECOM2, -1);
  // 骚橘  猾及隙烂
  CHAR_setWorkInt(petindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
  // 戊穴件玉OK及隙烂
  CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

  return TRUE;
}

// Robin fix 检查石化昏睡不可战斗
int checkErrorStatus(int char_index) {
  // if( BATTLE_CanMoveCheck( char_index) == FALSE) {

  if (CHAR_getWorkInt(char_index, CHAR_WORKPARALYSIS) > 0 // 麻痹
      || CHAR_getWorkInt(char_index, CHAR_WORKSTONE) > 0  // 石化
      || CHAR_getWorkInt(char_index, CHAR_WORKSLEEP) > 0  // 睡眠
  // 历史注释的原始编码已损坏，无法可靠恢复。
#ifdef _PROFESSION_SKILL                                 // WON ADD 人物职业技能
      || CHAR_getWorkInt(char_index, CHAR_WORKDIZZY) > 0 // 晕眩
      ||
      CHAR_getWorkInt(char_index, CHAR_WORKDRAGNET) > 0 // 天罗地网
  // 历史注释的原始编码已损坏，无法可靠恢复。
#ifdef _PROFESSION_ADDSKILL
// 历史注释的原始编码已损坏，无法可靠恢复。
#endif
#endif
  ) {

    char cdkey[128];
    if (CHAR_getInt(char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
      strcpy(cdkey, CHAR_getChar(char_index, CHAR_CDKEY));
    else
      strcpy(cdkey, CHAR_getChar(char_index, CHAR_OWNERCDKEY));
    return 1;
  }
  return 0;
}
void BATTLE_CharSendOne(int battleindex, int mychar_index) {
  int i, j, char_index, flg, pet, At_SoubiIndex, pindex;
  char szBp[256];
  BATTLE *pBattle;

  BATTLE_MakeCharaString(battleindex, szAllBattleString,
                         sizeof(szAllBattleString));
  print("\nCharSendOne %d %d", battleindex, mychar_index);
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      char_index = BattleArray[battleindex].Side[j].Entry[i].char_index;
      if (CHAR_CHECKINDEX(char_index) == FALSE)
        continue;
      if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
        continue;
      if (char_index != mychar_index)
        continue;
      flg = 0;
      if (BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_ENEMY_SURPRISAL;
      } else if (BattleArray[battleindex].Side[1 - j].flg &
                 BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_PLAYER_SURPRISAL;
      }
      if (CHAR_getWorkInt(char_index, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
        flg |= BP_FLG_JOIN; // 辅爵
      }
      At_SoubiIndex = CHAR_getItemIndex(char_index, CHAR_ARM);
      if (ITEM_CHECKINDEX(At_SoubiIndex) == FALSE) {
      } else {
        if (ITEM_getInt(At_SoubiIndex, ITEM_TYPE) == ITEM_BOOMERANG) {
          flg |= BP_FLG_BOOMERANG;
        }
      }

      pet = CHAR_getInt(char_index, CHAR_DEFAULTPET);
      if (pet >= 0) {
        snprintf(szBp, sizeof(szBp), "K%d", pet);
        CHAR_sendStatusString(char_index, szBp);

        pindex = CHAR_getCharPet(char_index, pet);

#ifdef _PETSKILL_DAMAGETOHP
        {
          char msg[32] = {0};
          sprintf(msg, "o%d", pet);
          CHAR_sendStatusString(char_index, msg);
        }
#endif

#ifdef _PETSKILL_BECOMEFOX
        if (CHAR_getWorkInt(pindex, CHAR_WORKFOXROUND) != -1) {
          char msg[32] = {0};
          sprintf(msg, "a%d", pet);
          CHAR_sendStatusString(char_index, msg);
        }
#endif

        if (CHAR_CHECKINDEX(pindex) == TRUE) {
          if (BATTLE_IsCharge(pindex) == TRUE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }
          if (BATTLE_CanMoveCheck(pindex) == FALSE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }

          if (CHAR_getFlg(pindex, CHAR_ISDIE) == TRUE) {
            flg |= BP_FLG_PET_MENU_OFF;
          }
        }
      } else {
        flg |= BP_FLG_PET_MENU_OFF;
      }

      if (BATTLE_CanMoveCheck(char_index) == FALSE ||
          CHAR_getFlg(char_index, CHAR_ISDIE) == TRUE) {
        flg |= BP_FLG_PLAYER_MENU_OFF;
      }

      if (BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE) {
        flg |= BP_FLG_PLAYER_MENU_OFF;
        flg |= BP_FLG_PET_MENU_OFF;
      }
      sprintf(szBp, "BP|%X|%X|%X", i + SIDE_OFFSET * j, flg,
              CHAR_getInt(char_index, CHAR_MP));
      BATTLE_CommandSend(char_index, szBp);
      BATTLE_CommandSend(char_index, szAllBattleString);
    }
  }
  pBattle = BattleArray[battleindex].pNext;
  for (; pBattle; pBattle = pBattle->pNext) {
    if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
      fprint("err:观战battle address错误(%p)\n", pBattle);
      break;
    }

    BATTLE_BpSendToWatch(pBattle, szAllBattleString);
  }
}
