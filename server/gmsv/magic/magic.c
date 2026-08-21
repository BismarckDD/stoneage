#include "version.h"
//
#include "common.h"
#include "gmsv_server.h"
//
#include "battle.h"
#include "battle_event.h"
#include "battle_magic.h"
#include "char_base.h"
#include "char_data.h"
#include "family.h"
#include "handletime.h"
#include "item.h"
#include "magic.h"
#include "magic_base.h"
#include "magic_field.h"

#ifdef _ITEM_ATTSKILLMAGIC
#include "pet_skill.h"
#endif
#ifdef _NO_USE_MAGIC_MAP
#include "config_file.h"
#endif
#define MP_RATE 0.7

int MAGIC_Use(int from_char_index, int haveitem_index, int to_char_index) {
  int item_index;
  int magicid, marray, mp;
  int ret;
  MAGIC_CALLFUNC func;

  item_index = CHAR_getItemIndex(from_char_index, haveitem_index);
  if (!ITEM_CHECKINDEX(item_index))
    return FALSE;
  magicid = ITEM_getInt(item_index, ITEM_MAGICID);
  marray = MAGIC_getMagicArray(magicid);
  if (marray == -1)
    return FALSE;

  func = MAGIC_getMagicFuncPointer(
      MAGIC_getChar(marray, MAGIC_FUNCNAME)); // MAGIC_NAME

  if ((mp = ITEM_getInt(item_index, ITEM_MAGICUSEMP)) < 0) {
    print("err mp:%d  -%s-%d\n", mp, ITEM_getChar(item_index, ITEM_NAME),
          ITEM_getInt(item_index, ITEM_ID));
  }
  // shan add begin
  if (CHAR_getInt(from_char_index, CHAR_FMINDEX) >= 1) {
    // 光精
    if (CHAR_getInt(from_char_index, CHAR_FMSPRITE) == 0) {
      if ((marray >= 0 && marray <= 31) || (marray >= 105 && marray <= 107))
        mp *= MP_RATE;
    }
    // 暗精
    if (CHAR_getInt(from_char_index, CHAR_FMSPRITE) == 1) {
      if (marray >= 32 && marray <= 81)
        mp *= MP_RATE;
    }
  }
  // shan end
  if (func) {
    ret = func(from_char_index, to_char_index, marray, mp);
  } else {
    ret = FALSE;
  }
  return ret;
}

int MAGIC_GetArrayNo(int from_char_index, int haveitem_index) {
  int item_index;
  int magicid;
  item_index = CHAR_getItemIndex(from_char_index, haveitem_index);
  if (!ITEM_CHECKINDEX(item_index))
    return FALSE;

  magicid = ITEM_getInt(item_index, ITEM_MAGICID);

  return magicid;
}
int MAGIC_DirectUse(int from_char_index, int magicid, int to_char_index,
                    int itemnum) {
  MAGIC_CALLFUNC func;
  int mp, ret, marray;
  int item_index = -1;
#ifdef _MAGIC_NOCAST // 沉默状态无法施法
  if (CHAR_getWorkInt(from_char_index, CHAR_WORKNOCAST) > 0) {
    CHAR_talkToCli(from_char_index, -1, "沉默中无法吟唱咒术", CHAR_COLORYELLOW);
    return FALSE;
  }
#endif
  if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    item_index = CHAR_getItemIndex(from_char_index, itemnum);
    if (!ITEM_CHECKINDEX(item_index)) {
      print("ANDY err MAGIC_DirectUse() item_index:%d !\n", item_index);
      return FALSE;
    }
  } else {
    item_index = itemnum;
  }
  marray = MAGIC_getMagicArray(magicid);
  func = MAGIC_getMagicFuncPointer(MAGIC_getChar(marray, MAGIC_FUNCNAME));
#ifdef _NO_USE_MAGIC_MAP
  int index;
  for (index = 0; index < 30; index++) {
    if (CHAR_getInt(from_char_index, CHAR_FLOOR) == getNoMagicMap(index)) {
      if (strcmp(MAGIC_getChar(marray, MAGIC_FUNCNAME), "MAGIC_MagicDef") ==
          0) {
        char ansmsg[256];
        strncpysafe(ansmsg, sizeof(ansmsg), "该地图禁止使用光镜守精灵。");
        CHAR_talkToCli(from_char_index, -1, ansmsg, CHAR_COLORWHITE);
        return FALSE;
      }
    }
  }
