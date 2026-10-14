#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F264 = 0;

s32 func_80035A94(void) {
    s32 *p = (s32 *)D_8005F264;
    s32 n = p[5];
    p[5] = n + 1;
    return n >= p[4];
}
