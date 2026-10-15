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

void func_80027600(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_t0;
    s32 var_v1;

    var_v1 = 0;
    var_t0 = 0;
    if (arg3 & 0x100) {
        var_v1 = *(s32 *)((char *)arg0 + 0x34);
    }
    if (arg3 & 0x200) {
        var_t0 = *(s32 *)((char *)arg0 + 0x38);
    }
    if ((var_t0 != 0) || (var_v1 != 0)) {
        func_800176DC(1, arg1, arg2, var_t0, var_v1, 1);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027664);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800277A8);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800278F8);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027980);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027A38);

extern s32 D_80069334;
extern s32 D_80069338;
extern s32 D_8006933C;

s32 func_80027B9C(void) {
    D_80069334 = 0;
    if (D_8006933C != 0) {
        func_800116C0(D_8006933C);
        D_8006933C = 0;
    }
    if (D_80069338 != 0) {
        func_800116C0(D_80069338);
        D_80069338 = 0;
    }
    return -0xFF;
}

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

void func_800289C4(s16 *arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    func_80028968(arg1, &sp10);
    if (arg2 == 0) {
        arg0[0x11] = sp10;
    } else {
        arg0[0x11] = func_8001E5FC(arg0[0x11], sp10, 0x100);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028A28);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028C48);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028FE0);

void func_800294B4(s16 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 var_a1;

    temp_a0 = *(s32 *)((u8 *)arg0 + 0xF0);
    if (temp_a0 != 0) {
        temp_v0 = func_800321D4(temp_a0, 0xF2, 0, 0, 0, 2, 0);
        if ((temp_v0 & 0xFFFF) != 0xFFFF) {
            var_a1 = (u32)(temp_v0 & 0x3F0) >> 4;
            if (var_a1 - 0x30 >= 0x10U) {
                var_a1 = -1;
            }
            func_800289C4(arg0, var_a1, 0);
        }
    }
}

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
