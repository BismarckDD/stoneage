#include "version.h"
//
#include "gmsv_server.h"
//
#ifdef _PROFESSION_SKILL // WON ADD
#include "autil.h"
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "char_base.h"
#include "config_file.h"
#include "magic_base.h"
#include "profession_skill.h"
#include "skill.h"
static Professionskill *PROFESSION_skill;

typedef struct tagProfessionSkillFunctionTable {
  char *functionname;
  PROFESSION_SKILL_CALLFUNC func;
  int hash;
} ProfressionSkillFunctionTable;

static ProfressionSkillFunctionTable sProfessionSkillFunctionTable[] = {
    {"PROFESSION_VOLCANO_SPRINGS", PROFESSION_volcano_springs, 0},
    {"PROFESSION_FIRE_BALL", PROFESSION_fire_ball, 0},
    {"PROFESSION_FIRE_SPEAR", PROFESSION_fire_spear, 0},
    {"PROFESSION_SUMMON_THUNDER", PROFESSION_summon_thunder, 0}, // 召雷术
    {"PROFESSION_CURRENT", PROFESSION_current, 0},               // 电流术
    {"PROFESSION_STORM", PROFESSION_storm, 0},                   // 暴风雨
    {"PROFESSION_ICE_ARROW", PROFESSION_ice_arrow, 0},           // 冰箭术
    {"PROFESSION_ICE_CRACK", PROFESSION_ice_crack, 0},           // 冰爆术
    {"PROFESSION_ICE_MIRROR", PROFESSION_ice_mirror, 0},         // 冰镜术
    {"PROFESSION_DOOM", PROFESSION_doom, 0},                     // 世界末日
    {"PROFESSION_BLOOD", PROFESSION_blood, 0},                   // 嗜血成性
    {"PROFESSION_BLOOD_WORMS", PROFESSION_blood_worms, 0},       // 嗜血蛊
    {"PROFESSION_SIGN", PROFESSION_sign, 0},                     // 一针见血
    {"PROFESSION_FIRE_ENCLOSE", PROFESSION_fire_enclose, 0},     // 火附体
    {"PROFESSION_ICE_ENCLOSE", PROFESSION_ice_eclose, 0},        // 冰附体
    {"PROFESSION_THUNDER_ENCLOSE", PROFESSION_thunder_enclose, 0},
    {"PROFESSION_FIRE_PRACTICE", PROFESSION_fire_practice, 0},   // 火魔法熟练度
    {"PROFESSION_ICE_PRACTICE", PROFESSION_ice_practice, 0},     // 冰魔法熟练度
    {"PROFESSION_THUNDER_PRACTICE", PROFESSION_thunder_practice, 0},
    {"PROFESSION_ENCLOSE", PROFESSION_enclose, 0},     // 附身术
    {"PROFESSION_TRANSPOSE", PROFESSION_transpose, 0}, // 移形换位
    // {"PROFESSION_ALCHEMY",	PROFESSION_alchemy, 0},
    {"PROFESSION_CHAIN_ATK", PROFESSION_chain_atk, 0}, // 连环攻击
    {"PROFESSION_AVOID", PROFESSION_avoid, 0},         // 回避
    {"PROFESSION_WEAPON_FOCUS", PROFESSION_weapon_focus, 0}, // 武器专精
    {"PROFESSION_REBACK", PROFESSION_reback, 0},             // 状态回复
    {"PROFESSION_BRUST", PROFESSION_brust, 0},               // 爆击
    {"PROFESSION_CHAIN_ATK_2", PROFESSION_chain_atk_2, 0}, // 双重攻击
    {"PROFESSION_SCAPEGOAT", PROFESSION_scapegoat, 0}, // 舍已为友
    {"PROFESSION_ENRAGE", PROFESSION_enrage, 0}, // 激化攻击
    {"PROFESSION_ENERGY_COLLECT", PROFESSION_energy_collect, 0}, // 能量聚集
    {"PROFESSION_FOCUS", PROFESSION_focus, 0},                   // 专注战斗
    {"PROFESSION_SHIELD_ATTACK", PROFESSION_shield_attack, 0}, // 盾击
    {"PROFESSION_DUAL_WEAPON", PROFESSION_dual_weapon, 0}, // 二刀流
    {"PROFESSION_DEFLECT", PROFESSION_deflect, 0},         // 格档
    {"PROFESSION_THROUGH_ATTACK", PROFESSION_through_attack, 0}, // 贯穿攻击
    {"PROFESSION_CAVALRY", PROFESSION_cavalry, 0},         // 座骑攻击
    {"PROFESSION_DEAD_ATTACK", PROFESSION_dead_attack, 0}, // 濒死攻击
    {"PROFESSION_CONVOLUTE", PROFESSION_convolute, 0}, // 回旋攻击
    {"PROFESSION_CHAOS", PROFESSION_chaos, 0},         // 混乱攻击

    // 猎人
    {"PROFESSION_TRACK", PROFESSION_track, 0},           // 追寻敌踪
    {"PROFESSION_ESCAPE", PROFESSION_escape, 0},         // 回避战斗
    {"PROFESSION_DOCILE", PROFESSION_docile, 0},         // 驯伏宠物
    {"PROFESSION_TRAP", PROFESSION_trap, 0},             // 陷阱
    {"PROFESSION_ENRAGE_PET", PROFESSION_enrage_pet, 0}, // 激怒宠物
    {"PROFESSION_DRAGNET", PROFESSION_dragnet, 0},       // 天罗地网
    {"PROFESSION_ENTWINE", PROFESSION_entwine, 0}, // 树根缠绕
    {"PROFESSION_AUTARKY", PROFESSION_autarky, 0}, // 自给自足
    {"PROFESSION_PLUNDER", PROFESSION_plunder, 0}, // 屍体掠夺
    {"PROFESSION_TOXIN_WEAPON", PROFESSION_toxin_weapon, 0}, // 毒素武器
    {"PROFESSION_RESIST_FIRE", PROFESSION_resist_fire, 0}, // 火抗性
    {"PROFESSION_RESIST_ICE", PROFESSION_resist_ice, 0},   // 冰抗性
    {"PROFESSION_RESIST_THUNDER", PROFESSION_resist_thunder,
     0}, // 雷抗性
#ifdef _PROFESSION_ADDSKILL
    {"PROFESSION_RESIST_F_I_T", PROFESSION_resist_f_i_t, 0}, // 自然威能
    {"PROFESSION_CALL_NATURE", PROFESSION_call_nature, 0}, // 号召自然
    {"PROFESSION_BOUNDARY", PROFESSION_boundary, 0}, // 四属性结界
#endif
    {"PROFESSION_G_RESIST_FIRE", PROFESSION_g_resist_fire, 0}, // 团体火抗性
    {"PROFESSION_G_RESIST_ICE", PROFESSION_g_resist_ice, 0}, // 团体冰抗性
    {"PROFESSION_G_RESIST_THUNDER", PROFESSION_g_resist_thunder,
     0}, // 团体雷抗性
    {"PROFESSION_ATTACK_WEAK", PROFESSION_attack_weak, 0}, // 弱点攻击
    {"PROFESSION_INSTIGATE", PROFESSION_instigate, 0},     // 挑拨
    {"PROFESSION_OBLIVION", PROFESSION_oblivion, 0},       // 遗忘

    // 共同技能
    {"PROFESSION_FULL_MP", PROFESSION_full_mp, 0}, // 气力充沛
    {"PROFESSION_STRONG_BACK", PROFESSION_strong_back, 0}, // 负重增加
    {"PROFESSION_STRENGTHEN", PROFESSION_strengthen, 0}, // 自我强化

};

