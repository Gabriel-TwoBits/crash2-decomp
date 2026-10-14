#include "common.h"

extern s16 D_8005BCB0[];

s32 func_8003D2EC(s32 arg0) {
    s16 *table = D_8005BCB0;
    s32 ret;

    arg0 &= 0xFFF;
    if (arg0 < 0x800) {
        if (arg0 < 0x400) {
            ret = table[arg0];
        } else {
            ret = *(s16 *)((u8 *)table - arg0 * 2 + 0x1000);
        }
    } else if (arg0 < 0xC00) {
        ret = -*(s16 *)((u8 *)table + arg0 * 2 - 0x1000);
    } else {
        ret = -*(s16 *)((u8 *)table - arg0 * 2 + 0x2000);
    }
    return ret;
}
