#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800155E0(void *arg0, s32 arg1) {
    char *p;

    p = (char *)arg0 + arg1 * 0x2C;
    if (*(s16 *)(p + 0x30) != 0) {
        *(s16 *)(p + 0x30) = 0;
    }
}
