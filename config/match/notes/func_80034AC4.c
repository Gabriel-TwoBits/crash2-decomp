#include "common.h"
#include "sdk.h"
#include "game.h"

s32 VSync(s32);
s32 erase(s32);
void func_800369F4(void);

typedef struct {
    s32 *f0;
    s32 f4;
    s32 *f8;
    s32 *fC;
    s32 f10;
} Unk80034AC4;

void func_80034AC4(Unk80034AC4 *arg0) {
    s32 i;
    s32 r;

    if (*arg0->fC != 0) {
        *arg0->fC = 0;
        func_800369F4();
    }
    if (func_80035A94() != 0) {
        func_80036084(2);
        func_800360CC(0);
    }
    func_80035F2C(arg0->f0);
    if (*arg0->f8 != 0) {
        VSync(0x3C);
        for (i = 0; i < 9; i++) {
            r = erase(arg0->f10);
            VSync(0x14);
            if (r == 1) {
                break;
            }
        }
        if (i < 3) {
            *arg0->f8 = 0;
        }
    }
}
