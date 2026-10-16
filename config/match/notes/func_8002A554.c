#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 n; u8 pad[0x28C]; s32 *p[1]; } Unk8006CB6C;
extern Unk8006CB6C D_8006CB6C;

typedef struct {
    u8 pad0[0x4];
    s32 unk4;
} Unk8002A554_C10;

typedef struct {
    u8 pad0[0x10];
    Unk8002A554_C10 *unk10;
} Unk8002A554_C;

typedef struct {
    u8 pad0[0xC];
    Unk8002A554_C *unkC;
    u8 pad10[0x9C - 0x10];
    u8 unk9C[0xA8 - 0x9C];
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    u8 padB4[0x100 - 0xB4];
    s32 unk100;
} Unk8002A554_Obj;

typedef struct {
    s32 unk0;
    Unk8002A554_Obj *unk4;
    u8 unk8[1];
} Unk8002A554_Entry;

typedef struct {
    u8 pad0[0x40];
    s32 unk40;
    u8 pad44[0x4];
    Unk8002A554_Obj *unk48;
} Unk8002A554_8;

typedef struct {
    u8 pad0[0x8];
    Unk8002A554_8 *unk8;
    u8 padC[0x100 - 0xC];
    s32 unk100;
    u8 pad104[0x4];
    s32 unk108;
} Unk8002A554_Arg0;

void func_8001A710(void *, void *, void *, void *);
s32 func_8002A8D0(void *, void *, s32, s32, s32, s32, void *);

s32 func_8002A554(Unk8002A554_Arg0 *arg0, s32 *arg1, s32 *arg2, u8 *arg3) {
    s32 ret;
    Unk8002A554_Obj *obj;
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
    void *data;

    if (D_8006CB6C.n == 0) {
        return 0xC4653601;
    }
    ret = 0xC4653601;
    best = 0xC4653601;
    diff = arg1[1] - arg2[1];
    limit = arg2[1] + 0xF424;
    if (diff > 0) {
        limit += diff;
    }
    *(s32 *)(arg3 + 0x1020) = arg2[0] - 0x4B00;
    *(s32 *)(arg3 + 0x1024) = arg2[1];
    *(s32 *)(arg3 + 0x1028) = arg2[2] - 0x4B00;
    last = 0;
    *(s32 *)(arg3 + 0x102C) = arg2[0] + 0x4B00;
    y = arg2[1];
    box = (s32 *)(arg3 + 0x1020);
    if (!(arg0->unk100 & 0x40000000)) {
        top = y + arg0->unk8->unk40;
    } else {
        top = y + 0x29900;
    }
    box[4] = top;
    box[5] = arg2[2] + 0x4B00;
    *(s32 *)(arg3 + 0x104C) += 0x10;
    for (i = 0; i < D_8006CB6C.n; i++) {
        entry = (Unk8002A554_Entry *)D_8006CB6C.p[i];
        obj = entry->unk4;
        if ((obj->unkAC & 0x400000) == 1) {
            lim = 0xC4653601;
        } else {
            lim = limit;
        }
        res = func_8002A8D0(entry, box, 1, 1, best, lim,
                            (obj->unkAC & 0x200) ? obj->unk9C : 0);
        if (res != 0xC4653601) {
            if ((obj->unkAC & 0x20000) &&
                ((((!(arg0->unk100 & 0x10) && arg0->unk108 != 5) ||
                    ((obj->unkC->unk10->unk4 != 0x300 || (obj->unkB0 & 0x1012)) && !(obj->unk100 & 0x20))) &&
                   !(arg0->unk100 & 0x10000)) ||
                  (obj->unk100 & 0x10000))) {
                s32 flags = arg0->unk100;
                s32 mode = flags & 0xA00000;
                if (mode != 0x200000 || (obj->unkB0 & mode) ||
                    ((flags & 8) ? obj->unk100 < 0 : (obj->unk100 & 0x200))) {
                    obj->unkA8 |= 0x4000;
                    ret = res + 1;
                    best = res;
                    arg0->unk8->unk48 = obj;
                }
            }
            last = entry;
            *(s32 *)(arg3 + 0x104C) += 0x1000;
        }
    }
    if (last != 0) {
        data = 0;
        if (!(last->unk0 & 7)) {
            data = last->unk8;
        }
        func_8001A710(arg0, box, last->unk4, data);
    }
    return ret;
}
