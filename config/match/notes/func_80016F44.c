#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80016F44(s32 arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    idx = -1;
    do { s3 = *(u8 **)((u8 *)arg0 + 0x10); } while (0);
    s2 = *(u8 **)((u8 *)arg0 + 0x20);
    n = *(s32 *)(s3 + 0x1C);
    if (arg2 != 0x6396347F) {
        for (i = 0; i < *(u32 *)(s3 + 0x28); i++) {
            e = ((s32 *)(s3 + 0x2C))[i];
            if (e & 1) {
                idx = i;
                if (arg2 == e) {
                    goto found;
                }
            } else {
                idx = i;
                if (arg2 == *(s32 *)(e + 4)) {
                    goto found;
                }
            }
        }
        return -0xFF;
    }
found:
    if (arg1 != 0) {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                func_80018474(s2, s3 + 0x2C);
            }
            s2 += 0xC;
        }
    } else {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                *(u16 *)(s2 + 6) &= 0xFDFF;
            }
            s2 += 0xC;
        }
    }
    return -0xFF;
}
