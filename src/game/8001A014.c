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

#ifdef NON_MATCHING
void func_8001A13C(s32 arg0, s32 a1, s32 a2) {
    s32 arg2 = a2;
    s32 arg1 = a1;
    s32 sp28;
    s32 temp_v0;
    s32 var_a1;
    s32 var_a2;

    var_a1 = 0x13C;
    if (arg2 == 2 && (var_a1 = 0x13B, arg2 == 2)) {
        var_a2 = arg1 - 0x100;
    } else {
        var_a2 = arg1 + 0x100;
    }
    temp_v0 = func_80031AE8(arg0, var_a1, var_a2, 0, &sp28, 1, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        func_8001A054(0x13B, temp_v0, sp28);
    }
    var_a1 = (arg2 == 2) ? 0x13C : 0x13B;
    temp_v0 = func_80031AE8(arg0, var_a1, arg1, 0, &sp28, 1, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        func_8001A054(0x13C, temp_v0, sp28);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A13C);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A23C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A4A4);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A710);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A90C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001AA80);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001ABF8);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001B83C);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BD84);

INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001BFD4);
