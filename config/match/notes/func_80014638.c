#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 f_e;
    s8 f_f;
    u8 pad10[4];
    s32 f_14;
    s32 f_18;
    u8 pad1c[0xC];
    s32 f_28;
} Unk80013BD8;

typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;

extern Unk80013BD8Table D_80067950;
extern Unk80013BD8Table D_800675B0;

void func_80014638(u32 *arg0) {
    s32 i;
    s32 bit;
    Unk80013BD8 *p;

    for (i = 7; i >= 0; i--) {
        arg0[i] = 0;
    }
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
            /* The redundant self-assignment orders the load before the mask, as in the retail code. */
            arg0[bit / 32] = arg0[bit / 32];
            arg0[bit / 32] |= 1 << (bit % 32);
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
            arg0[bit / 32] = arg0[bit / 32];
            arg0[bit / 32] |= 1 << (bit % 32);
        }
    }
}