static int profession_skill_num; // 职业技能总数

// 将 profession.txt 载入
BOOL PROFESSION_initSkill(char *filename) {

  FILE *f;
  char line[256];
  int linenum = -1;
  int skill_readlen = 0;
  int i, j;

  int max_skillid = 0;
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  f = fopen(realopfile, "r");
  if (f != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    f = fopen(filename, "r");
  }
  if (f == NULL) {
    print("file open error\n");
    return FALSE;
  }

  profession_skill_num = 0;

  while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    char token[256];
    linenum++;
    if (line[0] == '#')
      continue;
    if (line[0] == '\n')
      continue;
    chomp(line);
    profession_skill_num++;
#ifdef _PROSKILL_OPTIMUM
    if (getStringFromIndexWithDelim(
            line, ",", PROFESSION_SKILL_DATACHARNUM + PROFESSION_SKILL_ID + 1,
            token, sizeof(token)) == FALSE)
      continue;
    max_skillid = max(atoi(token), max_skillid);
#endif
  }

  if (fseek(f, 0, SEEK_SET) == -1) {
    fprint("Seek Error\n");
    fclose(f);
    return FALSE;
  }

#ifdef _PROSKILL_OPTIMUM
  profession_skill_num = max_skillid + 1;
  print("最大职技ID = %d ...", max_skillid);
