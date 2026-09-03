#include <cassert>
#include <cstdio>
#include <vector>
struct ACTION {};
struct Player { int id; ACTION *ptAct; };
struct Party { int useFlag, id; ACTION *ptAct; };
static const int MAX_PARTY = 3;
static ACTION self, remote;
static Player pc = {8014, &self};
static Party party[MAX_PARTY] = {{1, 8014, &self}, {1, 4816, &remote}};
static bool logOutFlag;
static int nowFloor = 100;
static std::vector<int> ids, deleted;
int getInteger62Token(char *, char, int n) {
  return n <= static_cast<int>(ids.size()) ? ids[n - 1] : -1;
}
void ClientRuntimeLog(const char *, const char *, ...) {}
void delCharObj(int id) {
  deleted.push_back(id);
  if (id == pc.id) pc.ptAct = nullptr;
}
void lssproto_CD_recv(int, char *);
int main() {
  char packet[] = "";
  ids = {8014, 4816, 8014, 4817};
  lssproto_CD_recv(0, packet);
  assert(pc.ptAct == &self && party[0].ptAct == &self);
  assert((deleted == std::vector<int>{4816, 4817}));
  assert(party[1].ptAct == nullptr);
  // A delayed self-CD during battle must not recreate a field actor either.
  pc.ptAct = nullptr;
  ids = {8014};
  lssproto_CD_recv(0, packet);
  assert(pc.ptAct == nullptr && deleted.size() == 2);
  logOutFlag = true;
  ids = {4818};
  lssproto_CD_recv(0, packet);
  assert(deleted.size() == 2);
  std::puts("Character deletion regression tests passed.");
}
