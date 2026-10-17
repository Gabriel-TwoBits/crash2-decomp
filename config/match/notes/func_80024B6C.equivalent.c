#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80024B6CPoint {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} Unk80024B6CPoint;

typedef struct Unk80024B6CBase {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk80024B6CBase;

typedef struct Unk80024B6CPath {
    /* 0x0 */ s32 unk_0;
    /* 0x4 */ s32 unk_4;
    /* 0x8 */ s32 pos;
    /* 0xC */ s32 mode;
} Unk80024B6CPath;

#define UNK_80024B6C_PLANE_MASK 5
#define UNK_80024B6C_PLANE_FIRST 1
#define UNK_80024B6C_ENTRY_KIND 0x4B
#define UNK_80024B6C_ENTRY_SIZE 2
#define UNK_80024B6C_MODE_FORWARD 2
#define UNK_80024B6C_MAX_WORSE 3
#define UNK_80024B6C_BASE_SLOT 5

/* Distance of a point against the query: plane projection, or height when no plane flag is set. */
static inline s32 func_80024B6C_dist(s32 flags, s32 x, s32 y, s32 z, s32 qx, s32 qy, s32 qz, s32 c, s32 s) {
    s32 dx;
    s32 dz;

    if (flags & UNK_80024B6C_PLANE_MASK) {
        dx = qx - x;
        dz = qz - z;
        if (flags == UNK_80024B6C_PLANE_FIRST) {
            return ((dz * c) >> 4) + ((dx * s) >> 4);
        } else {
            return ((dx * c) >> 4) - ((dz * s) >> 4);
        }
    } else {
        return (y - qy) << 8;
    }
}

/* Same, reading only the point fields the flag needs. */
static inline s32 func_80024B6C_dist_at(s32 flags, Unk80024B6CPoint *p, Unk80024B6CBase *base, s32 qx, s32 qy, s32 qz,
                                        s32 c, s32 s) {
    if (flags & UNK_80024B6C_PLANE_MASK) {
        return func_80024B6C_dist(flags, p->x + base->x, 0, p->z + base->z, qx, qy, qz, c, s);
    } else {
        return func_80024B6C_dist(flags, 0, p->y + base->y, 0, qx, qy, qz, c, s);
    }
}

s32 func_80024B6C(void *arg0, s32 arg1, s32 *arg2, s32 arg3, void *arg4, s32 arg5, s32 *arg6) {
    Unk80024B6CPath *path;
    Unk80024B6CBase *base;
    Unk80024B6CPoint *p0;
    Unk80024B6CPoint *p1;
    Unk80024B6CPoint *pn;
    s32 *q;
    s32 obj;
    s32 qx, qy, qz;
    s32 last;
    s32 pos;
    s32 frac;
    s32 idx;
    s32 next;
    s32 x0, y0, z0, x1, y1, z1;
    s32 target;
    s32 here;
    s32 dnext;
    s32 dcur;
    s32 dprev;
    s32 dnew;
    s32 best;
    s32 fp;
    s32 worse;
    s32 c;
    s32 s;
    s32 cur;

    path = (Unk80024B6CPath *)arg0;
    s = arg3;
    c = (s32)arg4;
    target = (s32)arg2;
    obj = path->unk_4;
    pos = path->pos;
    q = (s32 *)arg1;
    qx = q[0] >> 8;
    qy = q[1] >> 8;
    qz = q[2] >> 8;
    idx = pos >> 8;
    base = (Unk80024B6CBase *)((s32 **)((s32 **)obj)[1])[UNK_80024B6C_BASE_SLOT];
    last = func_800322C0(obj, UNK_80024B6C_ENTRY_KIND, 0, UNK_80024B6C_ENTRY_SIZE) - 1;
    p0 = (Unk80024B6CPoint *)func_80031AE8(obj, UNK_80024B6C_ENTRY_KIND, 0, idx, 0, UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
    next = idx + 1;
    if (idx < last) {
        p1 = p0 + 1;
    } else {
        next = idx;
        p1 = p0;
    }
    frac = pos & 0xFF;

    /* Only the fields the flag needs are read: x and z for a plane, y otherwise. */
    x0 = y0 = z0 = x1 = y1 = z1 = 0;
    if (arg5 & UNK_80024B6C_PLANE_MASK) {
        x0 = p0->x + base->x;
        z0 = p0->z + base->z;
        x1 = p1->x + base->x;
        z1 = p1->z + base->z;
    } else {
        y0 = p0->y + base->y;
        y1 = p1->y + base->y;
    }

    here = func_80024B6C_dist(arg5, x0 + (((x1 - x0) * frac) >> 8), y0 + (((y1 - y0) * frac) >> 8),
                              z0 + (((z1 - z0) * frac) >> 8), qx, qy, qz, c, s);
    if (target >= here) {
        *arg6 = pos;
        return target - here;
    } else {
        /* target not reached at the current position */
    }
    best = here - target;
    fp = pos;

    if (path->mode == UNK_80024B6C_MODE_FORWARD) {
        dnext = func_80024B6C_dist(arg5, x1, y1, z1, qx, qy, qz, c, s);
        if (target >= dnext) {
            *arg6 = pos + ((((next << 8) - pos) * (((here - target) << 8) / (here - dnext))) >> 8);
            return 0;
        } else {
            /* target lies further on */
        }
        worse = 0;
        if (dnext - target < best) {
            best = dnext - target;
            fp = next << 8;
        } else {
            /* not closer */
        }
        while (1) {
            cur = next;
            if (next == last) {
                *arg6 = fp;
                return best;
            } else {
                /* more points */
            }
            next = cur + 1;
            pn = (Unk80024B6CPoint *)func_80031AE8(obj, UNK_80024B6C_ENTRY_KIND, 0, (next << 8) >> 8, 0,
                                                   UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
            dprev = dnext;
            dnext = func_80024B6C_dist_at(arg5, pn, base, qx, qy, qz, c, s);
            dnew = dnext - target;
            if (target >= dnext) {
                *arg6 = (cur << 8) + (((dprev - target) << 8) / (dprev - dnext));
                return 0;
            } else {
                /* still short */
            }
            worse += 1;
            if (dnew < best) {
                best = dnew;
                fp = next << 8;
                worse = 0;
            } else if (worse >= UNK_80024B6C_MAX_WORSE) {
                *arg6 = fp;
                return -1;
            } else {
                /* keep scanning */
            }
        }
    } else {
        dcur = func_80024B6C_dist(arg5, x0, y0, z0, qx, qy, qz, c, s);
        if (target >= dcur) {
            *arg6 = pos - (((pos - (idx << 8)) * (((here - target) << 8) / (here - dcur))) >> 8);
            return 0;
        } else {
            /* target lies further back */
        }
        worse = 0;
        if (dcur - target < best) {
            best = dcur - target;
            fp = idx << 8;
        } else {
            /* not closer */
        }
        while (1) {
            cur = idx;
            if (idx == 0) {
                *arg6 = fp;
                return best;
            } else {
                /* more points */
            }
            idx -= 1;
            pn = (Unk80024B6CPoint *)func_80031AE8(obj, UNK_80024B6C_ENTRY_KIND, 0, (idx << 8) >> 8, 0,
                                                   UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
            dprev = dcur;
            dcur = func_80024B6C_dist_at(arg5, pn, base, qx, qy, qz, c, s);
            dnew = dcur - target;
            if (target >= dcur) {
                *arg6 = (cur << 8) - (((dprev - target) << 8) / (dprev - dcur));
                return 0;
            } else {
                /* still short */
            }
            worse += 1;
            if (dnew < best) {
                best = dnew;
                fp = idx << 8;
                worse = 0;
            } else if (worse >= UNK_80024B6C_MAX_WORSE) {
                *arg6 = fp;
                return -1;
            } else {
                /* keep scanning */
            }
        }
    }
}
