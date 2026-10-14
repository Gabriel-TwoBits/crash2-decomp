#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80013E7C(s8 *arg0) {
    s8 t = arg0[0xE];
    if (t != 0) {
        arg0[0xE] = t - 1;
        func_80013E10(arg0);
    }
}
