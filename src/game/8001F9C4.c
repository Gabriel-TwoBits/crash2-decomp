#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F1F4 = 0;
s32 D_8005F1F8 = 1;

#ifdef NON_MATCHING
#define SAVE_MAGIC 0x6396347F
#define GPU_CMD_TERMINATOR 0x0C01FFFF
#define NO_ENTRY 0x63
#define STATE_MENU 2
#define ENTRY_STRIDE 0x18
#define ENTRY_BASE_OFFSET 0x1DC
#define ENTRY_SEL_OFFSET 0x1E0
#define CLEAR_BYTES 0x1000
#define FLAG_BIT_3 0x8
#define FLAG_BIT_6 0x40
#define FLAG_BIT_8 0x100
#define FLAG_BIT_9 0x200
#define MODE_300 0x300
#define MODE_600 0x600
#define RETURN_CODE (-0xFF)
#define SLOT_AFTER_BIT 4
#define SLOT_NOT_FOUND 3
#define SLOT_FIRST 1

extern s32 g_TimerTicks;
extern s32 D_8005B914[];
extern s32 g_DemoEid;
extern s32 D_8005F3B0;
extern s32 D_8005F3B4;
extern s32 D_8005F3EC;
extern s32 g_CurrentZone;
extern s32 g_CameraPath;
extern s32 g_CameraProgress;
extern s8 D_8006094C;
extern s8 D_8006094D;
extern s8 D_8006094E;
extern s8 D_8006094F;
extern s32 D_80060954;
extern s32 D_8006097C;
extern s32 D_80060980;
extern s32 D_80060984;
extern s32 *D_80067598;
extern u8 *D_8006759C;
extern u8 g_EntityFlags[];
extern s32 D_8006CB80;
extern s32 D_8006CB94;
extern s32 D_8006CB98;
extern s32 D_8006CBB4;
extern s32 D_8006CBC0;
extern s32 D_8006CBDC;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 g_PlaybackState;
extern s32 D_8006CD7C;

#define FIELD(base, off) (*(s32 *)((base) + (off)))

