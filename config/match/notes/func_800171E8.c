#include "common.h"
#include "sdk.h"

void func_80018474(void *, void *);

s32 func_800171E8(void *arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    s3 = *(u8 **)((u8 *)arg0 + 0x10);
    do {
        s2 = *(u8 **)((u8 *)arg0 + 0x1C);
        n = *(s32 *)(s3 + 0x34);
        idx = -1;
        if (arg2 != 0x6396347F) {
            for (i = 0; i < *(s32 *)(s3 + 0x40); i++) {
                e = ((s32 *)(s3 + 0xC))[i];
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
                    func_80018474(s2, s3 + 0xC);
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
    } while (0);
    return -0xFF;
}
