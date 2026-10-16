#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030CD8(s32 *a, s32 *p) {
    s32 x = a[0];
    s32 px = p[0];
    s32 y, py, z, pz;
    s32 r = 0;

    if (px >= (x << 8)) {
        y = a[1];
        py = p[1];
        if (py >= (y << 8)) {
            z = a[2];
            pz = p[2];
            if (pz >= (z << 8) && px < ((x + a[3]) << 8) && py < ((y + a[4]) << 8)) {
                r = pz < ((z + a[5]) << 8);
            }
        }
    }
    return r;
}
