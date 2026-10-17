#include "common.h"
#include "sdk.h"
#include "game.h"

#define PTR_NONE ((void *)0)
#define OBJ_TYPE_ENTITY 7
#define OBJ_TYPE_PLAYER 0x11
#define PROP_KIND 0xA9
#define PROP_SLOT 0xAA
#define PROP_INDEX 0x9F
#define PROP_POSITION 0x4B
#define PROP_PARAMS 0xA4
#define PROP_ARG_MODE 2
#define KIND_CHAIN 0x22
#define SLOT_FLAG_USED 1
#define SLOT_FLAG_RESERVED 2
#define INDEX_MIN_EXCL 0
#define INDEX_MAX_EXCL 5
#define ERR_NO_OBJECT (-0xE)
#define ERR_NO_SLOT (-0x16)
#define ERR_THRESHOLD (-0xFE)
#define CHAIN_STEP 0x19000
#define POS_TOLERANCE 10
#define POS_Y_OFFSET 0x64
#define SLOT_COUNT 8
#define SLOT_SIZE 8
#define SLOT_FIRST_FREE 3
#define SLOT_FIRST_PLAYER 6
#define SLOT_LIST_OFFSET 0x40
#define PARAMS_NO_CHAIN 0x80000
#define MODEL_ID_SPECIAL 0x300
#define CLEAR_SIZE 0x20
#define FIXED_SHIFT 8
#define ENTITY_POS_ROW_SIZE 0x18
#define SLOT_STATE_ACTIVE 3
#define LIST_TYPE_FREE 2

