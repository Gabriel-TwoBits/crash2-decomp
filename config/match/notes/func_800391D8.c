#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8006CB64;

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
