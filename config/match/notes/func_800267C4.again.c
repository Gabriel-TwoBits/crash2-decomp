#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 a, b, c, d; } V;
typedef struct { s32 *obj; s32 id; s32 pad; s32 mode; } Arg;
extern s32 D_80060900;
extern s32 D_80060904;

s32 func_800267C4(V *out, Arg *arg) {
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
    u8 *p;

    if (arg->mode == 1) {
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
    cur = out;
    if (count == 0) {
        return 0;
    }
    bit = 1;
    i = 0;
    n = 0;
    for (; i < count; i++) {
        if (mask & bit) {
            e = list + 1;
            p = (u8 *)((u32)arg->obj[4] + (u32)(e[1] * 4 + 0x194));
            obj = Nsf_OpenEntry(p, 1, 0);
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
        list += 4;
        bit *= 2;
    }
    return n;
}
