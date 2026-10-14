#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80035F9C(u8 *arg0) {
    while (*arg0 != 0) {
        if ((u8)(*arg0 - 0x61) < 0x1A) {
            *arg0 = *arg0 - 0x20;
        }
        arg0++;
    }
}
