#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Slot80027 {
    s32 pad0[2];
    s32 used;
    s32 pad1;
    u32 size;
    s32 pad2;
    s32 w, h, bpp, arg3, stride, bytes, extra;
    s32 data, end, f3C, f40, f44, f48;
    s32 pad3[2];
} Slot80027;

typedef struct Pool80027 {
    s32 count;
    Slot80027 *slots;
    s32 pad;
    u32 free;
    s32 cursor;
    s32 a, b, c, d;
} Pool80027;

extern s32 D_80069338;

void *func_800277A8(s32 w, s32 h, s32 bpp, s32 arg3) {
    Pool80027 *pool;
    Slot80027 *s;
    s32 stride, bytes, extra, i, a;
    u32 total;

    if (bpp == 16) {
        stride = w * 2;
        extra = 0;
    } else if (bpp < 17) {
        if (bpp == 4) {
            stride = w >> 1;
            extra = 32;
        } else {
            stride = w;
            extra = 512;
        }
    } else if (bpp == 24) {
        stride = w * 3;
        extra = 0;
    } else {
        stride = w;
        extra = 512;
    }
    s = *(Slot80027 **)&D_80069338;
    i = 0;
    pool = (Pool80027 *)((char *)&D_80069338 - 4);
    bytes = h * stride;
loop:
    if (i >= *(s32 *)((char *)&D_80069338 - 4)) return 0;
    i++;
    if (s->used != 0) {
        s++;
        goto loop;
    }
    a = (bytes + 15) & ~15;
    total = a + ((extra + 15) & ~15);
    if (pool->free < total) return 0;
    s->used = 1;
    s->size = total;
    s->data = pool->cursor;
    s->end = pool->cursor + a;
    pool->cursor += total;
    pool->free -= total;
    s->f3C = pool->a;
    s->f40 = pool->b;
    s->f44 = pool->c;
    s->w = w; s->h = h; s->bpp = bpp; s->arg3 = arg3;
    s->stride = stride; s->bytes = bytes; s->extra = extra;
    s->f48 = pool->d;
    return s;
}
