#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800312CC(s32 arg0, s32 arg1, s32 arg2);
extern s32 D_8005BA4C;
extern s32 D_800608DC;
extern s32 D_800608E0;

s32 func_80023DC4(void) {
    s32 sp28;
    s32 v;
    s32 a0;
    s32 a1;

    v = func_80031AE8(D_800608DC, 0x16E, D_800608E0, 0, 0, 3, &sp28, 0, 0);
    if (v == 0) {
        return 0;
    }
    a0 = *(s16 *)v;
    if (D_8005BA4C != 0) {
        a1 = *(s16 *)(v + 2);
        if (D_8005BA4C == 0x400) {
            a0 = a1;
        } else {
            a0 = func_800312CC(a0, a1, D_8005BA4C >> 2);
        }
    }
    return a0;
}
