#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIXED_SHIFT 8
#define FRAC_MASK 0xFF
#define PLANE_FLAGS 5
#define SCALE_SHIFT 4
#define PATH_ITEM 0x4B
#define PATH_MODE 2
#define POINT_STRIDE 6
#define MAX_WORSE_STEPS 3
#define DIR_FORWARD 2
#define ROTATE_FLAG 1

/* Signed distance of an offset from the query along the projected axis. */
#define func_800254D4_proj(dx, dz, c, s, mode) \
    ((mode) != ROTATE_FLAG ? (((dx) * (c)) >> SCALE_SHIFT) - (((dz) * (s)) >> SCALE_SHIFT) \
                           : (((dz) * (c)) >> SCALE_SHIFT) + (((dx) * (s)) >> SCALE_SHIFT))

/* Point `i` of the path of `owner`. */
#define PATH_POINT(owner, i) ((s16 *)func_80031AE8((owner), PATH_ITEM, 0, (i), 0, PATH_MODE, 0, 0, 0))

s32 func_800254D4(void *arg0, s32 arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5, s32 *arg6) {
    s32 target = (s32)arg2;
    s32 owner = *(s32 *)((u8 *)arg0 + 4);
    s32 pos = *(s32 *)((u8 *)arg0 + 8);
    s32 *origin;
    s32 qx, qy, qz;
    s32 last;
    s32 idx, next;
    s32 frac;
    s16 *p, *q;
    s32 ax, ay, az, bx, by, bz;
    s32 cur, best, bestPos, worse, prev, val, t;
    s32 planar;

    qx = ((s32 *)arg1)[0] >> FIXED_SHIFT;
    qy = ((s32 *)arg1)[1] >> FIXED_SHIFT;
    qz = ((s32 *)arg1)[2] >> FIXED_SHIFT;
    origin = *(s32 **)(*(s32 *)(owner + 4) + 0x14);
    idx = pos >> FIXED_SHIFT;
    last = func_800322C0(owner, PATH_ITEM, 0, PATH_MODE) - 1;
    p = PATH_POINT(owner, idx);
    if (idx < last) {
        next = idx + 1;
        q = (s16 *)((u8 *)p + POINT_STRIDE);
    } else {
        next = idx;
        q = p;
    }
    frac = pos & FRAC_MASK;
    planar = (arg5 & PLANE_FLAGS) != 0;

    /* Only the axes in use are read; the others may point at unmapped memory. */
    if (planar) {
        ax = p[0] + origin[0];
        az = p[2] + origin[2];
        bx = q[0] + origin[0];
        bz = q[2] + origin[2];
        cur = func_800254D4_proj(qx - (ax + (((bx - ax) * frac) >> FIXED_SHIFT)),
                                 qz - (az + (((bz - az) * frac) >> FIXED_SHIFT)), arg4, arg3, arg5);
    } else {
        ay = p[1] + origin[1];
        by = q[1] + origin[1];
        cur = ((ay + (((by - ay) * frac) >> FIXED_SHIFT)) - qy) << FIXED_SHIFT;
    }
    best = target - cur;
    if (cur >= target) {
        *arg6 = pos;
        return cur - target;
    } else {
        /* the query lies beyond the current position */
    }
    bestPos = pos;

    if (*(s32 *)((u8 *)arg0 + 0xC) == DIR_FORWARD) {
        if (planar) {
            val = func_800254D4_proj(qx - bx, qz - bz, arg4, arg3, arg5);
        } else {
            val = (by - qy) << FIXED_SHIFT;
        }
        if (val >= target) {
            *arg6 = pos + ((((next << FIXED_SHIFT) - pos) * (((target - cur) << FIXED_SHIFT) / (val - cur))) >> FIXED_SHIFT);
            return 0;
        } else {
            /* the target is past the next point */
        }
        worse = 0;
        if (target - val < best) {
            best = target - val;
            bestPos = next << FIXED_SHIFT;
            worse = 0;
        } else {
            /* not closer */
        }
        for (;;) {
            if (next == last) {
                *arg6 = bestPos;
                return best;
            } else {
                /* more points */
            }
            t = next;
            next = next + 1;
            q = PATH_POINT(owner, (next << FIXED_SHIFT) >> FIXED_SHIFT);
            prev = val;
            if (planar) {
                val = func_800254D4_proj(qx - (q[0] + origin[0]), qz - (q[2] + origin[2]), arg4, arg3, arg5);
            } else {
                val = ((q[1] + origin[1]) - qy) << FIXED_SHIFT;
            }
            if (val >= target) {
                *arg6 = (t << FIXED_SHIFT) + (((target - prev) << FIXED_SHIFT) / (val - prev));
                return 0;
            } else {
                /* still short of the target */
            }
            worse++;
            if (best >= target - val) {
                best = target - val;
                bestPos = next << FIXED_SHIFT;
                worse = 0;
            } else if (worse >= MAX_WORSE_STEPS) {
                *arg6 = bestPos;
                return -1;
            } else {
                /* keep walking */
            }
        }
    } else {
        if (planar) {
            val = func_800254D4_proj(qx - ax, qz - az, arg4, arg3, arg5);
        } else {
            val = (ay - qy) << FIXED_SHIFT;
        }
        if (val >= target) {
            *arg6 = pos - (((pos - (idx << FIXED_SHIFT)) * (((target - cur) << FIXED_SHIFT) / (val - cur))) >> FIXED_SHIFT);
            return 0;
        } else {
            /* the target is before the point */
        }
        worse = 0;
        if (target - val < best) {
            best = target - val;
            bestPos = idx << FIXED_SHIFT;
            worse = 0;
        } else {
            /* not closer */
        }
        for (;;) {
            t = idx;
            if (idx == 0) {
                *arg6 = bestPos;
                return best;
            } else {
                /* more points */
            }
            idx = idx - 1;
            p = PATH_POINT(owner, (idx << FIXED_SHIFT) >> FIXED_SHIFT);
            prev = val;
            if (planar) {
                val = func_800254D4_proj(qx - (p[0] + origin[0]), qz - (p[2] + origin[2]), arg4, arg3, arg5);
            } else {
                val = ((p[1] + origin[1]) - qy) << FIXED_SHIFT;
            }
            if (val >= target) {
                *arg6 = (t << FIXED_SHIFT) - (((target - prev) << FIXED_SHIFT) / (val - prev));
                return 0;
            } else {
                /* still short of the target */
            }
            worse++;
            if (target - val < best) {
                best = target - val;
                bestPos = idx << FIXED_SHIFT;
                worse = 0;
            } else if (worse >= MAX_WORSE_STEPS) {
                *arg6 = bestPos;
                return -1;
            } else {
                /* keep walking */
            }
        }
    }
}
