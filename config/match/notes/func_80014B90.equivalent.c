#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80014B44 {
    s32 unk0;
    u32 unk4;
} Unk80014B44;

#define TABLE_INDEX_SHIFT 13
#define TABLE_INDEX_MASK 0x3FC
#define COUNT_OFFSET 0x404
#define ENTRY_SIZE_SHIFT 3
#define NOT_FOUND -10

extern u8 *D_800675A0;
extern Unk80014B44 **D_80067590;
extern Unk80014B44 *D_80067594;

Unk80014B44 *func_80014B90(u32 arg0) {
    Unk80014B44 *p;
    s32 count;

    p = *(Unk80014B44 **)((u8 *)D_80067590 + ((arg0 >> TABLE_INDEX_SHIFT) & TABLE_INDEX_MASK));
    if (p->unk4 == arg0) {
        return p;
    }
    count = *(s32 *)(D_800675A0 + COUNT_OFFSET);
    p++;
    while (((s32)((u8 *)p - (u8 *)D_80067594) >> ENTRY_SIZE_SHIFT) < count) {
        if (p->unk4 == arg0) {
            return p;
        }
        p++;
    }
    return (Unk80014B44 *)NOT_FOUND;
}
