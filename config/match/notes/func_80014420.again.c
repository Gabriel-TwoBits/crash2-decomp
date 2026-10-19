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
typedef struct {
    u8 pad0[6];
    s16 f6;
    u8 pad8[6];
    s8 fE;
    s8 fF;
} Unk80013EB0;
extern Unk80013BD8Table D_80067950;
void func_80013EB0(Unk80013EB0 *arg0);
extern Unk80013BD8Table D_800675B0;
extern Unk80013BD8 D_80062D40;
extern Unk80013BD8 D_80063000;


void func_80014420(void) {
    Unk80013BD8 *e;
    Unk80013BD8 *p;
    Unk80013BD8 *q;
    s32 i;
    s32 pad[4];

    i = 0;
    if (D_80067950.count > 0) {
        e = D_80067950.e;
        q = e;
        do {
            p = q;
            if (p->state != 1) {
                func_80013EB0((Unk80013EB0 *)p);
            }
            i++;
            q = p + 1;
        } while (i < ((s32 *)e)[-1]);
    }
    i = 0;
    if (D_800675B0.count > 0) {
        p = e = D_800675B0.e;
        do {
            if (p->state != 1) {
                func_80013EB0((Unk80013EB0 *)p);
            }
            i++;
            p++;
        } while (i < ((s32 *)e)[-1]);
    }
    for (i = 15, p = &D_80063000; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0((Unk80013EB0 *)p);
        }
    }
    for (i = 9, p = &D_80062D40; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0((Unk80013EB0 *)p);
        }
    }
}
