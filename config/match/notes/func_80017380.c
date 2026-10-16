#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80017380(s32 mode, u8 *base, s32 arg2, u16 *out) {
    u8 *p;
    u32 flags;
    s32 sub;
    s32 minx, miny, maxx, maxy;
    s32 t, u, w, h;

    if (base[0] == 2) {
        p = base + (arg2 >> 8) * 16 + 8;
    } else if (base[0] == 5) {
        p = base + (arg2 >> 8) * 24 + 12;
    }
    func_80018474(p + 4, base + 4);
    flags = *(u16 *)(p + 0xA);
    sub = (flags >> 7) & 3;
    switch (mode) {
    case 2:
        out[0] = (*(u16 *)(p + 6) & 0x3F) * 16;
        out[1] = (*(u16 *)(p + 6) >> 6) & 0x1FF;
        if (sub == 0) {
            out[2] = 0x10;
        } else {
            out[2] = 0x100;
        }
        out[3] = 1;
        return;
    case 1:
        minx = p[4];
        miny = p[5];
        maxx = minx;
        maxy = miny;
        t = p[8];
        if (t < minx) minx = t;
        u = p[9];
        if (u < miny) miny = u;
        if (maxx < t) maxx = t;
        if (maxy < u) maxy = u;
        t = p[12];
        if (t < minx) minx = t;
        u = p[13];
        if (u < miny) miny = u;
        if (maxx < t) maxx = t;
        if (maxy < u) maxy = u;
        out[0] = (flags & 0xF) << 6;
        w = maxx - minx + 1;
        h = maxy - miny + 1;
        out[1] = ((*(u16 *)(p + 0xA) << 4) & 0x100) + miny;
        if (sub == mode) {
            out[2] = w / 2;
            out[0] = out[0] + (minx >> 1);
        } else if (sub < 2) {
            if (sub != 0) {
                out[3] = h;
                return;
            }
            out[0] = out[0] + minx / 4;
            out[2] = w / 4;
        } else if (sub == 2) {
            out[2] = w;
            out[0] = out[0] + minx;
        } else {
            out[3] = h;
            return;
        }
        out[3] = h;
        return;
    default:
        return;
    }
}
