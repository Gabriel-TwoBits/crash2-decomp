#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030F58(s32 *a, s32 *b) {
    if (a[0] >= b[0] && a[1] >= b[1] && a[2] >= b[2] && a[3] < b[3] && a[4] < b[4]) {
        return a[5] < b[5];
    }
    return 0;
}
