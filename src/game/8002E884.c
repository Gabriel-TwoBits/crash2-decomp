#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F208[] = "vcm?X";

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002E884);

extern s32 D_8005C514;
extern s32 D_8005F3EC;
extern u8 *D_8006759C[];
extern s32 D_8006CB20;
extern s32 D_8006CB88;
extern s32 D_8006CB8C;
extern s32 D_8006CBA0;
extern s32 D_8006CBA8;
extern s32 D_8006CBE4;

void func_8002EE48(void) {
    func_800158E0();
    D_8006CBA0 = 0;
    switch (D_8005F3EC) {
    case 0x28:
    case 0x29:
    case 0x3B:
    case 0x3C:
        break;
    default: {
        s32 *cb20 = &D_8006CB20;
        u8 **p;
        D_8006CB8C = func_80019800(cb20, 4, 0, 0, 0, 0);
        D_8006CB88 = func_80019800(cb20, 4, 1, 0, 0, 0);
        D_8006CBA8 = func_80019800(cb20, 4, 5, 0, 0, 0);
        D_8006CBE4 = func_80019800(cb20, 4, 0x16, 0, 0, 0);
        func_80014260(D_8006759C[0] + 0x10, 0, 1);
        func_80014260(D_8006759C[0] + 0x24, 0, 1);
        func_80014260(D_8006759C[0] + 0x84, 0, 1);
        func_80014260(D_8006759C[0] + 0x98, 0, 1);
        func_80014260(D_8006759C[0] + 0x1C, 0, 1);
        func_80014260(D_8006759C[0] + 0x20, 0, 1);
        func_80014260(D_8006759C[0] + 0x34, 0, 1);
        break;
    }
    }
    func_8002F228(1);
    *(s32 **)0x1F80005C = &D_8005C514;
}

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002EFE4);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F0DC);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F228);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F34C);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F6C8);
