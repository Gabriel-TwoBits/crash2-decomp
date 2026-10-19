#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_800100D0);
INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_800100F0);
INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_80010110);

s32 g_UseCd = 1;
char D_8005F1A8[] = "hde\n";
RECT D_8005F1B0 = { 0, 0, 0x100, 0x80 };
char D_8005F1B8[] = "de ls\n";
char D_8005F1C0[] = "de l\n";
char D_8005F1C8[] = "!mem\n";
s32 g_Quit = 0;

#ifdef NON_MATCHING
extern char D_8005F1A8[];
int CdIntToPos(int, unsigned char *);
int CdRead(int, s32, int);
int printf(const char *, ...);

s32 Cd_ReadFile(s32 *a0, s32 arg1, s32 arg2) {
    u8 sp10[8];
    u8 sp18[8];
    s32 s2;
    s32 *arg0;
    s32 ret;

    ret = g_UseCd;
    if (ret == 0) {
        goto end;
    }
    s2 = 0;
    if (arg2 != 1) {
        return arg1;
    }
    arg0 = a0;
    if (arg1 == 0) {
        s2 = (((arg0[1] & 0x1FFFF) + 0x7FF) / 2048) << 11;
        arg1 = Heap_Alloc(s2);
        if (arg1 == 0) {
            return -0xF;
        }
    }
loop:
    CdIntToPos(arg0[0] & 0x7FFFFF, sp18);
    if (CdControl(2, sp18, sp10) == 0) {
        goto loop;
    }
    if (CdRead((u32)(((arg0[1] & 0x1FFFF) + 0x7FF) / 2048), arg1, 0x80) == 0 || Cd_SyncRead(1) == -0x10) {
        printf(D_8005F1A8);
        goto loop;
    }
    if (s2 != 0 && s2 != (arg0[1] & 0x1FFFF)) {
        arg1 = Heap_Realloc(arg1, arg0[1] & 0x1FFFF);
    }
    ret = arg1;
end:
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Cd_ReadFile);
#endif

s32 Nsf_RelocatePage(NsfPage *page) {
    s32 count;
    s32 i;
    s32 j;
    u32 *table;
    u32 *p;
    u32 *q;
    NsfEntry *entry;
    u32 *data;

    table = (u32 *)page->entries;
    count = page->entry_count;
    if (page->type == NSF_PAGE_TYPE_TEXTURE) {
        return SUCCESS;
    }
    if (page->magic != NSF_PAGE_MAGIC) {
        return -0x12;
    }
    i = count - 1;
    if (i >= 0) {
        p = table - (-i);
        do {
            entry = (NsfEntry *)((u8 *)page + *p);
            *p = (u32)entry;
            j = entry->item_count;
            data = (u32 *)entry->items;
            if (j >= 0) {
                q = data - (-j);
                do {
                    j--;
                    *q = (u32)entry + *q;
                    q--;
                } while (j >= 0);
            }
            i--;
            p--;
        } while (i >= 0);
    }
    table[count] = (u32)page + table[count];
    return SUCCESS;
}

#ifdef NON_MATCHING
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

typedef struct Unk800125F8Handler {
    void *fn;
    u8 unk_4[0x18];
} Unk800125F8Handler;

typedef void (*Unk800125F8Fn2)(NsfEntry *, NsfEntry *);
typedef void (*Unk800125F8Fn3)(NsfEntry *, NsfEntry *, s32);

s32 Nsf_PageOut(NsfPageSlot *arg0);
extern int D_80067FB4[];
extern s32 *D_80067FC4;
extern NsdRecord **D_80067590;
extern Unk800125F8Handler D_8005B1E8[];
extern s32 D_80060150[];
extern NsfPageSlot D_80062D6C[];
extern s32 D_800675B4;

