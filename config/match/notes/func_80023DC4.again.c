#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005BA4C;
extern s32 D_800608DC;
extern s32 g_CameraProgress;

s32 func_80023DC4(void) {
    s32 sp28;
    s32 v;
    s32 a0;
    s32 a1;

    v = Ent_GetProperty(D_800608DC, 0x16E, g_CameraProgress, 0, 0, 3, &sp28, 0, 0);
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
