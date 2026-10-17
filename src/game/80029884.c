#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F204 = 0xF424;

#ifdef NON_MATCHING
#define POS_TOLERANCE_INPUT 10   /* snap window for the stored input vector */
#define POS_TOLERANCE_OUTPUT 2   /* window for "position unchanged" after the move */
#define SPEED_FLAGS_MASK 0x20000000
#define SPEED_DEFAULT 0xF424
#define SPEED_BOOSTED 0x27AC4
#define FLAG_ACTIVE 0x400
#define STATE_MAX_SIMPLE 0x3FFFF
#define STATE_RANGE_END 0x40024
#define STATE_RANGE_BASE 0x4001D
#define SHIFT_COUNT_MASK 31
#define STATE_ARG_DEFAULT 0x6400
#define LEVEL_FLOOR_MARGIN 0x19000
#define NEAR_ABS(a, b, n) ((((a) - (b)) >= 0) ? (((a) - (b)) < (n)) : (((b) - (a)) < (n)))

extern void *D_8006759C;
extern s32 D_8006CD48;
extern s32 D_8005F204;
extern s32 D_8005F2D0;
extern s32 D_8005F4A0;
extern s32 D_8005F4A4;
extern s32 D_8005F4A8;

void func_80029884(void *arg0, s32 arg1, s32 arg2) {
    s32 pos[4];
    s32 vel[4];
    s32 shiftArg[2];
    s32 *in = (s32 *)arg1;
    u32 state;
    s32 speed;
    u32 kind;

    if (arg0 == 0) {
        D_8005F2D0 = 0;
        return;
    }
    *(s32 *)((u8 *)*(void **)((u8 *)arg0 + 8) + 0x48) = 0;
    kind = *(u32 *)((u8 *)D_8006759C + 8);
    if (kind == 0x1D || kind == 0x17 || kind == 0x22 || kind == 0x25) {
        if (*(s32 *)((u8 *)arg0 + 0x100) & SPEED_FLAGS_MASK) {
            speed = SPEED_BOOSTED;
        } else {
            speed = SPEED_DEFAULT;
        }
    } else {
        speed = SPEED_DEFAULT;
    }
    D_8005F204 = speed;
    pos[0] = *(s32 *)((u8 *)arg0 + 0x60);
    pos[1] = *(s32 *)((u8 *)arg0 + 0x64);
    pos[2] = *(s32 *)((u8 *)arg0 + 0x68);
    vel[0] = in[0];
    vel[1] = in[1];
    vel[2] = in[2];
    if (D_8005F2D0 != 0 && NEAR_ABS(D_8005F4A0, in[0], POS_TOLERANCE_INPUT) &&
        NEAR_ABS(D_8005F4A4, in[1], POS_TOLERANCE_INPUT) &&
        NEAR_ABS(D_8005F4A8, in[2], POS_TOLERANCE_INPUT)) {
        vel[0] = 0;
        vel[2] = 0;
    }
    func_80029D0C(arg0, arg2, pos, vel);
    if (D_8005F2D0 == 0 && (in[0] != 0 || in[2] != 0) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x60), pos[0], POS_TOLERANCE_OUTPUT) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x64), pos[1], POS_TOLERANCE_OUTPUT) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x68), pos[2], POS_TOLERANCE_OUTPUT)) {
        D_8005F4A0 = in[0];
        D_8005F4A4 = in[1];
        D_8005F2D0 = 1;
        D_8005F4A8 = in[2];
    } else {
        D_8005F2D0 = 0;
    }
    *(s32 *)((u8 *)arg0 + 0x60) = pos[0];
    *(s32 *)((u8 *)arg0 + 0x64) = pos[1];
    *(s32 *)((u8 *)arg0 + 0x68) = pos[2];
    if (!(*(s32 *)((u8 *)arg0 + 0xA8) & FLAG_ACTIVE)) {
        return;
    }
    state = *(u32 *)((u8 *)arg0 + 0x11C);
    if (state > STATE_MAX_SIMPLE) {
        if (state > STATE_RANGE_END) {
            shiftArg[0] = (state - STATE_RANGE_END) << 8;
            func_80025FDC(0x100, shiftArg);
        } else {
            shiftArg[0] = 0x100 << ((state - STATE_RANGE_BASE) & SHIFT_COUNT_MASK);
            func_80025FDC(0, shiftArg);
        }
        return;
    }
    shiftArg[1] = STATE_ARG_DEFAULT;
    switch (state) {
    case 0x3000:
    case 0x3100:
        func_8001D194(0, state, 0, 0, 0);
        return;
    case 0x1F00:
        if (*(s32 *)((u8 *)arg2 + 0x1040) != 0) {
            return;
        }
        break;
    case 0x900:
        if (*(s32 *)((u8 *)arg0 + 0xA8) & 1) {
            return;
        }
        break;
    case 0x4200:
        if (!(*(s32 *)((u8 *)arg0 + 0x64) < (D_8006CD48 - LEVEL_FLOOR_MARGIN))) {
            return;
        }
        *(u32 *)((u8 *)arg0 + 0x11C) = 0x2100;
        shiftArg[1] = D_8006CD48;
        break;
    default:
        break;
    }
    func_8001CC10(0, (s32)arg0, *(u32 *)((u8 *)arg0 + 0x11C), 1, &shiftArg[1]);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029884);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029D0C);

#ifdef NON_MATCHING
extern s32 D_80062AD8;

typedef struct Unk8002A1B8 {
    u8 pad0[0xA8];
    u32 flags;
    u8 padAC[0x100 - 0xAC];
    u32 f100;
    u8 pad104[0x11C - 0x104];
    s32 f11C;
} Unk8002A1B8;

