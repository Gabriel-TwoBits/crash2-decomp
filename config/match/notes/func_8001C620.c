#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001C620(u8 *arg0, s32 arg1, s32 arg2) {
    u8 *cur;
    s32 prev;
    s32 *p;

    cur = *(u8 **)(arg0 + 0xBC);
    p = *(s32 **)(arg0 + 0xBC);
    prev = *(s32 *)(arg0 + 0xC4);
    *(u8 **)(arg0 + 0xC4) = cur;
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = arg2;
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = *(s32 *)(arg0 + 0xC0);
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = ((cur - arg1 * 4 - 0x40) - arg0) | ((prev - 0x40 - (s32)arg0) << 16);
    return -0xFF;
}
