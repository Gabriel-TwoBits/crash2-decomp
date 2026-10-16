#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 a, b, c; } Ent;
s32 D_8005F3B8;
extern Ent D_8006302C[];
extern Ent D_80063030[];
extern Ent D_80063034[];

void func_8001A054(s32 arg0, s32 *arg1, s32 arg2) {
    s32 *src;
    s32 n;
    s32 i;
    s32 v;

    n = arg2 - 1;
    src = arg1 + n;
    for (; n >= 0; n--, src--) {
        for (i = 0; i < D_8005F3B8; i++) {
            v = *src;
            if ((D_8006302C[i].a & 0xFFFF00) == (v & 0xFFFF00)) {
                D_8006302C[i].a = v;
                D_80063034[i].a = arg0;
                break;
            }
        }
        if (i == D_8005F3B8) {
            D_80063034[i].a = arg0;
            D_80063030[i].a = 0;
            D_8005F3B8 = i + 1;
            D_8006302C[i].a = *src;
        }
    }
}
