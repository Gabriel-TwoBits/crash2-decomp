#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030E1C(s32 *arg0, s32 *arg1) {
    s32 a = arg1[0];
    s32 b = arg1[1];
    s32 c = arg1[2];

    if ((a << 8) < arg0[3] && (b << 8) < arg0[4] && (c << 8) < arg0[5]
        && ((a + arg1[3]) << 8) >= arg0[0]
        && ((b + arg1[4]) << 8) >= arg0[1]) {
        return ((c + arg1[5]) << 8) >= arg0[2];
    }
    return 0;
}
