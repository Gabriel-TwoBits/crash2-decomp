#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F220 = 1;

#ifdef NON_MATCHING
s32 func_8003147C(s32 arg0) {
    u32 s;
    u32 a;
    s32 v;

    if (arg0 == 0) {
        return 0;
    }
    s = D_8005F220 * 0x41C64E6D + 0x3039;
    a = s / 15;
    D_8005F220 = s;
    v = a - a / 0x7FFFFFF * 0x7FFFFFF;
    if (v >= 0) {
        return v % arg0;
    }
    return -v % arg0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8003147C", func_8003147C);
#endif

/* The seed func_8003155C restores, also the generator's increment */
#define RANDOM_RESET_SEED 0x3039

void func_8003155C(void) {
    D_8005F220 = RANDOM_RESET_SEED;
}

void func_8003156C(s32 seed) {
    D_8005F220 = seed;
}
