#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Unk80014148;
extern s32 *D_80067FC4;

s32 func_80014918(u32 **arg0, s32 arg1) {
    s32 sp10[42];
    Unk80014148 *e;
    s32 *out;
    s32 cnt;
    u32 **in;
    s32 *base;
    u32 *r;
    s32 n;
    s32 i;
    s32 j;
    s32 v;
    s32 w;

    n = 0;
    i = 0;
    cnt = 0;
    if (arg1 > 0) {
        base = sp10;
        out = base;
        in = arg0;
        do {
            r = (u32 *)**in;
            if ((u32)r & 1) {
                r = (u32 *)func_80014B44((u32)r);
                **in = (u32)r;
            }
            w = *r;
            if (!(w & 1)) {
                e = func_80014148(w);
                if (e != 0 && e->kind != 2 && (e->pad == 1 || *((s8 *)e + 0xF) == 0)) {
                    for (j = 0; j < n; j++) {
                        if (*(s32 *)((u8 *)e + 0x14) == base[j]) break;
                    }
                    if (j == n) {
                        v = *(s32 *)((u8 *)e + 0x14);
                        cnt++;
                        goto store;
                    }
                }
            } else {
                v = D_80067FC4[w >> 1];
                if (v == -0x12) {
                    for (j = 0; j < n; j++) {
                        if (w == base[j]) break;
                    }
                    if (j == n) {
                        v = *r;
                        cnt++;
                        goto store;
                    }
                } else if (v != -0xFF && *(s16 *)(v + 6) < 2 && *(s8 *)(v + 0xF) == 0) {
                    for (j = 0; j < n; j++) {
                        if (*(s32 *)(v + 0x14) == base[j]) break;
                    }
                    if (j == n) {
                        v = *(s32 *)(v + 0x14);
                        cnt++;
                        do { store: n++; *out = v; out++; } while (0);
                    }
                }
            }
            i++;
            in++;
        } while (i < arg1);
    }
    return cnt;
}
