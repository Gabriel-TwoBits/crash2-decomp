#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019098);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019800);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019988);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019BBC);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019D30);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019E08);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019E28);

typedef struct {
    u8 pad[0x190];
    s32 count;
    s32 items[1];
} Unk19E5C;

typedef struct {
    u8 pad[0x10];
    Unk19E5C *unk10;
} Unk608CC;

/* volatile: the retail code reloads it after the null check. */
extern Unk608CC *volatile D_800608CC;

void func_80019E5C(void) {
    Unk19E5C *obj;
    s32 i;

    if (D_800608CC != 0) {
        obj = D_800608CC->unk10;
        for (i = 0; i < obj->count; i++) {
            if (func_800143C0(&obj->items[i], 0) != 0) {
                func_80019E28(func_80014260(&obj->items[i], 1, 0));
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019F08);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019FA0);
