#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 w[9]; } Blk;
#define F(p, t, o) (*(t *)((char *)(p) + (o)))

s32 func_8001C340(void *, s32, s32, s32);
extern char *D_8006759C;
extern s32 D_8006CB10;

s32 func_80019988(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    void *p = F(arg0, void *, 0x44);
    s32 *src;
    s32 *dst;
    s32 *entry;
    s32 r;
    void *o;
    u16 idx;

    F(arg0, s32, 0x138) = 0xFFFF;
    F(arg0, s32, 0xB8) = 0;
    F(arg0, s32, 0xF0) = 0;
    F(arg0, s32, 0xF4) = 0;
    F(arg0, s32, 0xF8) = 0;
    F(arg0, s32, 0) = 1;
    F(arg0, s32, 0x84) = 0;
    F(arg0, s32, 0x88) = 0;
    F(arg0, s32, 0x8C) = 0;
    F(arg0, s32, 0x90) = 0;
    F(arg0, s32, 0x94) = 0;
    F(arg0, s32, 0x98) = 0;
    F(arg0, s32, 0x9C) = 0;
    F(arg0, s32, 0xA0) = 0;
    F(arg0, s32, 0xA4) = 0;
    F(arg0, s32, 0x104) = 0;
    F(arg0, s32, 0xEC) = 0;
    F(arg0, s32, 0xD8) = 0;
    F(arg0, s32, 0x94) = 0;
    F(arg0, s32, 0xA8) = 0;
    F(arg0, s32, 0xB0) = 0;
    F(arg0, s32, 0xAC) = 0;
    F(arg0, s32, 0x108) = 0;
    F(arg0, s32, 0x118) = 0;
    F(arg0, s32, 0x110) = 0;
    F(arg0, s32, 0x128) = 0;
    F(arg0, s32, 0x12C) = -2;
    F(arg0, s32, 0x120) = 0;
    if (F(p, s32, 0) == 1) {
        o = F(arg0, void *, 0x44);
        F(arg0, s32, 0x18) = F(p, s32, 0x18);
        *(Blk *)((char *)arg0 + 0x60) = *(Blk *)((char *)o + 0x60);
    } else {
        F(arg0, s32, 0x18) = 0;
        F(arg0, s32, 0x6C) = 0;
        F(arg0, s32, 0x70) = 0;
        F(arg0, s32, 0x74) = 0;
        F(arg0, s32, 0x78) = 0x1000;
        F(arg0, s32, 0x7C) = 0x1000;
        F(arg0, s32, 0x80) = 0x1000;
    }
    if (arg1 >= 4 && (arg1 < 6 || arg1 == 0x1D)) {
        F(arg0, s32, 0x18) = 0;
    }
    r = -0xE;
    if (F(arg0, s32, 4) == 3) {
        entry = (s32 *)(D_8006759C + (arg1 * 4 + 0x10));
        if (*entry != 0x6396347F && arg1 < 0x40) {
            o = func_80014260(entry, 1, 0);
            F(arg0, void *, 0xC) = o;
            F(arg0, void *, 0x10) = o;
            if (F(arg0, s32, 0xC) < -0xFE) {
                F(arg0, void *, 0x40) = arg0;
                F(arg0, s32, 0x58) = 0;
                F(arg0, s32, 0x50) = 0;
                F(arg0, s32, 0x5C) = 0;
                o = F(arg0, void *, 0xC);
                F(arg0, s32, 0xB4) = arg2;
                F(arg0, s32, 0xE8) = 0;
                F(arg0, s32, 0xD0) = 0;
                F(arg0, s32, 0x54) = D_8006CB10;
                idx = *(u16 *)((char *)F(o, void *, 0x1C) + (F(F(o, void *, 0x10), s32, 0x10) + F(arg0, s32, 0xB4)) * 2);
                if (idx == 0xFF) {
                    return -0x1B;
                }
                r = -0xFF;
                if (func_8001C340(arg0, idx, arg3, arg4) < -0xFE) {
                    return r;
                }
            }
        }
        r = -0xE;
    }
    return r;
}
