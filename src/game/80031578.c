#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F224 = 1;

s32 func_80031578(s32 arg0) {
    s32 seed;
    u32 q;
    s32 v;
    s32 r;
    if (arg0 == 0) {
        return 0;
    }
    seed = D_8005F224 * 0x41C64E6D + 0x3039;
    D_8005F224 = seed;
    q = (u32)seed / 15;
    v = q % 0x7FFFFFF;
    if (v >= 0) {
        r = v % arg0;
    } else {
        r = -v % arg0;
    }
    return r;
}

/* The seed func_80031658 restores, also the generator's increment */
#define RANDOM_RESET_SEED 0x3039

void func_80031658(void) {
    D_8005F224 = RANDOM_RESET_SEED;
}

INCLUDE_ASM("asm/nonmatchings/game/80031578", func_80031668);

int CdIntToPos(int, unsigned char *);
int CdRead(int, unsigned long *, int);

s32 func_80031680(s32 arg0, s32 arg1, unsigned long *arg2) {
    u8 sp10[8];

    CdIntToPos(arg1, sp10);
    CdControl(2, sp10, 0);
    CdRead(arg0, arg2, 0x80);
    return CdReadSync(0, 0) == 0;
}
