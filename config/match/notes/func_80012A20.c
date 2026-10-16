#include "common.h"
#include "sdk.h"
#include "game.h"

extern void *D_80067FB4[];
extern s32 *D_80067FC4;
extern s32 **D_80067590;
extern struct { void (*fn)(void *); s32 pad[6]; } D_8005B1EC[];

typedef struct Sub { s32 a; u32 key; s32 type; } Sub;
typedef struct Hdr { u16 a; u16 kind; s32 c; u32 count; s32 d; Sub *items[1]; } Hdr;
typedef struct Obj { Hdr *hdr; u16 state; u8 pad[8]; u8 c; u8 d; s32 f; s32 idx; } Obj;

s32 func_80012A20(Obj *arg0) {
    Hdr *h;
    s32 *s1;
    s32 *s0;
    u32 i;
    s32 idx;
    Sub *it;
    s32 key;
    s32 pad[2];

    if (D_80067FB4[0] == arg0) {
        D_80067FB4[0] = 0;
    }
    if ((u16)(arg0->state - 0x14) < 2) {
        h = arg0->hdr;
        if (!(h->kind == 1 || (u16)(h->kind - 3) < 2)) {
            goto loop;
        }
        D_80067FC4[arg0->idx >> 1] = -0x12;
    }
    arg0->state = 1;
    arg0->d = 0;
    arg0->c = 0;
    return -0xFF;
loop:
    idx = arg0->idx;
    s1 = D_80067590[(h->items[0]->key >> 15) & 0xFF];
    i = 0;
    if (h->count != 0) {
        s0 = s1 + 1;
        do {
            it = arg0->hdr->items[i];
            key = it->key;
            if (*s0 != key) {
                do {
                    s0 += 2;
                    s1 += 2;
                } while (*s0 != key);
            }
            if (D_8005B1EC[it->type].fn != 0) {
                D_8005B1EC[it->type].fn(it);
            }
            *s1 = idx;
            i++;
        } while (i < arg0->hdr->count);
    }
    func_80013F28(arg0);
    return -0xFF;
}
