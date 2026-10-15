#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80026CA0);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80026EF4);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_8002706C);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800270C4);

void func_800271D8(s32 *arg0, s32 arg1);

void func_8002719C(s32 *arg0) {
    arg0[0] = -0x100;
    arg0[5] = 0;
    func_800271D8(arg0, 1);
    arg0[0] = 0;
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800271D8);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800272A4);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027420);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027500);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027600);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027664);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800277A8);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800278F8);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027980);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027A38);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027B9C);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027C00);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027CD0);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027DEC);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027F3C);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028750);

extern s32 D_8005BB68[];

void func_8002890C(u8 *arg0, s32 arg1, s32 arg2) {
    s32 off;

    if (*(s32 *)(arg0 + 0xAC) & 0x04000000) {
        off = -((*(s32 *)(arg0 + 0x100) & 0x10) == 0) & 0x18;
        if (arg1 >= 0) {
            *(s32 *)(arg0 + 0x118) = off + D_8005BB68[arg1];
        } else {
            *(s32 *)(arg0 + 0x118) = off + arg2;
        }
    }
}

extern u8 D_8005BB78[];

void func_80028968(s32 arg0, s32 *arg1) {
    if (((u32) (arg0 - 0x30) < 0x10U) && (*arg1 = 0, ((arg0 < 0x40) != 0))) {
        if (arg0 >= 0x30) {
            *arg1 = 0x1000 - (D_8005BB78[arg0] * 0x14);
        }
    } else {
        *arg1 = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800289C4);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028A28);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028C48);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028FE0);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800294B4);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80029538);

void func_8002973C(s32 arg0, s32 arg1) {
    func_80029538(arg0 + 0x60, arg0, arg1);
}

extern s32 D_8005F390;
extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_80029768(void) {
    s16 *p;

    p = (s16 *)func_8001148C(0xBE4);
    D_8005F390 = (s32)p;
    D_8005F3D0 = (s32)p;
    if (p != 0) {
        p[1] = 0;
        p = (s16 *)func_8001148C(0xBE4);
        D_8005F398 = (s32)p;
        if (p != 0) {
            p[1] = 0;
            return -0xFF;
        }
    }
    return -0xF;
}

extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_800297C8(void) {
    func_800116C0(D_8005F3D0);
    func_800116C0(D_8005F398);
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80029800);
