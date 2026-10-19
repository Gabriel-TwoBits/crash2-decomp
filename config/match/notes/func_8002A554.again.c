#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 n; u8 pad[0x28C]; s32 *p[1]; } Unk8006CB6C;
#define NO_HIT 0xC4653601 /* func_8002A8D0's "nothing hit" result */
#define BOX_HALF_EXTENT 0x4B00 /* half-width of the query box in x and z */

extern Unk8006CB6C D_8006CB6C;

/* The first item of GoolObject.exe, as this function reads it */
typedef struct {
    u8 pad0[0x4];
    s32 unk4;
} Unk8002A554_C10;

#define KIND(obj) (((Unk8002A554_C10 *)(obj)->exe->items[0])->unk4)

typedef struct {
    s32 unk0;
    GoolObject *unk4;
    u8 unk8[1];
} Unk8002A554_Entry;

/* What GoolObject.unk_8 points to, as this function reads it */
typedef struct {
    u8 pad0[0x40];
    s32 unk40;
    u8 pad44[0x4];
    GoolObject *unk48;
} Unk8002A554_8;


s32 func_8002A554(GoolObject *arg0, s32 *arg1, s32 *arg2, u8 *arg3) {
    s32 ret;
    GoolObject *obj;
    s32 i;
    Unk8002A554_Entry *entry;
    Unk8002A554_Entry *last;
    s32 *box;
    s32 limit;
    s32 best;
    s32 diff;
    s32 y;
    s32 top;
    s32 lim;
    s32 res;
    s32 *data;
    s32 state;

    if (D_8006CB6C.n == 0) {
        return NO_HIT;
    }
    ret = NO_HIT;
    best = NO_HIT;
    diff = arg1[1] - arg2[1];
    limit = arg2[1] + 0xF424;
    if (diff > 0) {
        limit += diff;
    }
    *(s32 *)(arg3 + 0x1020) = arg2[0] - BOX_HALF_EXTENT;
    *(s32 *)(arg3 + 0x1024) = arg2[1];
    *(s32 *)(arg3 + 0x1028) = arg2[2] - BOX_HALF_EXTENT;
    last = 0;
    *(s32 *)(arg3 + 0x102C) = arg2[0] + BOX_HALF_EXTENT;
    state = arg0->state_flags;
    y = arg2[1];
    box = (s32 *)(arg3 + 0x1020);
    if (!(state & 0x40000000)) {
        top = y + ((Unk8002A554_8 *)arg0->unk_8)->unk40;
    } else {
        top = y + 0x29900;
    }
    box[4] = top;
    box[5] = arg2[2] + BOX_HALF_EXTENT;
    *(s32 *)(arg3 + 0x104C) += 0x10;
    for (i = 0; i < D_8006CB6C.n; i++) {
        entry = (Unk8002A554_Entry *)D_8006CB6C.p[i];
        obj = entry->unk4;
        if ((obj->status_b & 0x400000) == 1) {
            lim = NO_HIT;
        } else {
            lim = limit;
        }
        res = func_8002A8D0(entry, box, 1, 1, best, lim,
                            (obj->status_b & 0x200) ? (s32)obj->unk_9C : 0);
        if (res != NO_HIT) {
            if ((obj->status_b & 0x20000) &&
                ((((!(arg0->state_flags & 0x10) && arg0->display_mode != 5) ||
                    ((KIND(obj) != 0x300 || (obj->block_flags & 0x1012)) && !(obj->state_flags & 0x20))) &&
                   !(arg0->state_flags & 0x10000)) ||
                  (obj->state_flags & 0x10000))) {
                s32 flags = arg0->state_flags;
                s32 mode = flags & 0xA00000;
                if (mode != 0x200000 || (obj->block_flags & mode) ||
                    ((flags & 8) ? obj->state_flags < 0 : (obj->state_flags & 0x200))) {
                    obj->status_a |= 0x4000;
                    ret = res + 1;
                    best = res;
                    ((Unk8002A554_8 *)arg0->unk_8)->unk48 = obj;
                }
            }
            last = entry;
            *(s32 *)(arg3 + 0x104C) += 0x1000;
        }
    }
    if (last != 0) {
        data = 0;
        if (!(last->unk0 & 7)) {
            data = (s32 *)last->unk8;
        }
        func_8001A710(arg0, box, last->unk4, data);
    }
    return ret;
}
