#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80031AE8(s32, s32, s32, s32, s32 *, s32, s32 *, s32, s32);

s32 func_800322C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp28[4];
    s32 sp38;

    if (func_80031AE8(arg0, arg1, arg2, 0, &sp38, arg3, sp28, 1, 0) != 0) {
        return sp38;
    }
    return 0;
}
