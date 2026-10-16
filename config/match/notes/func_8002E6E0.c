#include "common.h"
#include "sdk.h"
#include "game.h"

void func_8002E6E0(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 dx, dz, pz, px, len;

    dx = (arg3 - arg2) >> 8;
    dz = (arg5 - arg4) >> 8;
    pz = (arg1[2] - arg4) >> 8;
    px = (arg1[0] - arg2) >> 8;
    len = func_80041A78(dx * dx + dz * dz);
    dz = (dz << 12) / len;
    dx = (dx << 12) / len;
    pz = (pz * dz - px * dx) >> 12;
    arg0[0] = arg2 - ((pz * dx) >> 16);
    arg0[1] = arg1[1];
    arg0[2] = ((pz * dz) >> 16) + arg4;
}
