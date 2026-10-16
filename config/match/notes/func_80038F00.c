#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001456C(s32 *);
s32 func_80014918(u8 *, s32);
s32 *func_80036E30(s32, s32, s32);

void func_80038F00(s32 arg0, u32 arg1) {
    s32 *src;
    u8 *sp;
    s32 op;
    s32 n;
    s32 v;

    op = *func_80036E30(arg0, (arg1 >> 12) & 0xFFF, 0);
    src = func_80036E30(arg0, arg1 & 0xFFF, 0);
    switch (op) {
    case 1:
        *(void **)(arg0 + 0xD4) = func_80014260(src, 0, 1);
        break;
    case 6:
        *(void **)(arg0 + 0xD4) = func_80014260(src, 1, 1);
        break;
    case 2:
        *(s32 *)(arg0 + 0xD4) = func_800143C0(src, 1);
        break;
    case 3:
        v = func_800143C0(src, 0);
        goto push;
    case 4:
        v = func_8001456C(src);
    push:
        *(*(s32 **)(arg0 + 0xBC))++ = v;
        break;
    case 5:
        n = *src;
        v = func_80014918(*(u8 **)(arg0 + 0xBC) - n * 4, n);
        sp = *(u8 **)(arg0 + 0xBC) - n * 4;
        *(u8 **)(arg0 + 0xBC) = sp + 4;
        *(s32 *)sp = v;
        break;
    default:
        break;
    }
}