#endif

  PROFESSION_skill = allocateMemory(
      sizeof(struct tagProfessionkill) * profession_skill_num + 1);

  if (PROFESSION_skill == NULL) {
    fprint("无法分配内存 %d\n",
           sizeof(struct tagProfessionkill) * profession_skill_num);
    fclose(f);
    return FALSE;
  }

  // init skill
  for (i = 0; i < profession_skill_num; i++) {
    for (j = 0; j < PROFESSION_SKILL_DATAINTNUM; j++)
      PROFESSION_SKILL_setInt(i, j, -1);

    for (j = 0; j < PROFESSION_SKILL_DATACHARNUM; j++)
      PROFESSION_SKILL_setChar(i, j, "");
  }

  linenum = -1;
  while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    linenum++;
    if (line[0] == '#')
      continue;
    if (line[0] == '\n')
      continue;

    chomp(line);

    replaceString(line, '\t', ' ');

    {
      char buf[256];
      for (i = 0; i < strlen(line); i++) {
        if (line[i] != ' ') {
          break;
        }
        strcpy(buf, &line[i]);
      }

      if (i != 0) {
        strcpy(line, buf);
      }
    }
    {
      char token[256];
      int ret;

#ifdef _PROSKILL_OPTIMUM // 读取本行职技的ID, 直接以职技ID当Table index
      ret = getStringFromIndexWithDelim(
          line, ",", PROFESSION_SKILL_DATACHARNUM + PROFESSION_SKILL_ID + 1,
          token, sizeof(token));
      if (ret == FALSE) {
        fprint("Syntax Error file:%s line:%d\n", filename, linenum);
        break;
      }
      skill_readlen = atoi(token);
#endif
      for (i = 0; i < PROFESSION_SKILL_DATACHARNUM; i++) {

        ret =
            getStringFromIndexWithDelim(line, ",", i + 1, token, sizeof(token));
        if (ret == FALSE) {
          fprint("Syntax Error file:%s line:%d\n", filename, linenum);
          break;
        }
        PROFESSION_SKILL_setChar(skill_readlen, i, token);
      }

      for (i = 0; i < PROFESSION_SKILL_DATAINTNUM; i++) {
        ret = getStringFromIndexWithDelim(line, ",",
                                          i + PROFESSION_SKILL_DATACHARNUM + 1,
                                          token, sizeof(token));

        if (ret == FALSE) {
          fprint("文件语法错误:%s 第%d行\n", filename,
                 linenum);
          break;
        }
        if (strlen(token) != 0) {
          PROFESSION_SKILL_setInt(skill_readlen, i, atoi(token));
        }
      }
      skill_readlen++;
    }
  }
  fclose(f);

  profession_skill_num = skill_readlen;

  for (i = 0; i < arraysizeof(sProfessionSkillFunctionTable); i++) {
    sProfessionSkillFunctionTable[i].hash =
        hashpjw(sProfessionSkillFunctionTable[i].functionname);
  }

  return TRUE;
}

// 重新载入 profession.txt
void rePROFESSION_initSkill() {
  freeMemory(PROFESSION_skill);
  PROFESSION_initSkill(getProfession());
}

