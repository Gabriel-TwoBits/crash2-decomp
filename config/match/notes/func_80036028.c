#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F264 = 0;

void func_80036084(void);

s32 func_80036028(void) {
    s32 *p = (s32 *)D_8005F264;

    p[2] = 0;
    p[5] = 0;
    func_80036084();
    return 0;
}
