#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80016F04(s32 arg0) {
    if (arg0 < 0) {
        goto L22;
    }
    if (arg0 < 0x13) {
        arg0 = 0x11;
    } else if (arg0 < 0x24) {
L22:
        arg0 = 0x22;
    } else if (arg0 < 0x35) {
        arg0 = 0x33;
    }
    return arg0;
}
