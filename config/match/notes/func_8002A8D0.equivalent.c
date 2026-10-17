#include "common.h"
#include "sdk.h"
#include "game.h"

#define NO_LIMIT (-0x3B9AC9FF)
#define KIND_MASK 7
#define KIND_BOX 0
#define KIND_QUAD_BOX 1
#define NUM_FACES 6
#define FACE_CORNERS 4
#define NUM_VERTS 8
#define COORD_SHIFT 7
#define FLAG_BIT 1

extern s32 D_8005BBB8[];
extern u8 *D_8005F38C;

void func_8003EAD4(s32 *dst, s32 *src, s32 arg2, void *arg3, s32 count, s32 arg5);

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Unk80030498;

typedef struct {
    s32 x;
    s32 y;
} Unk80030680;

s32 func_80030680(Unk80030680 *point, Unk80030680 *quad);
s32 func_80030498(s32 arg0, s32 arg1, Unk80030498 *arg2);

/* The function's frame in retail's layout, its spill slots included: a D_8005BBB8 index outside 0..7 reads past v. */
typedef struct {
    s32 v[NUM_VERTS * 3];
    s32 *p[FACE_CORNERS];
    s32 w[FACE_CORNERS * 3];
    s32 q[FACE_CORNERS * 2];
    s32 pt[2];
    s32 arg2;
    s32 unk_CC;
    s32 arg3;
    s32 unk_D4;
    s32 *qp;
    s32 unk_DC;
    s32 arg4;
    s32 arg5;
} Unk8002A8D0;

