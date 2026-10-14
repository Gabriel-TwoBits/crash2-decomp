#include "common.h"
#include "sdk.h"

s32 func_800312F0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 d;

    d = (s32)((arg1 - arg0) << 20) >> 20;
    if (d > 0 && arg3 < d) {
        goto clamp;
    }
    if (d < 0) {
        arg3 = -arg3;
        if (d < arg3) {
clamp:
            d = arg3;
        }
    }
    return (arg0 + ((arg2 * d) >> 8)) & 0xFFF;
}
