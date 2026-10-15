#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80035A7C(s32);
s32 func_8003613C();

s32 func_80036628(void) {
    if (func_8003613C() != 0) {
        func_80036084(2);
        return 1;
    }
    func_800360CC(3);
    func_80035A7C(3);
    return 0;
}
