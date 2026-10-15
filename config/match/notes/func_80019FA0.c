#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80018BE4(u8 *, void *, s32);
extern u8 D_8006CB30[];
extern u8 func_80018E48[];

s32 func_80019FA0(s32 arg0) {
    u8 *p;
    u8 *a;
    s32 i;
    s32 r;

    i = 3;
    p = D_8006CB30;
    do {
        a = p;
        p += 8;
        i++;
        r = func_80018BE4(a, func_80018E48, arg0);
        if (r != 0) {
            break;
        }
        a = p;
    } while (i < 5);
    return r;
}
