#include "common.h"
#include "sdk.h"
#include "game.h"

#define EDGE_FRAC_SHIFT 13
#define EDGE_FRAC_ROUND 0x1FFF
#define EDGE_SCALE 4
#define GRID_ROWS 32
#define GRID_ORIGIN 0x10
#define GRID_CLIP_MARGIN 0x20
#define GRID_LAST 0x1F
#define CELL_CLAMP_LO (-1)
#define CELL_CLAMP_HI 0x20
#define ROW_EMPTY_WORD 0xFE21FE21
#define ROW_TABLE_WORDS 16
#define NUM_BOX_CORNERS 8
#define CLIP_CELL_PENDING_NONE 0x3039
#define CALL_INTERVAL 8
#define CLIP_WINDOW_LO (-0x10)
#define CLIP_WINDOW_HI 0x2F
#define MIN_CLIPPED_EDGES 3
#define POINT_WORDS 3
/* func_8002FFFC's segments: two points each, 6 at most */
#define CLIP_POINTS 12
/* A row's lowest and highest cell */
#define ROW_BOUNDS 2

typedef struct {
    /* 0x00 */ s32 unk_0;
    /* 0x04 */ s32 *unk_4;
    /* 0x08 */ s32 unk_8;
    /* 0x0C */ s32 unk_C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s32 unk_20[3];
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34[2];
    /* 0x3C */ s32 unk_3C;
} Unk8002CC50;

