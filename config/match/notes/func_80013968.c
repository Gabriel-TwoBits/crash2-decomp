#include "common.h"
#include "sdk.h"

typedef struct Unk80013968 {
    s16 state;
    u8 pad02[0x2A];
} Unk80013968;

typedef struct Unk80013968B {
    s32 v;
    u8 pad04[0x28];
} Unk80013968B;

typedef struct Unk80013968C {
    s32 *v;
    u8 pad04[0x28];
} Unk80013968C;

extern Unk80013968 D_80062D70[];
extern Unk80013968B D_80062D80[];
extern Unk80013968B D_80062D84[];
extern Unk80013968C D_80062D8C[];
extern s32 *D_80067FC4;
extern void func_800170B4(s32, s32);

s32 func_80013968(s32 arg0) {
    s16 st = D_80062D70[arg0].state;

    if (st != 0x14) {
        if (st != 0x15) {
            return -0xE;
        }
        goto tail;
    }
    func_800170B4(D_80062D84[arg0].v, 0);
    *D_80062D8C[arg0].v = D_80062D80[arg0].v;
    D_80067FC4[D_80062D80[arg0].v >> 1] = -0x12;
tail:
    D_80062D70[arg0].state = 1;
    return -0xE;
}