#endif

  if ((mp = ITEM_getInt(item_index, ITEM_MAGICUSEMP)) < 0) {
  }
  // shan add begin
  if (CHAR_getInt(from_char_index, CHAR_FMINDEX) >= 1) {
    // 光精
    if (CHAR_getInt(from_char_index, CHAR_FMSPRITE) == 0) {
      if ((marray >= 0 && marray <= 31))
        mp *= MP_RATE;
    }
    // 暗精
    if (CHAR_getInt(from_char_index, CHAR_FMSPRITE) == 1) {
      if (marray >= 32 && marray <= 81)
        mp *= MP_RATE;
    }
  }
  if (func) {
    ret = func(from_char_index, to_char_index, marray, mp);
  } else {
    ret = FALSE;
  }
  return ret;
}
#ifdef _MAGIC_FEATHERS
int MAGIC_Feathers(int from_char_index, int to_char_index, int marray, int mp) {
  int battlemode;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }

  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  char buf[256];
  int ff = 0, fx = 0, fy = 0, flg = 0;
  if (sscanf(MAGIC_getChar(marray, MAGIC_OPTION), "%d %d %d %d", &flg, &ff, &fx,
             &fy) == 4) {

#ifdef _ITEM_CHECKWARES
    if (CHAR_CheckInItemForWares(from_char_index, 0) == FALSE) {
      CHAR_talkToCli(from_char_index, -1, "携带货物无法使用。",
                     CHAR_COLORYELLOW);
      return;
    }
#endif
    sprintf(buf, "%d", flg);
    MAGIC_setChar(marray, MAGIC_OPTION, buf);
    MAGIC_setChar(marray, MAGIC_COMMENT, "未曾记录地点");
    if (ITEM_WarpForAny(from_char_index, ff, fx, fy, flg) == FALSE)
      return;

  } else if (sscanf(MAGIC_getChar(marray, MAGIC_OPTION), "%d", &flg) == 1) {
    int Mf, Mx, My;
    Mf = CHAR_getInt(from_char_index, CHAR_FLOOR);
    Mx = CHAR_getInt(from_char_index, CHAR_X);
    My = CHAR_getInt(from_char_index, CHAR_Y);
    char escapeshowstring[64];
    char *showstr = MAP_getfloorShowstring(Mf);
    getStringFromIndexWithDelim(showstr, "|", 1, escapeshowstring,
                                sizeof(escapeshowstring));
    sprintf(buf, "%d %d %d %d", flg, Mf, Mx, My);
    MAGIC_setChar(marray, MAGIC_OPTION, buf);
    sprintf(buf, "坐标记录点位于(%s,%d,%d)", escapeshowstring, Mx, My);
    MAGIC_setChar(marray, MAGIC_COMMENT, buf);
    CHAR_talkToCli(from_char_index, -1, buf, CHAR_COLORYELLOW);
  }
}
#endif

int MAGIC_Recovery(const int from_char_index, // 施术玩家
                   const int to_char_index,   // 目标玩家
                   const int magic_index,     // 魔法id
                   const int mp) {            // 耗魔mp
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    // 战斗中使用恢复魔法
#ifdef _PREVENT_TEAMATTACK // 恩惠不得对敌方使用
    int battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
    if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
      if (BATTLE_CheckSameSide(from_char_index, to_char_index) == 0) { // 不同边
        int battleindex =
            CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
        BATTLE_NoAction(battleindex,
                        BATTLE_Index2No(battleindex, from_char_index));
        CHAR_talkToCli(from_char_index, -1, "滋润精灵...不得施予非玩家敌方。",
                       CHAR_COLORYELLOW);
        return FALSE;
      }
    }
