#include "common.h"
#include "sdk.h"
#include "game.h"

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
