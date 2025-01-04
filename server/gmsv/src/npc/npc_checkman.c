#include "version.h"
#include "char.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "object.h"

// �ػ�������ƥ���°׷º뼰��
#define MAXEVENTFLG 96

static void NPC_CheckMan_selectWindow(int meindex, int toindex, int num);
int NPC_NowFlgCheck(int meindex, int talker, int now[MAXEVENTFLG]);
int NPC_EndFlgCheck(int meindex, int talker, int nowflg[MAXEVENTFLG]);
BOOL NPC_FlgCheckMain(int meindex, int talker, int nowindex,
                      int now[MAXEVENTFLG], char *work2);

BOOL NPC_CheckManInit(int meindex) {
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPECHECKMAN);
  return TRUE;
}

void NPC_CheckManTalked(int meindex, int talkerindex, char *szMes, int color) {

  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }
  if (NPC_Util_isFaceToFace(meindex, talkerindex, 2) == FALSE) {
    if (NPC_Util_isFaceToChara(talkerindex, meindex, 1) == FALSE)
      return;
  }
  NPC_CheckMan_selectWindow(meindex, talkerindex, 0);
}

static void NPC_CheckMan_selectWindow(int meindex, int talker, int num) {

  char token[1024];
  char work[256];
  char work2[512];

  char escapedname[1024];
  int fd = getfdFromCharaIndex(talker);
  int buttontype = 0;
  int windowtype = 0;
  int windowno = 0;
  int now[MAXEVENTFLG];
  int nowindex;
  int i;
  int page;

  work[0] = 0;
  work2[0] = 0;
  token[0] = 0;

  now[0] = 0;

  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (num) {

  case 0:
    CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 0);
    CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, 0);

    /*--��      --*/
    sprintf(token, "3\n ����������������������Ѩ�������� "
                   "\n�������ػ����׷º���������ë������"
                   "�"
                   "�"
                   "\n"
                   "\n�������� "
                   "�գ�NOW�׷º��������ͣ��� "
                   "\n�������� "
                   "�գ�END�׷º��������ͣ��� "
                   "\n\n���� �գ�NOW�׷º���������  ��� "
                   " "
                   "�� "
                   "\n���� �գ�END�׷º���������  ��� "
                   " "
                   "�� ");

    buttontype = WINDOW_BUTTONTYPE_CANCEL;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_CHECKMAN_START;
    break;
  case 1:
    nowindex = NPC_NowFlgCheck(meindex, talker, now);
    // ��ʸ����  ��
    page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);
    if (page == 0 || page == 1) {
      i = 0;
    } else {
      i = 83;
    }
    for (; i < nowindex; i++) {
      sprintf(work, "%d,", now[i]);
      strcat(work2, work);
    }
    sprintf(token,
            "������������������������Ѩ�������� "
            "\n�����ػ�����  ��  �Ȼ����� NOW��ì����"
            "\n%s",
            work2);

    if (page != 2 && nowindex > 83) {
      // ƹ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 1);
      // ʸ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, 2);

      buttontype = WINDOW_BUTTONTYPE_NEXT;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN;

    } else {
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    }

    break;

    // END�׷º뼰��ǩ  ��
  case 2:
    // ��  �׷º뼰��������
    nowindex = NPC_EndFlgCheck(meindex, talker, now);

    // ��ʸ����  ��
    page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);

    if (page == 0 || page == 1) {
      i = 0;
    } else {
      i = 83;
    }
    work2[0] = 0;

    // ʸ����
    for (; i < nowindex; i++) {
      sprintf(work, "%d,", now[i]);
      strcat(work2, work);
    }
    sprintf(token,
            "������������������������Ѩ�������� "
            "\n�����ػ�����  ��  �Ȼ����� END��ì����"
            "\n%s",
            work2);

    if (page != 2 && nowindex > 83) {
      // ƹ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 2);
      // ʸ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, 2);

      buttontype = WINDOW_BUTTONTYPE_NEXT;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN;

    } else {
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    }

    break;

    // NOW�׷º뼰���  ��
  case 4: {

    // NOW�׷º���������
    nowindex = NPC_NowFlgCheck(meindex, talker, now);

    if (NPC_FlgCheckMain(meindex, talker, nowindex, now, work2) == FALSE) {
      return;
    }

    sprintf(token,
            "������������������������Ѩ�������� "
            "\n�����ػ�����  ��  �Ȼ����� NOW��ì����"
            "\n%s",
            work2);

    page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);
    nowindex = (nowindex / ((6 * page) + 1));

    if (page != 16 && nowindex != 0) {
      // ƹ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 4);
      // ʸ����
      page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);
      page++;
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, page);

      buttontype = WINDOW_BUTTONTYPE_NEXT;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN;
    } else {
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    }

  } break;

    // END�׷º뼰���  ��
  case 5: {
    // END�׷º뼰��������
    nowindex = NPC_EndFlgCheck(meindex, talker, now);

    if (NPC_FlgCheckMain(meindex, talker, nowindex, now, work2) == FALSE) {
      return;
    }

    sprintf(token,
            "������������������������Ѩ�������� "
            "\n�����ػ�����  ��  �Ȼ����� END��ì����"
            "\n%s",
            work2);

    page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);

    nowindex = (nowindex / ((6 * page) + 1));

    if (page != 16 && nowindex != 0) {
      // ƹ����
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 5);
      // ʸ����
      page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);
      page++;
      CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, page);

      buttontype = WINDOW_BUTTONTYPE_NEXT;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN;
    } else {
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    }
  }
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-���ƥ˪������--*/
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
�ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
-------------------------------------------*/
void NPC_CheckManWindowTalked(int meindex, int talkerindex, int seqno,
                              int select, char *data) {
  int datano;

  if (NPC_Util_CharDistance(talkerindex, meindex) > 2)
    return;

  datano = atoi(data);

  if (select == WINDOW_BUTTONTYPE_OK) {
    NPC_CheckMan_selectWindow(meindex, talkerindex, 0);
  } else if (select == WINDOW_BUTTONTYPE_CANCEL) {
    return;
  }

  switch (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT)) {
  case 1:
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANTSEC) == 2) {
      NPC_CheckMan_selectWindow(meindex, talkerindex, 1);
    }
    break;

  case 2:
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANTSEC) == 2) {
      NPC_CheckMan_selectWindow(meindex, talkerindex, 2);
    }
    break;

  case 4:
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANTSEC) >= 2) {
      NPC_CheckMan_selectWindow(meindex, talkerindex, 4);
    }

  case 5:
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANTSEC) >= 2) {
      NPC_CheckMan_selectWindow(meindex, talkerindex, 5);
    }

    break;
  }

  switch (datano) {
  case 1:
    NPC_CheckMan_selectWindow(meindex, talkerindex, 1);
    break;

  case 2:
    NPC_CheckMan_selectWindow(meindex, talkerindex, 2);
    break;

  case 4:
    NPC_CheckMan_selectWindow(meindex, talkerindex, 4);
    break;

  case 5:
    NPC_CheckMan_selectWindow(meindex, talkerindex, 5);
    break;
  }
}