INLINE BOOL PROFESSION_SKILL_CHECKINDEX(int index) {
  if (profession_skill_num < index || index < 0)
    return FALSE;
  return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKINTDATAINDEX(int index) {
  if (PROFESSION_SKILL_DATAINTNUM < index || index < 0)
    return FALSE;
  return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKCHARDATAINDEX(int index) {
  if (PROFESSION_SKILL_DATACHARNUM < index || index < 0)
    return FALSE;
  return TRUE;
}

INLINE int PROFESSION_SKILL_getInt(int index,
                                   PROFESSION_SKILL_DATAINT element) {
  return PROFESSION_skill[index].data[element];
}

INLINE int PROFESSION_SKILL_setInt(int index, PROFESSION_SKILL_DATAINT element,
                                   int data) {
  int buf;
  buf = PROFESSION_skill[index].data[element];
  PROFESSION_skill[index].data[element] = data;
  return buf;
}

INLINE char *PROFESSION_SKILL_getChar(int index,
                                      PROFESSION_SKILL_DATACHAR element) {
  if (!PROFESSION_SKILL_CHECKINDEX(index))
    return "\0";
  if (!PROFESSION_SKILL_CHECKCHARDATAINDEX(element))
    return "\0";
  return PROFESSION_skill[index].string[element].string;
}

INLINE BOOL PROFESSION_SKILL_setChar(int index,
                                     PROFESSION_SKILL_DATACHAR element,
                                     char *new) {
  if (!PROFESSION_SKILL_CHECKINDEX(index))
    return FALSE;
  if (!PROFESSION_SKILL_CHECKCHARDATAINDEX(element))
    return FALSE;

  strncpysafe(PROFESSION_skill[index].string[element].string,
             sizeof(PROFESSION_skill[index].string[element].string), new);
  return TRUE;
}

int PROFESSION_SKILL_getskillNum(void) { return profession_skill_num; }
int PROFESSION_SKILL_getskillArray(int skillid) {
#ifdef _PROSKILL_OPTIMUM //
  if (skillid >= 0 && skillid < profession_skill_num)
    return PROFESSION_skill[skillid].data[PROFESSION_SKILL_ID];
#else
  int i;
  for (i = 0; i <= profession_skill_num; i++) { // 待优化
    if (PROFESSION_skill[i].data[PROFESSION_SKILL_ID] == skillid)
      return i;
  }
#endif
  return -1;
}

PROFESSION_SKILL_CALLFUNC PROFESSION_SKILL_getskillFuncPointer(char *name) {
  int i;
  int hash = hashpjw(name);
  for (i = 0; i < arraysizeof(sProfessionSkillFunctionTable); i++) {
    if (sProfessionSkillFunctionTable[i].hash == hash) {
      if (strcmp(sProfessionSkillFunctionTable[i].functionname, name) == 0) {
        return sProfessionSkillFunctionTable[i].func;
      }
    }
  }

  return NULL;
}

int _PROFESSION_SKILL_GetArray(char *file, int line, int char_index,
                               int skill) {
  int skillid, array;
  skillid = CHAR_getCharSkill(char_index, skill);
  if (skillid < 0)
    return -1;
  array = PROFESSION_SKILL_getskillArray(skillid);
  return array;
}

int PROFESSION_SKILL_Use(int char_index, int skill, int toNo, char *data) {
  if (!CHAR_CHECKINDEX(char_index))
    return -1;
#ifdef _DISABLE_PROFESSION_SKILL
  if (getDisableProfessionSkill(CHAR_getInt(char_index, CHAR_FLOOR)) == TRUE) {
    CHAR_talkToCli(char_index, -1, "该地图已禁止使用职业技能!",
                   CHAR_COLORYELLOW);
    return -1;
  }
#endif
  int Pskillid, skillid;
  int ret = -1, skill_level;
  PROFESSION_SKILL_CALLFUNC func;
  CHAR_HaveSkill *hskill;
  int char_pskill, profession_skill;

  skillid = CHAR_getCharSkill(char_index, skill);
  Pskillid = PROFESSION_SKILL_getskillArray(skillid);
  if (Pskillid == -1) {
    return ret;
  }

#ifdef _PROSKILL_OPTIMUM // Robin fix 检查职业是否相符
  char_pskill = CHAR_getInt(char_index, PROFESSION_CLASS);
  profession_skill =
      PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);
  if ((char_pskill <= 0) || (char_pskill != profession_skill)) {
    return ret;
  }
#endif

  func = PROFESSION_SKILL_getskillFuncPointer(
      PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME));

  if (CHAR_getInt(char_index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return ret;

  if (func) {
    hskill = CHAR_getCharHaveSkill(char_index, skill);
    skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);

    if (PROFESSION_SKILL_DEC_COST_MP(char_index, skill, Pskillid,
                                     skill_level) != 1) {
      return ret;
    }

    ret = func(char_index, toNo, Pskillid, data, skill_level);
    if (ret == -1) {
      int rands = rand() % 10;
      if (rands > 5)
        return ret;
    }

    if (Pskillid == 57 &&
        CHAR_getInt(
            BATTLE_No2Index(CHAR_getWorkInt(char_index, CHAR_WORKBATTLEINDEX),
                            toNo),
            CHAR_WHICHTYPE) != CHAR_TYPEPET)
      return ret;

    PROFESSION_NORMAL_SKILL_LEVLE_UP(&hskill->skill, Pskillid, char_index);
  } else {
    logErr("\n func ");
  }

  return ret;
}

void PROFESSION_SKILL_LVEVEL_UP(int char_index, char *name) {
  int skill = -1;
  int skillid = 0, Pskillid = 0, i;
  char *skill_name = "\0";
  CHAR_HaveSkill *hskill;

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {

    skillid = CHAR_getCharSkill(char_index, i);
    Pskillid = PROFESSION_SKILL_getskillArray(skillid);
    if (Pskillid < 0)
      continue;

    skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
    if (skill_name == "\0")
      continue;

    if ((strcmp(skill_name, name)) == 0) {
      skill = i;
      break;
    }
  }

  if ((skill < 0) || (skill > CHAR_SKILLMAXHAVE))
    return;

  skillid = CHAR_getCharSkill(char_index, skill);
  Pskillid = PROFESSION_SKILL_getskillArray(skillid);
  if (Pskillid == -1)
    return;
  hskill = CHAR_getCharHaveSkill(char_index, skill);
  PROFESSION_NORMAL_SKILL_LEVLE_UP(&hskill->skill, Pskillid, char_index);
  return;
}

void PROFESSION_SKILL_DUAL_WEAPON_LVEVEL_UP(int char_index, char *name) {
  int skill = -1, arm = 0, shield = 0;
  int skillid = 0, Pskillid = 0, i;
  char *skill_name = "\0";
  CHAR_HaveSkill *hskill;
  for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
    int id = CHAR_getItemIndex(char_index, i);
    if (ITEM_CHECKINDEX(id)) {
      if (i == CHAR_ARM)
        arm = 1;
      else if (i == CHAR_EQSHIELD)
        shield = 1;
    }
  }

  if (arm != 1 || shield != 1)
    return;

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
    skillid = CHAR_getCharSkill(char_index, i);
    Pskillid = PROFESSION_SKILL_getskillArray(skillid);
    if (Pskillid < 0)
      continue;

    skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
    if (skill_name == "\0")
      continue;

    if ((strcmp(skill_name, name)) == 0) {
      skill = i;
      break;
    }
  }

  if ((skill < 0) || (skill > CHAR_SKILLMAXHAVE))
    return;

  skillid = CHAR_getCharSkill(char_index, skill);
  Pskillid = PROFESSION_SKILL_getskillArray(skillid);
  if (Pskillid == -1)
    return;
  hskill = CHAR_getCharHaveSkill(char_index, skill);
  PROFESSION_NORMAL_SKILL_LEVLE_UP(&hskill->skill, Pskillid, char_index);

  return;
}

