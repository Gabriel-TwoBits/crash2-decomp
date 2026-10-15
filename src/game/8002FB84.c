#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F210 = 0xA;
s32 D_8005F214 = 0x7F;

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FB84);

s32 func_8002FC78(void *arg0, s32 arg1, s32 arg2) {
    s32 *p = (s32 *)((char *)func_80014260(arg0, 1, 0) + 0x14);
    return p[arg1 * 2 + arg2] + 3;
}

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FCD0);

void func_8002FCE8(void) {
}

void func_8002FCF0(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FCF8);

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FDCC);
