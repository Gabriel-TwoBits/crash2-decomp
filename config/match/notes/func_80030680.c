#include "common.h"

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
    s32 *a;
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
        a = xs;
        b = xs + 1;
        for (i = n - 1; i > 0; i--) {
            if (*b < *a) {
                s32 t = *a;
                swapped = 1;
                *a = *b;
                *b = t;
            }
            a++;
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
        if (px == s[0]) {
            return 1;
        }
        return 0;
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
