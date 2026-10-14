#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

void func_80036050(void) {
    s8 *p;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        if (D_8005F264 != 0) {
            p = ((i * 0x4C) + ((s8 *)D_8005F264)) + 0x34;
            *p = 0;
        }
    }
}
