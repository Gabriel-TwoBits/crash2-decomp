#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001148C(s32);

s32 func_80014C08(void) {
    s32 temp_v0;

    temp_v0 = func_8001148C(4);
    if (temp_v0 != 0) {
        func_800116C0(temp_v0);
    }
    return temp_v0;
}
