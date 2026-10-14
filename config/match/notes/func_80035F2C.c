#include "common.h"
#include "sdk.h"
#include "game.h"

s32 close(s32);

void func_80035F2C(s32 *arg0) {
    s32 temp_a0;

    temp_a0 = *arg0;
    if (temp_a0 >= 0) {
        close(temp_a0);
    }
    *arg0 = -1;
}
