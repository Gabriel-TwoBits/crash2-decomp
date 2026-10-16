#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *func_80011764(s32 count, s32 size) {
    s32 total;
    s32 p;

    total = size * count;
    p = func_8001148C(total);
    if (p == 0) {
        return 0;
    }
    bzero((u8 *)p, total);
    return (s32 *)p;
}
