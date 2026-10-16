#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800608D8;
s32 func_800312CC(s32, s32, s32);

void func_80026A08(s32 *arg0, s32 arg1, s32 *arg2, s32 *arg3) {
    s32 t[10];
    s32 *base;
    s16 *a;
    s16 *b;
    s32 frac;
    s32 *new_var;
    s32 idx;
    s32 v;

    base = *(s32 **)(*(s32 *)(arg0 + 1) + 0x14);
    a =(s16 *)func_80031AE8((s32)arg0, 0x4B, 0, idx = arg1 >> 8, 0, 2, 0, 0, 0);
    new_var = &D_800608D8;
    b = (s16 *)func_80031AE8(*new_var,0x4B, 0, idx * 2, 0, 2, 0, 0, 0);
    arg2[0] = (base[0] + a[0]) << 8;
    arg2[1] = (base[1] + a[1]) << 8;
    arg2[2] = (base[2] + a[2]) << 8;
    arg2[3] = b[0];
    arg2[4] = b[1];
    arg2[5] = b[2];
    if (arg3 != 0) {
        arg3[0] = b[3];
        arg3[1] = b[4];
        arg3[2] = b[5];
    }
    frac = arg1 & 0xFF;
    if (frac != 0 && idx + 1 < func_800322C0((s32)arg0, 0x4B, 0, 2)) {
        t[0] = (base[0] + a[3]) << 8;
        t[1] = (base[1] + a[4]) << 8;
        t[2] = (base[2] + a[5]) << 8;
        t[3] = b[6];
        t[4] = b[7];
        t[5] = b[8];
        arg2[0] += ((t[0] - arg2[0]) * frac) >> 8;
        arg2[1] += ((t[1] - arg2[1]) * frac) >> 8;
        arg2[2] += ((t[2] - arg2[2]) * frac) >> 8;
        arg2[3] = func_800312CC(arg2[3], t[3], frac);
        arg2[4] = func_800312CC(arg2[4], t[4], frac);
        arg2[5] = func_800312CC(arg2[5], t[5], frac);
    }
}
