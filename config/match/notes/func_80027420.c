#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80027CD0(u8 *, u8 *, s32);
void *memcpy(void *, const void *, u32);

u8 *func_80027420(u8 *src, u8 *dst, s32 mode, s32 *outLen, s32 noCopy, s32 arg5) {
    s32 len;

    switch (mode) {
    case 1:
        len = (src[0] << 8) | src[1];
        src += 2;
        if (noCopy != 0) {
            dst = src;
        } else {
            memcpy(dst, src, len);
        }
        break;
    case 2:
        len = func_80027C00(src, dst);
        break;
    case 3:
        len = func_80027CD0(src, dst, arg5);
        break;
    default:
        len = 0;
        break;
    }
    if (outLen != 0) {
        *outLen = len;
    }
    return dst;
}
