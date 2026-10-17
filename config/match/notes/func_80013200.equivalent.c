#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATE_READ_START 2
#define STATE_READ_WAIT 3
#define STATE_READ_DONE 4
#define STATE_PLAY_QUEUE 10
#define STATE_PLAYING 11
#define STATE_CHECK_DEST 13
#define STATE_FINISHED 20
#define STATE_IDLE 1
#define STATE_TABLE_SIZE 30
#define STATE_RESET 31
#define NO_FILTER (-1)
#define ERR_RETRY (-0x10)
#define ERR_NO_SLOT (-0xC)
#define READ_POLL_LIMIT (-0xFE)
#define DEST_MAGIC 0x8765
#define KIND_FIRST 3
#define KIND_INDIRECT 4
#define KIND_TABLE 5
#define DEFAULT_SIZE 0x10000
#define CD_SYNC_NOT_BUSY 2
#define BASE_KIND(b) (*(u16 *)((b) + 2))
#define BASE_PTR(b, off) (*(u8 **)((b) + (off)))
#define PTR_SLOT 0x10

typedef struct Unk80062BB4 {
    u8 pad0[4];
    s16 unk4;
    s16 unk6;
    u8 pad8[4];
    s16 unkC;
    s8 unkE;
    s8 unkF;
    u8 pad10[8];
    s32 unk18;
    u8 pad1C[0x10];
} Unk80062BB4;
extern Unk80062BB4 D_80062BB4[];
s32 func_80013084(Unk800130A0 *);
s32 func_80051238(u8 *, s32);
s32 func_80051298(s32, s32);
extern char D_800100D0[];
extern char D_800100F0[];
extern s32 D_80067FC0;
extern int D_80067FBC[];

void func_80013200(Unk80013BD8 *entry, s32 arg1) {
    Unk800130A0 *s0;
    Unk800130A0 *next;
    Unk80062BB4 *slot;
    u8 *base;
    u8 *ent;
    u8 *ent2;
    u32 kind;
    u32 count;
    u32 i;
    s32 idx;
    s32 a1;
    s32 r;
    s32 key;

    s0 = (Unk800130A0 *)entry;
loop:
    switch ((s16)(s0->state - 2)) {
    case 0:
        if (D_80067FB4[0] != 0) {
            return;
        }
        if (arg1 != NO_FILTER && func_80013038((Unk80013038 *)s0, arg1) == 0) {
            return;
        }
        D_80067FC0 = s0->index;
        if (D_8005F1A4 == 0) {
            goto cancel;
        }
        if (CdSync(1, 0) != CD_SYNC_NOT_BUSY && arg1 == NO_FILTER) {
            return;
        }
        next = func_800130A0(s0);
        D_80067FB4[0] = (int)s0;
        do {
        } while (func_80012F24((u8 *)next, s0->count) >= READ_POLL_LIMIT);
        s0 = next;
        goto loop;
    case 1:
        if (D_8005F1A4 == 0 || D_80067FB4[0] != (int)s0) {
            goto cancel;
        }
        r = func_80012ED0(0);
        if (r == 1) {
            if (func_800130A0(s0) == (Unk800130A0 *)ERR_RETRY) {
                printf(D_800100F0, func_80011F1C(s0->index));
                D_80067FB4[0] = 0;
                goto loop;
            }
            D_80067FB4[0] = 0;
            goto read_done;
        } else if (r == ERR_RETRY) {
            printf(D_8005F1C0);
            D_80067FB4[0] = 0;
            func_80013084(s0);
            goto loop;
        } else {
            return;
        }
    case 11:
        if (*s0->dest == DEST_MAGIC) {
            return;
        }
        if (func_800130A0(s0) == (Unk800130A0 *)ERR_RETRY) {
            printf(D_800100D0, func_80011F1C(s0->index));
            goto loop;
        }
        goto read_done;
    case 2:
        goto read_done;
    case 8:
        goto play_queue;
    case 9:
        goto playing;
    case 29:
        s0->state = STATE_IDLE;
        *((s8 *)s0 + 0xF) = 0;
        return;
    default:
        return;
    }

cancel:
    func_80013F28(s0);
    D_80067FB4[0] = 0;
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;

read_done:
    kind = BASE_KIND(s0->base);
    if ((s32)kind < KIND_TABLE && (s32)kind >= KIND_FIRST) {
        s0->state = STATE_PLAY_QUEUE;
        goto play_queue;
    }
    func_800125F8(*(s16 *)((u8 *)s0 + 0xC));
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;

play_queue:
    if (D_80067FBC[0] != 0) {
        return;
    }
    if (func_80051328(0) == 0) {
        return;
    }
    base = s0->base;
    kind = BASE_KIND(base);
    a1 = NO_FILTER;
    if (kind == KIND_INDIRECT) {
        a1 = **(s32 **)(BASE_PTR(base, PTR_SLOT) + PTR_SLOT);
    }
    idx = func_80013AE8(kind, a1);
    if (idx == ERR_NO_SLOT) {
        if (BASE_KIND(s0->base) == KIND_TABLE) {
            return;
        }
        func_80013F28(s0);
        D_80067FB0[0] = D_80067FB0[0] - 1;
        return;
    }
    slot = &D_80062BB4[idx];
    s0->state = STATE_PLAYING;
    s0->pad20[1] = (s32)slot;
    D_80067FBC[0] = (int)s0;
    slot->unk4 = STATE_PLAYING;
    *(s32 *)((u8 *)slot + 0x14) = s0->index;
    key = s0->pad18;
    slot->unkF = 0;
    slot->unkE = 0;
    slot->unk18 = key;
    func_80051298(*(s32 *)((u8 *)slot + 0x1C), STATE_PLAYING);
    base = s0->base;
    kind = BASE_KIND(base);
    if (kind == KIND_INDIRECT) {
        ent = BASE_PTR(base, PTR_SLOT);
        ent2 = BASE_PTR(ent, PTR_SLOT);
        func_80051238(BASE_PTR(ent, 0x14), *(s32 *)(ent2 + 4));
    } else if (kind == KIND_TABLE) {
        count = *(u32 *)(base + 8);
        if (count != 0) {
            for (i = 0; i < count; i++) {
                ent = BASE_PTR(base + i * 4, PTR_SLOT);
                if (*(s32 *)(ent + 4) == key) {
                    break;
                }
            }
            if (i < count) {
                a1 = func_800123BC((s32 *)ent, 0);
                func_80051238(BASE_PTR(BASE_PTR(s0->base + i * 4, PTR_SLOT), PTR_SLOT), a1);
            }
        }
    } else if (kind == KIND_FIRST) {
        func_80051238(base, DEFAULT_SIZE);
    } else {
        /* other kinds start nothing */
    }

playing:
    if (D_80067FBC[0] != (int)s0) {
        return;
    }
    if (func_80051328(0) == 0) {
        return;
    }
    if (BASE_KIND(s0->base) == KIND_TABLE) {
        s0->state = STATE_FINISHED;
        ((Unk80062BB4 *)s0->pad20[1])->unk4 = STATE_FINISHED;
    } else {
        func_800125F8(*(s16 *)((u8 *)s0 + 0xC));
    }
    D_80067FBC[0] = 0;
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;
}
