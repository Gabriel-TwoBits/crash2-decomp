#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012010);

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_800120EC);

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012100);

void func_80012170(s32 a0, s32 a1, s32 a2, s32 a3);

void func_8001214C(s32 a0, s32 a1) {
    func_80012170(a0, a1, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012170);

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012250);

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012294);

INCLUDE_ASM("asm/nonmatchings/game/80012010", func_8001232C);

s32 func_800123BC(s32 *arg0, s32 arg1) {
    s32 *p;
    s32 r;

    if (arg0[3] == 0) {
        r = 0;
    } else {
        p = arg0 + arg1;
        r = p[5] - p[4];
    }
    return r;
}
