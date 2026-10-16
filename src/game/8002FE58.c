#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F218[] = "pb??B";

typedef struct { char c[6]; } S6;
extern char D_8005F218[];
extern char D_80067588[];
extern s32 D_8006758C, D_800675AC, D_8006CBB4, D_8006CD14, D_8005F3EC;
void func_8002FE58(s32 *arg0) {
    char buf[8];
    s32 r = func_80012294(D_80067588, 0x13);
    s32 t;
    *(S6 *)buf = *(S6 *)D_8005F218;
    memset(buf + 6, 0, 2);
    if (r == 0) {
        t = D_8005F3EC;
        *arg0 = 0x6396347F;
        if (t != 2) {
            D_8006CBB4 = 0;
        }
    } else {
        buf[2] = func_80031578(r) + 0x30;
        buf[3] = func_800120EC(D_8006758C);
        *arg0 = func_80011F78(buf);
        D_800675AC = -1;
        D_8006CD14 = 3;
    }
}

void func_8002FF54(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 a;
    s32 b;
    s32 t;

    t = (arg1 >> 8) * func_8003D2EC(arg2);
    arg0[1] = t >> 4;
    a = arg1 >> 8;
    b = a * func_8003D360(arg2);
    arg0[0] = ((b >> 12) * func_8003D2EC(arg3)) >> 4;
    arg0[2] = ((b >> 12) * func_8003D360(arg3)) >> 4;
}

INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_8002FFFC);

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Unk80030498;

s32 func_80030498(s32 arg0, s32 arg1, Unk80030498 *arg2) {
    s32 i;
    s32 found;
    s32 d0, d1, r;
    s32 s0, s1;
    s32 x1, y1, x2, y2, den;
    Unk80030498 *p;
    Unk80030498 *q;
    Unk80030498 *p1;
    Unk80030498 *q1;

    found = 0;
    p = arg2;
    for (i = 0; i < 4; i++) {
        q = &arg2[(i + 1) & 3];
        d1 = q->z - arg1;
        d0 = arg1 - p->z;
        if (d0 <= 0) {
            if (d1 > 0) {
                if (d0 >= 0) {
                    goto l4;
                }
                goto skip;
            }
            goto cross;
        }
l4:
        if (d1 >= 0) {
cross:
            if (!found) {
                found = 1;
                s0 = d0;
                s1 = d1;
                p1 = p;
                q1 = q;
                goto skip;
            }
            den = s0 + s1;
            if (den == 0) den = 1;
            r = (s0 << 12) / den;
            x1 = ((r * (q1->x - p1->x)) >> 12) + p1->x;
            y1 = ((r * (q1->y - p1->y)) >> 12) + p1->y;
            den = d0 + d1;
            if (den == 0) den = 1;
            r = (d0 << 12) / den;
            x2 = ((r * (q->x - p->x)) >> 12) + p->x;
            y2 = ((r * (q->y - p->y)) >> 12) + p->y;
            den = x2 - x1;
            if (den == 0) den = 1;
            r = ((arg0 - x1) << 12) / den;
            return ((r * (y2 - y1)) >> 12) + y1;
        }
skip:
        p = q;
    }
    return 0xC4653601;
}

#ifdef NON_MATCHING
typedef struct {
    s32 x;
    s32 y;
} Unk80030680;

