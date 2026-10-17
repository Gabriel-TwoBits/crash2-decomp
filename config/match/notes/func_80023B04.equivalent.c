#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Arg80023B04;

extern s32 D_800608D4[];
extern s32 D_800608F0;
extern s32 D_800608D8;
extern s32 D_800608DC;
extern s32 D_800608FC;
extern s32 D_8005BA28[];

#define KEY_QUERY 0x29
#define IDX_QUERY 0x173
#define LOOKUP_ID 0x142
#define QUERY_FLAGS 2
#define LOOKUP_FLAGS 3
#define KEY_COUNT 11
#define ALT_TABLE_WORDS 6
#define BLEND_WORD 3
#define BLEND_FULL 0x400
#define BLEND_SHIFT 10
#define FIXED_SHIFT 8
#define ENTRY_WORDS_PER_IDX 3
#define OBJ_CTX_OFFSET 0x10
#define CTX_BASE_OFFSET 0x184
#define ENTRY_PRIMARY_OFFSET 0x10
#define ENTRY_FALLBACK_OFFSET 0x14

void func_80023B04(Arg80023B04 *arg0, s32 *arg1, s32 *arg2) {
    s32 sp28;
    s32 key;
    s32 *tbl;
    s32 idx;
    s16 *p;
    s32 x, y, w, x1, y1;
    u8 *obj;
    s32 arg;
    s32 unk8;

    idx = 0;
    if (arg0->unk4 == D_800608D4[0]) {
        key = D_800608F0;
        arg = D_800608DC;
    } else {
        key = func_800321D4(arg0->unk4, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        idx = func_800321D4(arg0->unk4, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        obj = (u8 *)arg0->unk0;
        arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + D_800608FC + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_PRIMARY_OFFSET);
    }
    tbl = D_8005BA28;
    unk8 = arg0->unk8;
    if (key == 0) {
        tbl = D_8005BA28 + ALT_TABLE_WORDS;
    }
    p = (s16 *)func_80031AE8(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, &sp28, 0, 0);
    if (p == 0) {
        if (arg0->unk4 == D_800608D4[0]) {
            arg = D_800608D8;
        } else {
            obj = (u8 *)arg0->unk0;
            arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_FALLBACK_OFFSET);
        }
        p = (s16 *)func_80031AE8(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, &sp28, 0, 0);
    }
    switch (key) {
    case 0:
    case 3:
    case 4:
    case 10:
        w = tbl[BLEND_WORD];
        x = p[0] << FIXED_SHIFT;
        y = p[1] << FIXED_SHIFT;
        if (w != 0) {
            x1 = p[2] << FIXED_SHIFT;
            y1 = p[3] << FIXED_SHIFT;
            if (w == BLEND_FULL) {
                x = x1;
                y = y1;
            } else {
                x += ((x1 - x) * w) >> BLEND_SHIFT;
                y += ((y1 - y) * w) >> BLEND_SHIFT;
            }
        }
        break;
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        x = p[0] << FIXED_SHIFT;
        y = p[1] << FIXED_SHIFT;
        break;
    }
    *arg1 = x;
    *arg2 = y;
}
