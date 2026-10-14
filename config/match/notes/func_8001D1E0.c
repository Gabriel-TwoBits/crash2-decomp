#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_8001CEDC();
extern s32 func_80018D28(s32 a, void *b, s32 *c);

s32 func_8001D1E0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 sp[6];

    sp[0] = arg0;
    sp[1] = arg2;
    sp[3] = arg3;
    sp[4] = 0;
    sp[2] = arg4;
    sp[5] = arg5;
    func_80018D28(arg1, func_8001CEDC, sp);
    return -0xFF;
}