void PROFESSION_SKILL_WEAPON_FOCUS_LVEVEL_UP(int char_index, char *name) {
  int skill = -1;
  int skillid = 0, Pskillid = 0, i;
  char *skill_name = "\0", *pszOption = "\0", *pszP, diff[20];
  int WeaponType = -1;
  CHAR_HaveSkill *hskill;

  memset(diff, -1, sizeof(diff));
  WeaponType = BATTLE_GetWepon(char_index);

  switch (WeaponType) {
  case ITEM_AXE:
    sprintf(diff, "斧子");
    break;
  case ITEM_CLUB:
    sprintf(diff, "棍棒");
    break;
  case ITEM_SPEAR:
    sprintf(diff, "矛");
    break;
  case ITEM_BOW:
    sprintf(diff, "弓");
    break;
  case ITEM_BOOMERANG:
    sprintf(diff, "镖");
    break;
  case ITEM_BOUNDTHROW:
    sprintf(diff, "Ͷ");
    break;
  case ITEM_BREAKTHROW:
    sprintf(diff, "ʯ");
    break;
  default:
    sprintf(diff, "无");
    break;
  }

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
    skillid = CHAR_getCharSkill(char_index, i);
    Pskillid = PROFESSION_SKILL_getskillArray(skillid);
    if (Pskillid < 0)
      continue;

    skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
    if (skill_name == "\0")
      continue;

    pszOption = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_OPTION);
    if (pszOption == "\0")
      continue;

    if ((strcmp(skill_name, name)) == 0) {
      if ((pszP = strstr(pszOption, diff)) != NULL) {
        skill = i;
        break;
      }
    }
  }

  if ((skill < 0) || (skill > CHAR_SKILLMAXHAVE))
    return;

  skillid = CHAR_getCharSkill(char_index, skill);
  Pskillid = PROFESSION_SKILL_getskillArray(skillid);
  if (Pskillid == -1)
    return;

  hskill = CHAR_getCharHaveSkill(char_index, skill);

  PROFESSION_NORMAL_SKILL_LEVLE_UP(&hskill->skill, Pskillid, char_index);
  return;
}

// 耗损 MP
int PROFESSION_SKILL_DEC_COST_MP(int char_index, int skill, int Pskillid,
                                 int skill_level) {

  if (skill_level > 0) {
    int dec_mp;
    int old_mp = CHAR_getInt(char_index, CHAR_MP);
    int mp = 0;
    // 取出耗损 MP
    if ((dec_mp = PROFESSION_MAGIC_COST_MP(char_index, skill)) == -1)
      dec_mp = PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_COST_MP);
    // 检查 MP量
    if (old_mp < dec_mp)
      return -1;
    // 嗜血成性
    if (Pskillid != 11
#ifdef _PROFESSION_ADDSKILL
    //	&& Pskillid != 2 //针针相对不扣 MP
#endif
    )
      if (dec_mp <= 0)
        return -1;
    // 扣掉人物 MP
    mp = old_mp - dec_mp;
    if (mp < 0)
      mp = 0;
    CHAR_setInt(char_index, CHAR_MP, mp);
    CHAR_send_P_StatusString(char_index, CHAR_P_STRING_MP);
    return 1;
  }
  return -1;
}

void PROFESSION_NORMAL_SKILL_LEVLE_UP(Skill *skill, int Pskillid,
                                      int char_index) {
  int skill_level = 0, up_fix_value = 0;
  int rand_num = RAND(0, 10000), rand_num2 = 0;

  skill_level = SKILL_getRealInt(skill, SKILL_LEVEL);
  if (skill_level >= PROFESSION_SKILL_MAX_LEVEL * 100)
    return;

  up_fix_value =
      PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_FIX_VALUE) * 100;
  rand_num2 = RAND(0, up_fix_value);

  if (rand_num > skill_level + rand_num2) {
    skill_level += PROFESSION_SKILL_ADD_POINT;
    SKILL_setInt(skill, SKILL_LEVEL, skill_level);

    if ((skill_level % 100) == 0) {
      char msg[256];
      sprintf(msg, "%s技能熟练度上升为%d",
              PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_NAME),
              skill_level / 100);
      CHAR_talkToCli(char_index, -1, msg, CHAR_COLORYELLOW);
      CHAR_sendStatusString(char_index, "S");
      PROFESSION_LEVEL_CHECK_UP(char_index);
    }
  }
  return;
}

