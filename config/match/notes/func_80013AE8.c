#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013AE8 {
    /* 0x00 */ s16 unk0[2];
    /* 0x04 */ s16 unk4;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ s16 unk8[2];
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s8 unkE;
    /* 0x0F */ s8 unkF;
    /* 0x10 */ s8 unk10[0x1C];
} Unk80013AE8;

extern Unk80013AE8 D_80062BB4[];

s32 func_80013AE8(s32 arg0, s32 arg1) {
    s32 i;
    Unk80013AE8 *p;

    for (i = 9; i >= 0; i--) {
        p = &D_80062BB4[i];
        if (p->unk4 == 1 && D_80062BB4[i].unk6 == arg0 &&
            (arg1 == -1 || arg1 == D_80062BB4[i].unkC)) {
            return i;
        }
    }
    if (arg0 == 5) {
        for (i = 9; i >= 0; i--) {
            p = &D_80062BB4[i];
            if ((u16)p->unk4 - 0x14 < 2U && D_80062BB4[i].unk6 == arg0 &&
                D_80062BB4[i].unkF == 0) {
                return i;
            }
        }
    }
    return -12;
}
