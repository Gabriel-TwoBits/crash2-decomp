#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F180;
extern s32 D_8005F4AC[16];
s32 D_8005F210;
s32 D_8005F214;

void func_8002FB84(s32 i) {
    s32 *p;
    s32 v;

    if (i != 0) {
        i = 0;
        do {
            D_8005F4AC[i] = ((D_8005F214 + 1) / 8) * i - D_8005F214;
            i++;
        } while (i < 16);
        return;
    }
    if (D_8005F180 == 0) {
        do {
            D_8005F4AC[i] += D_8005F210;
            if (D_8005F214 < D_8005F4AC[i]) {
                D_8005F4AC[i] = -D_8005F214 + 1;
            }
            i++;
        } while (i < 16);
        i = 0;
    }
    p = D_8005F4AC;
    do {
        v = *p;
        if (v < 0) {
            v = -v;
        }
        ((s8 *)0x1F800100)[i] = v;
        i++;
        p++;
    } while (i < 16);
}
