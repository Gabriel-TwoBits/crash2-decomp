#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800365E4(void) {
    func_80035B70();
    _card_info(0);
    if (func_80035B3C() == 2) {
        return 1;
    }
    return 0;
}
