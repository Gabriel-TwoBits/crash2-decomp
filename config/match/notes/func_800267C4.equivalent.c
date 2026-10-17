#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 *obj; s32 id; s32 pad; s32 mode; } Unk800267C4;
extern s32 D_80060900;
extern s32 D_80060904;

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
        list = (u8 *)func_80031AE8(arg->id, 0x109, 0, 0, 0, 1, 0, 0, 0);
        if (list != 0) {
            count = func_800322C0(arg->id, 0x109, 0, 1);
            mask = D_80060900;
        } else {
            count = 0;
        }
    } else {
        list = (u8 *)func_80031AE8(arg->id, 0x109, (func_800322C0(arg->id, 0x4B, 0, 2) - 1) << 8, 0, 0, 1, 0, 0, 0);
        if (list != 0) {
            count = func_800322C0(arg->id, 0x109, (func_800322C0(arg->id, 0x4B, 0, 2) - 1) << 8, 1);
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
                cur->c = (func_800322C0(b, 0x4B, 0, 2) - 1) << 8;
            }
            cur++;
            n++;
        }
        list += ENTRY_BYTES;
        bit *= 2;
    }
    return n;
}
