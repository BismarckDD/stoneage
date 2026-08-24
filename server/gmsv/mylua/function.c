#include "version.h"
#include "battle.h"
#include "config_file.h"
#include "char.h"
#include "item.h"
#include "mylua/base.h"
#include "mylua/mylua.h"

extern MY_Lua MYLua;

lua_State *FindLua(char *filename) {
  MY_Lua *mylua = &MYLua;
  char newfilename[256];
  while (mylua->next != NULL) {
    if (strcmptail(mylua->luapath, ".allblues") == 0) {
      sprintf(newfilename, "%s.allblues", filename);
    } else {
      sprintf(newfilename, "%s", filename);
    }
    if (strcmp(newfilename, mylua->luapath) == 0) {
      return mylua->lua;
    }
    mylua = mylua->next;
  }
  return NULL;
}

#ifdef _MO_LUA_MERGE_CALLBACK
BOOL MergeCallBack(int char_index, int petindex, char *data, int flg) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/mergecallback.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "MergeCallBack");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);
  lua_pushstring(lua, data);
  lua_pushnumber(lua, flg);
  docall(lua, 4, 1);

  return TRUE;
}
#endif

#ifdef _RIDEFLG_
BOOL FamilyRideCheck(int meindex, int petindex, int petid) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/familyridefunction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FamilyRideCheck");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, petid);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _NEW_TITLE
int GetCharNewTitleNo(int char_index, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freechartitle.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "GetCharNewTitleNo");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

char *GetCharNewTitleName(int char_index, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freechartitle.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "GetCharNewTitleName");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isstring(lua, -1))
    luaL_error(lua, "function `f' must return a string");

  char *ret = lua_tostring(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _CHAR_TITLE_STR_
char *GetCharNewTitleString(int char_index, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freechartitle.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "GetCharNewTitleString");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isstring(lua, -1))
    luaL_error(lua, "function `f' must return a string");

  char *ret = lua_tostring(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

#endif

#ifdef _FAMILYBADGE_
BOOL LoadFamilyBadge(int fd) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/other/familybadge/init.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "LoadFamilyBadge");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, fd);
  docall(lua, 1, 1);

  return TRUE;
}
#endif

#ifdef _ALLBLUES_LUA_1_9
int FreeCharDelet(int fd, char *cdkey, char *passwd) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freechardelete.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCharDelete");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, fd);
  lua_pushstring(lua, cdkey);
  lua_pushstring(lua, passwd);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    return TRUE;

  if (!lua_isnumber(lua, -1))
    return TRUE;

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL BattleCommand(int char_index, int battleindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/battlecommand.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleCommand");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, battleindex);
  docall(lua, 2, 1);

  return TRUE;
}
#ifdef _MO_LUA_KS_CALLBACK
BOOL FreeSelectBattlePet(int char_index, int petarray) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/FreeSelectBattlePet.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeSelectBattlePet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入2个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petarray);
  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  if (ret == FALSE) {
    return FALSE;
  }
  return TRUE;
}
#endif

