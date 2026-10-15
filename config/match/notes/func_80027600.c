#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800176DC(s32, s32, s32, s32, s32, s32);

void func_80027600(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_t0;
    s32 var_v1;

    var_v1 = 0;
    var_t0 = 0;
    if (arg3 & 0x100) {
        var_v1 = *(s32 *)((char *)arg0 + 0x34);
    }
    if (arg3 & 0x200) {
        var_t0 = *(s32 *)((char *)arg0 + 0x38);
    }
    if ((var_t0 != 0) || (var_v1 != 0)) {
        func_800176DC(1, arg1, arg2, var_t0, var_v1, 1);
    }
}
