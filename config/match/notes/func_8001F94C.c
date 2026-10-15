#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_8006759C;
extern u16 D_8006935C[];

void func_8001F94C(s32 arg0) {
    u16 *p;
    u16 key;
    s32 want;

    want = (arg0 >> 8) | (D_8006759C[2] << 9);
    p = D_8006935C;
    key = *p;
    if (key != 0) {
        want &= 0xFFFF;
        do {
            if (key == want) {
                if (p[1] != 0) {
                    p[0] = 1;
                } else {
                    p[0] = 0;
                }
                return;
            }
            p += 1;
            key = *p;
        } while (key != 0);
    }
}
