#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005B88C[];

u8 *D_8005F1EC = D_8005B88C;
s32 D_8005F1F0 = 0x80;

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001D63C);

extern s32 *D_80063450;

s32 func_8001E3DC(s32 arg0, s32 arg1, s32 arg2, s32 *arg3) {
    u32 n;
    s32 prod;
    s32 step;
    s32 diff;
    s32 dist;
    s32 res;

    n = D_80063450[5];
    if (n >= 0x67) {
        n = 0x66;
    }
    prod = arg2 * n;
    step = prod / 1024;
    diff = (s32)((arg1 - arg0) << 20) >> 20;
    dist = diff;
    if (dist < 0) {
        dist = -dist;
    }
    if (arg3 != 0 && (arg3[0xAC / 4] & 0x20000000) && dist < step * 4) {
        if (dist < step * 2) {
            if (dist < step) {
                step = step / 8;
            } else {
                step = step / 4;
            }
        } else if (dist < step * 3) {
            step = step / 2;
        } else {
            step = (step * 3) / 4;
        }
    }
    if (dist < step) {
        arg0 = arg1;
        if (arg3 != 0) {
            arg3[0xA8 / 4] |= 0x800;
        }
    } else {
        if ((dist < 0 ? -dist : dist) == 0x800 && arg1 >= 0x800) {
            diff = -diff;
        }
        if (diff < 0) {
            arg0 = (arg0 - step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] |= 8;
            }
        } else {
            arg0 = (arg0 + step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] &= ~8;
            }
        }
    }
    return arg0;
}

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E544);

s32 func_8001E5FC(s32 arg0, s32 arg1, s32 arg2) {
    s32 diff;

    diff = arg1 - arg0;
    if (arg2 > 0 && (diff < 0 ? -diff : diff) < arg2) {
        return arg1;
    }
    if (diff > 0) {
        return arg0 + arg2;
    }
    return arg0 - arg2;
}

#ifdef NON_MATCHING
s32 func_8001E640(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 sh;
    s32 v;
    s32 c;

    a = arg0 & 0xFFF;
    sh = (arg1 > 0xFFFF) * 2;
    v = (func_8003D2EC(((a % arg1) << 11) / arg1 - 0x400) * arg2) >> 12;
    if (v < 0x1000) {
        c = -0x1000;
        if (v >= -0x1000) {
            c = v;
        }
    } else {
        c = 0x1000;
    }
    return (((c + 0x1000) >> sh) * arg1 >> (13 - sh)) + a / arg1 * arg1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E640);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E780);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E8E4);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E9EC);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EAD8);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EC2C);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001ED60);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EE64);

extern s32 D_8005F38C;
extern s32 D_8006CB10;
extern s32 D_8006CB14;

void func_8001EFD8(void) {
    func_800116C0(D_8006CB14);
    D_8005F38C = 0;
    func_800116C0(D_8006CB10);
}

extern void *D_8006759C;
extern s32 D_8006CB70;

s32 func_8001F018(void) {
    D_8006CB70 = *(s32 *)((s8 *)D_8006759C + 8) << 8;
    if (*(s32 *)((s8 *)D_8006759C + 4) == 0) {
        return -0xE;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F054);

extern u16 D_8006935C[];
extern s32 D_8006B1C4[];

void func_8001F858(s32 arg0) {
    u16 *p;

    for (p = D_8006935C; *p != 0; p++) {
        if ((*p >> 9) == arg0) {
            D_8006B1C4[*p & 0x1FF] |= 8;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F8B4);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F94C);
