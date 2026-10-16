#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 a;
    s32 flags;
    s32 pad8;
    struct Obj *key;
    u8 pad10[0xD];
    u8 b;
    u8 pad1E[0x32];
} Ent;
typedef struct {
    s32 mask;
    s32 pad4;
    s16 x;
    s16 y;
    u8 pad[0x28];
} Out;
s32 D_8005F304;
extern Ent D_8005F4EC[];
extern u8 D_8005FC9D;
extern void func_800521A8(Out *);

s32 func_800328D4(void) {
    Out o;
    Ent *p;
    s32 i;
    s32 j;
    s32 ret;
    s32 min;
    s32 ax;
    s32 v;
    s32 dist;

    i = D_8005F304;
    dist = 0x3FFF;
    min = 0xFF;
    p = &D_8005F4EC[i];
    for (; i < 0x18; i++, p++) {
        if (p->b < min) {
            min = p->b;
        }
        if (!(p->flags & 8)) {
            return i;
        }
    }
    ret = -1;
    if (D_8005FC9D >= min) {
        j = D_8005F304;
        p = &D_8005F4EC[j];
        for (; j < 0x18; j++, p++) {
            if (p->b == min) {
                o.mask = 1 << j;
                func_800521A8(&o);
                ax = o.x;
                if (ax < 0) {
                    ax = -ax;
                }
                if (o.y >= 0) {
                    if (o.y < ax) {
                        v = o.x;
                    } else {
                        v = o.y;
                    }
                } else if (-o.y < ax) {
                    v = o.x;
                } else {
                    v = o.y;
                }
                if (v < 0) {
                    v = -v;
                }
                if (v < dist) {
                    ret = j;
                    dist = v;
                }
            }
        }
        if (min == D_8005FC9D && dist >= 0x800 && func_80031578(0x64) >= 0x32) {
            ret = -1;
        }
    }
    return ret;
}
