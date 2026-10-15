#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[6];
    s16 f6;
    u8 pad8[6];
    s8 fE;
    s8 fF;
} Unk80013EB0;

extern s32 D_80067FB0[];

void func_80013EB0(Unk80013EB0 *arg0) {
    s8 d = arg0->fE;

    if (d != 0) {
        arg0->fF = arg0->fF - d;
        arg0->fE = 0;
        if (arg0->fF <= 0 && arg0->f6 == 0) {
            func_80013F28(arg0);
            *D_80067FB0 -= 1;
        }
    }
}