int FreeItemInslay(int char_index, int inslayindex, int item_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freeiteminslay.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeItemInslay");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, inslayindex);
  lua_pushnumber(lua, item_index);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#ifdef _LOCK_PET_ITEM
#ifdef _CTRL_TRANS_DEVELOP
int FreeCtrlTransDevelop(int petindex, int type) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freectrltransdevelop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCtrlTransDevelop");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, type);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif
int FreeFmPk() {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freefmpk.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFmPk");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  if (lua_pcall(lua, 0, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeStreetVendorPet(int char_index, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freestreetvendor.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeStreetVendorPet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeStreetVendorItem(int char_index, int item_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freestreetvendor.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeStreetVendorItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeTradePet(int char_index, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freetrade.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeTradePet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeTradeItem(int char_index, int item_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freetrade.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeTradeItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

#ifdef _LOCK_PET_ITEM
int FreeDropItem(int char_index, int item_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freedrop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeDropItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

int FreeLockPetSave(int char_index, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freelockpetsave.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeLockPetSave");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif
void FreeAddFMAdv(int char_index, int shiftbit) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freeaddfmadv.lua");
    if (lua == NULL)
      return;
  }

  lua_getglobal(lua, "FreeAddFMAdv");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, shiftbit);
  docall(lua, 2, 1);
}
#ifdef _FIRST_LOCK_ITEM
void FreeFirstLockPet(int char_index, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freefirstlock.lua");
    if (lua == NULL)
      return;
  }

  lua_getglobal(lua, "FreeFirstLockPet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);

  docall(lua, 2, 1);
}
void FreeFirstLockItem(int char_index, int item_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freefirstlock.lua");
    if (lua == NULL)
      return;
  }

  lua_getglobal(lua, "FreeFirstLockItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);
}
#endif

void FreeCharExpSave(int charindex, int exp) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freecharexpsave.lua");
    if (lua == NULL)
      return;
  }

  lua_getglobal(lua, "FreeCharExpSave");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, charindex);
  lua_pushnumber(lua, exp);

  docall(lua, 2, 1);
}
#ifdef _PET_FUSION_LUA
int FreeFusionTableForBase(int MainIndex, int Subindex1, int Subindex2) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freefusion.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFusionTableForBase");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, MainIndex);
  lua_pushnumber(lua, Subindex1);
  lua_pushnumber(lua, Subindex2);
  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeFusionSkill(int petskill) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freefusion.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFusionSkill");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, petskill);
  if (lua_pcall(lua, 1, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

BOOL FreeAction(int char_index, int x, int y, int actionno) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/freeaction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeAction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, x);
  lua_pushnumber(lua, y);
  lua_pushnumber(lua, actionno);
  docall(lua, 4, 1);

  return TRUE;
}

int FreeModeExp(int charindex, int getexp, int modexp) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freemodeexp.lua");
    if (lua == NULL)
      return 1;
  }

  lua_getglobal(lua, "FreeModeExp");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return 1;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, charindex);
  lua_pushnumber(lua, getexp);
  lua_pushnumber(lua, modexp);
  if (lua_pcall(lua, 3, 1, 0) != 0)
    return 1;

  if (!lua_isnumber(lua, -1))
    return 1;

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreePlayerExp(int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freeplayerexp.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreePlayerExp");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, char_index);

  if (lua_pcall(lua, 1, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeSaMenu(int char_index, int index) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/freesamenu.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeSaMenu");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, index);

  // printf("FreeSaMenu:%d,%d\n", char_index, index);
  docall(lua, 2, 1);

  return TRUE;
}

#ifdef _DATA_INFO_SHOW
BOOL FreeDataInfoShow(int char_index) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/freedatainfoshow.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeDataInfoShow");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, char_index);
  docall(lua, 1, 1);

  return TRUE;
}
#endif
#ifdef _FAME_REG_TIME
BOOL FreeFameFeatures(int char_index, int kind, int flg) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/freefamefeatures.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeFameFeatures");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, kind);
  lua_pushnumber(lua, flg);

  docall(lua, 3, 1);

  return TRUE;
}
#endif
BOOL FreeComplianceParameter(int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freecomplianceparameter.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeComplianceParameter");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, char_index);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL FreeLoginCheck(int fd) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freelogincheck.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeLoginCheck");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, fd);

  if (lua_pcall(lua, 1, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeCharLogout(int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freecharlogout.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharLogout");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  // 依次放入一个参数
  lua_pushnumber(lua, char_index);
  docall(lua, 1, 1);

  return TRUE;
}

BOOL EquipEffectFunction(int char_index, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/equipeffectfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "EquipEffectFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL EquipChangeFunction(int char_index, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/equipeffectfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "EquipChangeFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL WalkFunction(int char_index) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "WalkFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }

    lua_pushnumber(mylua->lua, char_index);

    if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
      luaL_error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      luaL_error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == TRUE) {
      return TRUE;
    }

    mylua = mylua->next;
  }

  return FALSE;
}
#ifdef _ITEM_OVER_LAP
BOOL ItemOverlapFunction(int charindex, int fromitem_index, int toitem_index) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "ItemOverlapFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    // 依次放入二个参数
    lua_pushnumber(mylua->lua, charindex);
    lua_pushnumber(mylua->lua, fromitem_index);
    lua_pushnumber(mylua->lua, toitem_index);

    lua_pcall(mylua->lua, 3, 1, 0);

    lua_isnumber(mylua->lua, -1);

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == 1) {
      mylua = mylua->next;
      continue;
    }
    return ret;
  }

  return TRUE;
}

