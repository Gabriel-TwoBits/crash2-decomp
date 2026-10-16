#include "common.h"
#include "sdk.h"
#include "game.h"

extern void *D_800608CC;
extern void *D_8006759C;
extern s32 D_8006CB64;
extern s32 D_8006CD48;

s32 func_8002BF58(void *arg0, void *arg1) {
    s32 sp18;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_a2;
    u32 temp_v1;
    s8 *new_var;
    void *temp_a1;
    void *temp_s2;

    temp_v0 = func_80027DEC(D_800608CC);
    if (temp_v0 >= -0xFE) {
        temp_a1 = *(void **)((s8 *)arg0 + 0x18);
        temp_s2 = *(void **)((s8 *)temp_a1 + 0x14);
        if (*(s32 *)((s8 *)arg1 + 4) < ((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34))) {
            var_a0 = 1;
            new_var = (s8 *)arg1;
            if ((*(s32 *)((s8 *)*(void **)((s8 *)temp_a1 + 0x10) + 0x29C) & 2) && (*(s32 *)((s8 *)arg0 + 0x108) != 2)) {
                temp_v1 = *(u32 *)((s8 *)D_8006759C + 8);
                sp18 = 0x6400;
                var_a2 = 0x900;
                if (temp_v1 != 0x19) {
                    if (temp_v1 >= 0x19U) {
                        if (temp_v1 < 0x22U) {
                            if (temp_v1 >= 0x20U) {
                                goto block_8;
                            }
                        }
                    }
                } else {
block_8:
                    if (!(*(s32 *)((s8 *)*(void **)((s8 *)D_800608CC + 0x10) + 0x29C) & 0x800)) {
                        var_a2 = 0x2100;
                        sp18 = D_8006CD48;
                    }
                }
                func_8001CC10(0, (s32)arg0, var_a2, 1, &sp18);
                var_a0 = 0;
            }
            if (var_a0 != 0) {
                *(s32 *)(new_var + 4) = (s32)((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34));
                temp_v1_2 = *(s32 *)((s8 *)arg0 + 0x88);
                *(s32 *)((s8 *)(temp_s2 = arg0) + 0x88) = 0;
                *(s32 *)((s8 *)arg0 + 0x114) = temp_v1_2;
                *(s32 *)((s8 *)arg0 + 0xA8) = (s32)(*(s32 *)((s8 *)arg0 + 0xA8) | 1);
                *(s32 *)((s8 *)arg0 + 0x110) = (s32)D_8006CB64;
            }
        }
    } else {
        *(void **)((s8 *)arg0 + 0x18) = (void *)temp_v0;
    }
    return -0xFF;
}