s32 func_8001F054(struct Obj *, u32, s32 *);
extern s32 D_8005F38C;
extern s32 D_8006B1C4[];
extern s32 *D_8006CB10[];
extern s32 D_8006CB58[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;
typedef struct {
    u8 pad[0x190];
    s32 count;
    s32 items[1];
} Unk19E5C;

typedef struct {
    u8 pad[0x10];
    Unk19E5C *unk10;
} Unk608CC;

/* volatile: the retail code reloads it after the null check. */
extern Unk608CC *volatile D_800608CC;
extern s32 D_8005F3F0;
extern u8 D_80060958;
extern u8 *D_8006759C;
extern u8 D_8006CB18;
extern s32 D_8006CB38;
extern s32 D_8006CC84;
extern s32 D_8006CD7C;
extern s32 *D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;
extern char func_80019074[];

#define PROP(o, id) func_800321D4(o, id, 0, 0, 0, PROP_ARG_MODE, 0)
#define LOOKUP(o, id, out) func_80031AE8(o, id, 0, 0, out, PROP_ARG_MODE, PTR_NONE, 0, 0)
/* The value of type T at byte offset off of p. */
#define AT(p, off, T) (*(T *)((u8 *)(p) + (off)))

s32 func_80019098(void *arg0, u8 arg1) {
    s32 obj;
    s32 sp28;
    s32 *slot;
    s32 *prev;
    s32 *a;
    s32 *b;
    s32 r;
    s32 idx;
    s32 i;
    s32 first;
    u8 *p;
    u8 *row;
    u8 *e;
    s32 *entry;

    if (arg0 == PTR_NONE) {
        return ERR_NO_OBJECT;
    }
    prev = PTR_NONE;
    /* For any other type the retail code uses whatever the caller left in $s2. */
    if (AT(arg0, 8, s32) == OBJ_TYPE_ENTITY) {
        e = AT(arg0, 0x10, u8 *);
        obj = AT(arg0, 0x10 + (arg1 + AT(e, 0x184, s32) + AT(e, 0x188, s32)) * 4, s32);
    }
    if (PROP(obj, PROP_KIND) == 0 || (PROP(obj, PROP_INDEX) > INDEX_MIN_EXCL && PROP(obj, PROP_INDEX) < INDEX_MAX_EXCL)) {
        if (D_8005F38C != 0) {
            return ERR_NO_OBJECT;
        }
    }
    if (PROP(obj, PROP_KIND) == KIND_CHAIN) {
        if (D_8006CDF8 != 0) {
            a = (s32 *)LOOKUP(obj, PROP_POSITION, PTR_NONE);
            b = (s32 *)LOOKUP(D_8006CDF8, PROP_POSITION, PTR_NONE);
            if (D_8006CDF8 != 0
                && func_80031668(((s16 *)a)[0], ((s16 *)b)[0]) < POS_TOLERANCE
                && func_80031668(((s16 *)a)[2], ((s16 *)b)[2]) < POS_TOLERANCE
                && func_80031668(((s16 *)a)[1], ((s16 *)b)[1] + POS_Y_OFFSET) < POS_TOLERANCE) {
                prev = D_8006CDF0;
            } else {
                prev = PTR_NONE;
                D_8006CDF4 = CHAIN_STEP;
                D_8006CDF0 = PTR_NONE;
            }
        } else {
            prev = PTR_NONE;
            D_8006CDF4 = CHAIN_STEP;
            D_8006CDF0 = PTR_NONE;
        }
        D_8006CDF8 = obj;
    } else {
        /* not a chain object */
    }
    PROP(obj, PROP_INDEX);
    idx = PROP(obj, PROP_INDEX);
    first = D_8006B1C4[idx] & SLOT_FLAG_USED;
    if (first == 0) {
        PROP(obj, PROP_INDEX);
        idx = PROP(obj, PROP_INDEX);
        first = D_8006B1C4[idx] & SLOT_FLAG_RESERVED;
    }
    if (first != 0) {
        if (PROP(obj, PROP_KIND) == KIND_CHAIN) {
            D_8006CDF4 += CHAIN_STEP;
        }
        return ERR_NO_OBJECT;
    }

    if (PROP(obj, PROP_KIND) == 0 || (PROP(obj, PROP_INDEX) > INDEX_MIN_EXCL && PROP(obj, PROP_INDEX) < INDEX_MAX_EXCL)) {
        slot = D_8006CB10[0];
        D_8005F38C = (s32)slot;
        i = SLOT_FIRST_PLAYER;
    } else {
        while (1) {
            if (D_8006CB58[0] == LIST_TYPE_FREE) {
                slot = (s32 *)D_8006CB5C;
            } else {
                slot = (s32 *)D_8006CBA4;
            }
            i = SLOT_FIRST_FREE;
            if (slot != PTR_NONE) {
                break;
            }
            p = &D_8006CB18;
            for (idx = 0; idx < SLOT_COUNT; idx++) {
                r = func_80018BE4(p, func_80019074, PARAMS_NO_CHAIN);
                if (r != 0) {
                    break;
                }
                p += SLOT_SIZE;
            }
            if (idx >= SLOT_COUNT) {
                return ERR_NO_SLOT;
            }
            func_80019E08(r);
        }
    }
    func_800189CC(slot, (s32 *)(&D_8006CB18 + i * SLOT_SIZE));
    slot[1] = SLOT_STATE_ACTIVE;
    entry = (s32 *)LOOKUP(obj, PROP_PARAMS, &sp28);
    if (entry == PTR_NONE) {
        sp28 = 0;
    }
    r = func_80019988(slot, PROP(obj, PROP_KIND), PROP(obj, PROP_SLOT), sp28, (s32)entry);
    if (r >= ERR_THRESHOLD) {
        func_800189CC(slot, (s32 *)(&D_8006CB18 + SLOT_LIST_OFFSET));
        return r;
    }
    /* The flag goes at the first call's index; the second call's result is unused. */
    idx = PROP(obj, PROP_INDEX);
    PROP(obj, PROP_INDEX);
    D_8006B1C4[idx] |= SLOT_FLAG_USED;
    slot[0xB8 / 4] = PROP(obj, PROP_INDEX) << FIXED_SHIFT;
    if (AT(arg0, 8, s32) == OBJ_TYPE_PLAYER) {
        arg0 = (void *)D_800608CC;
    }
    AT(slot, 0x18, void *) = arg0;
    memset(slot + 0x20 / 4, 0, CLEAR_SIZE);
    AT(slot, 0xF0, s32) = obj;
    AT(slot, 0xF8, s32) = func_800322C0(obj, PROP_POSITION, 0, PROP_ARG_MODE) << FIXED_SHIFT;
    func_8001F054((struct Obj *)slot, 0, slot + 0x60 / 4);
    if ((s32)slot == D_8005F38C) {
        row = D_8006759C + D_8006CD7C * ENTITY_POS_ROW_SIZE;
        AT(slot, 0x60, s32) = AT(row, 0x1E8, s32);
        AT(slot, 0x64, s32) = AT(row, 0x1EC, s32);
        AT(slot, 0x68, s32) = AT(row, 0x1F0, s32);
        if ((s32)slot == D_8005F38C && D_8005F3F0 == -1) {
            D_8006CC84 = D_8005F3F0;
            func_8002F34C((u8 *)slot, &D_80060958, 1, 0);
        }
    }
    if (AT(AT(AT(slot, 0xC, u8 *), 0x10, u8 *), 4, s32) == MODEL_ID_SPECIAL) {
        func_800189CC(slot, &D_8006CB38);
    }
    if (AT(AT(AT(slot, 0xC, u8 *), 0x10, u8 *), 0, s32) == KIND_CHAIN) {
        if (entry == PTR_NONE || (AT(entry, 4, s32) & PARAMS_NO_CHAIN) == 0) {
            if (prev != PTR_NONE) {
                AT(slot, 0x84, s32 *) = prev;
                AT(slot, 0x88, s32) = 0;
                AT(prev, 0x88, s32 *) = slot;
            } else {
                AT(slot, 0x84, s32 *) = PTR_NONE;
                AT(slot, 0x88, s32) = 0;
            }
            AT(slot, 0x64, s32) = AT(slot, 0x64, s32) + CHAIN_STEP - D_8006CDF4;
        } else {
            /* chain link suppressed by the params */
        }
        D_8006CDF0 = slot;
    } else {
        /* not a chain object */
    }
    return (s32)slot;
}
