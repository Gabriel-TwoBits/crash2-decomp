#include "common.h"
#include "sdk.h"
#include "game.h"

extern void MulMatrix(s16 *, s16 *);
extern void ScaleMatrix(s16 *, s32);

void func_8001E780(s16 *arg0, s32 *arg1, s32 arg2) {
    s16 sp[16];
    s16 c;
    s32 s;
    s32 one;

    one = 0x1000;
    s = func_8003D2EC(arg1[2] & 0xFFF);
    c = func_8003D360(arg1[2] & 0xFFF);
    arg0[0] = c;
    arg0[1] = 0;
    arg0[2] = s;
    arg0[3] = 0;
    arg0[4] = one;
    arg0[5] = 0;
    arg0[6] = -s;
    arg0[7] = 0;
    arg0[8] = c;
    s = func_8003D2EC(arg1[0]);
    c = func_8003D360(arg1[0]);
    sp[0] = one;
    sp[1] = 0;
    sp[2] = 0;
    sp[3] = 0;
    sp[4] = c;
    sp[5] = -s;
    sp[6] = 0;
    sp[7] = s;
    sp[8] = c;
    MulMatrix(arg0, sp);
    s = func_8003D2EC(-((arg1[2] - arg1[1]) & 0xFFF));
    c = func_8003D360(-((arg1[2] - arg1[1]) & 0xFFF));
    sp[0] = c;
    sp[1] = 0;
    sp[2] = s;
    sp[3] = 0;
    sp[4] = one;
    sp[5] = 0;
    sp[6] = -s;
    sp[7] = 0;
    sp[8] = c;
    MulMatrix(arg0, sp);
    if (arg2 != 0) {
        ScaleMatrix(arg0, arg2);
    }
}
