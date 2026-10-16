#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001232C(s32 *arg0) {
    s32 *b;
    s32 v;
    s32 t = *(u16 *)((u8 *)arg0 + 2);

    b = arg0;
    if (t < 6) {
        if (t < 3 && t != 0) {
            if (t == 1) {
                return 0x10000;
            }
            return -0xE;
        }
        v = b[b[2] + 4];
    } else {
        if (t != 0x100) {
            return -0xE;
        }
        v = arg0[arg0[3] + 4];
    }
    if (v < 0) {
        return v - (s32)b;
    }
    return v;
}
