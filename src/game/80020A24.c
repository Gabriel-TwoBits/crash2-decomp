#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F1FC[] = "DirnD";

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80020A24);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800217F8);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021900);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021A08);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021F4C);

extern s8 D_8005B99B;
extern s32 D_8005B9C4;
extern s32 D_800608F8;

void func_800221DC(void) {
    D_800608F8 = 0;
    if (D_8005B99B != 0) {
        D_8005B9C4 = 0;
        return;
    }
    func_80023154();
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022224);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022314);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800224A0);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800225C0);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_8002271C);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022878);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022A5C);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022E74);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022F58);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80023084);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80023154);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800231B0);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800236DC);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800237F8);
