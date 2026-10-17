#include "common.h"
#include "sdk.h"
#include "game.h"

#define NODE_TYPE_SHIFTED 7
#define NODE_SHIFT_SHIFTED 2
#define PATH_KIND 0x4B
#define PATH_ARG_SHIFT 2
#define FRAC_BITS 8
#define FRAC_ONE 0x100
#define FRAC_MAX 0xFF
#define FRAC_MASK 0xFF
#define STEP_SHIFT 4
#define ERR_NO_PATH -0xE
#define FLAG_AC_FACE 0x4
#define FLAG_AC_MIRROR 0x2
#define FLAG_AC_PITCH 0x800
#define FLAG_AC_SEGMENT 0x8000
#define FLAG_A8_EDGE 0x4
#define FLAG_A8_EDGE_CHANGED 0x10
#define FLAG_A8_OFF_SEGMENT 0x200
#define PATH_POINT_SIZE 3
#define INDEX_SIGN_SHIFT 8

struct Unk8001F054Node {
    /* 0x00 */ u8 unk_00[4];
    /* 0x04 */ struct Unk8001F054Info *unk_04;
};

struct Unk8001F054Info {
    /* 0x00 */ u8 unk_00[8];
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ u8 unk_0C[8];
    /* 0x14 */ s32 *unk_14;
};

struct Unk8001F054Obj {
    /* 0x00 */ u8 unk_00[0x60];
    /* 0x60 */ s32 unk_60;
    /* 0x64 */ u8 unk_64[4];
    /* 0x68 */ s32 unk_68;
    /* 0x6C */ u8 unk_6C[8];
    /* 0x74 */ s32 unk_74;
    /* 0x78 */ u8 unk_78[0x1C];
    /* 0x94 */ s32 unk_94;
    /* 0x98 */ s32 unk_98;
    /* 0x9C */ u8 unk_9C[4];
    /* 0xA0 */ s32 unk_A0;
    /* 0xA4 */ u8 unk_A4[4];
    /* 0xA8 */ s32 unk_A8;
    /* 0xAC */ s32 unk_AC;
    /* 0xB0 */ u8 unk_B0[0x24];
    /* 0xD4 */ s32 unk_D4;
    /* 0xD8 */ u8 unk_D8[0x18];
    /* 0xF0 */ struct Unk8001F054Node *unk_F0;
    /* 0xF4 */ s32 unk_F4;
    /* 0xF8 */ u8 unk_F8[0x3C];
    /* 0x134 */ s32 unk_134;
};

