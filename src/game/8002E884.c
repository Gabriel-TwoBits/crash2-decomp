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

extern s32 D_8006CBB4;
extern s32 D_8006CBB8;
extern s32 D_8006CBBC;
extern s32 D_8006CBC0;
extern s32 D_8006CBC4;
extern s32 D_8006CBEC;
extern s32 D_8006CBF0;
extern s32 D_8006CBF4;
extern s32 D_8006CBF8;
extern s32 D_8006CBFC;
extern s32 D_8006CC80;
extern s32 D_8006CCEC;
extern s32 D_8006CCF0;
extern s32 D_8006CD24;
extern s32 D_8006CD3C;
extern s32 D_8006CDCC;
extern s32 D_8006CDD0;
extern s32 D_8006CDD4;
extern s32 D_8006CDD8;
extern s32 D_8006CDDC;
extern s32 D_8006CDE0;
extern s32 D_8006CDE4;
extern s32 D_8006CDE8;

void func_8002EFE4(void) {
    D_8006CBC0 = 0x63;
    D_8006CBBC = -1;
    D_8006CBB8 = 7;
    D_8006CBF8 = 0x100;
    D_8006CBFC = 0x100;
    D_8006CBEC = 0x400;
    D_8006CCEC = 1;
    D_8006CDD0 = 0x80;
    D_8006CDD4 = 0x80;
    D_8006CBC4 = 0;
    D_8006CBF0 = 0;
    D_8006CBF4 = 0;
    D_8006CC80 = 0;
    D_8006CD24 = 0;
    D_8006CD3C = 0;
    D_8006CCF0 = 0;
    D_8006CDE8 = 0;
    D_8006CDCC = 0;
    D_8006CDD8 = 0;
    D_8006CDDC = 0;
    D_8006CDE0 = 0xFF;
    D_8006CDE4 = 0xFF;
    D_8006CBB4 = 0;
    func_8002F0DC(1);
}

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F0DC);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F228);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F34C);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F6C8);
