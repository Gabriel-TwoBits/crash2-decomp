#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80012294(void *arg0, s32 arg1) {
    s32 i;
    s32 n;
    s32 off;

    i = 0;
    n = 0;
    if (*(s32 *)(*(char **)((char *)arg0 + 0x18) + 0x404) > 0) {
        off = 0x520;
        do {
            if (func_80012250((s32 *)(*(char **)((char *)arg0 + 0x18) + off)) == arg1) {
                n++;
            }
            i++;
            off += 8;
        } while (i < *(s32 *)(*(char **)((char *)arg0 + 0x18) + 0x404));
    }
    return n;
}
