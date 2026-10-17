#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATE_IDLE 1
#define STATE_QUEUED_FIRST 0x14
#define STATE_QUEUED_COUNT 2
#define KIND_A 1
#define KIND_B_FIRST 3
#define KIND_B_COUNT 2
#define SLOT_FREED -0x12
#define RESULT_DONE -0xFF
#define KEY_TABLE_SHIFT 15
#define KEY_TABLE_MASK 0xFF

extern int D_80067FB4[];
extern s32 *D_80067FC4;
extern struct Unk80014B44 **D_80067590;
extern struct { void (*fn)(void *); s32 pad[6]; } D_8005B1EC[];

typedef struct Unk80012A20Item { s32 unk_0; u32 unk_4; s32 unk_8; } Unk80012A20Item;
typedef struct Unk80012A20Hdr { u16 unk_0; u16 unk_2; s32 unk_4; u32 unk_8; s32 unk_C; Unk80012A20Item *unk_10[1]; } Unk80012A20Hdr;
typedef struct Unk80012A20Obj { Unk80012A20Hdr *unk_0; u16 unk_4; u8 unk_6[8]; u8 unk_E; u8 unk_F; s32 unk_10; s32 unk_14; } Unk80012A20Obj;

s32 func_80012A20(Unk80012A20Obj *arg0) {
    Unk80012A20Hdr *h;
    s32 *s1;
    s32 *s0;
    u32 i;
    s32 idx;
    Unk80012A20Item *it;
    s32 key;

    if (D_80067FB4[0] == (int)arg0) {
        D_80067FB4[0] = 0;
    }
    if ((u16)(arg0->unk_4 - STATE_QUEUED_FIRST) < STATE_QUEUED_COUNT) {
        h = arg0->unk_0;
        if (!(h->unk_2 == KIND_A || (u16)(h->unk_2 - KIND_B_FIRST) < KIND_B_COUNT)) {
            goto loop;
        }
        D_80067FC4[arg0->unk_14 >> 1] = SLOT_FREED;
    }
    arg0->unk_4 = STATE_IDLE;
    arg0->unk_F = 0;
    arg0->unk_E = 0;
    return RESULT_DONE;
loop:
    idx = arg0->unk_14;
    s1 = (s32 *)((s32 **)D_80067590)[(h->unk_10[0]->unk_4 >> KEY_TABLE_SHIFT) & KEY_TABLE_MASK];
    i = 0;
    if (h->unk_8 != 0) {
        s0 = s1 + 1;
        do {
            it = arg0->unk_0->unk_10[i];
            key = it->unk_4;
            if (*s0 != key) {
                do {
                    s0 += 2;
                    s1 += 2;
                } while (*s0 != key);
            }
            if (D_8005B1EC[it->unk_8].fn != 0) {
                D_8005B1EC[it->unk_8].fn(it);
            }
            *s1 = idx;
            i++;
        } while (i < arg0->unk_0->unk_8);
    }
    func_80013F28(arg0);
    return RESULT_DONE;
}