void PROFESSION_LEVEL_CHECK_UP(int char_index) {
  int skill_level_add = 0, skill_level_sum = 0, i = 0;
  int old_level = CHAR_getInt(char_index, PROFESSION_LEVEL);
  int next_level_need_point = 9999999;
  CHAR_HaveSkill *hskill;

  // 下一级职业升级点数
  next_level_need_point = old_level * 70 * 100;

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
    int skillID = -1;
    hskill = CHAR_getCharHaveSkill(char_index, i);
    skillID = SKILL_getInt(&hskill->skill, SKILL_IDENTITY);
    if (skillID <= 0)
      continue;

    if ((skillID == 63) || (skillID == 64) || (skillID == 65)) {
      skill_level_add = 50 * 100;
    } else {
      skill_level_add = SKILL_getRealInt(&hskill->skill, SKILL_LEVEL);
    }
    skill_level_sum += skill_level_add;
  }

  if (skill_level_sum >= next_level_need_point) {
    int old_skill_point = CHAR_getInt(char_index, PROFESSION_SKILL_POINT);
    int next_skill_point = old_skill_point + 1;
    char msg[64];

    CHAR_setInt(char_index, PROFESSION_LEVEL, old_level + 1);
    CHAR_setInt(char_index, PROFESSION_SKILL_POINT, next_skill_point);
    CHAR_sendStatusString(char_index, "P");

    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(char_index, CHAR_WORKOBJINDEX));

    sprintf(msg, "职业等级上升为%d级，技能点数上升１点", old_level + 1);
    CHAR_talkToCli(char_index, -1, msg, CHAR_COLORYELLOW);
  }
}

int PROFESSION_SKILL_ADDSK(int char_index, int skill, int level) {
  int i;
  CHAR_HaveSkill *pSkil;
  if (level > 100)
    level = 100;
  else if (level < 1)
    level = 1;
  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {
    int skill_id = -1;

    pSkil = CHAR_getCharHaveSkill(char_index, i);
    skill_id = SKILL_getInt(&pSkil->skill, SKILL_IDENTITY);
    if (skill == skill_id && skill_id != -1) {
      CHAR_talkToCli(char_index, -1, "非法的职业技能", CHAR_COLORYELLOW);
      return -1;
    }

    if (pSkil == (CHAR_HaveSkill *)NULL)
      continue;
    if (pSkil->use != 0)
      continue;
    break;
  }
  if (i >= CHAR_SKILLMAXHAVE) {
    CHAR_talkToCli(char_index, -1, "所掌握的技能已经超过上限", CHAR_COLORYELLOW);
    return -1;
  } else {
    level *= 100;
    SKILL_makeSkillData(&pSkil->skill, skill, level);
    pSkil->use = TRUE;
  }

  CHAR_sendStatusString(char_index, "S");

  return 1;
}

void profession_common_fun(int char_index, int toNo, int skill_level, int array,
                           int com1) {
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, com1);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM2, toNo);
  CHAR_setWorkInt(char_index, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
  CHAR_SETWORKINT_HIGH(char_index, CHAR_WORKBATTLECOM3, skill_level);
  CHAR_SETWORKINT_LOW(char_index, CHAR_WORKBATTLECOM3, array);
#ifdef _PROFESSION_ADDSKILL
  if (com1 == BATTLE_COM_S_DOOM) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    CHAR_setWorkInt(char_index, CHAR_WORK_com1, com1);
    CHAR_setWorkInt(char_index, CHAR_WORK_toNo, toNo);
    CHAR_setWorkInt(char_index, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK);
    CHAR_setWorkInt(char_index, CHAR_WORK_skill_level, skill_level);
    CHAR_setWorkInt(char_index, CHAR_WORK_array, array);
    CHAR_setWorkInt(char_index, CHAR_DOOMTIME, 3);
  } else if (com1 == BATTLE_COM_S_FIRE_SPEAR) {
    CHAR_setWorkInt(char_index, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    CHAR_setWorkInt(char_index, CHAR_WORK_com1, com1);
    CHAR_setWorkInt(char_index, CHAR_WORK_toNo, toNo);
    CHAR_setWorkInt(char_index, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK);
    CHAR_setWorkInt(char_index, CHAR_WORK_skill_level, skill_level);
    CHAR_setWorkInt(char_index, CHAR_WORK_array, array);
    CHAR_setWorkInt(char_index, CHAR_DOOMTIME, 2);
  }
#endif
}

int PROFESSION_volcano_springs(int char_index, int toNo, int array, char *data,
                               int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_VOLCANO_SPRINGS);
  return TRUE;
}

int PROFESSION_fire_ball(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo < 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_FIRE_BALL);
  return TRUE;
}

int PROFESSION_fire_spear(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_FIRE_SPEAR);
  return TRUE;
}

int PROFESSION_summon_thunder(int char_index, int toNo, int array, char *data,
                              int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_SUMMON_THUNDER);
  return TRUE;
}

int PROFESSION_current(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CURRENT);
  return TRUE;
}

int PROFESSION_storm(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_STORM);
  return TRUE;
}

int PROFESSION_ice_arrow(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ICE_ARROW);
  return TRUE;
}

// 巫师技能-火星球
int PROFESSION_ice_crack(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ICE_CRACK);
  return TRUE;
}

// 巫师技能-火龙枪
int PROFESSION_ice_mirror(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ICE_MIRROR);
  return TRUE;
}

