#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

s32 func_800360AC(s32 arg0) {
    D_8005F264[1] &= ~arg0;
    return 0;
}
