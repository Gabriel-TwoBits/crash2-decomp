#include "common.h"
#include "sdk.h"
#include "game.h"

s32 g_RandSeed = 1;

s32 Rand_Below(s32 arg0) {
    s32 seed;
    u32 q;
    s32 v;
    s32 r;
    if (arg0 == 0) {
        return 0;
    }
    seed = g_RandSeed * 0x41C64E6D + 0x3039;
    g_RandSeed = seed;
    q = (u32)seed / 15;
    v = q % 0x7FFFFFF;
    if (v >= 0) {
        r = v % arg0;
    } else {
        r = -v % arg0;
    }
    return r;
}

/* The seed Rand_ResetSeed restores, also the generator's increment */
#define RANDOM_RESET_SEED 0x3039

void Rand_ResetSeed(void) {
    g_RandSeed = RANDOM_RESET_SEED;
}

s32 func_80031668(s32 arg0, s32 arg1) {
    s32 diff = arg0 - arg1;

    if (diff < 0) {
        diff = arg1 - arg0;
    }
    return diff;
}

int CdIntToPos(int, unsigned char *);
int CdRead(int, unsigned long *, int);

s32 func_80031680(s32 arg0, s32 arg1, unsigned long *arg2) {
    u8 sp10[8];

    CdIntToPos(arg1, sp10);
    CdControl(2, sp10, 0);
    CdRead(arg0, arg2, 0x80);
    return CdReadSync(0, 0) == 0;
}
