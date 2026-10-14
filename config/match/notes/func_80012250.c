#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80012010(s32);

s32 func_80012250(s32 *arg0) {
    s32 v;
    s32 r;

    v = arg0[0];
    if (v & 3) {
        r = func_80012010(arg0[1]);
    } else {
        r = ((s32 *)v)[2];
    }
    return r;
}
