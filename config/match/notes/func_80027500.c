#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80027500(u8 *src, u8 *mask, u8 *dst, s32 n, s32 mode, s32 w, s32 stride) {
    u8 *row;
    s32 x;
    s32 y;
    u32 bit;
    u32 m;

    if (n > 0) {
        if (mode != 0) {
            w >>= 3;
            y = stride - 1;
            w--;
            if (y >= 0) {
                row = mask;
                do {
                    x = w;
                    mask++;
                    if (x >= 0) {
                        do {
                            m = *row;
                            if (m != 0) {
                                bit = 0x80;
                                do {
                                    if (bit & m) {
                                        *dst = *src++;
                                        if (--n == 0) {
                                            return;
                                        }
                                    }
                                    bit >>= 1;
                                    dst++;
                                } while (bit & 0xFF);
                            } else {
                                dst += 8;
                            }
                            x--;
                            row += stride;
                        } while (x >= 0);
                    }
                    y--;
                    row = mask;
                } while (y >= 0);
            }
        } else {
            for (;;) {
                m = *mask++;
                if (m == 0) {
                    dst += 8;
                    continue;
                }
                bit = 0x80;
                do {
                    if (m & bit) {
                        *dst = *src++;
                        if (--n == 0) {
                            return;
                        }
                    }
                    bit >>= 1;
                    dst++;
                } while (bit & 0xFF);
            }
        }
    }
}