#endif
    if (to_char_index == 22) {
      logErr("ERROR: to_char_index:%d\n", to_char_index);
      return FALSE;
    } // shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
    MAGIC_Recovery_Battle(from_char_index, to_char_index, magic_index, mp);
  } else {
    // 非战斗状态使用恢复魔法
    if (CHAR_CHECKINDEX(to_char_index) == FALSE)
      return FALSE; // shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
    MAGIC_Recovery_Field(from_char_index, magic_index);
  }
  return TRUE;
}

int MAGIC_OtherRecovery(int from_char_index, int to_char_index, int marray,
                        int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE; // ｛撩
  int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE; //   井匀凶日母丢
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
#ifdef _PREVENT_TEAMATTACK // 滋润 不得使用敌方
    int battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
    if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
      if (BATTLE_CheckSameSide(from_char_index, to_char_index) == 0) { // 不同边
        int battleindex =
            CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
        BATTLE_NoAction(battleindex,
                        BATTLE_Index2No(battleindex, from_char_index));
        CHAR_talkToCli(from_char_index, -1, "滋润精灵...不得施予非玩家敌方。",
                       CHAR_COLORYELLOW);
        return FALSE;
      }
    }
#endif
    MAGIC_Recovery_Battle(from_char_index, to_char_index, marray, mp);
  } else {
    if (CHAR_CHECKINDEX(to_char_index) == FALSE)
      return FALSE; // 目标玩家不存在
    MAGIC_OtherRecovery_Field(from_char_index, to_char_index, marray);
  }
  return TRUE;
}

int MAGIC_FieldAttChange(int from_char_index, int to_char_index, int marray,
                         int mp) {
  int battlemode;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_FieldAttChange_Battle(from_char_index, to_char_index, marray,
                                       mp);
  } else {
    return FALSE;
  }
}

int MAGIC_StatusChange(int from_char_index, int to_char_index, int marray,
                       int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
#ifdef _PREVENT_TEAMATTACK // 异常状态魔法不得对友方使用
    int battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
    if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
      if (BATTLE_CheckSameSide(from_char_index, to_char_index) == 1) { // 同边
        int battleindex =
            CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
        BATTLE_NoAction(battleindex,
                        BATTLE_Index2No(battleindex, from_char_index));
        CHAR_talkToCli(from_char_index, -1, "异常状态精灵...不得施予友方。",
                       CHAR_COLORYELLOW);
        return FALSE;
      }
    }
#endif
    return MAGIC_StatusChange_Battle(from_char_index, to_char_index, marray,
                                     mp);
  } else {
    // 非战斗状态使用异常状态魔法
    return FALSE;
  }
}

#ifdef _OTHER_MAGICSTAUTS
int MAGIC_MagicStatusChange(int from_char_index, int to_char_index, int marray,
                            int mp) {
  int battlemode;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_MagicStatusChange_Battle(from_char_index, to_char_index,
                                          marray, mp);
  } else {
    return FALSE;
  }
}
#endif

#ifdef _MAGIC_DEEPPOISON // 新增ver2,主要设定毒状态5回未解时人物会阵亡

int MAGIC_StatusChange2(int from_char_index, int to_char_index, int marray,
                        int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_StatusChange_Battle2(from_char_index, to_char_index, marray,
                                      mp);
  } else {
    return FALSE;
  }
}
#endif

int MAGIC_StatusRecovery(int from_char_index, int to_char_index, int marray,
                         int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_StatusRecovery_Battle(from_char_index, to_char_index, marray,
                                       mp);
  } else {
    return FALSE;
  }
}

int MAGIC_MagicDef(int from_char_index, int to_char_index, int marray, int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
#ifdef _PREVENT_TEAMATTACK // 光镜守..不得使用敌方
    if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      if (BATTLE_CheckSameSide(from_char_index, to_char_index) == 0) { // 不同边
        int battleindex =
            CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
        BATTLE_NoAction(battleindex,
                        BATTLE_Index2No(battleindex, from_char_index));
        CHAR_talkToCli(from_char_index, -1, "光镜守..不得施予敌方。",
                       CHAR_COLORYELLOW);
        return FALSE;
      }
    }
#endif
    return MAGIC_MagicDef_Battle(from_char_index, to_char_index, marray, mp);
  } else {
    return FALSE;
  }
}

