#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;
extern s32 D_8005F2BC;

void func_8001144C(s32 *arg0, u32 arg1) {
    D_8005F2A4 = arg0;
    do { *arg0 = 0; D_8005F2B4 = &arg0[arg1 >> 2] + 1; } while (0);
    D_8005F2AC = arg1 - 4;
    D_8005F2BC = 0;
}
