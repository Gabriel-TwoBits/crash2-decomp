#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F204 = 0xF424;
extern char func_8002A1B8[];
void func_8003CD0C(void *, void *, void *, void *, s32, s32, s32, void *);

void func_8002A3B0(void *arg0, void *arg1, void *arg2, void *arg3, s32 arg4, s32 arg5) {
    s32 *t1;
    s32 v0;
    s32 v1;
    s32 t0;
    s32 a, b;

    t0 = ((s32 *)arg2)[1] + D_8005F204;
    v1 = ((s32 *)arg1)[1] - ((s32 *)arg2)[1];
    t1 = (s32 *)arg3 + 0x408;
    if (v1 > 0) {
        t0 += v1;
    }
    a = -0x9600 / arg4;
    b = 0x9600 / arg4;
    ((s32 *)arg3)[0x408] = ((s32 *)arg2)[0] + a + 0xC80;
    ((s32 *)arg3)[0x409] = ((s32 *)arg2)[1] - 0x4B00;
    ((s32 *)arg3)[0x40A] = ((s32 *)arg2)[2] + a + 0xC80;
    ((s32 *)arg3)[0x40B] = ((s32 *)arg2)[0] + b - 0xC80;
    v0 = ((s32 *)arg2)[1];
    if (!(((s32 *)arg0)[0x40] & 0x40000000)) {
        v0 += *(s32 *)(((s32 *)arg0)[2] + 0x40);
    } else {
        v0 += 0x29900;
    }
    t1[4] = v0;
    t1[5] = ((s32 *)arg2)[2] + 0x9600 / arg4 - 0xC80;
    func_8003CD0C(arg0, arg3, (s32 *)arg3 + 0x402, t1, t0, arg5, 0xC4653601, func_8002A1B8);
}
