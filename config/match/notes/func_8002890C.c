#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005BB68[];

void func_8002890C(u8 *arg0, s32 arg1, s32 arg2) {
    s32 off;

    if (*(s32 *)(arg0 + 0xAC) & 0x04000000) {
        off = -((*(s32 *)(arg0 + 0x100) & 0x10) == 0) & 0x18;
        if (arg1 >= 0) {
            *(s32 *)(arg0 + 0x118) = off + D_8005BB68[arg1];
        } else {
            *(s32 *)(arg0 + 0x118) = off + arg2;
        }
    }
}
