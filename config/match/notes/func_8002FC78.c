#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8002FC78(void *arg0, s32 arg1, s32 arg2) {
    s32 *p = (s32 *)((char *)func_80014260(arg0, 1, 0) + 0x14);
    return p[arg1 * 2 + arg2] + 3;
}
