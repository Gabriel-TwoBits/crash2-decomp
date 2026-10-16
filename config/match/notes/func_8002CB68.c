#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F3C4;

void func_8002CB68(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 *p;
    s32 x;
    s32 y;
    s32 end;
    s32 mask;

    arg2 += 0x10;
    mask = -1;
    if (arg2 < 0x20) {
        if (arg2 <= 0) {
            return;
        }
        mask <<= 0x20 - arg2;
    }
    arg0 += 0xF;
    if (arg0 >= 0x20) {
        return;
    }
    if (arg0 >= 0) {
        mask &= ~((s32)0x80000000 >> arg0);
    }
    if (mask == 0) {
        return;
    }
    x = arg1 + 0x10;
    if (x < 0) {
        x = 0;
    }
    end = arg3 + 0x10;
    if (end >= 0x21) {
        end = 0x20;
    }
    if (x >= 0x20) {
        return;
    }
    p = D_8005F3C4 + x;
    if (arg4 != 0) {
        if (x < end) {
            do {
                x++;
                *p |= mask;
                p++;
            } while (x < end);
        }
    } else {
        mask = ~mask;
        while (x < end) {
            x++;
            *p &= mask;
            p++;
        }
    }
}
