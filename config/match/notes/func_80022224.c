#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80022224Inner {
    char pad[0x184];
    s32 base;
} Unk80022224Inner;

typedef struct Unk80022224 {
    char pad0[0x10];
    Unk80022224Inner *inner;
    s32 words[1];
} Unk80022224;

extern Unk80022224 *D_800608CC;
extern volatile s32 D_800608D4;
extern s32 D_800608D8;
extern s32 D_800608DC;
extern s32 D_800608FC;

void func_80022224(s32 arg0) {
    s32 t;
    s32 v;

    D_800608FC = arg0;
    if ((D_800608D4 != 0) && (D_800608CC != 0)) {
        t = func_800321D4(D_800608D4, 0x173, 0, 0, 0, 2, 0) * 3;
        v = *(s32 *)((char *)D_800608CC + (t + D_800608CC->inner->base) * 4 + 0x14);
        D_800608D8 = v;
        if (arg0 != 1) {
            D_800608DC = *(s32 *)((char *)D_800608CC + (t + arg0 + D_800608CC->inner->base) * 4 + 0x10);
            return;
        }
        D_800608DC = v;
    }
}
