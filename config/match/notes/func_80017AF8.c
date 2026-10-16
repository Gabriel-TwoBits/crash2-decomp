#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80017AF8 {
    u16 a, b, c;
    s16 d, e, f;
    s16 g, h, i;
    s16 pad;
    s32 x, y, z;
} Unk80017AF8;

void func_80017AF8(Unk80017AF8 *dst, Unk80017AF8 *src) {
    dst->a = src->a;
    dst->b = src->b;
    dst->c = src->c;
    dst->d = -(src->d * 5) >> 3;
    dst->e = -(src->e * 5) >> 3;
    dst->f = -(src->f * 5) >> 3;
    dst->g = -src->g;
    dst->h = -src->h;
    dst->i = -src->i;
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
