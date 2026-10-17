#include "common.h"
#include "sdk.h"
#include "game.h"

#define TABLE_INDEX_SHIFT 13
#define TABLE_INDEX_MASK 0x3FC
#define COUNT_OFFSET 0x404
#define ENTRY_SIZE_SHIFT 3
#define NOT_FOUND -10

extern s32 D_80067594;

s32 func_80014B90(s32 arg0, s32 arg1) {
    s32 *p;
    s32 count;
    s32 base;

    p = *(s32 **)((u8 *)D_80067590 + (((u32)arg0 >> TABLE_INDEX_SHIFT) & TABLE_INDEX_MASK));
    if (p[1] == arg0) {
        return (s32)p;
    }
    count = *(s32 *)(D_800675A0 + COUNT_OFFSET);
    base = D_80067594;
    p += 2;
    while ((((s32)p - base) >> ENTRY_SIZE_SHIFT) < count) {
        p += 2;
        if (p[-1] == arg0) {
            return (s32)(p - 2);
        }
    }
    return NOT_FOUND;
}
