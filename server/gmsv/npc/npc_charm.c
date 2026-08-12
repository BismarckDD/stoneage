#include "version.h"
#include "npc_charm.h"
#include "char.h"
#include "gmsv_server.h"
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

  /*--  及蟆卞中月井升丹井＂--*/
  if (NPC_Util_isFaceToFace(meindex, talkerindex, 2) == FALSE) {
    /* ㄠ弘伉永玉动  及心 */
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

  /*--它奶件玉它正奶皿丢永本□斥互云云中及匹燮卞涩烂--*/
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (num) {
  case 0:
    /*--蓟      --*/
    sprintf(token, "4\n 　　　       美容师     "
                   "\n\n 　欢迎光临，今天要做什麽呢？"
                   "\n "
                   "\n\n　　　　《　给我魅力　》 "
                   "\n\n　　　　《 什麽也不做 》 ");

    buttontype = WINDOW_BUTTONTYPE_NONE;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_CHARM_START;
    break;

  case 1:
    cost = NPC_CharmCost(meindex, toindex);
    if (cost == -1) {
      sprintf(token, "　　　　         美容师      "
                     "\n\n　　　　你的魅力真是完美"
                     "\n\n　　　　已经无法再提升魅力了。");
      buttontype = WINDOW_BUTTONTYPE_OK;
    } else {
      sprintf(token,
              "　　　　         美容师       "
              "\n　现在,千禧特惠价实施中??"
              "\n\n 要将你的魅力上升五点的话"
              "\n　　　　需要%6d的stone唷！"
              "\n\n　　　　即使后悔也无法恢复唷！",
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
      sprintf(token, "　　　　         美容师      "
                     "\n\n　　 哎呀!你的钱不够唷！"
                     "\n　　    先去存好钱下次再来吧！");

    } else {
      NPC_CharmUp(meindex, toindex);

      /*--平乓仿及正奶皿卞方匀化丢永本□斥毛  尹化心凶--*/
      switch (chartype) {
      case CHAR_IMAGETYPE_GIRL:
        sprintf(token, "　　　　         美容师       "
                       "\n\n 　嗯,这样你的魅力就上升罗！"
                       "\n\n　　　愈来愈可爱了呢！");

        break;
      case CHAR_IMAGETYPE_BOY:
        sprintf(token, "　　　　         美容师     "
                       "\n\n 　嗯,这样你的魅力就上升罗！"
                       "\n\n　　是不是比刚才要帅多了呢？");
        break;

      case CHAR_IMAGETYPE_CHILDBOY:
      case CHAR_IMAGETYPE_CHILDGIRL:
        sprintf(token, "　　　　         美容师      "
                       "\n\n 　嗯,这样你的魅力就上升罗！"
                       "\n\n　　  真的变得好可爱唷！");
        break;

      case CHAR_IMAGETYPE_MAN:
        sprintf(token, "　　　　         美容师       "
                       "\n\n 　嗯,这样你的魅力就上升罗！"
                       "\n\n 　　　变得更酷了呢！");
        break;

      case CHAR_IMAGETYPE_WOMAN:
        sprintf(token, "　　　　         美容师      "
                       "\n\n 　嗯,这样你的魅力就上升罗！"
                       "\n\n    可真是变得愈来愈美了呢！");
        break;
      }
    }

    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_CHARM_END;
    break;
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-仇仇匹霜耨允月--*/
  GmsvServer_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
弁仿奶失件玄井日忒匀化五凶凛卞裟太请今木月［
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

  /*-- 煌遥挚 --*/
  cost = level * RATE * (charm / WARU) * (trans + 1);

  return cost;
}
