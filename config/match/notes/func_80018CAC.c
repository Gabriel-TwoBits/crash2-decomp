#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80018AA8(void *, s32, s32);

s32 func_80018CAC(s32 *arg0, s32 arg1, s32 arg2) {
    s32 *p;
    s32 *next;

    if (arg0[0] == 2) {
        p = (s32 *)arg0[1];
    } else {
        p = (s32 *)arg0[0x4C / 4];
    }
    while (p != 0) {
        next = (s32 *)p[0x48 / 4];
        func_80018AA8(p, arg1, arg2);
        p = next;
    }
    return -0xFF;
}
