#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005BB78[];

void func_80028968(s32 arg0, s32 *arg1) {
    if (((u32) (arg0 - 0x30) < 0x10U) && (*arg1 = 0, ((arg0 < 0x40) != 0))) {
        if (arg0 >= 0x30) {
            *arg1 = 0x1000 - (D_8005BB78[arg0] * 0x14);
        }
    } else {
        *arg1 = 0;
    }
}
