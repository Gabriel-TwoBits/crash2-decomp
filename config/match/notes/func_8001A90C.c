#include "common.h"
#include "sdk.h"
#include "game.h"

u32 func_8001A90C(void *arg0, s32 *arg1, s32 *arg2) {
    char *s2;
    char *s3;
    s32 odd;
    u32 idx;
    s32 *p;

    s2 = *(char **)((char *)arg0 + 0xE8);
    s3 = s2 + 4;
    func_80014260(s3, 1, 0);
    odd = *(s32 *)(s2 + 8) & 1;
    idx = *(u32 *)((char *)arg0 + 0xEC) >> 8;
    if (arg2 == 0) {
        p = (s32 *)((char *)func_80014260(s3, 1, 0) + 0x10);
        if (odd) {
            *arg1 = p[idx >> 1];
        } else {
            *arg1 = p[idx];
        }
        return idx;
    }
    if (odd == 0) {
        p = (s32 *)((char *)func_80014260(s3, 1, 0) + 0x10);
        *arg1 = *arg2 = p[idx];
    } else if ((idx & 1) && (s32)(idx + 1) < *(u16 *)(s2 + 2)) {
        u32 h = idx >> 1;
        *arg1 = ((s32 *)((char *)func_80014260(s3, 1, 0) + 0x10))[h];
        *arg2 = ((s32 *)((char *)func_80014260(s3, 1, 0) + 0x10))[h + 1];
    } else {
        s32 h = (s32)(idx + 1) >> 1;
        p = (s32 *)((char *)func_80014260(s2 + 4, 1, 0) + 0x10);
        *arg1 = *arg2 = p[h];
    }
    return idx;
}
