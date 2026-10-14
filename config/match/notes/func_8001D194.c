#include "common.h"
#include "sdk.h"
#include "game.h"

extern char func_80018D28;
extern char func_8001CEDC;

s32 func_8001D194(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 args[6];

    args[0] = arg0;
    args[1] = arg1;
    args[3] = arg2;
    args[2] = arg3;
    args[4] = 0;
    args[5] = arg4;
    func_80018DD0(&func_80018D28, &func_8001CEDC, args);
    return -0xFF;
}
