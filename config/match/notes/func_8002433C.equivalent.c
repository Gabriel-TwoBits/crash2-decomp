#include "common.h"
#include "sdk.h"
#include "game.h"

#define KEY_QUERY 0x29
#define IDX_QUERY 0x173
#define KEY_ENTRY 0x4B
#define QUERY_FLAGS 2
#define MODE_ROTATE_ADD 1
#define MODE_NO_ROTATE 2
#define MODE_FROZEN 3
#define FIXED_SHIFT 8
#define TRIG_SHIFT 4
#define ENTRY_STRIDE_WORDS 3

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Arg80023B04;

typedef struct {
    s32 x, y, z;
} Unk8002433CVec;
typedef struct {
    s32 pad0[5];
    Unk8002433CVec *pos;
} Unk8002433CInfo;
typedef struct {
    s32 pad0[1];
    Unk8002433CInfo *info;
} Unk8002433CObj;
typedef struct {
    s32 pad[0x61];
    s32 base;
} Unk8002433CTab;
typedef struct {
    s32 pad[4];
    Unk8002433CTab *tab;
} Unk8002433CRoot;
typedef struct {
    s32 pad[2];
    s32 mode;
} Unk8002433CMode;

extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060828;
extern s32 D_800608D4[];
extern Unk8002433CMode *D_8006759C;
void func_80023B04(Arg80023B04 *arg0, s32 *arg1, s32 *arg2);
s32 func_800242F8(s32 arg0);

s32 func_8002433C(s32 *arg0, s32 *arg1, s32 *a2, s32 *a3, s32 *a4) {
    Arg80023B04 *a0 = (Arg80023B04 *)arg0;
    Unk8002433CVec *a1 = (Unk8002433CVec *)arg1;
    s32 dx, dy, dz, lo, hi;
    s32 obj, ang, mode, ang2, s, c, r;
    s16 *e;
    Unk8002433CVec *o;

    func_80023B04(a0, &lo, &hi);
    if (a2 != 0) {
        *a2 = lo;
    }
    if (a3 != 0) {
        *a3 = hi;
    }
    obj = a0->unk4;
    ang = a0->unk8;
    mode = func_800242F8(func_800321D4(obj, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0));
    if (obj == D_800608D4[0]) {
        dx = a1->x - D_800607F4;
        dy = a1->y - D_800607F8;
        ang2 = D_80060828;
        dz = a1->z - D_800607FC;
    } else {
        ang >>= FIXED_SHIFT;
        e = (s16 *)func_80031AE8(obj, KEY_ENTRY, 0, ang, 0, QUERY_FLAGS, 0, 0, 0);
        o = ((Unk8002433CObj *)obj)->info->pos;
        dx = a1->x - ((o->x + e[0]) << FIXED_SHIFT);
        dy = a1->y - ((o->y + e[1]) << FIXED_SHIFT);
        dz = a1->z - ((o->z + e[2]) << FIXED_SHIFT);
        if (mode != MODE_NO_ROTATE) {
            Unk8002433CRoot *rt = (Unk8002433CRoot *)a0->unk0;
            s32 i = func_800321D4(obj, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
            e = (s16 *)func_80031AE8(*(s32 *)((s32)rt + (i * ENTRY_STRIDE_WORDS + rt->tab->base) * 4 + 0x14), KEY_ENTRY, 0, ang * 2, 0, QUERY_FLAGS, 0, 0, 0);
            ang2 = e[1];
        } else {
            ang2 = 0;
        }
    }
    if (D_8006759C->mode == MODE_FROZEN) {
        ang2 = 0;
    }
    if (mode == MODE_NO_ROTATE) {
        r = -dy;
    } else {
        ang2 = -ang2;
        if (a4 != 0) {
            *a4 = ang2;
        }
        s = func_8003D360(ang2);
        c = func_8003D2EC(ang2);
        if (mode == MODE_ROTATE_ADD) {
            r = ((dz >> FIXED_SHIFT) * c >> TRIG_SHIFT) + ((dx >> FIXED_SHIFT) * s >> TRIG_SHIFT);
        } else {
            r = ((dx >> FIXED_SHIFT) * c >> TRIG_SHIFT) - ((dz >> FIXED_SHIFT) * s >> TRIG_SHIFT);
        }
    }
    if (hi < r) {
        return 1;
    }
    return -(r < lo);
}
