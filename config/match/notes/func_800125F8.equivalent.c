#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_SIZE 0x2C
#define STATE_BUSY 0x1E
#define STATE_READY 0x14
#define STATE_QUEUED 1
#define NO_FREE_SLOT -0xC
#define HASH_TO_TABLE_SHIFT 13
#define HASH_TABLE_MASK 0x3FC
#define PAGE_X_SHIFT 8
#define PAGE_Y_SHIFT 7
#define DEFAULT_HASH 0x6396347F
#define LINK_TAG 0xC0000002
#define OBJ_TYPE_ALT 4

typedef struct Unk800125F8Slot {
    s32 unk0;
    u32 unk4;
} Unk800125F8Slot;

typedef struct Unk800125F8Child {
    u32 unk_0;
    u32 unk_4;
    s32 unk_8;
    s32 unk_C;
    s32 unk_10;
} Unk800125F8Child;

typedef struct Unk800125F8Obj {
    u16 unk_0;
    u16 unk_2;
    u32 unk_4;
    u32 unk_8;
    s32 unk_C;
    Unk800125F8Child *unk_10[1];
} Unk800125F8Obj;

typedef struct Unk800125F8Entry {
    Unk800125F8Obj *unk_0;
    s16 unk_4;
    u8 unk_6[6];
    s16 unk_C;
    u8 unk_E;
    u8 unk_F;
    u8 unk_10[4];
    s32 unk_14;
    u32 unk_18;
    s32 unk_1C;
    Unk800125F8Slot *unk_20;
    struct Unk800125F8Entry *unk_24;
    u8 unk_28[4];
} Unk800125F8Entry;

typedef struct Unk800125F8Handler {
    void *fn;
    u8 unk_4[0x18];
} Unk800125F8Handler;

typedef void (*Unk800125F8Fn2)(Unk800125F8Child *, Unk800125F8Child *);
typedef void (*Unk800125F8Fn3)(Unk800125F8Child *, Unk800125F8Child *, s32);

struct Unk80012A20Obj;
s32 func_80012A20(struct Unk80012A20Obj *arg0);
extern int D_80067FB4[];
extern s32 *D_80067FC4;
extern struct Unk80014B44 **D_80067590;
extern Unk800125F8Handler D_8005B1E8[];
extern s32 D_80060150[];
extern Unk800125F8Entry D_80062D6C[];
extern s32 D_800675B4;

s32 func_800125F8(s32 arg0) {
    Unk800125F8Entry *entry;
    Unk800125F8Entry *slot;
    Unk800125F8Entry *next;
    Unk800125F8Obj *obj;
    Unk800125F8Child *child;
    Unk800125F8Slot *dst;
    Unk800125F8Handler *handler;
    u32 i;
    s32 id;
    s32 page;
    s32 offset;
    s32 hash;

    entry = (Unk800125F8Entry *)((u8 *)&D_800675B4 + arg0 * ENTRY_SIZE);
    if (D_80067FB4[0] == (s32)entry) {
        D_80067FB4[0] = 0;
    }
    entry->unk_4 = STATE_BUSY;
    switch (entry->unk_0->unk_2) {
    case 0:
    case 5:
        obj = entry->unk_0;
        dst = *(Unk800125F8Slot **)((u8 *)D_80067590 +
                                    ((obj->unk_10[0]->unk_4 >> HASH_TO_TABLE_SHIFT) & HASH_TABLE_MASK));
        if (obj->unk_8 != 0) {
            i = 0;
            do {
                child = entry->unk_0->unk_10[i];
                hash = child->unk_4;
                while (dst->unk4 != hash) {
                    dst++;
                }
                dst->unk0 = (s32)child;
                entry->unk_F++;
                handler = &D_8005B1E8[child->unk_8];
                if (handler->fn != 0) {
                    ((Unk800125F8Fn2)handler->fn)(child, child);
                }
                entry->unk_F--;
                i++;
            } while (i < entry->unk_0->unk_8);
        }
        entry->unk_4 = STATE_READY;
        return (s32)entry->unk_0;
    case 1:
        id = func_80013888();
        if (id == NO_FREE_SLOT) {
            func_80012A20((struct Unk80012A20Obj *)entry);
            return STATE_BUSY;
        }
        slot = &D_80062D6C[id];
        page = id >> 2;
        D_8005F1B0.x = (id - page * 4) << PAGE_X_SHIFT;
        D_8005F1B0.y = page << PAGE_Y_SHIFT;
        LoadImage(&D_8005F1B0, entry->unk_0);
        slot->unk_4 = STATE_READY;
        slot->unk_18 = entry->unk_0->unk_4;
        slot->unk_14 = entry->unk_14;
        slot->unk_20 = (Unk800125F8Slot *)func_80014B44(slot->unk_18);
        slot->unk_F = entry->unk_F;
        slot->unk_E = entry->unk_E;
        slot->unk_20->unk0 = slot->unk_1C;
        entry->unk_4 = STATE_QUEUED;
        entry->unk_F = 0;
        D_80067FC4[entry->unk_14 >> 1] = (s32)slot;
        func_800170B4(entry->unk_0->unk_4, 1);
        return id * 2;
    case 3:
    case 4:
        obj = entry->unk_0;
        next = entry->unk_24;
        dst = *(Unk800125F8Slot **)((u8 *)D_80067590 +
                                    ((obj->unk_10[0]->unk_4 >> HASH_TO_TABLE_SHIFT) & HASH_TABLE_MASK));
        if (obj->unk_8 != 0) {
            i = 0;
            do {
                child = entry->unk_0->unk_10[i];
                hash = child->unk_4;
                offset = child->unk_10;
                while (dst->unk4 != hash) {
                    dst++;
                }
                if (i == 0) {
                    if (entry->unk_0->unk_2 == OBJ_TYPE_ALT) {
                        D_80060150[next->unk_C] = dst->unk4;
                    } else {
                        D_80060150[next->unk_C] = DEFAULT_HASH;
                    }
                }
                dst->unk0 = (((offset - (s32)entry->unk_0) + next->unk_1C) * 4) | LINK_TAG;
                handler = &D_8005B1E8[child->unk_8];
                if (handler->fn != 0) {
                    ((Unk800125F8Fn3)handler->fn)(child, child, offset);
                }
                i++;
            } while (i < entry->unk_0->unk_8);
        }
        next->unk_4 = STATE_READY;
        entry->unk_4 = STATE_QUEUED;
        entry->unk_F = 0;
        D_80067FC4[entry->unk_14 >> 1] = (s32)next;
        return next->unk_1C;
    default:
        entry->unk_4 = STATE_READY;
        return (s32)entry->unk_0;
    }
}