BOOL ItemOverlapedFunction(int charindex, int fromitem_index, int fromid,
                           int toitem_index, int toid) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "ItemOverlapedFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    // 依次放入二个参数
    lua_pushnumber(mylua->lua, charindex);
    lua_pushnumber(mylua->lua, fromitem_index);
    lua_pushnumber(mylua->lua, fromid);
    lua_pushnumber(mylua->lua, toitem_index);
    lua_pushnumber(mylua->lua, toid);

    docall(mylua->lua, 5, 1);
    mylua = mylua->next;
  }

  return TRUE;
}
#endif

BOOL BattleFinishPvEFunction(int battleindex, int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/battlefinishfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleFinishPvEFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, battleindex);
  lua_pushnumber(lua, char_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL BattleFinishFunction(int char_index, int battletime, int battleturn,
                          int battletype) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/battlefinishfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleFinishFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, battletime);
  lua_pushnumber(lua, battleturn);
  lua_pushnumber(lua, battletype);

  docall(lua, 4, 1);

  return TRUE;
}

#ifdef _MO_LUA_BATTLE_START
BOOL BattleStartFunction(int battleindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/battlestartfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleStartFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, battleindex);

  docall(lua, 1, 1);

  return TRUE;
}
#endif

BOOL SetBattleEnmeyFunction(int meindex, int enemy_index, int id) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_BATTLESETFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, enemy_index);
  lua_pushnumber(lua, id);
  docall(lua, 3, 1);

  return TRUE;
}

#endif

#ifdef _ALLBLUES_LUA_1_8
BOOL CaptureOkFunction(int attackindex, int defindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CaptureOkFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    // 依次放入二个参数
    lua_pushnumber(mylua->lua, attackindex);
    lua_pushnumber(mylua->lua, defindex);

    docall(mylua->lua, 2, 1);
    mylua = mylua->next;
  }

  return TRUE;
}

BOOL CaptureCheckFunction(int attackindex, int defindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/capturefunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "CaptureCheckFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, attackindex);
  lua_pushnumber(lua, defindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_7
BOOL CharVsEnemyFunction(int char_index) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CharVsEnemyFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }

    lua_pushnumber(mylua->lua, char_index);

    if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
      luaL_error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      luaL_error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == TRUE) {
      return TRUE;
    }

    mylua = mylua->next;
  }

  return FALSE;
}
#endif

#ifdef _ALLBLUES_LUA_1_6
BOOL CharTalkFunction(int char_index, char *message, int color) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/chartalkfunction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "CharTalkFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushstring(lua, message);
  lua_pushnumber(lua, color);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL FamilyRideFunction(int meindex, int petindex, int petid) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/familyridefunction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FamilyRideFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, petid);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_5
BOOL NetLoopFunction(void) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "NetLoopFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    docall(mylua->lua, 0, 1);
    mylua = mylua->next;
  }

  return TRUE;
}

BOOL FreeCharCreate(int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freecharcreate.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharCreate");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL FreeCharLogin(int char_index) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freecharlogin.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharLogin");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, char_index);

  if (lua_pcall(lua, 1, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL FreeVsPlayer(int char_index, int toindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freevsplayer.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeVsPlayer");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, toindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL FreePartyJoin(int char_index, int toindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "FreePartyJoin");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    // 依次放入二个参数
    lua_pushnumber(mylua->lua, char_index);
    lua_pushnumber(mylua->lua, toindex);

    if (lua_pcall(mylua->lua, 2, 1, 0) != 0)
      luaL_error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      luaL_error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == FALSE) {
      return FALSE;
    }

    mylua = mylua->next;
  }

  return TRUE;
}
#endif

#ifdef _ALLBLUES_LUA_1_4

