#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8006CB18[];

void func_80018DD0(void *arg0, void *arg1, void *arg2) {
    u8 *var_s0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = D_8006CB18;
    do {
        ((void (*)(void *, void *, void *))arg0)(var_s0, arg1, arg2);
        var_s1 += 1;
        var_s0 += 8;
    } while (var_s1 < 8);
}
