#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001557C(s32);

u32 func_80032A98(s32 *arg0, s32 *arg1) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_a0;
    s32 var_v0_2;
    u32 var_a0_2;
    u32 var_v0;

    var_a0 = *arg1;
    if (!(var_a0 & 1)) {
        var_a0 = ((s32 *)var_a0)[1];
    }
    temp_v0 = func_8001557C(var_a0);
    arg0[0] = arg0[0] & ~0x7000;
    arg0[3] = *arg1;
    arg0[5] = temp_v0;
    if (temp_v0 != 0) {
        var_a0_2 = 0;
        var_v0_2 = (arg0[0] & ~0x7000) | 0xA000;
        goto block_10;
    }
    temp_v0_2 = (s32)func_80014260(arg1, 0, 1);
    if (temp_v0_2 != 0) {
        if (temp_v0_2 >= -0xFE) {
            return -0xE;
        }
        if ((temp_v0_2 != 0) && ((temp_v0_2 & 0xC0000002) == 0xC0000002)) {
            var_a0_2 = (u32) (temp_v0_2 * 4) >> 4;
        } else {
            goto block_9;
        }
        goto block_11;
    } else {
block_9:
        var_a0_2 = 0;
        var_v0_2 = (arg0[0] & ~0x7000) | 0x9000;
block_10:
        arg0[0] = var_v0_2;
block_11:
        var_v0 = var_a0_2;
    }
    return var_v0;
}
