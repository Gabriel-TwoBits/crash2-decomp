#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Ent { s32 f0; s32 f4; u16 f8; u16 fA; u16 fC; u16 fE; } Ent;

s32 func_8001C620(void *, s32, s32);
s32 func_8003A014(void *, s32, s32);
extern s32 D_8006CB60;
extern s32 D_8006CB64;

#define F(T, o) (*(T *)((char *)p + (o)))

s32 func_8001C340(void *p, s32 idx, s32 n, s32 *src) {
    s32 old = F(s32, 0xD0);
    Ent *e;
    u16 w;
    s32 r;
    char *base;
    char *a;
    s32 i;
    s32 *q;
    int new_var;

    if (idx == 0xFF || (F(s32, 0xAC) & 0x10000000)) {
        return -0xE;
    }
    new_var = 0xC4;
    F(s32, 0x1C) = idx;
    e = (Ent *)(*(char **)(*(char **)((char *)p + 0xC) + 0x20) + idx * 0x10);
    F(s32, 0xB0) = e->f4;
    a = func_80014260(*(char **)(*(char **)((char *)p + 0x10) + 0x18) + e->f8 * 4, 1, 0);
    F(void *, 0x14) = a;

    w = e->fE;
    if ((w & 0xFFFF) != 0x3FFF) {
        if (w & 0x4000) {
            r = *(s32 *)(a + 0x14);
        } else {
            r = *(s32 *)(F(char *, 0xC) + 0x14);
        }
        r += (w & 0x3FFF) * 4;
    } else {
        r = 0;
    }
    F(s32, 0xC0) = r;

    w = e->fA;
    if ((w & 0xFFFF) != 0x3FFF) {
        if (w & 0x4000) {
            base = F(char *, 0x14);
        } else {
            base = F(char *, 0xC);
        }
        r = *(s32 *)(base + 0x14) + (w & 0x3FFF) * 4;
    } else {
        r = 0;
    }
    F(s32, 0xCC) = r;

    w = e->fC;
    if ((w & 0xFFFF) != 0x3FFF) {
        if (w & 0x4000) {
            base = F(char *, 0x14);
        } else {
            base = F(char *, 0xC);
        }
        r = *(s32 *)(base + 0x14) + (w & 0x3FFF) * 4;
    } else {
        r = 0;
    }
    F(s32, 0xC8) = r;

    F(s32, 0xD0) = 0;
    F(char *, new_var) =(char *)p + 0x40;
    F(char *, 0xBC) = (char *)p + 0x40 + (*(s32 *)(*(char **)(*(char **)((char *)p + 0xC) + 0x10) + 0xC)) * 4;
    F(s32, 0xA8) |= 0x20020;
    F(s32, 0x100) = e->f0;
    for (i = 0; i < n; i++) {
        q = F(s32 *, 0xBC);
        F(s32 *, 0xBC) = q + 1;
        *q = *src++;
    }
    func_8001C620(p, n, 0xFFFF);
    if (old != 0) {
        func_8001C620(p, 0, 0xFFFF);
        F(s32, 0xC0) = old;
        i = func_8003A014(p, 0x13, 0);
        if (i >= -0xFE) {
            return i;
        }
    } else {
        q = F(s32 *, 0xBC);
        F(s32 *, 0xBC) = q + 1;
        *q = 0;
    }
    F(s32, 0xE0) = D_8006CB64;
    if (F(s32, 0xC8) != 0) {
        if ((s32)p == D_8006CB60) {
            func_8001C620(p, 0, 0xFFFF);
            F(s32, 0xC0) = F(s32, 0xC8);
            i = func_8003A014(p, 3, 0);
            if (i >= -0xFE) {
                return i;
            }
        } else {
            return -0xFF;
        }
    }
    return -0xFF;
}
