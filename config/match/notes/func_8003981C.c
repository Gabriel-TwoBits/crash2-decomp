#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001C688(void *, s32);

s32 func_8003981C(u8 *arg0, u32 arg1, s32 arg2, s32 *arg3) {
    s32 a0 = (arg1 >> 0x16) & 3;
    s32 mode = (arg1 >> 0x14) & 3;
    s32 t1;
    u32 idx;
    s32 *p;
    s32 v;
    s32 n;
    s32 *q;
    s32 *sp;

    *(s32 *)0x1F800048 = arg2;
    if (!(arg2 & 8)) {
        return -0xE;
    }
    switch (mode) {
    case 0:
        t1 = 1;
        break;
    case 1:
        idx = (arg1 >> 0xE) & 0x3F;
        if (idx == 0x1F) {
            p = *(s32 **)(arg0 + 0xBC) - 1;
            *(s32 **)(arg0 + 0xBC) = p;
        } else {
            p = (s32 *)(arg0 + idx * 4 + 0x40);
        }
        t1 = *p;
        break;
    case 2:
        idx = (arg1 >> 0xE) & 0x3F;
        if (idx == 0x1F) {
            p = *(s32 **)(arg0 + 0xBC) - 1;
            *(s32 **)(arg0 + 0xBC) = p;
        } else {
            p = (s32 *)(arg0 + idx * 4 + 0x40);
        }
        t1 = *p == 0;
        break;
    default:
        break;
    }
    arg2 |= 0x20;
    if (t1 != 0) {
        arg3[1] = (arg1 >> 0x18) != 0x3D;
        *(s32 *)0x1F800048 = arg2;
        if (a0 == 1) {
            n = (arg1 >> 0xA) & 0xF;
            for (v = n - 1; v >= 0; v--) {
                sp = *(s32 **)(arg0 + 0xBC);
                *(s32 **)(arg0 + 0xBC) = sp - 1;
                arg3[v + 3] = sp[-1];
            }
            arg3[2] = n;
            v = func_8001C688(arg0, 0);
            if (v < -0xFE) {
                arg3[0] = arg1 & 0x3FF;
                return -0xFF;
            }
            return v;
        } else if (a0 == 2) {
            v = func_8001C688(arg0, 0);
            if (v < -0xFE) {
                arg3[0] = 0xFF;
                return -0xFF;
            }
            return v;
        } else {
            return 0;
        }
    }
    if (a0 == 0) {
        *(s32 *)(arg0 + 0xC0) += (s32)(arg1 << 0x16) >> 0x14;
        *(s32 *)(arg0 + 0xBC) -= (arg1 >> 8) & 0x3C;
    }
    return 0;
}
