#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F218[] = "pb??B";

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_8002FE58);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_8002FF54);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_8002FFFC);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030498);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030680);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_800309A8);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030CD8);

void func_80030CD8(s32 arg0);

void func_80030D74(s32 *arg0) {
    func_80030CD8(arg0[5]);
}

typedef struct { s32 x, y, z; } Vec80030D98;
typedef struct { Vec80030D98 min; Vec80030D98 max; } Box80030D98;

s32 func_80030D98(Vec80030D98 *p, Box80030D98 *b) {
    if (p->x >= b->min.x && p->y >= b->min.y && p->z >= b->min.z &&
        p->x < b->max.x && p->y < b->max.y && p->z < b->max.z) {
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030E1C);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030EC0);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030F58);

s32 func_80030FEC(s32 *arg0, s32 *arg1) {
    s32 d[3];
    s32 x, y, z;

    x = arg0[0] - arg1[0];
    if (x < 0) x = arg1[0] - arg0[0];
    d[0] = x;
    y = arg0[1] - arg1[1];
    if (y < 0) y = arg1[1] - arg0[1];
    d[1] = y;
    z = arg0[2] - arg1[2];
    if (z < 0) z = arg1[2] - arg0[2];
    d[2] = z;
    if (x >= y && x >= z) return x + (y + z) / 4;
    if (d[1] >= d[0] && d[1] >= d[2]) return d[1] + (d[0] + d[2]) / 4;
    return d[2] + (d[1] + d[0]) / 4;
}

s32 func_800310EC(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 d;
    s32 nx = -arg3;
    s32 ny = -arg4;
    s32 d0;
    s32 nz = -arg5;

    d = arg1[0] - arg2[0];
    d0 = d;
    if (arg3 < d0 || d0 < nx ||
        (d = arg1[1] - arg2[1], arg4 < d) || d < ny ||
        (d = arg1[2] - arg2[2], arg5 < d) || nz > d) {
        if (arg0) {
            if (nz) {
                arg0[42] |= 0x8000;
            } else {
                arg0[42] |= 0x8000;
            }
        }
        return 1;
    }
    return 0;
}

typedef struct { s32 x, y, z; } V;

s32 func_8003118C(V *a, V *b) {
    V v;
    v.x = (a->x - b->x) >> 8;
    v.y = (a->y - b->y) >> 8;
    v.z = (a->z - b->z) >> 8;
    return func_80041A78(v.x * v.x + v.y * v.y + v.z * v.z) << 8;
}


s32 func_80031208(s32 *arg0, s32 *arg1) {
    V v;

    v.x = (arg0[0] - arg1[0]) >> 8;
    v.z = (arg0[2] - arg1[2]) >> 8;
    return func_80041A78(v.x * v.x + v.z * v.z) << 8;
}

/* Angle (12-bit, 0x1000 per turn) from arg0 to arg1 on the plane of words 0 and 2 */
s32 func_80031264(s32 *arg0, s32 *arg1) {
    return ratan2(arg1[0] - arg0[0], arg1[2] - arg0[2]) & 0xFFF;
}

s32 func_80031298(s32 *arg0, s32 *arg1) {
    return ratan2(arg1[0] - arg0[0], arg1[1] - arg0[1]) & 0xFFF;
}

s32 func_800312CC(s32 arg0, s32 arg1, s32 arg2) {
    return (arg0 + ((s32) (arg2 * ((s32) ((arg1 - arg0) << 0x14) >> 0x14)) >> 8)) & 0xFFF;
}

s32 func_800312F0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 d;

    d = (s32)((arg1 - arg0) << 20) >> 20;
    if (d > 0 && arg3 < d) {
        goto clamp;
    }
    if (d < 0) {
        arg3 = -arg3;
        if (d < arg3) {
clamp:
            d = arg3;
        }
    }
    return (arg0 + ((arg2 * d) >> 8)) & 0xFFF;
}

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80031340);

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_8003145C);

void func_80031474(void) {
}
