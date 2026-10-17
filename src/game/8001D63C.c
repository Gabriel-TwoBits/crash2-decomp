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

#ifdef NON_MATCHING
#define SCALE_CAP 0x66
#define SCALE_SHIFT 10
#define SCALE_ROUND 0x3FF
#define ANGLE_MASK 0xFFF
#define ANGLE_SIGN_SHIFT 20
#define FLAG_REACHED 0x800
#define SCALE_OFFSET 0x14
#define FLAGS_OFFSET 0xA8

s32 func_8001E544(s32 arg0, s32 arg1, s32 arg2, void *arg3) {
    u32 scale;
    s32 product;
    s32 step;
    s32 diff;
    s32 dist;
    s32 snap;

    scale = *(u32 *)((u8 *)D_80063450 + SCALE_OFFSET);
    if (scale > SCALE_CAP) {
        scale = SCALE_CAP;
    }
    product = arg2 * scale;
    if (product < 0) {
        product += SCALE_ROUND;
    }
    step = product >> SCALE_SHIFT;

    diff = (s32)((arg1 - arg0) << ANGLE_SIGN_SHIFT) >> ANGLE_SIGN_SHIFT;
    dist = diff;
    if (diff < 0) {
        dist = -diff;
    }

    if (diff == 0) {
        snap = 1;
    } else if (step >= 0) {
        snap = dist < step && (step ^ diff) >= 0;
    } else {
        snap = dist < -step && (step ^ diff) >= 0;
    }

    if (snap) {
        if (arg3 != 0) {
            *(s32 *)((u8 *)arg3 + FLAGS_OFFSET) |= FLAG_REACHED;
        }
        return arg1;
    } else {
        return (arg0 + step) & ANGLE_MASK;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E544);
#endif

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

#ifdef NON_MATCHING
extern void MulMatrix(s16 *, s16 *);
extern void ScaleMatrix(s16 *, s32);

void func_8001E780(s16 *arg0, s32 *arg1, s32 arg2) {
    s16 sp[16];
    s16 c;
    s32 s;
    s32 one;

    one = 0x1000;
    s = func_8003D2EC(arg1[2] & 0xFFF);
    c = func_8003D360(arg1[2] & 0xFFF);
    arg0[0] = c;
    arg0[1] = 0;
    arg0[2] = s;
    arg0[3] = 0;
    arg0[4] = one;
    arg0[5] = 0;
    arg0[6] = -s;
    arg0[7] = 0;
    arg0[8] = c;
    s = func_8003D2EC(arg1[0]);
    c = func_8003D360(arg1[0]);
    sp[0] = one;
    sp[1] = 0;
    sp[2] = 0;
    sp[3] = 0;
    sp[4] = c;
    sp[5] = -s;
    sp[6] = 0;
    sp[7] = s;
    sp[8] = c;
    MulMatrix(arg0, sp);
    s = func_8003D2EC(-((arg1[2] - arg1[1]) & 0xFFF));
    c = func_8003D360(-((arg1[2] - arg1[1]) & 0xFFF));
    sp[0] = c;
    sp[1] = 0;
    sp[2] = s;
    sp[3] = 0;
    sp[4] = one;
    sp[5] = 0;
    sp[6] = -s;
    sp[7] = 0;
    sp[8] = c;
    MulMatrix(arg0, sp);
    if (arg2 != 0) {
        ScaleMatrix(arg0, arg2);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E780);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E8E4);

#ifdef NON_MATCHING
#define FIXED_SHIFT 4
#define POS_OFFSET 0x60
#define MATRIX_OFFSET 8

void func_8001E9EC(void *arg0, void *arg1, void *arg2) {
    s32 *in = (s32 *)arg1;
    s32 *out = (s32 *)arg2;
    s32 *base = (s32 *)((u8 *)arg0 + POS_OFFSET);
    s16 *matrix = *(s16 **)((u8 *)arg0 + MATRIX_OFFSET);
    s32 vec[3];
    s32 res[3];

    vec[0] = in[0] / (1 << FIXED_SHIFT);
    vec[1] = in[1] / (1 << FIXED_SHIFT);
    vec[2] = in[2] / (1 << FIXED_SHIFT);
    func_80018194(vec, matrix, res);
    out[0] = (res[0] << FIXED_SHIFT) + base[0];
    out[1] = (res[1] << FIXED_SHIFT) + base[1];
    out[2] = (res[2] << FIXED_SHIFT) + base[2];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E9EC);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EAD8);