s32 Nsf_PageIn(s32 arg0) {
    NsfPageSlot *entry;
    NsfPageSlot *slot;
    NsfPageSlot *next;
    NsfPage *obj;
    NsfEntry *child;
    NsdRecord *dst;
    Unk800125F8Handler *handler;
    u32 i;
    s32 id;
    s32 page;
    s32 offset;
    s32 hash;

    entry = (NsfPageSlot *)((u8 *)&D_800675B4 + arg0 * ENTRY_SIZE);
    if (D_80067FB4[0] == (s32)entry) {
        D_80067FB4[0] = 0;
    }
    entry->state = STATE_BUSY;
    switch (entry->page->type) {
    case 0:
    case 5:
        obj = entry->page;
        dst = *(NsdRecord **)((u8 *)D_80067590 + ((obj->entries[0]->eid >> HASH_TO_TABLE_SHIFT) & HASH_TABLE_MASK));
        if (obj->entry_count != 0) {
            i = 0;
            do {
                child = entry->page->entries[i];
                hash = child->eid;
                while (dst->eid != hash) {
                    dst++;
                }
                dst->entry = child;
                entry->ref_count++;
                handler = &D_8005B1E8[child->type];
                if (handler->fn != 0) {
                    ((Unk800125F8Fn2)handler->fn)(child, child);
                }
                entry->ref_count--;
                i++;
            } while (i < entry->page->entry_count);
        }
        entry->state = STATE_READY;
        return (s32)entry->page;
    case NSF_PAGE_TYPE_TEXTURE:
        id = func_80013888();
        if (id == NO_FREE_SLOT) {
            Nsf_PageOut(entry);
            return STATE_BUSY;
        }
        slot = &D_80062D6C[id];
        page = id >> 2;
        D_8005F1B0.x = (id - page * 4) << PAGE_X_SHIFT;
        D_8005F1B0.y = page << PAGE_Y_SHIFT;
        LoadImage(&D_8005F1B0, entry->page);
        slot->state = STATE_READY;
        slot->unk_18 = entry->page->unk_4;
        slot->unk_14 = entry->unk_14;
        slot->unk_20 = Nsf_LookupEid(slot->unk_18);
        slot->ref_count = entry->ref_count;
        slot->unk_E = entry->unk_E;
        slot->unk_20->entry = (NsfEntry *)slot->unk_1C;
        entry->state = STATE_QUEUED;
        entry->ref_count = 0;
        D_80067FC4[entry->unk_14 >> 1] = (s32)slot;
        Gfx_BindTexturePage(entry->page->unk_4, 1);
        return id * 2;
    case 3:
    case 4:
        obj = entry->page;
        next = entry->unk_24;
        dst = *(NsdRecord **)((u8 *)D_80067590 + ((obj->entries[0]->eid >> HASH_TO_TABLE_SHIFT) & HASH_TABLE_MASK));
        if (obj->entry_count != 0) {
            i = 0;
            do {
                child = entry->page->entries[i];
                hash = child->eid;
                offset = (s32)child->items[0];
                while (dst->eid != hash) {
                    dst++;
                }
                if (i == 0) {
                    if (entry->page->type == OBJ_TYPE_ALT) {
                        D_80060150[next->unk_C] = dst->eid;
                    } else {
                        D_80060150[next->unk_C] = DEFAULT_HASH;
                    }
                }
                dst->entry = (NsfEntry *)((((offset - (s32)entry->page) + next->unk_1C) * 4) | LINK_TAG);
                handler = &D_8005B1E8[child->type];
                if (handler->fn != 0) {
                    ((Unk800125F8Fn3)handler->fn)(child, child, offset);
                }
                i++;
            } while (i < entry->page->entry_count);
        }
        next->state = STATE_READY;
        entry->state = STATE_QUEUED;
        entry->ref_count = 0;
        D_80067FC4[entry->unk_14 >> 1] = (s32)next;
        return next->unk_1C;
    default:
        entry->state = STATE_READY;
        return (s32)entry->page;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_PageIn);
#endif

#ifdef NON_MATCHING
#define STATE_IDLE 1
#define STATE_QUEUED_FIRST 0x14
#define STATE_QUEUED_COUNT 2
#define KIND_B_FIRST 3
#define KIND_B_COUNT 2
#define SLOT_FREED -0x12
#define RESULT_DONE -0xFF

extern int D_80067FB4[];
extern s32 *D_80067FC4;
extern NsdRecord **D_80067590;
extern struct { void (*fn)(void *); s32 pad[6]; } D_8005B1EC[];

s32 Nsf_PageOut(NsfPageSlot *arg0) {
    NsfPage *h;
    s32 *s1;
    s32 *s0;
    u32 i;
    s32 idx;
    NsfEntry *it;
    s32 key;

    if (D_80067FB4[0] == (int)arg0) {
        D_80067FB4[0] = 0;
    }
    if ((u16)(arg0->state - STATE_QUEUED_FIRST) < STATE_QUEUED_COUNT) {
        h = arg0->page;
        if (!(h->type == NSF_PAGE_TYPE_TEXTURE || (u16)(h->type - KIND_B_FIRST) < KIND_B_COUNT)) {
            goto loop;
        }
        D_80067FC4[arg0->unk_14 >> 1] = SLOT_FREED;
    }
    arg0->state = STATE_IDLE;
    arg0->ref_count = 0;
    arg0->unk_E = 0;
    return RESULT_DONE;
loop:
    idx = arg0->unk_14;
    /* s1 walks the records' entry words, s0 their EIDs */
    s1 = (s32 *)D_80067590[(h->entries[0]->eid >> NSD_BUCKET_SHIFT) & NSD_BUCKET_MASK];
    i = 0;
    if (h->entry_count != 0) {
        s0 = s1 + 1;
        do {
            it = arg0->page->entries[i];
            key = it->eid;
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
        } while (i < arg0->page->entry_count);
    }
    Nsf_FreePageSlot(arg0);
    return RESULT_DONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_PageOut);
#endif

/* A count and its slots: NsfLevel's page_slot_count and page_slots (D_800675B0), unk_3C8 and unk_3CC (D_80067950) */
typedef struct Unk80013BD8Table {
    s32 count;
    NsfPageSlot e[1];
} Unk80013BD8Table;

#ifdef NON_MATCHING
#define SLOT_STATE_IN_USE 2
#define SLOT_KEY_ANY -1
#define CHANNEL_MARKER_MAX -0xFE
#define CALL_MAGIC 0x6396347F

extern s32 D_80067FB0[];
extern Unk80013BD8Table D_80067950;
extern Unk80013BD8Table D_800675B0;
extern Nsd *D_800675A0;
void Nsf_UpdatePageSlot(NsfPageSlot *, s32);

void Nsf_UpdatePaging(s32 arg0) {
    s32 sp18;
    s32 key;
    s32 found;
    s32 i;
    s32 v;
    s32 lim;
    s32 idx;
    s32 nxt;
    s32 *tbl;
    NsfPageSlot *p;

    if (D_80067FB0[0] != 0) {
        found = -1;
        if (D_80067FB4[0] == 0) {
            for (i = 0; i < D_80067950.count; i++) {
                p = &D_80067950.e[i];
                if (p->state == SLOT_STATE_IN_USE) {
                    v = p->unk_14;
                    if ((arg0 == SLOT_KEY_ANY && (found == arg0 || v < key)) || v == arg0) {
                        key = v;
                        found = i;
                        if (key == arg0) {
                            break;
                        }
                    }
                }
            }
            if (found != -1) {
                sp18 = 1;
                if (g_UseCd != 0) {
                    idx = key >> 1;
                    lim = D_800675A0->page_count - 1;
                    if (idx < lim) {
                        tbl = D_80067FC4;
                        nxt = idx + 1;
                        do {
                            idx = (nxt * 2) >> 1;
                            v = tbl[idx];
                            if (v >= CHANNEL_MARKER_MAX) {
                                break;
                            }
                            if (((NsfPageSlot *)v)->unk_6 != 0) {
                                break;
                            }
                            sp18++;
                            nxt = idx + 1;
                        } while (idx < lim);
                    }
                }
                p = &D_80067950.e[found];
                func_80013C98(p, p->unk_14, CALL_MAGIC, &sp18, ~arg0 != 0);
            }
        }
        for (i = 0; i < D_800675B0.count; i++) {
            Nsf_UpdatePageSlot(&D_800675B0.e[i], arg0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_UpdatePaging);
#endif

extern s32 D_80067FB0[];
extern s32 D_80067FC8;
extern s16 D_8005F3E0;
extern NsfEntry *g_CurrentZone;

void func_80012DC0(void) {
    D_80067FC8 = 1;
    while (*D_80067FB0 != 0) {
        if (g_CurrentZone != 0 && D_8005F3E0 != -1) {
            func_80034254((s32)g_CurrentZone->items[0] + 0x2A4);
        }
        Nsf_UpdatePaging(-1);
    }
}

extern int D_80067FB4[];
extern int g_LevelFiles[];

void Nsf_SeekLevel(s32 arg0) {
    char sp10[8];
    int v;

    if (g_UseCd != 0 && D_80067FB4[0] == 0 && CdSync(1, 0) == 0) {
        v = g_LevelFiles[v = arg0 * 3];
        v = 0x7FFFFF & v;
        CdIntToPos(v, sp10);
        CdControlF(0x15, sp10);
    }
}

s32 Cd_SyncRead(s32 wait) {
    s32 ret;

    ret = CdReadSync(wait == 0, 0);
    if (ret < 0) {
        return -16;
    }
    if (ret != 0) {
        return 0;
    }
    ret = CdDataSync(wait == 0);
    if (ret < 0) {
        return -16;
    }
    return ret == 0;
}

#ifdef NON_MATCHING
#define SECTOR_TABLE_SHIFT 6
#define SHIFT_2K 11
#define CD_READ_SECTORS 0x80
#define CD_CMD_SEEKL 2
#define CD_SYNC_POLL 1
#define CD_SYNC_BLOCK 0
#define CD_STATUS_DISK_ERROR 5
#define LOAD_RESULT -0xFF

extern Nsd *D_800675A0;
extern s32 g_NsfStartSector;

s32 CdControlF(s32 cmd, u8 *param);
s32 CdSync(s32 mode, u8 *result);

s32 Nsf_ReadPages(NsfPageSlot *arg0, s32 arg1) {
    u8 pos[16];
    s32 sector;
    s32 count;
    s32 buf;
    s32 index;
    s32 poll;
    s32 block;

    index = arg0->unk_14;
    if (arg0->unk_A != 0) {
        s32 off = arg0->unk_8;

        count = (arg1 << NSF_PAGE_SECTOR_SHIFT) - off;
        buf = (s32)arg0->page + (off << SHIFT_2K);
        sector = g_NsfStartSector +
                 (D_800675A0->unk_420[index >> 1] >> SECTOR_TABLE_SHIFT);
    } else {
        count = arg1 << NSF_PAGE_SECTOR_SHIFT;
        buf = (s32)arg0->page;
        sector = g_NsfStartSector + D_800675A0->unk_418 + ((index >> 1) << NSF_PAGE_SECTOR_SHIFT);
    }

    /* Seek, wait for the seek to finish and issue the read; any failure retries from the seek. */
    for (;;) {
        CdIntToPos(sector, pos);
        if (CdControlF(CD_CMD_SEEKL, pos) == 0) {
            continue;
        }
        poll = CdSync(CD_SYNC_POLL, 0);
        block = CD_STATUS_DISK_ERROR;
        if (poll == 0) {
            block = CdSync(CD_SYNC_BLOCK, 0);
            poll = CD_STATUS_DISK_ERROR;
        }
        if (block == poll) {
            continue;
        }
        if (CdRead(count, buf, CD_READ_SECTORS) != 0) {
            return LOAD_RESULT;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_ReadPages);
#endif

s32 func_80013038(NsfPageSlot *arg0, s32 arg1) {
    s32 temp;

    if (arg1 == -1) {
        return 1;
    }
    arg1 >>= 1;
    temp = arg0->unk_24->unk_14 >> 1;
    if (temp >= arg1) {
        if (temp - arg0->unk_1C < arg1) {
            return 1;
        }
    }
    return 0;
}

/* The value Cd_SyncRead returns when a CD read fails */
#define CD_FAILURE -0x10
/* Nsf_UpdatePageSlot switches on the state from this one up */
#define FIRST_STATE 2

s32 func_80013084(NsfPageSlot *arg0) {
    arg0->state = FIRST_STATE;
    arg0->unk_1C = 1;
    arg0->unk_24 = arg0;
    return CD_FAILURE;
}

extern Nsd *D_800675A0;

NsfPageSlot *func_800130A0(NsfPageSlot *arg0) {
    NsfPageSlot *p;
    s16 off;
    s32 state;
    s32 count;
    s32 i;
    s32 flag;
    NsfPageSlot *last;

    p = arg0;
    state = p->state;
    count = p->unk_1C;
    off = 0;
    if (state == 3 || state == 13) {
        p->state = 4;
        Nsf_RelocatePage(p->page);
        return 0;
    }
    flag = 0;
    if ((p->unk_14 >> 1) < D_800675A0->unk_41C) {
        if (p) {
            flag = D_80067FC8 != 0;
        } else {
            flag = D_80067FC8 != 0;
        }
    }
    for (i = 0; i < count; i++) {
        if (state == 2) {
            p->unk_A = flag;
            if (flag != 0) {
                off = off + (1 << NSF_PAGE_SECTOR_SHIFT);
                off -= D_800675A0->unk_420[p->unk_14 >> 1] & 0x3F;
                p->unk_8 = off;
            }
            if (p->state == state) {
                p->state = 3;
            } else {
                p->state = 13;
                if (flag != 0) {
                    p->unk_10 = (u16 *)((u8 *)p->page +
                                        (((D_800675A0->unk_420[p->unk_14 >> 1] & 0x3F) + p->unk_8) << 11));
                } else {
                    /* the next slot's buffer */
                    p->unk_10 = (u16 *)p[1].page;
                }
                *p->unk_10 = 0x8765;
            }
        }
        last = p;
        p--;
    }
    return last;
}


#ifdef NON_MATCHING
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
#define CD_SYNC_NOT_BUSY 2

extern NsfPageSlot D_80062BB4[];
s32 func_80051238(u8 *, s32);
s32 func_80051298(s32, s32);
extern char D_800100D0[];
extern char D_800100F0[];
extern s32 D_80067FC0;
extern int D_80067FBC[];

void Nsf_UpdatePageSlot(NsfPageSlot *entry, s32 arg1) {
    NsfPageSlot *s0;
    NsfPageSlot *next;
    NsfPageSlot *slot;
    NsfPage *page;
    NsfEntry *ent;
    u8 *ent2;
    u32 kind;
    u32 count;
    u32 i;
    s32 idx;
    s32 a1;
    s32 r;
    s32 key;

    s0 = (NsfPageSlot *)entry;
loop:
    switch ((s16)(s0->state - 2)) {
    case 0:
        if (D_80067FB4[0] != 0) {
            return;
        }
        if (arg1 != NO_FILTER && func_80013038(s0, arg1) == 0) {
            return;
        }
        D_80067FC0 = s0->unk_14;
        if (g_UseCd == 0) {
            goto cancel;
        }
        if (CdSync(1, 0) != CD_SYNC_NOT_BUSY && arg1 == NO_FILTER) {
            return;
        }
        next = func_800130A0(s0);
        D_80067FB4[0] = (int)s0;
        do {
        } while (Nsf_ReadPages(next, s0->unk_1C) >= READ_POLL_LIMIT);
        s0 = next;
        goto loop;
    case 1:
        if (g_UseCd == 0 || D_80067FB4[0] != (int)s0) {
            goto cancel;
        }
        r = Cd_SyncRead(0);
        if (r == 1) {
            if (func_800130A0(s0) == (NsfPageSlot *)ERR_RETRY) {
                printf(D_800100F0, Nsf_PageRefName(s0->unk_14));
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
        if (*s0->unk_10 == DEST_MAGIC) {
            return;
        }
        if (func_800130A0(s0) == (NsfPageSlot *)ERR_RETRY) {
            printf(D_800100D0, Nsf_PageRefName(s0->unk_14));
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
        s0->ref_count = 0;
        return;
    default:
        return;
    }

cancel:
    Nsf_FreePageSlot(s0);
    D_80067FB4[0] = 0;
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;

read_done:
    kind = s0->page->type;
    if ((s32)kind < KIND_TABLE && (s32)kind >= KIND_FIRST) {
        s0->state = STATE_PLAY_QUEUE;
        goto play_queue;
    }
    Nsf_PageIn(s0->unk_C);
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;

play_queue:
    if (D_80067FBC[0] != 0) {
        return;
    }
    if (func_80051328(0) == 0) {
        return;
    }
    page = s0->page;
    kind = page->type;
    a1 = NO_FILTER;
    if (kind == KIND_INDIRECT) {
        a1 = *(s32 *)page->entries[0]->items[0];
    }
    idx = func_80013AE8(kind, a1);
    if (idx == ERR_NO_SLOT) {
        if (s0->page->type == KIND_TABLE) {
            return;
        }
        Nsf_FreePageSlot(s0);
        D_80067FB0[0] = D_80067FB0[0] - 1;
        return;
    }
    slot = &D_80062BB4[idx];
    s0->state = STATE_PLAYING;
    s0->unk_24 = slot;
    D_80067FBC[0] = (int)s0;
    slot->state = STATE_PLAYING;
    slot->unk_14 = s0->unk_14;
    key = s0->unk_18;
    slot->ref_count = 0;
    slot->unk_E = 0;
    slot->unk_18 = key;
    func_80051298(slot->unk_1C, STATE_PLAYING);
    page = s0->page;
    kind = page->type;
    if (kind == KIND_INDIRECT) {
        ent = page->entries[0];
        ent2 = ent->items[0];
        func_80051238(ent->items[1], *(s32 *)(ent2 + 4));
    } else if (kind == KIND_TABLE) {
        count = page->entry_count;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                ent = page->entries[i];
                if (ent->eid == key) {
                    break;
                }
            }
            if (i < count) {
                a1 = func_800123BC(ent, 0);
                func_80051238(s0->page->entries[i]->items[0], a1);
            }
        }
    } else if (kind == KIND_FIRST) {
        func_80051238((u8 *)page, 1 << NSF_PAGE_SHIFT);
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
    if (s0->page->type == KIND_TABLE) {
        s0->state = STATE_FINISHED;
        s0->unk_24->state = STATE_FINISHED;
    } else {
        Nsf_PageIn(s0->unk_C);
    }
    D_80067FBC[0] = 0;
    D_80067FB0[0] = D_80067FB0[0] - 1;
    return;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_UpdatePageSlot);
#endif

#ifdef NON_MATCHING
#define SLOT_FREE 1
#define SLOT_STATE_FIRST_RELEASABLE 0x14
#define SLOT_STATE_RELEASABLE_COUNT 2
#define NO_RUN -1

extern Unk80013BD8Table D_800675B0;
extern char D_8005F1C8[];
s32 Nsf_PageOut(NsfPageSlot *arg0);

s32 Nsf_EvictPageSlots(s32 *arg0, s32 arg1) {
    s32 i, j, n, t, best, bestLen;
    u32 cost, bestCost;
    NsfPageSlot *p;
    s32 cnt = D_800675B0.count;

    best = NO_RUN;
    bestLen = 0;
    bestCost = 0;
    cost = 0;
    for (i = 0; i < cnt; i++) {
        cost = 0;
        n = cnt - i;
        for (j = 0;; j++) {
            t = *arg0;
            if (n < t) {
                t = n;
            } else {
                /* t stays the requested length */
            }
            if (j >= t) {
                break;
            } else {
                p = &D_800675B0.e[i + j];
            }
            if (p->state != SLOT_FREE) {
                if ((u16)(p->state - SLOT_STATE_FIRST_RELEASABLE) < SLOT_STATE_RELEASABLE_COUNT && p->ref_count <= 0) {
                    cost = cost + 1 + p->unk_28;
                } else {
                    break;
                }
            } else {
                /* free slot: no cost */
            }
            if (best == NO_RUN || bestLen < j + 1 || (bestLen == j + 1 && cost < bestCost)) {
                best = i;
                bestLen = j + 1;
                bestCost = cost;
            } else {
                /* keep the earlier run */
            }
        }
    }
    if (best != NO_RUN) {
        for (j = best; j < best + bestLen; j++) {
            Nsf_PageOut((NsfPageSlot *)&D_800675B0.e[j]);
        }
        *arg0 = bestLen;
        return best;
    } else {
        *arg0 = 0;
        if (arg1 != 0) {
            printf(D_8005F1C8);
        } else {
            /* silent failure */
        }
        return -1;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_EvictPageSlots);
#endif

/* D_80062D70, D_80062D80, D_80062D84 and D_80062D8C read D_80062D6C's slots (NsfPageSlot) at 0x04, 0x14, 0x18 and
   0x20 by their own addresses */
typedef struct Unk80062D70 {
    s16 state;
    u8 pad[0x2A];
} Unk80062D70;

#ifdef NON_MATCHING
extern Unk80062D70 D_80062D70[];

#define SLOT_COUNT 16
#define STATE_ACTIVE 1
#define STATE_ALT 0x15
#define STATE_RANGE_FIRST 0x14
#define STATE_RANGE_COUNT 2
#define FLAG_BYTE_OFFSET 0xB
#define ERR_NO_SLOT -0xC

/* D_80063000 + 4 is D_80062D70[SLOT_COUNT - 1].state; every scan reads the slot table. */
s32 func_80013888(void) {
    s32 i;

    for (i = SLOT_COUNT - 1; i >= 0; i--) {
        if (D_80062D70[i].state == STATE_ACTIVE) {
            return i;
        }
    }
    for (i = SLOT_COUNT - 1; i >= 0; i--) {
        if (D_80062D70[i].state == STATE_ALT) {
            return i;
        }
    }
    for (i = SLOT_COUNT - 1; i >= 0; i--) {
        if ((u32)((u16)D_80062D70[i].state - STATE_RANGE_FIRST) < STATE_RANGE_COUNT) {
            if (((s8 *)&D_80062D70[i])[FLAG_BYTE_OFFSET] == 0) {
                Nsf_ReleaseTextureSlot(i);
                return i;
            }
        }
    }
    return ERR_NO_SLOT;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013888);
#endif


typedef struct Unk80062D80 {
    s32 v;
    u8 pad04[0x28];
} Unk80062D80;

typedef struct Unk80062D8C {
    s32 *v;
    u8 pad04[0x28];
} Unk80062D8C;

extern Unk80062D70 D_80062D70[];
extern Unk80062D80 D_80062D80[];
extern Unk80062D80 D_80062D84[];
extern Unk80062D8C D_80062D8C[];
extern s32 *D_80067FC4;

s32 Nsf_ReleaseTextureSlot(s32 arg0) {
    s16 st = D_80062D70[arg0].state;

    if (st != 0x14) {
        if (st != 0x15) {
            return -0xE;
        }
        goto tail;
    }
    Gfx_BindTexturePage(D_80062D84[arg0].v, 0);
    *D_80062D8C[arg0].v = D_80062D80[arg0].v;
    D_80067FC4[D_80062D80[arg0].v >> 1] = -0x12;
tail:
    D_80062D70[arg0].state = 1;
    return -0xE;
}


void func_80013A54(s32 arg0, s32 arg1) {
    if (arg1 != 0) {
        Nsf_ReleaseTextureSlot(arg0);
        D_80062D70[arg0].state = 0x1E;
    } else if (D_80062D70[arg0].state == 0x1E) {
        D_80062D70[arg0].state = 1;
    }
}

extern NsfPageSlot D_80062BB4[];

s32 func_80013AE8(s32 arg0, s32 arg1) {
    s32 i;
    NsfPageSlot *p;

    for (i = 9; i >= 0; i--) {
        p = &D_80062BB4[i];
        if (p->state == 1 && D_80062BB4[i].unk_6 == arg0 &&
            (arg1 == -1 || arg1 == D_80062BB4[i].unk_C)) {
            return i;
        }
    }
    if (arg0 == 5) {
        for (i = 9; i >= 0; i--) {
            p = &D_80062BB4[i];
            if ((u16)p->state - 0x14 < 2U && D_80062BB4[i].unk_6 == arg0 &&
                D_80062BB4[i].ref_count == 0) {
                return i;
            }
        }
    }
    return -12;
}

extern s32 g_TimerTicks;
extern Unk80013BD8Table D_80067950;

void *func_80013BD8(s32 arg0, s32 arg1, s32 arg2, s8 arg3) {
    s32 i;
    NsfPageSlot *p;

    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state == 1) {
            p->state = 2;
            p->unk_14 = arg0;
            p->ref_count = arg3;
            if (arg2 & 4) {
                p->unk_E = arg3;
            } else {
                p->unk_E = 0;
            }
            do {
                p->unk_18 = arg1;
                p->unk_28 = g_TimerTicks;
            } while (0);
            D_80067FC4[arg0 >> 1] = (s32)p;
            *(s32 *)((u8 *)&D_80067950 + 0x660) += 1;
            return p;
        }
    }
    return 0;
}

#ifdef NON_MATCHING
#define ERR_NO_SLOT (-0xC)
#define KEEP_FIELD 0x6396347F
#define FLAG_LAST 2
#define FLAG_MID 0xC

extern char D_80067588[];

NsfPageSlot *func_80013C98(NsfPageSlot *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4) {
    s32 idx;
    s32 count;
    s32 i;
    s32 base;
    NsfPageSlot *first;
    NsfPageSlot *dst;
    NsfPageSlot *src;

    idx = Nsf_EvictPageSlots(arg3, arg4);
    if (idx == -1) {
        return (NsfPageSlot *)ERR_NO_SLOT;
    }
    count = *arg3;
    /* D_80067588.page_slots[idx + count - 1] */
    dst = (NsfPageSlot *)(D_80067588 + (idx + count) * sizeof(NsfPageSlot));
    if (arg0 == 0) {
        return dst;
    }
    first = dst;
    i = count - 1;
    if (i >= 0) {
        base = arg0->unk_14 >> 1;
        do {
            src = (NsfPageSlot *)D_80067FC4[base + i];
            dst->unk_14 = src->unk_14;
            if (arg2 != KEEP_FIELD) {
                dst->unk_18 = arg2;
            } else {
                dst->unk_18 = src->unk_18;
            }
            src->state = 1;
            dst->ref_count = src->ref_count;
            src->ref_count = 0;
            dst->unk_E = src->unk_E;
            dst->unk_28 = src->unk_28;
            dst->unk_24 = first;
            dst->unk_1C = *arg3;
            if (i == *arg3 - 1) {
                dst->state = FLAG_LAST;
            } else {
                dst->state = FLAG_MID;
            }
            D_80067FC4[dst->unk_14 >> 1] = (s32)dst;
            dst--;
            i--;
        } while (i >= 0);
    }
    return first;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013C98);
#endif

extern s32 D_80067FB0[];

void Nsf_DropPageRef(NsfPageSlot *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = arg0->ref_count;
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        arg0->ref_count = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (arg0->unk_6 == 0)) {
            Nsf_FreePageSlot(arg0);
            D_80067FB0[0] -= 1;
        }
    }
}

void func_80013E7C(NsfPageSlot *arg0) {
    s8 t = arg0->unk_E;
    if (t != 0) {
        arg0->unk_E = t - 1;
        Nsf_DropPageRef(arg0);
    }
}

void func_80013EB0(NsfPageSlot *arg0) {
    s8 d = arg0->unk_E;

    if (d != 0) {
        arg0->ref_count = arg0->ref_count - d;
        arg0->unk_E = 0;
        if (arg0->ref_count <= 0 && arg0->unk_6 == 0) {
            Nsf_FreePageSlot(arg0);
            *D_80067FB0 -= 1;
        }
    }
}

void Nsf_FreePageSlot(NsfPageSlot *arg0) {
    arg0->state = 1;
    arg0->ref_count = 0;
    arg0->unk_E = 0;
    D_80067FC4[arg0->unk_14 >> 1] = -0x12;
}

#ifdef NON_MATCHING
#define SLOT_EMPTY -0x12
#define SLOT_RESERVED -0xFF
#define FLAG_ADJUST_E 4

u8 *func_80013F5C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    NsfPageSlot *entry;

    entry = (NsfPageSlot *)D_80067FC4[arg0 >> 1];
    if ((s32)entry == SLOT_EMPTY) {
        /* The retail call passes arg3 as a whole register, not narrowed to the definition's s8 */
        return ((u8 *(*)(s32, s32, s32, s32))func_80013BD8)(arg0, arg1, arg2, arg3);
    } else if ((s32)entry == SLOT_RESERVED) {
        return (u8 *)entry;
    } else {
        if (arg2 & FLAG_ADJUST_E) {
            entry->unk_E += arg3;
        }
        entry->unk_18 = arg1;
        entry->ref_count += arg3;
        return (u8 *)entry;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013F5C);
#endif

s8 func_80013FD8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    NsfPageSlot *p;

loop:
    p = (NsfPageSlot *)func_80013F5C(arg0, arg3, arg1, arg2);
    if (p == (NsfPageSlot *)-0xFF) {
        return 1;
    }
    if ((arg1 & 1) && (u32)((u16)p->state - 0x14) >= 2U) {
        Nsf_UpdatePaging(arg0);
        arg2 = 0;
        goto loop;
    }
    return p->ref_count;
}

#ifdef NON_MATCHING
s32 func_80014078(s32 arg0, s32 arg1) {
    NsfPageSlot *p;
    s32 i;

    p = (NsfPageSlot *)D_80067FC4[arg0 >> 1];
    if ((s32)p >= -0xFE) {
        return 0;
    }
    if ((s32)p == -0xFF || p->unk_6 == 3 || p->unk_6 == 5) {
        return 1;
    }
    if (arg1 < 0) {
        func_80013E7C(p);
    } else {
        for (i = 0; i < arg1; i++) {
            Nsf_DropPageRef(p);
        }
    }
    return p->ref_count + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014078);
#endif

#ifdef NON_MATCHING
#define HANDLE_FLAG_SLOT 0x2
#define HANDLE_TYPE_MASK 0xC0000002
#define HANDLE_TYPE_SCAN 0x80000002
#define SCAN_LAST_INDEX 15
#define KIND_SCAN_FIRST 0x14
#define KIND_SCAN_COUNT 2
#define HANDLE_INDEX_SHIFT 16
#define HANDLE_INDEX_BIAS 0xFFFF

extern Unk80013BD8Table D_800675B0;
extern NsfPageSlot D_80063000;
extern s32 D_800675B4;

NsfPageSlot *func_80014148(u32 arg0) {
    NsfPageSlot *e;
    s32 i;
    s32 d;
    s32 idx;

    if (arg0 & HANDLE_FLAG_SLOT) {
        if ((arg0 & HANDLE_TYPE_MASK) != HANDLE_TYPE_SCAN) {
            return 0;
        }
        e = &D_80063000;
        for (i = SCAN_LAST_INDEX; i >= 0; i--, e--) {
            if ((u32)((u16)e->state - KIND_SCAN_FIRST) < KIND_SCAN_COUNT &&
                arg0 == (u32)e->unk_1C) {
                return e;
            }
        }
        return 0;
    } else {
        d = arg0 - D_800675B4;
        if (d < 0) {
            d += HANDLE_INDEX_BIAS;
        }
        idx = d >> HANDLE_INDEX_SHIFT;
        if (idx < 0 || idx >= D_800675B0.count) {
            return 0;
        }
        return (NsfPageSlot *)((u8 *)&D_800675B4 + idx * sizeof(NsfPageSlot));
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014148);
#endif


NsfPageSlot *func_80014148(u32);

s32 func_80014220(u32 arg0) {
    NsfPageSlot *e;

    e = func_80014148(arg0);
    if (e && e->unk_6 == 1) {
        return (s32)e->page;
    }
    return 0;
}

#ifdef NON_MATCHING
void *Nsf_OpenEntry(void *a0, s32 arg1, s32 arg2) {
    u32 *arg0 = a0;
    u32 *p;
    u32 v;
    NsfPageSlot *e;

    p = arg0;
    if (!(arg1 & 2)) {
        v = *p;
        if (v == 0x6396347F) {
            return (void *)-0xA;
        }
        if (v & 1) {
            p = (u32 *)Nsf_LookupEid(v);
            *arg0 = (u32)p;
        } else {
            p = (u32 *)v;
        }
    }
    v = *p;
    if (!(v & 1)) {
        if (arg2 != 0) {
            e = func_80014148(v);
            if (e != 0) {
                func_80013FD8(e->unk_14, arg1, arg2, p[1]);
            }
        }
        return (void *)*p;
    }
    func_80013FD8(v, arg1, arg2, p[1]);
    v = *p;
    if (v & 1) {
        v = 0;
    }
    return (void *)v;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_OpenEntry);
#endif

s32 Nsf_CloseRecord(u32 arg0, s32 arg1) {
    NsfPageSlot *t;
    s32 a;
    s32 r;

    a = *(s32 *)arg0;
    if (!(a & 1)) {
        r = (s32)arg0;
        if (arg1 != 0) {
            t = func_80014148(a);
            if (t != 0) {
                a = t->unk_14;
                goto call;
            }
            return 1;
        }
        return r;
    }
    r = 0;
    if (arg1 != 0) {
call:
        r = func_80014078(a, arg1);
    }
    return r;
}

s32 Nsf_CloseEntry(void *a, s32 arg1) {
    u32 *arg0 = a;
    u32 v;
    u32 t;

    t = *arg0;
    if (t & 1) {
        v = (u32)Nsf_LookupEid(t);
        *arg0 = v;
    } else {
        v = t;
    }
    return Nsf_CloseRecord(v, arg1);
}

#ifdef NON_MATCHING
void func_80013EB0(NsfPageSlot *arg0);
extern Unk80013BD8Table D_800675B0;
extern NsfPageSlot D_80062D40;
extern NsfPageSlot D_80063000;


void func_80014420(void) {
    NsfPageSlot *e;
    NsfPageSlot *p;
    NsfPageSlot *q;
    s32 i;
    s32 pad[4];

    i = 0;
    if (D_80067950.count > 0) {
        e = D_80067950.e;
        q = e;
        do {
            p = q;
            if (p->state != 1) {
                func_80013EB0(p);
            }
            i++;
            q = p + 1;
        } while (i < ((s32 *)e)[-1]);
    }
    i = 0;
    if (D_800675B0.count > 0) {
        p = e = D_800675B0.e;
        do {
            if (p->state != 1) {
                func_80013EB0(p);
            }
            i++;
            p++;
        } while (i < ((s32 *)e)[-1]);
    }
    for (i = 15, p = &D_80063000; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0(p);
        }
    }
    for (i = 9, p = &D_80062D40; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0(p);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014420);
#endif

#ifdef NON_MATCHING
s32 Nsf_CountFreePages(void) {
    s32 i;
    s32 live;
    NsfPageSlot *p;

    live = D_800675B0.count;
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            live--;
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            live--;
        }
    }
    return live;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_CountFreePages);
#endif

extern Unk80013BD8Table D_800675B0;

void func_80014638(u32 *arg0) {
    s32 i;
    s32 bit;
    NsfPageSlot *p;

    for (i = 7; i >= 0; i--) {
        arg0[i] = 0;
    }
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            bit = p->unk_14 >> 1;
            /* The redundant self-assignment orders the load before the mask, as in the retail code. */
            arg0[bit / 32] = arg0[bit / 32];
            arg0[bit / 32] |= 1 << (bit % 32);
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            bit = p->unk_14 >> 1;
            arg0[bit / 32] = arg0[bit / 32];
            arg0[bit / 32] |= 1 << (bit % 32);
        }
    }
}

void func_80014790(u32 *arg0) {
    u32 word0;
    u32 word;
    s32 i;
    s32 bit;
    NsfPageSlot *p;
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            bit = p->unk_14 >> 1;
            word0 = arg0[bit / 32];
            if (!(word0 & (1 << (bit % 32)))) {
                func_80013EB0(p);
            }
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->ref_count != 0) {
            bit = p->unk_14 >> 1;
            word = arg0[bit / 32];
            if (!(word & (1 << (bit % 32)))) {
                func_80013EB0(p);
            }
        }
    }
}

