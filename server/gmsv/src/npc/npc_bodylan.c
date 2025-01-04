#include "version.h"
#include "char.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"
#include "npcutil.h"
#include "object.h"

enum {
  BODYLAN_E_COMMANDNUM = CHAR_NPCWORKINT1, // ��Ѩ����Ӯ��
};

// ���̼�����ƹ����
enum {
  BODYLAN_WIN_FIRST,
  BODYLAN_WIN_LAST_GOOD,
  BODYLAN_WIN_LAST_NG,
  BODYLAN_WIN_GOOD_NO,
  BODYLAN_WIN_ALREADY,
  BODYLAN_WIN_NOT_PREEVENT,
  BODYLAN_WIN_END
};

static void NPC_BodyLan_Profit(int meindex, int playerindex);

static void NPC_BodyLan_Window(int meindex, int talkerindex, int mode);

BOOL NPC_BodyLanInit(int meindex) {

  char szP[256], szArg[4096];
  char buf[256];
  int i, needSeq;

  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPEEVENT);

  if (NPC_Util_GetArgStr(meindex, szArg, sizeof(szArg)) == NULL) {
    print("npc_bodylan.c:û������(%s)\n",
          CHAR_getChar(meindex, CHAR_NPCARGUMENT));
    return FALSE;
  }

  if (NPC_Util_GetStrFromStrWithDelim(szArg, "Act", szP, sizeof(szP)) == NULL) {
    print("npc_bodylan:������������δ�趨(%s)\n", szArg);
    return FALSE;
  }

  for (i = 0;; i++) {
    //   ۢئ��
    if (getStringFromIndexWithDelim(szP, ",", i, buf, sizeof(buf)) != FALSE) {
      needSeq = atoi(buf);
      // Ѩ�̹ϵ���  ���ճ����ƥ
      if (needSeq < 0) {
        if (i <= 0) {
          print("npc_bodylan:��������δ�趨(%s)\n", szArg);
        }
        //     ��ë������
        CHAR_setWorkInt(meindex, BODYLAN_E_COMMANDNUM, i);
        break;
      } else {
      }
    } else {
      if (i <= 0) {
        print("npc_bodylan:��������δ�趨(%s)\n", szArg);
      }
      CHAR_setWorkInt(meindex, BODYLAN_E_COMMANDNUM, i);
      break;
    }
  }

  return TRUE;
}

void NPC_BodyLanTalked(int meindex, int talkerindex, char *szMes, int color) {
  char szP[256], szArg[4096];
  int EventNo = -1, Pre_Event = -1;

  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  if (NPC_Util_CharDistance(talkerindex, meindex) > 2) {
    return;
  }

  if (NPC_Util_GetArgStr(meindex, szArg, sizeof(szArg)) == NULL) {
    print("npc_bodylan.c:û������(%s)\n",
          CHAR_getChar(meindex, CHAR_NPCARGUMENT));
    return;
  } else {
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "EventNo", szP, sizeof(szP)) != NULL) {
      EventNo = atoi(szP);
    }
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Pre_Event", szP, sizeof(szP)) != NULL) {
      Pre_Event = atoi(szP);
    }
  }

  // ����  ۢ��ì����  į��ؤľ��
  if (Pre_Event >= 0) {
    // ��ì�����帲�ƻ������ƻ����¾���������
    if (NPC_EventCheckFlg(talkerindex, Pre_Event) == FALSE) {
      //   ۢ��ì����ë����ʧ�ƻ���ئ�������ճ𼰱�����
      NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_NOT_PREEVENT);
      return;
    }
  }
  // ��ì����  į��ؤľ��
  if (EventNo >= 0) {
    // ��ì�����帲�ƻ������ƻ����¾���������
    if (NPC_EventCheckFlg(talkerindex, EventNo) == TRUE) {
      //   �Ȼ������ճ𼰱�����
      NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_ALREADY);
      return;
    }
  }

  // �Գ����������������������Ȼ����׾�
  if (CHAR_getWorkInt(talkerindex, CHAR_WORKTRADER) == meindex) {

    // ���ձ���Ѩ������  ��  �ƻ�������
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT) >=
        CHAR_getWorkInt(meindex, BODYLAN_E_COMMANDNUM)) {
      // ��  �����    ����
      NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_LAST_GOOD);
      return;
    } else {
      // ��
      NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_LAST_NG);
      return;
    }
  } else {
    // �ٻ���ئ��ƥ������
    // ��������������м��̼������͵�ë���������
    CHAR_setWorkInt(talkerindex, CHAR_WORKTRADER, meindex);
    // �ˡ���������  �پ��ձ������
    CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 1);

    // �����˱������羮�������微ئؤ������
    NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_FIRST);
    return;
  }
}

