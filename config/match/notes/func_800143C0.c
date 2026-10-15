#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001434C(u32, s32);
u32 func_80014B44(u32);

s32 func_800143C0(void *a, s32 arg1) {
    u32 *arg0 = a;
    u32 v;
    u32 t;

    t = *arg0;
    if (t & 1) {
        v = func_80014B44(t);
        *arg0 = v;
    } else {
        v = t;
    }
    return func_8001434C(v, arg1);
}
