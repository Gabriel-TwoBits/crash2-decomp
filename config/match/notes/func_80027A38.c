#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F3EC;
extern s32 D_80069334[2];
extern s32 D_8006933C;
extern s32 D_80069340;
extern s32 D_80069344;
extern s32 D_80069348;
extern s32 D_8006934C;
extern s32 D_80069350;
extern s32 D_80069354;
extern u8 *D_80069358;

s32 func_80027A38(void) {
    s32 *p;
    s32 i;
    s32 size;
    s32 buf;
    s32 pad;
    s32 left;
    s32 *g;
    s32 *h;

    g = D_80069334;
    h = g;
    g[0] = 0;
    D_8006933C = 0;
    g[1] = 0;
    D_80069358 = 0;
    if (D_8005F3EC == 2 || D_8005F3EC == 0x3C) {
        g[0] = 5;
        p = (s32 *)func_8001148C(0x1A4);
        g[1] = (s32)p;
        if (p != 0) {
            for (i = 0; i < h[0]; i++) {
                p[1] = 0;
                p[2] = 0;
                p += 0x15;
            }
            size = D_80069334[0] * 0x1600 + 0x4210;
            buf = func_8001148C(size);
            D_8006933C = buf;
            if (buf != 0) {
                pad = buf & 0xF;
                if (pad != 0) {
                    pad = 0x10 - pad;
                }
                buf += pad;
                left = size - pad;
                D_80069348 = buf + 0x1000;
                D_8006934C = buf + 0x2000;
                D_80069344 = buf;
                D_80069350 = buf;
                D_80069340 = left;
                D_80069354 = buf + 0x2200;
                D_80069344 = buf + 0x4200;
                D_80069340 = left - 0x4200;
                return -0xFF;
            }
        }
        return -0xF;
    }
    return -0xFF;
}
