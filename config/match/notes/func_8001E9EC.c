#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80018194(s32 *, s32, s32 *);

void func_8001E9EC(s32 *p, s32 *a, s32 *d) {
    s32 out[3];
    s32 pad;
    s32 in[3];
    s32 t = a[0];
    s32 v = p[2];

    in[0] = t / 16;
    in[1] = a[1] / 16;
    in[2] = a[2] / 16;
    func_80018194(in, v, out);
    out[0] *= 16;
    out[1] *= 16;
    out[2] *= 16;
    d[0] = out[0] + p[0x60 / 4];
    d[1] = out[1] + p[0x64 / 4];
    d[2] = out[2] + p[0x68 / 4];
}
