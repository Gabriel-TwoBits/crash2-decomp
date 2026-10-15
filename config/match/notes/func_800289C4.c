#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800289C4(s16 *arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    func_80028968(arg1, &sp10);
    if (arg2 == 0) {
        arg0[0x11] = sp10;
    } else {
        arg0[0x11] = func_8001E5FC(arg0[0x11], sp10, 0x100);
    }
}
