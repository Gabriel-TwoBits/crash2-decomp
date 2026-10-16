#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F290;

s32 *func_80036D0C(struct Obj *a0, s32 arg1) {
    u8 *arg0 = (u8 *)a0;
    s32 *scratch = (s32 *)0x1F800040;
    s32 *r;
    s32 t;
    u8 *p;
    s32 pad[4];

    if (arg1 & 0x800) {
        if (arg1 & 0x400) {
            p = *(u8 **)(arg0 + (((u32)arg1 >> 4) & 0x1C) + 0x40);
            if (p != 0) {
                return (s32 *)(p + ((arg1 & 0x3F) * 4 + 0x40));
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
                    return (s32 *)0xBF0;
                }
                return (s32 *)(*(u8 **)(arg0 + 0xC4) + ((arg1 << 25) >> 23));
            }
            t = D_8005F290 == 0;
            r = scratch + t;
            D_8005F290 = t;
            *r = (arg1 << 24) >> 20;
            return r;
        }
        t = D_8005F290 == 0;
        r = scratch + t;
        D_8005F290 = t;
        *r = (arg1 << 23) >> 15;
        return r;
    }
    if (arg1 & 0x400) {
        p = *(u8 **)(arg0 + 0x14);
    } else {
        p = *(u8 **)(arg0 + 0x10);
    }
    return *(s32 **)(p + 0x18) + (arg1 & 0x3FF);
}
