#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F390;
extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_80029768(void) {
    s16 *p;

    p = (s16 *)func_8001148C(0xBE4);
    D_8005F390 = (s32)p;
    D_8005F3D0 = (s32)p;
    if (p != 0) {
        p[1] = 0;
        p = (s16 *)func_8001148C(0xBE4);
        D_8005F398 = (s32)p;
        if (p != 0) {
            p[1] = 0;
            return -0xFF;
        }
    }
    return -0xF;
}
