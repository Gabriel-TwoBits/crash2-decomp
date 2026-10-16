#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001C340(void *, s32, s32, s32 *);

s32 func_8003900C(char *arg0, u32 arg1, s32 arg2) {
    s32 args[16];
    s32 n;
    s32 i;
    s32 *dst;
    s32 mask;
    s32 state;
    s32 flags;
    s32 hit;
    s32 r;
    s32 *sp;

    n = (arg1 >> 10) & 0xF;
    i = n - 1;
    if (i >= 0) {
        dst = args + i;
        do {
            sp = *(s32 **)(arg0 + 0xBC);
            *(s32 **)(arg0 + 0xBC) = sp - 1;
            i--;
            *dst = sp[-1];
            dst--;
        } while (i >= 0);
    }
    mask = *(s32 *)(arg0 + 0xB0);
    state = *(s32 *)(arg0 + 0x108);
    flags = *(s32 *)(*(char **)(*(char **)(arg0 + 0xC) + 0x20) + (arg1 & 0x3FF) * 16);
    if ((u32)(state - 3) >= 2U) {
        hit = mask & flags;
        if (state == 2) {
            goto b;
        }
    } else {
b:
        hit = (mask | 0x1002) & flags;
    }
    if (hit == 0) {
        r = func_8001C340(arg0, arg1 & 0x3FF, n, args);
        if (r < -0xFE && (arg2 & 2)) {
            return r;
        }
        *(char **)(arg0 + 0xBC) -= 4;
    } else {
        return 0;
    }
    return 0;
}
