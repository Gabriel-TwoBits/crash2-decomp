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

/* This and the next 19 functions only store arg3 in its slot of the caller's frame: what taking its address does */
void func_8003230C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032314(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003231C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032324(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003232C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032334(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003233C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032344(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

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

#ifdef NON_MATCHING
extern u32 D_8005FC84;
extern s32 D_8005FC8C;
extern s32 D_8005FC94;
extern s32 D_8005FC98;
extern u8 D_8005FC9C;
extern u8 D_8005FC9D;
extern s16 D_8005FC9E;
extern s16 D_8005FCA0;
extern s16 D_8005FCA2;
extern s32 D_8005FCA4;
extern s32 D_8005FCA8;
extern s32 D_8005FCAC;

void func_800323AC(void) {
    D_8005FC94 = 0x6396347F;
    D_8005FC9C = 1;
    D_8005FC9D = 0x80;
    D_8005FC9E = 0x3FFF;
    D_8005FCA2 = 0x1000;
    D_8005FC8C = 0;
    D_8005FCA0 = 0;
    D_8005FC98 = 0;
    D_8005FCA4 = 0;
    D_8005FCA8 = 0;
    D_8005FCAC = 0;
    D_8005FC84 &= ~2;
    D_8005FC84 &= ~1;
    D_8005FC84 &= ~4;
    D_8005FC84 &= ~8;
    D_8005FC84 &= ~0x10;
    D_8005FC84 &= ~0x20;
    D_8005FC84 &= ~0x40;
    D_8005FC84 &= ~0x80;
    D_8005FC84 &= ~0x100;
    D_8005FC84 |= 0x600;
    D_8005FC84 &= ~0x800;
    D_8005FC84 &= ~0x7000;
    D_8005FC84 &= 0xFFFF7FFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_800323AC);
#endif

void func_80032498(void) {
}

void func_800324A0(void) {
}
