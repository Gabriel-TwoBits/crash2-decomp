#include "common.h"
#include "sdk.h"
#include "game.h"

#define OBJ_STATE_ACTIVE 1
#define OBJ_MODE_LOADED 3
#define COPY_BLOCK_WORDS 9
#define BLOCK_OFFSET_WORDS (0x60 / 4)
#define ENTRY_TABLE_OFFSET 0x10
#define ENTRY_INVALID 0x6396347F
#define ENTRY_COUNT 0x40
#define RESULT_OK_LIMIT (-0xFE)
#define RESULT_FAIL (-0xFF)
#define RESULT_BAD_ENTRY (-0xE)
#define RESULT_NO_FRAME (-0x1B)
#define FRAME_NONE 0xFF
#define SCALE_ONE 0x1000
#define NO_ANIM 0xFFFF
#define ANIM_FIRST 4
#define ANIM_LAST_EXCL 6
#define ANIM_EXTRA 0x1D

extern u8 *D_8006759C;

#define W(base, off) (((s32 *)(base))[(off) / 4])

s32 func_80019988(s32 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 *parent;
    s32 *entry;
    void *res;
    s32 *table;
    u16 frame;
    s32 i;

    parent = (s32 *)W(arg0, 0x44);
    W(arg0, 0x138) = NO_ANIM;
    W(arg0, 0xB8) = 0;
    W(arg0, 0xF0) = 0;
    W(arg0, 0xF4) = 0;
    W(arg0, 0xF8) = 0;
    W(arg0, 0) = 1;
    W(arg0, 0x84) = 0;
    W(arg0, 0x88) = 0;
    W(arg0, 0x8C) = 0;
    W(arg0, 0x90) = 0;
    W(arg0, 0x94) = 0;
    W(arg0, 0x98) = 0;
    W(arg0, 0x9C) = 0;
    W(arg0, 0xA0) = 0;
    W(arg0, 0xA4) = 0;
    W(arg0, 0x104) = 0;
    W(arg0, 0xEC) = 0;
    W(arg0, 0xD8) = 0;
    W(arg0, 0x94) = 0;
    W(arg0, 0xA8) = 0;
    W(arg0, 0xB0) = 0;
    W(arg0, 0xAC) = 0;
    W(arg0, 0x108) = 0;
    W(arg0, 0x118) = 0;
    W(arg0, 0x110) = 0;
    W(arg0, 0x128) = 0;
    W(arg0, 0x12C) = -2;
    W(arg0, 0x120) = 0;
    if (parent[0] == OBJ_STATE_ACTIVE) {
        parent = (s32 *)W(arg0, 0x44);
        W(arg0, 0x18) = parent[0x18 / 4];
        for (i = 0; i < COPY_BLOCK_WORDS; i++) {
            arg0[BLOCK_OFFSET_WORDS + i] = parent[BLOCK_OFFSET_WORDS + i];
        }
    } else {
        W(arg0, 0x18) = 0;
        W(arg0, 0x6C) = 0;
        W(arg0, 0x70) = 0;
        W(arg0, 0x74) = 0;
        W(arg0, 0x78) = SCALE_ONE;
        W(arg0, 0x7C) = SCALE_ONE;
        W(arg0, 0x80) = SCALE_ONE;
    }
    if (arg1 >= ANIM_FIRST && (arg1 < ANIM_LAST_EXCL || arg1 == ANIM_EXTRA)) {
        W(arg0, 0x18) = 0;
    }
    if (W(arg0, 4) != OBJ_MODE_LOADED) {
        return RESULT_BAD_ENTRY;
    }
    entry = (s32 *)(D_8006759C + arg1 * 4 + ENTRY_TABLE_OFFSET);
    if (*entry == ENTRY_INVALID || arg1 >= ENTRY_COUNT) {
        return RESULT_BAD_ENTRY;
    }
    res = func_80014260(entry, 1, 0);
    W(arg0, 0xC) = (s32)res;
    W(arg0, 0x10) = (s32)res;
    if (W(arg0, 0xC) >= RESULT_OK_LIMIT) {
        return RESULT_BAD_ENTRY;
    }
    W(arg0, 0x40) = (s32)arg0;
    W(arg0, 0x58) = 0;
    W(arg0, 0x50) = 0;
    W(arg0, 0x5C) = 0;
    table = (s32 *)W(arg0, 0xC);
    W(arg0, 0xB4) = arg2;
    W(arg0, 0xE8) = 0;
    W(arg0, 0xD0) = 0;
    W(arg0, 0x54) = (s32)D_8006CB10[0];
    frame = *(u16 *)(W(table, 0x1C) + (W((s32 *)W(table, 0x10), 0x10) + W(arg0, 0xB4)) * 2);
    if (frame == FRAME_NONE) {
        return RESULT_NO_FRAME;
    }
    if (func_8001C340(arg0, frame, arg3, (s32 *)arg4) < RESULT_OK_LIMIT) {
        return RESULT_FAIL;
    }
    return RESULT_BAD_ENTRY;
}
