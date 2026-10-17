#include "common.h"
#include "sdk.h"
#include "game.h"

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
extern s32 D_8005FC84;
extern s32 D_8006CB94;
extern s32 D_8006CD2C;

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
