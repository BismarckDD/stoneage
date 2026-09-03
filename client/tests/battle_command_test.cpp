#include <cassert>
#include <cstdio>
#include <cstring>

// A nonzero preceding byte exposes the old parser's dependence on adjacent memory.
static char storage[256];
static char *BattleCmd = storage + 1;
static int command_point, sav_command_point, command_no, executed;
static int actorState;
static int *a1 = &actorState;
#define ATR_VCT_NO(actor) (*(actor))
#define ATT_MALFUNCTION 'M'
static char get_command(void);
void dispatch();

static void load(const char *text) {
    storage[0] = 'X';
    std::strcpy(BattleCmd, text);
    command_point = 0;
}

int main() {
    load("BH|a0|rA|f0|dB|p0|FF|BH|a5|rA|f0|d1|p0|FF|");
    assert(get_command() == 'H');
    assert(command_point == 2);
    assert(get_command() == 'H'); // Hex damage B must not be read as a command.
    assert(command_point == 23);
    assert(get_command() == -1);
    int end = command_point;
    assert(get_command() == -1 && command_point == end);

    load("BH|a0|rA|f0|d1|p0|FF|");
    sav_command_point = command_point;
    command_no = get_command();
    command_point = 6; // Attacker number has also been consumed by master().
    actorState = 4; // Still returning from its preceding action.
    dispatch();
    assert(executed == 0 && command_point == sav_command_point);
    command_no = get_command();
    assert(command_no == 'H');
    actorState = 0;
    dispatch();
    assert(executed == 1 && command_point == 2);

    load("BM|0|");
    command_no = get_command();
    actorState = 4;
    dispatch(); // Malfunction commands retain their existing readiness exception.
    assert(executed == 2);

    const char *truncated[] = {"", "B", "BH"};
    for (const char *text : truncated) {
        load(text);
        assert(get_command() == -1);
        assert(command_point == static_cast<int>(std::strlen(text)));
    }
    std::puts("Battle command regression tests passed.");
}
