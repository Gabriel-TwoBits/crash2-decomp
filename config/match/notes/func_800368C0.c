#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

void func_8003690C(void *);

void func_800368C0(void) {
    s32 i;
    s32 offset;

    i = 0;
    offset = 0x6F4;
    do {
        i++;
        func_8003690C((s8 *)D_8005F264 + offset);
        offset += 0x2A4;
    } while (i < 4);
}
