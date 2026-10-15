#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001C688(u8 *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = *(u32 **)(arg0 + 0xC4);
    *(u32 **)(arg0 + 0xBC) = v1 + 2;
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    *(u32 **)(arg0 + 0xBC) = v1 + 1;
    *(u32 *)(arg0 + 0xC0) = v1[1];
    v1b = *(u32 * volatile *)(arg0 + 0xBC);
    *(u32 **)(arg0 + 0xBC) = v1b - 1;
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    *(u8 **)(arg0 + 0xBC) = &arg0[(temp_a3 & 0xFFFF) + 0x40];
    *(u8 **)(arg0 + 0xC4) = &arg0[temp_t0 + 0x40];
    return -0xFF;
}
