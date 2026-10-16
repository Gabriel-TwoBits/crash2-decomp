#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800310EC(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 d;
    s32 nx = -arg3;
    s32 ny = -arg4;
    s32 d0;
    s32 nz = -arg5;

    d = arg1[0] - arg2[0];
    d0 = d;
    if (arg3 < d0 || d0 < nx ||
        (d = arg1[1] - arg2[1], arg4 < d) || d < ny ||
        (d = arg1[2] - arg2[2], arg5 < d) || nz > d) {
        if (arg0) {
            if (nz) {
                arg0[42] |= 0x8000;
            } else {
                arg0[42] |= 0x8000;
            }
        }
        return 1;
    }
    return 0;
}
