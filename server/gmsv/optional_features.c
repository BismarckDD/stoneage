/*
 * Safe defaults for optional scripting modules that are not part of the
 * native Windows build.  These preserve the non-scripted game behavior while
 * keeping the call sites shared with scripting-enabled builds intact.
 */
#include "version.h"

#include "char_base.h"
#include "npc_roomadminnew.h"
#include "util.h"

#ifdef _WIN32
int luaplayernum = 0;

BOOL FamilyRideFunction(int char_index, int pet_index, int pet_id) {
  (void)char_index;
  (void)pet_index;
  (void)pet_id;
  return FALSE;
}

int FreeTradeItem(int char_index, int item_index) {
  (void)char_index;
  (void)item_index;
  return FALSE;
}

int FreeTradePet(int char_index, int pet_index) {
  (void)char_index;
  (void)pet_index;
  return FALSE;
}

int FreeFmPk(void) { return 50; }

BOOL FreePlayerExp(int char_index) {
  (void)char_index;
  return 100;
}

BOOL BattleFinishPvEFunction(int battle_index, int char_index) {
  (void)battle_index;
  (void)char_index;
  return FALSE;
}

void NPC_Lua_NEWSHOP_Recv(char *function_name, int char_index) {
  (void)function_name;
  (void)char_index;
}
#endif

#if !defined(_WIN32) && !defined(_OFFLINE_SYSTEM)
int luaplayernum = 0;
#endif


BOOL NPC_RoomAdminNew_ReadFile(char *room_name, NPC_ROOMINFO *data) {
  char filename[512];
  char line[1024];
  char token[128];
  FILE *file;
  if (room_name == NULL || data == NULL)
    return FALSE;
  if (strstr(room_name, "..") != NULL || strchr(room_name, '/') != NULL ||
      strchr(room_name, '\\') != NULL)
    return FALSE;
  snprintf(filename, sizeof(filename), "roomadmin/%s.room", room_name);
  file = fopen(filename, "r");
  if (file == NULL)
    return FALSE;
  if (fgets(line, sizeof(line), file) == NULL) {
    fclose(file);
    return FALSE;
  }
  fclose(file);
  memset(data, 0, sizeof(*data));
  if (!getStringFromIndexWithDelim(line, "|", 1, token, sizeof(token)))
    return FALSE;
  strncpysafe(data->cdkey, sizeof(data->cdkey), token);
  if (!getStringFromIndexWithDelim(line, "|", 2, token, sizeof(token)))
    return FALSE;
  strncpysafe(data->charaname, sizeof(data->charaname),
              makeStringFromEscaped(token));
  if (!getStringFromIndexWithDelim(line, "|", 3, token, sizeof(token)))
    return FALSE;
  strncpysafe(data->passwd, sizeof(data->passwd), token);
  if (!getStringFromIndexWithDelim(line, "|", 4, token, sizeof(token)))
    return FALSE;
  data->expire = strcasecmp(token, "max") == 0 ? -1 : atoi(token);
  return TRUE;
}
