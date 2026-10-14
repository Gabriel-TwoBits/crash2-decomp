#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
s32 D_8005F268 = 1;

s32 func_800363D0(void) {
    u32 v = func_80036150();
    int has_ptr;
    do { } while (0); has_ptr = D_8005F264 != 0;
    D_8005F268 = (v >> 4) & 1;
    if (has_ptr) {
        func_800116C0((s32)D_8005F264);
        D_8005F264 = 0;
    }
    return 0;
}
