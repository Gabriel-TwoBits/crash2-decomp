#include "common.h"
#include "sdk.h"

void *func_80014260(void *, s32, s32);

typedef struct { char pad[0x40]; s32 f40; } Sub;
typedef struct { char pad[8]; Sub *sub; char pad2[0xF4]; u32 flags; } Ctx;
typedef struct { u16 id; char pad[6]; } Ent;
typedef struct {
    Ent ent[0x200];
    s32 one;
    s32 count;
    s32 b[8];
} Out;
typedef struct { char pad[0x190]; s32 n; s32 slot[1]; } Lst;
typedef struct { char pad[0x10]; Lst *lst; } Glob;

extern Glob *D_800608CC;
s32 func_80030E1C(s32, s32);
s32 func_8003BF90(s32, s32, s32, s32);

s32 func_80029538(s32 *a, Ctx *b, Out *c) {
    s32 pad[8];
    Lst *l;
    s32 *s;
    s32 t;
    s32 y;
    char *p;
    s32 off;
    s32 i;

    c->count = 0;
    c->one = 1;
    c->b[0] = a[0] - 0x12C00;
    y = a[1];
    if (!(b->flags & 0x40000000)) {
        c->b[1] = y + b->sub->f40 / 2 - 0x25800;
    } else {
        c->b[1] = y - 0x10B80;
    }
    c->b[2] = a[2] - 0x12C00;
    c->b[3] = a[0] + 0x12C00;
    y = a[1];
    if (!(b->flags & 0x40000000)) {
        c->b[4] = y + b->sub->f40 / 2 + 0x25800;
    } else {
        c->b[4] = y + 0x3A480;
    }
    c->b[5] = a[2] + 0x12C00;
    l = D_800608CC->lst;
    i = 0;
    if (l->n > 0) {
        s = c->b;
        off = 0x194;
        p = (char *)l;
        do {
            if (*(s32 *)(p + 0x1B4) & 4) {
                t = *(s32 *)((char *)func_80014260((char *)l + off, 1, 0) + 0x14);
                if (func_80030E1C((s32)s, t) != 0) {
                    c->count = func_8003BF90(t, (s32)s, (s32)&c->ent[c->count], c->count);
                }
            }
            off += 4;
            i += 1;
            p += 4;
        } while (i < l->n);
    }
    c->ent[c->count].id = 0xFFFF;
    return c->count;
}