#ifdef NON_MATCHING
s32 Nsf_CountPagesFor(u32 **arg0, s32 arg1) {
    s32 sp10[42];
    NsfPageSlot *e;
    s32 *out;
    s32 cnt;
    u32 **in;
    s32 *base;
    u32 *r;
    s32 n;
    s32 i;
    s32 j;
    s32 v;
    s32 w;

    n = 0;
    i = 0;
    cnt = 0;
    if (arg1 > 0) {
        base = sp10;
        out = base;
        in = arg0;
        do {
            r = (u32 *)**in;
            if ((u32)r & 1) {
                r = (u32 *)Nsf_LookupEid((u32)r);
                **in = (u32)r;
            }
            w = *r;
            if (!(w & 1)) {
                e = func_80014148(w);
                if (e != 0 && e->unk_6 != 2 && (e->state == 1 || e->ref_count == 0)) {
                    for (j = 0; j < n; j++) {
                        if (e->unk_14 == base[j]) break;
                    }
                    if (j == n) {
                        v = e->unk_14;
                        cnt++;
                        goto store;
                    }
                }
            } else {
                v = D_80067FC4[w >> 1];
                if (v == -0x12) {
                    for (j = 0; j < n; j++) {
                        if (w == base[j]) break;
                    }
                    if (j == n) {
                        v = *r;
                        cnt++;
                        goto store;
                    }
                } else if (v != -0xFF && ((NsfPageSlot *)v)->unk_6 < 2 && ((NsfPageSlot *)v)->ref_count == 0) {
                    for (j = 0; j < n; j++) {
                        if (((NsfPageSlot *)v)->unk_14 == base[j]) break;
                    }
                    if (j == n) {
                        v = ((NsfPageSlot *)v)->unk_14;
                        cnt++;
                        do { store: n++; *out = v; out++; } while (0);
                    }
                }
            }
            i++;
            in++;
        } while (i < arg1);
    }
    return cnt;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_CountPagesFor);
