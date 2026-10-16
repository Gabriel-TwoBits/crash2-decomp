#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800175D8(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3) {
    s32 i;
    s32 n;
    s32 off;
    u8 c;

    off = 0;
    n = arg1 >> 8;
    for (i = 0; i < n; i++) {
        while (arg0[off++ + 0xC] != 0) {
        }
    }
    i = 0;
    if (arg3 != 0) {
        do {
            c = *arg2;
            if (c == 0) break;
            if (c < 0x20) c = 0x20;
            if (c == 0x7B) c = 0x28;
            if (c == 0x7D) c = 0x29;
            if (c == 0x7E) c = 0x7B;
            if (c == 0x25) c = 0x7D;
            if (c == 0x26) c = 0x5F;
            arg0[off + i + 0xC] = c;
            arg2++;
            arg3--;
            i++;
        } while (arg3 != 0);
        while (arg3 != 0) {
            arg0[off + i + 0xC] = 0x20;
            arg3--;
            i++;
        }
    }
}
