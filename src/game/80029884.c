#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F204 = 0xF424;

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029884);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029D0C);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A1B8);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A3B0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A554);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A8D0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B164);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B57C);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002BC38);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002BF58);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002C0F0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002C324);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CA1C);

extern char D_8006CB68[];
extern s32 D_8006CB6C;

void func_8002CA98(void) {
    *(char **)D_8006CB68 = D_8006CB68 + 0x414;
    D_8006CB6C = 0;
}

extern s32 D_8005BC18[];
s32 D_8005F378;
s32 D_8005F3C4;
s32 D_8005F3E8;

s32 func_8002CAB8(void) {
    s32 i;
    s32 *dst;
    s32 *src;

    D_8005F3C4 = 0x1F800100;
    D_8005F378 = 0x1F800180;
    D_8005F3E8 = 0x1F800380;
    for (i = 0, dst = (s32 *)0x1F800380, src = D_8005BC18; i < 0x20; i++) {
        *dst++ = *src++;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CB14);

void func_8002CB1C(s32 arg0, s32 arg1) {
    if (((arg0 < 0) ? (-arg0) : (arg0)) < 0x20) {
        if (((arg1 < 0) ? (-arg1) : (arg1)) < 0x20) {
            if (arg0 || arg1) {
                func_8003CBE4();
            } else {
                func_8003CBE4();
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CB68);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CC50);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002D6E4);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DE08);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DF68);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DFA8);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E280);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E628);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E6E0);

void func_8002E854(s32 *arg0, s32 *arg1, s32 *arg2) {
    s32 b = arg2[0];
    s32 a = *arg1;

    arg0[0] = a - (b - a);
    arg0[1] = arg2[1];
    arg0[2] = arg2[2];
}
