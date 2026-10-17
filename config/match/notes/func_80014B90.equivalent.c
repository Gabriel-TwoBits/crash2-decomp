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
    s32 p;
    s32 count;
    s32 base;

    p = *(s32 *)((u8 *)D_80067590 + (((u32)arg0 >> TABLE_INDEX_SHIFT) & TABLE_INDEX_MASK));
    if (((Unk80014B44 *)p)->unk4 == (u32)arg0) {
        return p;
    }
    count = *(s32 *)(D_800675A0 + COUNT_OFFSET);
    base = D_80067594;
    p += sizeof(Unk80014B44);
    while (((p - base) >> ENTRY_SIZE_SHIFT) < count) {
        if (((Unk80014B44 *)p)->unk4 == (u32)arg0) {
            return p;
        }
        p += sizeof(Unk80014B44);
    }
    return NOT_FOUND;
}