#ifdef NON_MATCHING
extern s32 RotTrans(s16 *, s32 *, s32 *, s32);
extern void SetRotMatrix(void *);
extern void SetTransMatrix(void *);
extern char D_80060130;
extern char D_80060774;
extern char D_800607D4;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_8006083C;
extern s32 D_80060840;
extern s32 D_80060844;

s32 func_8001EC2C(s32 *arg0, s32 *arg1, s32 arg2) {
    s16 sv[3];
    s32 wv[3];
    s32 out[2];
    void *m;
    s32 a2;
    s32 a3;
    s32 x;

    wv[0] = arg0[0];
    wv[1] = arg0[1];
    m = &D_80060774;
    wv[2] = arg0[2];
    if (arg2 != 0) {
        m = &D_800607D4;
    }
    SetRotMatrix(m);
    SetTransMatrix(&D_80060130);
    if (arg2 != 0) {
        a2 = D_80060840;
        a3 = D_80060844;
        x = wv[0] - D_8006083C;
    } else {
        a2 = D_800607F8;
        a3 = D_800607FC;
        x = wv[0] - D_800607F4;
    }
    sv[0] = x >> 8;
    sv[1] = (wv[1] - a2) >> 8;
    sv[2] = (wv[2] - a3) >> 8;
    RotTrans(sv, arg1, out, a3);
    a2 = arg1[0];
    a3 = arg1[1];
    x = arg1[2];
    arg1[0] = a2 << 8;
    arg1[1] = -(a3 << 8);
    arg1[2] = x << 8;
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EC2C);
#endif

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

#ifdef NON_MATCHING
#define SLOT_FREE 1
#define OWNER_OFFSET 0x10
#define FLAGS_OFFSET 0x29C
#define FLAG_LOCKED 0x2000
#define KEY_HI_SHIFT 9
#define KEY_LO_SHIFT 8

extern char *D_800608CC;

void func_8001F8B4(s32 arg0) {
    u16 *p;
    u16 *free_slot;
    u16 key;

    key = (arg0 >> KEY_LO_SHIFT) | (*(s32 *)((char *)D_8006759C + 8) << KEY_HI_SHIFT);
    free_slot = 0;
    p = D_8006935C;
    if (*(s32 *)(*(char **)(D_800608CC + OWNER_OFFSET) + FLAGS_OFFSET) & FLAG_LOCKED) {
        return;
    }
    for (; *p != 0; p++) {
        if (*p == key) {
            return;
        }
        if (*p == SLOT_FREE) {
            free_slot = p;
        }
    }
    if (free_slot == 0) {
        free_slot = p;
    }
    *free_slot = key;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F8B4);
#endif

#ifdef NON_MATCHING
#define KEY_SHIFT_LOW 8
#define KEY_SHIFT_HIGH 9
#define KEY_MASK 0xFFFF


void func_8001F94C(s32 arg0) {
    u16 *p;
    s32 key;

    key = (arg0 >> KEY_SHIFT_LOW) | (((s32 *)D_8006759C)[2] << KEY_SHIFT_HIGH);
    key &= KEY_MASK;
    for (p = D_8006935C; *p != 0; p++) {
        if (*p == key) {
            if (p[1] != 0) {
                *p = 1;
            } else {
                *p = 0;
            }
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F94C);
#endif
