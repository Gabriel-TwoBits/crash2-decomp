#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[9];
    s8 f_f;
    u8 pad10[0x18];
    s32 f_28;
} Unk80013BD8;
typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;
extern Unk80013BD8Table D_800675B0;
extern char D_8005F1C8[];
void func_80012A20(void *);

s32 func_800136C8(s32 *arg0, s32 arg1) {
    s32 i, j, n, t, best, bestLen;
    u32 cost, bestCost;
    Unk80013BD8 *p;
    Unk80013BD8Table *tab = &D_800675B0;
    s32 cnt = tab->count;
    Unk80013BD8 *e = tab->e;

    best = -1;
    i = 0;
    if (cnt > 0) {
        cost = 0;
        do {
            j = 0;
            n = cnt - i;
        again:
            t = *arg0;
            if (n < t) t = n;
            if (j < t) {
                p = &e[i + j];
                if (p->state != 1) {
                    if ((u16)(p->state - 0x14) < 2 && p->f_f <= 0) {
                        cost = cost + 1 + p->f_28;
                    } else goto next;
                }
                if (best == -1 || bestLen < j + 1 || (bestLen == j + 1 && cost < bestCost)) {
                    best = i;
                    bestLen = j + 1;
                    bestCost = cost;
                }
                j++;
                goto again;
            }
        next:
            i++;
            cost = 0;
        } while (i < cnt);
    }
    if (best != -1) {
        for (j = best; j < best + bestLen; j++) func_80012A20(&D_800675B0.e[j]);
        *arg0 = bestLen;
        return best;
    }
    *arg0 = 0;
    if (arg1 != 0) printf(D_8005F1C8);
    return -1;
}
