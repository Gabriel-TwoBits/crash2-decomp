#include "common.h"
#include "sdk.h"
#include "game.h"

#define FLAG_ACTIVE 0x04000000
#define SEARCH_NONE (-0x3B9AC9FF)
#define SLOT_FLAG_MASK 0x40020000
#define SLOT_FLAG_VALUE 0x20000
#define KIND_MASK 7
#define KIND_ALT 1
#define ALT_BOX_OFFSET 0x2C
#define BOX_OFFSET 8
#define BOX_MARGIN 0x88B8
#define LIST_OFFSET 0x290
#define ID_MASK 0x3C00
#define ID_SHIFT 10
#define FIXED_SHIFT 8

extern s32 D_8006CB6C;

/* func_8003DA00 takes six more stack arguments than game.h's prototype. */
typedef void *(*Func8003DA00Full)(s32, void *, s32, s32, s32, s32, s32, s32, s32);

void func_80028C48(s32 unit, s32 *arg, s32 arg2) {
    char *arg0 = (char *)unit;
    s32 x, y, z;
    s32 i;
    char *p;
    s32 *ent;
    s32 *rng;
    void *res;
    void *best;
    s32 bestd;
    s32 d;
    s32 *q;
    s32 *a;
    char *o;
    s32 hit;

    bestd = SEARCH_NONE;
    res = 0;
    best = 0;
    hit = 0;
    if (!(*(s32 *)(arg0 + 0xAC) & FLAG_ACTIVE)) {
        return;
    }
    p = *(char **)(D_800608CC + 0x10);
    x = arg[0];
    y = arg[1];
    z = arg[2];
    while (1) {
        i = 0;
        if (*(s32 *)(p + 0x190) > 0) {
            do {
                if (*(s32 *)(p + 0x1B4 + i * 4) & 4) {
                    ent = func_80014260(p + 0x194 + i * 4, 1, 0);
                    if (func_80030D74(ent, arg) != 0) {
                        rng = (s32 *)ent[5];
                        res = ((Func8003DA00Full)func_8003DA00)(
                            (s32)rng, arg, 1, rng[0] << FIXED_SHIFT, rng[1] << FIXED_SHIFT,
                            rng[2] << FIXED_SHIFT, rng[3] << FIXED_SHIFT, rng[4] << FIXED_SHIFT,
                            rng[5] << FIXED_SHIFT);
                        break;
                    }
                }
                i++;
            } while (i < *(s32 *)(p + 0x190));
        }
        if (i == *(s32 *)(p + 0x190)) {
            res = 0;
            break;
        } else if (res != 0) {
            break;
        } else {
            continue;
        }
    }
    if (D_8006CB6C > 0) {
        q = (s32 *)((char *)&D_8006CB6C + LIST_OFFSET);
        for (i = 0; i < D_8006CB6C; i++, q++) {
            o = *(char **)(*q + 4);
            if ((*(s32 *)(o + 0xAC) & SLOT_FLAG_MASK) != SLOT_FLAG_VALUE) {
                continue;
            }
            if ((*(s32 *)*q & KIND_MASK) == KIND_ALT) {
                a = (s32 *)(*q + ALT_BOX_OFFSET);
            } else {
                a = (s32 *)(*q + BOX_OFFSET);
            }
            d = a[4];
            if (x < a[0] - BOX_MARGIN || a[3] + BOX_MARGIN < x || z < a[2] - BOX_MARGIN ||
                a[5] + BOX_MARGIN < z) {
                continue;
            }
            if (!(d < arg[1]) && !(arg[1] < a[1])) {
                res = o;
                arg[1] = d;
                hit = 1;
                goto done;
            }
            if (bestd < d && !(y < d)) {
                best = o;
                bestd = d;
            }
        }
        if (best != 0 && (res == 0 || !(bestd > arg[1]))) {
            arg[1] = bestd;
            res = best;
            hit = 1;
        }
    }
done:
    if (res != 0) {
        if (hit != 0) {
            func_8002890C(*(u8 **)(arg0 + 0x44), -1, *(s32 *)((char *)res + 0x118));
        } else {
            func_8002890C(*(u8 **)(arg0 + 0x44), ((s32)res & ID_MASK) >> ID_SHIFT, -1);
        }
    } else {
        func_8002890C(*(u8 **)(arg0 + 0x44), 0, -1);
    }
}
