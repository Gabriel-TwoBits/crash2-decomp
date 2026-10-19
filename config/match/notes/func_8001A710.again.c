#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk8001A710 {
    /* 0x00 */ u8 pad0[0x58];
    /* 0x58 */ struct Unk8001A710 *link;
    /* 0x5C */ u8 pad5C[0x4];
    /* 0x60 */ s32 pos[3];
    /* 0x6C */ u8 pad6C[0x3C];
    /* 0xA8 */ s32 flagsA8;
    /* 0xAC */ u8 padAC[0x54];
    /* 0x100 */ s32 flags100;
    /* 0x104 */ u8 pad104[0x24];
    /* 0x128 */ s32 offset;
} Unk8001A710;

s32 func_8001A710(Unk8001A710 *arg0, s32 *arg1, Unk8001A710 *arg2, s32 *arg3) {
    s32 box0[6];
    s32 box1[6];
    Unk8001A710 *other;
    s32 d;

    other = arg0->link;
    if (other != 0 && other != arg2) {
        if (arg2->flags100 & 0x800) {
            arg2->link = arg0;
            return -0x1C;
        }
        d = Math_ApproxDistance(arg0->pos, other->pos);
        if (!(Math_ApproxDistance(arg0->pos, arg2->pos) < d) && !(other->flags100 & 0x800)) {
            return -0x1C;
        }
    }
    arg0->link = arg2;
    arg2->link = arg0;
    if (arg1 == 0) {
        return -0xFF;
    }
    if (arg3 != 0) {
        if (arg0->offset != 0) {
            box0[0] = arg1[0] + arg0->offset;
            box0[1] = arg1[1];
            box0[2] = arg1[2] + arg0->offset;
            box0[3] = arg1[3] - arg0->offset;
            box0[4] = arg1[4];
            box0[5] = arg1[5] - arg0->offset;
            if (func_80030EC0(box0, arg3) != 0) {
                arg0->flagsA8 |= 0x1000;
            }
        }
        if (arg2->offset != 0) {
            box1[0] = arg3[0] + arg2->offset;
            box1[1] = arg3[1];
            box1[2] = arg3[2] + arg2->offset;
            box1[3] = arg3[3] - arg2->offset;
            box1[4] = arg3[4];
            box1[5] = arg3[5] - arg2->offset;
            if (func_80030EC0(box1, arg1) != 0) {
                arg2->flagsA8 |= 0x1000;
            }
        }
    }
    return -0xFF;
}