s32 func_8001F054(struct Obj *arg0, u32 arg1, s32 *out) {
    struct Unk8001F054Obj *obj;
    s32 pos;
    s32 *base;
    s32 shift;
    s32 count;
    s32 seg;
    s32 frac;
    s32 len;
    s32 t;
    s32 t4;
    s32 delta[3];
    s32 da;
    s32 db;
    s32 flags;
    s32 oldA8;
    s32 curA8;
    s32 next[3];
    s32 ax;
    s32 az;
    s32 posBase;
    s16 *pt;
    struct Unk8001F054Node *node;
    struct Unk8001F054Info *info;

    obj = (struct Unk8001F054Obj *)arg0;
    pos = arg1;
    node = obj->unk_F0;
    if (obj == 0 || node == 0) {
        return ERR_NO_PATH;
    }
    info = node->unk_04;
    /* base and shift stay unset for any other node type, as in the retail code */
    if (info->unk_08 == NODE_TYPE_SHIFTED) {
        base = info->unk_14;
        shift = NODE_SHIFT_SHIFTED;
    }
    count = func_800322C0((s32)node, PATH_KIND, 0, PATH_ARG_SHIFT);
    while (1) {
        if (pos >= 0) {
            seg = pos >> FRAC_BITS;
        } else {
            seg = -pos >> FRAC_BITS;
        }
        t = func_800322C0((s32)node, PATH_KIND, 0, PATH_ARG_SHIFT);
        frac = pos & FRAC_MASK;
        if (seg == t - 1 && seg != 0) {
            frac = (pos & FRAC_MASK) + FRAC_ONE;
            seg = t - 2;
        }
        pt = (s16 *)func_80031AE8((s32)node, PATH_KIND, 0, 0, 0, PATH_ARG_SHIFT, 0, 0, 0);
        /* The segment index keeps its low 24 bits, sign-extended */
        pt += ((seg << INDEX_SIGN_SHIFT) >> INDEX_SIGN_SHIFT) * PATH_POINT_SIZE;
        out[0] = ((pt[0] << shift) + base[0]) << FRAC_BITS;
        out[1] = ((pt[1] << shift) + base[1]) << FRAC_BITS;
        out[2] = ((pt[2] << shift) + base[2]) << FRAC_BITS;
        if (count == 1) {
            return 1;
        }
        pt += PATH_POINT_SIZE;
        next[0] = ((pt[0] << shift) + base[0]) << FRAC_BITS;
        next[1] = ((pt[1] << shift) + base[1]) << FRAC_BITS;
        next[2] = ((pt[2] << shift) + base[2]) << FRAC_BITS;
        delta[0] = next[0] - out[0];
        delta[1] = next[1] - out[1];
        delta[2] = next[2] - out[2];
        if (obj->unk_AC & FLAG_AC_SEGMENT) {
            ax = delta[0] >> FRAC_BITS;
            az = delta[2] >> FRAC_BITS;
            len = func_80041A78(ax * ax + az * az);
            posBase = pos & ~FRAC_MASK;
            if (len != 0) {
                t = ((((obj->unk_60 - out[0]) >> STEP_SHIFT) * (delta[0] >> STEP_SHIFT)) +
                     (((obj->unk_68 - out[2]) >> STEP_SHIFT) * (delta[2] >> STEP_SHIFT))) / len;
                frac = t / len;
                if (frac >= FRAC_ONE && (pos >> FRAC_BITS) < count - 1) {
                    pos = (seg + 1) << FRAC_BITS;
                    continue;
                }
                t4 = t >> STEP_SHIFT;
                ax = ((t4 * (delta[0] >> STEP_SHIFT)) / len) + out[0];
                az = ((t4 * (delta[2] >> STEP_SHIFT)) / len) + out[2];
                da = ax - obj->unk_60;
                if (da < 0) {
                    da = obj->unk_60 - ax;
                }
                db = az - obj->unk_68;
                if (db < 0) {
                    db = obj->unk_68 - az;
                }
                /* Octagon-style distance estimate: max + min / 2 */
                if (db < da) {
                    obj->unk_A0 = da + db / 2;
                } else {
                    obj->unk_A0 = db + da / 2;
                }
                if (obj->unk_134 < obj->unk_A0 || (frac < 0 && seg == 0) || frac >= FRAC_ONE) {
                    obj->unk_A8 |= FLAG_A8_OFF_SEGMENT;
                }
                if (((delta[2] * (obj->unk_60 >> FRAC_BITS)) - (delta[0] * (obj->unk_68 >> FRAC_BITS)) -
                     (next[2] * (out[0] >> FRAC_BITS)) + (next[0] * (out[2] >> FRAC_BITS))) < 0) {
                    obj->unk_A0 = -obj->unk_A0;
                }
            } else {
                obj->unk_A8 |= FLAG_A8_OFF_SEGMENT;
            }
            if (frac < FRAC_MAX) {
                obj->unk_D4 = posBase;
                if (frac >= 0) {
                    obj->unk_D4 = posBase + frac;
                }
            } else {
                obj->unk_D4 = posBase + FRAC_MAX;
            }
            if (len == 0) {
                obj->unk_A0 = 0;
            }
        }
        break;
    }

    oldA8 = obj->unk_A8;
    if (obj->unk_F4 < 0 || (pos >> FRAC_BITS) >= count - 1) {
        obj->unk_A8 = oldA8 | FLAG_A8_EDGE;
    } else {
        obj->unk_A8 = oldA8 & ~FLAG_A8_EDGE;
    }
    curA8 = obj->unk_A8;
    if ((curA8 & FLAG_A8_EDGE) != (oldA8 & FLAG_A8_EDGE) && !(curA8 & FLAG_A8_EDGE_CHANGED)) {
        obj->unk_A8 = curA8 | FLAG_A8_EDGE_CHANGED;
    } else {
        obj->unk_A8 = obj->unk_A8 & ~FLAG_A8_EDGE_CHANGED;
    }

    flags = obj->unk_AC;
    if (flags & FLAG_AC_FACE) {
        if ((flags & FLAG_AC_MIRROR) && (obj->unk_A8 & FLAG_A8_EDGE)) {
            obj->unk_94 = ratan2(-delta[0], -delta[2]);
        } else {
            obj->unk_94 = ratan2(delta[0], delta[2]);
        }
        if (obj->unk_AC & FLAG_AC_PITCH) {
            obj->unk_74 = ratan2(delta[0], delta[2]);
            if (delta[0] < 0) {
                delta[0] = -delta[0];
            }
            if (delta[2] < 0) {
                delta[2] = -delta[2];
            }
            if (delta[2] < delta[0]) {
                da = delta[0] + delta[2] / 2;
            } else {
                da = delta[2] + delta[0] / 2;
            }
            obj->unk_98 = -ratan2(delta[1], da);
        }
    } else if (flags & FLAG_AC_PITCH) {
        if (delta[0] < 0) {
            da = -delta[0];
        } else {
            da = delta[0];
        }
        if (delta[2] < 0) {
            db = -delta[2];
        } else {
            db = delta[2];
        }
        if (db < da) {
            da = da + db / 2;
        } else {
            da = db + da / 2;
        }
        obj->unk_98 = ratan2(delta[1], da);
        obj->unk_74 = ratan2(-delta[0], -delta[2]);
    } else {
        /* neither flag: no angles are written */
    }

    out[0] += ((next[0] - out[0]) * frac) >> FRAC_BITS;
    out[1] += ((next[1] - out[1]) * frac) >> FRAC_BITS;
    out[2] += ((next[2] - out[2]) * frac) >> FRAC_BITS;
    return count;
}