s32 func_8001F9C4(void) {
    s32 *table;
    s32 entry;
    s32 key;
    s32 slot;
    s32 extra;
    s32 off;
    u8 *obj;

    g_PlaybackState = 0;
    if (*D_80067598 == SAVE_MAGIC) {
        FIELD(D_8006759C, 4) = 0;
        D_8005F1F8 = 0;
        return RETURN_CODE;
    }
    D_8006CB80 = GPU_CMD_TERMINATOR;
    D_8006CB94 = GPU_CMD_TERMINATOR;
    D_8005F3B0 = 0;
    D_8005F3B4 = 0;
    slot = D_8006CD7C;
    extra = 0;
    if (FIELD(D_8006759C, 8) != D_8005F3EC) {
        for (;;) {
        }
    }
    if (FIELD(D_8006759C, 8) == STATE_MENU) {
        if (D_8006CCD8 & 1) {
            slot = SLOT_AFTER_BIT;
        } else {
            key = D_8006CCD4;
            slot = SLOT_FIRST;
            if (key == NO_ENTRY) {
                key = D_8006CBC0;
            }
            if (key != NO_ENTRY) {
                key = key >> 8;
            }
            table = D_8005B914;
            switch (key) {
            case 37:
                key = 0x17;
                break;
            case 6:
                if (D_8006CBDC & FLAG_BIT_6) {
                    key = 0x11;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x11;
                } else {
                    key = 0x1E;
                }
                break;
            case 8:
                if (D_8006CBDC & FLAG_BIT_8) {
                    key = 0x21;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x21;
                } else {
                    key = 0x11;
                }
                break;
            case 3:
                if (D_8006CBDC & FLAG_BIT_3) {
                    key = 0xD;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0xD;
                } else {
                    key = 0x21;
                }
                break;
            case 9:
                if (D_8006CBDC & FLAG_BIT_9) {
                    key = 0x10;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x10;
                } else {
                    key = 0xD;
                }
                break;
            case 7:
                key = 0x10;
                break;
            case 39:
                key = 0xD;
                break;
            default:
                break;
            }
            for (;;) {
                entry = *table++;
                if (entry < 0) {
                    slot = SLOT_NOT_FOUND;
                    break;
                } else if (entry == key) {
                    break;
                } else {
                    slot++;
                }
            }
        }
    }
    D_8006CD7C = slot;
    if (slot >= FIELD(D_8006759C, 0)) {
        slot = 0;
        D_8006CD7C = 0;
    }
    memset(g_EntityFlags, 0, CLEAR_BYTES);
    func_8001F858(FIELD(D_8006759C, 8));
    Gfx_ResetView();
    if (D_8005F1F4 == 1) {
        off = slot * ENTRY_STRIDE;
        FIELD(D_8006759C + off, ENTRY_BASE_OFFSET) = D_8006097C;
        D_8005F1F4 = 0;
        FIELD(D_8006759C + off, ENTRY_SEL_OFFSET) = D_80060980;
        extra = D_80060984;
    }
    g_CurrentZone = 0;
    g_CameraPath = 0;
    g_CameraProgress = 0;
    off = slot * ENTRY_STRIDE;
    obj = (u8 *)Nsf_OpenEntry(D_8006759C + off + ENTRY_BASE_OFFSET, 1, 1);
    g_DemoEid = SAVE_MAGIC;
    D_8006094C = 0;
    D_8006094D = 0;
    D_8006094E = 0;
    D_8006094F = 0;
    D_80060954 = g_TimerTicks;
    if (D_8006CBB4 == MODE_600) {
        func_8002FE58(&g_DemoEid);
    }
    Cam_MoveAlongPath((s32)obj,
                  FIELD(obj, (FIELD(D_8006759C + off, ENTRY_SEL_OFFSET) * 3 + FIELD((u8 *)FIELD(obj, 0x10), 0x184)) * 4 + 0x10),
                  extra, 0);
    Nsf_CloseEntry(D_8006759C + off + ENTRY_BASE_OFFSET, 1);
    D_8005F1F8 = 0;
    return RETURN_CODE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", func_8001F9C4);
#endif

typedef struct Header {
    u8 pad[0x184];
    s32 unk184;
    s32 unk188;
    s32 unk18C;
} Header;

typedef struct Node {
    s32 unk0;
    struct Unk8001FDDC *unk4;
    struct Node *unk8;
} Node;

typedef struct Unk8001FDDC {
    u8 pad[0x10];
    Node *items[1];
} Unk8001FDDC;

void func_8001FFCC(Unk8001FDDC *);
extern s32 g_CurrentZone;

s32 Zone_PageIn(Unk8001FDDC *obj) {
    s32 n;
    s32 i;
    s32 j;
    Node *first;
    Node *node;
    s32 count;

    count = ((Header *)obj->items[0])->unk188;
    /* This empty loop and the next are scheduling barriers decomp-permuter found; the original's shape there is
       unknown */
    do {
    } while (0);
    n = count / 3;
    for (i = 0; i < n; i++) {
        first = obj->items[i * 3 + ((Header *)obj->items[0])->unk184];
        do {
        } while (0);
        j = 1;
        first->unk4 = obj;
        for (; j < 3; j++) {
            node = obj->items[i * 3 + j + ((Header *)obj->items[0])->unk184];
            node->unk4 = obj;
            node->unk8 = first;
        }
    }
    for (i = 0; i < ((Header *)obj->items[0])->unk18C; i++) {
        obj->items[i + ((Header *)obj->items[0])->unk184 + ((Header *)obj->items[0])->unk188]->unk4 = obj;
    }
    if (g_CurrentZone != 0 && Zone_IsLinked(g_CurrentZone, obj) != 0) {
        func_8001FFCC(obj);
    }
    return -0xFF;
}

s32 Zone_PageOut(char *arg0) {
    func_8002001C(arg0);
    return -0xFF;
}

#ifdef NON_MATCHING
s32 Zone_IsLinked(s32 arg0, s32 *arg1) {
    s32 key;
    s32 count;
    s32 n;
    s32 v;
    s32 i;
    char *obj;
    char *slot;
    s32 pad[2];

    obj = *(char **)(arg0 + 0x10);
    count = *(s32 *)(obj + 0x190);
    i = 0;
    if (count > 0) {
        key = arg1[1];
        n = count;
        slot = obj;
        do {
            v = *(s32 *)(slot + 0x194);
            if (v & 1) {
                if (v != key) {
                    i++;
                    goto next;
                }
                return 1;
            } else {
                i++;
                if (((s32 *)v)[1] == key) return 1;
            }
next:
            slot += 4;
        } while (i < n);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001F9C4", Zone_IsLinked);
#endif

extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;


void func_8001FFCC(Unk8001FDDC *arg0) {
    s32 *flags;

    flags = (s32 *)((u8 *)arg0->items[0] + 0x27C);
    if (!(*flags & 1)) {
        D_8006CDF0 = 0;
        D_8006CDF8 = 0;
        D_8006CDF4 = 0x19000;
        *flags |= 1;
    }
}

extern GoolObject *g_Player;
extern int D_800608D0;

void func_8002001C(char *arg0) {
    char *s1 = *(char **)(arg0 + 0x10);

    do {
        if (*(unsigned *)(s1 + 0x27C) & 1) {
            Zone_KillObjects(arg0);
            if (g_Player->zone == arg0) {
                g_Player->zone = (void *)D_800608D0;
            }
            *(unsigned *)(s1 + 0x27C) &= ~3;
        }
    } while (0);
}
