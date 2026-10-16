#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { u8 pad0[2]; u16 count; u8 pad4[4]; u32 flags; } Unk8001A90C;

u32 func_8001A90C(void *arg0, s32 *arg1, s32 *arg2) {
    Unk8001A90C *s2;
    char *s3;
    char *p;
    s32 s1;
    u32 s0;

    s2 = *(Unk8001A90C **)((char *)arg0 + 0xE8);
    s3 = (char *)s2 + 4;
    func_80014260(s3, 1, 0);
    s1 = s2->flags & 1;
    s0 = *(u32 *)((char *)arg0 + 0xEC) >> 8;
    if (arg2 == 0) {
        p = func_80014260(s3, 1, 0) + 0x10;
        if (s1 != 0) {
            p += (s0 >> 1) * 4;
        } else {
            p += s0 * 4;
        }
        *arg1 = *(s32 *)p;
        return s0;
    }
    if (s1 == 0) {
        p = func_80014260(s3, 1, 0);
        p += s0 * 4;
        goto done;
    }
    if ((s0 & 1) && (s32)(s0 + 1) < (s32)s2->count) {
        s1 = s0 >> 1;
        p = func_80014260(s3, 1, 0);
        *arg1 = *(s32 *)(p + 0x10 + s1 * 4);
        s1++;
        p = func_80014260(s3, 1, 0);
        *arg2 = *(s32 *)(p + 0x10 + s1 * 4);
        return s0;
    }
    s1 = (s32)(s0 + 1) >> 1;
    p = func_80014260((char *)s2 + 4, 1, 0);
    p += s1 * 4;
done:
    *arg1 = *arg2 = *(s32 *)(p + 0x10);
    return s0;
}
