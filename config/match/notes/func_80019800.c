#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80019988(void *, s32, s32, s32, s32);
void func_80019E08(s32, s32);
s32 func_800189CC(void *, void *);
extern void *D_8005F38C;
extern void *D_8006CB10;
extern u8 D_8006CB18;
extern s32 D_8006CB58;
extern void *D_8006CB5C;
extern void *D_8006CBA4;
extern char func_80019074[];

s32 func_80019800(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s1;
    s32 var_v0;
    u8 *var_s0;
    void *var_a0;
    void *var_s2;

    if ((arg1 == 0) && (arg2 == 0)) {
        var_s2 = D_8006CB10;
        D_8005F38C = var_s2;
        goto block_13;
    }
loop_3:
    if (D_8006CB58 == 2) {
        var_s2 = D_8006CB5C;
    } else {
        var_s2 = D_8006CBA4;
    }
    var_a0 = var_s2;
    if (var_s2 == 0) {
        var_s1 = 0;
        if (arg5 != 0) {
            var_s0 = &D_8006CB18;
loop_9:
            temp_v0 = func_80018BE4(var_s0, func_80019074, 0x80000);
            var_s1 += 1;
            if (temp_v0 == 0) {
                var_s0 += 8;
                if (var_s1 >= 8) {
                    goto block_13;
                }
                goto loop_9;
            }
            func_80019E08(temp_v0, 0);
            goto loop_3;
        }
block_13:
        var_a0 = var_s2;
        if (var_s2 == 0) {
            return -0x16;
        }
        goto block_15;
    }
block_15:
    func_800189CC(var_a0, arg0);
    *((s32 *)var_s2 + 1) = 3;
    temp_s0 = func_80019988(var_s2, arg1, arg2, arg3, arg4);
    memset((char *)var_s2 + 0x20, 0, 0x20);
    var_v0 = temp_s0;
    if (var_v0 < -0xFE) {
        var_v0 = (s32) var_s2;
    }
    return var_v0;
}