s32 func_8002A1B8(Unk8002A1B8 *p, s32 arg1) {
    s32 a3;
    u32 t;
    s32 r;

    if (arg1 != 0) {
        a3 = ((u32)(arg1 & 0xE) >> 1) + 1;
    } else {
        a3 = 0;
    }
    switch ((u32)(arg1 & 0x3F0) >> 4) {
    case 1: p->f11C = 0x700; p->flags |= 0x400; break;
    case 2: p->f11C = 0xC00; p->flags |= 0x400; break;
    case 5: p->f11C = 0x1E00; p->flags |= 0x400; break;
    case 4:
        p->f11C = 0x1F00;
        p->flags |= 0x400;
        if ((p->f100 & 0x40000000) && a3 == 1) {
            p->f11C = 0x900;
            return 1;
        }
        break;
    case 3: p->f11C = 0x2100; p->flags |= 0x400; break;
    case 12: p->f11C = 0x2300; p->flags |= 0x400; break;
    case 11:
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    case 6: p->f11C = 0xD00; p->flags |= 0x400; break;
    case 13: p->f11C = 0x3300; p->flags |= 0x400; break;
    case 18: p->f11C = 0x4200; p->flags |= 0x400; break;
    case 14: p->f11C = 0x3500; p->flags |= 0x02000400; break;
    case 15: p->flags |= 0x04000000; break;
    case 7: case 8: case 9: case 10:
        p->f11C = 0x1200; p->flags |= 0x2000; break;
    case 27: p->f11C = 0x3000; p->flags |= 0x400; break;
    case 28: p->f11C = 0x3100; p->flags |= 0x400; break;
    case 29: case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 37: case 38:
        p->f11C = ((u32)(arg1 & 0x3F0) >> 4) + 0x40000; p->flags |= 0x400; break;
    default:
        break;
    }
    if (a3 == 4) {
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    } else if (a3 == 6) {
        p->f11C = 0x4000;
        p->flags |= 0x400;
        return 0;
    } else if (a3 == 5) {
        return 1;
    } else if (a3 == 3) {
        if (!(D_80062AD8 & 0x200)) {
            return 1;
        }
        r = 1;
        if ((p->f100 & 0x24000000) == 0x24000000) {
            r = 0;
        }
        return r;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A1B8);
#endif

#ifdef NON_MATCHING
#define UNK_FLAGS_OFFSET 0x100
#define UNK_FLAG_FIXED_HEIGHT 0x40000000
#define UNK_PTR_OFFSET 8
#define UNK_PTR_HEIGHT_OFFSET 0x40
#define UNK_FIXED_HEIGHT 0x29900
#define UNK_BOX_OFFSET 0x1020
#define UNK_OUT_OFFSET 0x1008
#define UNK_BOX_NEAR_NUMERATOR (-0x9600)
#define UNK_BOX_FAR_NUMERATOR 0x9600
#define UNK_BOX_MARGIN 0xC80
#define UNK_BOX_DROP 0x4B00
#define UNK_CALL_KEY 0xC4653601

void func_8002A3B0(void *arg0, void *arg1, void *arg2, void *arg3, s32 arg4, s32 arg5) {
    s32 *src = (s32 *)arg2;
    s32 *box = (s32 *)((char *)arg3 + UNK_BOX_OFFSET);
    s32 base;
    s32 diff;
    s32 near;
    s32 far;
    s32 y;

    base = src[1] + D_8005F204;
    diff = ((s32 *)arg1)[1] - src[1];
    if (diff > 0) {
        base += diff;
    }
    near = UNK_BOX_NEAR_NUMERATOR / arg4;
    far = UNK_BOX_FAR_NUMERATOR / arg4;
    box[0] = src[0] + near + UNK_BOX_MARGIN;
    box[1] = src[1] - UNK_BOX_DROP;
    box[2] = src[2] + near + UNK_BOX_MARGIN;
    box[3] = src[0] + far - UNK_BOX_MARGIN;
    if (!(*(s32 *)((char *)arg0 + UNK_FLAGS_OFFSET) & UNK_FLAG_FIXED_HEIGHT)) {
        y = src[1] + *(s32 *)(*(char **)((char *)arg0 + UNK_PTR_OFFSET) + UNK_PTR_HEIGHT_OFFSET);
    } else {
        y = src[1] + UNK_FIXED_HEIGHT;
    }
    box[4] = y;
    box[5] = src[2] + far - UNK_BOX_MARGIN;
    func_8003CD0C(arg0, arg3, (char *)arg3 + UNK_OUT_OFFSET, box, base, arg5, UNK_CALL_KEY, &func_8002A1B8);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A3B0);
#endif

typedef struct { s32 n; u8 pad[0x28C]; s32 *p[1]; } Unk8006CB6C;

#ifdef NON_MATCHING
#define NO_HIT 0xC4653601 /* func_8002A8D0's "nothing hit" result */
#define BOX_HALF_EXTENT 0x4B00 /* half-width of the query box in x and z */

extern Unk8006CB6C D_8006CB6C;

typedef struct {
    u8 pad0[0x4];
    s32 unk4;
} Unk8002A554_C10;

typedef struct {
    u8 pad0[0x10];
    Unk8002A554_C10 *unk10;
} Unk8002A554_C;

typedef struct {
    u8 pad0[0xC];
    Unk8002A554_C *unkC;
    u8 pad10[0x9C - 0x10];
    u8 unk9C[0xA8 - 0x9C];
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    u8 padB4[0x100 - 0xB4];
    s32 unk100;
} Unk8002A554_Obj;

typedef struct {
    s32 unk0;
    Unk8002A554_Obj *unk4;
    u8 unk8[1];
} Unk8002A554_Entry;

typedef struct {
    u8 pad0[0x40];
    s32 unk40;
    u8 pad44[0x4];
    Unk8002A554_Obj *unk48;
} Unk8002A554_8;

typedef struct {
    u8 pad0[0x8];
    Unk8002A554_8 *unk8;
    u8 padC[0x100 - 0xC];
    s32 unk100;
    u8 pad104[0x4];
    s32 unk108;
} Unk8002A554_Arg0;


s32 func_8002A554(Unk8002A554_Arg0 *arg0, s32 *arg1, s32 *arg2, u8 *arg3) {
    s32 ret;
    Unk8002A554_Obj *obj;
    s32 i;
    Unk8002A554_Entry *entry;
    Unk8002A554_Entry *last;
    s32 *box;
    s32 limit;
    s32 best;
    s32 diff;
    s32 y;
    s32 top;
    s32 lim;
    s32 res;
    s32 *data;

    if (D_8006CB6C.n == 0) {
        return NO_HIT;
    }
    ret = NO_HIT;
    best = NO_HIT;
    diff = arg1[1] - arg2[1];
    limit = arg2[1] + 0xF424;
    if (diff > 0) {
        limit += diff;
    }
    *(s32 *)(arg3 + 0x1020) = arg2[0] - BOX_HALF_EXTENT;
    *(s32 *)(arg3 + 0x1024) = arg2[1];
    *(s32 *)(arg3 + 0x1028) = arg2[2] - BOX_HALF_EXTENT;
    last = 0;
    *(s32 *)(arg3 + 0x102C) = arg2[0] + BOX_HALF_EXTENT;
    y = arg2[1];
    box = (s32 *)(arg3 + 0x1020);
    if (!(arg0->unk100 & 0x40000000)) {
        top = y + arg0->unk8->unk40;
    } else {
        top = y + 0x29900;
    }
    box[4] = top;
    box[5] = arg2[2] + BOX_HALF_EXTENT;
    *(s32 *)(arg3 + 0x104C) += 0x10;
    for (i = 0; i < D_8006CB6C.n; i++) {
        entry = (Unk8002A554_Entry *)D_8006CB6C.p[i];
        obj = entry->unk4;
        if ((obj->unkAC & 0x400000) == 1) {
            lim = NO_HIT;
        } else {
            lim = limit;
        }
        res = func_8002A8D0(entry, box, 1, 1, best, lim,
                            (obj->unkAC & 0x200) ? (s32)obj->unk9C : 0);
        if (res != NO_HIT) {
            if ((obj->unkAC & 0x20000) &&
                ((((!(arg0->unk100 & 0x10) && arg0->unk108 != 5) ||
                    ((obj->unkC->unk10->unk4 != 0x300 || (obj->unkB0 & 0x1012)) && !(obj->unk100 & 0x20))) &&
                   !(arg0->unk100 & 0x10000)) ||
                  (obj->unk100 & 0x10000))) {
                s32 flags = arg0->unk100;
                s32 mode = flags & 0xA00000;
                if (mode != 0x200000 || (obj->unkB0 & mode) ||
                    ((flags & 8) ? obj->unk100 < 0 : (obj->unk100 & 0x200))) {
                    obj->unkA8 |= 0x4000;
                    ret = res + 1;
                    best = res;
                    arg0->unk8->unk48 = obj;
                }
            }
            last = entry;
            *(s32 *)(arg3 + 0x104C) += 0x1000;
        }
    }
    if (last != 0) {
        data = 0;
        if (!(last->unk0 & 7)) {
            data = (s32 *)last->unk8;
        }
        func_8001A710(arg0, box, last->unk4, data);
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A554);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A8D0);

#ifdef NON_MATCHING
#define B164_FLD(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define B164_PTR(p, off) (*(void **)((u8 *)(p) + (off)))
#define B164_NO_HIT ((s32)0xC4653601)
#define B164_BOX 0x1020
#define B164_OUT 0x1008
#define B164_ACTIVE 0x1000
#define B164_FIXED_BASE_FLAG 0x40000000
#define B164_FIXED_HEIGHT 0x29900
#define B164_HEIGHT_OFFSET 0x40
#define B164_HALF_EXTENT 0x9600
#define B164_SKIP_FLAG 0x40
#define B164_FLAGS_INIT 0x40001
#define B164_FLAGS_DROP_BIT18 0xFFFBFFFF
#define B164_FLAGS_BASE 0x4001
#define B164_FLAG_SET 0x200000
#define B164_FLAG_LOW 1
#define B164_CLEAR_MASK 0xFFDBBFFE
#define B164_PARENT_FLAG 0x400000
#define B164_EXTRA_RANGE 0x19000
#define B164_FLAG_PARENT_4000 0x4000
#define B164_TIME_WINDOW 4

extern s32 D_8006CB64;

void func_8002B164(void *arg0, s32 *arg1, s32 *arg2, void *arg3, s32 *arg4) {
    s32 sp18[3];
    s32 *box;
    s32 flags;
    s32 extra;
    s32 hitA;
    s32 hitB;
    s32 res;
    s32 base;
    s32 limit;
    s32 y;
    s32 v;
    s32 w;
    void *p;
    void *q;

    box = (s32 *)((u8 *)arg3 + B164_BOX);
    B164_FLD(arg3, 0x1020) = arg2[0] - B164_HALF_EXTENT;
    B164_FLD(arg3, 0x1024) = arg2[1];
    B164_FLD(arg3, 0x1028) = arg2[2] - B164_HALF_EXTENT;
    extra = 0;
    flags = B164_FLAGS_INIT;
    B164_FLD(arg3, 0x102C) = arg2[0] + B164_HALF_EXTENT;
    y = arg2[1];
    if (!(B164_FLD(arg0, 0x100) & B164_FIXED_BASE_FLAG)) {
        v = y + B164_FLD(B164_PTR(arg0, 8), B164_HEIGHT_OFFSET);
    } else {
        v = y + B164_FIXED_HEIGHT;
    }
    box[4] = v;
    box[5] = arg2[2] + B164_HALF_EXTENT;
    if (B164_FLD(arg3, B164_ACTIVE) == 0 ||
        func_80030F58(box, (s32 *)((u8 *)arg3 + B164_OUT)) == 0) {
        func_80029538((s32)arg2, (s32)arg0, (s32)arg3);
    }
    func_8002A3B0(arg0, arg1, arg2, arg3, 8, (s32)sp18);
    hitB = sp18[1];
    hitA = sp18[2];
    if (B164_FLD(arg0, 0x100) & B164_SKIP_FLAG) {
        hitB = B164_NO_HIT;
        hitA = B164_NO_HIT;
    }
    if (hitA == B164_NO_HIT) {
        B164_FLD(arg3, 0x1040) = 0;
    } else {
        B164_FLD(arg3, 0x1040) = hitA;
    }
    if (hitB == B164_NO_HIT) {
        B164_FLD(arg3, 0x1044) = 0;
    } else {
        B164_FLD(arg3, 0x1044) = hitB;
    }
    res = func_8002A554(arg0, arg1, arg2, arg3);
    if (res == B164_NO_HIT) {
        B164_FLD(arg3, 0x1048) = 0;
    } else if (hitA == B164_NO_HIT || res >= hitA) {
        hitA = res;
        B164_FLD(arg3, 0x1048) = hitA;
        p = B164_PTR(arg0, 0x58);
        flags = B164_FLAGS_INIT & B164_FLAGS_DROP_BIT18;
        if (p != 0) {
            if (B164_FLD(p, 0xAC) & B164_PARENT_FLAG) {
                extra = B164_EXTRA_RANGE;
            }
            if ((u32)(B164_FLD(arg0, 0xDC) - B164_FLD(arg0, 0x110)) >= B164_TIME_WINDOW) {
                flags = B164_FLAGS_BASE;
            }
        }
        flags |= B164_FLAG_SET;
    } else {
        /* res is below hitA: nothing changes, flags stay at their initial value */
    }
    if ((hitA == B164_NO_HIT && hitB == hitA) || B164_FLD(arg0, 0x88) > 0) {
        if (B164_FLD(arg0, 0x88) > 0) {
            B164_FLD(arg0, 0xA8) &= B164_CLEAR_MASK;
        }
        B164_FLD(arg3, 0x1038) = 0;
    } else {
        base = hitA;
        if (hitA == B164_NO_HIT) {
            base = hitB;
            flags = B164_FLAG_LOW;
        }
        B164_FLD(arg3, 0x1038) = base;
        w = base + 1;
        if (hitA == B164_NO_HIT) {
            limit = B164_FLD(arg0, 0x64);
        } else {
            limit = B164_FLD(arg0, 0x64) + D_8005F204 + extra;
        }
        *arg4 = limit;
        if (limit < w) {
            w = arg1[1];
            box[1] = w;
        } else {
            y = arg2[1];
            if (w >= y) {
                if (flags & B164_FLAG_SET) {
                    q = B164_PTR(B164_PTR(arg0, 8), 0x48);
                    if (q != 0) {
                        B164_FLD(q, 0xA8) |= B164_FLAG_PARENT_4000;
                    }
                }
            } else {
                w = y;
            }
            box[1] = w;
        }
        if (!(B164_FLD(arg0, 0x100) & B164_FIXED_BASE_FLAG)) {
            v = w + B164_FLD(B164_PTR(arg0, 8), B164_HEIGHT_OFFSET);
        } else {
            v = w + B164_FIXED_HEIGHT;
        }
        box[4] = v;
        arg2[1] = w;
        if (B164_FLD(arg3, B164_ACTIVE) == 0 ||
            func_80030F58(box, (s32 *)((u8 *)arg3 + B164_OUT)) == 0) {
            func_80029538((s32)arg2, (s32)arg0, (s32)arg3);
        }
        y = B164_FLD(arg0, 0x88);
        if (y < 0 && (flags & B164_FLAG_LOW)) {
            B164_FLD(arg0, 0x114) = y;
            B164_FLD(arg0, 0x88) = 0;
        }
        B164_FLD(arg0, 0xA8) |= flags;
        B164_FLD(arg0, 0x110) = D_8006CB64;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B164);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B57C);

#ifdef NON_MATCHING
#define F(p, off) (*(s32 *)((s8 *)(p) + (off)))
#define SUB(p) (*(void **)((s8 *)(p) + 8))

void func_8002BC38(void *arg0, s32 *arg1, s32 *arg2, void *arg3) {
    s32 sp18;
    s32 ret;
    s32 y;
    s32 a;
    s32 b;
    s32 flag;
    void *sub;

    flag = 0;
    if (F(arg0, 0xA8) & 0x100000) {
        sub = SUB(arg0);
        if ((F(sub, 0x4C) & 0x100000) && (F(sub, 0x50) & 0x100000) && (F(sub, 0x54) & 0x100000)
            && (F(sub, 0x58) & 0x100000) && (F(sub, 0x5C) & 0x100000) && (F(sub, 0x60) & 0x100000)
            && (F(sub, 0x64) & 0x100000) && (F(sub, 0x68) & 0x100000)) {
            a = F(arg0, 0x100) & 0x40000000;
            flag = a == 0;
        }
    }
    ret = func_8002B57C(arg0, arg2, arg3, flag);
    if (ret == 0xC4653601) {
        F(arg3, 0x103C) = 0;
    } else {
        F(arg3, 0x103C) = ret;
    }
    if (ret == 0xC4653601) {
        goto fail;
    }
    b = arg2[1];
    if (!(F(arg0, 0x100) & 0x40000000)) {
        if (ret - 1 >= b + F(SUB(arg0), 0x40)) {
            goto fail;
        }
    } else if (ret - 1 >= b + 0x29900) {
        goto fail;
    }
    if (!(F(arg0, 0xA8) & 1)) {
        y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
        if (F(arg0, 0x100) & 0x40) {
            if (arg2[1] < y) {
                y = arg2[1];
            }
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
        } else {
            b = arg2[1];
            a = arg1[1];
            if (a < b) {
                if (y < a) {
                    y = a;
                }
            } else if (y < b) {
                y = b;
            }
            arg2[1] = y;
        }
        F(arg0, 0xA8) |= 0x80;
        if (F(arg0, 0x88) > 0) {
            F(arg0, 0x88) = 0;
        }
        return;
    }
fail:
    if (F(arg0, 0x100) & 0x40) {
        if (ret == 0xC4653601) {
            arg2[1] = arg1[1];
        } else {
            sp18 = 0;
            y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
            func_8001CC10(0, (s32)arg0, 0x3300, 1, &sp18);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002BC38);
#endif

extern void *D_800608CC;
extern void *D_8006759C;
extern s32 D_8006CB64;
extern s32 D_8006CD48;

s32 func_8002BF58(void *arg0, void *arg1) {
    s32 sp18;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_a2;
    u32 temp_v1;
    s8 *new_var;
    void *temp_a1;
    void *temp_s2;

    temp_v0 = func_80027DEC(D_800608CC);
    if (temp_v0 >= -0xFE) {
        temp_a1 = *(void **)((s8 *)arg0 + 0x18);
        temp_s2 = *(void **)((s8 *)temp_a1 + 0x14);
        if (*(s32 *)((s8 *)arg1 + 4) < ((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34))) {
            var_a0 = 1;
            new_var = (s8 *)arg1;
            if ((*(s32 *)((s8 *)*(void **)((s8 *)temp_a1 + 0x10) + 0x29C) & 2) && (*(s32 *)((s8 *)arg0 + 0x108) != 2)) {
                temp_v1 = *(u32 *)((s8 *)D_8006759C + 8);
                sp18 = 0x6400;
                var_a2 = 0x900;
                if (temp_v1 != 0x19) {
                    if (temp_v1 >= 0x19U) {
                        if (temp_v1 < 0x22U) {
                            if (temp_v1 >= 0x20U) {
                                goto block_8;
                            }
                        }
                    }
                } else {
block_8:
                    if (!(*(s32 *)((s8 *)*(void **)((s8 *)D_800608CC + 0x10) + 0x29C) & 0x800)) {
                        var_a2 = 0x2100;
                        sp18 = D_8006CD48;
                    }
                }
                func_8001CC10(0, (s32)arg0, var_a2, 1, &sp18);
                var_a0 = 0;
            }
            if (var_a0 != 0) {
                *(s32 *)(new_var + 4) = (s32)((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34));
                temp_v1_2 = *(s32 *)((s8 *)arg0 + 0x88);
                *(s32 *)((s8 *)(temp_s2 = arg0) + 0x88) = 0;
                *(s32 *)((s8 *)arg0 + 0x114) = temp_v1_2;
                *(s32 *)((s8 *)arg0 + 0xA8) = (s32)(*(s32 *)((s8 *)arg0 + 0xA8) | 1);
                *(s32 *)((s8 *)arg0 + 0x110) = (s32)D_8006CB64;
            }
        }
    } else {
        *(void **)((s8 *)arg0 + 0x18) = (void *)temp_v0;
    }
    return -0xFF;
}

void func_8002C0F0(void *arg0, void *arg1, s32 *arg2, s32 *arg3, s32 *arg4) {
    s32 pos[4];
    s32 delta[4];
    s32 sp40;
    s32 sp44;
    u32 tmp;
    s32 sp48;
    s32 temp_s2;
    s32 temp_s3;
    s32 x;
    s32 y;
    s32 d;
    s32 i;

    for (i = 0; i < 3; i++) {
        delta[i] = arg3[i];
    }
    for (i = 0; i < 3; i++) {
        pos[i] = arg2[i] + delta[i];
    }
    func_8002B164(arg0, arg2, pos, arg1, &sp40);
    func_8002DE08(arg2, pos, arg0, arg1);
    x = (pos[0] - arg2[0]) * 4;
    if (x < 0) {
        x += 0x1FFF;
    }
    y = (pos[2] - arg2[2]) * 4;
    temp_s2 = (x >> 13) + 0x10;
    if (y < 0) {
        y += 0x1FFF;
    }
    temp_s3 = (y >> 13) + 0x10;
    if (func_8002E280(arg2, temp_s2, temp_s3, &sp44, &sp48, arg0, 0) != 0) {
        pos[0] = ((sp44 - 0x10) << 13 >> 2) + arg2[0];
        pos[2] = ((sp48 - 0x10) << 13 >> 2) + (tmp = arg2[2]);
    } else {
        pos[0] = arg2[0];
        pos[2] = arg2[2];
    }
    *(s32 *)((u8 *)arg1 + 0x104C) += 1;
    if ((temp_s2 != 0x10 || temp_s3 != temp_s2) && temp_s2 == sp44 && temp_s3 == sp48) {
        *(s32 *)((u8 *)arg0 + 0xA8) |= 0x100;
    }
    func_8002BC38(arg0, arg2, pos, arg1);
    func_8002BF58(arg0, pos);
    for (i = 0; i < 3; i++) {
        tmp = pos[i];
        arg4[i] = tmp;
    }
}

#ifdef NON_MATCHING
extern u8 *D_8005F38C;

#define F(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define SLOT_STRIDE 0x28
#define SLOT_BASE 0x18
#define POOL_NEXT_OFFSET (-4)
#define POOL_LIMIT_OFFSET 0x1410
#define POOL_MAX_ENTRIES 0x60
#define BOX_SIZE_PLAIN 0x20
#define BOX_SIZE_XFORM 0x44
#define SCALE_NUM 0xC8
#define SCALE_SHIFT 4
#define ANGLE_MASK 0xFFF
#define IDENTITY_SCALE 0x1000
#define FLAG_OFFSET_ON 0x200
#define OBJ_FLAG_SHIFT 0x20000000
#define SHIFT_NEG 0xFFFF5FD8
#define SHIFT_POS 0xA028

/* Builds a bounding box in the pool; pos/in/ext are arrays to keep the frame layout of the retail code. */
s32 *func_8002C324(void *arg0, s32 arg1, s32 arg2) {
    s32 pos[3];
    s32 in[3];
    s32 ext[9];
    u8 *obj = (u8 *)arg0;
    u8 *slot = 0;
    u8 *entry = 0;
    s32 idx = arg2;
    s32 kind;
    s32 *box;
    s32 ang;
    s32 a, b, c, d;
    s32 v;
    u32 next;
    u8 *flagp;

    if (idx == -2) {
        flagp = (u8 *)F(obj, 0xE8);
        if (flagp != 0 && *flagp == 1) {
            idx = 0;
        }
    }
    if (idx < 0) {
        kind = 0;
    } else {
        slot = (u8 *)F((u8 *)F(obj, 8), 0x20);
        if ((u32)idx >= (u32)F(slot, 0xC)) {
            return 0;
        }
        entry = slot + idx * SLOT_STRIDE + SLOT_BASE;
        kind = *(s32 *)entry & 7;
    }
    if (D_8006CB6C.n >= POOL_MAX_ENTRIES) {
        func_8002CA1C();
    }
    do {
        box = *(s32 **)((u8 *)&D_8006CB6C + POOL_NEXT_OFFSET);
        next = (u32)box + BOX_SIZE_PLAIN;
        if (kind != 0) {
            if (kind == 1) {
                next = (u32)box + BOX_SIZE_XFORM;
            } else {
                kind = 0;
            }
        }
    } while (next >= (u32)((u8 *)&D_8006CB6C + POOL_LIMIT_OFFSET));
    *(u32 *)((u8 *)&D_8006CB6C + POOL_NEXT_OFFSET) = next;
    D_8006CB6C.p[D_8006CB6C.n] = box;
    D_8006CB6C.n += 1;
    box[1] = (s32)arg0;
    box[0] = kind;

    if (slot == 0) {
        a = F(obj, 0x78);
        if (a < 0) a = -a;
        box[2] = F(obj, 0x60) - ((a * SCALE_NUM) >> SCALE_SHIFT);
        a = F(obj, 0x78);
        if (a < 0) a = -a;
        box[5] = F(obj, 0x60) + ((a * SCALE_NUM) >> SCALE_SHIFT);
        box[3] = F(obj, 0x64) - ((F(obj, 0x7C) * SCALE_NUM) >> SCALE_SHIFT);
        box[6] = F(obj, 0x64) + ((F(obj, 0x7C) * SCALE_NUM) >> SCALE_SHIFT);
        box[4] = F(obj, 0x68) - ((F(obj, 0x80) * SCALE_NUM) >> SCALE_SHIFT);
        box[7] = F(obj, 0x68) + ((F(obj, 0x80) * SCALE_NUM) >> SCALE_SHIFT);
        return box;
    }
    if (kind != 0) {
        if (kind == 1) {
            u8 *src = slot + idx * SLOT_STRIDE;
            box[2] = F(src, 0x28);
            box[3] = F(src, 0x2C);
            box[4] = F(src, 0x30);
            box[5] = F(src, 0x34);
            box[6] = F(src, 0x38);
            box[7] = F(src, 0x3C);
            box[8] = F(obj, 0x60);
            box[9] = F(obj, 0x64);
            box[10] = F(obj, 0x68);
            func_800309A8(box + 11, box + 2, (s16 *)F(obj, 8), box + 8);
            return box;
        }
        return box;
    }
    v = F(obj, 0x70);
    ang = v & ANGLE_MASK;
    if (ang == 0 && v == 0 && F(obj, 0x6C) == 0 && F(obj, 0x78) == IDENTITY_SCALE) {
        pos[0] = F(obj, 0x60) + F(entry, 4);
        pos[1] = F(obj, 0x64) + F(entry, 8);
        pos[2] = F(obj, 0x68) + F(entry, 0xC);
    } else {
        in[0] = F(entry, 4);
        in[1] = F(entry, 8);
        in[2] = F(entry, 0xC);
        func_8001E8E4(in, (s32 *)(obj + 0x60), (s32 *)(obj + 0x6C), (s32)(obj + 0x78), pos);
    }
    a = F(obj, 0x78);
    if (a < 0) a = -a;
    ext[6] = a;
    b = F(obj, 0x7C);
    ext[7] = b;
    c = F(obj, 0x80);
    ext[8] = c;
    d = (F(entry, 0x10) >> 8) * a >> SCALE_SHIFT;
    ext[0] = d;
    ext[1] = (F(entry, 0x14) >> 8) * b >> SCALE_SHIFT;
    ext[2] = (F(entry, 0x18) >> 8) * c >> SCALE_SHIFT;
    ext[3] = (F(entry, 0x1C) >> 8) * a >> SCALE_SHIFT;
    ext[4] = (F(entry, 0x20) >> 8) * b >> SCALE_SHIFT;
    ext[5] = (F(entry, 0x24) >> 8) * c >> SCALE_SHIFT;
    if ((u32)(ang - 0x200) >= 0xC01U) {
        box[2] = pos[0] + ext[0];
        box[3] = pos[1] + ext[1];
        box[4] = pos[2] + ext[2];
        box[5] = pos[0] + ext[3];
        box[6] = pos[1] + ext[4];
        v = pos[2] + ext[5];
    } else if (ang < 0x600) {
        box[2] = pos[0] + ext[2];
        box[5] = pos[0] + ext[5];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] - ext[3];
        v = pos[2] - ext[0];
    } else if (ang < 0xA00) {
        box[2] = pos[0] + ext[0];
        box[5] = pos[0] + ext[3];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] - ext[5];
        v = pos[2] - ext[2];
    } else {
        box[2] = pos[0] - ext[5];
        box[5] = pos[0] - ext[2];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] + ext[0];
        v = pos[2] + ext[3];
    }
    box[7] = v;
    if ((D_80062AD8 & FLAG_OFFSET_ON) && (F(D_8005F38C, 0x100) & OBJ_FLAG_SHIFT)) {
        box[2] += SHIFT_NEG;
        box[4] += SHIFT_NEG;
        box[5] += SHIFT_POS;
        box[7] += SHIFT_POS;
    }
    return box;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002C324);
