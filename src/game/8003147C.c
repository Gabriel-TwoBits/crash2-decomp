#include "common.h"
#include "sdk.h"
#include "game.h"

s32 g_RandSeedAlt = 1;

#ifdef NON_MATCHING
s32 Rand_BelowAlt(s32 arg0) {
    u32 s;
    u32 a;
    s32 v;

    if (arg0 == 0) {
        return 0;
    }
    s = g_RandSeedAlt * 0x41C64E6D + 0x3039;
    a = s / 15;
    g_RandSeedAlt = s;
    v = a - a / 0x7FFFFFF * 0x7FFFFFF;
    if (v >= 0) {
        return v % arg0;
    }
    return -v % arg0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8003147C", Rand_BelowAlt);
#endif

/* The seed Rand_ResetSeedAlt restores, also the generator's increment */
#define RANDOM_RESET_SEED 0x3039

void Rand_ResetSeedAlt(void) {
    g_RandSeedAlt = RANDOM_RESET_SEED;
}

void Rand_SetSeedAlt(s32 seed) {
    g_RandSeedAlt = seed;
}
