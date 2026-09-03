#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CDKEYLEN 32
#define FALSE 0
#define ISSPACETAB(c) ((c) == ' ' || (c) == '\t')
enum { CHAR_BASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, CHAR_CDKEY, CHAR_NAME,
       CHAR_WORKOBJINDEX, CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_COLORYELLOW };
static int images[2][2], writes;
static void CHAR_talkToCli(int a, int b, const char *text, int color) {
  (void)a; (void)b; (void)text; (void)color;
}
static int CHAR_getPlayerMaxNum(void) { return 2; }
#define CHAR_CHECKINDEX(i) ((i) >= 0 && (i) < 2)
static const char *CHAR_getChar(int i, int field) {
  (void)field; return i == 0 ? "self" : "target";
}
static void CHAR_setInt(int i, int field, int value) { images[i][field] = value; ++writes; }
static void CHAR_complianceParameter(int i) { (void)i; }
static int CHAR_getWorkInt(int i, int field) { (void)field; return i; }
static void CHAR_sendCToArroundCharacter(int i) { (void)i; }
static void CHAR_send_P_StatusString(int i, int field) { (void)i; (void)field; }
void easyGetTokenFromString(const char *, int, char *, int);
void CHAR_CHAT_DEBUG_metamo(int, char *);
int main(void) {
  char *invalid[] = {"", " ", "abc", "0", "99", "-1", "100230x", "100230.5",
                     "999999999999999999999", "1002300", "0 target", "abc target"};
  images[0][0] = images[0][1] = 100230;
  images[1][0] = images[1][1] = 100220;
  for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
    CHAR_CHAT_DEBUG_metamo(0, invalid[i]);
    assert(writes == 0 && images[0][0] == 100230 && images[0][1] == 100230);
    assert(images[1][0] == 100220 && images[1][1] == 100220);
  }
  CHAR_CHAT_DEBUG_metamo(0, "100250");
  assert(writes == 2 && images[0][0] == 100250 && images[0][1] == 100250);
  CHAR_CHAT_DEBUG_metamo(0, "100230 target");
  assert(writes == 4 && images[1][0] == 100230 && images[1][1] == 100230);
  CHAR_CHAT_DEBUG_metamo(0, "100230 offline");
  assert(writes == 4);
  puts("Metamo regression tests passed.");
}
