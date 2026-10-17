#include "common.h"
#include "sdk.h"
#include "game.h"

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

s32 func_8002CC50(void *, s32 *, s32 *, s32);
extern Unk8006CB6C D_8006CB6C;

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
