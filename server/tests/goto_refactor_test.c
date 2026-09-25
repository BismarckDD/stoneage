#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define arraysizeof(a) ((int)(sizeof(a) / sizeof((a)[0])))
#define NONCHAR_DELIMITER "|"
char *strncpysafe(char *, int, const char *);
char *strncpysafe2(char *, int, const char *, const char *);
BOOL getDelimitedField(const char *, const char *, int, char *, int);
/* These fixtures isolate control flow from the game's data tables. */
static char *makeStringFromEscaped(char *s) { return s; }
static int diagnostics;
static void printEx(const char *format, ...) { (void)format; ++diagnostics; }
#define print printEx

enum { SKILL_DATAINTNUM = 2, SKILL_DATACHARNUM = 1 };
typedef struct { char string[128]; } TestString;
typedef struct { int data[2]; TestString string[1]; } Skill;
static struct { const char *dumpskill; } SKILL_setint[] = {{"lv"}, {"id"}};
static struct { const char *dumpskill; } SKILL_setchar[] = {{"name"}};
BOOL SKILL_makeSkillFromStringToArg(char *, Skill *);

enum { ITEM_ID, ITEM_CANBEPILE, ITEM_USEPILENUMS, ITEM_DATA_ENUM_MAX,
       ITEM_CHAR_DATA_ENUM_MAX = 1 };
typedef struct { int data[3]; TestString string[1]; } ITEM_Item;
static struct { const char *dumpchar; int table; } ITEM_setIntData[] = {
    {"id", ITEM_ID}, {"pile", ITEM_CANBEPILE}, {"count", ITEM_USEPILENUMS}};
static struct { const char *dumpchar; int table; } ITEM_setCharData[] = {{"name", 0}};
#define ITEM_CHECKITEMTABLE(id) ((id) == 42)
static void ITEM_getDefaultItemSetting(ITEM_Item *item) {
  memset(item, 0, sizeof(*item));
}
#ifdef _SIMPLIFY_ITEMSTRING
static void ITEM_getDefaultItemData(int id, ITEM_Item *item) {
  ITEM_getDefaultItemSetting(item);
  item->data[ITEM_ID] = id;
}
#endif
BOOL ITEM_makeExistItemsFromStringToArg(char *, ITEM_Item *, int);

enum { CHAR_GOLD, CHAR_DIR, CHAR_X, CHAR_Y, CHAR_FLOOR, CHAR_CDKEY,
       CHAR_ISDIE, CHAR_ACTSTAND, CHAR_P_STRING_GOLD };
#define CHAR_CHECKINDEX(index) ((index) == 1)
static int dropResults[10], dropCalls, watchCalls, statusCalls, forcedCalls;
static int CHAR_getMaxHaveGold(int index) { (void)index; return 1000; }
static int CHAR_getFlg(int index, int flag) { (void)index; (void)flag; return 0; }
static int CHAR_getInt(int index, int field) {
  (void)index; return field == CHAR_GOLD ? 100 : 0;
}
static void CHAR_setInt(int index, int field, int value) {
  (void)index; (void)field; (void)value;
}
static char *CHAR_getChar(int index, int field) {
  (void)index; (void)field; return "test";
}
static int CHAR_getDX(int dir) { (void)dir; return 1; }
static int CHAR_getDY(int dir) { (void)dir; return 0; }
static int CHAR_DropMoneyFXY(int index, int amount, int floor, int x, int y,
                             BOOL force, int *objindex) {
  (void)floor; (void)x; (void)y;
  assert(index == 1 && amount == 10 && dropCalls < 10);
  if (force) { assert(dropCalls == 9); ++forcedCalls; }
  *objindex = 123;
  return dropResults[dropCalls++];
}
static void CHAR_sendWatchEvent(int objindex, int act, void *data, int count,
                                BOOL send) {
  assert(objindex == 123 && act == CHAR_ACTSTAND && !data && !count && send);
  assert(statusCalls == 0);
  ++watchCalls;
}
static void CHAR_send_P_StatusString(int index, int fields) {
  assert(index == 1 && fields == CHAR_P_STRING_GOLD);
  ++statusCalls;
}
void CHAR_DropMoney(int, int);

int main(void) {
  Skill skill = {0};
  assert(SKILL_makeSkillFromStringToArg("unknown=1|lv=3|id=9|name=pet|lv=7", &skill));
  assert(skill.data[0] == 7 && skill.data[1] == 9);
  assert(strcmp(skill.string[0].string, "pet") == 0 && diagnostics == 1);

  ITEM_Item item = {0};
  /* id occurs after another field: neither the scan nor dispatch may skip it. */
  assert(ITEM_makeExistItemsFromStringToArg(
      "unknown=1|id=42|pile=1|name=stone|count=5|count=8", &item, 0));
  assert(item.data[ITEM_ID] == 42 && item.data[ITEM_USEPILENUMS] == 8);
  assert(strcmp(item.string[0].string, "stone") == 0);
  assert(ITEM_makeExistItemsFromStringToArg("name:gold,id:42,pile:0,count:8", &item, 1));
  assert(item.data[ITEM_USEPILENUMS] == 1);
  assert(strcmp(item.string[0].string, "gold") == 0);

  /* Every success position must emit exactly one watch event and gold update. */
  for (int success = 0; success < 10; ++success) {
    for (int i = 0; i < 10; ++i) dropResults[i] = -2;
    dropResults[success] = 0;
    dropCalls = watchCalls = statusCalls = forcedCalls = 0;
    CHAR_DropMoney(1, 10);
    assert(dropCalls == success + 1 && watchCalls == 1 && statusCalls == 1);
    assert(forcedCalls == (success == 9));
  }
  for (int failure = -6; failure <= -1; ++failure) {
    for (int i = 0; i < 10; ++i) dropResults[i] = failure;
    dropCalls = watchCalls = statusCalls = forcedCalls = 0;
    CHAR_DropMoney(1, 10);
    assert(watchCalls == 0 && statusCalls == 0);
    assert(dropCalls == ((failure == -1 || failure == -3) ? 1 : 10));
  }
  puts("Field dispatch, record advancement, and money-drop completion passed.");
  return 0;
}
