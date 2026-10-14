#include "common.h"
#include "sdk.h"

s32 D_8005F1F4 = 0;
s32 D_8005F1F8 = 1;

INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001F9C4);

INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001FDDC);

INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001FF30);

INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001FF50);

INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001FFCC);

extern void func_80019E28();
extern char *D_8005F38C;
extern int D_800608D0;

void func_8002001C(char *arg0) {
    char *s1 = *(char **)(arg0 + 0x10);

    do {
        if (*(unsigned *)(s1 + 0x27C) & 1) {
            func_80019E28();
            if (*(char **)(D_8005F38C + 0x18) == arg0) {
                *(int *)(0x18 + D_8005F38C) = D_800608D0;
            }
            *(unsigned *)(s1 + 0x27C) &= ~3;
        }
    } while (0);
}