// 巫师技能-召雷术
int PROFESSION_doom(int char_index, int toNo, int array, char *data,
                    int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_DOOM);
  return TRUE;
}

// 巫师技能-电流术
int PROFESSION_blood(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_BLOOD);
  return TRUE;
}

// 巫师技能-暴风雨
int PROFESSION_blood_worms(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_BLOOD_WORMS);
  return TRUE;
}

// 巫师技能-冰箭术
int PROFESSION_sign(int char_index, int toNo, int array, char *data,
                    int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_SIGN);
  return TRUE;
}

// 巫师技能-冰爆术
int PROFESSION_fire_enclose(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_FIRE_ENCLOSE);
  return TRUE;
}

// 巫师技能-冰镜术
int PROFESSION_ice_eclose(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ICE_ENCLOSE);
  return TRUE;
}

// 巫师技能-世界末日
int PROFESSION_thunder_enclose(int char_index, int toNo, int array, char *data,
                               int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_THUNDER_ENCLOSE);
  return TRUE;
}

// 巫师技能-嗜血成性
int PROFESSION_fire_practice(int char_index, int toNo, int array, char *data,
                             int skill_level) {
  return TRUE;
}

// 巫师技能-嗜血蛊
int PROFESSION_ice_practice(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  return TRUE;
}

// 巫师技能-一针见血
int PROFESSION_thunder_practice(int char_index, int toNo, int array, char *data,
                                int skill_level) {
  return TRUE;
}

// 巫师技能-火附体
int PROFESSION_enclose(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ENCLOSE);
  return TRUE;
}

// 巫师技能-冰附体
int PROFESSION_transpose(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_TRANSPOSE);
  return TRUE;
}

/*----------------------------------------------------------------------*/

// 猎人技能-陷阱
int PROFESSION_brust(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_BRUST);
  return TRUE;
}

// 猎人技能-驯伏宠物
int PROFESSION_chain_atk(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CHAIN_ATK);
  return TRUE;
}

// 猎人技能-激怒宠物
int PROFESSION_avoid(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_AVOID);
  return TRUE;
}

// 猎人技能-天罗地网
int PROFESSION_recovery(int char_index, int toNo, int array, char *data,
                        int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_RECOVERY);
  return TRUE;
}

// 猎人技能-树根缠绕
int PROFESSION_weapon_focus(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  return TRUE;
}

// 猎人技能-自给自足
int PROFESSION_reback(int char_index, int toNo, int array, char *data,
                      int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_REBACK);
  return TRUE;
}

// 猎人技能-屍体掠夺
int PROFESSION_chain_atk_2(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CHAIN_ATK_2);
  return TRUE;
}

// 猎人技能-毒素武器
int PROFESSION_scapegoat(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_SCAPEGOAT);
  return TRUE;
}

// 猎人技能-火抗性提升
int PROFESSION_enrage(int char_index, int toNo, int array, char *data,
                      int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ENRAGE);
  return TRUE;
}

// 猎人技能-冰抗性提升
int PROFESSION_energy_collect(int char_index, int toNo, int array, char *data,
                              int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_COLLECT);
  return TRUE;
}

// 猎人技能-雷抗性提升
int PROFESSION_focus(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_FOCUS);
  return TRUE;
}

// 勇士技能-盾击
int PROFESSION_shield_attack(int char_index, int toNo, int array, char *data,
                             int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_SHIELD_ATTACK);
  return TRUE;
}

// 勇士技能-二刀流
int PROFESSION_dual_weapon(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  return TRUE;
}

// 勇士技能-格档
int PROFESSION_deflect(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_DEFLECT);
  return TRUE;
}

// 勇士技能-贯穿攻击
int PROFESSION_through_attack(int char_index, int toNo, int array, char *data,
                              int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_THROUGH_ATTACK);
  return TRUE;
}

// 勇士技能-座骑攻击
int PROFESSION_cavalry(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CAVALRY);
  return TRUE;
}

// 勇士技能-濒死攻击
int PROFESSION_dead_attack(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_DEAD_ATTACK);
  return TRUE;
}

// 勇士技能-回旋攻击
int PROFESSION_convolute(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo < 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CONVOLUTE);
  return TRUE;
}

// 勇士技能-混乱攻击
int PROFESSION_chaos(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CHAOS);
  return TRUE;
}

/*----------------------------------------------------------------------*/

// 猎人技能-陷阱
int PROFESSION_trap(int char_index, int toNo, int array, char *data,
                    int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_TRAP);
  return TRUE;
}

// 猎人技能-驯伏宠物
int PROFESSION_docile(int char_index, int toNo, int array, char *data,
                      int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_DOCILE);
  return TRUE;
}

// 猎人技能-激怒宠物
int PROFESSION_enrage_pet(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ENRAGE_PET);
  return TRUE;
}

// 猎人技能-天罗地网
int PROFESSION_dragnet(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_DRAGNET);
  return TRUE;
}

// 猎人技能-树根缠绕
int PROFESSION_entwine(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ENTWINE);
  return TRUE;
}

