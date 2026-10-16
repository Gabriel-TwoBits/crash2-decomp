#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80039B14(struct Obj *arg0, u32 arg1) {
    s32 *var_s1;
    s32 *var_v0;
    s32 temp_a0_2;
    s32 temp_a1;
    u32 temp_a0;

    temp_a0 = arg1 >> 12;
    temp_a1 = temp_a0 & 0xFFF;
    if ((temp_a0 & 0xE00) == 0xE00) {
        if (temp_a1 == 0xE1F) {
            var_s1 = *(s32 **)((char *)arg0 + 0xBC) - 1;
            *(s32 **)((char *)arg0 + 0xBC) = var_s1;
        } else {
            var_s1 = (s32 *)((char *)arg0 + ((temp_a0 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_s1 = func_80036D0C(arg0, temp_a1);
    }
    temp_a0_2 = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a0_2 == 0xE1F) {
            var_v0 = *(s32 **)((char *)arg0 + 0xBC) - 1;
            *(s32 **)((char *)arg0 + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)((char *)arg0 + ((arg1 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_v0 = func_80036D0C(arg0, temp_a0_2);
    }
    *(s32 *)((char *)arg0 + 0xD4) = func_800270C4((s32 *) *var_v0, *var_s1);
}
