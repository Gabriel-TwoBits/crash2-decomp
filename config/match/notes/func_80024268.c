#include "common.h"
#include "sdk.h"

s32 func_800322C0(s32, s32, s32, s32);

s32 func_80024268(s32 *p, s32 d, s32 *out) {
    s32 v;
    s32 lim;
    s32 over;
    s32 k;
    k = p[3];
    do {
        v = p[2];
        if (k == 2) {
            v += d;
            lim = (func_800322C0(p[1], 0x4B, 0, 2) - 1) << 8;
            over = v - lim;
            if (over > 0) {
                *out = lim;
                return over;
            }
        } else {
            v -= d;
            if (v < 0) {
                *out = 0;
                return -v;
            }
        }
    } while (0);
    *out = v;
    return 0;
}
