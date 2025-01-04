#include "version.h"
#include "npc_charm.h"
#include "char.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "object.h"

#define RATE 10     //
#define CHARMHEAL 5 //
#define WARU 3      //

static void NPC_Charm_selectWindow(int meindex, int toindex, int num);
int NPC_CharmCost(int meindex, int talker);
void NPC_CharmUp(int meindex, int talker);

BOOL NPC_CharmInit(int meindex) {
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPECHARM);
  return TRUE;
}

void NPC_CharmTalked(int meindex, int talkerindex, char *szMes, int color) {

  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  /*--  �������¾���������--*/
  if (NPC_Util_isFaceToFace(meindex, talkerindex, 2) == FALSE) {
    /* ���������  ���� */
    if (NPC_Util_isFaceToChara(talkerindex, meindex, 1) == FALSE)
      return;
  }

  NPC_Charm_selectWindow(meindex, talkerindex, 0);
}

static void NPC_Charm_selectWindow(int meindex, int toindex, int num) {

  char token[1024];
  char escapedname[1024];
  int fd = getfdFromCharaIndex(toindex);
  int buttontype = 0;
  int windowtype = 0;
  int windowno = 0;
  int cost = 0;
  int chartype;

  /*--���̼������������������⻥�����м�ƥ�Ʊ�ɬ��--*/
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (num) {
  case 0:
    /*--��      --*/
    sprintf(token, "4\n ������       ����ʦ     "
                   "\n\n ����ӭ���٣�����Ҫ��ʲ���أ�"
                   "\n "
                   "\n\n������������������������ "
                   "\n\n���������� ʲ��Ҳ���� �� ");

    buttontype = WINDOW_BUTTONTYPE_NONE;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_CHARM_START;
    break;

  case 1:
    cost = NPC_CharmCost(meindex, toindex);
    if (cost == -1) {
      sprintf(token, "��������         ����ʦ      "
                     "\n\n�����������������������"
                     "\n\n����������Զ�����������ࡡ�");
      buttontype = WINDOW_BUTTONTYPE_OK;
    } else {
      sprintf(token,
              "��������         ����ʦ       "
              "\n������,ǧ���ػݼ�ʵʩ��??"
              "\n\n Ҫ����������������Ļ�"
              "\n����������Ҫ%6d��stoneࡣ�"
              "\n\n��������ʹ����Ҳ������",
              cost);
      buttontype = WINDOW_BUTTONTYPE_YESNO;
    }
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_CHARM_END;

    break;

  case 2:
    cost = NPC_CharmCost(meindex, toindex);
    chartype = CHAR_getInt(toindex, CHAR_IMAGETYPE);

    if (cost > CHAR_getInt(toindex, CHAR_GOLD)) {
      sprintf(token, "��������         ����ʦ      "
                     "\n\n���� ��ѽ!���Ǯ����ࡣ�"
                     "\n����    ��ȥ���Ǯ�´������ɣ�");

    } else {
      NPC_CharmUp(meindex, toindex);

      /*--ƽ�ҷ¼�������巽�Ȼ�����������ë  ��������--*/
      switch (chartype) {
      case CHAR_IMAGETYPE_GIRL:
        sprintf(token, "��������         ����ʦ       "
                       "\n\n ����,������������������ޣ�"
                       "\n\n�������������ɰ����أ�");

        break;
      case CHAR_IMAGETYPE_BOY:
        sprintf(token, "��������         ����ʦ     "
                       "\n\n ����,������������������ޣ�"
                       "\n\n�����ǲ��Ǳȸղ�Ҫ˧�����أ�");
        break;

      case CHAR_IMAGETYPE_CHILDBOY:
      case CHAR_IMAGETYPE_CHILDGIRL:
        sprintf(token, "��������         ����ʦ      "
                       "\n\n ����,������������������ޣ�"
                       "\n\n����  ��ı�úÿɰ�ࡣ�");
        break;

      case CHAR_IMAGETYPE_MAN:
        sprintf(token, "��������         ����ʦ       "
                       "\n\n ����,������������������ޣ�"
                       "\n\n ��������ø������أ�");
        break;

      case CHAR_IMAGETYPE_WOMAN:
        sprintf(token, "��������         ����ʦ      "
                       "\n\n ����,������������������ޣ�"
                       "\n\n    �����Ǳ�������������أ�");
        break;
      }
    }

    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_CHARM_END;
    break;
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-���ƥ˪������--*/
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
�ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
-------------------------------------------*/
void NPC_CharmWindowTalked(int meindex, int talkerindex, int seqno, int select,
                           char *data) {
  if (NPC_Util_CharDistance(talkerindex, meindex) > 2)
    return;

  switch (seqno) {
  case CHAR_WINDOWTYPE_CHARM_START:
    if (atoi(data) == 2) {
      NPC_Charm_selectWindow(meindex, talkerindex, 1);
    }
    break;

  case CHAR_WINDOWTYPE_CHARM_END:
    if (select == WINDOW_BUTTONTYPE_YES) {
      NPC_Charm_selectWindow(meindex, talkerindex, 2);
    }
    break;
  }
}

void NPC_CharmUp(int meindex, int talker) {
  int cost;
  int i;
  int petindex;
  char petsend[64];

  cost = NPC_CharmCost(meindex, talker);
  CHAR_setInt(talker, CHAR_GOLD, CHAR_getInt(talker, CHAR_GOLD) - cost);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  if (CHAR_getInt(talker, CHAR_CHARM) + CHARMHEAL >= 100) {
    CHAR_setInt(talker, CHAR_CHARM, 100);
  } else {
    CHAR_setInt(talker, CHAR_CHARM,
                (CHAR_getInt(talker, CHAR_CHARM) + CHARMHEAL));
  }

  CHAR_complianceParameter(talker);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_CHARM);

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);
    if (petindex == -1)
      continue;
    if (!CHAR_CHECKINDEX(talker))
      continue;

    CHAR_complianceParameter(petindex);
    sprintf(petsend, "K%d", i);
    CHAR_sendStatusString(talker, petsend);
  }
}

int NPC_CharmCost(int meindex, int talker) {
  int cost;
  int level;
  int charm;
  int trans;

  level = CHAR_getInt(talker, CHAR_LV);
  charm = CHAR_getInt(talker, CHAR_CHARM);
  trans = CHAR_getInt(talker, CHAR_TRANSMIGRATION);

  if (charm >= 100)
    return -1;

  if (charm <= 1)
    charm = WARU;

  /*-- ��ңֿ --*/
  cost = level * RATE * (charm / WARU) * (trans + 1);

  return cost;
}
