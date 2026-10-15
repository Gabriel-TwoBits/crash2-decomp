#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8003118C(V *a, V *b) {
    V v;
    v.x = (a->x - b->x) >> 8;
    v.y = (a->y - b->y) >> 8;
    v.z = (a->z - b->z) >> 8;
    return func_80041A78(v.x * v.x + v.y * v.y + v.z * v.z) << 8;
}
