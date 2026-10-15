#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001232C(u8 *arg0) {
    s32 v;
    u8 *base = arg0;
    u16 type = *(u16 *)(arg0 + 2);

    if (type < 6) {
        if (type < 3 && type != 0) {
            if (type == 1) {
                return 0x10000;
            }
            return -0xE;
        }
        v = *(s32 *)(base + *(s32 *)(base + 8) * 4 + 0x10);
    } else {
        if (type != 0x100) {
            return -0xE;
        }
        v = *(s32 *)(arg0 + *(s32 *)(arg0 + 0xC) * 4 + 0x10);
    }
    if (v < 0) {
        return v - (s32)base;
    }
    return v;
}
