#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 x, y, z; } Vec3;
typedef struct { s32 pad0[5]; Vec3 *pos; } Obj14;
typedef struct { s32 pad0[1]; Obj14 *info; } ObjHdr;
typedef struct { s32 pad[0x61]; s32 base; } Tab;
typedef struct { s32 pad[4]; Tab *tab; } Root;
typedef struct { Root *root; s32 obj; s32 ang; } Arg0;
typedef struct { s32 pad[2]; s32 mode; } Mode;

extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060828;
extern s32 D_800608D4[];
extern Mode *D_8006759C;
s32 func_80023B04(s32 *, s32 *);
s32 func_800242F8(s32);

s32 func_8002433C(Arg0 *a0, Vec3 *a1, s32 *a2, s32 *a3, s32 *a4) {
    s32 sp28, sp2C, sp30, sp38, sp3C;
    s32 obj, ang, mode, ang2, s, c, r;
    s16 *e;
    Vec3 *o;

    func_80023B04(&sp38, &sp3C);
    if (a2 != 0) {
        *a2 = sp38;
    }
    if (a3 != 0) {
        *a3 = sp3C;
    }
    obj = a0->obj;
    ang = a0->ang;
    mode = func_800242F8(func_800321D4(obj, 0x29, 0, 0, 0, 2, 0));
    if (obj == D_800608D4[0]) {
        sp28 = a1->x - D_800607F4;
        sp2C = a1->y - D_800607F8;
        ang2 = D_80060828;
        sp30 = a1->z - D_800607FC;
    } else {
        ang >>= 8;
        e = (s16 *)func_80031AE8(obj, 0x4B, 0, ang, 0, 2, 0, 0, 0);
        o = ((ObjHdr *)obj)->info->pos;
        sp28 = a1->x - ((o->x + e[0]) << 8);
        sp2C = a1->y - ((o->y + e[1]) << 8);
        sp30 = a1->z - ((o->z + e[2]) << 8);
        if (mode != 2) {
            Root *rt = a0->root;
            s32 i = func_800321D4(obj, 0x173, 0, 0, 0, 2, 0);
            e = (s16 *)func_80031AE8(*(s32 *)((s32)rt + (i * 3 + rt->tab->base) * 4 + 0x14), 0x4B, 0, ang * 2, 0, 2, 0, 0, 0);
            ang2 = e[1];
        }
    }
    if (D_8006759C->mode == 3) {
        ang2 = 0;
    }
    if (mode == 2) {
        r = -sp2C;
    } else {
        ang2 = -ang2;
        if (a4 != 0) {
            *a4 = ang2;
        }
        s = func_8003D360(ang2);
        c = func_8003D2EC(ang2);
        if (mode == 1) {
            r = ((sp30 >> 8) * c >> 4) + ((sp28 >> 8) * s >> 4);
        } else {
            r = ((sp28 >> 8) * c >> 4) - ((sp30 >> 8) * s >> 4);
        }
    }
    if (sp3C < r) {
        return 1;
    }
    return -(r < sp38);
}