// 复活魔法
int MAGIC_Ressurect(int from_char_index, int to_char_index, int marray,
                    int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_Ressurect_Battle(from_char_index, to_char_index, marray, mp);
  } else {
    return FALSE;
  }
}

// 属性反转.
int MAGIC_AttReverse(int from_char_index, int to_char_index, int marray,
                     int mp) {
  int battlemode;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_AttReverse_Battle(from_char_index, to_char_index, marray, mp);
  } else {
    return FALSE;
  }
}

int MAGIC_ResAndDef(int from_char_index, int to_char_index, int marray,
                    int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE; // ｛撩
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE; //
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_ResAndDef_Battle(from_char_index, to_char_index, marray, mp);
  } else {
    return FALSE;
  }
}

#ifdef _ATTACK_MAGIC

int MAGIC_AttMagic(int from_char_index, int to_char_index, int marray, int mp) {
  int battlemode;

  if (FALSE == CHAR_CHECKINDEX(from_char_index))
    return FALSE;

  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (BATTLE_CHARMODE_INIT == battlemode)
    return FALSE;
  // 若是宠物魔法,不扣mp
  if (CHAR_getInt(from_char_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
      return FALSE;

    CHAR_setInt(from_char_index, CHAR_MP,
                CHAR_getInt(from_char_index, CHAR_MP) - mp);
  }

  if (TRUE == IsBATTLING(from_char_index))
    return MAGIC_AttMagic_Battle(from_char_index, to_char_index, marray, mp);

  return FALSE;
}

#endif

#ifdef _ITEM_METAMO
int MAGIC_Metamo(int from_char_index, int to_char_index, int marray, int mp) {
  int battlemode, haveindex, power, i;
  char msg[128];

  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;

  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  // 爵  钒铵凛反  骰允月
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  // 爵    分匀凶日
  if (IsBATTLING(from_char_index) == TRUE) {
    // MAGIC_Recovery_Battle( from_char_index, to_char_index, marray, mp );
    // return FALSE;
    to_char_index = BATTLE_No2Index(
        CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX), to_char_index);
    // print(" battle_metamo:%d ", to_char_index);
  }

  if (CHAR_CHECKINDEX(to_char_index) == FALSE)
    return FALSE;

  if (CHAR_getInt(from_char_index, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(from_char_index, -1, "无法变身，骑乘中不能变身！",
                   CHAR_COLORWHITE);
    return FALSE;
  }
#ifdef _FIX_METAMORIDE
  if (CHAR_CHECKJOINENEMY(from_char_index) == TRUE) {
    CHAR_talkToCli(from_char_index, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORYELLOW);
    return FALSE;
  }
#else
  if (CHAR_getInt(from_char_index, CHAR_BASEIMAGENUMBER) == 100259) {
    CHAR_talkToCli(from_char_index, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORWHITE);
    return FALSE;
  }
#endif
#ifdef _FIXBUG_ATTACKBOW
  {
    int armindex = CHAR_getItemIndex(from_char_index, CHAR_ARM);
    if (ITEM_CHECKINDEX(armindex) == TRUE) {
      int armtype = BATTLE_GetWepon(from_char_index);
      if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
          armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
        CHAR_talkToCli(from_char_index, -1, "使用远距离武器无法变身！",
                       CHAR_COLORYELLOW);
        return FALSE;
      }
    }
  }
#endif
  haveindex = -1;
  for (i = 0; i < 5; i++) {
    if (CHAR_getCharPet(from_char_index, i) == to_char_index) {
      haveindex = i;
      break;
    }
  }
  if (haveindex == -1 && from_char_index != to_char_index) {
    CHAR_talkToCli(from_char_index, -1, "无法变身，只能变成自己的宠物！",
                   CHAR_COLORWHITE);
    return FALSE;
  }

  if (CHAR_getInt(to_char_index, CHAR_BASEIMAGENUMBER) == 100259) {
    CHAR_talkToCli(from_char_index, -1, "不能变身成加美！", CHAR_COLORWHITE);
    return FALSE;
  }

  power = atoi(MAGIC_getChar(marray, MAGIC_OPTION));

  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  CHAR_send_P_StatusString(from_char_index, CHAR_P_STRING_MP);

  if (to_char_index != from_char_index) {
    CHAR_setWorkInt(from_char_index, CHAR_WORKITEMMETAMO,
                    NowTime.tv_sec + power);
    sprintf(msg, "变身成%s！", CHAR_getChar(to_char_index, CHAR_NAME));
  } else {
    CHAR_setWorkInt(from_char_index, CHAR_WORKITEMMETAMO, 0);
    sprintf(msg, "变回自己！");
  }
  CHAR_talkToCli(from_char_index, -1, msg, CHAR_COLORWHITE);

  CHAR_setInt(from_char_index, CHAR_BASEIMAGENUMBER,
              CHAR_getInt(to_char_index, CHAR_BASEBASEIMAGENUMBER));
  CHAR_complianceParameter(from_char_index);
  CHAR_sendCToArroundCharacter(
      CHAR_getWorkInt(from_char_index, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(from_char_index, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  // #endif

  return TRUE;
}
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill(int from_char_index, int to_char_index, int marray, int mp) {
  int battlemode;
  int battleindex = -1;
  char *magicarg, funName[256];
  int skillID = -1;
  PETSKILL_CALLFUNC func;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  if (IsBATTLING(from_char_index) == FALSE)
    return FALSE;
  battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return FALSE;

  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT)
    return FALSE;
  if (to_char_index < 0 || to_char_index > 19)
    return FALSE;
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp) {
    if (MAGIC_getInt(marray, MAGIC_TARGET) != 8) {
      return FALSE;
    }
  }

  memset(funName, 0, sizeof(char) * 256);
  // MAGIC_Recovery_Battle( from_char_index, to_char_index, marray, mp );
  magicarg = MAGIC_getChar(marray, MAGIC_OPTION);

  if (magicarg == "\0")
    return FALSE;
  if (strstr(magicarg, ";") != NULL) {
    char buff1[256];
    if (getStringFromIndexWithDelim(magicarg, ";", 1, buff1, sizeof(buff1)) ==
        FALSE)
      return FALSE;
    snprintf(funName, sizeof(funName), buff1);
    if (getStringFromIndexWithDelim(magicarg, ";", 2, buff1, sizeof(buff1)) ==
        FALSE)
      return FALSE;
    skillID = atoi(buff1);
  } else
    return FALSE;
  func = PETSKILL_getPetskillFuncPointer(funName);
  if (!func) {
    print("\n *func not FOUND !!");
    return FALSE;
  }
  if (func(from_char_index, to_char_index, skillID, "MAGICSKILL") == FALSE)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  return TRUE;
}
#endif
#ifdef _MAGIC_BARRIER // vincent  精灵:魔障
int MAGIC_Barrier(int from_char_index, int to_char_index, int marray, int mp) {
  int i, turn, perStatus;
  int ToList[SIDE_OFFSET * 2 + 1];
  char *magicarg;
  int attackNo = 0;
  int battleindex, Success = 0;
  char szTurn[] = "turn";
  char szSuccess[] = "成";
  char *pszP;
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  magicarg = MAGIC_getChar(marray, MAGIC_OPTION);
  if (magicarg == "\0") {
    print("\n magicarg == NULL ");
    return FALSE;
  }
  pszP = magicarg;
  // 读取作用回合数
  if ((pszP = strstr(pszP, szTurn)) != NULL) {
    pszP += sizeof(szTurn);
    sscanf(pszP, "%d", &turn);
  }
  if ((pszP = strstr(pszP, szSuccess)) != NULL) {
    pszP += sizeof(szSuccess);
    sscanf(pszP, "%d", &Success);
  }
  battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
  attackNo = BATTLE_Index2No(battleindex, from_char_index);
  BATTLE_MultiList(battleindex, to_char_index, ToList);
  // 施魔法动画效果
  BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER,
                     SPR_hoshi);

  from_char_index = BATTLE_No2Index(battleindex, attackNo);
  // check是否战斗中
  if (IsBATTLING(from_char_index) == TRUE) {

    for (i = 0; ToList[i] != -1; i++) {
      to_char_index = BATTLE_No2Index(battleindex, ToList[i]);
      if (BATTLE_StatusAttackCheck(from_char_index, to_char_index,
                                   BATTLE_ST_BARRIER, Success, 30, 1.0,
                                   &perStatus) == TRUE) {
        CHAR_setWorkInt(to_char_index, CHAR_WORKBARRIER, turn + 1);
        //		       BATTLE_BadStatusString( ToList[i],
        // BATTLE_ST_BARRIER );
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}
#endif
#ifdef _MAGIC_NOCAST // vincent  精灵:沉默
int MAGIC_Nocast(int from_char_index, int to_char_index, int marray, int mp) {

  int battlemode;
  int i, turn, perStatus;
  int ToList[SIDE_OFFSET * 2 + 1];
  char *magicarg;
  int attackNo = 0;
  int battleindex, Success = 0;
  char szTurn[] = "turn";
  char szSuccess[] = "成";
  char *pszP;

  // check index
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;

  battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);

  // 爵  钒铵凛反  骰允月//??
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }

  // mp不足
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  // 消耗mp
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  // 魔法名称
  magicarg = MAGIC_getChar(marray, MAGIC_OPTION);
  if (magicarg == "\0") {
    print("\n magicarg == NULL ");
    return FALSE;
  }
  pszP = magicarg;

  // 读取作用回合数
  if ((pszP = strstr(pszP, szTurn)) != NULL) {
    pszP += sizeof(szTurn);
    sscanf(pszP, "%d", &turn);
  }
  if ((pszP = strstr(pszP, szSuccess)) != NULL) {
    pszP += sizeof(szSuccess);
    sscanf(pszP, "%d", &Success);
  }
  battleindex = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEINDEX);
  attackNo = BATTLE_Index2No(battleindex, from_char_index);
  BATTLE_MultiList(battleindex, to_char_index, ToList);
  // 施魔法动画效果
  BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER,
                     SPR_hoshi);
  from_char_index = BATTLE_No2Index(battleindex, attackNo);
  // check是否战斗中
  if (IsBATTLING(from_char_index) == TRUE) {

    for (i = 0; ToList[i] != -1; i++) {
      to_char_index = BATTLE_No2Index(battleindex, ToList[i]);
      if (BATTLE_StatusAttackCheck(from_char_index, to_char_index,
                                   BATTLE_ST_NOCAST, Success, 30, 1.0,
                                   &perStatus) == TRUE &&
          CHAR_getInt(to_char_index, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
        CHAR_setWorkInt(to_char_index, CHAR_WORKNOCAST, turn);
        GmsvServer_NC_send(getfdFromfrom_char_index(to_char_index), 1);
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}
#endif

#ifdef _MAGIC_WEAKEN // vincent  精灵:虚弱
int MAGIC_Weaken(int from_char_index, int to_char_index, int marray, int mp) {
  if (CHAR_CHECKINDEX(from_char_index) == FALSE)
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return FALSE;
  }
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (IsBATTLING(from_char_index) == TRUE) {
    return MAGIC_ParamChange_Turn_Battle(from_char_index, to_char_index, marray,
                                         mp);
  } else {
    return FALSE;
  }
}
#endif

#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon(int from_char_index, int to_char_index, int marray,
                       int mp) {
  if (FALSE == CHAR_CHECKINDEX(from_char_index))
    return FALSE;
  const int battlemode = CHAR_getWorkInt(from_char_index, CHAR_WORKBATTLEMODE);
  if (BATTLE_CHARMODE_INIT == battlemode)
    return FALSE;
  if (CHAR_getInt(from_char_index, CHAR_MP) < mp)
    return FALSE;
  CHAR_setInt(from_char_index, CHAR_MP,
              CHAR_getInt(from_char_index, CHAR_MP) - mp);
  if (TRUE == IsBATTLING(from_char_index))
    return MAGIC_ToCallDragon_Battle(from_char_index, to_char_index, marray,
                                     mp);
  return FALSE;
}
#endif
