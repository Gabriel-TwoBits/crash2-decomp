#include "common.h"
#include "sdk.h"
#include "game.h"

#define ROW_STRIDE 65424
#define ROW_STRIDE_ROUND 65423
#define ROW_BASE 0x80000
#define ROW_SNAP 16
#define ROW_SNAP_ROUND 15
#define FILL_COUNT 10
#define FILL_MIN_Y 0x1010
#define FILL_DROP 0x10000
#define KIND_FIRST 4
#define KIND_EXTRA 5
#define KIND_FILL 3
#define MODE_ACTIVE 1
#define MODE_SKIPPED 0x1E
#define EXTRA_VALUE 0x6396347F

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 unk_0C[2];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C[4];
} Unk80015344;

extern Unk80015344 D_80062BB8[];
extern s32 D_80067FB8;

void func_80015344(s32 unused, s32 y, s32 total, s32 step, s32 count) {
    s32 i;
    s32 rows;
    s32 last;
    s32 end;
    s32 saved;
    s32 next;
    s32 rem;

    y = ROW_BASE - y;
    rows = (total + ROW_STRIDE_ROUND) / ROW_STRIDE;
    last = rows - 1;
    rem = total % ROW_STRIDE;
    for (i = last; i >= 0; i--) {
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_00 = MODE_ACTIVE;
        D_80062BB8[i].unk_02 = KIND_FIRST;
        if (i == last) {
            y -= ((rem + ROW_SNAP_ROUND) / ROW_SNAP) * ROW_SNAP;
        } else {
            y -= ROW_STRIDE;
        }
        D_80062BB8[i].unk_18 = y;
    }
    D_80067FB8 = y;
    saved = y;
    i = rows;
    if (count < 0) {
        end = rows - count;
    } else {
        end = rows + count;
    }
    for (; i < end; i++) {
        y -= step;
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_00 = MODE_ACTIVE;
        D_80062BB8[i].unk_02 = KIND_EXTRA;
        D_80062BB8[i].unk_14 = EXTRA_VALUE;
        D_80062BB8[i].unk_18 = y;
    }
    if (count < 0) {
        y = saved;
    } else {
        /* y carries on from the last extra row */
    }
    for (; i < FILL_COUNT; i++) {
        next = y - FILL_DROP;
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_02 = KIND_FILL;
        if (next < FILL_MIN_Y) {
            D_80062BB8[i].unk_00 = MODE_SKIPPED;
        } else {
            y = next;
            D_80062BB8[i].unk_00 = MODE_ACTIVE;
            D_80062BB8[i].unk_18 = y;
        }
    }
}
