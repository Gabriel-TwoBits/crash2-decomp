#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005C4B4[];
extern u8 D_8005C4B5[];

void func_8003516C(u8 *arg0) {
    s32 var_a1;
    s32 var_t1;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;
    u32 temp_v1;
    u8 *var_a0;
    u8 *var_t2;
    u8 temp_a1;
    u8 temp_a3;
    u8 var_a2;

    var_a0 = arg0 + 4;
    var_t2 = var_a0;
    var_t1 = 0;
loop_1:
    temp_a1 = var_a0[0];
    temp_a3 = var_a0[1];
    temp_v1 = temp_a1 & 0xFF;
    var_a2 = temp_a1;
    if (temp_v1 < 0x81U) {
        var_a0 -= 1;
        goto block_20;
    }
    switch (temp_v1) {
    case 0xA0:
        var_a2 = 0x20;
        var_a0 -= 1;
block_20:
        var_v0 = var_a2 & 0xFF;
        break;
    case 0x81:
        var_a2 = 0x2E;
        if (D_8005C4B4[0] != 0xFF) {
            var_a1 = 0;
            var_v1 = 0;
loop_8:
            if (D_8005C4B5[var_v1] != (temp_a3 & 0xFF)) {
                var_a1 = var_v1 + 2;
                var_v1 = var_a1;
                if (D_8005C4B4[var_v1] == 0xFF) {
                    var_v0 = 0x2E & 0xFF;
                } else {
                    goto loop_8;
                }
            } else {
                var_a2 = D_8005C4B4[var_a1];
                var_v0 = var_a2 & 0xFF;
            }
        } else {
            goto block_20;
        }
        break;
    case 0x82:
        var_a2 = 0x3F;
        if (((u32) (temp_a3 - 0x60) < 0x1AU) || (var_v0_2 = temp_a3 + 0x7F, (((u32) (temp_a3 - 0x4F) < 0xAU) != 0))) {
            var_a2 = temp_a3 - 0x1F;
            var_v0_2 = temp_a3 + 0x7F;
        }
        var_v0 = var_a2 & 0xFF;
        if ((u32) (var_v0_2 & 0xFF) < 0x1AU) {
            var_a2 = temp_a3 - 0x20;
            goto block_20;
        }
        break;
    default:
        var_a2 = 0x3F;
        if ((u32) ((temp_a1 + 0x5F) & 0xFF) < 0x3FU) {
            var_a0 -= 1;
        }
        goto block_20;
    }
    if ((var_v0 != 0) && ((u32) ((var_a2 - 0x20) & 0xFF) >= 0x5FU)) {
        var_a2 = 0x20;
    }
    *var_t2 = var_a2;
    var_t2 += 1;
    if (var_a2 & 0xFF) {
        var_t1 += 2;
        var_a0 += 2;
        if (var_t1 < 0x40) {
            goto loop_1;
        }
    }
}
