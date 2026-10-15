#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

s32 func_800360CC(s32 arg0) {
    s32 *p = D_8005F264;
    *p = arg0;
    if (arg0 >= 0) {
        if (arg0 < 2 || arg0 == 4) {
            p[6] = 1;
        }
        if (*D_8005F264 == 0) {
            func_800360AC(1);
        } else {
            func_80036084(1);
        }
    } else {
        func_80036084(1);
    }
    return 0;
}
