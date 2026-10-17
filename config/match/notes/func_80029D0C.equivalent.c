#include "common.h"
#include "sdk.h"
#include "game.h"

#define FLAG_COLLIDE_ON 0x04000000
#define MODE_STEP 0x20000
#define MODE_SLIDE 0x40000
#define MODE_BOUNCE 0x80000
#define MODE_ALT 0x200
#define OBJ_FLAGS_OFFSET 0x100
#define OBJ_FLAG_SOLID 0x20000000
#define STEP_ALT 0x1C2
#define STEP_SLIDE 0x200
#define STEP_DEFAULT 0x40
#define OBJ_STATE_OFFSET 0x104C
#define FIXED_SHIFT 8
#define MAGIC_XZ 0x51EB851F
#define MAGIC_Y 0x1B4E81B5
#define SHIFT_XZ 45
#define SHIFT_Y 46
#define AXES 3

extern u8 *D_8005F38C;
extern s32 D_80062AD8;
extern s32 D_8006CB94;

void func_8002E884(s32 *, s32 *, s32, s32);

/* Division of |v| by a constant through its reciprocal multiplier, as the original does. */
static s32 absDiv(s32 v, s32 magic, s32 shift) {
    s64 prod;
    if (v < 0) {
        v = -v;
    }
    prod = (s64)v * (s64)magic;
    return (s32)(prod >> shift) - (v >> 31);
}

void func_80029D0C(void *arg0, s32 arg1, s32 *arg2, s32 *arg3) {
    s32 delta[AXES + 1];
    s32 out[AXES + 1];
    s32 prev[AXES];
    s32 p[AXES];
    s32 v[AXES];
    s32 step;
    s32 len;
    s32 nx;
    s32 nz;
    s32 dot;
    s32 n;
    s32 m;
    s32 i;
    s32 k;
    s32 changed;
    s32 t;
    s32 cur;

    len = 0;
    nx = 0;
    nz = 0;
    if ((D_8006CB94 & FLAG_COLLIDE_ON) && (D_80062AD8 & MODE_STEP) &&
        (!(D_80062AD8 & MODE_SLIDE) || (*(s32 *)((u8 *)arg0 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_SOLID))) {
        if ((D_80062AD8 & MODE_ALT) && (*(s32 *)(D_8005F38C +OBJ_FLAGS_OFFSET) & OBJ_FLAG_SOLID)) {
            step = STEP_ALT;
        } else if (D_80062AD8 & MODE_SLIDE) {
            step = STEP_SLIDE;
        } else {
            step = STEP_DEFAULT;
        }
        if (D_80062AD8 & MODE_BOUNCE) {
            s32 a = arg3[0] >> FIXED_SHIFT;
            s32 b = arg3[2] >> FIXED_SHIFT;
            len = func_80041A78(a * a + b * b);
            if (len == 0) {
                len = 1;
            }
            nx = arg3[0] / len;
            nz = arg3[2] / len;
        }
        func_8002E884(arg2, arg3, D_80062AD8 & MODE_SLIDE, step);
        if (D_80062AD8 & MODE_BOUNCE) {
            s32 x = arg3[0];
            dot = (((x >> FIXED_SHIFT) * nx) + ((arg3[2] >> FIXED_SHIFT) * nz)) >> FIXED_SHIFT;
            if (dot < 0) {
                dot = 0;
            }
            if (dot < len) {
                t = len - dot;
                p[0] = arg2[0] + x;
                v[0] = nx * t;
                p[2] = arg2[2] + arg3[2];
                v[2] = nz * t;
                func_8002E884(p, v, D_80062AD8 & MODE_SLIDE, step);
                arg3[0] = (p[0] + v[0]) - arg2[0];
                arg3[2] = (p[2] + v[2]) - arg2[2];
            }
        }
    }
    *(s32 *)((u8 *)(void *)arg1 + OBJ_STATE_OFFSET) = 0;
    n = absDiv(arg3[0], MAGIC_XZ, SHIFT_XZ);
    m = absDiv(arg3[1], MAGIC_Y, SHIFT_Y);
    k = absDiv(arg3[2], MAGIC_XZ, SHIFT_XZ);
    if (n < m) {
        n = m;
    }
    if (n < k) {
        n = k;
    }
    n = n + 1;
    for (i = 0; i < AXES; i++) {
        prev[i] = 0;
    }
    if (n > 0) {
        s32 s;
        for (s = 1; s <= n; s++) {
            changed = 0;
            t = (s << FIXED_SHIFT) / n;
            for (i = 0; i < AXES; i++) {
                cur = (arg3[i] * t) >> FIXED_SHIFT;
                delta[i] = cur - prev[i];
                changed |= delta[i];
                prev[i] = cur;
            }
            if (changed != 0) {
                func_8002C0F0(arg0, (void *)arg1, arg2, delta, out);
                for (i = 0; i < AXES; i++) {
                    arg2[i] = out[i];
                }
            }
        }
    }
}