s32 *D_8005F3C4;
/* A point of func_8002FFFC's, as 8002FE58.c defines it */
typedef struct Unk8002FFFCVec {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk8002FFFCVec;

s32 func_8002FFFC(s32, Unk8002FFFCVec *, Unk8002FFFCVec *);
void func_8003EAD4(s32 *, s32 *, s32, void *, s32, s32);

/* (d * 4) / 8192 with the compiler's signed-division rounding, pre-shift */
static inline s32 func_8002CC50_ScaleRaw(s32 d) {
    s32 raw;

    raw = d * EDGE_SCALE;
    if (raw < 0) {
        raw += EDGE_FRAC_ROUND;
    }
    return raw;
}

/* Widens the row's [min, max] pair with column x, clamped to [-1, 32]. */
static inline void func_8002CC50_Widen(s8 *row, s32 x) {
    s32 v;

    v = x;
    if (v < CELL_CLAMP_LO) {
        v = CELL_CLAMP_LO;
    }
    if (v > CELL_CLAMP_HI) {
        v = CELL_CLAMP_HI;
    }
    if (v < row[0]) {
        row[0] = v;
    }
    if (row[1] < v) {
        row[1] = v;
    }
}

typedef struct {
    s32 pad_0[2];
    s32 corners[NUM_BOX_CORNERS * POINT_WORDS];
    s32 pts[POINT_WORDS * CLIP_POINTS];
    s8 tbl[GRID_ROWS * ROW_BOUNDS];
} Frame8002CC50;

s32 func_8002CC50(void *arg0, s32 *arg1, s32 *p2, s32 p3) {
    Frame8002CC50 f;
    s32 *arg2;
    s32 arg3;
    s32 lvlEnd;
    s32 lvlMid;
    s32 lvl;
    s32 pendZ;
    Unk8002CC50 *obj;
    s32 *words;
    s32 *p;
    s32 *bits;
    s8 *row;
    s32 ret;
    s32 n;
    s32 i;
    s32 e;
    s32 minX, maxX, minZ, maxZ;
    s32 bx, bz;
    s32 rawMaxX, rawMaxZ, rawMinX, rawMinZ;
    s32 hiX, hiZ, loX, loZ;
    s32 colLo, colHi, rowLo, rowHi;
    s32 x1, z1, x2, z2, t;
    s32 dz, dx, step, a3, t2, s3, s5, t4, pendX;
    s32 lo, hi, v, mask;

    arg2 = p2;
    arg3 = p3;
    obj = arg0;
    ret = func_80030EC0(arg1, &obj->unk_2C);
    if (ret == 0) {
        return ret;
    }

    f.corners[0] = obj->unk_8;
    f.corners[1] = obj->unk_C;
    f.corners[2] = obj->unk_10;
    f.corners[3] = obj->unk_8;
    f.corners[4] = obj->unk_18;
    f.corners[5] = obj->unk_10;
    f.corners[6] = obj->unk_14;
    f.corners[7] = obj->unk_18;
    f.corners[8] = obj->unk_10;
    f.corners[9] = obj->unk_14;
    f.corners[10] = obj->unk_C;
    f.corners[11] = obj->unk_10;
    f.corners[12] = obj->unk_8;
    f.corners[13] = obj->unk_C;
    f.corners[14] = obj->unk_1C;
    f.corners[15] = obj->unk_8;
    f.corners[16] = obj->unk_18;
    f.corners[17] = obj->unk_1C;
    f.corners[18] = obj->unk_14;
    f.corners[19] = obj->unk_18;
    f.corners[20] = obj->unk_1C;
    f.corners[21] = obj->unk_14;
    f.corners[22] = obj->unk_C;
    f.corners[23] = obj->unk_1C;
    func_8003EAD4(f.corners, f.corners, obj->unk_4[2], obj->unk_20, 8, 4);

    lvlEnd = arg1[4];
    lvl = arg1[1];
    lvlMid = (lvlEnd + lvl) >> 1;
    if (obj->unk_3C < lvlEnd) {
        lvlEnd = obj->unk_3C;
    }
    if (lvl < obj->unk_30) {
        lvl = obj->unk_30;
    }
    if (obj->unk_3C < lvlMid) {
        lvlMid = obj->unk_3C;
    }
    if (lvlMid < obj->unk_30) {
        lvlMid = obj->unk_30;
    }

    for (;;) {
        n = func_8002FFFC(lvl, (Unk8002FFFCVec *)f.corners, (Unk8002FFFCVec *)f.pts);
        /* The retail code returns whatever v0 holds at exit; ret tracks it. */
        ret = n * 2;
        if (n >= MIN_CLIPPED_EDGES) {
            minX = f.pts[0];
            maxX = f.pts[0];
            minZ = f.pts[2];
            maxZ = f.pts[2];
            for (i = 1; i <= n * 2 - 1; i++) {
                p = &f.pts[i * POINT_WORDS];
                if (p[0] < minX) {
                    minX = p[0];
                }
                if (maxX < p[0]) {
                    maxX = p[0];
                }
                if (p[2] < minZ) {
                    minZ = p[2];
                }
                if (maxZ < p[2]) {
                    maxZ = p[2];
                }
            }
            bx = arg2[0];
            bz = arg2[2];
            rawMaxX = func_8002CC50_ScaleRaw(maxX - bx);
            hiX = rawMaxX >> EDGE_FRAC_SHIFT;
            rawMaxZ = func_8002CC50_ScaleRaw(maxZ - bz);
            hiZ = rawMaxZ >> EDGE_FRAC_SHIFT;
            rawMinX = func_8002CC50_ScaleRaw(minX - bx);
            loX = rawMinX >> EDGE_FRAC_SHIFT;
            rawMinZ = func_8002CC50_ScaleRaw(minZ - bz);
            loZ = rawMinZ >> EDGE_FRAC_SHIFT;
            colHi = hiX + GRID_ORIGIN;
            rowHi = hiZ + GRID_ORIGIN;
            colLo = loX + GRID_ORIGIN;
            rowLo = loZ + GRID_ORIGIN;
            ret = rawMinZ;

            if (arg3 != 0) {
                /* Each early exit leaves the value its branch delay slot computed in $v0. */
                if (hiX + GRID_CLIP_MARGIN < 0) {
                    ret = hiZ + GRID_CLIP_MARGIN;
                    goto next_level;
                }
                if (hiZ + GRID_CLIP_MARGIN < 0) {
                    ret = loX < GRID_CLIP_MARGIN;
                    goto next_level;
                }
                if (loX >= GRID_CLIP_MARGIN) {
                    ret = loZ < GRID_CLIP_MARGIN;
                    goto next_level;
                }
                if (loZ >= GRID_CLIP_MARGIN) {
                    ret = 0;
                    goto next_level;
                }
                colLo = CLIP_WINDOW_LO;
                rowLo = CLIP_WINDOW_LO;
                colHi = CLIP_WINDOW_HI;
                rowHi = CLIP_WINDOW_HI;
            } else {
                if (colHi < 0) {
                    goto next_level;
                }
                if (rowHi < 0) {
                    ret = colLo < GRID_ROWS;
                    goto next_level;
                }
                if (colLo >= GRID_ROWS) {
                    ret = rowLo < GRID_ROWS;
                    goto next_level;
                }
                if (rowLo >= GRID_ROWS) {
                    ret = 0;
                    goto next_level;
                }
                colLo = 0;
                rowLo = 0;
                colHi = GRID_LAST;
                rowHi = GRID_LAST;
            }

            words = (s32 *)f.tbl;
            for (i = 0; i < ROW_TABLE_WORDS; i++) {
                words[i] = ROW_EMPTY_WORD;
            }

            p = f.pts;
            for (e = n - 1; e >= 0; e--) {
                x1 = (func_8002CC50_ScaleRaw(p[0] - bx) >> EDGE_FRAC_SHIFT) + GRID_ORIGIN;
                z1 = (func_8002CC50_ScaleRaw(p[2] - bz) >> EDGE_FRAC_SHIFT) + GRID_ORIGIN;
                p += POINT_WORDS;
                x2 = (func_8002CC50_ScaleRaw(p[0] - bx) >> EDGE_FRAC_SHIFT) + GRID_ORIGIN;
                z2 = (func_8002CC50_ScaleRaw(p[2] - bz) >> EDGE_FRAC_SHIFT) + GRID_ORIGIN;
                p += POINT_WORDS;

                if (z2 < z1) {
                    t = z1;
                    z1 = z2;
                    z2 = t;
                    t = x1;
                    x1 = x2;
                    x2 = t;
                }
                if (rowHi < z1 || z2 < rowLo) {
                    continue;
                }
                dz = z2 - z1;
                dx = x2 - x1;
                if (dz == 0) {
                    /* horizontal edge */
                    if ((u32)z1 < GRID_ROWS) {
                        row = &f.tbl[z1 * 2];
                        func_8002CC50_Widen(row, x1);
                        func_8002CC50_Widen(row, x2);
                    }
                    if (arg3 != 0) {
                        if (x2 < x1) {
                            t = x1;
                            x1 = x2;
                            x2 = t;
                        }
                        if (colHi < x1 || x2 < colLo) {
                            continue;
                        }
                        if (x1 < colLo) {
                            x1 = colLo;
                        }
                        if (colHi < x2) {
                            x2 = colHi;
                        }
                        do {
                            func_8002CB1C(x1 - GRID_ORIGIN, z1 - GRID_ORIGIN);
                            x1 += CALL_INTERVAL;
                        } while (x1 <= x2);
                        if (x1 != x2) {
                            func_8002CB1C(x2 - GRID_ORIGIN, z1 - GRID_ORIGIN);
                        }
                    }
                    continue;
                }

                step = 1;
                if (dx < 0) {
                    dx = -dx;
                    step = -1;
                }
                if (rowHi < z2) {
                    z2 = rowHi;
                }
                pendX = CLIP_CELL_PENDING_NONE;
                s5 = 0;
                if (dz < dx) {
                    /* x-major */
                    a3 = dz * 2;
                    t2 = (dz - dx) * 2;
                    s3 = a3 - dx;
                    if (z1 < rowLo) {
                        do {
                            if (s3 > 0) {
                                s3 += t2;
                                z1 += 1;
                            } else {
                                s3 += a3;
                            }
                            x1 += step;
                        } while (z1 < rowLo);
                    }
                    row = &f.tbl[(z1 < 0 ? 0 : z1) * 2];
                    if ((u32)z1 < GRID_ROWS) {
                        func_8002CC50_Widen(row, x1);
                    }
                    t4 = x2 + step;
                    for (;;) {
                        if (arg3 != 0 && x1 >= colLo && x1 <= colHi) {
                            s5 -= 1;
                            if (s5 <= 0) {
                                s5 = CALL_INTERVAL;
                                func_8002CB1C(x1 - GRID_ORIGIN, z1 - GRID_ORIGIN);
                                pendX = CLIP_CELL_PENDING_NONE;
                            } else {
                                pendX = x1;
                                pendZ = z1;
                            }
                        }
                        if (s3 <= 0) {
                            x1 += step;
                            if (x1 == t4) {
                                break;
                            }
                            s3 += a3;
                            continue;
                        }
                        s3 += t2;
                        if ((u32)z1 < GRID_ROWS) {
                            func_8002CC50_Widen(row, x1);
                            row += 2;
                        }
                        x1 += step;
                        if (x1 == t4) {
                            break;
                        }
                        z1 += 1;
                        if (z2 < z1) {
                            break;
                        }
                        if ((u32)z1 < GRID_ROWS) {
                            func_8002CC50_Widen(row, x1);
                        }
                    }
                } else {
                    /* z-major */
                    a3 = dx * 2;
                    t2 = (dx - dz) * 2;
                    s3 = a3 - dz;
                    if (z1 < rowLo) {
                        do {
                            if (s3 > 0) {
                                s3 += t2;
                                x1 += step;
                            } else {
                                s3 += a3;
                            }
                            z1 += 1;
                        } while (z1 < rowLo);
                    }
                    row = &f.tbl[(z1 < 0 ? 0 : z1) * 2];
                    do {
                        if ((u32)z1 < GRID_ROWS) {
                            func_8002CC50_Widen(row, x1);
                            row += 2;
                        }
                        if (arg3 != 0 && x1 >= colLo && x1 <= colHi) {
                            s5 -= 1;
                            if (s5 <= 0) {
                                s5 = CALL_INTERVAL;
                                func_8002CB1C(x1 - GRID_ORIGIN, z1 - GRID_ORIGIN);
                                pendX = CLIP_CELL_PENDING_NONE;
                            } else {
                                pendX = x1;
                                pendZ = z1;
                            }
                        }
                        if (s3 > 0) {
                            s3 += t2;
                            x1 += step;
                        } else {
                            s3 += a3;
                        }
                        z1 += 1;
                    } while (!(z2 < z1));
                }
                if (arg3 != 0 && pendX != CLIP_CELL_PENDING_NONE) {
                    func_8002CB1C(pendX - GRID_ORIGIN, pendZ - GRID_ORIGIN);
                }
            }

            bits = D_8005F3C4;
            row = f.tbl;
            for (i = GRID_LAST; i >= 0; i--) {
                lo = row[0];
                hi = row[1];
                row += 2;
                if (hi < CELL_CLAMP_LO) {
                    ret = 1;
                } else {
                    if (lo < 0 && hi >= 0) {
                        lo = 0;
                    }
                    if (hi >= GRID_ROWS && lo < GRID_ROWS) {
                        hi = GRID_LAST;
                    }
                    /* An off-grid span leaves the delay slot's value in $v0. */
                    if ((u32)lo >= GRID_ROWS) {
                        ret = (u32)hi < GRID_ROWS;
                    } else if ((u32)hi >= GRID_ROWS) {
                        ret = -1U >> lo;
                    } else {
                        mask = (-1U >> lo) & (-1 << (GRID_LAST - hi));
                        *bits |= mask;
                        ret = mask;
                    }
                }
                bits += 1;
            }
        }
next_level:
        if (lvl == lvlEnd) {
            break;
        }
        if (lvl == lvlMid) {
            lvl = lvlEnd;
        } else {
            lvl = lvlMid;
        }
    }
    return ret;
}
#undef f.corners
#undef f.pts
#undef f.tbl
#undef arg2
#undef arg3
#undef lvlEnd
#undef lvlMid
#undef lvl
#undef pendZ
