#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030EC0(s32 *a, s32 *b) {
    if (b[4] < a[1]) return 0;
    if (b[1] >= a[4]) return 0;
    if (b[0] >= a[3]) return 0;
    if (b[2] >= a[5]) return 0;
    if (b[3] < a[0]) return 0;
    return b[5] >= a[2];
}
