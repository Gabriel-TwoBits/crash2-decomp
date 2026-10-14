#include "common.h"
#include "sdk.h"
#include "game.h"

extern void *D_8006759C;
extern s32 D_8006CB70;

s32 func_8001F018(void) {
    D_8006CB70 = *(s32 *)((s8 *)D_8006759C + 8) << 8;
    if (*(s32 *)((s8 *)D_8006759C + 4) == 0) {
        return -0xE;
    }
    return -0xFF;
}
