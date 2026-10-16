#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 pad[10]; s32 f28; s32 pad2[8]; } Unk80034A2C;

s32 *D_8005F264 = 0;

void func_80034A2C(s32 *arg0) {
    Unk80034A2C *p;

    if (func_80035A94() != 0) {
        D_8005F264[2] = 0;
        func_80036084(2);
        func_800360CC(0);
        p = (Unk80034A2C *)D_8005F264;
        p += arg0[1];
        if (p->f28 != 0) {
            func_80036084(0x80);
            func_800368C0();
            func_80036850();
        }
    }
    func_80035F2C((s32 *)arg0[0]);
}
