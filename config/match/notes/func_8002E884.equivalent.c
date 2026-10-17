#include "common.h"
#include "sdk.h"
#include "game.h"

#define CELL_SOLID 0x2000
#define CELL_HEIGHT_MASK 0x1FFF
#define CELL_EDGE_MASK 0xC000
#define EDGE_SLOPE_UP 0x4000
#define EDGE_FLAT 0x8000
#define CELL_HI_SHIFT 16
#define FIXED_SHIFT 8
#define MAP_CELLS_OFFSET 0x1C
#define MAP_DATA_OFFSET 0x10
#define NAME_LEN 6
#define NAME_PAD_OFFSET 6
#define NAME_PAD_LEN 2
#define NAME_TAG_OFFSET 3
#define SCORE_NONE 0x7FFFFFFF
#define NEIGHBOUR_PASSES 2

extern s32 D_8006758C;

void func_8002E884(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3) {
    char name[8];
    s32 key[4];
    s32 sp20[2];
    s32 sp28[2];
    s32 sp30[2];
    s32 *map;
    s32 *cells;
    s32 cellW;
    s32 px, pz;
    s32 row;
    s32 rowLo, rowHi, rowMin, rowMax;
    s32 seenOpen;
    s32 i, k;
    s32 cell, hi, c0, c1;
    s32 pos;
    s32 best, bestX, bestZ;
    s32 rowOff, dz, s2, limit;
    s32 a0, a1, dir, step, d0, d1, score, lim, ext;
    s32 v, a2v;
    s32 near;

    for (i = 0; i < NAME_LEN; i++) {
        name[i] = D_8005F208[i];
    }
    memset(&name[NAME_PAD_OFFSET], 0, NAME_PAD_LEN);
    name[NAME_TAG_OFFSET] = func_800120EC(D_8006758C);
    key[0] = func_80011F78(name);
    map = *(s32 **)((char *)func_80014260(key, 1, 0) + MAP_DATA_OFFSET);
    cells = (s32 *)((char *)map + MAP_CELLS_OFFSET);
    cellW = map[3];
    pz = (arg0[2] + arg1[2] - map[2]) >> FIXED_SHIFT;
    row = pz / cellW;
    best = SCORE_NONE;
    seenOpen = 0;
    px = (arg0[0] + arg1[0] - map[0]) >> FIXED_SHIFT;
    rowMin = row - 1;
    rowMax = row + 1;
    rowLo = row - 2;

    for (i = rowMin; i <= rowMax; i++) {
        if (!(cells[i] & CELL_SOLID)) {
            seenOpen = 1;
            if (cells[i - 1] & CELL_SOLID) {
                rowLo = i;
            } else {
                /* no edge here */
            }
        } else if (arg2 == 0) {
            return;
        } else {
            /* solid cell, caller allows it */
        }
    }
    if (seenOpen == 0) {
        return;
    }
    rowHi = rowMax + 1;
    for (i = rowMax; i >= rowMin; i--) {
        if (!(cells[i] & CELL_SOLID)) {
            if (cells[i + 1] & CELL_SOLID) {
                rowHi = i;
            } else {
                /* no edge here */
            }
        } else if (arg2 == 0) {
            return;
        } else {
            /* solid cell, caller allows it */
        }
    }

    near = 0;
    if (pz < rowLo * cellW + arg3) {
        near = 1;
    } else if (rowHi * cellW + (cellW - arg3) < pz) {
        near = 1;
    } else {
        cell = cells[rowMin + 1];
        a2v = pz - (rowMin + 1) * cellW;
        for (k = 0; k < NEIGHBOUR_PASSES; k++) {
            if (!(cell & CELL_EDGE_MASK)) {
                v = px - a2v;
            } else if (cell & EDGE_FLAT) {
                if (k == 0) {
                    v = px - cellW;
                } else {
                    v = px;
                }
            } else {
                v = px + a2v - cellW;
            }
            v -= cellW * (cell & CELL_HEIGHT_MASK);
            if (k != 0) {
                lim = v < arg3;
            } else {
                lim = -arg3 < v;
            }
            if (lim != 0) {
                near = 1;
                break;
            } else {
                cell >>= CELL_HI_SHIFT;
            }
        }
        if (near == 0) {
            return;
        } else {
            /* falls through to the search */
        }
    }

    rowOff = rowMin * cellW;
    for (i = rowMin; i <= rowMax; i++, rowOff += cellW) {
        cell = cells[i];
        if (cell & CELL_SOLID) {
            continue;
        }
        c0 = cell & CELL_EDGE_MASK;
        hi = cell >> CELL_HI_SHIFT;
        c1 = hi & CELL_EDGE_MASK;
        s2 = 0;
        limit = cellW;
        sp28[0] = c0;
        sp28[1] = c1;
        sp20[0] = (cell & CELL_HEIGHT_MASK) * cellW - arg3;
        sp20[1] = (hi & CELL_HEIGHT_MASK) * cellW + arg3;
        dz = pz - rowOff;
        if (c1 == EDGE_SLOPE_UP) {
            sp20[1] += cellW;
        } else {
            /* upper edge not raised */
        }
        if (c0 != 0) {
            sp20[0] += cellW;
        } else {
            /* lower edge not raised */
        }
        if (i == rowLo || i == rowHi) {
            a1 = arg3;
            if (i == rowHi) {
                a1 = cellW - a1;
            } else {
                /* margin measured from the near side */
            }
            sp30[0] = sp20[0];
            if (c0 == 0) {
                sp30[0] = sp20[0] + a1;
            } else if (c0 == EDGE_SLOPE_UP) {
                sp30[0] = sp20[0] - a1;
            } else {
                /* flat edge keeps its value */
            }
            sp30[1] = sp20[1];
            if (c1 == 0) {
                sp30[1] = sp20[1] + a1;
            } else if (c1 == EDGE_SLOPE_UP) {
                sp30[1] = sp20[1] - a1;
            } else {
                /* flat edge keeps its value */
            }
            v = sp30[0];
            if (v < px) {
                a0 = v;
            } else {
                v = sp30[1];
                if (px < v) {
                    a0 = v;
                } else {
                    a0 = px;
                }
            }
            d0 = px - a0;
            d1 = dz - a1;
            score = d0 * d0 + d1 * d1;
            if (score < best) {
                bestX = a0;
                bestZ = a1 + rowOff;
                best = score;
            } else {
                /* not closer */
            }
            limit = cellW - a1;
            if (i == rowHi) {
                limit = a1;
            } else {
                s2 = a1;
                sp20[0] = sp30[0];
                sp20[1] = sp30[1];
            }
        } else {
            /* interior row */
        }
        for (k = 0; k < NEIGHBOUR_PASSES; k++) {
            dir = sp28[k];
            a0 = sp20[k];
            a1 = s2;
            if (dir == EDGE_FLAT) {
                step = dz - a1;
            } else {
                if (dir != 0) {
                    ext = (dz - px) - (a1 - a0);
                } else {
                    ext = (dz + px) - (a1 + a0);
                }
                step = ext >> 1;
            }
            if (limit < step) {
                step = limit;
            } else {
                /* step within limit */
            }
            d0 = px - a0;
            if (step > 0) {
                a1 += step;
                if (sp28[k] == EDGE_SLOPE_UP) {
                    a0 -= step;
                    d0 = px - a0;
                } else if (sp28[k] == 0) {
                    a0 += step;
                    d0 = px - a0;
                } else {
                    /* flat edge does not move */
                }
            } else {
                /* no movement */
            }
            d1 = dz - a1;
            score = d0 * d0 + d1 * d1;
            if (score < best) {
                bestX = a0;
                bestZ = a1 + rowOff;
                best = score;
            } else {
                /* not closer */
            }
        }
    }

    if (best != SCORE_NONE) {
        arg1[0] = (bestX << FIXED_SHIFT) - (arg0[0] - map[0]);
        arg1[2] = (bestZ << FIXED_SHIFT) - (arg0[2] - map[2]);
    } else {
        /* no candidate found */
    }
}