s32 func_80030680(Unk80030680 *point, Unk80030680 *quad) {
    s32 xs[8];
    s32 minX, maxX, minY, maxY;
    s32 i, n, swapped;
    s32 px, py;
    s32 x0, y0, x1, y1;
    s32 x;
    s32 last;
    s32 *s;
    s32 *b;
    Unk80030680 *q;

    maxX = quad[0].x;
    maxY = quad[0].y;
    minX = maxX;
    minY = maxY;
    for (i = 1; i < 4; i++) {
        if (quad[i].x < minX) {
            minX = quad[i].x;
        }
        if (maxX < quad[i].x) {
            maxX = quad[i].x;
        }
        if (quad[i].y < minY) {
            minY = quad[i].y;
        }
        if (maxY < quad[i].y) {
            maxY = quad[i].y;
        }
    }
    px = point->x;
    py = point->y;
    if (px < minX) {
        return 0;
    }
    if (maxX < px) {
        return 0;
    }
    if (py < minY) {
        return 0;
    }
    if (maxY < py) {
        return 0;
    }
    if (minX == maxX) {
        return 1;
    }
    if (minY == maxY) {
        return 1;
    }
    n = 0;
    s = xs;
    i = 0;
    x0 = quad[0].x;
    y0 = quad[0].y;
    for (; i < 4; i++, x0 = x1, y0 = y1) {
        q = &quad[(i + 1) & 3];
        x1 = q->x;
        y1 = q->y;
        if (y0 == py) {
            if (y1 == y0) {
                *s = x1;
                s++;
                n++;
            }
            *s = x0;
        } else if (y1 == py) {
            *s = x1;
        } else {
            if (y0 < y1) {
                if (y1 < py || py < y0) {
                    goto next;
                }
                y0 = ((((py - y0) << 12) / (y1 - y0)) * (x1 - x0) >> 12) + x0;
                x = y0;
            } else {
                if (py < y1 || y0 < py) {
                    goto next;
                }
                y0 = ((((py - y1) << 12) / (y0 - y1)) * (x0 - x1) >> 12) + x1;
                x = y0;
            }
            *s = x;
        }
        s++;
        n++;
    next:;
    }
    do {
        swapped = 0;
        s = xs;
        b = xs + 1;
        for (i = n - 1; i > 0; i--) {
            if (*b < *s) {
                s32 t = *s;
                swapped = 1;
                *s = *b;
                *b = t;
            }
            s++;
            b++;
        }
    } while (swapped);
    s = xs;
    while (n >= 2 && s[1] == xs[0]) {
        n--;
        s++;
    }
    while (n >= 3 && s[n - 2] == s[n - 1]) {
        n--;
    }
    if (n == 1) {
        if (px != s[0]) {
            return 0;
        }
        return 1;
    }
    do {
        if (s[0] <= px && px <= s[1]) {
            return 1;
        }
        n -= 2;
        s += 2;
    } while (n > 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030680);
#endif

typedef struct {
    s32 min[3];
    s32 max[3];
} Unk800309A8Box;


void func_800309A8(Unk800309A8Box *out, Unk800309A8Box *in, s16 *m, s32 *trans) {
    s32 cz;
    s32 cy;
    s32 center[4];
    s32 mid[4];
    s32 ext[3][4];
    s32 extent;

    mid[0] = (in->min[0] + in->max[0]) >> 1;
    mid[1] = (in->min[1] + in->max[1]) >> 1;
    mid[2] = (in->min[2] + in->max[2]) >> 1;
    mid[0] /= 16;
    mid[1] /= 16;
    mid[2] /= 16;
    func_80018194(mid, m, center);
    center[0] <<= 4;
    center[1] <<= 4;
    center[2] <<= 4;
    center[0] += trans[0];
    center[1] += trans[1];
    center[2] += trans[2];

    extent = (in->max[0] - in->min[0]) >> 2;
    ext[0][0] = (extent * m[0]) >> 12;
    if (ext[0][0] < 0) {
        ext[0][0] = -ext[0][0];
    }
    ext[0][1] = (extent * m[3]) >> 12;
    if (ext[0][1] < 0) {
        ext[0][1] = -ext[0][1];
    }
    ext[0][2] = (extent * m[6]) >> 12;
    if (ext[0][2] < 0) {
        ext[0][2] = -ext[0][2];
    }
    extent = (in->max[1] - in->min[1]) >> 2;
    ext[1][0] = (extent * m[1]) >> 12;
    if (ext[1][0] < 0) {
        ext[1][0] = -ext[1][0];
    }
    ext[1][1] = (extent * m[4]) >> 12;
    if (ext[1][1] < 0) {
        ext[1][1] = -ext[1][1];
    }
    ext[1][2] = (extent * m[7]) >> 12;
    if (ext[1][2] < 0) {
        ext[1][2] = -ext[1][2];
    }
    extent = (in->max[2] - in->min[2]) >> 2;
    ext[2][0] = (extent * m[2]) >> 12;
    if (ext[2][0] < 0) {
        ext[2][0] = -ext[2][0];
    }
    ext[2][1] = (extent * m[5]) >> 12;
    if (ext[2][1] < 0) {
        ext[2][1] = -ext[2][1];
    }
    ext[2][2] = (extent * m[8]) >> 12;
    if (ext[2][2] < 0) {
        ext[2][2] = -ext[2][2];
    }

    extent = (ext[0][0] + ext[1][0] + ext[2][0]) * 2;
    out->max[0] = center[0] + extent;
    out->min[0] = center[0] - extent;
    cy = center[1];
    extent = (ext[0][1] + ext[1][1] + ext[2][1]) * 2;
    out->max[1] = cy + extent;
    out->min[1] = center[1] - extent;
    cz = center[2];
    extent = (ext[0][2] + ext[1][2] + ext[2][2]) * 2;
    out->max[2] = cz + extent;
    out->min[2] = center[2] - extent;
}

#ifdef NON_MATCHING
s32 func_80030CD8(s32 *a, s32 *p) {
    s32 x = a[0];
    s32 px = p[0];
    s32 y, py, z, pz;
    s32 r = 0;

    if (px >= (x << 8)) {
        y = a[1];
        py = p[1];
        if (py >= (y << 8)) {
            z = a[2];
            pz = p[2];
            if (pz >= (z << 8) && px < ((x + a[3]) << 8) && py < ((y + a[4]) << 8)) {
                r = pz < ((z + a[5]) << 8);
            }
        }
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030CD8);
#endif


s32 func_80030D74(s32 *arg0, void *arg1) {
    return func_80030CD8((void *)arg0[5], arg1);
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

#ifdef NON_MATCHING
s32 func_80030E1C(s32 *arg0, s32 *arg1) {
    s32 a = arg1[0];
    s32 b = arg1[1];
    s32 c = arg1[2];

    if ((a << 8) < arg0[3] && (b << 8) < arg0[4] && (c << 8) < arg0[5]
        && ((a + arg1[3]) << 8) >= arg0[0]
        && ((b + arg1[4]) << 8) >= arg0[1]) {
        return ((c + arg1[5]) << 8) >= arg0[2];
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FE58", func_80030E1C);
#endif

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
