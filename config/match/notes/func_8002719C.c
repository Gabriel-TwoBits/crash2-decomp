#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800271D8(s32 *arg0, s32 arg1);

void func_8002719C(s32 *arg0) {
    arg0[0] = -0x100;
    arg0[5] = 0;
    func_800271D8(arg0, 1);
    arg0[0] = 0;
}
