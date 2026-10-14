#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
s32 func_80036150();
void func_80036324(s32);

s32 func_80036674(void) {
    s32 ret;
    if (D_8005F264 != 0) {
        func_80036324((func_80036150() & 0x10) | 4);
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}
