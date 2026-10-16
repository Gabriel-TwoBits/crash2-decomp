#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013C98 {
    s32 f0;
    s16 f4;
    s16 f6;
    s32 f8;
    u8 pad_c[2];
    u8 fE;
    u8 fF;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
} Unk80013C98;

s32 func_800136C8(s32 *, s32);
extern Unk80013C98 D_80067588[];
extern Unk80013C98 **D_80067FC4;

Unk80013C98 *func_80013C98(s32 *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4) {
    Unk80013C98 *src;
    Unk80013C98 *dst;
    Unk80013C98 *base;
    s32 idx;
    s32 i;

    idx = func_800136C8(arg3, arg4);
    if (idx == -1) {
        return (Unk80013C98 *)-0xC;
    }
    dst = &D_80067588[idx + *arg3];
    if (arg0 != 0) {
        base = dst;
        for (i = *arg3 - 1; i >= 0; i--) {
            src = D_80067FC4[(arg0[5] >> 1) + i];
            dst->f14 = src->f14;
            if (arg2 != 0x6396347F) {
                dst->f18 = arg2;
            } else {
                dst->f18 = src->f18;
            }
            src->f4 = 1;
            dst->fF = src->fF;
            src->fF = 0;
            dst->fE = src->fE;
            dst->f28 = src->f28;
            dst->f24 = (s32)base;
            dst->f1C = *arg3;
            if (i == *arg3 - 1) {
                dst->f4 = 2;
            } else {
                dst->f4 = 0xC;
            }
            idx = dst->f14;
            D_80067FC4[idx >> 1] = dst;
            dst--;
        }
        return base;
    }
    return dst;
}
