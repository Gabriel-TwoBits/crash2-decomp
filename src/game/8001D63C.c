#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005B88C[];

u8 *D_8005F1EC = D_8005B88C;
s32 D_8005F1F0 = 0x80;

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001D63C);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E3DC);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E544);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E5FC);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E640);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E780);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E8E4);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E9EC);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EAD8);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EC2C);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001ED60);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EE64);

extern s32 D_8005F38C;
extern s32 D_8006CB10;
extern s32 D_8006CB14;

void func_8001EFD8(void) {
    func_800116C0(D_8006CB14);
    D_8005F38C = 0;
    func_800116C0(D_8006CB10);
}

extern void *D_8006759C;
extern s32 D_8006CB70;

s32 func_8001F018(void) {
    D_8006CB70 = *(s32 *)((s8 *)D_8006759C + 8) << 8;
    if (*(s32 *)((s8 *)D_8006759C + 4) == 0) {
        return -0xE;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F054);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F858);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F8B4);

INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F94C);
