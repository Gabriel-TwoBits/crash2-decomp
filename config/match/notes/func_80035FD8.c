#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264;

s32 func_80035FD8(void) {
    if (!(func_80036150() & 0xD)) {
        func_800368C0();
        func_800360CC(1);
        func_80035A7C(0x40);
        D_8005F264[7] = 0;
        return 0;
    }
    return 1;
}
