#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060828;
extern s32 D_800608D8;
extern s32 D_800608E0;
void func_80025FDC(s32, s32 *);
s32 func_80031AE8(s32, s32, s32, s32, s32 *, s32, s32 *, s32, s32);
s32 func_8003D2EC(s32);
s32 func_8003D360(s32);

void func_80025D18(s32 *arg0) {
    s32 sp28;
    s32 lo0, hi0, lo1, hi1;
    s16 *a;
    s16 *b;
    s16 *c;
    s32 x, z, s, co, v, xx, zz;

    a = (s16 *)func_80031AE8(D_800608D8, 0x1E3, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (a != 0) {
        lo0 = a[0];
        hi0 = a[1];
        if (hi0 >= lo0) {
            v = arg0[1] - D_800607F8;
            if (v < (lo0 << 8) || (hi0 << 8) < v) {
                sp28 = a[2] << 8;
                func_80025FDC(0, &sp28);
            }
        }
    }
    b = (s16 *)func_80031AE8(D_800608D8, 0x1E2, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (b != 0) {
        lo0 = b[0];
        hi0 = b[1];
        if (hi0 < lo0) {
            b = 0;
        }
    }
    c = (s16 *)func_80031AE8(D_800608D8, 0x1E4, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (c != 0) {
        lo1 = c[0];
        hi1 = c[1];
        if (hi1 < lo1) {
            c = 0;
        }
    }
    if (b != 0 || c != 0) {
        x = arg0[0] - D_800607F4;
        z = arg0[2] - D_800607FC;
        s = -D_80060828;
        co = func_8003D360(s);
        v = func_8003D2EC(s);
        zz = z >> 8;
        xx = x >> 8;
        x = (zz * v >> 4) + (xx * co >> 4);
        z = (xx * v >> 4) - (zz * co >> 4);
        s = x;
        if (b != 0 && (s < (lo0 << 8) || (hi0 << 8) < s)) {
            sp28 = b[2] << 8;
            func_80025FDC(0, &sp28);
        }
        if (c != 0 && (z < (lo1 << 8) || (hi1 << 8) < z)) {
            sp28 = c[2] << 8;
            func_80025FDC(0, &sp28);
        }
    }
}
