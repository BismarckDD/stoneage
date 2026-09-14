#include "version.h"

#include "net.h"
#include "npc_roomadminnew.h"
#include "util.h"


#if !defined(_OFFLINE_SYSTEM)
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
