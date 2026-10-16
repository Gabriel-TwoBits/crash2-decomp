#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;

s32 func_800111B0(u32 n) {
    s32 lim;
    s32 hi;
    s32 lo;

    if (n != 0) {
        hi = n;
        lo = n;
        if ((u32)D_8005F2AC < n) {
            lo = D_8005F2AC;
        } else {
            hi = D_8005F2AC;
        }
        lim = ((((char *)D_8005F2B4 - (char *)D_8005F2A4) >> 2) * 4) - 8;
        if (lim >= hi) {
            n = hi;
        } else if (lim >= lo) {
            n = lo;
        } else {
            return -1;
        }
    } else {
        n = 0;
        if ((u32)D_8005F2A4 >= (u32)D_8005F2B4) {
            return -1;
        }
    }
    D_8005F2A4[(s32)n / 4] = -2;
    D_8005F2A4[-1] = n | 1;
    D_8005F2A4 = &D_8005F2A4[((s32)n + 4) / 4];
    return 0;
}
