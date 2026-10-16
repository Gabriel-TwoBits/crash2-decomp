#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80014790 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 fE;
    s8 fF;
    u8 pad10[4];
    s32 f14;
    u8 pad18[0x14];
} Unk80014790;
typedef struct Unk80014790Table {
    s32 count;
    Unk80014790 e[1];
} Unk80014790Table;
typedef struct { u8 pad0[6]; s16 f6; u8 pad8[6]; s8 fE; s8 fF; } Unk80013EB0;
extern Unk80014790Table D_80067950;
extern Unk80014790Table D_800675B0;

void func_80014790(u32 *arg0) {
    u32 word0;
    u32 word;
    s32 i;
    s32 bit;
    Unk80014790 *p;
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->fF != 0) {
            bit = p->f14 >> 1;
            word0 = arg0[bit / 32];
            if (!(word0 & (1 << (bit % 32)))) {
                func_80013EB0((Unk80013EB0 *)p);
            }
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->fF != 0) {
            bit = p->f14 >> 1;
            word = arg0[bit / 32];
            if (!(word & (1 << (bit % 32)))) {
                func_80013EB0((Unk80013EB0 *)p);
            }
        }
    }
}
