#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800607FC;
extern s32 D_800608D8[];
extern s32 D_800608E0;
extern s32 D_8006CD98;

void func_800224A0(s32 *arg0) {
    s32 sp28;
    s16 *p;
    s32 new_var;
    s32 lo;
    s32 hi;
    s32 scale;
    s32 d;

    if (func_80031AE8(D_800608D8[0], 0x252, D_800608E0, 0, 0, 0, 0, 0, 0) != 0) {
        p = (s16 *)func_80031AE8(D_800608D8[0], 0x254, D_800608E0, 0, 0, 3, &sp28, 0, 0);
        if (p != 0) {
            lo = p[0] << 8;
            hi = p[1];
            hi = hi << 8;
            new_var = hi;
            scale = p[2];
            if (D_8006CD98 != 0x100) {
                scale = (scale * D_8006CD98) >> 8;
            }
            d = D_800607FC - arg0[2];
            if (new_var < d) {
                d = new_var;
            }
            d -= lo;
            if (d < 0) {
                d = 0;
            }
            D_800607FC -= (d * scale) >> 8;
        }
    }
}
