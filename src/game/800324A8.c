#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F244 = 0x1E;
s32 D_8005F248 = 0;
s32 D_8005F24C = 0;
s16 D_8005F250 = 0;
s32 D_8005F254 = 0;
s32 D_8005F258 = -1;
s32 D_8005F25C = -1;
s32 D_8005F260 = 0;

typedef struct {
    s32 mode;
    s16 left;
    s16 right;
    s32 pad[7];
} Unk800324A8;

s16 D_8005F2FC;
s16 D_8005F300;
s32 D_8005F308;
extern u8 D_8005F30C[];
extern s32 D_8005F4F0[];
extern u8 D_8005FCF8[];
extern s32 D_8006CBF8;

void func_800324A8(void) {
    Unk800324A8 cfg;
    s32 i;
    s32 *p;
    s32 mask;
    s16 vol;

    D_8005F248 = 1;
    i = 0;
    mask = ~8;
    for (p = D_8005F4F0; i < 24; i++) {
        *p &= mask;
        p += 0x14;
    }
    vol = 0x3FFF;
    SsInit();
    SsSetTableSize(D_8005FCF8, 1, 2);
    SpuInitMalloc(0, D_8005F30C);
    cfg.mode = 3;
    cfg.left = vol;
    cfg.right = vol;
    func_800513D8(&cfg);
    func_800512F8(0);
    func_800323AC();
    func_800325D0((D_8006CBF8 * 0x3FFF00) >> 16);
    D_8005F2FC = vol;
    D_8005F300 = 0;
    SsSetMVol(0x7F, 0x7F);
    D_8005F308 = 0;
}

typedef struct {
    s32 a;
    s32 b;
    s16 c;
    s16 d;
    s32 e;
    s32 f;
} Unk80032588;

extern void func_8004FE38(s32);
extern void func_800501A8(Unk80032588 *);