#endif

extern NsdRecord **D_80067590;

NsdRecord *Nsf_LookupEid(u32 eid) {
    NsdRecord **table;
    NsdRecord *p;

    table = D_80067590;
    do {
        p = table[(eid >> NSD_BUCKET_SHIFT) & NSD_BUCKET_MASK];
        while (p->eid != eid) {
            p++;
        }
    } while (0);
    return p;
}

#ifdef NON_MATCHING
#define TABLE_INDEX_SHIFT 13
#define TABLE_INDEX_MASK 0x3FC
#define ENTRY_SIZE_SHIFT 3
#define NOT_FOUND -10

extern s32 D_80067594;

s32 func_80014B90(s32 arg0, s32 arg1) {
    NsdRecord *p;
    s32 count;
    s32 base;

    /* The bucket by its byte offset: indexing D_80067590 with (EID >> 15) & 0xFF changes the code */
    p = *(NsdRecord **)((u8 *)D_80067590 + (((u32)arg0 >> TABLE_INDEX_SHIFT) & TABLE_INDEX_MASK));
    if (p->eid == arg0) {
        return (s32)p;
    }
    count = D_800675A0->record_count;
    base = D_80067594;
    p++;
    while ((((s32)p - base) >> ENTRY_SIZE_SHIFT) < count) {
        p++;
        if (p[-1].eid == arg0) {
            return (s32)(p - 1);
        }
    }
    return NOT_FOUND;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014B90);
