#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80063034;

s32 D_8005F3B8;
extern Unk80063034 D_80063034[];

void func_8001A014(void) {
    s32 n;
    s32 i;

    n = D_8005F3B8;
    /* Empty statement: without it the frame adjustment is scheduled before the load. */
    do {
    } while (0);
    for (i = 0; i < n; i++) {
        D_80063034[i].unk0 = 0x13B;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A054);

#ifdef NON_MATCHING
void func_8001A13C(s32 arg0, s32 a1, s32 a2) {
    s32 arg2 = a2;
    s32 arg1 = a1;
    s32 sp28;
    s32 temp_v0;
    s32 var_a1;
    s32 var_a2;

    var_a1 = 0x13C;
    if (arg2 == 2 && (var_a1 = 0x13B, arg2 == 2)) {
        var_a2 = arg1 - 0x100;
    } else {
        var_a2 = arg1 + 0x100;
    }
    temp_v0 = func_80031AE8(arg0, var_a1, var_a2, 0, &sp28, 1, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        func_8001A054(0x13B, temp_v0, sp28);
    }
    var_a1 = (arg2 == 2) ? 0x13C : 0x13B;
    temp_v0 = func_80031AE8(arg0, var_a1, arg1, 0, &sp28, 1, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        func_8001A054(0x13C, temp_v0, sp28);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A13C);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A23C);

#ifdef NON_MATCHING
extern s32 D_8005F38C;

typedef struct {
    u8 pad0[0x20];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30[6];
} Unk8001A4A4Inner;

typedef struct {
    u8 pad0[8];
    Unk8001A4A4Inner *inner;
    u8 pad1[0xDC];
    u8 *e8;
} Unk8001A4A4Obj;

void func_8001A4A4(Unk8001A4A4Obj *arg0, s32 *arg1) {
    s32 sp[6];
    s32 *s;
    s32 neg;
    s32 r;
    u8 *a;

    arg0->inner->f28 = 0;
    a = arg0->e8;
    if (a != 0) {
        if (*a != 4) {
            if (((s32)(a + 4) & 0xFF800003) != 0x80000000) {
                return;
            }
            r = (s32)func_80014260(a + 4, 1, 0);
            arg0->inner->f28 = r;
            if (r >= -0xFE) {
                arg0->inner->f28 = 0;
                return;
            }
        }
        if (*arg0->e8 == 1) {
            do {
                arg0->inner->f2C = func_8001A90C(arg0, (void **)&s, &arg0->inner->f24);
            } while (0);
            arg0->inner->f28 = (s32)func_80014260((s32 *)(arg0->inner->f20 = (s32)s) + 4, 1, 0);
            if ((s32)arg0 == D_8005F38C) {
                neg = arg1[0];
                if (neg < 0) {
                    arg1[0] = -neg;
                }
                sp[0] = s[7] + s[10];
                sp[1] = s[8] + s[11];
                sp[2] = s[9] + s[12];
                sp[3] = s[7] + s[13];
                sp[4] = s[8] + s[14];
                sp[5] = s[9] + s[15];
                arg0->inner->f30[0] = (sp[0] >> 8) * arg1[0] >> 4;
                arg0->inner->f30[1] = (sp[1] >> 8) * arg1[1] >> 4;
                arg0->inner->f30[2] = (sp[2] >> 8) * arg1[2] >> 4;
                arg0->inner->f30[3] = (sp[3] >> 8) * arg1[0] >> 4;
                arg0->inner->f30[4] = (sp[4] >> 8) * arg1[1] >> 4;
                arg0->inner->f30[5] = (sp[5] >> 8) * arg1[2] >> 4;
                arg1[0] = neg;
            }
        } else {
            arg0->inner->f28 = -0xE;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A4A4);
#endif

#ifdef NON_MATCHING
typedef struct Unk8001A710 {
    /* 0x00 */ u8 pad0[0x58];
    /* 0x58 */ struct Unk8001A710 *link;
    /* 0x5C */ u8 pad5C[0x4];
    /* 0x60 */ s32 pos[3];
    /* 0x6C */ u8 pad6C[0x3C];
    /* 0xA8 */ s32 flagsA8;
    /* 0xAC */ u8 padAC[0x54];
    /* 0x100 */ s32 flags100;
    /* 0x104 */ u8 pad104[0x24];
    /* 0x128 */ s32 offset;
} Unk8001A710;


s32 func_8001A710(Unk8001A710 *arg0, s32 *arg1, Unk8001A710 *arg2, s32 *arg3) {
    s32 box0[6];
    s32 box1[6];
    Unk8001A710 *other;
    s32 d;

    other = arg0->link;
    if (other != 0 && other != arg2) {
        if (arg2->flags100 & 0x800) {
            arg2->link = arg0;
            return -0x1C;
        }
        d = func_80030FEC(arg0->pos, other->pos);
        if (!(func_80030FEC(arg0->pos, arg2->pos) < d) && !(other->flags100 & 0x800)) {
            return -0x1C;
        }
    }
    arg0->link = arg2;
    arg2->link = arg0;
    if (arg1 == 0) {
        return -0xFF;
    }
    if (arg3 != 0) {
        if (arg0->offset != 0) {
            box0[0] = arg1[0] + arg0->offset;
            box0[1] = arg1[1];
            box0[2] = arg1[2] + arg0->offset;
            box0[3] = arg1[3] - arg0->offset;
            box0[4] = arg1[4];
            box0[5] = arg1[5] - arg0->offset;
            if (func_80030EC0(box0, arg3) != 0) {
                arg0->flagsA8 |= 0x1000;
            }
        }
        if (arg2->offset != 0) {
            box1[0] = arg3[0] + arg2->offset;
            box1[1] = arg3[1];
            box1[2] = arg3[2] + arg2->offset;
            box1[3] = arg3[3] - arg2->offset;
            box1[4] = arg3[4];
            box1[5] = arg3[5] - arg2->offset;
            if (func_80030EC0(box1, arg1) != 0) {
                arg2->flagsA8 |= 0x1000;
            }
        }
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A710);
#endif

#ifdef NON_MATCHING
u32 func_8001A90C(void *arg0, s32 *arg1, s32 *arg2) {
    char *s2;
    char *s3;
    s32 odd;
    u32 idx;
    s32 *p;

    s2 = *(char **)((char *)arg0 + 0xE8);
    s3 = s2 + 4;
    func_80014260(s3, 1, 0);
    odd = *(s32 *)(s2 + 8) & 1;
    idx = *(u32 *)((char *)arg0 + 0xEC) >> 8;
    if (arg2 == 0) {
        p = (s32 *)((char *)func_80014260(s3, 1, 0) + 0x10);
        if (odd) {
            *arg1 = p[idx >> 1];
        } else {
            *arg1 = p[idx];
        }
        return idx;
    }
    if (odd == 0) {
        p = (s32 *)((char *)func_80014260(s3, 1, 0) + 0x10);
        *arg1 = *arg2 = p[idx];
    } else if ((idx & 1) && (s32)(idx + 1) < *(u16 *)(s2 + 2)) {
        u32 h = idx >> 1;
        *arg1 = ((s32 *)((char *)func_80014260(s3, 1, 0) + 0x10))[h];
        *arg2 = ((s32 *)((char *)func_80014260(s3, 1, 0) + 0x10))[h + 1];
    } else {
        s32 h = (s32)(idx + 1) >> 1;
        p = (s32 *)((char *)func_80014260(s2 + 4, 1, 0) + 0x10);
        *arg1 = *arg2 = p[h];
    }
    return idx;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A90C);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001AA80);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001ABF8);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001B83C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BD84);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BFD4);
