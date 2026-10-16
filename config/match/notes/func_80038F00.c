#include "common.h"
#include "sdk.h"
#include "game.h"

void *func_80036E30(void *, s32, s32);
void *func_80014260(void *, s32, s32);
s32 func_800143C0(void *, s32);
s32 func_8001456C(void);
s32 func_80014918(void *, s32);

void func_80038F00(u8 *arg0, u32 arg1) {
    s32 n;
    s32 m;
    s32 val;
    s32 *p;
    s32 *sp;

    n = *(s32 *)func_80036E30(arg0, (arg1 >> 12) & 0xFFF, 0);
    p = func_80036E30(arg0, arg1 & 0xFFF, 0);
    switch (n) {
    case 1:
        *(void **)(arg0 + 0xD4) = func_80014260(p, 0, 1);
        break;
    case 6:
        *(void **)(arg0 + 0xD4) = func_80014260(p, 1, 1);
        break;
    case 2:
        *(s32 *)(arg0 + 0xD4) = func_800143C0(p, 1);
        break;
    case 3:
        val = func_800143C0(p, 0);
        goto push;
    case 4:
        val = func_8001456C();
    push:
        *(*(s32 **)(arg0 + 0xBC))++ = val;
        break;
    case 5:
        m = *p;
        val = func_80014918(*(u8 **)(arg0 + 0xBC) - m * 4, m);
        sp = *(s32 **)(arg0 + 0xBC) - m;
        *(s32 **)(arg0 + 0xBC) = sp + 1;
        *sp = val;
        break;
    default:
        break;
    }
}
