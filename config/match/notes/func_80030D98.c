#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 x, y, z; } Vec80030D98;
typedef struct { Vec80030D98 min; Vec80030D98 max; } Box80030D98;

s32 func_80030D98(Vec80030D98 *p, Box80030D98 *b) {
    if (p->x >= b->min.x && p->y >= b->min.y && p->z >= b->min.z &&
        p->x < b->max.x && p->y < b->max.y && p->z < b->max.z) {
        return 1;
    }
    return 0;
}
