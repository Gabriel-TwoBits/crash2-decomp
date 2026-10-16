#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F294 = 0x1F800040;
s32 D_8005F298 = 0;

s32 *func_80036E30(u8 *arg0, u32 arg1, s32 arg2) {
    s32 *p;
    s32 *q;
    s32 idx;
    s32 t;
    s32 pad[4];

    if (arg1 & 0x800) {
        if (arg1 & 0x400) {
            if (arg1 & 0x200) {
                idx = arg1 & 0x1FF;
                if (idx == 0x1F) {
                    if (arg2 == 0) {
                        p = *(s32 **)(arg0 + 0xBC) - 1;
                        *(s32 **)(arg0 + 0xBC) = p;
                    } else {
                        p = *(s32 **)(arg0 + 0xBC);
                        *(s32 **)(arg0 + 0xBC) = p + 1;
                    }
                } else {
                    p = (s32 *)(arg0 + idx * 4 + 0x40);
                }
                return p;
            }
            q = *(s32 **)(arg0 + ((arg1 >> 4) & 0x1C) + 0x40);
            if (q != 0) {
                return (s32 *)((u8 *)q + (arg1 & 0x3F) * 4 + 0x40);
            }
            return 0;
        }
        if (arg1 & 0x200) {
            if (arg1 & 0x100) {
                if (arg1 & 0x80) {
                    if (arg1 == 0xBE0) {
                        return 0;
                    }
                    if (arg1 == 0xBF0) {
                        return (s32 *)1;
                    }
                    return (s32 *)arg1;
                }
                return (s32 *)(*(u8 **)(arg0 + 0xC4) + ((s32)(arg1 << 25) >> 23));
            }
            t = D_8005F298 == 0;
            p = (s32 *)((u8 *)D_8005F294 + t * 4);
            D_8005F298 = t;
            *p = (s32)(arg1 << 24) >> 20;
            return p;
        }
        t = D_8005F298 == 0;
        p = (s32 *)((u8 *)D_8005F294 + t * 4);
        D_8005F298 = t;
        *p = (s32)(arg1 << 23) >> 15;
        return p;
    }
    if (arg1 & 0x400) {
        q = *(s32 **)(arg0 + 0x14);
    } else {
        q = *(s32 **)(arg0 + 0x10);
    }
    return (s32 *)(*(u8 **)((u8 *)q + 0x18) + (arg1 & 0x3FF) * 4);
}
