#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad[0x190];
    s32 count;
    s32 items[1];
} Unk19E5C;
typedef struct {
    u8 pad[0x10];
    Unk19E5C *unk10;
} Unk608CC;

#ifdef NON_MATCHING
#define PTR_NONE ((void *)0)
#define OBJ_TYPE_ENTITY 7
#define OBJ_TYPE_PLAYER 0x11
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

s32 func_8001F054(GoolObject *, u32, s32 *);
extern s32 g_Player;
extern s32 g_EntityFlags[];
extern s32 *g_PlayerSlot[];
extern s32 g_ObjFreeList[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;


/* volatile: the retail code reloads it after the null check. */
extern Unk608CC *volatile g_CurrentZone;
extern s32 g_LevelRequest;
extern u8 D_80060958;
extern u8 *D_8006759C;
extern u8 g_ObjLists;
extern s32 D_8006CB38;
extern s32 D_8006CC84;
extern s32 D_8006CD7C;
extern s32 *D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;
extern char Obj_HasStateFlags[];

#define PROP(o, id) Ent_GetPropertyValue(o, id, 0, 0, 0, ENT_ROW_INDEX, 0)
#define LOOKUP(o, id, out) Ent_GetProperty(o, id, 0, 0, out, ENT_ROW_INDEX, PTR_NONE, 0, 0)
/* The value of type T at byte offset off of p. */
#define AT(p, off, T) (*(T *)((u8 *)(p) + (off)))

s32 Obj_SpawnEntity(void *arg0, u8 arg1) {
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
    if (PROP(obj, ENT_PROP_TYPE) == 0 || (PROP(obj, ENT_PROP_ID) > INDEX_MIN_EXCL && PROP(obj, ENT_PROP_ID) < INDEX_MAX_EXCL)) {
        if (g_Player != 0) {
            return ERR_NO_OBJECT;
        }
    }
    if (PROP(obj, ENT_PROP_TYPE) == KIND_CHAIN) {
        if (D_8006CDF8 != 0) {
            a = (s32 *)LOOKUP(obj, ENT_PROP_POSITIONS, PTR_NONE);
            b = (s32 *)LOOKUP(D_8006CDF8, ENT_PROP_POSITIONS, PTR_NONE);
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
    PROP(obj, ENT_PROP_ID);
    idx = PROP(obj, ENT_PROP_ID);
    first = g_EntityFlags[idx] & SLOT_FLAG_USED;
    if (first == 0) {
        PROP(obj, ENT_PROP_ID);
        idx = PROP(obj, ENT_PROP_ID);
        first = g_EntityFlags[idx] & SLOT_FLAG_RESERVED;
    }
    if (first != 0) {
        if (PROP(obj, ENT_PROP_TYPE) == KIND_CHAIN) {
            D_8006CDF4 += CHAIN_STEP;
        }
        return ERR_NO_OBJECT;
    }

    if (PROP(obj, ENT_PROP_TYPE) == 0 || (PROP(obj, ENT_PROP_ID) > INDEX_MIN_EXCL && PROP(obj, ENT_PROP_ID) < INDEX_MAX_EXCL)) {
        slot = g_PlayerSlot[0];
        g_Player = (s32)slot;
        i = SLOT_FIRST_PLAYER;
    } else {
        while (1) {
            if (g_ObjFreeList[0] == LIST_TYPE_FREE) {
                slot = (s32 *)D_8006CB5C;
            } else {
                slot = (s32 *)D_8006CBA4;
            }
            i = SLOT_FIRST_FREE;
            if (slot != PTR_NONE) {
                break;
            }
            p = &g_ObjLists;
            for (idx = 0; idx < SLOT_COUNT; idx++) {
                r = Obj_Find(p, Obj_HasStateFlags, PARAMS_NO_CHAIN);
                if (r != 0) {
                    break;
                }
                p += SLOT_SIZE;
            }
            if (idx >= SLOT_COUNT) {
                return ERR_NO_SLOT;
            }
            Obj_KillWithEvent(r);
        }
    }
    Obj_SetParent(slot, (s32 *)(&g_ObjLists + i * SLOT_SIZE));
    slot[1] = SLOT_STATE_ACTIVE;
    entry = (s32 *)LOOKUP(obj, ENT_PROP_SETTINGS, &sp28);
    if (entry == PTR_NONE) {
        sp28 = 0;
    }
    r = Obj_Init(slot, PROP(obj, ENT_PROP_TYPE), PROP(obj, ENT_PROP_SUBTYPE), sp28, (s32)entry);
    if (r >= ERR_THRESHOLD) {
        Obj_SetParent(slot, (s32 *)(&g_ObjLists + SLOT_LIST_OFFSET));
        return r;
    }
    /* The flag goes at the first call's index; the second call's result is unused. */
    idx = PROP(obj, ENT_PROP_ID);
    PROP(obj, ENT_PROP_ID);
    g_EntityFlags[idx] |= SLOT_FLAG_USED;
    slot[0xB8 / 4] = PROP(obj, ENT_PROP_ID) << FIXED_SHIFT;
    if (AT(arg0, 8, s32) == OBJ_TYPE_PLAYER) {
        arg0 = (void *)g_CurrentZone;
    }
    AT(slot, 0x18, void *) = arg0;
    memset(slot + 0x20 / 4, 0, CLEAR_SIZE);
    AT(slot, 0xF0, s32) = obj;
    AT(slot, 0xF8, s32) = Ent_GetPropertyRowLength(obj, ENT_PROP_POSITIONS, 0, ENT_ROW_INDEX) << FIXED_SHIFT;
    func_8001F054((GoolObject *)slot, 0, slot + 0x60 / 4);
    if ((s32)slot == g_Player) {
        row = D_8006759C + D_8006CD7C * ENTITY_POS_ROW_SIZE;
        AT(slot, 0x60, s32) = AT(row, 0x1E8, s32);
        AT(slot, 0x64, s32) = AT(row, 0x1EC, s32);
        AT(slot, 0x68, s32) = AT(row, 0x1F0, s32);
        if ((s32)slot == g_Player && g_LevelRequest == -1) {
            D_8006CC84 = g_LevelRequest;
            Sys_SaveCheckpoint((GoolObject *)slot, &D_80060958, 1, 0);
        }
    }
    if (AT(AT(AT(slot, 0xC, u8 *), 0x10, u8 *), 4, s32) == MODEL_ID_SPECIAL) {
        Obj_SetParent(slot, &D_8006CB38);
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
#else
INCLUDE_ASM("asm/nonmatchings/game/80019098", Obj_SpawnEntity);
#endif

#ifdef NON_MATCHING
#define SLOT_STATE_ACTIVE 3
#define SLOT_STATE_OFFSET_WORDS 1
#define SLOT_CLEAR_OFFSET_BYTES 0x20
#define SLOT_CLEAR_SIZE_BYTES 0x20
#define SEARCH_SLOT_COUNT 8
#define SEARCH_SLOT_STRIDE 8
#define SEARCH_FLAGS 0x80000
#define LIST_TYPE_FREE 2
#define ERR_INVALID -0x16
#define RESULT_ERROR_LIMIT -0xFE

extern s32 g_Player;
extern s32 *g_PlayerSlot[];
extern s32 g_ObjFreeList[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;
extern u8 g_ObjLists;
extern char Obj_HasStateFlags[];

s32 Obj_Spawn(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 *slot;
    s32 result;
    s32 found;
    s32 i;
    u8 *entry;

    if (arg1 == 0 && arg2 == 0) {
        slot = *g_PlayerSlot;
        g_Player = (s32)slot;
    } else {
        while (1) {
            if (*g_ObjFreeList == LIST_TYPE_FREE) {
                slot = (s32 *)D_8006CB5C;
            } else {
                slot = (s32 *)D_8006CBA4;
            }
            if (slot != 0 || arg5 == 0) {
                break;
            }
            found = 0;
            entry = &g_ObjLists;
            for (i = 0; i < SEARCH_SLOT_COUNT; i++) {
                found = Obj_Find(entry, Obj_HasStateFlags, SEARCH_FLAGS);
                if (found != 0) {
                    break;
                }
                entry += SEARCH_SLOT_STRIDE;
            }
            if (found == 0) {
                break;
            }
            Obj_KillWithEvent(found);
        }
    }
    if (slot == 0) {
        return ERR_INVALID;
    }
    Obj_SetParent(slot, arg0);
    slot[SLOT_STATE_OFFSET_WORDS] = SLOT_STATE_ACTIVE;
    result = Obj_Init(slot, arg1, arg2, arg3, arg4);
    memset((u8 *)slot + SLOT_CLEAR_OFFSET_BYTES, 0, SLOT_CLEAR_SIZE_BYTES);
    if (result < RESULT_ERROR_LIMIT) {
        result = (s32)slot;
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80019098", Obj_Spawn);
#endif

#ifdef NON_MATCHING
#define OBJ_STATE_ACTIVE 1
#define OBJ_MODE_LOADED 3
#define COPY_BLOCK_WORDS 9
#define BLOCK_OFFSET_WORDS (0x60 / 4)
#define ENTRY_TABLE_OFFSET 0x10
#define ENTRY_INVALID 0x6396347F
#define ENTRY_COUNT 0x40
#define RESULT_OK_LIMIT (-0xFE)
#define RESULT_FAIL (-0xFF)
#define RESULT_BAD_ENTRY (-0xE)
#define RESULT_NO_FRAME (-0x1B)
#define FRAME_NONE 0xFF
#define SCALE_ONE 0x1000
#define NO_ANIM 0xFFFF
#define ANIM_FIRST 4
#define ANIM_LAST_EXCL 6
#define ANIM_EXTRA 0x1D

extern u8 *D_8006759C;

#define W(base, off) (((s32 *)(base))[(off) / 4])

s32 Obj_Init(s32 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 *parent;
    s32 *entry;
    void *res;
    s32 *table;
    u16 frame;
    s32 i;

    parent = (s32 *)W(arg0, 0x44);
    W(arg0, 0x138) = NO_ANIM;
    W(arg0, 0xB8) = 0;
    W(arg0, 0xF0) = 0;
    W(arg0, 0xF4) = 0;
    W(arg0, 0xF8) = 0;
    W(arg0, 0) = 1;
    W(arg0, 0x84) = 0;
    W(arg0, 0x88) = 0;
    W(arg0, 0x8C) = 0;
    W(arg0, 0x90) = 0;
    W(arg0, 0x94) = 0;
    W(arg0, 0x98) = 0;
    W(arg0, 0x9C) = 0;
    W(arg0, 0xA0) = 0;
    W(arg0, 0xA4) = 0;
    W(arg0, 0x104) = 0;
    W(arg0, 0xEC) = 0;
    W(arg0, 0xD8) = 0;
    W(arg0, 0x94) = 0;
    W(arg0, 0xA8) = 0;
    W(arg0, 0xB0) = 0;
    W(arg0, 0xAC) = 0;
    W(arg0, 0x108) = 0;
    W(arg0, 0x118) = 0;
    W(arg0, 0x110) = 0;
    W(arg0, 0x128) = 0;
    W(arg0, 0x12C) = -2;
    W(arg0, 0x120) = 0;
    if (parent[0] == OBJ_STATE_ACTIVE) {
        parent = (s32 *)W(arg0, 0x44);
        W(arg0, 0x18) = parent[0x18 / 4];
        for (i = 0; i < COPY_BLOCK_WORDS; i++) {
            arg0[BLOCK_OFFSET_WORDS + i] = parent[BLOCK_OFFSET_WORDS + i];
        }
    } else {
        W(arg0, 0x18) = 0;
        W(arg0, 0x6C) = 0;
        W(arg0, 0x70) = 0;
        W(arg0, 0x74) = 0;
        W(arg0, 0x78) = SCALE_ONE;
        W(arg0, 0x7C) = SCALE_ONE;
        W(arg0, 0x80) = SCALE_ONE;
    }
    if (arg1 >= ANIM_FIRST && (arg1 < ANIM_LAST_EXCL || arg1 == ANIM_EXTRA)) {
        W(arg0, 0x18) = 0;
    }
    if (W(arg0, 4) != OBJ_MODE_LOADED) {
        return RESULT_BAD_ENTRY;
    }
    entry = (s32 *)(D_8006759C + arg1 * 4 + ENTRY_TABLE_OFFSET);
    if (*entry == ENTRY_INVALID || arg1 >= ENTRY_COUNT) {
        return RESULT_BAD_ENTRY;
    }
    res = Nsf_OpenEntry(entry, 1, 0);
    W(arg0, 0xC) = (s32)res;
    W(arg0, 0x10) = (s32)res;
    if (W(arg0, 0xC) >= RESULT_OK_LIMIT) {
        return RESULT_BAD_ENTRY;
    }
    W(arg0, 0x40) = (s32)arg0;
    W(arg0, 0x58) = 0;
    W(arg0, 0x50) = 0;
    W(arg0, 0x5C) = 0;
    table = (s32 *)W(arg0, 0xC);
    W(arg0, 0xB4) = arg2;
    W(arg0, 0xE8) = 0;
    W(arg0, 0xD0) = 0;
    W(arg0, 0x54) = (s32)g_PlayerSlot[0];
    frame = *(u16 *)(W(table, 0x1C) + (W((s32 *)W(table, 0x10), 0x10) + W(arg0, 0xB4)) * 2);
    if (frame == FRAME_NONE) {
        return RESULT_NO_FRAME;
    }
    if (Gool_ChangeState(arg0, frame, arg3, (s32 *)arg4) < RESULT_OK_LIMIT) {
        return RESULT_FAIL;
    }
    return RESULT_BAD_ENTRY;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80019098", Obj_Init);
#endif

s32 g_Player;
extern s32 g_EntityFlags[];
extern s32 *g_PlayerSlot[];
extern s32 g_ObjFreeList[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;

s32 Obj_SetParent(s32 *, s32 *);
void func_80032808(s32 *);

s32 Obj_Kill(s32 *arg0, s32 arg1) {
    s32 *child;
    s32 *next;
    s32 *slot;
    s32 *base;
    s32 cur;

    if (arg0 != 0 && (s32)arg0 != g_Player) {
        if (arg1 != 0) {
            Gool_SendEvent(0, (s32)arg0, 0x1A00, 0, 0);
        }
        child = (s32 *)arg0[0x4C / 4];
        while (child != 0) {
            next = (s32 *)child[0x48 / 4];
            Obj_Kill(child, arg1);
            child = next;
        }
        func_80032808(arg0);
        cur = g_Player;
        g_EntityFlags[arg0[0xB8 / 4] >> 8] &= ~1;
        arg0[0] = 0;
        if ((s32)arg0 == cur) {
            if (g_ObjFreeList[0] == 2) {
                arg1 = D_8006CB5C;
            } else {
                arg1 = D_8006CBA4;
            }
            base = g_ObjFreeList;
            Obj_SetParent(arg0, base);
            /* the block wrapper changes the store scheduling */
            do {
                slot = base + 0x13;
                if (base[0] == 2) {
                    slot = base + 1;
                }
                *slot = arg1;
                g_PlayerSlot[0][0x44 / 4] = 0;
                g_PlayerSlot[0][0x4C / 4] = 0;
                g_PlayerSlot[0][0x48 / 4] = 0;
            } while (0);
            g_Player = 0;
            return -0xFF;
        }
        Obj_SetParent(arg0, g_ObjFreeList);
    }
    return -0xFF;
}

#ifdef NON_MATCHING
#define FLAGS_A_BLOCK 0x1000000
#define FLAGS_B_BLOCK 0x40000
#define CALL_ARG_1A00 0x1A00
#define RESULT_REJECTED -0xFF
#define NO_VALUE -1

extern s32 D_800608D0;

s32 func_80019D30(GoolObject *obj, s32 key) {
    if (obj->entity_id != 0) {
        return RESULT_REJECTED;
    }
    if (obj->unk_0 == 0 || (s32)obj->zone != key ||
        (obj->status_b & FLAGS_A_BLOCK) != 0 ||
        (obj->state_flags & FLAGS_B_BLOCK) != 0) {
        return RESULT_REJECTED;
    }
    Gool_SendEvent(0, (s32)obj, CALL_ARG_1A00, 0, 0);
    if ((s32)obj->zone == key || D_800608D0 == NO_VALUE) {
        return Obj_Kill((s32 *)obj, 0);
    }
    return RESULT_REJECTED;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019D30);
#endif

void Obj_KillWithEvent(s32 a0) {
    Obj_Kill((s32 *)a0, 1);
}

/* Without a prototype: its equivalent C under NON_MATCHING defines the parameters' types */

void Zone_KillObjects(void *arg0) {
    Obj_ForEachList(Obj_ForEachDescendant, func_80019D30, arg0);
}

/* volatile: the retail code reloads it after the null check. */
extern Unk608CC *volatile g_CurrentZone;

void Zone_KillOutsideObjects(void) {
    Unk19E5C *obj;
    s32 i;

    if (g_CurrentZone != 0) {
        obj = g_CurrentZone->unk10;
        for (i = 0; i < obj->count; i++) {
            if (Nsf_CloseEntry(&obj->items[i], 0) != 0) {
                Zone_KillObjects(Nsf_OpenEntry(&obj->items[i], 1, 0));
            }
        }
    }
}

extern char g_GoolOpcodeHandlers[];
extern char D_8005C650[];
extern s32 D_80060944;
extern u32 *g_DisplayBuffer;
extern char g_GoolGlobals[];

void Gool_InitScratchpad(void) {
    u32 n;
    u32 *p;

    n = g_DisplayBuffer[5];
    *(s32 *)0x1F800050 = D_80060944;
    p = (u32 *)0x1F800054;
    if (n >= 0x67) {
        n = 0x66;
    }
    *p = n;
    *(char **)0x1F800058 = g_GoolGlobals;
    Sys_Memcpy(g_GoolOpcodeHandlers, (void *)0x1F800060, (D_8005C650 - g_GoolOpcodeHandlers) / 2);
    *(s32 *)0x1F80005C = 0x1F800060;
}

extern u8 D_8006CB30[];
extern u8 Obj_HasId[];

s32 Obj_FindById(s32 arg0) {
    u8 *p;
    u8 *a;
    s32 i;
    s32 r;

    i = 3;
    p = D_8006CB30;
    do {
        a = p;
        p += 8;
        i++;
        r = Obj_Find(a, Obj_HasId, arg0);
        if (r != 0) {
            break;
        }
        a = p;
    } while (i < 5);
    return r;
}
