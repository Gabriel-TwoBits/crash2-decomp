#include "common.h"
#include "sdk.h"
#include "game.h"

#define TYPE_A 2
#define TYPE_B 5
#define STRIDE_A 16
#define OFFSET_A 8
#define STRIDE_B 24
#define OFFSET_B 12
#define MODE_BOUNDS 1
#define MODE_VRAM 2

void func_80017380(s32 mode, u8 *base, s32 arg2, u16 *out) {
    u32 flags;
    s32 sub;
    s32 minx, miny, maxx, maxy;
    s32 t, u, w, h;
    u8 *p;

    /* p stays unassigned for other types; retail reads the caller's $s0 */
    if (base[0] == TYPE_A) {
        p = base + (arg2 >> 8) * STRIDE_A + OFFSET_A;
    } else if (base[0] == TYPE_B) {
        p = base + (arg2 >> 8) * STRIDE_B + OFFSET_B;
    }
    func_80018474(p + 4, base + 4);
    flags = *(u16 *)(p + 0xA);
    sub = (flags >> 7) & 3;
    if (mode == MODE_BOUNDS) {
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
    } else if (mode == MODE_VRAM) {
        out[0] = (*(u16 *)(p + 6) & 0x3F) * 16;
        out[1] = (*(u16 *)(p + 6) >> 6) & 0x1FF;
        if (sub == 0) {
            out[2] = 0x10;
        } else {
            out[2] = 0x100;
        }
        out[3] = 1;
    } else {
        /* other modes write nothing */
    }
}
