#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F208[] = "vcm?X";

#ifdef NON_MATCHING
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
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002E884);
#endif

extern s32 D_8005C514;
extern s32 D_8005F3EC;
extern u8 *D_8006759C[];
extern s32 D_8006CB20;
extern s32 D_8006CB88;
extern s32 D_8006CB8C;
extern s32 D_8006CBA0;
extern s32 D_8006CBA8;
extern s32 D_8006CBE4;

void func_8002EE48(void) {
    func_800158E0();
    D_8006CBA0 = 0;
    switch (D_8005F3EC) {
    case 0x28:
    case 0x29:
    case 0x3B:
    case 0x3C:
        break;
    default: {
        s32 *cb20 = &D_8006CB20;
        u8 **p;
        D_8006CB8C = func_80019800(cb20, 4, 0, 0, 0, 0);
        D_8006CB88 = func_80019800(cb20, 4, 1, 0, 0, 0);
        D_8006CBA8 = func_80019800(cb20, 4, 5, 0, 0, 0);
        D_8006CBE4 = func_80019800(cb20, 4, 0x16, 0, 0, 0);
        func_80014260(D_8006759C[0] + 0x10, 0, 1);
        func_80014260(D_8006759C[0] + 0x24, 0, 1);
        func_80014260(D_8006759C[0] + 0x84, 0, 1);
        func_80014260(D_8006759C[0] + 0x98, 0, 1);
        func_80014260(D_8006759C[0] + 0x1C, 0, 1);
        func_80014260(D_8006759C[0] + 0x20, 0, 1);
        func_80014260(D_8006759C[0] + 0x34, 0, 1);
        break;
    }
    }
    func_8002F228(1);
    *(s32 **)0x1F80005C = &D_8005C514;
}

extern s32 D_8006CBB4;
extern s32 D_8006CBB8;
extern s32 D_8006CBBC;
extern s32 D_8006CBC0;
extern s32 D_8006CBC4;
extern s32 D_8006CBEC;
extern s32 D_8006CBF0;
extern s32 D_8006CBF4;
extern s32 D_8006CBF8;
extern s32 D_8006CBFC;
extern s32 D_8006CC80;
extern s32 D_8006CCEC;
extern s32 D_8006CCF0;
extern s32 D_8006CD24;
extern s32 D_8006CD3C;
extern s32 D_8006CDCC;
extern s32 D_8006CDD0;
extern s32 D_8006CDD4;
extern s32 D_8006CDD8;
extern s32 D_8006CDDC;
extern s32 D_8006CDE0;
extern s32 D_8006CDE4;
extern s32 D_8006CDE8;

void func_8002EFE4(void) {
    D_8006CBC0 = 0x63;
    D_8006CBBC = -1;
    D_8006CBB8 = 7;
    D_8006CBF8 = 0x100;
    D_8006CBFC = 0x100;
    D_8006CBEC = 0x400;
    D_8006CCEC = 1;
    D_8006CDD0 = 0x80;
    D_8006CDD4 = 0x80;
    D_8006CBC4 = 0;
    D_8006CBF0 = 0;
    D_8006CBF4 = 0;
    D_8006CC80 = 0;
    D_8006CD24 = 0;
    D_8006CD3C = 0;
    D_8006CCF0 = 0;
    D_8006CDE8 = 0;
    D_8006CDCC = 0;
    D_8006CDD8 = 0;
    D_8006CDDC = 0;
    D_8006CDE0 = 0xFF;
    D_8006CDE4 = 0xFF;
    D_8006CBB4 = 0;
    func_8002F0DC(1);
}

