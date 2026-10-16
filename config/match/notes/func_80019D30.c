#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800608D0;

s32 func_80019D30(s32 *p, s32 a1) {
    s32 r = -0xFF;
    if (p[0xB8 / 4] == 0) {
        if (p[0] != 0 && p[0x18 / 4] == a1 && !(p[0xAC / 4] & 0x1000000) && !(p[0x100 / 4] & 0x40000)) {
            func_8001CC10(0, (s32)p, 0x1A00, 0, 0);
            if (p[0x18 / 4] == a1 || D_800608D0 == -1) {
                func_80019BBC((s32)p, 0);
                return;
            }
        }
    }
    return r;
}
