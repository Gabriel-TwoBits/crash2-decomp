#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80063034;

/* The entity queue's actions, the draw lists' ids: a kill for list A's entities, a spawn for list B's */
#define QUEUE_KILL ENT_PROP_DRAW_LIST_A
#define QUEUE_SPAWN ENT_PROP_DRAW_LIST_B

s32 g_EntityQueueCount;
extern Unk80063034 D_80063034[];

void Zone_QueueKillAll(void) {
    s32 n;
    s32 i;

    n = g_EntityQueueCount;
    /* Empty statement: without it the frame adjustment is scheduled before the load. */
    do {
    } while (0);
    for (i = 0; i < n; i++) {
        D_80063034[i].unk0 = QUEUE_KILL;
    }
}

#ifdef NON_MATCHING
#define KEY_MASK 0xFFFF00


extern s32 g_EntityQueueCount;
extern Unk80063034 g_EntityQueue[];

/* Entries are 0xC bytes from g_EntityQueue: key, 0, value (D_80063034). */
void Zone_QueueEntities(s32 arg0, s32 arg1, s32 arg2) {
    s32 *src;
    s32 n;
    s32 i;

    n = arg2 - 1;
    src = (s32 *)(arg1 + n * (s32)sizeof(s32));
    for (; n >= 0; n--, src--) {
        for (i = 0; i < g_EntityQueueCount; i++) {
            if ((g_EntityQueue[i].unk0 & KEY_MASK) == (*src & KEY_MASK)) {
                g_EntityQueue[i].unk0 = *src;
                D_80063034[i].unk0 = arg0;
                break;
            }
        }
        if (i == g_EntityQueueCount) {
            D_80063034[i].unk0 = arg0;
            g_EntityQueue[i].unk4 = 0;
            g_EntityQueueCount = i + 1;
            g_EntityQueue[i].unk0 = *src;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Zone_QueueEntities);
#endif

#ifdef NON_MATCHING
/* Cam_MoveAlongPath's direction toward the path's end, and one node's progress */
#define STEP_FORWARD 2
#define NODE_PROGRESS 0x100

void Zone_QueueDrawLists(s32 arg0, s32 a1, s32 a2) {
    s32 arg2 = a2;
    s32 arg1 = a1;
    s32 sp28;
    s32 temp_v0;
    s32 var_a1;
    s32 var_a2;

    var_a1 = ENT_PROP_DRAW_LIST_B;
    if (arg2 == STEP_FORWARD && (var_a1 = ENT_PROP_DRAW_LIST_A, arg2 == STEP_FORWARD)) {
        var_a2 = arg1 - NODE_PROGRESS;
    } else {
        var_a2 = arg1 + NODE_PROGRESS;
    }
    temp_v0 = Ent_GetProperty(arg0, var_a1, var_a2, 0, &sp28, ENT_ROW_EXACT, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        Zone_QueueEntities(QUEUE_KILL, temp_v0, sp28);
    }
    var_a1 = (arg2 == STEP_FORWARD) ? ENT_PROP_DRAW_LIST_B : ENT_PROP_DRAW_LIST_A;
    temp_v0 = Ent_GetProperty(arg0, var_a1, arg1, 0, &sp28, ENT_ROW_EXACT, 0, 0, 0);
    if (temp_v0 != 0 && sp28 != 0) {
        Zone_QueueEntities(QUEUE_SPAWN, temp_v0, sp28);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Zone_QueueDrawLists);
#endif

#ifdef NON_MATCHING
#define ENTRY_ANY (-1)
#define ADDR_MASK 0xFFFF00
#define ADDR_FLAG_SHIFT 6
#define ENTITY_SPAWNED 1
#define SLOT_MASK 0xFF
#define SLOT_COUNT 8
#define OWNER_SLOTS_OFFSET 0x10
#define SLOT_TABLE_OFFSET 0x194
#define SLOT_STRIDE_SHIFT 2
#define LOAD_FAILED_BELOW (-0xFE)
#define OBJ_FLAGS_A_OFFSET 0xAC
#define OBJ_FLAGS_A_MASK 0x01000000
#define OBJ_FLAGS_B_OFFSET 0x100
#define OBJ_FLAGS_B_MASK 0x40000
#define ENTRY_BYTE3_SHIFT 24
#define INIT_D_8006CDF4 0x19000

extern u8 g_EntityFlags[];
extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;

void Zone_RunEntityQueue(void *arg0, s32 arg1) {
    s32 i;
    s32 j;
    s32 remove;
    u32 addr;
    u32 slot;
    s32 obj;
    Unk80063034 *e;

    D_8006CDF0 = 0;
    D_8006CDF8 = 0;
    D_8006CDF4 = INIT_D_8006CDF4;
    for (i = g_EntityQueueCount - 1; i >= 0; i--) {
        e = &g_EntityQueue[i];
        if (arg1 != ENTRY_ANY && arg1 != e->unk8) {
            continue;
        }
        if (e->unk8 == QUEUE_KILL) {
            addr = (u32)e->unk0 & ADDR_MASK;
            if (*(u32 *)(g_EntityFlags + (addr >> ADDR_FLAG_SHIFT)) & ENTITY_SPAWNED) {
                if (e->unk4 == 0) {
                    e->unk4 = Obj_FindById((s32)addr);
                }
                obj = e->unk4;
                if (obj != 0 &&
                    !(*(u32 *)(obj + OBJ_FLAGS_A_OFFSET) & OBJ_FLAGS_A_MASK) &&
                    !(*(u32 *)(obj + OBJ_FLAGS_B_OFFSET) & OBJ_FLAGS_B_MASK)) {
                    Obj_Kill((s32 *)obj, 1);
                }
            } else {
                /* not spawned: nothing to kill */
            }
            remove = 1; /* a kill is always consumed */
        } else if (e->unk8 == QUEUE_SPAWN) {
            addr = (u32)e->unk0 & ADDR_MASK;
            slot = (u32)e->unk0 & SLOT_MASK;
            remove = 0;
            if (!(*(u32 *)(g_EntityFlags + (addr >> ADDR_FLAG_SHIFT)) & ENTITY_SPAWNED) && slot < SLOT_COUNT) {
                e->unk4 = Obj_SpawnEntity(
                    Nsf_OpenEntry((u8 *)*(s32 *)((u8 *)arg0 + OWNER_SLOTS_OFFSET) + (slot << SLOT_STRIDE_SHIFT) + SLOT_TABLE_OFFSET, 1, 0),
                    (u32)e->unk0 >> ENTRY_BYTE3_SHIFT);
                if (e->unk4 >= LOAD_FAILED_BELOW) {
                    remove = 1;
                }
            }
            if (!remove) {
                if (e->unk4 == 0) {
                    e->unk4 = Obj_FindById((s32)addr);
                }
                e->unk8 = 0;
            }
        } else {
            remove = 0;
        }
        if (remove) {
            for (j = i + 1; j < g_EntityQueueCount; j++) {
                g_EntityQueue[j - 1] = g_EntityQueue[j];
            }
            g_EntityQueueCount -= 1;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Zone_RunEntityQueue);
#endif

#ifdef NON_MATCHING
extern s32 g_Player;

typedef struct {
    u8 pad0[0x20];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30[6];
} Unk8001A4A4Inner;

/* What GoolObject.unk_8 points to, as this function reads it */
#define INNER(obj) ((Unk8001A4A4Inner *)(obj)->unk_8)

void func_8001A4A4(GoolObject *arg0, s32 *arg1) {
    s32 sp[6];
    s32 *s;
    s32 neg;
    s32 r;
    u8 *a;

    INNER(arg0)->f28 = 0;
    a = arg0->anim;
    if (a != 0) {
        if (*a != 4) {
            if (!RAM_ADDRESS((s32)(a + 4))) {
                return;
            }
            r = (s32)Nsf_OpenEntry(a + 4, 1, 0);
            INNER(arg0)->f28 = r;
            if (r >= -0xFE) {
                INNER(arg0)->f28 = 0;
                return;
            }
        }
        if (*arg0->anim == 1) {
            do {
                INNER(arg0)->f2C = func_8001A90C(arg0, (void **)&s, &INNER(arg0)->f24);
            } while (0);
            INNER(arg0)->f28 = (s32)Nsf_OpenEntry((s32 *)(INNER(arg0)->f20 = (s32)s) + 4, 1, 0);
            if ((s32)arg0 == g_Player) {
                neg = arg1[0];
                if (neg < 0) {
                    arg1[0] = -neg;
                }
                sp[0] = s[7] + s[10];
                sp[1] = s[8] + s[11];
                sp[2] = s[9] + s[12];
                sp[3] = s[7] + s[13];
                sp[4] = s[8] + s[14];
                sp[5] = s[9] + s[15];
                INNER(arg0)->f30[0] = (sp[0] >> 8) * arg1[0] >> 4;
                INNER(arg0)->f30[1] = (sp[1] >> 8) * arg1[1] >> 4;
                INNER(arg0)->f30[2] = (sp[2] >> 8) * arg1[2] >> 4;
                INNER(arg0)->f30[3] = (sp[3] >> 8) * arg1[0] >> 4;
                INNER(arg0)->f30[4] = (sp[4] >> 8) * arg1[1] >> 4;
                INNER(arg0)->f30[5] = (sp[5] >> 8) * arg1[2] >> 4;
                arg1[0] = neg;
            }
        } else {
            INNER(arg0)->f28 = -0xE;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A4A4);
#endif

#ifdef NON_MATCHING
s32 func_8001A710(GoolObject *arg0, s32 *arg1, GoolObject *arg2, s32 *arg3) {
    s32 box0[6];
    s32 box1[6];
    GoolObject *other;
    s32 d;

    other = arg0->links[GOOL_LINK_COLLIDER];
    if (other != 0 && other != arg2) {
        if (arg2->state_flags & 0x800) {
            arg2->links[GOOL_LINK_COLLIDER] = arg0;
            return -0x1C;
        }
        d = Math_ApproxDistance(arg0->pos, other->pos);
        if (!(Math_ApproxDistance(arg0->pos, arg2->pos) < d) && !(other->state_flags & 0x800)) {
            return -0x1C;
        }
    }
    arg0->links[GOOL_LINK_COLLIDER] = arg2;
    arg2->links[GOOL_LINK_COLLIDER] = arg0;
    if (arg1 == 0) {
        return -0xFF;
    }
    if (arg3 != 0) {
        if (arg0->unk_128 != 0) {
            box0[0] = arg1[0] + arg0->unk_128;
            box0[1] = arg1[1];
            box0[2] = arg1[2] + arg0->unk_128;
            box0[3] = arg1[3] - arg0->unk_128;
            box0[4] = arg1[4];
            box0[5] = arg1[5] - arg0->unk_128;
            if (func_80030EC0(box0, arg3) != 0) {
                arg0->status_a |= 0x1000;
            }
        }
        if (arg2->unk_128 != 0) {
            box1[0] = arg3[0] + arg2->unk_128;
            box1[1] = arg3[1];
            box1[2] = arg3[2] + arg2->unk_128;
            box1[3] = arg3[3] - arg2->unk_128;
            box1[4] = arg3[4];
            box1[5] = arg3[5] - arg2->unk_128;
            if (func_80030EC0(box1, arg1) != 0) {
                arg2->status_a |= 0x1000;
            }
        }
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A710);
#endif

#ifdef NON_MATCHING
u32 func_8001A90C(GoolObject *arg0, s32 *arg1, s32 *arg2) {
    char *s2;
    char *s3;
    s32 odd;
    u32 idx;
    s32 *p;

    s2 = (char *)arg0->anim;
    s3 = s2 + 4;
    Nsf_OpenEntry(s3, 1, 0);
    odd = *(s32 *)(s2 + 8) & 1;
    idx = arg0->anim_frame >> 8;
    if (arg2 == 0) {
        p = (s32 *)((char *)Nsf_OpenEntry(s3, 1, 0) + 0x10);
        if (odd) {
            *arg1 = p[idx >> 1];
        } else {
            *arg1 = p[idx];
        }
        return idx;
    }
    if (odd == 0) {
        p = (s32 *)((char *)Nsf_OpenEntry(s3, 1, 0) + 0x10);
        *arg1 = *arg2 = p[idx];
    } else if ((idx & 1) && (s32)(idx + 1) < *(u16 *)(s2 + 2)) {
        u32 h = idx >> 1;
        *arg1 = ((s32 *)((char *)Nsf_OpenEntry(s3, 1, 0) + 0x10))[h];
        *arg2 = ((s32 *)((char *)Nsf_OpenEntry(s3, 1, 0) + 0x10))[h + 1];
    } else {
        s32 h = (s32)(idx + 1) >> 1;
        p = (s32 *)((char *)Nsf_OpenEntry(s2 + 4, 1, 0) + 0x10);
        *arg1 = *arg2 = p[h];
    }
    return idx;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001A90C);
#endif

#ifdef NON_MATCHING
extern void (*D_8005B650[])(void *, void *, void *, s32);
extern s32 D_8005B690[];
extern s32 D_80062AE0;
extern s32 D_80062AE4;

void func_8001AA80(GoolObject *obj, s32 *out) {
    u8 *state;
    s32 idx;
    s32 v;

    state = obj->anim;
    if (state != 0) {
        switch (*state) {
        case 2:
        case 5:
            v = obj->scale[0];
            if (v < 0) {
                v = -v;
            }
            out[1] = v / 27279;
            idx = obj->unk_120;
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE0;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](obj->unk_8, obj->rot, obj->scale, 0);
            break;
        case 1:
            idx = obj->unk_120;
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE4;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](obj->unk_8, obj->rot, obj->scale,
                            *(s32 *)(*(u8 **)((u8 *)obj->unk_8 + 0x28) + 0x10));
            break;
        default:
            break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", func_8001AA80);
#endif

#ifdef NON_MATCHING
/* Field of type T at byte offset OFF of the object P points to. */
#define FLD(T, P, OFF) (*(T *)((u8 *)(P) + (OFF)))
#define OBJ(p) ((GoolObject *)(p))

#define PACKET_LIST_SLACK 0xC8
#define PACKET_LIST_LIMIT 0x1771

#define KIND_ONE 1
#define KIND_TWO 2
#define KIND_FOUR 4
#define KIND_FIVE 5

#define ROT_SHIFT 18
#define DIR_SHIFT 12
#define DIR_Y_DEFAULT 0xB50
#define FLAG_NEG_SIGN 0x80000000

#define MODE_SLOPE 12
#define MODE_A 8
#define MODE_B 19
#define MODE_C 6

extern GfxFrame *g_BuildBuffer;
extern void *g_CurrentZone;
extern s32 D_8006CCE4;
extern s32 D_80060944;
extern s32 g_CameraX;
extern s32 g_CameraY;
extern s32 g_CameraZ;
extern s32 D_80062A3C;
extern s32 D_80062A40;
extern s32 D_80062A44;
extern s32 D_80062ACC;
extern s32 D_80062AD0;
extern s32 D_80062AD4;
extern s32 D_80062AD8;
extern s32 D_80062ADC;
extern s32 D_8005B6D0[];
extern s32 D_8005B6EC[];
extern s32 D_8005B708[];
extern s32 D_8005B70C[];
extern s32 D_8005B71C;
extern s32 D_8005B75C[];
extern s32 D_8005B760[];
extern s32 D_8005B770;

void Gfx_QueueObject(u8 *arg0, void *arg1) {
    GfxFrame *list = g_BuildBuffer;
    u8 *prim = list->next_queue_packet;
    u8 *data = OBJ(arg0)->anim;
    u32 frame = OBJ(arg0)->anim_frame >> 8;
    u8 kind;
    s32 i;
    s32 n;

    if (!((u32)((prim - list->queue_buffer) + PACKET_LIST_SLACK) < PACKET_LIST_LIMIT) &&
        (s32)arg0 != g_Player) {
        return;
    }

    kind = FLD(u8, data, 0);
    if (kind == KIND_TWO) {
        s32 arg1_4 = FLD(s32, arg1, 4);
        u32 flags;
        s32 idx;
        s32 sel;
        u8 *hdr;

        func_80018474(data + (frame * 0x10 + 8) + 4, data + 4);
        prim = g_BuildBuffer->next_queue_packet;
        g_BuildBuffer->next_queue_packet = prim + 0x2C;
        FLD(s32, prim, 0) = (s32)g_BuildBuffer->object_queue;
        g_BuildBuffer->object_queue = prim;
        flags = FLD(u32, arg0, 0x120);
        hdr = arg0 + 0x20;
        idx = (flags >> 8) & 0xFF;
        sel = flags & 0xFF;
        if (idx == 0) {
            if (sel == 6 || sel == 0xD) {
                idx = 3;
            } else {
                idx = D_80062ADC;
            }
        } else {
            /* explicit table index */
        }
        FLD(s32, prim, 0x14) = FLD(s32, arg0, 8);
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        FLD(s32, prim, 8) = D_8005B75C[idx];
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(u8 *, prim, 0x1C) = data + (frame * 0x10 + 8);
        FLD(s32, prim, 0xC) = D_8005B708[idx];
        if (FLD(s16, hdr, 6) != 0) {
            FLD(s32, prim, 0x28) = (FLD(s16, hdr, 0x1A) << 0x14) | (FLD(s16, hdr, 0x1C) << 0xC) |
                                   (FLD(s16, hdr, 0x1E) * 0x10);
            FLD(s32, prim, 0x20) = FLD(s16, hdr, 6);
        } else {
            FLD(s32, prim, 0x20) = 0;
        }
        FLD(s32, prim, 0x24) = arg1_4;
        return;
    } else if (kind == KIND_FOUR) {
        s32 dir = 1;
        s32 f = OBJ(arg0)->status_b;
        if (f & 0x200000) {
            dir = -1;
        } else {
            /* keep forward */
        }
        if (f & 0x400) {
            dir = 0;
        } else {
            /* keep */
        }
        Gfx_DrawObjectText((GoolObject *)arg0, (char *)data, frame, (s32)g_BuildBuffer->ot, dir);
        return;
    } else if (kind == KIND_FIVE) {
        u8 *src = data + (frame * FLD(s32, data, 8) * 0x18 + 0xC);
        s32 arg1_4 = FLD(s32, arg1, 4);

        n = FLD(s32, data, 8);
        i = 0;
        if (n > 0) {
            do {
                func_80018474(src + 4, data + 4);
                i += 1;
                src += 0x18;
            } while (i < FLD(s32, data, 8));
        } else {
            /* no entries to copy */
        }
        prim = g_BuildBuffer->next_queue_packet;
        g_BuildBuffer->next_queue_packet = prim + 0x30;
        FLD(s32, prim, 0) = (s32)g_BuildBuffer->object_queue;
        g_BuildBuffer->object_queue = prim;
        FLD(s32, prim, 0x14) = FLD(s32, arg0, 8);
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        FLD(s32, prim, 8) = D_8005B770;
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(u8 *, prim, 0x1C) = data;
        FLD(s32, prim, 0x20) = 0;
        FLD(s32, prim, 0x24) = arg1_4;
        FLD(u32, prim, 0x2C) = frame;
        FLD(s32, prim, 0xC) = D_8005B71C;
        return;
    } else if (kind == KIND_ONE) {
        u8 *obj = FLD(u8 *, arg0, 8);
        s32 lo = FLD(s32, obj, 0x20);
        s32 hi = FLD(s32, obj, 0x24);
        u8 *model;
        u8 *hdr;
        s32 mode;
        u8 sub;
        s32 flags;
        s32 v;

        list->next_queue_packet = prim + 0x64;
        FLD(s32, prim, 0) = (s32)g_BuildBuffer->object_queue;
        g_BuildBuffer->object_queue = prim;
        obj = FLD(u8 *, arg0, 8);
        mode = FLD(u8, arg0, 0x121);
        model = FLD(u8 *, obj, 0x28);
        FLD(u8 *, prim, 0x14) = obj;
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        v = FLD(s32, arg0, 0xA0);
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(s32, prim, 0x50) = v;
        hdr = arg0 + 0x20;
        flags = OBJ(arg0)->status_b;
        if (flags & 0x100000) {
            FLD(s32, prim, 0x18) = FLD(s32, arg0, 0xE4);
        } else {
            FLD(s32, prim, 0x18) = D_80060944;
        }
        if (OBJ(arg0)->status_b & 0x40000) {
            FLD(void *, prim, 0x1C) = (void *)func_80041E50;
        } else {
            FLD(void *, prim, 0x1C) = (void *)func_80041D14;
        }
        if (mode == 0) {
            mode = D_80062ACC;
        } else {
            /* explicit mode */
        }
        sub = FLD(u8, arg0, 0x122);
        FLD(s32, prim, 0x20) = D_8005B6D0[sub];
        FLD(s32, prim, 0x24) = D_8005B6EC[sub];
        if ((s32)sub < 6 && (s32)sub >= 4) {
            s16 px = FLD(s16, arg0, 0x2E);
            s16 py = FLD(s16, arg0, 0x30);
            s16 pz = FLD(s16, arg0, 0x32);
            s32 dx;
            s32 dy;
            s32 dz;
            s32 sx;
            s32 sy;
            s32 len;
            u8 which;

            mode = 1;
            if (sub == 5) {
                s32 a = px * FLD(s32, arg0, 0x9C);
                s32 b = px * FLD(s32, arg0, 0x84);
                s32 c = py * FLD(s32, arg0, 0x88);
                px = (s16)((s32)((px * FLD(s32, arg0, 0x90)) + (py * FLD(s32, arg0, 0x94)) +
                                 (pz * FLD(s32, arg0, 0x98))) >> ROT_SHIFT);
                py = (s16)((s32)(a + (py * FLD(s32, arg0, 0xA0)) + (pz * FLD(s32, arg0, 0xA4))) >> ROT_SHIFT);
                pz = (s16)((s32)(b + c + (pz * FLD(s32, arg0, 0x8C))) >> ROT_SHIFT);
            } else {
                /* sub 4: vector used as is */
            }
            FLD(s32, prim, 0x50) = (py << 0x10) | (px & 0xFFFF);
            FLD(s32, prim, 0x54) = pz;
            which = FLD(u8, arg0, 0x120);
            if (which == 0xA || which == 0xF) {
                s32 ex = (g_CameraX - OBJ(arg0)->pos[0]) >> 8;
                s32 ey = (g_CameraY - OBJ(arg0)->pos[1]) >> 8;
                s32 ez = (g_CameraZ - OBJ(arg0)->pos[2]) >> 8;
                s32 sq = (ex * ex) + (ey * ey) + (ez * ez);
                if (sq == 0) {
                    dx = 0;
                    dy = 0;
                    dz = 0;
                } else {
                    len = func_80041A78(sq);
                    dx = (ex << DIR_SHIFT) / len;
                    dy = (ey << DIR_SHIFT) / len;
                    dz = (ez << DIR_SHIFT) / len;
                }
            } else {
                dx = -DIR_Y_DEFAULT;
                dy = 0;
                dz = DIR_Y_DEFAULT;
            }
            if (sub == 5) {
                s32 a = dx * FLD(s32, arg0, 0x9C);
                s32 b = dx * FLD(s32, arg0, 0x84);
                s32 c = dy * FLD(s32, arg0, 0x88);
                dx = (s32)((dx * FLD(s32, arg0, 0x90)) + (dy * FLD(s32, arg0, 0x94)) +
                           (dz * FLD(s32, arg0, 0x98))) >> ROT_SHIFT;
                dy = (s32)(a + (dy * FLD(s32, arg0, 0xA0)) + (dz * FLD(s32, arg0, 0xA4))) >> ROT_SHIFT;
                dz = (s32)(b + c + (dz * FLD(s32, arg0, 0x8C))) >> ROT_SHIFT;
            } else {
                /* sub 4: vector used as is */
            }
            sx = dx + px;
            sy = dy + py + pz;
            v = (sx * sx) + (sy * sy) + (dz * dz);
            if (v == 0) {
                dx = 0;
                dy = 0;
                dz = 0;
            } else {
                len = func_80041A78(v);
                dx = (sx << DIR_SHIFT) / len;
                dy = (sy << DIR_SHIFT) / len;
                dz = (dz << DIR_SHIFT) / len;
            }
            FLD(s32, prim, 0x58) = (dy << 0x10) | (dx & 0xFFFF);
            FLD(s32, prim, 0x5C) = dz;
        } else {
            /* not a vector sub-kind */
        }
        FLD(s32, prim, 0x48) = FLD(s32, arg0, 0x118) + D_80062AD0;
        FLD(s32, prim, 0x4C) = FLD(s32, arg0, 0x118) + D_80062AD4;
        switch (mode) {
        case MODE_SLOPE:
            if ((s32)arg0 == g_Player) {
                s32 floor;
                s32 y;
                if (!(D_80062AD8 & 0x1000) || (FLD(s32, FLD(u8 *, g_CurrentZone, 0x10), 0x29C) & 0x800) ||
                    (y = OBJ(arg0)->pos[1], floor = D_8006CCE4 - 0x3200, (y < floor)) ||
                    (FLD(s32, arg0, 0x13C) < floor)) {
                    mode = 1;
                } else {
                    s32 t = (D_8006CCE4 - y) << 5;
                    FLD(s32, prim, 0x50) = t;
                    FLD(s32, prim, 0x50) = t / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                }
            } else {
                s32 t = (FLD(s32, arg0, 0xFC) - OBJ(arg0)->pos[1]) << 5;
                FLD(s32, prim, 0x50) = t;
                FLD(s32, prim, 0x50) = t / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                FLD(s32, prim, 0x4C) = FLD(s32, arg0, 0x118) - 0xC8;
            }
            break;
        case MODE_A:
        case MODE_B:
        case MODE_C:
            if (OBJ(arg0)->status_b < 0) {
                FLD(s32, prim, 0x54) = FLD(s32, arg0, 0x9C) * 0x10;
                FLD(s32, prim, 0x58) = FLD(s32, arg0, 0xA0);
                FLD(s32, prim, 0x5C) = FLD(s32, arg0, 0xA4);
            } else {
                FLD(s32, prim, 0x54) = D_80062A3C;
                FLD(s32, prim, 0x58) = D_80062A40;
                FLD(s32, prim, 0x5C) = D_80062A44;
            }
            break;
        default:
            break;
        }
        if (lo == hi) {
            FLD(s32, prim, 8) = D_8005B75C[mode];
            v = D_8005B708[mode];
            FLD(s32, prim, 0x28) = hi;
        } else {
            FLD(s32, prim, 8) = D_8005B760[mode];
            v = D_8005B70C[mode];
            FLD(s32, prim, 0x28) = lo;
            FLD(s32, prim, 0x2C) = hi;
        }
        FLD(s32, prim, 0xC) = v;
        FLD(s32, prim, 0x30) = FLD(s32, model, 0x14);
        FLD(s32, prim, 0x34) = FLD(s32, model, 0x18);
        FLD(s32, prim, 0x38) = FLD(s32, model, 0x1C);
        FLD(s32, prim, 0x3C) = FLD(s32, model, 0x20);
        if (FLD(s32, model, 0xC) < 6) {
            FLD(s32, prim, 0x60) = 0;
        } else {
            FLD(s32, prim, 0x60) = FLD(s32, model, 0x24);
        }
        if (FLD(s16, hdr, 6) != 0) {
            FLD(s32, prim, 0x44) = (FLD(s16, hdr, 0x1A) << 0x14) | (FLD(s16, hdr, 0x1C) << 0xC) |
                                   (FLD(s16, hdr, 0x1E) * 0x10);
            FLD(s32, prim, 0x40) = FLD(s16, hdr, 6);
        } else {
            FLD(s32, prim, 0x40) = 0;
        }
        v = FLD(s32, prim, 0x40);
        if (OBJ(arg0)->scale[0] < 0) {
            v |= FLAG_NEG_SIGN;
        } else {
            /* sign flag stays clear */
        }
        FLD(s32, prim, 0x40) = v;
        return;
    } else {
        return;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Gfx_QueueObject);
#endif

#ifdef NON_MATCHING
#define TEXT_CMD '~'
#define TEXT_NEWLINE 0xA
#define TEXT_END '&'
#define DEFAULT_ADVANCE 0x190
#define BOUNDS_INIT 0xF423F
#define BOUNDS_FLAG 0x04000000
#define BOUNDS_MARGIN 0x64
#define BOUNDS_BASELINE 0xA
#define GLYPH_STRIDE 0x14
#define GLYPH_BASE_BIAS 0x278
#define PALETTE_BASE 0x2E
#define WORD_SIZE 4
#define OPT_BUF_SIZE 8
#define POS_X 0
#define POS_Y_TOP 1
#define POS_X_RIGHT 3
#define POS_Y_BOTTOM 4
#define POS_COUNT 5
#define BOUNDS_COUNT 5

#define FIELD(p, T, o) (*(T *)((u8 *)(p) + (o)))

s32 atoi(u8 *, s32);

/* Reads a "~"-terminated token into buf and returns its value. */
static s32 read_token(u8 **p, u8 *buf) {
    s32 n;
    u8 c;

    n = 0;
    c = *(*p)++;
    while (c != TEXT_CMD) {
        buf[n++] = c;
        c = *(*p)++;
    }
    buf[n] = 0;
    return atoi(buf, n);
}

s32 Gfx_DrawText(void *arg0, char *arg1, s32 arg2, s32 arg3, void *arg4, s32 arg5, s32 arg6) {
    s32 pos[POS_COUNT];
    s32 bounds[BOUNDS_COUNT];
    u8 buf_s[OPT_BUF_SIZE];
    u8 buf_x[OPT_BUF_SIZE];
    u8 *s;
    u8 *font;
    u8 *glyph;
    u8 *palette;
    s32 right;
    s32 bottom;
    s32 top;
    s32 adv_x;
    s32 adv_y;
    s32 y;
    s32 max_x;
    s32 has_bounds;
    s32 flag_c;
    u8 c;
    u8 kind;

    font = (u8 *)arg3;
    s = (u8 *)arg1;
    flag_c = 1;
    y = 0;
    max_x = 0;
    has_bounds = 0;
    if (arg6 != 0) {
        has_bounds = OBJ(arg0)->status_b & BOUNDS_FLAG;
        if (has_bounds != 0) {
            bounds[0] = BOUNDS_INIT;
            bounds[1] = BOUNDS_INIT;
            bounds[3] = -BOUNDS_INIT;
            bounds[4] = -BOUNDS_INIT;
        }
    }
    pos[POS_X] = arg2;
    adv_x = DEFAULT_ADVANCE;
    while (*s != 0) {
        adv_y = DEFAULT_ADVANCE;
        for (;;) {
            if (*s == TEXT_CMD) {
                s++;
                c = *s++;
                switch (c) {
                case 'p':
                    palette = FIELD(arg0, u8 *, 0xBC) - (*s - PALETTE_BASE) * WORD_SIZE;
                    s++;
                    if (*(s32 *)palette == 1) {
                        s++;
                    }
                    if (*s == 0) {
                        goto done;
                    }
                    continue;
                case 'c':
                    kind = *s++;
                    flag_c = kind == '1';
                    continue;
                case 's':
                    kind = *s++;
                    if (kind == 'x') {
                        adv_x = read_token(&s, buf_s);
                    } else {
                        adv_y = read_token(&s, buf_s);
                    }
                    continue;
                case 'x':
                    pos[POS_X] += read_token(&s, buf_x);
                    continue;
                case 'y':
                    y += read_token(&s, buf_x);
                    continue;
                case 'l':
                    if (*s == 0) {
                        goto done;
                    }
                    y -= FIELD(font, s16, 0x1A) << arg5;
                    continue;
                case '%':
                case 'n':
                    if (*s == 0) {
                        goto done;
                    }
                    pos[POS_X] = arg2;
                    y -= FIELD(font, s16, 0x1A) << arg5;
                    continue;
                default:
                    break;
                }
            }
            break;
        }
        c = *s;
        if (c == TEXT_NEWLINE) {
            pos[POS_X] = arg2;
            y -= FIELD(font, s16, 0x1A) << arg5;
        } else if (c == TEXT_END) {
            goto done;
        } else {
            right = pos[POS_X] + (adv_x << arg5);
            glyph = font + c * GLYPH_STRIDE - GLYPH_BASE_BIAS;
            pos[POS_X_RIGHT] = right;
            bottom = y + (FIELD(glyph, s16, 0x12) << arg5);
            top = bottom - (adv_y << arg5);
            pos[POS_Y_BOTTOM] = bottom;
            pos[POS_Y_TOP] = top;
            if (max_x < right) {
                max_x = right;
            }
            if (arg6 != 0 && FIELD(glyph, s32, 0) != 0) {
                if (has_bounds != 0) {
                    if (pos[POS_X] < bounds[0]) {
                        bounds[0] = pos[POS_X];
                    }
                    if (top < bounds[1]) {
                        bounds[1] = top;
                    }
                    if (bounds[3] < right) {
                        bounds[3] = right;
                    }
                    if (bounds[4] < bottom) {
                        bounds[4] = bottom;
                    }
                }
                func_80018474(glyph + 4, font + 4);
                Gfx_DrawGlyph(arg0, glyph, FIELD(arg0, s32, 0x118), arg4, pos, arg6, flag_c);
            }
            pos[POS_X] += FIELD(glyph, s16, 0x10) << arg5;
        }
        s++;
        adv_x = DEFAULT_ADVANCE;
    }
done:
    if (has_bounds != 0 && arg6 != 0 && FIELD(font, s32, 0x80) != 0) {
        bounds[0] -= BOUNDS_MARGIN;
        bounds[3] += BOUNDS_MARGIN;
        func_80018474(font + 0x84, font + 4);
        Gfx_DrawTextBox((s32)(font + 0x80), FIELD(arg0, s32, 0x118) - BOUNDS_BASELINE, arg4, bounds, arg6);
    }
    return max_x;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Gfx_DrawText);
#endif

#ifdef NON_MATCHING
extern char D_8006083C[];
extern s32 D_80060884;

void Gfx_DrawObjectText(GoolObject *arg0, char *arg1, s32 arg2, s32 arg3, s32 arg4) {
    char buf[200];
    s32 i;
    s32 t;
    s32 cnt;
    s32 off;
    s32 neg;
    void *mem;
    s32 *q;
    char *p;
    char *base = arg1;

    t = arg0->scale[0];
    if (t < 0) {
        t = -t;
    }
    cnt = t / 27279;
    if (func_8003D3CC(arg0->pos, D_8006083C, (u32)(arg0->unk_120 - 6) < 2U, cnt, D_8006083C - 0x68, D_80060884,
                      0x3C00, (((u32)arg0->status_b >> 18) ^ 1) & 1) != 0) {
        arg1 = base + 0xC;
        for (i = 0; i < arg2;) {
            if (*arg1 == 0) {
                i++;
            }
            arg1++;
        }
        off = (u32)arg0->display_mode >> 8;
        if (off == 0) {
            off = *(s32 *)(base + 8);
        }
        arg2 = (s32)arg0->exe->items[5] + off * 4;
        mem = Nsf_OpenEntry((void *)(arg2 + 4), 1, 0);
        q = arg0->sp;
        sprintf(buf, arg1, q[-2], q[-3], q[-4], arg0->sp[-5]);
        arg1 = buf;
        for (p = arg1; *p != 0; p++) {
        }
        if (p != arg1) {
            p--;
            if (p != arg1) {
                do {
                    if (*p != 0x20) {
                        break;
                    }
                    p--;
                } while (p != arg1);
            }
            p[1] = 0;
        }
        neg = 0;
        if (arg4 <= 0) {
            neg = Gfx_DrawText(arg0, arg1, 0, arg2, mem, cnt, 0);
            neg = -neg;
            if (arg4 == 0) {
                neg = neg / 2;
            }
        }
        Gfx_DrawText(arg0, arg1, neg, arg2, mem, cnt, arg3);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001A014", Gfx_DrawObjectText);
#endif

typedef struct {
    /* 0x00 */ u8 pad0[0x24];
    /* 0x24 */ s16 unk24;
} Unk8001BFD4;

void func_8001BFD4(void *arg0, s32 arg1) {
    ((Unk8001BFD4 *)arg0)->unk24 = 0;
}
