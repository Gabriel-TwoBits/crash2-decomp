#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk800271D8List {
    s32 pad0;
    s32 pad4;
    s32 pad8;
    s32 count;
    s32 *items[1];
} Unk800271D8List;

typedef struct Unk800271D8 {
    s32 pos;
    u8 pad4[0x10];
    s32 index;
    u8 pad18[0x38];
    Unk800271D8List *list;
} Unk800271D8;

s32 func_800272A4(Unk800271D8 *, u8 *);

s32 func_800271D8(Unk800271D8 *arg0, s32 arg1) {
    s32 result;
    s32 count;
    u8 *item;
    s32 index;

    result = 0;
    count = arg0->list->count;
    while (arg1 > 0) {
        index = arg0->index;
        if (index >= count) {
            break;
        }
        item = (u8 *)arg0->list->items[index];
        arg0->index = index + 1;
        result |= func_800272A4(arg0, item);
        if ((*item >> 6) == 2) {
            arg1--;
            arg0->pos += 0x100;
        }
    }
    return result;
}
