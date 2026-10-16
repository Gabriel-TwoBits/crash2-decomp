#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8003D2EC(s32);
s32 func_8003D360(s32);

void func_8002FF54(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 a;
    s32 b;
    s32 t;

    t = (arg1 >> 8) * func_8003D2EC(arg2);
    arg0[1] = t >> 4;
    a = arg1 >> 8;
    b = a * func_8003D360(arg2);
    arg0[0] = ((b >> 12) * func_8003D2EC(arg3)) >> 4;
    arg0[2] = ((b >> 12) * func_8003D360(arg3)) >> 4;
}
