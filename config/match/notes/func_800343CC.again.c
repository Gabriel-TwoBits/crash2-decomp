#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk800343CC {
    s16 f0;
    s16 pad2;
    s32 f4;
    s32 pad8[2];
    s16 f10;
    s16 f12;
} Unk800343CC;

s16 D_8005F2F8;
u16 D_8005F308;
s16 g_SndSepAccess;
void func_80056760(s16, s16, s16, s16);

void func_800343CC(Unk800343CC *arg0, u32 arg1, s32 arg2) {
    if (D_8005F2F8 > 1 && arg2 == 1) {
        arg0->f12 = D_8005F308;
        arg0->f10 = D_8005F308;
    } else {
        arg0->f10 = (s32)arg1 >> 16;
        arg0->f12 = arg1;
    }
    arg0->f0 = arg2;
    arg0->f4 = -1;
    func_80056760(g_SndSepAccess, arg2, arg0->f10, arg0->f12);
}
