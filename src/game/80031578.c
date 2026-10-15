#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F224 = 1;

INCLUDE_ASM("asm/nonmatchings/game/80031578", func_80031578);

INCLUDE_ASM("asm/nonmatchings/game/80031578", func_80031658);

INCLUDE_ASM("asm/nonmatchings/game/80031578", func_80031668);

int CdIntToPos(int, unsigned char *);
int CdRead(int, unsigned long *, int);

s32 func_80031680(s32 arg0, s32 arg1, unsigned long *arg2) {
    u8 sp10[8];

    CdIntToPos(arg1, sp10);
    CdControl(2, sp10, 0);
    CdRead(arg0, arg2, 0x80);
    return CdReadSync(0, 0) == 0;
}