#endif

extern char D_8006CB68[];

extern Unk8006CB6C D_8006CB6C;

void func_8002CA1C(void) {
    char *v;
    if (D_8006CB6C.n != 0) {
        D_8006CB6C.n -= 1;
        v = *(char **)D_8006CB68;
        switch (*D_8006CB6C.p[D_8006CB6C.n] & 7) {
        case 0:
            v -= 0x20;
            break;
        case 1:
            v -= 0x44;
            break;
        }
        *(char **)D_8006CB68 = v;
    }
}

void func_8002CA98(void) {
    *(char **)D_8006CB68 = D_8006CB68 + 0x414;
    D_8006CB6C.n = 0;
}

extern s32 D_8005BC18[];
s32 D_8005F378;
s32 *D_8005F3C4;
s32 D_8005F3E8;

s32 func_8002CAB8(void) {
    s32 i;
    s32 *dst;
    s32 *src;

    D_8005F3C4 = (s32 *)0x1F800100;
    D_8005F378 = 0x1F800180;
    D_8005F3E8 = 0x1F800380;
    for (i = 0, dst = (s32 *)0x1F800380, src = D_8005BC18; i < 0x20; i++) {
        *dst++ = *src++;
    }
    return -0xFF;
}

s32 func_8002CB14(void) {
    return SUCCESS;
}

