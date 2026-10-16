#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80018194(s32 *v, s16 *m, s32 *out) {
    out[0] = (m[0] * v[0] + m[1] * v[1] + m[2] * v[2]) >> 12;
    out[1] = (m[3] * v[0] + m[4] * v[1] + m[5] * v[2]) >> 12;
    out[2] = (m[6] * v[0] + m[7] * v[1] + m[8] * v[2]) >> 12;
}