// 猎人技能-自给自足
int PROFESSION_autarky(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  return TRUE;
}

int PROFESSION_plunder(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_PLUNDER);
  return TRUE;
}

int PROFESSION_toxin_weapon(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_TOXIN_WEAPON);
  return TRUE;
}

int PROFESSION_resist_fire(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_RESIST_FIRE);
  return TRUE;
}

int PROFESSION_resist_ice(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_RESIST_ICE);
  return TRUE;
}

// 猎人技能-雷抗性提升
int PROFESSION_resist_thunder(int char_index, int toNo, int array, char *data,
                              int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_RESIST_THUNDER);
  return TRUE;
}

#ifdef _PROFESSION_ADDSKILL
// 猎人技能-自然威能
int PROFESSION_resist_f_i_t(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_RESIST_F_I_T);
  return TRUE;
}
// 猎人技能-号召自然
int PROFESSION_call_nature(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_CALL_NATURE);
  return TRUE;
}
// 猎人技能-四属性结界
int PROFESSION_boundary(int char_index, int toNo, int array, char *data,
                        int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_BOUNDARY);
  return TRUE;
}
#endif

int PROFESSION_g_resist_fire(int char_index, int toNo, int array, char *data,
                             int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_G_RESIST_FIRE);
  return TRUE;
}

int PROFESSION_g_resist_ice(int char_index, int toNo, int array, char *data,
                            int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_G_RESIST_ICE);
  return TRUE;
}

int PROFESSION_g_resist_thunder(int char_index, int toNo, int array, char *data,
                                int skill_level) {
  if (toNo != 20 && toNo != 21)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_G_RESIST_THUNDER);
  return TRUE;
}

int PROFESSION_attack_weak(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_ATTACK_WEAK);
  return TRUE;
}

int PROFESSION_instigate(int char_index, int toNo, int array, char *data,
                         int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_INSTIGATE);
  return TRUE;
}

int PROFESSION_oblivion(int char_index, int toNo, int array, char *data,
                        int skill_level) {
  if (toNo > 19)
    toNo = 0;
  profession_common_fun(char_index, toNo, skill_level, array,
                        BATTLE_COM_S_OBLIVION);
  return TRUE;
}

int PROFESSION_track(int char_index, int toNo, int array, char *data,
                     int skill_level) {
  char *pszP = NULL, *pszOption = "\0", msg[20];
  int per = 0, rate = 0, img1, ret = 1;
  skill_level /= 10;

  if (CHAR_getWorkInt(char_index, CHAR_ENCOUNT_NUM) >= (int)time(NULL)) {
    ret = -1;
  }

  pszOption = PROFESSION_SKILL_getChar(array, PROFESSION_SKILL_OPTION);

  if ((pszP = strstr(pszOption, "倍%")) != NULL)
    sscanf(pszP + 3, "%d", &rate);
  per = skill_level * rate;

  img1 = PROFESSION_SKILL_getInt(array, PROFESSION_SKILL_IMG_1);

  CHAR_setWorkInt(char_index, CHAR_ENCOUNT_FIX, per);
  CHAR_talkToCli(char_index, -1, "遇敌率上升", CHAR_COLORRED);
  CHAR_setWorkInt(char_index, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3));

  sprintf(msg, "%d", img1);
  GmsvServer_BATTLESKILL_send(getfdFromCharaIndex(char_index), msg);

  return ret;
}

int PROFESSION_escape(int char_index, int toNo, int array, char *data,
                      int skill_level) {
  char *pszP = NULL, *pszOption = "\0", msg[20];
  int per = 0, rate = 0, img1, ret = 1;
  skill_level /= 10;

  if (CHAR_getWorkInt(char_index, CHAR_ENCOUNT_NUM) >= (int)time(NULL)) {
    ret = -1;
  }
  pszOption = PROFESSION_SKILL_getChar(array, PROFESSION_SKILL_OPTION);
  if ((pszP = strstr(pszOption, "倍%")) != NULL)
    sscanf(pszP + 3, "%d", &rate);
  per = skill_level * rate;
  img1 = PROFESSION_SKILL_getInt(array, PROFESSION_SKILL_IMG_1);

  CHAR_setWorkInt(char_index, CHAR_ENCOUNT_FIX, (-1) * per);
  CHAR_talkToCli(char_index, -1, "遇敌率下降", CHAR_COLORGREEN);

  CHAR_setWorkInt(char_index, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3));

  sprintf(msg, "%d", img1);
  GmsvServer_BATTLESKILL_send(getfdFromCharaIndex(char_index), msg);

  return ret;
}
int PROFESSION_full_mp(int char_index, int toNo, int array, char *data,
                       int skill_level) {
  return TRUE;
}
int PROFESSION_strong_back(int char_index, int toNo, int array, char *data,
                           int skill_level) {
  return TRUE;
}
int PROFESSION_strengthen(int char_index, int toNo, int array, char *data,
                          int skill_level) {
  return TRUE;
}

#endif
