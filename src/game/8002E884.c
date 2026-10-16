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

#ifdef NON_MATCHING
extern s32 D_8006935C[];
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC84;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;
extern s32 D_8006CB98;
extern s32 D_8006CBCC;
extern s32 D_8006CBD0;
extern s32 D_8006CBD4;
extern s32 D_8006CBD8;
extern s32 D_8006CBDC;
extern s32 D_8006CBE0;
extern s32 D_8006CC28;
extern s32 D_8006CC2C;
extern s32 D_8006CC30;
extern s32 D_8006CC34;
extern s32 D_8006CC50;
extern s32 D_8006CC54;
extern s32 D_8006CC6C;
extern s32 D_8006CC70;
extern s32 D_8006CC88;
extern s32 D_8006CC8C;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 D_8006CD7C;
extern s32 D_8006CDA4;
extern s32 D_8006CDA8;
extern s32 D_8006CDC8;

void func_8002F0DC(s32 arg0) {
    s32 t;
    s32 *p;
    if (arg0 != 0) {
        p = D_80069718;
        t = D_8006CBEC;
        D_8006CD7C = 0;
        D_8006CD20 = 0;
        D_8006CB84 = 0;
        D_8006CC00 = 0;
        D_8006CD40 = 0;
        D_8006CBD4 = 0;
        D_8006CBD8 = 0;
        D_8006CBDC = 0;
        D_8006CBE0 = 0;
        D_8006CC28 = 0;
        D_8006CC2C = 0;
        D_8006CDA4 = 0;
        D_8006CDA8 = 0;
        D_8006CDC8 = 0;
        D_8006CC50 = 0;
        D_8006CC54 = 0;
        D_8006CC30 = 0;
        D_8006CC34 = 0;
        D_8006CC84 = -1;
        D_8006CBC0 = 0x63;
        D_8006CCD4 = 0x63;
        D_8006CCD8 = 0;
        D_8006CB98 = 0;
        D_8006CC6C = 0;
        D_8006CC70 = 0;
        D_8006CC88 = 0;
        D_8006CC8C = 0;
        D_8006CDB4 = -1;
        D_8006CDB8 = 0;
        D_8006CBD0 = t;
        memset(p, 0, 0x200);
        memset(D_8006935C, 0, 0x200);
        D_8006CBCC = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F0DC);
#endif

extern s32 D_80068FE4;
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC68;
extern s32 D_8006CC74;
extern s32 D_8006CC84;
extern s32 D_8006CC94;
extern s32 D_8006CC98;
extern s32 D_8006CCA0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CD68;
extern s32 D_8006CD90;
extern s32 D_8006CD9C;
extern s32 D_8006CDA0;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;

void func_8002F228(s32 arg0) {
    int new_var;
    s32 *new_var2;

    D_8006CC94 = 0;
    D_8006CC68 = 0;
    D_8006CC74 = 0;
    if (D_8006CD14 != 2) {
        D_8006CCA0 = 0;
    }
    D_8006CC98 = 1;
    D_8006CD68 = 0;
    D_8006CD90 = 0;
    if (arg0 != 0) {
        new_var2 = (s32 *)D_8006759C[0] + 2;
        D_8006CD9C = 0;
        if (1) {
            D_8006CDA0 = 0;
            D_8006CB84 = 0;
            D_8006CC00 = 0;
        }
        D_8006CD40 = 0;
        D_8006CD20 = 0;
        D_8006CC84 = -1;
        new_var = (*new_var2) * 2;
        D_8006CDB4 = 0;
        D_8006CDB8 = (D_80069718[new_var] & 0xF) << 8;
    }
    func_80029884(0, 0, 0);
    D_80068FE4 = 0;
    func_8002FB84(1);
    D_8006CD1C = 0x20;
    if (D_8006CBB4 != 0x600) {
        D_8006CD18 = 0x120;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F34C);

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F6C8);
