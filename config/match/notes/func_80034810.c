#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
s32 func_80036414(s32);

s32 func_80034810(void) {
    D_8005F264[6] = 0;
    if (func_80036414(0) == 1) {
        func_800360AC(0xA);
        func_800360CC(0);
        func_80035FD8();
        return 1;
    }
    if (func_80035A94() != 0) {
        func_80036028(0);
        func_800360AC(6);
        func_80036084(8);
        func_800360CC(0);
        return 1;
    }
    return 0;
}
