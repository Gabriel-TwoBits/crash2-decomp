#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F210 = 0xA;
s32 D_8005F214 = 0x7F;

#ifdef NON_MATCHING
extern s32 D_8005F180;
extern s32 D_8005F4AC[16];

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
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FB84);
#endif

s32 func_8002FC78(void *arg0, s32 arg1, s32 arg2) {
    s32 *p = (s32 *)((char *)func_80014260(arg0, 1, 0) + 0x14);
    return p[arg1 * 2 + arg2] + 3;
}

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FCD0);

void func_8002FCE8(void) {
}

void func_8002FCF0(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FCF8);

INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FDCC);
