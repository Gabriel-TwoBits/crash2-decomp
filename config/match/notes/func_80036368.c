#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *func_80011764(s32, s32);
extern s32 *D_8006CCE8;
s32 *D_8005F264 = 0;
s32 D_8005F268 = 1;

s32 func_80036368(void) {
    s32 *p;

    p = func_80011764(1, 0x11F4);
    D_8005F264 = p;
    D_8006CCE8 = p + 0x1BD;
    if (D_8005F268 != 0) {
        func_80036084(0x10);
    } else {
        func_800360AC(0x10);
    }
    func_80036324(4);
    func_800368C0();
    return 0;
}
