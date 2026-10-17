#include "common.h"
#include "sdk.h"
#include "game.h"

#define KIND_300 0x300
#define KIND_400 0x400
#define FAIL_RESULT -0xFF
#define SPLIT_PERIOD 5
#define SPLIT_MIN_COUNT 3

typedef struct { s32 x, y, z; } Vec80030D98;
typedef struct { Vec80030D98 min; Vec80030D98 max; } Box80030D98;
typedef struct { s32 pad0[1]; s32 kind; } UnkQ;
typedef struct { s32 pad0[4]; UnkQ *q; } UnkP;
typedef struct { u8 pad0[0xC]; UnkP *p; u8 pad1[0x50]; Vec80030D98 pos; } UnkA;
typedef struct { u8 pad0[0x60]; Vec80030D98 lo; u8 pad1[0x90 - 0x6C]; Vec80030D98 sz; } UnkE;
typedef struct { UnkE *e; s32 f4; s32 f8; s32 kind; s32 count; void *f14; } UnkB;

s32 func_8001CEDC(UnkA *a, UnkB *b) {
    s32 *v;
    s32 *w;
    s32 r;
    s32 n;
    Box80030D98 box;

    switch (b->kind) {
    case 1:
        v = func_8002C324(a, 0, -2);
        w = func_8002C324(b->e, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        r = func_80030EC0((s32 *)((u8 *)v + 8), (s32 *)((u8 *)w + 8));
        func_8002CA1C();
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 2:
        v = func_8002C324(b->e, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        r = func_80030D98(&a->pos, (Box80030D98 *)((u8 *)v + 8));
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 3:
        if (a->p->q->kind != KIND_300 && a->p->q->kind != KIND_400) {
            return FAIL_RESULT;
        }
        v = func_8002C324(b->e, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        r = func_80030D98(&a->pos, (Box80030D98 *)((u8 *)v + 8));
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 4:
        if (a->p->q->kind != KIND_300 && a->p->q->kind != KIND_400) {
            return FAIL_RESULT;
        }
        v = func_8002C324(a, 0, -2);
        w = func_8002C324(b->e, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        r = func_80030EC0((s32 *)((u8 *)v + 8), (s32 *)((u8 *)w + 8));
        func_8002CA1C();
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 5:
        if (a->p->q->kind != KIND_300 && a->p->q->kind != KIND_400) {
            return FAIL_RESULT;
        }
        v = func_8002C324(a, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        box.min.x = b->e->lo.x - b->e->sz.x;
        box.min.y = b->e->lo.y - b->e->sz.y;
        box.min.z = b->e->lo.z - b->e->sz.z;
        box.max.x = b->e->sz.x + b->e->lo.x;
        box.max.y = b->e->sz.y + b->e->lo.y;
        box.max.z = b->e->sz.z + b->e->lo.z;
        r = func_80030EC0((s32 *)((u8 *)v + 8), (s32 *)&box);
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        n = b->count;
        if (n >= SPLIT_MIN_COUNT && n % SPLIT_PERIOD != 0) {
            b->count = n + 1;
            return FAIL_RESULT;
        }
        break;
    default:
        break;
    }
    b->count++;
    func_8001CC10((s32)b->e, (s32)a, b->f4, b->f8, b->f14);
    return FAIL_RESULT;
}
