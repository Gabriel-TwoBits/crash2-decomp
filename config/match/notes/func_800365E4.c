#include "common.h"
#include "sdk.h"
#include "game.h"

s32 _card_info(s32 chan);
void func_80035B70(void);
s32 func_80035B3C(void);

s32 func_800365E4(void) {
    func_80035B70();
    _card_info(0);
    return func_80035B3C() == 2;
}
