#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80038F00);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003900C);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039104);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_800391D8);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039310);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_800394C4);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039684);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003981C);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039A14);

void func_80039AA4(struct Obj *arg0, s32 arg1) {
    s32 *var_v0;
    s32 temp_a2;
    s32 a3;

    temp_a2 = arg1 & 0xFFF;
    a3 = temp_a2;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a2 == 0xE1F) {
            var_v0 = *(s32 **)((u8 *)arg0 + 0xBC) - 1;
            *(s32 **)((u8 *)arg0 + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)((u8 *)arg0 + (((arg1 & 0x1FF) * 4) + 0x40));
        }
    } else {
        var_v0 = func_80036D0C(arg0, a3);
    }
    func_800278F8(*var_v0);
}

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039B14);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039C08);
