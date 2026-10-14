#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80063034;

s32 D_8005F3B8;
extern Unk80063034 D_80063034[];

void func_8001A014(void) {
    s32 n;
    s32 i;

    n = D_8005F3B8;
    /* Empty statement: without it the frame adjustment is scheduled before the load. */
    do {
    } while (0);
    for (i = 0; i < n; i++) {
        D_80063034[i].unk0 = 0x13B;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A054);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A13C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A23C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A4A4);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A710);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A90C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001AA80);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001ABF8);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001B83C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BD84);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BFD4);