s32 func_8002A8D0(void *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    s32 *o = (s32 *)arg0;
    s32 *out = (s32 *)arg6;
    Unk8002A8D0 f;
    s32 pad;
    s32 face;
    s32 k;
    s32 cross;
    s32 *pa;
    s32 *pb;
    s32 *pc;
    s32 cx;
    s32 cy;
    s32 cz;
    s32 r;
    s32 t;
    s32 minY;
    s32 minK;
    s32 *next;
    s32 *cur;
    s32 *prev;

    f.arg2 = arg2;
    f.arg3 = arg3;
    switch (o[0] & KIND_MASK) {
    case KIND_BOX:
        pad = 0;
        if (*(s32 *)(*(s32 *)((char *)D_8005F38C + 8) + 0x4C) & FLAG_BIT) {
            pad = *(s32 *)(o[1] + 0x128);
        }
        if (arg3 != 0) {
            if (o[6] < arg1[1]) {
                return NO_HIT;
            }
            if (arg1[4] < o[3]) {
                return NO_HIT;
            }
        }
        if (arg2 != 0) {
            if (arg4 != NO_LIMIT && o[6] < arg4) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && arg5 < o[6]) {
                return NO_HIT;
            }
        } else {
            if (arg4 != NO_LIMIT && arg4 < o[3]) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && o[3] < arg5) {
                return NO_HIT;
            }
        }
        if (arg1[3] - pad < o[2]) {
            return NO_HIT;
        }
        if (o[5] < arg1[0] + pad) {
            return NO_HIT;
        }
        if (arg1[5] - pad < o[4]) {
            return NO_HIT;
        }
        if (o[7] < arg1[2] + pad) {
            return NO_HIT;
        }
        if (arg2 != 0) {
            return o[6];
        } else {
            return o[3];
        }
    case KIND_QUAD_BOX:
        if (arg3 != 0) {
            if (o[15] < arg1[1]) {
                return NO_HIT;
            }
            if (arg1[4] < o[12]) {
                return NO_HIT;
            }
        }
        if (arg2 != 0) {
            if (arg4 != NO_LIMIT && o[15] < arg4) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && arg5 < o[12]) {
                return NO_HIT;
            }
        } else {
            if (arg4 != NO_LIMIT && arg4 < o[12]) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && o[15] < arg5) {
                return NO_HIT;
            }
        }
        if (arg1[3] < o[11]) {
            return NO_HIT;
        }
        if (o[14] < arg1[0]) {
            return NO_HIT;
        }
        if (arg1[5] < o[13]) {
            return NO_HIT;
        }
        if (o[16] < arg1[2]) {
            return NO_HIT;
        }
        break;
    default:
        return NO_HIT;
    }

    /* Corners of the local box: x from o[2]/o[5], y from o[3]/o[6], z from o[4]/o[7]. */
    f.v[0] = o[2];  f.v[1] = o[3];  f.v[2] = o[4];
    f.v[3] = o[2];  f.v[4] = o[6];  f.v[5] = o[4];
    f.v[6] = o[5];  f.v[7] = o[6];  f.v[8] = o[4];
    f.v[9] = o[5];  f.v[10] = o[3]; f.v[11] = o[4];
    f.v[12] = o[2]; f.v[13] = o[3]; f.v[14] = o[7];
    f.v[15] = o[2]; f.v[16] = o[6]; f.v[17] = o[7];
    f.v[18] = o[5]; f.v[19] = o[6]; f.v[20] = o[7];
    f.v[21] = o[5]; f.v[22] = o[3]; f.v[23] = o[7];
    /* Retail's spill slots of &q, arg4 and arg5; arg4 and arg5 are read back only through a bad index. */
    f.qp = f.q;
    f.arg4 = arg4;
    f.arg5 = arg5;
    func_8003EAD4(f.v, f.v, *(s32 *)(o[1] + 8), (char *)arg0 + 0x20, NUM_VERTS, FACE_CORNERS);

    for (face = 0; face < NUM_FACES; face++) {
        for (k = 0; k < FACE_CORNERS; k++) {
            f.p[k] = &f.v[D_8005BBB8[face * FACE_CORNERS + k] * 3];
        }
        pa = f.p[0];
        pb = f.p[1];
        pc = f.p[2];
        cross = (((pb[2] - pa[2]) >> COORD_SHIFT) * ((pc[0] - pa[0]) >> COORD_SHIFT))
              - (((pb[0] - pa[0]) >> COORD_SHIFT) * ((pc[2] - pa[2]) >> COORD_SHIFT));
        if (f.arg2 != 0) {
            if (cross >= 0) {
                continue;
            }
        } else {
            if (cross <= 0) {
                continue;
            }
        }
        cx = (f.p[0][0] + f.p[1][0] + f.p[2][0] + f.p[3][0]) >> 2;
        cz = (f.p[0][2] + f.p[1][2] + f.p[2][2] + f.p[3][2]) >> 2;
        for (k = 0; k < FACE_CORNERS; k++) {
            f.q[k * 2] = (f.p[k][0] - cx) >> COORD_SHIFT;
            f.q[k * 2 + 1] = (f.p[k][2] - cz) >> COORD_SHIFT;
        }
        f.pt[0] = (((arg1[0] + arg1[3]) >> 1) - cx) >> COORD_SHIFT;
        f.pt[1] = (((arg1[2] + arg1[5]) >> 1) - cz) >> COORD_SHIFT;
        if (func_80030680((Unk80030680 *)f.pt, (Unk80030680 *)f.qp) == 0) {
            continue;
        }
        cy = (f.p[0][1] + f.p[1][1] + f.p[2][1] + f.p[3][1]) >> 2;
        for (k = 0; k < FACE_CORNERS; k++) {
            f.w[k * 3] = f.qp[k * 2];
            f.w[k * 3 + 2] = f.qp[k * 2 + 1];
            f.w[k * 3 + 1] = (f.p[k][1] - cy) >> COORD_SHIFT;
        }
        r = func_80030498(f.pt[0], f.pt[1], (Unk80030498 *)f.w);
        if (r == NO_HIT) {
            continue;
        }
        t = (r << COORD_SHIFT) + cy;
        if (f.arg3 != 0) {
            if (arg1[4] < t) {
                return NO_HIT;
            }
            if (t < arg1[1]) {
                return NO_HIT;
            }
        }
        if (f.arg2 != 0) {
            if (arg4 != NO_LIMIT && t < arg4) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && arg5 < t) {
                return NO_HIT;
            }
        } else {
            if (arg4 != NO_LIMIT && arg4 < t) {
                return NO_HIT;
            }
            if (arg5 != NO_LIMIT && t < arg5) {
                return NO_HIT;
            }
        }
        if (out != 0) {
            minY = f.p[0][1];
            minK = 0;
            for (k = 1; k < FACE_CORNERS; k++) {
                if (f.p[k][1] < minY) {
                    minY = f.p[k][1];
                    minK = k;
                }
            }
            out[1] = minY - cy;
            next = f.p[(minK + 1) & 3];
            cur = f.p[minK];
            prev = f.p[(minK - 1) & 3];
            if (next[1] == minY) {
                if (prev[1] == minY) {
                    out[0] = 0;
                    out[2] = 0;
                } else {
                    out[0] = ((cur[0] + next[0]) >> 1) - cx;
                    out[2] = ((cur[2] + next[2]) >> 1) - cz;
                }
            } else if (prev[1] == minY) {
                out[0] = ((cur[0] + prev[0]) >> 1) - cx;
                out[2] = ((cur[2] + prev[2]) >> 1) - cz;
            } else {
                out[0] = cur[0] - cx;
                out[2] = cur[2] - cz;
            }
        }
        return t;
    }
    return NO_HIT;
}
