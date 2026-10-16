#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80031AE8);

s32 func_800321D4(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    s32 sp28[4];
    u8 *sp38;
    u8 *p;

    p = (u8 *) func_80031AE8(arg0, arg1, arg2, arg3, (s32 *) arg4, arg5, sp28, 1, (s32) &sp38);
    if (p == 0) {
        return arg6;
    }
    switch (sp38[4] & 0x1F) {
    case 1:
        return *p;
    case 17:
        return *(s8 *) p;
    case 2:
        return *(u16 *) p;
    case 18:
        return *(s16 *) p;
    default:
        return *(s32 *) p;
    }
}

s32 func_800322C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp28[4];
    s32 sp38;

    if (func_80031AE8(arg0, arg1, arg2, 0, &sp38, arg3, sp28, 1, 0) != 0) {
        return sp38;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003230C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032314);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003231C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032324);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003232C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032334);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003233C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032344);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003234C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032354);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003235C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032364);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003236C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032374);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003237C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032384);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003238C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_80032394);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_8003239C);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_800323A4);

INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_800323AC);

void func_80032498(void) {
}

void func_800324A0(void) {
}
