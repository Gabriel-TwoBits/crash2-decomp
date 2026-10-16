#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
extern void _card_clear(long);
s32 func_80036028(s32);

void func_800346E8(void) {
    if (D_8005F264[6] != 0) {
        D_8005F264[6] = 0;
        _card_info(0);
    }
    switch (func_80035AB8()) {
    case 1:
        func_800360CC(0);
        func_80036084(2);
        return;
    case 0:
        if (!(func_80036150() & 4)) {
            func_800360CC(0);
            return;
        }
    case 3:
        func_80035C74();
        _card_clear(0);
        if (func_80035C40() == 0) {
            func_800360AC(6);
            func_80036084(0x18);
            func_800360CC(4);
            func_80035A7C(0x96);
        } else {
    case 2:
            func_800360CC(0);
            func_80036084(4);
            func_800360AC(8);
        }
        func_80036028(0);
        return;
    }
}
