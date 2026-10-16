#include "common.h"
#include "sdk.h"
#include "game.h"

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
