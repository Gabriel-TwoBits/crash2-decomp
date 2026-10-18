#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
extern s8 D_8005B994;
extern s8 D_8005B99B;
extern s32 D_8005BA14;
extern s32 D_8005BA18;
extern s32 D_8005BA1C;
extern s32 D_8005BA20;
extern s32 D_8005BA24;
extern s32 D_8005BAD8;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_8005BAE4;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 *D_800608D4;
extern s32 D_800608E4;
extern s32 D_800608F8;
extern s32 D_800608FC;
extern s32 D_80060900;
extern s32 D_80060904;

typedef struct { s8 a; s8 b; s8 c; } Arg;
typedef struct { u8 a; u8 b; u8 c; u8 d; } Ent;

void func_8002648C(Arg *arg0, s32 arg1) {
    Ent *e;
    s32 *p;
    s32 *v;
    s32 s;
    s32 a2;

    switch ((s8)(arg0->c - 1)) {
    case 3:
        if (D_800608FC != arg0->b) {
            func_80022224(arg0->b);
            func_80023154();
        }
        break;
    case 1:
        if (arg1 == 1) {
            D_80060900 = (u8)arg0->b;
        } else {
            D_80060904 = (u8)arg0->b;
        }
        break;
    case 5:
        D_8005B99B = 1;
        break;
    case 0:
        if (D_8005B994 != 7) {
            D_8005B994 = 7;
            goto common;
        }
        break;
    case 2:
        if (D_8005B994 != 6) {
            D_8005B994 = 6;
    common:
        a2 = 0;
        if (arg1 != 1) a2 = D_800608E4;
        e = (Ent *)((u8 *)Ent_GetProperty((s32)D_800608D4, 0x109, a2, 0, 0, 1, 0, 0, 0) + arg0->b * 4);
        p = (s32 *)(((s32 *)D_800608D4[1])[4] + (e->c * 4 + 0x194));
        v = (s32 *)func_80014260(p, 1, 0);
        s = *(s32 *)((u8 *)v + (e->b * 3 + *(s32 *)(v[4] + 0x184)) * 4 + 0x10);
        D_8005BAD8 = *p;
        D_8005BADC = e->b;
        if (e->a == 1) {
            D_8005BAE0 = 0;
        } else {
            D_8005BAE0 = (Ent_GetPropertyRowLength(s, 0x4B, 0, 2) - 1) << 8;
        }
        D_8005BAE4 = arg1;
        if (arg0->c == 3) {
            if (arg0->a != 0) {
                D_8005BA18 = 0x10;
                D_8005BA14 = 0x10;
                D_8005BA1C = D_800607F4;
                D_8005BA20 = D_800607F8;
                D_8005BA24 = D_800607FC;
            }
        }
        if (arg0->c == 1) {
            func_80023084(s, D_8005BAE0);
        }
        }
        break;
    case 4:
        if (D_8005B994 != 9) {
            D_8005B994 = 9;
            D_800608F8 = arg1;
        }
        break;
    default:
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002648C", func_8002648C);
#endif

typedef struct { s32 a, b, c, d; } V;
typedef struct { V v; s32 pad[20]; } Buf;
s32 func_800267C4(Buf *, s32);

s32 func_80026764(s32 arg0, V *arg1) {
    Buf buf;

    if (func_800267C4(&buf, arg0) != 0) {
        *arg1 = buf.v;
        return 1;
    }
    return 0;
}

#ifdef NON_MATCHING
typedef struct { s32 *obj; s32 id; s32 pad; s32 mode; } Unk800267C4;

#define MODE_DIRECT 1
#define SLOT_TABLE_OFFSET 0x194
#define ENTRY_BYTES 4

s32 func_800267C4(Buf *out, s32 argAddr) {
    Unk800267C4 *arg = (Unk800267C4 *)argAddr;
    u8 *list;
    s32 count;
    s32 mask;
    s32 bit;
    s32 n;
    s32 i;
    u8 *e;
    s32 *obj;
    s32 b;
    V *cur;

    if (arg->mode == MODE_DIRECT) {
        list = (u8 *)Ent_GetProperty(arg->id, 0x109, 0, 0, 0, 1, 0, 0, 0);
        if (list != 0) {
            count = Ent_GetPropertyRowLength(arg->id, 0x109, 0, 1);
            mask = D_80060900;
        } else {
            count = 0;
        }
    } else {
        list = (u8 *)Ent_GetProperty(arg->id, 0x109, (Ent_GetPropertyRowLength(arg->id, 0x4B, 0, 2) - 1) << 8, 0, 0, 1, 0, 0, 0);
        if (list != 0) {
            count = Ent_GetPropertyRowLength(arg->id, 0x109, (Ent_GetPropertyRowLength(arg->id, 0x4B, 0, 2) - 1) << 8, 1);
            mask = D_80060904;
        } else {
            count = 0;
        }
    }
    cur = &out->v;
    if (count == 0) {
        return 0;
    }
    bit = 1;
    n = 0;
    for (i = 0; i < count; i++) {
        if (mask & bit) {
            e = list + 1;
            obj = func_80014260((u8 *)arg->obj[4] + (e[1] * ENTRY_BYTES + SLOT_TABLE_OFFSET), 1, 0);
            b = ((s32 *)((u8 *)obj + ((e[0] * 3 + ((s32 *)obj[4])[0x184 / 4]) * 4)))[4];
            cur->a = (s32)obj;
            cur->b = b;
            if (*list == 1) {
                cur->d = 2;
                cur->c = 0;
            } else {
                cur->d = 1;
                cur->c = (Ent_GetPropertyRowLength(b, 0x4B, 0, 2) - 1) << 8;
            }
            cur++;
            n++;
        }
        list += ENTRY_BYTES;
        bit *= 2;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002648C", func_800267C4);
#endif

#ifdef NON_MATCHING
extern s32 D_800608D8;

void func_80026A08(s32 *arg0, s32 arg1, s32 *arg2, s32 *arg3) {
    s32 t[10];
    s32 *base;
    s16 *a;
    s16 *b;
    s32 frac;
    s32 *new_var;
    s32 idx;
    s32 v;

    base = *(s32 **)(*(s32 *)(arg0 + 1) + 0x14);
    a =(s16 *)Ent_GetProperty((s32)arg0, 0x4B, 0, idx = arg1 >> 8, 0, 2, 0, 0, 0);
    new_var = &D_800608D8;
    b = (s16 *)Ent_GetProperty(*new_var,0x4B, 0, idx * 2, 0, 2, 0, 0, 0);
    arg2[0] = (base[0] + a[0]) << 8;
    arg2[1] = (base[1] + a[1]) << 8;
    arg2[2] = (base[2] + a[2]) << 8;
    arg2[3] = b[0];
    arg2[4] = b[1];
    arg2[5] = b[2];
    if (arg3 != 0) {
        arg3[0] = b[3];
        arg3[1] = b[4];
        arg3[2] = b[5];
    }
    frac = arg1 & 0xFF;
    if (frac != 0 && idx + 1 < Ent_GetPropertyRowLength((s32)arg0, 0x4B, 0, 2)) {
        t[0] = (base[0] + a[3]) << 8;
        t[1] = (base[1] + a[4]) << 8;
        t[2] = (base[2] + a[5]) << 8;
        t[3] = b[6];
        t[4] = b[7];
        t[5] = b[8];
        arg2[0] += ((t[0] - arg2[0]) * frac) >> 8;
        arg2[1] += ((t[1] - arg2[1]) * frac) >> 8;
        arg2[2] += ((t[2] - arg2[2]) * frac) >> 8;
        arg2[3] = func_800312CC(arg2[3], t[3], frac);
        arg2[4] = func_800312CC(arg2[4], t[4], frac);
        arg2[5] = func_800312CC(arg2[5], t[5], frac);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002648C", func_80026A08);
#endif
