#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80027C00(u8 *src, u8 *dst) {
    u8 *start;
    s8 c;
    s32 n;
    u8 b;
    s32 hi;

    start = dst;
    while (1) {
        c = *src++;
        if (c >= 0) {
            n = c + 1;
            if (c <= 0) {
                hi = *src++;
                n = (hi << 8) | *src++;
                if (n == 0) {
                    return dst - start;
                }
            }
            b = *src++;
            n--;
            while (n != -1) {
                *dst++ = b;
                n--;
            }
        } else {
            n = -c;
            if (c == -0x80) {
                hi = *src++;
                n = (hi << 8) | *src++;
            }
            n--;
            while (n != -1) {
                *dst++ = *src++;
                n--;
            }
        }
    }
}