void func_8002CB1C(s32 arg0, s32 arg1) {
    if (((arg0 < 0) ? (-arg0) : (arg0)) < 0x20) {
        if (((arg1 < 0) ? (-arg1) : (arg1)) < 0x20) {
            if (arg0 || arg1) {
                func_8003CBE4();
            } else {
                func_8003CBE4();
            }
        }
    }
}

void func_8002CB68(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 *p;
    s32 x;
    s32 y;
    s32 end;
    s32 mask;

    arg2 += 0x10;
    mask = -1;
    if (arg2 < 0x20) {
        if (arg2 <= 0) {
            return;
        }
        mask <<= 0x20 - arg2;
    }
    arg0 += 0xF;
    if (arg0 >= 0x20) {
        return;
    }
    if (arg0 >= 0) {
        mask &= ~((s32)0x80000000 >> arg0);
    }
    if (mask == 0) {
        return;
    }
    x = arg1 + 0x10;
    if (x < 0) {
        x = 0;
    }
    end = arg3 + 0x10;
    if (end >= 0x21) {
        end = 0x20;
    }
    if (x >= 0x20) {
        return;
    }
    p = D_8005F3C4 + x;
    if (arg4 != 0) {
        if (x < end) {
            do {
                x++;
                *p |= mask;
                p++;
            } while (x < end);
        }
    } else {
        mask = ~mask;
        while (x < end) {
            x++;
            *p &= mask;
            p++;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CC50);

#ifdef NON_MATCHING
#define FIELD(p, type, off) (*(type *)((s8 *)(p) + (off)))

#define FLAG_PLAYER_NO_CHECK 0x100000
#define FLAG_SELF_SKIP 0x10000000
#define BUCKET_COUNT 0x20
#define BUCKET_BIAS 0x10
#define BUCKET_SHIFT 13
#define BUCKET_ROUND 0x1FFF
#define BUCKET_STEP 8
#define MARGIN_NEAR 0x7FBC
#define MARGIN_FAR 0x6400
#define SIGN_PROBE 0x20000000
#define FLAG_A 0x10
#define FLAG_B 0x10000
#define FLAG_C 0x20
#define KIND_5 5
#define KIND_300 0x300
#define MASK_1012 0x1012
#define FLAG_4000 0x4000
#define KIND_E 0xE
#define SHAPE_MASK 7


/* (x * 4) / 0x2000 rounded toward zero, as the retail code does it. */
#define SHAPE(obj) FIELD(FIELD(obj, void *, 0xC), s32 *, 0x10)
#define Cell(x) ((((x) * 4) + (((x) & SIGN_PROBE) ? BUCKET_ROUND : 0)) >> BUCKET_SHIFT)

void func_8002D6E4(s32 *arg0, s32 *arg1, void *arg2, void *arg3, s32 arg4) {
    s32 box[6];
    s32 wide[6];
    s32 rect[6];
    s32 *base;
    s32 kind;
    s32 originX;
    s32 originZ;
    s32 checkSelf;
    s32 selfObj;
    u32 row;
    u32 bit;
    s32 seen;
    s32 i;
    s32 t;
    s32 lo;
    s32 *entry;
    s32 *fp;
    void *obj;
    s32 x;
    s32 y;
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;
    s32 v;
    s32 m;

    checkSelf = 0;
    originX = arg0[0];
    originZ = arg0[2];
    if (!(FIELD(FIELD(D_800608CC, void *, 0x10), s32, 0x29C) & FLAG_PLAYER_NO_CHECK)) {
        checkSelf = (FIELD(arg2, s32, 0x100) & FLAG_SELF_SKIP) == 0;
    }
    if (D_8006CB6C.n == 0) {
        return;
    }
    selfObj = FIELD(arg2, s32, 0x58);
    row = Cell(arg1[2] - originZ) + BUCKET_BIAS;
    bit = Cell(arg1[0] - originX) + BUCKET_BIAS;
    if (bit < BUCKET_COUNT && row < BUCKET_COUNT) {
        bit = 0x80000000U >> bit;
        seen = D_8005F3C4[row] & bit;
    } else {
        bit = 0;
        row = 0;
        seen = 0;
    }
    base = FIELD(arg2, s32 *, 8);
    rect[0] = arg0[0] + base[0x30 / 4];
    rect[1] = arg0[1] + base[0x34 / 4];
    rect[2] = arg0[2] + base[0x38 / 4];
    rect[3] = arg0[0] + base[0x3C / 4];
    rect[4] = arg0[1] + base[0x40 / 4];
    rect[5] = arg0[2] + base[0x44 / 4];

    t = arg0[0] - MARGIN_NEAR;
    lo = rect[0];
    if (t < lo) {
        lo = t;
    }
    wide[0] = lo - MARGIN_FAR;
    wide[1] = rect[1];
    lo = rect[2];
    t = arg0[2] - MARGIN_NEAR;
    if (t < lo) {
        lo = t;
    }
    wide[2] = lo - MARGIN_FAR;
    t = arg0[0] + MARGIN_NEAR;
    lo = rect[5];
    if (lo < t) {
        lo = t;
    }
    wide[3] = lo + MARGIN_FAR;
    wide[4] = rect[4];
    t = arg0[2] + MARGIN_NEAR;
    if (rect[5] < t) {
        wide[5] = t + MARGIN_FAR;
    } else {
        wide[5] = rect[5] + MARGIN_FAR;
    }

    FIELD(arg3, s32, 0x104C) += 0x100;

    for (i = 0; i < D_8006CB6C.n; i++) {
        entry = D_8006CB6C.p[i];
        obj = (void *)entry[1];
        kind = entry[0] & SHAPE_MASK;
        if (kind == 0) {
            fp = entry + 8 / 4;
        } else {
            fp = entry + 0x2C / 4;
        }
        if (((s32)obj != selfObj || arg4 == 0 || wide[1] < fp[4]) && func_80030EC0(wide, fp) != 0) {
            if (checkSelf != 0 && (FIELD(obj, s32, 0xAC) & FLAG_B)
                && ((((!(FIELD(arg2, s32, 0x100) & FLAG_A) && FIELD(arg2, s32, 0x108) != KIND_5)
                      || ((SHAPE(obj)[1] != KIND_300 || (FIELD(obj, s32, 0xB0) & MASK_1012))
                          && !(FIELD(obj, s32, 0x100) & FLAG_C)))
                     && !(FIELD(arg2, s32, 0x100) & FLAG_B))
                    || (FIELD(obj, s32, 0x100) & FLAG_B))
                && !(FIELD(obj, s32, 0xA8) & FLAG_4000)
                && (SHAPE(obj)[0] != KIND_E || wide[1] < fp[1])) {
                D_8005F3C4[row] &= ~bit;
                if (kind == 0) {
                    m = FIELD(obj, s32, 0x128);
                    x0 = Cell(entry[2] + m - originX);
                    y0 = Cell(entry[4] + m - originZ);
                    x1 = Cell(entry[5] - m - originX);
                    y1 = Cell(entry[7] - m - originZ);
                    func_8002CB68(x0, y0, x1, y1, 1);
                    if (arg4 != 0) {
                        for (x = x0; x < x1; x += BUCKET_STEP) {
                            func_8002CB1C(x, y0);
                        }
                        x = x1;
                        for (y = y0; y < y1; y += BUCKET_STEP) {
                            func_8002CB1C(x, y);
                        }
                        y = y1;
                        for (x = x1; x0 < x; x -= BUCKET_STEP) {
                            func_8002CB1C(x, y);
                        }
                        for (y = y1; y0 < y; y -= BUCKET_STEP) {
                            func_8002CB1C(x0, y);
                        }
                    }
                } else if (kind == 1) {
                    func_8002CC50(entry, wide, arg0, arg4);
                } else {
                    /* other shapes only run the common tail */
                }
                if (arg4 != 0) {
                    v = D_8005F3C4[row] & bit;
                    seen |= v;
                    if (v != 0) {
                        if (kind == 0) {
                            func_8001A710(arg2, rect, (void *)entry[1], fp);
                        } else {
                            func_8001A710(arg2, rect, (void *)entry[1], 0);
                        }
                    }
                }
            }
            if (kind == 0 && arg4 != 0) {
                if (func_80030EC0(rect, entry + 2) != 0) {
                    func_8001A710(arg2, rect, (void *)entry[1], entry + 2);
                }
            }
        }
    }
    D_8005F3C4[row] |= seen;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002D6E4);
#endif

#ifdef NON_MATCHING
#define WORLD_FLAGS_OFFSET 0x29C
#define WORLD_FLAG_SKIP 0x100000
#define OBJ_FLAGS_OFFSET 0x100
#define OBJ_FLAG_SKIP 0x10000000
#define OBJ_FLAG_FIXED_BASE 0x40000000
#define OBJ_MODE_OFFSET 0x108
#define OBJ_MODE_MIN 3
#define OBJ_MODE_SPAN 2
#define OBJ_MODE_ALT 2
#define OBJ_BITS_OFFSET 0xB0
#define OBJ_BITS_MASK 2
#define OBJ_ALT_VALUE 2
#define OBJ_X_OFFSET 0x64
#define OBJ_X_END_DELTA 0x3200
#define OBJ_PARENT_OFFSET 0x8
#define PARENT_BASE_OFFSET 0x40
#define FIXED_BASE_OFFSET 0x29900
#define BUF_B_OFFSET 0x1008
#define TABLE_A_LAST 0x7C
#define TABLE_A_WORDS 32
#define TABLE_B_LAST 0x1FC
#define TABLE_B_WORDS 128

extern s32 D_8005F204;
extern s32 D_8005F378;
extern s32 *D_8005F3C4;


void func_8002DE08(s32 *arg0, s32 *arg1, void *arg2, void *arg3) {
    s32 *p;
    s32 i;
    s32 mode;
    s32 flags;
    s32 base;
    s32 x;

    if (!(*(s32 *)(*(s8 **)((s8 *)D_800608CC + 0x10) + WORLD_FLAGS_OFFSET) & WORLD_FLAG_SKIP)) {
        if (!(*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_SKIP)) {
            p = (s32 *)((s8 *)D_8005F3C4 + TABLE_A_LAST);
            for (i = TABLE_A_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            p = (s32 *)(D_8005F378 + TABLE_B_LAST);
            for (i = TABLE_B_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            mode = *(s32 *)((s8 *)arg2 + OBJ_MODE_OFFSET);
            flags = *(s32 *)((s8 *)arg2 + OBJ_BITS_OFFSET);
            if ((u32)(mode - OBJ_MODE_MIN) < OBJ_MODE_SPAN) {
                flags = OBJ_ALT_VALUE;
            } else if (mode == OBJ_MODE_ALT) {
                flags = OBJ_ALT_VALUE;
            } else {
                flags = flags & OBJ_BITS_MASK;
            }
            base = arg0[1];
            if (*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_FIXED_BASE) {
                base += FIXED_BASE_OFFSET;
            } else {
                base += *(s32 *)(*(s8 **)((s8 *)arg2 + OBJ_PARENT_OFFSET) + PARENT_BASE_OFFSET);
            }
            x = *(s32 *)((s8 *)arg2 + OBJ_X_OFFSET);
            func_8003C88C(arg3, (s8 *)arg3 + BUF_B_OFFSET, flags, x + D_8005F204,
                          x + OBJ_X_END_DELTA, base, arg0[0], arg0[2]);
        }
    }
    func_8002D6E4(arg0, arg1, arg2, arg3, 1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DE08);
#endif

#ifdef NON_MATCHING
extern s32 *D_8005F3C4;

#define BIT_COUNT 32
#define TOP_BIT 0x80000000U

s32 func_8002DF68(u32 arg0, u32 arg1) {
    s32 result;

    result = 0;
    if ((arg0 < BIT_COUNT) && (arg1 < BIT_COUNT)) {
        result = (D_8005F3C4[arg1] & (TOP_BIT >> arg0)) == 0;
    } else {
        /* out of range: result stays 0 */
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DF68);
#endif

#ifdef NON_MATCHING
typedef struct {
    u16 w[4];
} Ent;

#define TBL_COUNT_OFFSET 0x1004
#define TBL_X_OFFSET 0x1008
#define TBL_Y_OFFSET 0x100C
#define TBL_Z_OFFSET 0x1010
#define TBL_FIELD(off) (*(s32 *)(D_80067FE4 + (off)))

/* Multiplies by 4 with 32-bit wrap-around, then divides by 8192 truncating. */
#define SCALE_SHIFT 2
#define SCALE_DIVISOR 8192
#define SCALE(x) (((s32)((u32)(x) << SCALE_SHIFT)) / SCALE_DIVISOR)

extern u8 D_80067FE4[];
extern s32 D_80068FE8;

s32 func_8002DFA8(s32 arg0, s32 arg1, s32 *arg2, void *arg3) {
    s32 a[3];
    s32 b[3];
    s32 d[3];
    s32 sp40;
    u16 s1, fp, s7, s6;
    s32 i;
    s32 cnt;
    u32 t;
    s32 kind;
    Ent *e;
    u16 m1, m2;
    s32 v;

    sp40 = arg0;
    i = 0;
    cnt = 0;
    if (D_80068FE8 > 0) {
        do {
            e = (Ent *)D_80067FE4 + i;
            if (e->w[0] == 0) {
                d[0] = e->w[1] << 8;
                d[1] = e->w[2] << 8;
                i++;
                d[2] = e->w[3] << 8;
                e = (Ent *)D_80067FE4 + i;
                fp = e->w[1];
                s7 = e->w[2];
                s6 = e->w[3];
            } else {
                t = (e->w[0] >> 2) | 1;
                if (t != 0) {
                    kind = ((t & 0xE) >> 1) + 1;
                } else {
                    kind = 0;
                }
                if (kind != 5 && kind != 0) {
                    switch (arg1) {
                    case 0:
                        if (kind != 2 && kind != 6) {
                            goto next;
                        }
                        break;
                    case 1:
                        if (kind == 2 || kind == 6) {
                            goto next;
                        }
                        break;
                    default:
                        break;
                    }
                    m1 = s7;
                    a[1] = TBL_FIELD(TBL_Y_OFFSET) + (s16)e->w[2] * 16 - arg2[1];
                    if (s1 < m1) {
                        m1 = s1;
                    }
                    v = a[1] + (d[1] >> m1);
                    b[1] = v;
                    switch (arg1) {
                    case 0:
                        if (a[1] > 0x6400) goto next;
                        if (v < (s32)0xFFFE7000) goto next;
                        break;
                    case 1:
                        if (v < 0) goto next;
                        break;
                    default:
                        break;
                    }
                    a[0] = TBL_FIELD(TBL_X_OFFSET) + (s16)e->w[1] * 16 - arg2[0];
                    a[2] = TBL_FIELD(TBL_Z_OFFSET) + (s16)e->w[3] * 16 - arg2[2];
                    s1 = e->w[0] & 7;
                    m1 = fp;
                    if (s1 < m1) m1 = s1;
                    m2 = s6;
                    b[0] = a[0] + (d[0] >> m1);
                    if (s1 < m2) m2 = s1;
                    b[2] = a[2] + (d[2] >> m2);
                    func_8002CB68(SCALE(a[0]), SCALE(a[2]), SCALE(b[0]), SCALE(b[2]), sp40);
                    cnt++;
                }
            }
next:
            i++;
        } while (i < TBL_FIELD(TBL_COUNT_OFFSET));
    }
    return cnt;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DFA8);
#endif

#ifdef NON_MATCHING
#define FIELD(base, type, off) (*(type *)((u8 *)(base) + (off)))

#define SEARCH_RANGE 0x98
#define SLOT_COUNT 0x20
#define CENTER_COORD 0x10
#define DEPTH_LIMIT 0x100
#define FLAG_BLOCKED 0x100000
#define FLAG_RETRY 0x8000
#define CTX_FLAGS_OFFSET 0x29C
#define OBJ_NO_SEARCH 0x10000000
#define STATE_PTR_OFFSET 0x58
#define STATE_KIND_VALUE 0x22
#define EMPTY_SLOT -1

extern s8 D_8005B520[];
extern u8 D_80067FE4[];

/* Nonzero when arg5's state object holds the special kind. */
static s32 has_special_state(void *obj) {
    void *state = FIELD(obj, void *, STATE_PTR_OFFSET);

    if (state == 0) {
        return 0;
    } else if (*FIELD(FIELD(state, void *, 0xC), s32 *, 0x10) == STATE_KIND_VALUE) {
        return 1;
    } else {
        return 0;
    }
}

s32 func_8002E280(s32 *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 *arg4, void *arg5, s32 arg6) {
    s32 x = arg1;
    s32 y = arg2;
    s32 depth = arg6;
    s32 i;
    s32 dx;
    s32 dy;
    s32 px;
    s32 py;
    s32 ny;
    s32 *slot;
    s8 *step;

    for (;;) {
        step = D_8005B520;
        if (func_8002DF68(x, y) != 0
            || (FIELD(FIELD(D_800608CC, void *, 0x10), s32, CTX_FLAGS_OFFSET) & FLAG_BLOCKED)
            || (FIELD(arg5, s32, 0x100) & OBJ_NO_SEARCH)) {
            *arg3 = x;
            *arg4 = y;
            return depth + 2;
        }

        for (i = 0; i < SEARCH_RANGE; i++) {
            dx = step[0];
            dy = step[1];
            step += 2;

            px = x + dx;
            py = y + dy;
            if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
            ny = y - dy;
            if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
            px = x - dx;
            if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
            if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }

            if (dx != dy) {
                px = x + dy;
                py = y + dx;
                if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
                ny = y - dx;
                if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
                px = x - dy;
                if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
                if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
            }
        }

        /* Nothing found: retry from the centre when any slot is in use. */
        if (!(x == CENTER_COORD && y == x) && !has_special_state(arg5)) {
            slot = D_8005F3C4;
            for (i = 0; i < SLOT_COUNT; i++) {
                if (slot[i] != EMPTY_SLOT) {
                    break;
                }
            }
            if (i < SLOT_COUNT) {
                x = CENTER_COORD;
                y = CENTER_COORD;
                depth++;
                continue;
            }
        }

        if (depth < DEPTH_LIMIT && !has_special_state(arg5)) {
            FIELD(arg5, s32, 0xA8) |= FLAG_RETRY;
            if (func_8002DFA8(0, 0, arg0, arg5) != 0) {
                func_8002DFA8(1, 1, arg0, arg5);
                func_8002D6E4(arg0, arg0, arg5, D_80067FE4, 0);
            }
            return func_8002E280(arg0, x, y, arg3, arg4, arg5, DEPTH_LIMIT);
        }

        FIELD(arg5, s32, 0xA8) |= FLAG_BLOCKED;
        *arg3 = CENTER_COORD;
        *arg4 = CENTER_COORD;
        return 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E280);
#endif

#ifdef NON_MATCHING
typedef struct Unk8002E628 {
    s32 lo;
    s32 hi;
    s32 *data;
} Unk8002E628;

s32 *func_8002E628(s32 *arg0, s32 arg1, s32 arg2, Unk8002E628 *arg3) {
    s32 q;

    if (arg3->data != 0) {
        do {
            if (arg1 >= arg3->lo && arg3->hi >= arg1) {
                q = (arg1 - arg3->lo) / arg2;
                *arg0 = arg3->lo + q * arg2;
                return arg3->data + (q >> 8);
            }
            arg3++;
        } while (arg3->data != 0);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E628);
#endif

#ifdef NON_MATCHING
void func_8002E6E0(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 dx, dz, pz, px, len;

    dx = (arg3 - arg2) >> 8;
    dz = (arg5 - arg4) >> 8;
    pz = (arg1[2] - arg4) >> 8;
    px = (arg1[0] - arg2) >> 8;
    len = func_80041A78(dx * dx + dz * dz);
    dz = (dz << 12) / len;
    dx = (dx << 12) / len;
    pz = (pz * dz - px * dx) >> 12;
    arg0[0] = arg2 - ((pz * dx) >> 16);
    arg0[1] = arg1[1];
    arg0[2] = ((pz * dz) >> 16) + arg4;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E6E0);
#endif

void func_8002E854(s32 *arg0, s32 *arg1, s32 *arg2) {
    s32 b = arg2[0];
    s32 a = *arg1;

    arg0[0] = a - (b - a);
    arg0[1] = arg2[1];
    arg0[2] = arg2[2];
}
