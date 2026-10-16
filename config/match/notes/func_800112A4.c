#include "common.h"
#include "sdk.h"
#include "game.h"

#define var_a0 arg0
extern u8 *D_8005F2A4;
extern u8 *D_8005F2B4;

void *func_800112A4(void *arg0, s32 arg1) {
    s32 *temp_v0_3;
    s32 temp_v0;
    s32 temp_v0_2;
    u32 *temp_t1;
    u32 *var_a3;
    u32 temp_a1;
    u32 temp_a2_2;
    u32 temp_v1;
    u32 temp_v1_2;
    u32 var_t0;
    u32 var_v0_2;
    u32 var_v1;
    void *temp_a2;
    void *temp_v0_4;
    void *var_v0;

    temp_a1 = (arg1 + 3) & ~3;
    temp_v0 = *(s32 *)((u8 *)var_a0 - 4);
    temp_t1 = (u32 *)((u8 *)var_a0 - 4);
    if (temp_v0 == -2) {
        return 0;
    }
    temp_v1 = temp_v0 & ~1;
    if (temp_v1 < temp_a1) {
        var_a3 = temp_t1;
        var_t0 = temp_v1;
loop_4:
        temp_a2 = (u8 *)var_a3 + (((u32) *var_a3 >> 2) * 4);
        temp_v0_2 = *(s32 *)((u8 *)temp_a2 + 4);
        var_a3 = (u32 *)((u8 *)temp_a2 + 4);
        if (temp_v0_2 != -2) {
            if (temp_v0_2 & 1) {
                goto block_9;
            }
        } else {
            temp_v1_2 = temp_a1 - var_t0;
            if ((u32) (((s32) (D_8005F2B4 - (u8 *)var_a3) >> 2) - 4) >= temp_v1_2) {
                *(s32 *)((u8 *)temp_a2 + 4) = (s32) (temp_v1_2 | 1);
                temp_v0_3 = (s32 *)((u8 *)var_a3 + ((temp_v1_2 >> 2) * 4));
                D_8005F2A4 = (u8 *)temp_v0_3;
                *temp_v0_3 = -2;
                D_8005F2A4 += 4;
block_9:
                *temp_t1 += *var_a3 & ~1;
                var_t0 += *var_a3 & ~1;
                if (var_t0 < temp_a1) {
                    goto loop_4;
                }
            }
        }
    }
    temp_a2_2 = *temp_t1;
    if (temp_a1 < (u32) (temp_a2_2 & ~3)) {
        var_v1 = temp_a1;
        if ((s32) temp_a1 < 0) {
            var_v1 = temp_a1 + 3;
        }
        *(s32 *)((u8 *)temp_t1 + (((s32) var_v1 >> 2) * 4) + 4) = (s32) (((temp_a2_2 & ~1) - temp_a1) - 4);
        var_v0_2 = temp_a1;
        if ((s32) temp_a1 < 0) {
            var_v0_2 = temp_a1 + 3;
        }
        temp_v0_4 = (u8 *)temp_t1 + (((s32) var_v0_2 >> 2) * 4);
        *(s32 *)((u8 *)temp_v0_4 + 4) = (s32) (*(s32 *)((u8 *)temp_v0_4 + 4) | 1);
        *temp_t1 = temp_a1 | (*temp_t1 & 1);
        return arg0;
    }
    if ((u32) (temp_a2_2 & ~1) < temp_a1) {
        return 0;
    }
    return arg0;
}
