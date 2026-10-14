#include "common.h"
#include "sdk.h"
#include "game.h"

u32 *D_8005F3C4;

s32 func_8002DF68(u32 arg0, u32 arg1) {
    return arg0 < 32 && arg1 < 32 && !(D_8005F3C4[arg1] & (0x80000000U >> arg0));
}
