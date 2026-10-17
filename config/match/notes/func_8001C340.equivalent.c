#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk8001C340Entry {
    s32 unk_0;
    s32 unk_4;
    u16 unk_8;
    u16 unk_A;
    u16 unk_C;
    u16 unk_E;
} Unk8001C340Entry;

#define FIELD(T, o) (*(T *)((char *)arg0 + (o)))

#define ENTRY_STRIDE 0x10
#define OFFSET_NONE 0x3FFF
#define OFFSET_MASK 0x3FFF
#define OFFSET_FROM_OWN_BASE 0x4000
#define BLOCK_BASE_OFFSET 0x14
#define STACK_BASE_OFFSET 0x40
#define FLAG_BLOCKED 0x10000000
#define FLAGS_STARTED 0x20020
#define ARG_MASK 0xFFFF
#define RESULT_BAD_ENTRY -0xE
#define RESULT_FAIL -0xFF
#define RESULT_MIN_OK -0xFE
#define ENTRY_INDEX_NONE 0xFF
#define CMD_RESUME 0x13
#define CMD_START 3

extern s32 D_8006CB60;
extern u32 D_8006CB64;

s32 func_8001C340(void *arg0, s32 arg1, s32 arg2, s32 *arg3) {
    s32 prev_d0;
    Unk8001C340Entry *entry;
    char *own;
    u16 word;
    s32 r;
    char *base;
    s32 i;
    s32 *sp;

    prev_d0 = FIELD(s32, 0xD0);
    if (arg1 == ENTRY_INDEX_NONE || (FIELD(s32, 0xAC) & FLAG_BLOCKED)) {
        return RESULT_BAD_ENTRY;
    }
    FIELD(s32, 0x1C) = arg1;
    entry = (Unk8001C340Entry *)(*(char **)(*(char **)((char *)arg0 + 0xC) + 0x20) + arg1 * ENTRY_STRIDE);
    FIELD(s32, 0xB0) = entry->unk_4;
    own = func_80014260(*(char **)(*(char **)((char *)arg0 + 0x10) + 0x18) + entry->unk_8 * 4, 1, 0);
    FIELD(void *, 0x14) = own;

    word = entry->unk_E;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            r = *(s32 *)(own + BLOCK_BASE_OFFSET);
        } else {
            r = *(s32 *)(FIELD(char *, 0xC) + BLOCK_BASE_OFFSET);
        }
        r += (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xC0) = r;

    word = entry->unk_A;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = FIELD(char *, 0x14);
        } else {
            base = FIELD(char *, 0xC);
        }
        r = *(s32 *)(base + BLOCK_BASE_OFFSET) + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xCC) = r;

    word = entry->unk_C;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = FIELD(char *, 0x14);
        } else {
            base = FIELD(char *, 0xC);
        }
        r = *(s32 *)(base + BLOCK_BASE_OFFSET) + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xC8) = r;

    FIELD(s32, 0xD0) = 0;
    FIELD(char *, 0xC4) = (char *)arg0 + STACK_BASE_OFFSET;
    FIELD(char *, 0xBC) = (char *)arg0 + STACK_BASE_OFFSET + (*(s32 *)(*(char **)(*(char **)((char *)arg0 + 0xC) + 0x10) + 0xC)) * 4;
    FIELD(s32, 0xA8) |= FLAGS_STARTED;
    FIELD(s32, 0x100) = entry->unk_0;
    for (i = 0; i < arg2; i++) {
        sp = FIELD(s32 *, 0xBC);
        FIELD(s32 *, 0xBC) = sp + 1;
        *sp = *arg3++;
    }
    func_8001C620(arg0, arg2, ARG_MASK);
    if (prev_d0 != 0) {
        func_8001C620(arg0, 0, ARG_MASK);
        FIELD(s32, 0xC0) = prev_d0;
        i = func_8003A014(arg0, CMD_RESUME, 0);
        if (i >= RESULT_MIN_OK) {
            return i;
        }
    } else {
        sp = FIELD(s32 *, 0xBC);
        FIELD(s32 *, 0xBC) = sp + 1;
        *sp = 0;
    }
    FIELD(u32, 0xE0) = D_8006CB64;
    if (FIELD(s32, 0xC8) != 0) {
        if ((s32)arg0 == D_8006CB60) {
            func_8001C620(arg0, 0, ARG_MASK);
            FIELD(s32, 0xC0) = FIELD(s32, 0xC8);
            i = func_8003A014(arg0, CMD_START, 0);
            if (i >= RESULT_MIN_OK) {
                return i;
            }
        } else {
            return RESULT_FAIL;
        }
    }
    return RESULT_FAIL;
}
