#include "common.h"
#include "sdk.h"
#include "game.h"

#define KEY_SHIFT_LOW 8
#define KEY_SHIFT_HIGH 9
#define KEY_MASK 0xFFFF

extern void *D_8006759C;
extern u16 D_8006935C[];

void func_8001F94C(s32 arg0) {
    u16 *p;
    s32 key;

    key = (arg0 >> KEY_SHIFT_LOW) | (((s32 *)D_8006759C)[2] << KEY_SHIFT_HIGH);
    key &= KEY_MASK;
    for (p = D_8006935C; *p != 0; p++) {
        if (*p == key) {
            if (p[1] != 0) {
                *p = 1;
            } else {
                *p = 0;
            }
            return;
        }
    }
}
