#include "common.h"
#include "sdk.h"

s32 func_80030FEC(s32 *arg0, s32 *arg1) {
    s32 d[3];
    s32 x, y, z;

    x = arg0[0] - arg1[0];
    if (x < 0) x = arg1[0] - arg0[0];
    d[0] = x;
    y = arg0[1] - arg1[1];
    if (y < 0) y = arg1[1] - arg0[1];
    d[1] = y;
    z = arg0[2] - arg1[2];
    if (z < 0) z = arg1[2] - arg0[2];
    d[2] = z;
    if (x >= y && x >= z) return x + (y + z) / 4;
    if (d[1] >= d[0] && d[1] >= d[2]) return d[1] + (d[0] + d[2]) / 4;
    return d[2] + (d[1] + d[0]) / 4;
}
