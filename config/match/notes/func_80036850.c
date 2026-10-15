#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8006CCB0;
s32 *D_8005F264 = 0;

void func_80036850(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        func_800175D8(D_8006CCB0, i << 8, (char *)D_8005F264 + (i * 0x2A4 + 0x6F4) + 0xC, 0x28, 1);
    }
}
