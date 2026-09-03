#include <cassert>
#include <cstdint>
#include <cstdio>
using U4 = uint32_t;
enum { ANIM_ATTACK, ANIM_DAMAGE, ANIM_DEAD, ANIM_STAND, ANIM_WALK };
enum { ANM_LOOP, ANM_NO_LOOP, CG_INVISIBLE = 99, SPRSTART = 100000, mxSPRITE = 2 };
struct ACTION {
    int gra, oldGra, anim, oldAnim, dir, oldDir, timer, frame, hit, bmp, oldBmp, x, y;
};
#define ATR_CHR_NO(a) ((a)->gra)
#define ATR_CHR_NO_OLD(a) ((a)->oldGra)
#define ATR_CHR_ACT(a) ((a)->anim)
#define ATR_CHR_ACT_OLD(a) ((a)->oldAnim)
#define ATR_CHR_ANG(a) ((a)->dir)
#define ATR_CHR_ANG_OLD(a) ((a)->oldDir)
#define ATR_CHR_TIM(a) ((a)->timer)
#define ATR_CHR_CNT(a) ((a)->frame)
#define ATR_HIT(a) ((a)->hit)
#define ATR_PAT_NO(a) ((a)->bmp)
#define ATR_PAT_BAK_NO(a) ((a)->oldBmp)
#define ATR_CHR_H_POS(a) ((a)->x)
#define ATR_CHR_V_POS(a) ((a)->y)
#define ATR_H_POS(a) ((a)->x)
#define ATR_V_POS(a) ((a)->y)
struct FRAMELIST { int BmpNo, PosX, PosY, SoundNo; };
struct ANIMLIST { int dir, no, dtAnim; U4 frameCnt; FRAMELIST *ptFramelist; };
struct SPRITEDATA { int animSize; ANIMLIST *ptAnimlist; };
static SPRITEDATA SpriteData[mxSPRITE];
int getCharType(ACTION *) { return 0; }
void ClientRuntimeLog(const char *, const char *, ...) {}
void play_se(int, int, int) {}
void realGetNo(int gra, U4 *bmp) { *bmp = gra; }
void realGetPos(int, short *x, short *y) { *x = *y = 0; }
int pattern(ACTION *, int, int);

// Simulate the caller: an end result skips hit handling in monster().
static void checkAttack(int gra) {
    ACTION actor = {};
    actor.gra = gra;
    actor.oldGra = -1;
    actor.oldAnim = -1;
    int hits = 0;
    bool finished = false;
    for (int tick = 0; tick < 20; ++tick) {
        if (pattern(&actor, 1, ANM_NO_LOOP)) {
            finished = true;
            break;
        }
        if (actor.hit) { ++hits; actor.hit = 0; }
    }
    assert(finished && hits == 1);
}

int main() {
    checkAttack(0); // Actual reproduction: invisible player, stuck in attack state.
    checkAttack(-1);
    checkAttack(CG_INVISIBLE);
    checkAttack(SPRSTART + mxSPRITE); // Exact upper bound must not index SpriteData.
    checkAttack(SPRSTART + mxSPRITE + 1);
    checkAttack(SPRSTART); // No sprite loaded.
    SpriteData[0].animSize = 1;
    checkAttack(SPRSTART); // Null animation list.
    FRAMELIST frame = {123, 0, 0, 10000};
    ANIMLIST anim = {0, ANIM_STAND, 1, 1, &frame};
    SpriteData[0].ptAnimlist = &anim;
    checkAttack(SPRSTART); // Sprite exists but has no attack action.
    anim.no = ANIM_ATTACK;
    anim.frameCnt = 0;
    checkAttack(SPRSTART);
    anim.frameCnt = 1;
    anim.ptFramelist = nullptr;
    checkAttack(SPRSTART);
    anim.ptFramelist = &frame;
    checkAttack(SPRSTART); // A valid attack keeps its normal hit/completion behavior.
    ACTION idle = {};
    idle.anim = ANIM_STAND;
    for (int tick = 0; tick < 10; ++tick)
        assert(pattern(&idle, 1, ANM_LOOP) == 0 && idle.hit == 0);
    idle.anim = ANIM_DAMAGE;
    assert(pattern(&idle, 1, ANM_NO_LOOP) == 1 && idle.hit == 0);
    std::puts("Battle animation regression tests passed.");
}
