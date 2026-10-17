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

void Snd_Init(void) {
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

s16 D_8005F2E8;

void func_800325D0(s32 arg0) {
    D_8005F2E8 = arg0;
}

extern void SpuSetReverbDepth(Unk80032588 *);

s32 Snd_SetReverb(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s32 arg4) {
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
    a = Math_Cos(ratan2(y, x)) + 0x1000;
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

#ifdef NON_MATCHING
typedef struct {
    s32 mask;
    s32 pad4;
    s16 x;
    s16 y;
    u8 pad[0x28];
} Out;
extern u8 D_8005FC9D;
extern void func_800521A8(Out *);

s32 func_800328D4(void) {
    Out o;
    Ent *p;
    s32 i;
    s32 j;
    s32 ret;
    s32 min;
    s32 ax;
    s32 v;
    s32 dist;

    i = D_8005F304;
    dist = 0x3FFF;
    min = 0xFF;
    p = &D_8005F4EC[i];
    for (; i < 0x18; i++, p++) {
        if (p->b < min) {
            min = p->b;
        }
        if (!(p->flags & 8)) {
            return i;
        }
    }
    ret = -1;
    if (D_8005FC9D >= min) {
        j = D_8005F304;
        p = &D_8005F4EC[j];
        for (; j < 0x18; j++, p++) {
            if (p->b == min) {
                o.mask = 1 << j;
                func_800521A8(&o);
                ax = o.x;
                if (ax < 0) {
                    ax = -ax;
                }
                if (o.y >= 0) {
                    if (o.y < ax) {
                        v = o.x;
                    } else {
                        v = o.y;
                    }
                } else if (-o.y < ax) {
                    v = o.x;
                } else {
                    v = o.y;
                }
                if (v < 0) {
                    v = -v;
                }
                if (v < dist) {
                    ret = j;
                    dist = v;
                }
            }
        }
        if (min == D_8005FC9D && dist >= 0x800 && Rand_Below(0x64) >= 0x32) {
            ret = -1;
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800328D4);
#endif

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

#ifdef NON_MATCHING
#define FLAG_SPU_IDLE_MASK 0x8010
#define FLAG_ACTIVE 8
#define FLAG_FADE 0x40
#define FLAG_REVERB 0x400
#define FLAG_PITCH_CLAMP 0x200
#define FLAG_VOICE_MASK 0xFFC0FFFF
#define VOICE_MASK 0x3F
#define VOICE_SHIFT 16
#define CTRL_REQUIRED 0x1000
#define OBJ_FLAGS_REQUIRED 0x100000
#define VOL_MAX 0x3FFF
#define CMD_MASK 0xFF93
#define BAD_RESULT (-0xE)
#define TEMPLATE_FLAGS_KEEP 0x7000
#define TEMPLATE_FLAGS_SET 0x8000

extern s32 D_8005F2E0;
extern s16 D_8005F2E8;
extern s32 D_8005FC84;
extern s32 D_8006CB94;
extern s32 D_8006CD2C;
void SpuSetReverbVoice(s32, s32);
void func_800518B8(s32 *);

#define OBJ_W(o, off) (*(s32 *)((u8 *)(o) + (off)))
#define ENT_W(e, off) (*(s32 *)((u8 *)(e) + (off)))
#define ENT_H(e, off) (*(s16 *)((u8 *)(e) + (off)))

s32 func_80032B7C(struct Obj *arg0, s32 arg1, s32 arg2) {
    s32 cmd[0x14];
    s32 pos[3];
    Ent *e;
    s32 *src;
    s32 *dst;
    s32 voice;
    u32 res;
    s32 v;
    s16 vol;
    s32 i;

    if (D_8006CBF8 == 0) {
        return 0;
    }
    if (!(D_8006CB94 & CTRL_REQUIRED) && !(OBJ_W(arg0, 0x100) & OBJ_FLAGS_REQUIRED)) {
        return 0;
    }
    voice = func_800328D4();
    if (voice <= 0) {
        return -1;
    }
    e = &D_8005F4EC[voice];
    if (arg1 & 1) {
        res = arg1 & ~1;
    } else {
        res = func_80032A98(&D_8005FC84, (s32 *)arg1);
        if (res == BAD_RESULT) {
            return -1;
        }
        if (D_8005FC84 & TEMPLATE_FLAGS_KEEP) {
            D_8005FC84 |= TEMPLATE_FLAGS_SET;
        }
    }
    src = &D_8005FC84;
    dst = (s32 *)((u8 *)e + 4);
    for (i = 0; i < 4; i++) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
    }
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    ENT_H(e, 0x1E) = (arg2 * D_8005F2E8) >> 14;
    if (e->flags & FLAG_FADE) {
        ENT_W(e, 0x44) = (ENT_H(e, 0x3C) - ENT_H(e, 0x1E)) / ENT_W(e, 0x40);
    }
    e->key = arg0;
    if (arg0 != 0) {
        ENT_W(e, 0x30) = OBJ_W(arg0, 0x60);
        ENT_W(e, 0x34) = OBJ_W(arg0, 0x64);
        ENT_W(e, 0x38) = OBJ_W(arg0, 0x68);
        func_8001EC2C((s32 *)((u8 *)arg0 + 0x60), pos, 1);
        ENT_W(e, 0x24) = pos[0];
        ENT_W(e, 0x28) = pos[1];
        ENT_W(e, 0x2C) = pos[2];
    }
    func_800323AC();
    e->flags = (e->flags & FLAG_VOICE_MASK) | ((voice & VOICE_MASK) << VOICE_SHIFT);
    cmd[1] = CMD_MASK;
    cmd[0] = 1 << voice;
    if ((u32)(*((u8 *)e->key + 0x120) - 6) < 2U) {
        vol = ENT_H(e, 0x1E);
        if (vol >= VOL_MAX) {
            vol = VOL_MAX;
        } else if (vol < 0) {
            vol = 0;
        } else {
            /* in range */
        }
        ((s16 *)cmd)[5] = vol;
        ((s16 *)cmd)[4] = vol;
        e->flags &= ~FLAG_PITCH_CLAMP;
    } else {
        u8 *p = (u8 *)func_8003264C((s32 *)((u8 *)e + 0x24), ENT_H(e, 0x1E));
        /* the table entry may be unaligned: read it bytewise */
        ((s16 *)cmd)[5] = p[2] | (p[3] << 8);
        ((s16 *)cmd)[4] = p[0] | (p[1] << 8);
    }
    ((s16 *)cmd)[10] = ((s32)(*(u16 *)((u8 *)e + 0x22)) * D_8006CD2C) / 4096;
    cmd[9] = 1;
    cmd[10] = 1;
    cmd[11] = 3;
    cmd[7] = res;
    ((s16 *)cmd)[24] = 0;
    ((s16 *)cmd)[25] = 0;
    ((s16 *)cmd)[26] = 0;
    ((s16 *)cmd)[27] = 0;
    ((s16 *)cmd)[28] = 0xF;
    func_800518B8(cmd);
    SpuSetReverbVoice((e->flags >> 10) & 1, cmd[0]);
    if (!(e->flags & FLAG_SPU_IDLE_MASK)) {
        func_80051078(1, cmd[0]);
    }
    v = D_8005F2E0 + 1;
    e->a = v;
    D_8005F2E0 = v;
    e->flags |= FLAG_ACTIVE;
    return e->a;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032B7C);
#endif

#ifdef NON_MATCHING
#define LOOP_ENTRIES 0x18
#define OP_MASK 0x0FFFFFFF
#define OP_COUNT 0xF
#define ARG_SET_FLAG1 0x40000000
#define ARG_SET_FLAG2 0x20000000
#define OBJ_FLAGS_OFFSET 0x100
#define OBJ_FLAG_ACTIVE 0x100000
#define GATE_FLAG_ACTIVE 0x1000
#define ENT_FLAG_USED 0x8
#define ST_F_SET0 0x1
#define ST_F_SET1 0x2
#define ST_F_PENDING 0x4
#define ST_F_HAS_1C 0x10
#define ST_F_RAMP_A 0x40
#define ST_F_RAMP_B 0x80
#define ST_F_200 0x200
#define ST_F_400 0x400
#define ST_F_800 0x800
#define QUERY_MAGIC 0x100
#define QUERY_FLAGS_MASK 0x8010
#define KEY_EMPTY 0x6396347F
#define DIRTY_VEC 3
#define DIRTY_1E 0x10
#define SHIFT_TO_800 3
#define SHIFT_TO_400 2
#define SIGN_SHIFT 31

extern s32 D_8005F244;
extern s32 D_8005F304;

#define FIXED_ONE 4096

typedef struct Unk80032F18Raw4 {
    s32 w;
} __attribute__((packed)) Unk80032F18Raw4;

/* Argument block of func_800518B8, built on the stack. */
typedef struct Unk80032F18Update {
    s32 mask;
    s32 dirty;
    s32 data;
    s32 unk_C;
    s32 unk_10;
    s16 scale;
} Unk80032F18Update;

typedef struct Unk80032F18State {
    s32 flags;
    s32 unk_4;
    s32 key;
    s32 unk_C;
    s32 unk_10;
    s32 unk_14;
    s8 unk_18;
    s8 unk_19;
    s16 unk_1A;
    s16 unk_1C;
    u16 unk_1E;
    s32 vec[3];
    s32 unk_2C[3];
    s16 unk_38;
    u16 unk_3A;
    s32 unk_3C;
    s32 unk_40;
    s32 unk_44;
    s32 unk_48;
} Unk80032F18State;

/* arg0: entry id, 0 = shared state, -1 = entry owned by arg3, -2 = all entries. */
s32 func_80032F18(s32 arg0, s32 arg1, s32 *arg2, struct Obj *arg3) {
    Unk80032F18Update upd;
    s32 slot;
    u32 op;
    Ent *ent;
    Unk80032F18State *st;
    s32 tmp[3];
    volatile s32 peek[3];
    volatile Unk80032F18Raw4 *rawp;

    if (!(D_8006CB94 & GATE_FLAG_ACTIVE) && !(*(s32 *)((u8 *)arg3 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_ACTIVE)) {
        return 0;
    }
    slot = D_8005F304;
    ent = &D_8005F4EC[slot];
    op = arg1 & OP_MASK;
    for (; slot < LOOP_ENTRIES; slot++, ent++) {
        if (arg0 != 0 && (u32)(arg0 + 2) >= 2 && ent->a != arg0) {
            continue;
        }
        if (arg0 == -1 && ent->key != arg3) {
            continue;
        }
        if (arg0 == 0) {
            st = (Unk80032F18State *)&D_8005FC84;
        } else if (ent->flags & ENT_FLAG_USED) {
            st = (Unk80032F18State *)&ent->flags;
        } else {
            continue;
        }
        if (arg1 < 0) {
            st->flags |= ST_F_SET0;
        }
        if (arg1 & ARG_SET_FLAG1) {
            st->flags |= ST_F_SET1;
        }
        if (arg1 & ARG_SET_FLAG2) {
            st->flags |= ST_F_PENDING;
        }
        upd.dirty = 0;
        if (op < OP_COUNT) {
            switch (op) {
            case 0:
                if (st->flags & ST_F_PENDING) {
                    st->unk_38 = arg2[0];
                    st->unk_3C = D_8005F244;
                    if (arg0 != 0) {
                        st->unk_40 = (st->unk_38 - st->unk_1A) / D_8005F244;
                    }
                    st->flags |= ST_F_RAMP_A;
                } else {
                    st->unk_1A = arg2[0];
                    if (arg0 != 0) {
                        upd.dirty |= DIRTY_VEC;
                        rawp = (volatile Unk80032F18Raw4 *)func_8003264C(st->vec, st->unk_1A);
                        upd.data = rawp->w;
                    }
                }
                break;
            case 1:
                if (st->flags & ST_F_PENDING) {
                    st->unk_3A = arg2[0];
                    st->unk_44 = D_8005F244;
                    st->flags |= ST_F_RAMP_B;
                    st->unk_48 = (st->unk_3A - st->unk_1E) / D_8005F244;
                } else {
                    st->unk_1E = arg2[0];
                    if (arg0 != 0) {
                        upd.dirty = DIRTY_1E;
                        upd.scale = (st->unk_1E * D_8006CD2C) / FIXED_ONE;
                    }
                }
                break;
            case 2:
                tmp[0] = arg2[0];
                tmp[1] = arg2[1];
                tmp[2] = arg2[2];
                func_8001EC2C(tmp, arg2, 1);
                /* fall through */
            case 3:
                /* The vector is read even when it is not stored. */
                peek[0] = arg2[0];
                peek[1] = arg2[1];
                peek[2] = arg2[2];
                if (!(st->flags & ST_F_PENDING)) {
                    st->vec[0] = arg2[0];
                    st->vec[1] = arg2[1];
                    st->vec[2] = arg2[2];
                    if (arg0 != 0) {
                        upd.dirty = DIRTY_VEC;
                        rawp = (volatile Unk80032F18Raw4 *)func_8003264C(st->vec, st->unk_1A);
                        upd.data = rawp->w;
                    }
                }
                break;
            case 4:
                st->unk_18 = arg2[0];
                break;
            case 5:
                st->key = arg2[0];
                break;
            case 6:
                D_8005F244 = arg2[0];
                if (arg2[0] == 0) {
                    D_8005F244 = 1;
                }
                break;
            case 7:
                st->unk_1C = arg2[0];
                st->flags |= ST_F_HAS_1C;
                break;
            case 8:
                st->flags |= ST_F_200;
                break;
            case 9:
                st->flags &= ~ST_F_200;
                break;
            case 10:
                st->flags = (st->flags & ~ST_F_800) | ((arg2[0] << SHIFT_TO_800) & ST_F_800);
                break;
            case 11:
                st->flags = (st->flags & ~ST_F_400) | ((arg2[0] << SHIFT_TO_400) & ST_F_400);
                break;
            case 12:
                st->unk_19 = arg2[0];
                break;
            case 13:
                if (arg2 == 0) {
                    return st->unk_10 != KEY_EMPTY;
                }
                st->unk_10 = arg2[0];
                break;
            case 14:
                if (arg2 != 0 && arg2[0] == QUERY_MAGIC) {
                    return (st->flags & QUERY_FLAGS_MASK) == 0;
                }
                break;
            default:
                break;
            }
        }
        if (upd.dirty != 0) {
            upd.mask = 1 << slot;
            func_800518B8(&upd.mask);
        }
        if (arg0 >= 0) {
            return 0;
        }
    }
    return ~arg0 >> SIGN_SHIFT;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032F18);
#endif

void func_80013E10(void *arg0);
void func_80051078(s32 arg0, s32 arg1);

void func_80033448(void *arg0) {
    do { func_80051078(0, 1 << ((u16 *)arg0)[3]); } while (0);
    ((s32 *)arg0)[1] &= ~8;
    if (((void **)arg0)[6] != 0) {
        func_80013E10(((void **)arg0)[6]);
    }
}

typedef struct Unk80033DE0 {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk80033DE0;

#ifdef NON_MATCHING
#define MIX_SCALE_MUL 0x3FFF00
#define MIX_SCALE_SHIFT 16
#define MIX_VOL_MAX 0x3FFF
#define MIX_MVOL_SHIFT 7
#define MIX_FADE_IN_STEP 0x667
#define MIX_FADE_OUT_STEP (-0x222)
#define MIX_SEQ_MIN_MODE 2
#define MIX_SEQ_PLAYING 3
#define MIX_CMD_FADE_OUT 2
#define MIX_CMD_FADE_IN 3
#define MIX_VOICES 0x18
#define MIX_PITCH_DIV 4096
#define MIX_NO_RESOURCE 0x6396347F

#define EF_STOP 1
#define EF_STOP_IDLE 2
#define EF_ACTIVE 8
#define EF_STARTED 0x10
#define EF_VOL_RAMP 0x40
#define EF_PITCH_RAMP 0x80
#define EF_FOLLOW_OBJ 0x200
#define EF_SRC_SHIFT 12
#define EF_SRC_MASK 7
#define EF_SRC_BITS 0x7000
#define EF_SRC_QUEUED 0x2000
#define EF_BUSY 0x8000
#define SRC_QUEUE 1
#define SRC_QUEUED 2
#define SRC_RESOURCE 3

#define HDL_TAG_MASK 0xC0000002
#define HDL_ADDR_SHIFT 4
#define HDL_ADDR_PRE_SHIFT 2
#define RES_STATE_NEW 1
#define RES_STATE_USED 0x14
#define RES_STATE_USED_END 2
#define RES_STATE_WAIT 0xA
#define OBJ_TYPE_OFF 0x120
#define OBJ_TYPE_FIRST 6
#define OBJ_TYPE_COUNT 2
#define OBJ_POS_OFF 0x60

#define CMD_VOICE 0
#define CMD_MASK_IDX 1
#define CMD_ADDR_IDX 7
#define CMD_VOL_L 4
#define CMD_VOL_R 5
#define CMD_PITCH 10
#define CMD_WORDS 8
#define CMD_M_ADDR 0x80
#define CMD_M_VOL 3
#define CMD_M_PITCH 0x10

typedef struct Unk800334A0 {
    s32 unk_0;
    s32 flags;
    s32 unk_8;
    void *obj;
    u8 handle[4];
    s32 tag;
    u32 *res;
    u8 repeat;
    u8 unk_1D;
    u16 vol;
    u16 timer;
    u16 pitch;
    s32 vec[3];
    s32 prev[3];
    s32 unk_3C;
    s32 volCount;
    s32 volStep;
    s32 pitchCount;
    s32 pitchStep;
} Unk800334A0;

typedef struct Unk800334A0Res {
    s32 unk_0;
    s16 state;
    s16 unk_6;
    u8 unk_8[7];
    u8 refs;
    s32 unk_10;
    s32 unk_14;
    s32 unk_18;
    u32 addr;
} Unk800334A0Res;


extern void SsSetMVol(s32, s32);
extern void func_8005182C(u8 *);
extern void func_800518B8(s32 *);
extern s32 D_8005F18C;
extern u8 D_8005FC6C[];
extern void *D_800608CC;
extern s32 D_80067FB0;
extern s32 D_8005F304;
extern s32 D_8006CBFC;
extern s32 D_8005F25C;
extern s32 D_8005F258;
extern s16 D_8005F2F8;
extern s16 D_8005F2FC;
extern s16 D_8005F300;
extern s32 D_8005F308;
extern s32 D_8005F39C;
extern s16 D_8005FCE4;

void func_800334A0(void) {
    s32 cmd[CMD_WORDS + 8];
    s32 cmd2[CMD_WORDS + 8];
    s32 vec[3];
    s32 i;
    s32 busy;
    s32 mask;
    s32 flags;
    s32 voiceBit;
    s32 vol;
    u32 addr;
    u8 *status;
    u8 *p;
    Unk800334A0 *e;
    Unk800334A0Res *res;
    void *q;

    busy = 0;
    if (D_8006CBF8 != D_8005F25C) {
        D_8005F25C = D_8006CBF8;
        func_800325D0((D_8006CBF8 * MIX_SCALE_MUL) >> MIX_SCALE_SHIFT);
    }
    if (D_8006CBFC != D_8005F258) {
        D_8005F258 = D_8006CBFC;
        func_80033C84((D_8006CBFC * MIX_SCALE_MUL) >> MIX_SCALE_SHIFT);
    }
    if (D_800608CC != 0) {
        func_80034254(*(s32 *)((u8 *)D_800608CC + 0x10) + 0x2A4);
    }
    if (D_8005F300 != 0) {
        if (D_8005F300 < 0 && D_8005F2FC < -D_8005F300) {
            D_8005F2FC = 0;
            D_8005F300 = 0;
        }
        if (D_8005F300 > 0 && (MIX_VOL_MAX - D_8005F2FC) < D_8005F300) {
            D_8005F2FC = MIX_VOL_MAX;
            D_8005F300 = 0;
        }
        D_8005F2FC = (u16)D_8005F2FC + (u16)D_8005F300;
        vol = ((s32)((u32)(u16)D_8005F2FC << 16)) >> (16 + MIX_MVOL_SHIFT);
        SsSetMVol(vol, vol);
    }
    if (D_8005F18C == 1) {
        D_8005F300 = MIX_FADE_OUT_STEP;
        if (D_8005F308 != 0 && D_8005F2F8 >= MIX_SEQ_MIN_MODE && D_8005F39C == MIX_SEQ_PLAYING) {
            Snd_ControlSequence((Unk80033DE0 *)&D_8005FCE4, MIX_CMD_FADE_OUT, 0);
        }
    }
    if (D_8005F18C == -1) {
        D_8005F300 = MIX_FADE_IN_STEP;
        if (D_8005F308 != 0 && D_8005F2F8 >= MIX_SEQ_MIN_MODE && D_8005F39C == MIX_SEQ_PLAYING) {
            Snd_ControlSequence((Unk80033DE0 *)&D_8005FCE4, MIX_CMD_FADE_IN, 0);
        }
    }
    func_8005182C(D_8005FC6C);
    i = D_8005F304;
    if (i >= MIX_VOICES) {
        return;
    }
    e = (Unk800334A0 *)&D_8005F4EC[i];
    status = &D_8005FC6C[i];
    for (; i < MIX_VOICES; i++, e++, status++) {
        if (!(e->flags & EF_ACTIVE)) {
            continue;
        }
        voiceBit = 1 << i;
retry:
        switch ((e->flags >> EF_SRC_SHIFT) & EF_SRC_MASK) {
        case SRC_RESOURCE:
            addr = ((Unk800334A0Res *)e->res)->addr;
            goto start_voice;
        case SRC_QUEUE:
            if (func_800143C0(e->handle, 0) == 0) {
                break;
            }
            vol = (s32)func_80014260(e->handle, 1, 0);
            if ((vol & HDL_TAG_MASK) == HDL_TAG_MASK) {
                addr = ((u32)vol << HDL_ADDR_PRE_SHIFT) >> HDL_ADDR_SHIFT;
                goto start_voice;
            }
            e->flags = (e->flags & ~EF_SRC_BITS) | EF_SRC_QUEUED;
            /* fall through */
        case SRC_QUEUED:
            q = func_80014260(e->handle, 1, 0);
            res = (Unk800334A0Res *)func_8001557C(((s32 *)q)[1]);
            if (res != 0) {
                if (res->state == RES_STATE_NEW) {
                    res->state = RES_STATE_USED;
                } else if ((u32)((res->state - RES_STATE_USED) & 0xFFFF) >= RES_STATE_USED_END) {
                    break;
                } else {
                    /* already in use */
                }
                addr = res->addr;
                res->refs = res->refs + 1;
                e->res = (u32 *)res;
                func_80013E10(func_80014148((u32)q));
                goto start_voice;
            } else {
                Unk800334A0Res *r2 = (Unk800334A0Res *)func_80014148((u32)q);
                if ((u32)((u16)r2->state - RES_STATE_USED) < RES_STATE_USED_END && r2->unk_6 == 1) {
                    r2->state = RES_STATE_WAIT;
                    r2->unk_18 = ((s32 *)q)[1];
                    D_80067FB0 = D_80067FB0 + 1;
                }
            }
            break;
        default:
            break;
        }
        goto after;
start_voice:
        cmd[CMD_ADDR_IDX] = addr;
        cmd[CMD_MASK_IDX] = CMD_M_ADDR;
        cmd[CMD_VOICE] = voiceBit;
        func_800518B8(cmd);
        flags = e->flags & ~EF_SRC_BITS;
        e->flags = flags & ~EF_BUSY;
        if (!(flags & EF_STARTED) || e->timer == 0) {
            e->flags = (flags & ~EF_BUSY) | EF_STARTED;
            e->timer = 1;
        }
after:
        flags = e->flags;
        if (flags & EF_BUSY) {
            continue;
        }
        if (flags & EF_STARTED) {
            e->timer = e->timer - 1;
            if (e->timer != 0) {
                continue;
            }
            e->flags = e->flags & ~EF_STARTED;
            func_80051078(1, voiceBit);
            *status = 1;
        }
        if (*status == MIX_SEQ_PLAYING) {
            if (e->repeat >= 2) {
                e->repeat = e->repeat - 1;
                func_80051078(1, voiceBit);
                *status = 1;
            } else if (e->tag != MIX_NO_RESOURCE) {
                if (e->res != 0) {
                    func_80013E10(e->res);
                }
                addr = func_80032A98((s32 *)&e->flags, (s32 *)&e->tag);
                e->tag = MIX_NO_RESOURCE;
                if (addr == 0) {
                    goto retry;
                }
                cmd2[CMD_ADDR_IDX] = addr;
                cmd2[CMD_MASK_IDX] = CMD_M_ADDR;
                cmd2[CMD_VOICE] = voiceBit;
                func_800518B8(cmd2);
                func_80051078(1, voiceBit);
                *status = 1;
                goto after;
            } else {
                func_80033448(e);
                goto tail;
            }
        }
        mask = 0;
        if (e->flags & EF_VOL_RAMP) {
            e->vol = e->vol + e->volStep;
            e->volCount = e->volCount - 1;
            if (e->volCount > 0) {
                busy = 1;
            } else {
                e->flags = e->flags & ~EF_VOL_RAMP;
            }
            mask = mask | CMD_M_VOL;
            if ((u32)(*((u8 *)e->obj + OBJ_TYPE_OFF) - OBJ_TYPE_FIRST) < OBJ_TYPE_COUNT) {
                vol = (s16)e->vol;
                if (vol >= MIX_VOL_MAX) {
                    vol = MIX_VOL_MAX;
                } else if (vol < 0) {
                    vol = 0;
                } else {
                    /* in range */
                }
                ((s16 *)cmd)[CMD_VOL_R] = vol;
                ((s16 *)cmd)[CMD_VOL_L] = vol;
            } else {
                p = (u8 *)func_8003264C((s32 *)e->vec, (s16)e->vol);
                ((s16 *)cmd)[CMD_VOL_R] = p[2] | (p[3] << 8);
                ((s16 *)cmd)[CMD_VOL_L] = p[0] | (p[1] << 8);
            }
        }
        if (e->flags & EF_PITCH_RAMP) {
            e->pitch = e->pitch + e->pitchStep;
            e->pitchCount = e->pitchCount - 1;
            if (e->pitchCount > 0) {
                busy = 1;
            } else {
                e->flags = e->flags & ~EF_PITCH_RAMP;
            }
            mask = mask | CMD_M_PITCH;
            ((s16 *)cmd)[CMD_PITCH] = (s32)(e->pitch * D_8006CD2C) / MIX_PITCH_DIV;
        }
        if ((e->flags & EF_FOLLOW_OBJ) && e->obj != 0) {
            e->prev[0] = ((s32 *)e->obj)[OBJ_POS_OFF / 4];
            e->prev[1] = ((s32 *)e->obj)[OBJ_POS_OFF / 4 + 1];
            e->prev[2] = ((s32 *)e->obj)[OBJ_POS_OFF / 4 + 2];
            func_8001EC2C(&((s32 *)e->obj)[OBJ_POS_OFF / 4], vec, 1);
            e->vec[0] = vec[0];
            e->vec[1] = vec[1];
            e->vec[2] = vec[2];
            mask = mask | CMD_M_VOL;
            p = (u8 *)func_8003264C((s32 *)e->vec, (s16)e->vol);
            ((s16 *)cmd)[CMD_VOL_R] = p[2] | (p[3] << 8);
            ((s16 *)cmd)[CMD_VOL_L] = p[0] | (p[1] << 8);
        }
        cmd[CMD_MASK_IDX] = mask;
        if (mask != 0) {
            cmd[CMD_VOICE] = voiceBit;
            func_800518B8(cmd);
        }
tail:
        if ((e->flags & EF_STOP) || (busy == 0 && (e->flags & EF_STOP_IDLE))) {
            func_80033448(e);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800334A0);
#endif

#ifdef NON_MATCHING
extern u16 D_8005F2EC;
extern s16 D_8005F2F8;
extern s32 D_8005F39C;
extern s16 D_8005F3E4;
extern Unk80033DE0 D_8005FCD0[];

void func_80056760(s16, s16, s16, s16);

#define SEQ_MODE_ACTIVE 3
#define VOLUME_SHIFT 7

void func_80033C84(s32 arg0) {
    s32 i;
    s32 vol;

    D_8005F2EC = arg0;
    if (D_8005F39C == SEQ_MODE_ACTIVE) {
        vol = ((s16)arg0) >> VOLUME_SHIFT;
        for (i = 0; i < D_8005F2F8; i++) {
            func_80056760(D_8005F3E4, D_8005FCD0[i].seq, vol, vol);
        }
    } else {
        /* other modes: only the store above */
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033C84);
#endif

extern s32 D_8006CBFC;
extern s32 D_8005F3EC;
s32 D_8005F2F4;
s32 D_8005F39C;
s16 D_8005F3E4;
s16 D_8005F3E0;

void Snd_InitSequencer(void) {
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

/* Added to the master volume (D_8005F2FC, at most 0x3FFF) on each update of func_800334A0, until it reaches 0 */
#define VOLUME_FADE_OUT_STEP -0x2AA

void func_80033DD0(void) {
    D_8005F300 = VOLUME_FADE_OUT_STEP;
}

s32 Snd_ControlSequence(Unk80033DE0 *, s32, s32);
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
                Snd_ControlSequence(p, 1, 0);
                Snd_ControlSequence(p - 1, 3, 0);
                D_8005F308 = 0;
            } else {
                if (D_8006CBFC != 0 || D_8006CBF8 != 0) {
                    D_8005F308 = 0x3FFF;
                } else {
                    D_8005F308 = 0;
                }
                p = &D_8005FCD0[0];
                Snd_ControlSequence(p, 2, 0);
                Snd_ControlSequence(p + 1, 0, 1);
            }
            func_80056760(D_8005F3E4, D_8005FCE4, D_8005F308, D_8005F308);
        }
    }
}

#ifdef NON_MATCHING
typedef struct {
    s32 pad0;
    s32 unk4;
    s32 pad8;
    s32 padC;
    s32 *unk10;
    s32 pad14;
    s32 unk18;
} Unk80033EE0;

void SsSepClose(s16);
s16 SsSepOpen(s32, s16, s16);
void SsStart(void);
s16 SsVabFakeBody(s16);
s16 SsVabFakeHead(s32, s16, s32);
void exit(s32);
typedef struct Unk800343CC {
    s16 unk_0;
    s16 pad2;
    s32 unk_4;
    s32 pad8[2];
    s16 unk_10;
    s16 unk_12;
} Unk800343CC;

void func_800343CC(Unk800343CC *, s32, s32);
void func_800528C4(s16, s16, s32, s32);
u16 D_8005F2EC;
extern s32 D_80067FB8;

#define NONE 0x6396347F

s32 Snd_LoadSequence(s32 addr) {
    s32 *arg0 = (s32 *)addr;
    s32 tmp;
    s32 fresh;
    Unk80033EE0 *hdr;
    s32 i;
    s32 v;
    s32 vab;

    fresh = ~D_8005F3E0 == 0;
    if (D_8005F308 != 0) {
        return -1;
    }
    if (D_8005F39C == 1) {
        if (D_8005F260++ < 3) {
            return 0;
        }
        D_8005F260 = 0;
        if (D_8005F2F4 != NONE) {
            SsSepClose(D_8005F3E4);
            tmp = D_8005F2F4;
            func_800143C0(&tmp, 1);
        }
        if (*arg0 != NONE) {
            hdr = func_80014260(arg0, 1, 1);
            v = hdr->unk4;
            D_8005F2F4 = v;
            for (i = 0; i < 7; i++) {
                if (hdr->unk10[i + 2] != NONE) {
                    func_80014260(&hdr->unk10[i + 2], 0, 1);
                }
            }
        } else {
            D_8005F2F4 = *arg0;
            return 0;
        }
    }
    if (*arg0 == NONE) {
        D_8005F2F4 = *arg0;
        D_8005F39C = 0;
        return 0;
    }
    D_8005F39C = 2;
    hdr = func_80014260(arg0, 1, 0);
    D_8005F254 = 0;
    for (i = 0; i < 7; i++) {
        if (hdr->unk10[i + 2] != NONE) {
            if (func_800143C0(&hdr->unk10[i + 2], 0) == 0) {
                return 0;
            }
        }
    }
    D_8005F2F8 = hdr->unk10[0];
    vab = D_8005F3E0;
    D_8005F39C = 3;
    if (vab == -1) {
        D_8005F3E0 = SsVabFakeHead(((s32 *)func_80014260(&hdr->unk10[1], 1, 1))[5], -1, D_80067FB8);
        if (D_8005F3E0 == vab) {
            exit(-1);
        }
        D_8005F3E0 = SsVabFakeBody(D_8005F3E0);
        if (D_8005F3E0 == vab) {
            exit(-1);
        }
        SsStart();
    }
    D_8005F3E4 = SsSepOpen(hdr->unk18, D_8005F3E0, D_8005F2F8);
    if (D_8005F3E4 == -1) {
        exit(-1);
    }
    for (i = 0; i < D_8005F2F8; i++) {
        if (fresh) {
            v = (s16)D_8005F2EC >> 7;
            func_800343CC((Unk800343CC *)&D_8005FCD0[i], (v << 16) | v, i);
        } else {
            func_800343CC((Unk800343CC *)&D_8005FCD0[i], 0, i);
            v = (s16)D_8005F2EC >> 7;
            func_800528C4(D_8005F3E4, D_8005FCD0[i].seq, v, 0x1E);
        }
    }
    Snd_ControlSequence(D_8005FCD0, 0, 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", Snd_LoadSequence);
#endif

void func_80034238(void *arg0, s32 arg1, void *arg2, void *arg3) {
    if (D_8005F39C == 0) {
        D_8005F39C = 1;
    }
}

#ifdef NON_MATCHING
#define STATE_IDLE 0
#define STATE_ACTIVE 1
#define STATE_PENDING 3
#define STATE_WAITING 4
#define WAIT_FRAMES 30
#define LOW_BIT_MASK 1
#define U16_TO_S32_SHIFT 16
#define FIELD_SHIFT 23
#define NO_ENTRY -1

void func_80052B4C(s16, s16, s32, s32);

extern u16 D_8005F2EC;
extern s16 D_8005F250;
extern s32 D_8005F254;
extern s32 D_8005F308;
extern s32 D_8005F2F4;
extern s32 D_8005F39C;
extern s16 D_8005F3E4;
extern s16 D_8005F3E0;

void func_80034254(s32 arg0) {
    s32 word;
    s32 key;

    if (D_8005F39C == STATE_IDLE) {
        return;
    }
    if (D_8005F39C == STATE_WAITING) {
        if (D_8005F250 == 0) {
            D_8005F39C = STATE_ACTIVE;
        } else {
            D_8005F250 = D_8005F250 - 1;
            if (D_8005F250 == 0) {
                D_8005F39C = STATE_ACTIVE;
            } else {
                /* still waiting */
            }
        }
    } else {
        /* state unchanged */
    }
    if (D_8005F39C == STATE_PENDING) {
        word = *(s32 *)arg0;
        if (word & LOW_BIT_MASK) {
            key = word;
        } else {
            key = *(s32 *)(word + 4);
        }
        if (D_8005F2F4 != key && D_8005F308 == 0 && D_8005F3E0 != NO_ENTRY && D_8005F254 == 0) {
            func_80052B4C(D_8005F3E4, D_8005FCD0->seq,
                          (s32)(D_8005F2EC << U16_TO_S32_SHIFT) >> FIELD_SHIFT, WAIT_FRAMES);
            D_8005F250 = WAIT_FRAMES;
            D_8005F254 = arg0;
            D_8005F39C = STATE_WAITING;
        } else {
            /* nothing to start */
        }
    } else if (D_8005F39C < STATE_PENDING && D_8005F39C > 0) {
        Snd_LoadSequence(arg0);
    } else {
        /* no per-frame update in this state */
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80034254);
#endif

void Snd_CloseSequence(s16 a, s32 b);

void Snd_ShutdownSequencer(void) {
    if (D_8005F24C != 0) {
        Snd_CloseSequence(D_8005F3E4, 1);
        SsEnd();
        D_8005F24C = 0;
    }
}

#ifdef NON_MATCHING
#define SOUND_REVERB_THRESHOLD 2
#define SEQ_FORCE_FLAT_VOLUME 1
#define SEQ_NO_NEXT (-1)
#define HALF_SHIFT 16


extern s16 D_8005F2F8;
extern s32 D_8005F308;
extern s16 D_8005F3E4;

void func_800343CC(Unk800343CC *arg0, s32 arg1, s32 arg2) {
    if (D_8005F2F8 >= SOUND_REVERB_THRESHOLD && arg2 == SEQ_FORCE_FLAT_VOLUME) {
        arg0->unk_12 = D_8005F308;
        arg0->unk_10 = D_8005F308;
    } else {
        arg0->unk_10 = arg1 >> HALF_SHIFT;
        arg0->unk_12 = arg1;
    }
    arg0->unk_0 = arg2;
    arg0->unk_4 = SEQ_NO_NEXT;
    func_80056760(D_8005F3E4, arg2, arg0->unk_10, arg0->unk_12);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800343CC);
#endif

void SsSepClose(s16);
void SsVabClose(s16);

void Snd_CloseSequence(s16 a, s32 b) {
    if (a != -1) {
        SsSepClose(a);
    }
    if ((b != 0) && (D_8005F3E0 != -1)) {
        SsVabClose(D_8005F3E0);
    }
}

/* Returns nothing, yet declared s32: as void, GCC fills the delay slot before the jump table otherwise. */
s32 Snd_ControlSequence(Unk80033DE0 *obj, s32 cmd, s32 vol) {
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
