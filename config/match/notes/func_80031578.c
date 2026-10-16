#include "common.h"
#include "sdk.h"

s32 D_8005F224 = 1;

s32 func_80031578(s32 arg0) {
    s32 seed;
    u32 q;
    s32 v;
    s32 r;
    if (arg0 == 0) {
        return 0;
    }
    seed = D_8005F224 * 0x41C64E6D + 0x3039;
    D_8005F224 = seed;
    q = (u32)seed / 15;
    v = q % 0x7FFFFFF;
    if (v >= 0) {
        r = v % arg0;
    } else {
        r = -v % arg0;
    }
    return r;
}