void func_80032588(void) {
    Unk80032588 s;

    if (D_8005F248 != 0) {
        s.a = 7;
        s.b = 0;
        s.c = 0;
        s.d = 0;
        func_800501A8(&s);
        func_8004FE38(0);
        D_8005F248 = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800325D0);

extern void SpuSetReverbDepth(Unk80032588 *);

s32 func_800325DC(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s32 arg4) {
    Unk80032588 s;

    s.a = 7;
    s.b = arg0 | 0x100;
    s.c = arg1;
    s.d = arg2;
    if ((u32)(arg0 - 7) < 2U) {
        s.e = arg3;
        s.f = arg4;
    }
    func_800501A8(&s);
    SpuSetReverbDepth(&s);
    func_8004FE38(arg0 != 0);
    return 0;
}

#ifdef NON_MATCHING
s16 D_8005F2E4;
s16 D_8005F2E6;
extern s32 D_8006CBF4;

s16 *func_8003264C(s32 *arg0, s32 arg1) {
    s32 x;
    s32 y;
    s32 d;
    s32 a;
    s32 lo;
    s32 hi;
    s16 c;

    x = arg0[0] / 32;
    y = (arg0[2] - 0x4B000) / 32;
    d = func_8003BEA8(x, y);
    if (d <= 0x7FFF) {
        D_8005F2E4 = arg1;
        goto set6;
    }
    a = func_8003D360(ratan2(y, x)) + 0x1000;
    lo = (arg1 << 15) / d;
    hi = lo;
    if (D_8006CBF4 == 0) {
        lo = (lo + lo * (0x2000 - a)) >> 11;
        hi = (hi + hi * a) >> 11;
    }
    if (lo < arg1) {
        if (lo >= 0) {
            D_8005F2E4 = lo;
        } else {
            D_8005F2E4 = 0;
        }
    } else {
        D_8005F2E4 = arg1;
    }
    if (hi < arg1) {
        if (hi >= 0) {
            D_8005F2E6 = hi;
        } else {
            D_8005F2E6 = 0;
        }
    } else {
set6:
        D_8005F2E6 = arg1;
    }
    c = D_8005F2E4;
    if (c >= 0x4000) {
        c = 0x3FFF;
    }
    d = D_8005F2E6;
    D_8005F2E4 = c;
    if (d >= 0x4000) {
        d = 0x3FFF;
    }
    D_8005F2E6 = d;
    return &D_8005F2E4;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_8003264C);
#endif

typedef struct {
    s32 a;
    s32 flags;
    s32 pad8;
    struct Obj *key;
    u8 pad10[0xD];
    u8 b;
    u8 pad1E[0x32];
} Ent;

s32 D_8005F304;
extern Ent D_8005F4EC[];

void func_80032808(struct Obj *arg0) {
    s32 sp10;
    s32 i;
    Ent *p;

    if (arg0 != 0) {
        i = D_8005F304;
        p = &D_8005F4EC[i];
        for (; i < 0x18; i++, p++) {
            if ((p->flags & 8) && p->key == arg0) {
                if (i) {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                } else {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                }
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800328D4);

u32 func_80032A98(s32 *arg0, s32 *arg1) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_a0;
    s32 var_v0_2;
    u32 var_a0_2;
    u32 var_v0;

    var_a0 = *arg1;
    if (!(var_a0 & 1)) {
        var_a0 = ((s32 *)var_a0)[1];
    }
    temp_v0 = (s32)func_8001557C(var_a0);
    arg0[0] = arg0[0] & ~0x7000;
    arg0[3] = *arg1;
    arg0[5] = temp_v0;
    if (temp_v0 != 0) {
        var_a0_2 = 0;
        var_v0_2 = (arg0[0] & ~0x7000) | 0xA000;
        goto block_10;
    }
    temp_v0_2 = (s32)func_80014260(arg1, 0, 1);
    if (temp_v0_2 != 0) {
        if (temp_v0_2 >= -0xFE) {
            return -0xE;
        }
        if ((temp_v0_2 != 0) && ((temp_v0_2 & 0xC0000002) == 0xC0000002)) {
            var_a0_2 = (u32) (temp_v0_2 * 4) >> 4;
        } else {
            goto block_9;
        }
        goto block_11;
    } else {
block_9:
        var_a0_2 = 0;
        var_v0_2 = (arg0[0] & ~0x7000) | 0x9000;
block_10:
        arg0[0] = var_v0_2;
block_11:
        var_v0 = var_a0_2;
    }
    return var_v0;
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032B7C);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032F18);

void func_80013E10(void *arg0);
void func_80051078(s32 arg0, s32 arg1);

void func_80033448(void *arg0) {
    do { func_80051078(0, 1 << ((u16 *)arg0)[3]); } while (0);
    ((s32 *)arg0)[1] &= ~8;
    if (((void **)arg0)[6] != 0) {
        func_80013E10(((void **)arg0)[6]);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800334A0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033C84);

extern s32 D_8006CBFC;
extern s32 D_8005F3EC;
s32 D_8005F2F4;
s32 D_8005F39C;
s16 D_8005F3E4;
s16 D_8005F3E0;

void func_80033D10(void) {
    D_8005F24C = 1;
    SsSetTickMode(1);
    func_80033C84((D_8006CBFC * 0x3FFF) << 8 >> 16);
    D_8005F2F4 = 0x6396347F;
    D_8005F39C = 0;
    D_8005F3E4 = -1;
    D_8005F3E0 = -1;
    switch (D_8005F3EC) {
    case 2: case 5: case 6: case 7: case 8: case 9:
    case 16: case 18: case 25: case 26: case 32: case 38:
        D_8005F304 = 0x12;
        break;
    case 23: case 24: case 27:
        D_8005F304 = 0x11;
        break;
    case 15:
        D_8005F304 = 0xC;
        break;
    case 1:
    default:
        D_8005F304 = 0x10;
        break;
    }
    SsSetReservedVoice((u8)D_8005F304);
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033DD0);

typedef struct Unk80033DE0 {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk80033DE0;

s32 func_800344B0(Unk80033DE0 *, s32, s32);
void func_80056304(s16, s16);
void func_80056760(s16, s16, s16, s16);
extern Unk80033DE0 D_8005FCD0[];
extern s16 D_8005FCE4;
extern s32 D_8006CBFC;
s16 D_8005F2F8;

void func_80033DE0(s32 arg0) {
    s32 a;
    Unk80033DE0 *p;

    if (D_8005F2F8 >= 2) {
        a = arg0 >> 8;
        if (D_8005F39C == 3) {
            if (a <= 0 || a < 3 || a != D_8005F39C) {
                p = &D_8005FCD0[1];
                func_800344B0(p, 1, 0);
                func_800344B0(p - 1, 3, 0);
                D_8005F308 = 0;
            } else {
                if (D_8006CBFC != 0 || D_8006CBF8 != 0) {
                    D_8005F308 = 0x3FFF;
                } else {
                    D_8005F308 = 0;
                }
                p = &D_8005FCD0[0];
                func_800344B0(p, 2, 0);
                func_800344B0(p + 1, 0, 1);
            }
            func_80056760(D_8005F3E4, D_8005FCE4, D_8005F308, D_8005F308);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033EE0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80034238);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80034254);

void func_8003444C(s16 a, s32 b);

void func_80034394(void) {
    if (D_8005F24C != 0) {
        func_8003444C(D_8005F3E4, 1);
        SsEnd();
        D_8005F24C = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800343CC);

void SsSepClose(s16);
void SsVabClose(s16);

void func_8003444C(s16 a, s32 b) {
    if (a != -1) {
        SsSepClose(a);
    }
    if ((b != 0) && (D_8005F3E0 != -1)) {
        SsVabClose(D_8005F3E0);
    }
}

/* Returns nothing, yet declared s32: as void, GCC fills the delay slot before the jump table otherwise. */
s32 func_800344B0(Unk80033DE0 *obj, s32 cmd, s32 vol) {
    s32 i;
    s32 pad[2];

    if (obj != 0) {
        switch (cmd) {
        case 0:
            obj->state = 1;
            SsSepPlay(D_8005F3E4, obj->seq, 1, (s16)vol);
            break;
        case 1:
            obj->state = 2;
            func_80056304(D_8005F3E4, obj->seq);
            break;
        case 2:
            obj->state = 0;
            SsSepPause(D_8005F3E4, obj->seq);
            break;
        case 3:
            obj->state = 1;
            SsSepReplay(D_8005F3E4, obj->seq);
            break;
        case 4:
            i = 0;
            obj->volL = vol >> 16;
            obj->volR = vol;
            if (D_8005F2F8 > 0) {
                do {
                    i++;
                    func_80056760(D_8005F3E4, obj->seq, obj->volL, obj->volR);
                } while (i < D_8005F2F8);
            }
            break;
        default:
            break;
        }
    }
}
