#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define KIND_300 0x300
#define KIND_400 0x400
#define FAIL_RESULT -0xFF
#define SPLIT_PERIOD 5
#define SPLIT_MIN_COUNT 3

typedef struct { s32 x, y, z; } Vec80030D98;
typedef struct { Vec80030D98 min; Vec80030D98 max; } Box80030D98;
typedef struct { s32 pad0[1]; s32 kind; } UnkQ;
typedef struct { GoolObject *e; s32 f4; s32 f8; s32 kind; s32 count; void *f14; } UnkB;

/* The kind in the first item of GoolObject.exe, as this function reads it */
#define KIND(obj) (((UnkQ *)(obj)->exe->items[0])->kind)

s32 func_8001CEDC(GoolObject *a, UnkB *b) {
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
        r = func_80030D98((Vec80030D98 *)a->pos, (Box80030D98 *)((u8 *)v + 8));
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 3:
        if (KIND(a) != KIND_300 && KIND(a) != KIND_400) {
            return FAIL_RESULT;
        }
        v = func_8002C324(b->e, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        r = func_80030D98((Vec80030D98 *)a->pos, (Box80030D98 *)((u8 *)v + 8));
        func_8002CA1C();
        if (r == 0) {
            return FAIL_RESULT;
        }
        break;
    case 4:
        if (KIND(a) != KIND_300 && KIND(a) != KIND_400) {
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
        if (KIND(a) != KIND_300 && KIND(a) != KIND_400) {
            return FAIL_RESULT;
        }
        v = func_8002C324(a, 0, -2);
        if (v == 0) {
            return FAIL_RESULT;
        }
        box.min.x = b->e->pos[0] - b->e->target_rot[0];
        box.min.y = b->e->pos[1] - b->e->target_rot[1];
        box.min.z = b->e->pos[2] - b->e->target_rot[2];
        box.max.x = b->e->target_rot[0] + b->e->pos[0];
        box.max.y = b->e->target_rot[1] + b->e->pos[1];
        box.max.z = b->e->target_rot[2] + b->e->pos[2];
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
    Gool_SendEvent((s32)b->e, (s32)a, b->f4, b->f8, b->f14);
    return FAIL_RESULT;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001CEDC", func_8001CEDC);
#endif

/* Without a prototype: its equivalent C under NON_MATCHING defines the parameters' types */
s32 func_8001CEDC();

s32 Gool_BroadcastEvent(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 args[6];

    args[0] = arg0;
    args[1] = arg1;
    args[3] = arg2;
    args[2] = arg3;
    args[4] = 0;
    args[5] = arg4;
    Obj_ForEachList(Obj_ForEachDescendant, &func_8001CEDC, args);
    return -0xFF;
}

s32 Gool_CascadeEvent(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 sp[6];

    sp[0] = arg0;
    sp[1] = arg2;
    sp[3] = arg3;
    sp[4] = 0;
    sp[2] = arg4;
    sp[5] = arg5;
    Obj_ForEachDescendant((void *)arg1, &func_8001CEDC, sp);
    return -0xFF;
}

#ifdef NON_MATCHING
#define INDEX_MASK 0x3FFF
#define ARG_ALL_BITS 0xFFFF
#define MODE_THREE 3
#define BYTES_PER_ENTRY 4


s32 func_8001D22C(void *arg0, u16 arg1, s32 arg2, s32 *arg3) {
    s32 i;
    s32 **write_ptr;
    s32 *slot;
    GoolObject *base;

    base = arg0;
    write_ptr = &base->sp;
    for (i = 0; i < arg2; i++) {
        slot = *write_ptr;
        *write_ptr = slot + 1;
        *slot = *arg3;
        arg3++;
    }
    Gool_PushFrame(base, arg2, ARG_ALL_BITS);
    base->pc = (s32)base->exe->items[1] + (arg1 & INDEX_MASK) * BYTES_PER_ENTRY;
    return Gool_Interpret(arg0, MODE_THREE, 0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001CEDC", func_8001D22C);
#endif

#ifdef NON_MATCHING
int abs(int);
extern s32 D_80060914;
extern s32 *g_DisplayBuffer;
extern s32 D_80069958;
extern s32 D_8006995C;
extern s32 D_80069960;

void func_8001D2C4(GoolObject *arg0, s32 *arg1, s32 *arg2) {
    s32 in[4];
    s32 out[4];
    s32 speed;
    s32 mode;
    s32 angle;
    s32 x;
    s32 z;
    s32 dx;
    s32 dz;
    s32 lim;
    s32 vx;
    s32 scale;
    s32 dist;
    u32 frames;

    speed = D_8006995C;
    mode = D_80069960;
    angle = D_80069958;
    if (speed < 0x99 || !(arg0->status_b & 0x80)) {
        mode = 8;
        speed = 0;
    } else {
        arg0->target_rot[1] = func_8001E3DC(arg0->target_rot[1], (D_80060914 + angle) & 0xFFF, arg1[0x10 / 4], 0);
    }
    if (arg0->event_received == 0x3500) {
        x = (arg0->pos[0] - arg2[0]) * 30;
        z = (arg0->pos[2] - arg2[2]) * 30;
        vx = arg0->vel[0];
        dx = x - vx;
        dz = z - arg0->vel[2];
        if (abs(dx) > 1000000 || abs(dz) > 1000000) {
            if ((vx ^ x) >= 0) {
                arg0->vel[0] = x;
            }
            if ((arg0->vel[2] ^ z) >= 0) {
                arg0->vel[2] = z;
            }
        }
    }
    frames = g_DisplayBuffer[0x14 / 4];
    if (frames >= 0x67) {
        frames = 0x66;
    }
    speed = (speed * ((s32)(arg1[0] * frames) / 1024)) >> 8;
    in[0] = ((Math_Sin(angle) >> 4) * speed) >> 8;
    in[1] = 0;
    in[2] = ((Math_Cos(angle) >> 4) * speed) >> 8;
    func_8001EAD8(in, out);
    arg0->vel[0] += out[0];
    arg0->vel[2] += out[2];
    if (mode == 8 || !(arg0->status_a & 0x02000000)) {
        scale = arg1[2];
        if (!(arg0->status_a & 0x02000000)) {
            scale = 0xC7;
        }
        arg0->vel[0] = (arg0->vel[0] * scale) >> 8;
        arg0->vel[2] = (arg0->vel[2] * scale) >> 8;
    }
    lim = arg1[1];
    if (arg0->vel[0] < lim) {
        lim = -lim;
        if (arg0->vel[0] >= lim) {
            lim = arg0->vel[0];
        }
    }
    arg0->vel[0] = lim;
    lim = arg1[1];
    if (arg0->vel[2] < lim) {
        lim = -lim;
        if (arg0->vel[2] >= lim) {
            lim = arg0->vel[2];
        }
    }
    arg0->vel[2] = lim;
    dx = (arg0->pos[0] - arg2[0]) >> 6;
    dz = (arg0->pos[2] - arg2[2]) >> 6;
    dist = func_80041A78(dx * dx + dz * dz) * 0xF00 << 1;
    arg0->unk_104 = dist;
    if (dist < 0x6400) {
        if (mode != 8) {
            arg0->unk_104 = 0x6400;
        }
    } else if (arg1[1] < dist) {
        arg0->unk_104 = arg1[1];
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001CEDC", func_8001D2C4);
#endif
