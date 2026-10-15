#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80041A78(s32);

typedef struct { s32 x, y, z; } V;

s32 func_80031208(s32 *arg0, s32 *arg1) {
    V v;

    v.x = (arg0[0] - arg1[0]) >> 8;
    v.z = (arg0[2] - arg1[2]) >> 8;
    return func_80041A78(v.x * v.x + v.z * v.z) << 8;
}
