#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

void func_80035E7C(void) {
    s32 i;
    s32 n;

    if (D_8005F264[2] >= 15) {
        n = D_8005F264[8];
        for (i = 0; i < n; i++) {
            if (func_80035F00((char *)D_8005F264 + 0x49C + i * 0x28) != 0) {
                return;
            }
        }
        func_80036084(0x40);
    }
}