#endif

s32 func_80014C08(void) {
    s32 temp_v0;

    temp_v0 = Heap_Alloc(4);
    if (temp_v0 != 0) {
        Heap_Free(temp_v0);
    }
    return temp_v0;
}

extern s32 D_8005F184;
extern s32 D_8005F188;

s32 func_80014C48(void) {
    s32 a = D_8005F184;
    s32 b = D_8005F188;

    /* The loop notes keep the scheduler from hoisting `lui 0x8000` above the loads. */
    do {
        a -= 0x80000000;
    } while (0);
    return b - a;
}

#ifdef NON_MATCHING
#define DISPATCH_COUNT 0x16
#define FILE_SIZE_BASE 0x714
#define FILE_SIZE_STRIDE 0x18
#define WORD_FILL -0x12
#define ADDR_MASK 0xFFFFFF
#define SLOT_COUNT 16
#define SLOT_NO_BUFFER 0x1E
#define SLOT_NO_BUFFER_TRY 0x15
#define SLOT_BUFFER 0x14
#define SLOT_IDLE 2
#define SLOT_ACTIVE_STATE 1
#define NO_ENTRY_LIMIT -0xFE
#define RET_DONE -0xFF
#define SLOT_QUAD_SPLIT 4
#define SLOT_HALF 8
#define CMD_FLAG_HI 0x10
#define CMD_FLAG_ODD 0x80
#define CMD_BASE 0x80000002
#define LEVEL_9 9
#define LEVEL_3C 0x3C
#define SCALE_DEFAULT 0x10000
#define SCALE_HALF 0x8000
#define SCALE_3C 0xEA60
#define SCALE_UNSET -1
#define SPAN_MUL 0xFF90
#define SPAN_BASE 0x1110
#define SPAN_3C 0x3EE40
#define SPAN_ARG 0xE

