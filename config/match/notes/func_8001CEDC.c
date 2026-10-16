#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 x, y, z; } Vec80030D98;
typedef struct { Vec80030D98 min; Vec80030D98 max; } Box80030D98;
typedef struct { s32 pad0[1]; s32 kind; } UnkQ;
typedef struct { s32 pad0[4]; UnkQ *q; } UnkP;
typedef struct { u8 pad0[0xC]; UnkP *p; u8 pad1[0x50]; Vec80030D98 pos; } UnkA;
typedef struct { u8 pad0[0x60]; Vec80030D98 lo; u8 pad1[0x90 - 0x6C]; Vec80030D98 sz; } UnkE;
typedef struct { UnkE *e; s32 f4; s32 f8; s32 kind; s32 count; void *f14; } UnkB;

extern void *func_8002C324(void *, s32, s32);
extern s32 func_80030EC0(void *, void *);
extern void func_8002CA1C(void);

s32 func_8001CEDC(UnkA *a, UnkB *b) {
    void *v;
    void *w;
    s32 r;
    Box80030D98 box;
    s32 n;

    if (b->kind != 0) {
        switch (b->kind) {
        case 1:
            if (a->p->q->kind != 0x300 && a->p->q->kind != 0x400) {
                return -0xFF;
            }
            v = func_8002C324(a, 0, -2);
            w = func_8002C324(b->e, 0, -2);
            if (v == 0) {
                return -0xFF;
            }
            r = func_80030EC0((u8 *)v + 8, (u8 *)w + 8);
            func_8002CA1C();
            func_8002CA1C();
            if (r == 0) {
                return -0xFF;
            }
            break;
        case 2:
            if (a->p->q->kind != 0x300 && a->p->q->kind != 0x400) {
                return -0xFF;
            }
            v = func_8002C324(b->e, 0, -2);
            if (v == 0) {
                return -0xFF;
            }
            r = func_80030D98(&a->pos, (Box80030D98 *)((u8 *)v + 8));
            func_8002CA1C();
            if (r == 0) {
                return -0xFF;
            }
            break;
        case 3:
            if (a->p->q->kind != 0x300 && a->p->q->kind != 0x400) {
                return -0xFF;
            }
            v = func_8002C324(a, 0, -2);
            if (v == 0) {
                return -0xFF;
            }
            box.min.x = b->e->lo.x - b->e->sz.x;
            box.min.y = b->e->lo.y - b->e->sz.y;
            box.min.z = b->e->lo.z - b->e->sz.z;
            box.max.x = b->e->sz.x + b->e->lo.x;
            box.max.y = b->e->sz.y + b->e->lo.y;
            box.max.z = b->e->sz.z + b->e->lo.z;
            r = func_80030EC0((u8 *)v + 8, &box);
            func_8002CA1C();
            if (r == 0) {
                return -0xFF;
            }
            n = b->count;
            if (n >= 3 && n % 5 != 0) {
                b->count = n + 1;
                return -0xFF;
            }
            break;
        case 4:
        case 5:
        default:
            break;
        }
    }
    b->count++;
    func_8001CC10((s32)b->e, (s32)a, b->f4, b->f8, b->f14);
    return -0xFF;
}
