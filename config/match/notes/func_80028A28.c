#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8002A8D0(void *, s32 *, s32, s32, s32, s32, s32);
void *func_8003DA00(s32, void *, s32);
extern char *D_800608CC;
extern s32 D_8006CB6C;

typedef struct {
    s32 x, y, z;
} V3;

void *func_80028A28(void *unused, s32 *arg) {
    V3 pos;
    s32 box[10];
    s32 i;
    char *p;
    s32 *ent;
    void *res;
    void *best;
    s32 bestd;
    s32 d;
    s32 *q;
    char *o;

    pos = *(V3 *)arg;
    res = 0;
    p = *(char **)(D_800608CC + 0x10);
    while (1) {
        i = 0;
        if (*(s32 *)(p + 0x190) > 0) {
            do {
                if (*(s32 *)(p + 0x1B4 + i * 4) & 4) {
                    ent = func_80014260(p + 0x194 + i * 4, 1, 0);
                    if (func_80030D74(ent, arg) != 0) {
                        res = func_8003DA00(ent[5], arg, 9);
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
            do { } while (0);
            break;
        } else {
            continue;
        }
    }
    if (D_8006CB6C > 0) {
        bestd = -0x3B9AC9FF;
        best = 0;
        box[0] = pos.x - 0x4E20;
        box[3] = pos.x + 0x4E20;
        box[2] = pos.z - 0x4E20;
        box[5] = pos.z + 0x4E20;
        box[4] = pos.y;
        box[1] = pos.y;
        if (D_8006CB6C > 0) {
            q = &D_8006CB6C + 0x290 / 4;
            i = 0;
        again:
            o = *(char **)(*q + 4);
            if (*(s32 *)(o + 0xAC) & 0x20000) {
                d = func_8002A8D0((void *)*q, box, 1, 0, bestd, pos.y, 0);
                if (d != -0x3B9AC9FF) {
                    best = o;
                    bestd = d;
                }
            }
            i++;
            q++;
            if (i < D_8006CB6C) {
                goto again;
            }
        }
        if (best != 0 && (res == 0 || !(bestd < arg[1]))) {
            arg[1] = bestd;
            res = best;
        }
    }
    return res;
}