typedef struct Unk8005B1E0 {
    void (*first)(void);
    void (*second)(void);
    u8 pad08[0x14];
} Unk8005B1E0;

typedef char *(*NameLookupFn)(s32, s32);
/* The declared prototype returns void; the callee leaves a string pointer in v0. */
#define func_8001214C_str(a, b) (((NameLookupFn)Nsf_FileName)((a), (b)))
extern int printf(const char *, ...);
extern char D_80010110[];
extern Unk8005B1E0 D_8005B1E0[];
extern s32 g_Player;
extern s32 D_8005F3EC;
extern s32 g_LevelRequest;
extern s32 D_8006903C[];

s32 Nsf_OpenLevel(NsfLevel *arg0, s32 arg1) {
    char name[0x100];
    s32 i;
    s32 count;
    Nsd *nsd;
    NsdRecord **p;
    s32 a0;
    s32 a2;
    s32 cmd;
    s32 ent;
    s32 mapped;
    s32 hi;
    s32 lo;
    s32 scale;
    s32 mode;
    s32 span;
    s32 num;
    NsfPageSlot *e;

    if (arg0->is_open != 0) {
        return RET_DONE;
    }
    g_LevelRequest = -1;
    g_Player = 0;
    D_8005F3EC = arg1;
    arg0->is_open = 1;
    arg0->unk_A2C = 0;
    arg0->unk_A28 = 0;
    arg0->unk_A34 = 0;
    arg0->level = arg1;
    arg0->unk_A38 = 0;
    arg0->unk_A40 = 1;
    strcpy(name, func_8001214C_str(0, arg1));
    if (g_UseCd != 0) {
        a0 = (s32)&g_LevelFiles[arg1 * 3];
        a2 = 1;
    } else {
        a0 = (s32)name;
        a2 = 0;
    }
    arg0->nsd = (Nsd *)Cd_ReadFile((s32 *)a0, 0, a2);
    nsd = arg0->nsd;
    arg0->unk_10 = arg0->nsd->unk_408;
    arg0->buckets = arg0->nsd->buckets;
    arg0->records = arg0->nsd->records;
    /* &nsd->records[nsd->record_count] adds the offset first and changes the code */
    arg0->unk_14 = (u8 *)nsd + nsd->record_count * sizeof(NsdRecord) + NSD_RECORDS_OFFSET;
    nsd = arg0->nsd;
    arg0->nsd = (Nsd *)Heap_Realloc((s32)nsd, nsd->record_count * sizeof(NsdRecord) +
                                                  (*(s32 *)arg0->unk_14 - 1) * FILE_SIZE_STRIDE + FILE_SIZE_BASE);
    i = 0;
    do {
        p = &arg0->buckets[i];
        i++;
        *p = &arg0->records[(s32)*p];
    } while (i < NSD_BUCKET_COUNT);
    strcpy(name, func_8001214C_str(1, arg1));
    if (g_UseCd != 0) {
        g_NsfStartSector = D_8006903C[arg1 * 3] & ADDR_MASK;
    }
    arg0->unk_A3C = (s32 *)Heap_Alloc(arg0->nsd->page_count * 4);
    i = 0;
    if (arg0->nsd->page_count > 0) {
        do {
            arg0->unk_A3C[i] = WORD_FILL;
            i++;
        } while (i < arg0->nsd->page_count);
    }
    for (i = 0; i < DISPATCH_COUNT; i++) {
        if (D_8005B1E0[i].first != 0) {
            D_8005B1E0[i].first();
        }
    }
    arg0->page_slot_count = NSF_LEVEL_PAGE_SLOTS;
    count = NSF_LEVEL_PAGE_SLOTS;
    do {
        arg0->page_slots[0].page = (NsfPage *)Heap_Alloc(count << NSF_PAGE_SHIFT);
        if (arg0->page_slots[0].page != 0) {
            break;
        }
        count--;
    } while (count >= 0);
    arg0->page_slot_count = count;
    num = 0;
    i = 0;
    e = arg0->page_slots;
    if (arg0->page_slot_count > 0) {
        do {
            e->unk_6 = 1;
            e->unk_C = i;
            e->ref_count = 0;
            e->state = 1;
            e->page = (NsfPage *)((u8 *)arg0->page_slots[0].page + (i << NSF_PAGE_SHIFT));
            num++;
            i++;
            e++;
        } while (i < arg0->page_slot_count);
    }
    arg0->unk_3C8 = NSF_LEVEL_UNK_SLOTS;
    i = 0;
    e = arg0->unk_3CC;
    do {
        e->state = 1;
        e->unk_6 = 0;
        e->page = 0;
        e->ref_count = 0;
        i++;
        e++;
    } while (i < arg0->unk_3C8);
    printf(D_80010110, num, func_800120EC(arg1) & 0xFF);
    i = 0;
    do {
        NsfPageSlot *slot = &D_80062D6C[i];

        slot->unk_6 = SLOT_IDLE;
        hi = i >> 2;
        lo = i - hi * 4;
        cmd = lo * 4;
        if (i >= SLOT_HALF) {
            cmd |= CMD_FLAG_HI;
        }
        if (hi & 1) {
            cmd |= (lo << 8) | (hi << 17) | CMD_FLAG_ODD;
        } else {
            cmd |= (lo << 8) | (hi << 17);
        }
        cmd |= CMD_BASE;
        slot->unk_1C = cmd;
        mapped = SLOT_NO_BUFFER;
        if ((u32)i >= SLOT_QUAD_SPLIT && (u32)(i - SLOT_QUAD_SPLIT) >= SLOT_QUAD_SPLIT) {
            mapped = SLOT_ACTIVE_STATE;
            if ((u32)((u16)slot->state - SLOT_BUFFER) < 2) {
                ent = func_80014B90(D_80062D84[i].v, hi);
                if (ent < NO_ENTRY_LIMIT) {
                    D_80062D70[i].state = SLOT_BUFFER;
                    D_80062D80[i].v = *(s32 *)ent;
                    D_80062D8C[i].v = (s32 *)ent;
                    slot->ref_count = 0;
                    slot->unk_E = 0;
                    D_80067FC4[D_80062D80[i].v >> 1] = (s32)slot;
                    *(s32 *)ent = slot->unk_1C;
                    continue;
                }
                D_80062D70[i].state = SLOT_NO_BUFFER_TRY;
                slot->ref_count = 0;
                slot->unk_E = 0;
                continue;
            }
        }
        D_80062D70[i].state = mapped;
    } while (++i < SLOT_COUNT);
    scale = SCALE_DEFAULT;
    mode = SCALE_UNSET;
    span = Nsf_CountEntriesOfType(arg0, SPAN_ARG) * SPAN_MUL - SPAN_BASE;
    switch (arg1) {
    case LEVEL_9:
        scale = SCALE_HALF;
        mode = 2;
        break;
    case 2:
    case 7:
    case 0x1C:
    case 0x28:
    case 0x29:
    case 0x3B:
        scale = SCALE_DEFAULT;
        mode = 2;
        break;
    case LEVEL_3C:
        scale = SCALE_3C;
        mode = 1;
        span = SPAN_3C;
        break;
    default:
        break;
    }
    func_80015344(arg0, 0, span, scale, mode);
    for (i = 0; i < DISPATCH_COUNT; i++) {
        if (D_8005B1E0[i].second != 0) {
            D_8005B1E0[i].second();
        }
    }
    return RET_DONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", Nsf_OpenLevel);
#endif

extern void func_80051328(int);
typedef struct { void (*fn)(); char pad[24]; } Entry;
extern Entry D_8005B1F0[];
extern int D_80067FB4[];
extern int D_80067FBC[];

s32 Nsf_CloseLevel(NsfLevel *p) {
    int i;
    int off;
    char buf[8];
    if (p->is_open == 1) {
        if (g_UseCd != 0 && D_80067FB4[0] != 0) Cd_SyncRead(1);
        if (D_80067FBC[0] != 0) func_80051328(1);
        i = 0;
        Snd_ShutdownSequencer();
        off = 0;
        Snd_Shutdown();
        do {
            void (*f)() = *(void (**)())((char *)D_8005B1F0 + off);
            if (f != 0) f();
            i++;
            off += 0x1C;
        } while (i < 22);
        i = 0;
        if (p->page_slot_count > 0) {
            do {
                func_800155E0(p, i);
                i++;
            } while (i < p->page_slot_count);
        }
        Heap_Free((s32)p->page_slots[0].page);
        Heap_Free((s32)p->unk_A3C);
        Heap_Free((s32)p->nsd);
        i = -0xFF;
        p->is_open = 0;
        return i;
    }
    return -0xFF;
}

#ifdef NON_MATCHING
#define ROW_STRIDE 65424
#define ROW_STRIDE_ROUND 65423
#define ROW_BASE 0x80000
#define ROW_SNAP 16
#define ROW_SNAP_ROUND 15
#define FILL_COUNT 10
#define FILL_MIN_Y 0x1010
#define FILL_DROP 0x10000
#define KIND_FIRST 4
#define KIND_EXTRA 5
#define KIND_FILL 3
#define MODE_ACTIVE 1
#define MODE_SKIPPED 0x1E
#define EXTRA_VALUE 0x6396347F

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 unk_0C[2];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C[4];
} Unk80015344;

