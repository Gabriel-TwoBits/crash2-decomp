#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005B198[];

s32 func_80011F78(u8 *arg0) {
    s32 n;
    s32 acc;
    s32 c;
    s32 i;
    u8 *p;

    acc = 0;
    n = 5;
    do {
        c = *arg0++ & 0xFF;
        if (c == 0) {
            if (n-- != 0) {
                do {
                    acc = (acc << 6) | 0x3E;
                } while (n-- != 0);
            }
            break;
        }
        i = 0;
        p = D_8005B198;
        while (1) {
            if (i == 0x40) {
                i = 0x3E;
                break;
            }
            if (*p++ == c) break;
            i++;
        }
        n--;
        acc = (acc << 6) | i;
    } while (n != 0);
    return (acc << 1) | 1;
}
