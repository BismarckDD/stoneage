#include "version.h"
//
#include "common.h"
//
#include "char.h"
#include "char_base.h"
#include "magic.h"
#include "magic_base.h"
#include "magic_field.h"

int MAGIC_Recovery_Field(int char_index, int magic_index) {
  char msgbuf[64];
#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(char_index, "HP") == FALSE)
    return TRUE;
#endif
  const char *magic_arg = MAGIC_getChar(magic_index, MAGIC_OPTION);
  float power = atoi(magic_arg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(char_index);
#endif
  const int prevhp = CHAR_getInt(char_index, CHAR_HP);
  const int workhp = prevhp + (int)power;
  CHAR_setInt(char_index, CHAR_HP,
              min(workhp, CHAR_getWorkInt(char_index, CHAR_WORKMAXHP)));
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    int mypartyarray = -1;
    int oyaindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex)) {
      int i;
      for (i = 0; i < getPartyNum(oyaindex); i++) {
        int workindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + i);
        if (CHAR_CHECKINDEX(workindex)) {
          if (workindex == char_index) {
            mypartyarray = i;
            break;
          }
        }
      }
      if (mypartyarray != -1) {
        for (i = 0; i < getPartyNum(char_index); i++) {
          int index = CHAR_getPartyIndex(char_index, i);
          if (CHAR_CHECKINDEX(index)) {
            if (index != char_index) {
              CHAR_send_N_StatusString(index, mypartyarray,
                                       CHAR_N_STRING_HP | CHAR_N_STRING_MP);
            }
          }
        }
      }
    }
  }
  snprintf(msgbuf, sizeof(msgbuf), "耐久力回复%d",
           min(workhp, CHAR_getWorkInt(char_index, CHAR_WORKMAXHP)) - prevhp);
  CHAR_talkToCli(char_index, -1, msgbuf, CHAR_COLORWHITE);
  return TRUE;
}

int MAGIC_OtherRecovery_Field(int from_char_index, int to_char_index,
                              int magic_index) {
  char msgbuf[64];
#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(to_char_index, "HP") == FALSE)
    return FALSE;
#endif
  const char *magic_arg = MAGIC_getChar(magic_index, MAGIC_OPTION);
  float power = atoi(magic_arg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(to_char_index);
#endif
  const int prevhp = CHAR_getInt(to_char_index, CHAR_HP);
  const int workhp = prevhp + (int)power;
  CHAR_setInt(to_char_index, CHAR_HP,
              min(workhp, CHAR_getWorkInt(to_char_index, CHAR_WORKMAXHP)));
  if (CHAR_getInt(to_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (from_char_index != to_char_index) {
      CHAR_send_P_StatusString(to_char_index, CHAR_P_STRING_HP);
    }
  }
  CHAR_send_P_StatusString(from_char_index,
                           CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (from_char_index != to_char_index) {
    CHAR_PartyUpdate(from_char_index, CHAR_N_STRING_MP);
    CHAR_PartyUpdate(to_char_index, CHAR_N_STRING_HP);
  } else {
    CHAR_PartyUpdate(from_char_index, CHAR_N_STRING_MP | CHAR_N_STRING_HP);
  }
  if (CHAR_getInt(to_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int workindex = CHAR_getCharPet(from_char_index, i);
      if (workindex == to_char_index) {
        CHAR_send_K_StatusString(from_char_index, i, CHAR_K_STRING_HP);
      }
    }
  }

  if (from_char_index != to_char_index) {
    // 施术者为其他玩家恢复生命
    snprintf(
        msgbuf, sizeof(msgbuf), "%s的耐久力回复%d", CHAR_getUseName(to_char_index),
        min(workhp, CHAR_getWorkInt(to_char_index, CHAR_WORKMAXHP)) - prevhp);
    CHAR_talkToCli(from_char_index, -1, msgbuf, CHAR_COLORWHITE);

    if (CHAR_getInt(to_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      // 受术者同样显示
      snprintf(msgbuf, sizeof(msgbuf), "藉由%s耐久力回复%d",
               CHAR_getUseName(from_char_index),
               min(workhp, CHAR_getWorkInt(to_char_index, CHAR_WORKMAXHP)) -
                   prevhp);
      CHAR_talkToCli(to_char_index, -1, msgbuf, CHAR_COLORWHITE);
    }
  } else {
    // 施术者为自身恢复生命
    snprintf(msgbuf, sizeof(msgbuf), "耐久力回复%d",
             min(workhp, CHAR_getWorkInt(from_char_index, CHAR_WORKMAXHP)) -
                 prevhp);
    CHAR_talkToCli(from_char_index, -1, msgbuf, CHAR_COLORWHITE);
  }
  return TRUE;
}
