#include "common.h"
#include "sdk.h"
#include "game.h"

#define SLOT_FREE 1
#define SLOT_STATE_FIRST_RELEASABLE 0x14
#define SLOT_STATE_RELEASABLE_COUNT 2
#define NO_RUN -1

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 f_e;
    s8 f_f;
    u8 pad10[4];
    s32 f_14;
    s32 f_18;
    u8 pad1c[0xC];
    s32 f_28;
} Unk80013BD8;
typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;
extern Unk80013BD8Table D_800675B0;
extern char D_8005F1C8[];
s32 func_80012A20(Unk80012A20Obj *arg0);

s32 func_800136C8(s32 *arg0, s32 arg1) {
    s32 i, j, n, t, best, bestLen;
    u32 cost, bestCost;
    Unk80013BD8 *p;
    s32 cnt = D_800675B0.count;

    best = NO_RUN;
    bestLen = 0;
    bestCost = 0;
    cost = 0;
    for (i = 0; i < cnt; i++) {
        cost = 0;
        n = cnt - i;
        for (j = 0;; j++) {
            t = *arg0;
            if (n < t) {
                t = n;
            } else {
                /* t stays the requested length */
            }
            if (j >= t) {
                break;
            } else {
                p = &D_800675B0.e[i + j];
            }
            if (p->state != SLOT_FREE) {
                if ((u16)(p->state - SLOT_STATE_FIRST_RELEASABLE) < SLOT_STATE_RELEASABLE_COUNT && p->f_f <= 0) {
                    cost = cost + 1 + p->f_28;
                } else {
                    break;
                }
            } else {
                /* free slot: no cost */
            }
            if (best == NO_RUN || bestLen < j + 1 || (bestLen == j + 1 && cost < bestCost)) {
                best = i;
                bestLen = j + 1;
                bestCost = cost;
            } else {
                /* keep the earlier run */
            }
        }
    }
    if (best != NO_RUN) {
        for (j = best; j < best + bestLen; j++) {
            func_80012A20((Unk80012A20Obj *)&D_800675B0.e[j]);
        }
        *arg0 = bestLen;
        return best;
    } else {
        *arg0 = 0;
        if (arg1 != 0) {
            printf(D_8005F1C8);
        } else {
            /* silent failure */
        }
        return -1;
    }
}
