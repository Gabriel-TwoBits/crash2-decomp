#include "common.h"
#include "sdk.h"
#include "game.h"

u32 *D_8005F3C4;

s32 func_8002DF68(u32 arg0, u32 arg1) {
    s32 var_v0;

    var_v0 = 0;
    if ((arg0 < 0x20U) && (arg1 < 0x20U)) {
        var_v0 = (D_8005F3C4[arg1] & (0x80000000U >> arg0)) == 0;
    }
    return var_v0;
}
