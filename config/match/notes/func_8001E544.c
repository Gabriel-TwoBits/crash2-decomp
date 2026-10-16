#include "common.h"
#include "sdk.h"
#include "game.h"

extern u32 *D_80063450;

s32 func_8001E544(s32 arg0, s32 arg1, s32 arg2, void *arg3) {
    s32 temp_v1;
    s32 var_a0;
    s32 var_a2;
    s32 var_t0;
    s32 var_v0;
    u32 speed;

    speed = D_80063450[5];
    if (speed >= 0x67U) {
        speed = 0x66;
    }
    var_a2 = (s32) (arg2 * speed) / 1024;
    temp_v1 = (s32) ((arg1 - arg0) << 20) >> 20;
    if (temp_v1 != 0) {
        var_t0 = temp_v1;
        if (temp_v1 < 0) {
            var_t0 = -var_t0;
        }
        if (var_a2 >= 0) {
            var_v0 = arg0 + var_a2;
            if (var_t0 >= var_a2) {
                goto block_14;
            }
            goto block_11;
        }
        var_v0 = arg0 + var_a2;
        if (var_t0 < -var_a2) {
block_11:
            var_v0 = arg0 + var_a2;
            if ((var_a2 ^ temp_v1) >= 0) {
                goto block_12;
            }
            goto block_14;
        }
block_14:
        var_a0 = var_v0 & 0xFFF;
    } else {
block_12:
        var_a0 = arg1;
        if (arg3 != 0) {
            *(s32 *) ((char *) arg3 + 0xA8) |= 0x800;
        }
    }
    return var_a0;
}
