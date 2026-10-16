#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80063450;

s32 func_8001E3DC(s32 arg0, s32 arg1, s32 arg2, s32 *arg3) {
    u32 n;
    s32 prod;
    s32 step;
    s32 diff;
    s32 dist;
    s32 res;

    n = D_80063450[5];
    if (n >= 0x67) {
        n = 0x66;
    }
    prod = arg2 * n;
    step = prod / 1024;
    diff = (s32)((arg1 - arg0) << 20) >> 20;
    dist = diff;
    if (dist < 0) {
        dist = -dist;
    }
    if (arg3 != 0 && (arg3[0xAC / 4] & 0x20000000) && dist < step * 4) {
        if (dist < step * 2) {
            if (dist < step) {
                step = step / 8;
            } else {
                step = step / 4;
            }
        } else if (dist < step * 3) {
            step = step / 2;
        } else {
            step = (step * 3) / 4;
        }
    }
    if (dist < step) {
        arg0 = arg1;
        if (arg3 != 0) {
            arg3[0xA8 / 4] |= 0x800;
        }
    } else {
        if ((dist < 0 ? -dist : dist) == 0x800 && arg1 >= 0x800) {
            diff = -diff;
        }
        if (diff < 0) {
            arg0 = (arg0 - step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] |= 8;
            }
        } else {
            arg0 = (arg0 + step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] &= ~8;
            }
        }
    }
    return arg0;
}
