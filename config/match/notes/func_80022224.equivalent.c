#include "common.h"
#include "sdk.h"
#include "game.h"

#define UNK_CALL_ARG 0x173
#define UNK_CALL_MODE 2
#define SLOT_STRIDE_WORDS 3
#define SUBTABLE_OFFSET 0x10
#define BASE_INDEX_OFFSET 0x184
#define ENTRY_DEFAULT_OFFSET 0x14
#define ENTRY_VARIANT_OFFSET 0x10
#define VARIANT_DEFAULT 1

extern u8 *D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608D8[];
extern s32 D_800608DC;
extern s32 D_800608FC;

void func_80022224(s32 arg0) {
    s32 slot;
    s32 value;
    u8 *table;

    D_800608FC = arg0;
    if (D_800608D4 == 0) {
        return;
    }
    table = D_800608CC;
    if (table == 0) {
        return;
    }
    slot = func_800321D4(D_800608D4, UNK_CALL_ARG, 0, 0, 0, UNK_CALL_MODE, 0) * SLOT_STRIDE_WORDS;
    table = D_800608CC;
    value = *(s32 *)(table + (slot + *(s32 *)(*(u8 **)(table + SUBTABLE_OFFSET) + BASE_INDEX_OFFSET)) * 4 + ENTRY_DEFAULT_OFFSET);
    D_800608D8[0] = value;
    if (arg0 == VARIANT_DEFAULT) {
        D_800608DC = value;
    } else {
        D_800608DC = *(s32 *)(table + (slot + arg0 + *(s32 *)(*(u8 **)(table + SUBTABLE_OFFSET) + BASE_INDEX_OFFSET)) * 4 + ENTRY_VARIANT_OFFSET);
    }
}
