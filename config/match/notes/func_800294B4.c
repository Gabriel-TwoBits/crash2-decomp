#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800321D4(s32, s32, s32, s32, s32, s32, s32);

void func_800294B4(s16 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 var_a1;

    temp_a0 = *(s32 *)((u8 *)arg0 + 0xF0);
    if (temp_a0 != 0) {
        temp_v0 = func_800321D4(temp_a0, 0xF2, 0, 0, 0, 2, 0);
        if ((temp_v0 & 0xFFFF) != 0xFFFF) {
            var_a1 = (u32)(temp_v0 & 0x3F0) >> 4;
            if (var_a1 - 0x30 >= 0x10U) {
                var_a1 = -1;
            }
            func_800289C4(arg0, var_a1, 0);
        }
    }
}
