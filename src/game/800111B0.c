#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F17C = 0;
s32 D_8005F180 = 0;
s32 D_8005F184 = 0x1000;
s32 D_8005F188 = 0x200000;
s32 D_8005F18C = 0;
char D_8005F190[] = "dead\n";

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800111B0);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800112A4);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_8001144C);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_8001148C);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116C0);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116D4);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011764);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800117BC);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011800);
