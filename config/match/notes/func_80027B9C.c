#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80069334;
extern s32 D_80069338;
extern s32 D_8006933C;

s32 func_80027B9C(void) {
    D_80069334 = 0;
    if (D_8006933C != 0) {
        func_800116C0(D_8006933C);
        D_8006933C = 0;
    }
    if (D_80069338 != 0) {
        func_800116C0(D_80069338);
        D_80069338 = 0;
    }
    return -0xFF;
}
