#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800312CC(s32 arg0, s32 arg1, s32 arg2) {
    return (arg0 + ((s32) (arg2 * ((s32) ((arg1 - arg0) << 0x14) >> 0x14)) >> 8)) & 0xFFF;
}
