#include "common.h"
#include "sdk.h"
#include "game.h"

s32 Gool_PopFrame(GoolObject *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = (u32 *)arg0->fp;
    arg0->sp = (s32 *)(v1 + 2);
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    arg0->sp = (s32 *)(v1 + 1);
    arg0->pc = v1[1];
    v1b = *(u32 * volatile *)&arg0->sp;
    arg0->sp = (s32 *)(v1b - 1);
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    arg0->sp = (s32 *)((u32)arg0 + (temp_a3 & 0xFFFF) + 0x40);
    arg0->fp = (u8 *)((u32)arg0 + temp_t0 + 0x40);
    return -0xFF;
}