/*
 *����ì����  �׷º�ë������������
 */
int NPC_NowFlgCheck(int meindex, int talker, int nowflg[MAXEVENTFLG]) {
  int i = 0;
  int j = 0;

  for (i = 0; i < MAXEVENTFLG; i++) {
    if (NPC_NowEventCheckFlg(talker, i) == TRUE) {
      nowflg[j] = i;
      j++;
    }
  }

  return j;
}

/*
 *����ì������
 *�׷º�ë������������
 */
int NPC_EndFlgCheck(int meindex, int talker, int nowflg[MAXEVENTFLG]) {
  int i = 0;
  int j = 0;

  for (i = 0; i < MAXEVENTFLG; i++) {

    if (NPC_EventCheckFlg(talker, i) == TRUE) {
      nowflg[j] = i;
      j++;
    }
  }

  return j;
}

/*
 * ���
 *
 */
BOOL NPC_FlgCheckMain(int meindex, int talker, int nowindex,
                      int now[MAXEVENTFLG], char *work2) {
  int page;
  int max;
  int i;
  int shou;
  int j = 1;
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  char work[512];
  char buf[40];
  char buf2[42];

  // ��ʸ����  ��
  page = CHAR_getWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC);

  if (page == 0) {
    page = 1;
    CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANTSEC, 1);
  }

  if (page == 1) {
    i = 0;
    if (nowindex > 6) {
      max = 7;
    } else {
      max = nowindex;
    }
  } else {
    max = (page * 6) + 1;
    i = max - 7;
    shou = nowindex / max;

    if (shou == 0) {
      max = nowindex;
    } else {
      i = max - 7;
    }
  }

  // ��ì��������ƻ�̤��ľ�����°����̻�ë  �ĳ��
  if (NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr)) == NULL) {
    print("NPC_CheckMan:GetArgStrErr");
    return FALSE;
  }

  work[0] = 0;
  work2[0] = 0;

  // ʸ����
  for (; i < max; i++) {
    sprintf(work, "#%d:", now[i]);
    j = 1;
    while (getStringFromIndexWithDelim(argstr, "|", j, buf, sizeof(buf)) !=
           FALSE) {
      j++;
      if (strstr(buf, work) != NULL) {
        sprintf(buf2, "%s\n", buf);
        strcat(work2, buf2);
        break;
      }
    }
  }

  return TRUE;
}