extern Unk80015344 D_80062BB8[];
extern s32 D_80067FB8;

void func_80015344(s32 unused, s32 y, s32 total, s32 step, s32 count) {
    s32 i;
    s32 rows;
    s32 last;
    s32 end;
    s32 saved;
    s32 next;
    s32 rem;

    y = ROW_BASE - y;
    rows = (total + ROW_STRIDE_ROUND) / ROW_STRIDE;
    last = rows - 1;
    rem = total % ROW_STRIDE;
    for (i = last; i >= 0; i--) {
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_00 = MODE_ACTIVE;
        D_80062BB8[i].unk_02 = KIND_FIRST;
        if (i == last) {
            y -= ((rem + ROW_SNAP_ROUND) / ROW_SNAP) * ROW_SNAP;
        } else {
            y -= ROW_STRIDE;
        }
        D_80062BB8[i].unk_18 = y;
    }
    D_80067FB8 = y;
    saved = y;
    i = rows;
    if (count < 0) {
        end = rows - count;
    } else {
        end = rows + count;
    }
    for (; i < end; i++) {
        y -= step;
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_00 = MODE_ACTIVE;
        D_80062BB8[i].unk_02 = KIND_EXTRA;
        D_80062BB8[i].unk_14 = EXTRA_VALUE;
        D_80062BB8[i].unk_18 = y;
    }
    if (count < 0) {
        y = saved;
    } else {
        /* y carries on from the last extra row */
    }
    for (; i < FILL_COUNT; i++) {
        next = y - FILL_DROP;
        D_80062BB8[i].unk_08 = i;
        D_80062BB8[i].unk_02 = KIND_FILL;
        if (next < FILL_MIN_Y) {
            D_80062BB8[i].unk_00 = MODE_SKIPPED;
        } else {
            y = next;
            D_80062BB8[i].unk_00 = MODE_ACTIVE;
            D_80062BB8[i].unk_18 = y;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80015344);
#endif

void *func_8001557C(s32 arg0) {
    s32 i;

    for (i = 0; i < 10; i++) {
        if (D_80062BB4[i].unk_6 == 5 && D_80062BB4[i].unk_18 == arg0) {
            return &D_80062BB4[i];
        }
    }
    return 0;
}

void func_800155E0(NsfLevel *arg0, s32 arg1) {
    if (arg0->page_slots[arg1].state != 0) {
        arg0->page_slots[arg1].state = 0;
    }
}