/*=======================================
 * watch ��
 *======================================*/
void NPC_BodyLanWatch(int objmeindex, int objmoveindex, CHAR_ACTION act, int x,
                      int y, int dir, int *opt, int optlen) {
  char szP[256], szArg[4096];
  char buf[256];
  int actindex;
  int meindex;
  int seqNo, needSeq;

  // ƽ�ҷ�������½����������
  if (OBJECT_getType(objmoveindex) != OBJTYPE_CHARA)
    return;
  actindex = OBJECT_getIndex(objmoveindex);
  // ������������½����������
  if (CHAR_getInt(actindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;

  // ���м��̼������͵�
  meindex = OBJECT_getIndex(objmeindex);

  // ���м�INDEXë��  �ƻ����¾�����ئ��ľ����������
  if (CHAR_getWorkInt(actindex, CHAR_WORKTRADER) != meindex) {
    return;
  }

  // ������ƥʧ�������ë�ƻ����¾�
  seqNo = CHAR_getWorkInt(actindex, CHAR_WORKSHOPRELEVANT);
  //   ئ������  �پ���
  if (seqNo < 1)
    seqNo = 1;

  // ¦��  ٯ
  if (NPC_Util_GetArgStr(meindex, szArg, sizeof(szArg)) == NULL) {
    print("npc_bodylan.c:û������(%s)\n",
          CHAR_getChar(meindex, CHAR_NPCARGUMENT));
    return;
  }

  // �ˡ�������ë����
  if (NPC_Util_GetStrFromStrWithDelim(szArg, "Act", szP, sizeof(szP)) == NULL) {
    print("npc_bodylan:������������δ�趨(%s)\n", szArg);
    return;
  }

  //   ۢئ��
  if (getStringFromIndexWithDelim(szP, ",", seqNo, buf, sizeof(buf)) != FALSE) {
    needSeq = atoi(buf);
  } else {
    // ئ�⾮ئ��������  �پ���
    CHAR_setWorkInt(actindex, CHAR_WORKSHOPRELEVANT, 1);

    //		print( "����ͷ�ˡ��ص������\n", seqNo, needSeq );
    // �Ͼ��������微������
    return;
  }

  // ���  ۢئʧ�����������������
  if (needSeq == act) {
    // �������ף۳�ľ��    ��
    //		print( "�ɹ�\(%d������%d)\n", seqNo, needSeq );
    seqNo++;
    if (seqNo >= CHAR_getWorkInt(meindex, BODYLAN_E_COMMANDNUM)) {
      //			print( "�ڴ˽�����\n" );
    }
    // ����  ë����
    CHAR_setWorkInt(actindex, CHAR_WORKSHOPRELEVANT, seqNo);
  } else {
    // ��  ����������  �پ���֧Ի  ��
    CHAR_setWorkInt(actindex, CHAR_WORKSHOPRELEVANT, 1);
    //		print( "(%d������%d)\n", seqNo, needSeq );
  }
}

//********* ��  �ݼ���     *********
static void NPC_BodyLan_Profit(int meindex, int playerindex) {
  char szArg[4096], szP[256];
  int fl, x, y, pmode, i, subindex, parent;

  // ¦��  ٯ
  if (NPC_Util_GetArgStr(meindex, szArg, sizeof(szArg)) == NULL) {
    print("npc_bodylan.c:û������(%s)\n",
          CHAR_getChar(meindex, CHAR_NPCARGUMENT));
    return;
  }

  //*********************************************
  //
  //   ��    ������۷�����ɬ��
  //
  //*********************************************
  if (NPC_Util_GetStrFromStrWithDelim(szArg, "Warp", szP, sizeof(szP)) !=
      NULL) {
    // ��    �工�������  ����
    if (sscanf(szP, "%d,%d,%d", &fl, &x, &y) == 3) {
    } else {
      return;
    }

    pmode = CHAR_getWorkInt(playerindex, CHAR_WORKPARTYMODE);
    switch (pmode) {
    case 1:
      parent = playerindex;
      break;
    case 2:
      parent = CHAR_getWorkInt(playerindex, CHAR_WORKPARTYINDEX1);
      break;
    default:
      CHAR_warpToSpecificPoint(playerindex, fl, x, y);
      return;
    }
    for (i = 0; i < getPartyNum(parent); i++) {
      subindex = CHAR_getWorkInt(parent, CHAR_WORKPARTYINDEX1 + i);
      if (CHAR_CHECKINDEX(subindex) == FALSE)
        continue;
      CHAR_warpToSpecificPoint(subindex, fl, x, y);
    }
  }
}

#if 1
static void NPC_BodyLan_Window(int meindex, int talkerindex, int mode) {
  char token[1024];
  char escapedname[2048];
  char szArg[4096];
  char szP[256];
  int fd;
  int buttontype = 0, windowtype = 0, windowno = 0;

  if (CHAR_CHECKINDEX(talkerindex) == FALSE)
    return;
  fd = getfdFromCharaIndex(talkerindex);

  // ¦��  ٯ
  if (NPC_Util_GetArgStr(meindex, szArg, sizeof(szArg)) == NULL) {
    print("npc_bodylan.c:û������(%s)\n",
          CHAR_getChar(meindex, CHAR_NPCARGUMENT));
    return;
  }

  szP[0] = 0;

  switch (mode) {
  case BODYLAN_WIN_FIRST:
    //   �ٱ�  ��ľ��������������
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "First", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:һ��ʼ����������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;

  case BODYLAN_WIN_LAST_GOOD:
    //     ƥ��ݩ��������
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Good", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:���ʱ˵������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_YESNO; // YES|NO
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;

  case BODYLAN_WIN_LAST_NG:
    //     ƥ��ݩ��������
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Ng", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:���ʱ˵������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_OK; // OK
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;

  case BODYLAN_WIN_GOOD_NO:
    // ��ݩ�ݱ�    ëƽ�Ҽ�����
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Good_No", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:���ʱȡ����Ʒ������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_OK; // OK
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;

  case BODYLAN_WIN_ALREADY:
    // ��ƥ����ì����ë��  �ƻ���������
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Good_No", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:�¼�����ʱ������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_OK; // OK
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;

  case BODYLAN_WIN_NOT_PREEVENT:
    // ����  ۢئ��ì����ë��ئ�ƻ���ئ������
    if (NPC_Util_GetStrFromStrWithDelim(szArg, "Pre_Not", szP, sizeof(szP)) ==
        NULL) {
      print("npc_bodylan:?��ǰ�¼�����������û������(%s)\n", szArg);
      return;
    }
    sprintf(token, "%s", szP);
    buttontype = WINDOW_BUTTONTYPE_OK; // OK
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = mode;
    break;
  default:
    return;
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-���ƥ˪������--*/
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
�ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
-------------------------------------------*/
void NPC_BodyLanWindowTalked(int meindex, int talkerindex, int seqno,
                             int select, char *data) {

  if (NPC_Util_CharDistance(talkerindex, meindex) > 2)
    return;

  switch (seqno) {
  case BODYLAN_WIN_LAST_GOOD: // ��ݩ�ݱ�OK�ɽ�ľ����
    if (select == WINDOW_BUTTONTYPE_YES) {
      NPC_BodyLan_Profit(meindex, talkerindex);
      // ��������������м��̼������͵�ë٨ľ������
      CHAR_setWorkInt(talkerindex, CHAR_WORKTRADER, -1);
      // �ˡ���������  �پ��ձ������
      CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 1);
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      // ��ݩ�ݱ�ƽ�Ҽ������ľ����
      NPC_BodyLan_Window(meindex, talkerindex, BODYLAN_WIN_GOOD_NO);
      // ��������������м��̼������͵�ë٨ľ������
      CHAR_setWorkInt(talkerindex, CHAR_WORKTRADER, -1);
      // �ˡ���������  �پ��ձ������
      CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 1);
    }
    break;
  default:
    break;
  }
}

#endif
