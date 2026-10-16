#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8003D2EC(s32);

s32 func_8001E640(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 sh;
    s32 v;
    s32 c;

    a = arg0 & 0xFFF;
    sh = (arg1 > 0xFFFF) * 2;
    v = (func_8003D2EC(((a % arg1) << 11) / arg1 - 0x400) * arg2) >> 12;
    if (v < 0x1000) {
        c = -0x1000;
        if (v >= -0x1000) {
            c = v;
        }
    } else {
        c = 0x1000;
    }
    return (((c + 0x1000) >> sh) * arg1 >> (13 - sh)) + a / arg1 * arg1;
}
