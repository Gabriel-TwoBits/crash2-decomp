#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80067590;

void func_80021900(s32 count, u32 *p, s32 flags, s32 arg3) {
    u32 *cur;
    u32 v;
    u32 *e;
    s32 neg;

    cur = 0;
    do {
        v = *p;
        if (v & 1) {
            e = *(u32 **)(((v >> 13) & 0x3FC) + D_80067590);
            if (cur == 0 || cur < e) {
                cur = e;
            }
            if (cur[1] != v) {
                while (cur[1] != v) {
                    cur += 2;
                }
                cur -= 2;
                cur += 2;
            }
        } else {
            cur = (u32 *)v;
        }
        if (flags & 8) {
            func_80014260(cur, flags | 2, arg3);
        } else {
            neg = arg3;
            if (flags & 4) {
                neg = -neg;
            }
            func_8001434C((u32)cur, neg);
        }
        count--;
        p++;
    } while (count != 0);
}
