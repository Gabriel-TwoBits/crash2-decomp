#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
s32 D_8005F268 = 1;
extern s32 _card_info(s32);
extern s32 func_80035B3C(void);

s32 func_8003653C(void) {
    s32 v;
    if (D_8005F264 != 0) {
        v = func_80036150() & 0x10;
    } else {
        v = D_8005F268;
    }
    if (v == 0) {
        func_80035B70();
        _card_info(0);
        return func_80035B3C() != 0;
    }
    return 1;
}
