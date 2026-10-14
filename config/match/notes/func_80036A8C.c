#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80036A8C(s32 *arg0, s32 arg1) {
    s32 sum;
    s32 i;
    s32 pad[2];

    sum = 0x12345678;
    i = arg1 - 1;
    if (arg1 != 0) {
        do {
            sum += *arg0++;
            i--;
        } while (i != -1);
    }
    return sum;
}
