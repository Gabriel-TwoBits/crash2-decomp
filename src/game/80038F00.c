#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80038F00);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003900C);

extern s32 D_8006CB64;

typedef struct Unk80039104Src {
    char pad0[0x24];
    u8 *unk24;
} Unk80039104Src;

typedef struct Unk80039104 {
    char pad0[0xC];
    Unk80039104Src *unkC;
    char pad10[0x68];
    s32 unk78;
    char pad7C[0x40];
    s32 *unkBC;
    char padC0[0x28];
    u8 *unkE8;
    s32 unkEC;
} Unk80039104;

s32 func_80039104(Unk80039104 *arg0, u32 arg1, s32 arg2) {
    u8 *q;
    s32 *p;
    s32 t;

    q = arg0->unkC->unk24;
    p = arg0->unkBC;
    arg0->unkEC = (arg1 & 0x7F) << 8;
    arg0->unkBC = p + 1;
    arg0->unkE8 = q + ((arg1 >> 5) & 0x7FC);
    t = D_8006CB64;
    *p = (((arg1 >> 16) & 0x3F) << 24) | t;
    switch ((arg1 >> 22) & 3) {
    case 0:
        if (arg0->unk78 < 0) {
            arg0->unk78 = -arg0->unk78;
        }
        break;
    case 1:
        if (arg0->unk78 > 0) {
            arg0->unk78 = -arg0->unk78;
        }
        break;
    case 2:
        arg0->unk78 = -arg0->unk78;
        break;
    default:
        break;
    }
    return -((arg2 & 4) != 0) & ~0xFE;
}

typedef struct Unk800391D8 {
    char pad0[0x78];
    s32 unk78;
    char pad7C[0x40];
    s32 *unkBC;
    char padC0[0x2C];
    s32 unkEC;
} Unk800391D8;

s32 func_800391D8(Unk800391D8 *arg0, u32 arg1, s32 arg2) {
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 *sp;
    s32 t;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = arg0->unkBC - 1;
            arg0->unkBC = var_v0;
        } else {
            var_v0 = (s32 *)((u8 *)arg0 + ((arg1 & 0x1FF) * 4 + 0x40));
        }
    } else {
        var_v0 = func_80036D0C((struct Obj *)arg0, idx2);
    }
    sp = arg0->unkBC;
    arg0->unkEC = *var_v0;
    arg0->unkBC = sp + 1;
    /* The do/while (0) wrapper keeps the D_8006CB64 load after the stores to 0xEC/0xBC. */
    do {
        t =D_8006CB64;
        *sp = (((arg1 >> 16) & 0x3F) << 24) | t;
        switch ((arg1 >> 22) & 3) {
        case 0:
            if (arg0->unk78 < 0) {
                arg0->unk78 = -arg0->unk78;
            }
            break;
        case 1:
            if (arg0->unk78 > 0) {
                arg0->unk78 = -arg0->unk78;
            }
            break;
        case 2:
            arg0->unk78 = -arg0->unk78;
            break;
        default:
            break;
        }
        return -((arg2 & 4) != 0) & ~0xFE;
    } while (0);
}

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039310);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_800394C4);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039684);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003981C);

void func_80039A14(struct Obj *arg0, s32 arg1) {
    u8 *base = (u8 *)arg0;
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 temp_v0;
    s32 *sp;

    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = *(s32 **)(base + 0xBC) - 1;
            *(s32 **)(base + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)(base + ((arg1 & 0x1FF) * 4 + 0x40));
        }
    } else {
        var_v0 = func_80036D0C(arg0, idx2);
    }
    temp_v0 = func_80027664(var_v0);
    sp = *(s32 **)(base + 0xBC);
    *(s32 **)(base + 0xBC) = sp + 1;
    *sp = temp_v0;
}

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
