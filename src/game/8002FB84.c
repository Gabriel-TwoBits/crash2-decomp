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

#ifdef NON_MATCHING
extern s32 D_8005F3D8;
extern s32 D_800675AC;
extern s32 D_8006CB20;

void func_8002FDCC(s32 *arg0) {
    int new_var2;
    s32 buf[2];
    s32 *new_var;
    s32 *temp_v0;

    D_800675AC = 0;
    temp_v0 = func_80014260(arg0, 1, 1);
    do { new_var2 = 0x6396347F; new_var = arg0; } while (0);
    *new_var = new_var2;
    D_8005F3D8 = temp_v0[4];
    buf[1] = 0x6400;
    buf[0] = 0;
    func_80019800(&D_8006CB20, 4, 8, 2, (s32)buf, 1);
    func_8002FCF8();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FDCC);
#endif
