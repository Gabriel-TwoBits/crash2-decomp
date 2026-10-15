#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80014078(s32, s32);

s32 func_8001434C(u32 arg0, s32 arg1) {
    Unk80014148 *t;
    s32 a;
    s32 r;

    a = *(s32 *)arg0;
    if (!(a & 1)) {
        r = (s32)arg0;
        if (arg1 != 0) {
            t = func_80014148(a);
            if (t != 0) {
                a = *(s32 *)((u8 *)t + 0x14);
                goto call;
            }
            return 1;
        }
        return r;
    }
    r = 0;
    if (arg1 != 0) {
call:
        r = func_80014078(a, arg1);
    }
    return r;
}
