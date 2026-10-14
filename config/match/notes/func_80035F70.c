#include "common.h"
#include "sdk.h"
#include "game.h"

extern volatile u32 D_8003BEA4;

void func_80035F70(u32 arg0) {
    u32 start;

    start = D_8003BEA4;
    while (start - D_8003BEA4 < arg0) {
    }
}
