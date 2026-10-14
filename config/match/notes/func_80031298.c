#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80031298(s32 *arg0, s32 *arg1) {
    return ratan2(arg1[0] - arg0[0], arg1[1] - arg0[1]) & 0xFFF;
}
