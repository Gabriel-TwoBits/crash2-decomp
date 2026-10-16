#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIELD(p, o) (*(s32 *)((s8 *)(p) + (o)))

void func_80031340(void *arg0, void *arg1, s32 arg2) {
    s32 temp_s0_2;
    s32 temp_s1;
    s32 temp_s1_2;
    u32 mask;
    s32 temp_s4;
    s32 temp_s5;
    s32 temp_v1;
    s32 var_v0_2;
    s32 var_v1;
    u32 var_v0;

    mask = 0xFFF;
    temp_s5 = -FIELD(arg1, 0);
    temp_s4 = -FIELD(arg1, 8);
    temp_s1 = ratan2(temp_s5, temp_s4) & mask;
    temp_v1 = temp_s1 - (arg2 & 0xFFF);
    if (temp_v1 >= 0x801) {
        var_v1 = temp_v1 - 0x1000;
        goto block_4;
    }
    var_v0 = temp_v1 + 0x400;
    if (temp_v1 < -0x800) {
        var_v1 = temp_v1 + 0x1000;
block_4:
        var_v0 = var_v1 + 0x400;
    }
    FIELD(arg0, 4) = temp_s1;
    if (var_v0 < 0x801U) {
        temp_s1_2 = -temp_s1;
        temp_s0_2 = ((s32)((temp_s4 >> 8) * func_8003D360(temp_s1_2)) >> 4) - ((s32)((temp_s5 >> 8) * func_8003D2EC(temp_s1_2)) >> 4);
        func_8003D2EC(temp_s1_2);
        func_8003D360(temp_s1_2);
        var_v0_2 = ratan2(FIELD(arg1, 4), temp_s0_2) & mask;
    } else {
        var_v0_2 = -0x400;
    }
    if (temp_s4) {
        FIELD(arg0, 0) = var_v0_2;
        FIELD(arg0, 8) = 0;
    } else {
        FIELD(arg0, 0) = var_v0_2;
        FIELD(arg0, 8) = 0;
    }
}
