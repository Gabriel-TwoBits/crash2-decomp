#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005B198[];

s32 func_80012100(s32 arg0) {
    s32 i;
    u8 c = arg0;

    for (i = 0; i < 0x40; i++) {
        if (D_8005B198[i] == c) {
            return i;
        }
    }
    if (c == 0x2D) {
        return 0x63;
    }
    return -0xE;
}
