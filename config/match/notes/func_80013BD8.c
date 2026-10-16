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

extern s32 D_8003BEA4;
extern Unk80013BD8Table D_80067950;
extern s32 *D_80067FC4;

void *func_80013BD8(s32 arg0, s32 arg1, s32 arg2, s8 arg3) {
    s32 i;
    Unk80013BD8 *p;

    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state == 1) {
            p->state = 2;
            p->f_14 = arg0;
            p->f_f = arg3;
            if (arg2 & 4) {
                p->f_e = arg3;
            } else {
                p->f_e = 0;
            }
            do {
                p->f_18 = arg1;
                p->f_28 = D_8003BEA4;
            } while (0);
            D_80067FC4[arg0 >> 1] = (s32)p;
            *(s32 *)((u8 *)&D_80067950 + 0x660) += 1;
            return p;
        }
    }
    return 0;
}
