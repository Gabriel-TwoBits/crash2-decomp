#include "common.h"
#include "sdk.h"
#include "game.h"

#define OFFSET_PARENT 0xC
#define OFFSET_TABLE 0x14
#define OFFSET_WRITE_PTR 0xBC
#define OFFSET_TARGET_PTR 0xC0
#define INDEX_MASK 0x3FFF
#define ARG_ALL_BITS 0xFFFF
#define MODE_THREE 3
#define BYTES_PER_ENTRY 4

s32 func_8003A014(void *, s32, s32);

s32 func_8001D22C(void *arg0, u16 arg1, s32 arg2, s32 *arg3) {
    s32 i;
    s32 **write_ptr;
    s32 *slot;
    u8 *base;

    base = (u8 *)arg0;
    write_ptr = (s32 **)(base + OFFSET_WRITE_PTR);
    for (i = 0; i < arg2; i++) {
        slot = *write_ptr;
        *write_ptr = slot + 1;
        *slot = *arg3;
        arg3++;
    }
    func_8001C620(base, arg2, ARG_ALL_BITS);
    *(s32 *)(base + OFFSET_TARGET_PTR) =
        *(s32 *)(*(u8 **)(base + OFFSET_PARENT) + OFFSET_TABLE) + (arg1 & INDEX_MASK) * BYTES_PER_ENTRY;
    return func_8003A014(arg0, MODE_THREE, 0);
}
