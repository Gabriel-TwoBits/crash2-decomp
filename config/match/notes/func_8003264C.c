#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8003BEA8(s32, s32);
s16 D_8005F2E4;
s16 D_8005F2E6;
extern s32 D_8006CBF4;

s16 *func_8003264C(s32 *arg0, s32 arg1) {
    s32 x;
    s32 y;
    s32 d;
    s32 a;
    s32 lo;
    s32 hi;
    s16 c;

    x = arg0[0] / 32;
    y = (arg0[2] - 0x4B000) / 32;
    d = func_8003BEA8(x, y);
    if (d <= 0x7FFF) {
        D_8005F2E4 = arg1;
        goto set6;
    }
    a = func_8003D360(ratan2(y, x)) + 0x1000;
    lo = (arg1 << 15) / d;
    hi = lo;
    if (D_8006CBF4 == 0) {
        lo = (lo + lo * (0x2000 - a)) >> 11;
        hi = (hi + hi * a) >> 11;
    }
    if (lo < arg1) {
        if (lo >= 0) {
            D_8005F2E4 = lo;
        } else {
            D_8005F2E4 = 0;
        }
    } else {
        D_8005F2E4 = arg1;
    }
    if (hi < arg1) {
        if (hi >= 0) {
            D_8005F2E6 = hi;
        } else {
            D_8005F2E6 = 0;
        }
    } else {
set6:
        D_8005F2E6 = arg1;
    }
    c = D_8005F2E4;
    if (c >= 0x4000) {
        c = 0x3FFF;
    }
    d = D_8005F2E6;
    D_8005F2E4 = c;
    if (d >= 0x4000) {
        d = 0x3FFF;
    }
    D_8005F2E6 = d;
    return &D_8005F2E4;
}
