#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8002719C(s32 *arg0);
s32 func_800271D8(s32 *arg0, s32 arg1);

s32 func_800270C4(s32 *arg0, s32 arg1) {
    s32 pos;
    s32 end;
    s32 limit;
    s32 delta;
    s32 ret;

    ret = 0;
    pos = arg0[0] >> 8;
    if (pos < 0) {
        ret = func_8002719C(arg0);
        pos = arg0[0] >> 8;
    }
    end = arg1 >> 8;
    if (end < 0) {
        end = 0;
    }
    limit = arg0[1] >> 8;
    delta = end - pos;
    if (end >= limit) {
        end = limit - 1;
        arg1 = (limit << 8) - 1;
    }
    delta = end - pos;
    if (delta < 0) {
        ret |= func_8002719C(arg0);
        delta = end - (arg0[0] >> 8);
    }
    if (delta != 0) {
        ret |= func_800271D8(arg0, delta);
    }
    arg0[0] = arg1;
    return ret;
}
