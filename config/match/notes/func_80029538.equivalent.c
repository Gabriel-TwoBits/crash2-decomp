#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_COUNT 0x200
#define FLAG_FIXED_HEIGHT 0x40000000
#define SLOT_ACTIVE 4
#define LIST_END 0xFFFF
#define LIST_OFFSET 0x10
#define HALF_EXTENT 0x12C00
#define Y_LOW_OFFSET 0x25800
#define Y_LOW_FIXED_OFFSET 0x10B80
#define Y_HIGH_FIXED_OFFSET 0x3A480

typedef struct { u16 id; char pad[6]; } Unk80029538Entry;
typedef struct {
    Unk80029538Entry ent[ENTRY_COUNT];
    s32 one;
    s32 count;
    s32 b[6];
} Unk80029538Out;
typedef struct { char pad[0x40]; s32 unk_40; } Unk80029538Sub;
typedef struct { char pad[8]; Unk80029538Sub *sub; char pad2[0xF4]; u32 flags; } Unk80029538Ctx;
typedef struct { char pad[0x190]; s32 n; void *slot[1]; } Unk80029538Lst;

s32 func_8003BF90(s32 *, s32 *, void *, s32);

void func_80029538(s32 arg0, s32 arg1, s32 arg2) {
    s32 *a = (s32 *)arg0;
    Unk80029538Ctx *b = (Unk80029538Ctx *)arg1;
    Unk80029538Out *c = (Unk80029538Out *)arg2;
    Unk80029538Lst *l;
    s32 *s;
    s32 *t;
    s32 y;
    s32 i;

    c->count = 0;
    c->one = 1;
    c->b[0] = a[0] - HALF_EXTENT;
    y = a[1];
    if (!(b->flags & FLAG_FIXED_HEIGHT)) {
        c->b[1] = y + b->sub->unk_40 / 2 - Y_LOW_OFFSET;
    } else {
        c->b[1] = y - Y_LOW_FIXED_OFFSET;
    }
    c->b[2] = a[2] - HALF_EXTENT;
    c->b[3] = a[0] + HALF_EXTENT;
    y = a[1];
    if (!(b->flags & FLAG_FIXED_HEIGHT)) {
        c->b[4] = y + b->sub->unk_40 / 2 + Y_LOW_OFFSET;
    } else {
        c->b[4] = y + Y_HIGH_FIXED_OFFSET;
    }
    c->b[5] = a[2] + HALF_EXTENT;
    l = *(Unk80029538Lst **)(D_800608CC + LIST_OFFSET);
    s = c->b;
    for (i = 0; i < l->n; i++) {
        if (*(s32 *)((char *)l + 0x1B4 + i * 4) & SLOT_ACTIVE) {
            t = *(s32 **)((char *)func_80014260((char *)l + 0x194 + i * 4, 1, 0) + 0x14);
            if (func_80030E1C(s, t) != 0) {
                c->count = func_8003BF90(t, s, &c->ent[c->count], c->count);
            }
        }
    }
    c->ent[c->count].id = LIST_END;
}
