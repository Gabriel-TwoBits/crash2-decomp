#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030D98(s32 *a, s32 *b) {
    s32 x;
    s32 y;
    s32 z;
    s32 r;

    x = a[0];
    r = 0;
    if (x >= b[0]) {
        y = a[1];
        if (y >= b[1]) {
            z = a[2];
            if (z >= b[2] && x < b[3] && y < b[4]) {
                r = z < b[5];
            }
        }
    }
    return r;
}
