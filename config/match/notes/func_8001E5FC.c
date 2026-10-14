#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001E5FC(s32 arg0, s32 arg1, s32 arg2) {
    s32 diff;

    diff = arg1 - arg0;
    if (arg2 > 0 && (diff < 0 ? -diff : diff) < arg2) {
        return arg1;
    }
    if (diff > 0) {
        return arg0 + arg2;
    }
    return arg0 - arg2;
}