BOOL RunCharLogOutEvent(int char_index) {
  lua_State *lua = CHAR_getLUAFunction(char_index, CHAR_LOGINOUTFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL BattleFinish(int battleindex, int char_index) {
  lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_FINISH);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL BattleEscape(int battleindex, int char_index) {
  lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_ESCAPE);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);

  docall(lua, 1, 1);

  return TRUE;
}
#endif

#ifdef _OFFLINE_SYSTEM
BOOL OffLineCommand(int battleindex, int char_index, int side) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/offlinecommand.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "OffLineCommand");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, battleindex);
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, side);

  docall(lua, 3, 1);

  return TRUE;
}
#endif

void ABNPC_Lua_NEWSHOP_Recv(int char_index) {
  static lua_State *lua;
  if (lua == NULL) {
    char token[256];
    snprintf(token, sizeof(token), "data/ablua/newshop.lua");
    lua = FindLua(token);
    if (lua == NULL)
      return;
  }
  lua_getglobal(lua, "newshop");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  docall(lua, 1, 1);

  return;
}

#ifdef _PETSKILL_SHOP_LUA
BOOL FreePetSkillShop(int talkerindex, int petindex, int oldSkillID,
                      int newSkillID) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freepetskillshop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreePetSkillShop");

  if (!lua_isfunction(lua, -1)) {
    return TRUE;
  }
  // 依次放入三个参数
  lua_pushnumber(lua, talkerindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, oldSkillID);
  lua_pushnumber(lua, newSkillID);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    luaL_error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_2
BOOL RunUseChatMagic(int char_index, char *data, lua_State *lua) {
  if (lua == NULL)
    return FALSE;

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushstring(lua, data);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemUseEvent(int item_index, int char_index, int toindex,
                     int haveitem_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_USEFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入四个参数
  lua_pushnumber(lua, item_index);
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, haveitem_index);

  docall(lua, 4, 1);

  return TRUE;
}

BOOL RunItemDieReLifeEvent(int char_index, int item_index, int haveitem_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_DIERELIFEFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入三个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);
  lua_pushnumber(lua, haveitem_index);

  docall(lua, 3, 1);

  return TRUE;
}

BOOL RunItemDetachEvent(int char_index, int item_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_DETACHFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemAttachEvent(int char_index, int item_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_ATTACHFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPickupEvent(int char_index, int item_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_PICKUPFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPostOverEvent(int item_index, int char_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_POSTOVERFUNC);
  if (lua == NULL) {
    return FALSE;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPreOverEvent(int item_index, int char_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_PREOVERFUNC);
  if (lua == NULL) {
    return FALSE;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemDropEvent(int char_index, int item_index) {
  lua_State *lua = ITEM_getLUAFunction(item_index, ITEM_DROPFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, item_index);

  docall(lua, 2, 1);

  return TRUE;
}

#endif

#ifdef _VISUAL_BEATITUDE
void FreeVisualBeatitude(int char_index, int petindex, int type) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/freevisualbeatitude.lua");
    if (lua == NULL)
      return;
  }
  lua_getglobal(lua, "FreeVisualBeatitude");
  if (!lua_isfunction(lua, -1)) {
    return;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, type);
  docall(lua, 3, 1);
  return;
}

char *BeatitudeCheck(int meindex, int petindex, int havepetindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/freevisualbeatitude.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "BeatitudeCheck");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, havepetindex);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    luaL_error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isstring(lua, -1))
    luaL_error(lua, "function `f' must return a string");

  int ret = lua_tostring(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _RIDEQUERY_
void FreeRideQuery(int char_index) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("data/ablua/familyridefunction.lua");
    if (lua == NULL)
      return;
  }
  lua_getglobal(lua, "FreeRideQuery");
  if (!lua_isfunction(lua, -1)) {
    return;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  docall(lua, 1, 1);
  return;
}
#endif

#ifdef _RED_MEMOY_
BOOL RedMoneyFunction(int char_index, char *data) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("data/ablua/redmoneyfunction.lua");
    if (lua == NULL)
      return TRUE;
  }
  lua_getglobal(lua, "RedMoneyFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  // 依次放入二个参数
  lua_pushnumber(lua, char_index);
  lua_pushstring(lua, data);
  docall(lua, 2, 1);
  return TRUE;
}
#endif
