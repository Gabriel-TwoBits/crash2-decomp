#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Unk80030498;

s32 func_80030498(s32 arg0, s32 arg1, Unk80030498 *arg2) {
    s32 i;
    s32 found;
    s32 d0, d1, r;
    s32 s0, s1;
    s32 x1, y1, x2, y2, den;
    Unk80030498 *p;
    Unk80030498 *q;
    Unk80030498 *p1;
    Unk80030498 *q1;

    found = 0;
    p = arg2;
    for (i = 0; i < 4; i++) {
        q = &arg2[(i + 1) & 3];
        d1 = q->z - arg1;
        d0 = arg1 - p->z;
        if (d0 <= 0) {
            if (d1 > 0) {
                if (d0 >= 0) {
                    goto l4;
                }
                goto skip;
            }
            goto cross;
        }
l4:
        if (d1 >= 0) {
cross:
            if (!found) {
                found = 1;
                s0 = d0;
                s1 = d1;
                p1 = p;
                q1 = q;
                goto skip;
            }
            den = s0 + s1;
            if (den == 0) den = 1;
            r = (s0 << 12) / den;
            x1 = ((r * (q1->x - p1->x)) >> 12) + p1->x;
            y1 = ((r * (q1->y - p1->y)) >> 12) + p1->y;
            den = d0 + d1;
            if (den == 0) den = 1;
            r = (d0 << 12) / den;
            x2 = ((r * (q->x - p->x)) >> 12) + p->x;
            y2 = ((r * (q->y - p->y)) >> 12) + p->y;
            den = x2 - x1;
            if (den == 0) den = 1;
            r = ((arg0 - x1) << 12) / den;
            return ((r * (y2 - y1)) >> 12) + y1;
        }
skip:
        p = q;
    }
    return 0xC4653601;
}