#ifdef NON_MATCHING
extern s32 D_8006935C[];
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC84;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;
extern s32 D_8006CB98;
extern s32 D_8006CBCC;
extern s32 D_8006CBD0;
extern s32 D_8006CBD4;
extern s32 D_8006CBD8;
extern s32 D_8006CBDC;
extern s32 D_8006CBE0;
extern s32 D_8006CC28;
extern s32 D_8006CC2C;
extern s32 D_8006CC30;
extern s32 D_8006CC34;
extern s32 D_8006CC50;
extern s32 D_8006CC54;
extern s32 D_8006CC6C;
extern s32 D_8006CC70;
extern s32 D_8006CC88;
extern s32 D_8006CC8C;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 D_8006CD7C;
extern s32 D_8006CDA4;
extern s32 D_8006CDA8;
extern s32 D_8006CDC8;

void func_8002F0DC(s32 arg0) {
    s32 t;
    s32 *p;
    if (arg0 != 0) {
        p = D_80069718;
        t = D_8006CBEC;
        D_8006CD7C = 0;
        D_8006CD20 = 0;
        D_8006CB84 = 0;
        D_8006CC00 = 0;
        D_8006CD40 = 0;
        D_8006CBD4 = 0;
        D_8006CBD8 = 0;
        D_8006CBDC = 0;
        D_8006CBE0 = 0;
        D_8006CC28 = 0;
        D_8006CC2C = 0;
        D_8006CDA4 = 0;
        D_8006CDA8 = 0;
        D_8006CDC8 = 0;
        D_8006CC50 = 0;
        D_8006CC54 = 0;
        D_8006CC30 = 0;
        D_8006CC34 = 0;
        D_8006CC84 = -1;
        D_8006CBC0 = 0x63;
        D_8006CCD4 = 0x63;
        D_8006CCD8 = 0;
        D_8006CB98 = 0;
        D_8006CC6C = 0;
        D_8006CC70 = 0;
        D_8006CC88 = 0;
        D_8006CC8C = 0;
        D_8006CDB4 = -1;
        D_8006CDB8 = 0;
        D_8006CBD0 = t;
        memset(p, 0, 0x200);
        memset(D_8006935C, 0, 0x200);
        D_8006CBCC = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F0DC);
#endif

extern s32 D_80068FE4;
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC68;
extern s32 D_8006CC74;
extern s32 D_8006CC84;
extern s32 D_8006CC94;
extern s32 D_8006CC98;
extern s32 D_8006CCA0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CD68;
extern s32 D_8006CD90;
extern s32 D_8006CD9C;
extern s32 D_8006CDA0;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;

void func_8002F228(s32 arg0) {
    int new_var;
    s32 *new_var2;

    D_8006CC94 = 0;
    D_8006CC68 = 0;
    D_8006CC74 = 0;
    if (D_8006CD14 != 2) {
        D_8006CCA0 = 0;
    }
    D_8006CC98 = 1;
    D_8006CD68 = 0;
    D_8006CD90 = 0;
    if (arg0 != 0) {
        new_var2 = (s32 *)D_8006759C[0] + 2;
        D_8006CD9C = 0;
        if (1) {
            D_8006CDA0 = 0;
            D_8006CB84 = 0;
            D_8006CC00 = 0;
        }
        D_8006CD40 = 0;
        D_8006CD20 = 0;
        D_8006CC84 = -1;
        new_var = (*new_var2) * 2;
        D_8006CDB4 = 0;
        D_8006CDB8 = (D_80069718[new_var] & 0xF) << 8;
    }
    func_80029884(0, 0, 0);
    D_80068FE4 = 0;
    func_8002FB84(1);
    D_8006CD1C = 0x20;
    if (D_8006CBB4 != 0x600) {
        D_8006CD18 = 0x120;
    }
}

#ifdef NON_MATCHING
#define CTX_FLAGS_OFFSET 0x29C
#define CTX_FLAGS_MASK 0x2000
#define MODE_INIT 0
#define MODE_MERGE 2
#define CLEAR_SIZE 0x4400
#define SRC_COPY_OFFSET 0x60
#define SRC_COPY_END_OFFSET 0x80
#define COPY_STEP_BYTES 0x10
#define KIND_OFFSET 0x120
#define KIND_FIRST 6
#define KIND_COUNT 2
#define CC00_STEP 0x100
#define ID_28 0x173
#define ID_38 0x288
#define CALL_FLAGS 2
#define NIBBLE_MASK 0xF
#define NIBBLE_BITS 4
#define NIBBLES_PER_WORD 8
#define TABLE_ROW_WORDS 2
#define BLOCK_WORDS 0x400
#define BLOCK_OFFSET 0x3C
#define BLOCK_SRC_DELTA 0x1060
#define TAIL_OFFSET 0x103C
#define CALLEE_OFFSET 0x1040
#define NO_ENTRY -1
#define VAL_SHIFT 8

extern u8 *D_8005F38C;
extern u8 *D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern u8 D_800619B8[];
extern s32 D_8006B1C4[];
extern s32 D_8006CD08[];

void func_8002F34C(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3) {
    s32 *src;
    s32 *dst;
    s32 *end;
    s32 i;
    s32 id;
    s32 isStatic;
    s32 *blk;

    if (*(s32 *)(*(u8 **)(D_800608CC + 0x10) + CTX_FLAGS_OFFSET) & CTX_FLAGS_MASK) {
        return;
    }
    isStatic = (arg1 == D_800619B8);
    blk = (s32 *)arg1;
    if (arg3 == MODE_INIT) {
        func_8001CC10((s32)arg0, (s32)D_8005F38C, CLEAR_SIZE, 0, (void *)0);
        if (arg2 != 0) {
            D_8006CC00 = 0;
            D_8006CC68 = 0;
        } else {
            D_8006CC00 += CC00_STEP;
        }
        *(s32 *)(arg1 + 0x34) = arg2;
        src = (s32 *)(D_8005F38C + SRC_COPY_OFFSET);
        end = (s32 *)(D_8005F38C + SRC_COPY_END_OFFSET);
        dst = (s32 *)arg1;
        do {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            src = (s32 *)((u8 *)src + COPY_STEP_BYTES);
            dst = (s32 *)((u8 *)dst + COPY_STEP_BYTES);
        } while (src != end);
        dst[0] = src[0];
        if ((u32)(arg0[KIND_OFFSET] - KIND_FIRST) >= KIND_COUNT) {
            blk[0] = *(s32 *)(arg0 + 0x60);
            blk[1] = *(s32 *)(arg0 + 0x64);
            blk[2] = *(s32 *)(arg0 + 0x68);
        }
        if (D_8006CC84 >= 0 && D_8006CC84 != 0) {
            blk[0] = D_8006CD08[0];
            blk[1] = D_8006CD08[1];
            blk[2] = D_8006CD08[2];
        }
        blk[3] = 0;
        blk[4] = 0;
        blk[5] = 0;
        *(s32 *)(arg1 + 0x24) = *(s32 *)(D_800608CC + 4);
        *(s32 *)(arg1 + 0x2C) = D_800608E0;
        *(s32 *)(arg1 + 0x30) = *(s32 *)(D_8006759C[0] + 8);
        *(s32 *)(arg1 + 0x28) = func_800321D4(D_800608D4, ID_28, 0, 0, 0, CALL_FLAGS, 0);
        if (D_8006CC84 < -1) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else if (*(s32 *)(arg0 + 0xF0) == 0) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else if (isStatic) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else {
            id = func_800321D4(*(s32 *)(arg0 + 0xF0), ID_38, 0, 0, 0, CALL_FLAGS, 0);
            *(s32 *)(arg1 + 0x38) = id;
            if (id != 0) {
                D_8006CDB8 = ((D_80069718[*(s32 *)(arg1 + 0x30) * TABLE_ROW_WORDS + id / NIBBLES_PER_WORD]
                               >> ((id % NIBBLES_PER_WORD) * NIBBLE_BITS)) & NIBBLE_MASK) << VAL_SHIFT;
                D_8006CDB4 = *(s32 *)(arg1 + 0x38);
            }
        }
        func_80014638((u32 *)(arg1 + CALLEE_OFFSET));
    }
    if (arg3 == MODE_MERGE) {
        *(s32 *)(arg1 + TAIL_OFFSET) += D_8006CC68;
        for (i = 0; i < BLOCK_WORDS; i++) {
            blk[BLOCK_OFFSET / 4 + i] |= ~*(s32 *)(arg1 + BLOCK_SRC_DELTA + BLOCK_OFFSET + i * 4) & D_8006B1C4[i];
        }
    } else {
        *(s32 *)(arg1 + TAIL_OFFSET) = D_8006CC68;
    }
    if (arg3 != MODE_MERGE) {
        for (i = 0; i < BLOCK_WORDS; i++) {
            blk[BLOCK_OFFSET / 4 + i] = D_8006B1C4[i];
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F34C);
#endif

#ifdef NON_MATCHING
#define NIBBLE_BITS 4
#define NIBBLE_MASK 0xF
#define NIBBLES_PER_WORD 8
#define WORDS_PER_ROW 2
#define COUNTER_MAX 0xF
#define COUNT_SHIFT 8
#define FLAG_CLEAR_2 (~2)
#define FLAG_SET_8 8
#define FLAG_CLEAR_1 (~1)
#define FLAG_TABLE_WORDS 0x400
#define SLOT_INDEX_SHIFT 8
#define SPECIAL_ARG_FIELD_BASE 0x3C
#define COPY_WORDS_END 0x20
#define COPY_DEST_OFFSET 0x60
#define COPY_STEP 0x10
#define INIT_ARG_FLAGS 0x0C01FFFF
#define STEP_AMOUNT 0x100
#define SCREEN_CALL_KIND_SPECIAL 0x1A
#define SCREEN_CALL_KIND_NORMAL 0x1B

extern u8 D_8006CB48[];
extern s32 D_8005F3B4;
extern s32 D_80060884;
extern s32 D_800608D0;
extern s32 D_80060944;
extern void *D_80063458;
extern s32 D_8006CB78;
extern s32 D_8006CB80;
extern s32 D_8006CC60;
void SetGeomScreen(s32);

void func_8002F6C8(u8 *arg0) {
    s32 sp18;
    s32 isSpecial;
    s32 i;
    s32 idx;
    u8 *obj;
    u8 *slot;
    u8 *t0;
    u8 *dst;
    u8 *src;
    u32 *word;
    s32 shift;

    isSpecial = arg0 == D_800619B8;
    idx = *(s32 *)(arg0 + 0x38);
    if (idx >= 0 && !isSpecial) {
        word = (u32 *)&D_80069718[*(s32 *)(arg0 + 0x30) * WORDS_PER_ROW + (idx >> 3)];
        shift = (idx % NIBBLES_PER_WORD) * NIBBLE_BITS;
        if (((*word >> shift) & NIBBLE_MASK) < COUNTER_MAX) {
            *word = *word + (1 << shift);
        }
        idx = *(s32 *)(arg0 + 0x38);
        D_8006CDB8 = ((D_80069718[*(s32 *)(arg0 + 0x30) * WORDS_PER_ROW + (idx >> 3)]
                       >> ((idx - (idx >> 3) * NIBBLES_PER_WORD) * NIBBLE_BITS)) & NIBBLE_MASK) << COUNT_SHIFT;
        D_8006CDB4 = *(s32 *)(arg0 + 0x38);
    } else {
        D_8006CDB8 = 0;
        D_8006CDB4 = 0;
    }
    D_8006CC60 = 0;
    func_8001D194(0, 0x1300, 0, 0, 0);
    if (D_800608CC != 0) {
        func_8001A014();
        func_8001A23C(D_800608CC, -1);
        obj = *(u8 **)(D_800608CC + 0x10);
        D_800608D0 = -1;
        for (i = 0; i < *(s32 *)(obj + 0x190); i++) {
            slot = obj + 0x194 + i * 4;
            if (func_800143C0(slot, 0) != 0) {
                func_8002001C(func_80014260(slot, 1, 0));
            }
        }
        func_80014790((u32 *)(arg0 + 0x1040));
        func_800163B4(D_80063458);
    }
    D_800608CC = 0;
    D_800608D4 = 0;
    SetGeomScreen(D_80060884);
    func_800241A8();
    for (i = 0; i < FLAG_TABLE_WORDS; i++) {
        D_8006B1C4[i] = *(s32 *)(arg0 + SPECIAL_ARG_FIELD_BASE + i * 4);
    }
    if (!isSpecial && D_8006CC84 > 0) {
        D_8006B1C4[(u32)D_8006CC84 >> SLOT_INDEX_SHIFT] &= FLAG_CLEAR_2;
        D_8006B1C4[(u32)D_8006CC84 >> SLOT_INDEX_SHIFT] |= FLAG_SET_8;
    }
    for (i = 0; i < FLAG_TABLE_WORDS; i++) {
        D_8006B1C4[i] &= FLAG_CLEAR_1;
    }
    func_8001F858(*(s32 *)(D_8006759C[0] + 8));
    sp18 = *(s32 *)(arg0 + 0x24);
    t0 = func_80014260(&sp18, 1, 1);
    {
        s32 a1 = *(s32 *)(t0 + (*(s32 *)(arg0 + 0x28) * 3 + *(s32 *)(*(u8 **)(t0 + 0x10) + 0x184)) * 4 + 0x10);
        s32 kind;
        if (isSpecial) {
            kind = SCREEN_CALL_KIND_SPECIAL;
        } else {
            kind = SCREEN_CALL_KIND_NORMAL;
        }
        func_800200A8((s32)t0, a1, *(s32 *)(arg0 + 0x2C), kind);
    }
    func_800143C0(&sp18, 1);
    if (D_8005F38C == 0) {
        func_80019800(D_8006CB48, 0, 0, 0, 0, 1);
    }
    src = arg0;
    dst = D_8005F38C + COPY_DEST_OFFSET;
    do {
        *(s32 *)(dst + 0) = *(s32 *)(src + 0);
        *(s32 *)(dst + 4) = *(s32 *)(src + 4);
        *(s32 *)(dst + 8) = *(s32 *)(src + 8);
        *(s32 *)(dst + 12) = *(s32 *)(src + 12);
        src += COPY_STEP;
        dst += COPY_STEP;
    } while (src != arg0 + COPY_WORDS_END);
    *(s32 *)dst = *(s32 *)src;
    {
        u8 *base = D_8005F38C;
        s32 v1 = *(s32 *)(base + 0xA8);
        s32 a0 = *(s32 *)(base + 0x70);
        u8 *a2 = *(u8 **)(base + 0x58);
        *(u8 **)(base + 0x18) = D_800608CC;
        *(s32 *)(base + 0x110) = 0;
        *(s32 *)(base + 0x84) = 0;
        *(s32 *)(base + 0x88) = 0;
        *(s32 *)(base + 0x8C) = 0;
        *(s32 *)(base + 0x104) = 0;
        *(s32 *)(base + 0xA8) = v1;
        *(s32 *)(base + 0x94) = a0;
        if (a2 != 0) {
            *(s32 *)(a2 + 0x58) = 0;
            *(s32 *)(base + 0x58) = 0;
        }
    }
    D_80060944 = 0;
    D_8006CB78 = 0;
    D_8005F3B4 = 0;
    D_8006CB80 = INIT_ARG_FLAGS;
    if (!isSpecial) {
        D_8006CB84 += STEP_AMOUNT;
        if (*(s32 *)(arg0 + 0x34) != 0) {
            D_8006CD20 = 0;
        } else {
            D_8006CD20 += STEP_AMOUNT;
        }
    }
    func_8002F228(0);
    D_8006CC68 = *(s32 *)(arg0 + 0x103C);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F6C8);
#endif
