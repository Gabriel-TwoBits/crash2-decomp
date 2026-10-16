#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80024B6C(void *, s32, s32 *, s32, void *, s32, s32 *);
extern char D_80010380[];

s32 func_80024A48(void *arg0, s32 arg1, s32 *arg2, s32 arg3, void *arg4, s32 arg5, s32 *arg6) {
    s32 sp20;
    s32 sp24;
    s32 temp_v0;
    s32 var_s0;
    s32 var_s2;

    var_s2 = 2;
    *(s32 *)((char *)arg0 + 0xC) = 2;
    var_s0 = func_80024B6C(arg0, arg1, arg2, arg3, arg4, arg5, &sp20);
    if (var_s0 != 0) {
        *(s32 *)((char *)arg0 + 0xC) = 1;
        temp_v0 = func_80024B6C(arg0, arg1, arg2, arg3, arg4, arg5, &sp24);
        if (temp_v0 >= 0) {
            if ((var_s0 < 0) || (temp_v0 < var_s0)) {
                var_s0 = temp_v0;
                var_s2 = 1;
                sp20 = sp24;
            }
        } else if (var_s0 < 0) {
            printf(D_80010380);
        }
    }
    *arg6 = sp20;
    *(s32 *)((char *)arg0 + 0xC) = var_s2;
    return var_s0;
}
