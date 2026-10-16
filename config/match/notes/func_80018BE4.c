#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80018BE4(u8 *arg0, void *arg1, s32 arg2) {
    s32 r;
    u8 *c;
    u8 *next;

    r = 0;
    if (arg0 == NULL) {
        return 0;
    }
    if (*(s32 *)arg0 != 2) {
        r = ((s32 (*)(u8 *, s32))arg1)(arg0, arg2);
        if (r != 0) {
            return r;
        }
    }
    if (*(s32 *)arg0 == 2) {
        c = *(u8 **)(arg0 + 4);
    } else {
        c = *(u8 **)(arg0 + 0x4C);
    }
    while (c != NULL && r == 0) {
        next = *(u8 **)(c + 0x48);
        r = func_80018BE4(c, arg1, arg2);
        c = next;
    }
    return r;
}
