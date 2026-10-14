#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800123BC(s32 *arg0, s32 arg1) {
    s32 *p;
    s32 r;

    if (arg0[3] == 0) {
        r = 0;
    } else {
        p = arg0 + arg1;
        r = p[5] - p[4];
    }
    return r;
}
