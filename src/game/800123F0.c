#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_800100D0);
INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_800100F0);
INCLUDE_RODATA("asm/nonmatchings/game/800123F0", D_80010110);

s32 D_8005F1A4 = 1;
char D_8005F1A8[] = "hde\n";
RECT D_8005F1B0 = { 0, 0, 0x100, 0x80 };
char D_8005F1B8[] = "de ls\n";
char D_8005F1C0[] = "de l\n";
char D_8005F1C8[] = "!mem\n";
s32 D_8005F1D0 = 0;

#ifdef NON_MATCHING
extern char D_8005F1A8[];
int CdIntToPos(int, unsigned char *);
int CdRead(int, s32, int);
int printf(const char *, ...);

s32 func_800123F0(s32 *a0, s32 arg1, s32 arg2) {
    u8 sp10[8];
    u8 sp18[8];
    s32 s2;
    s32 *arg0;
    s32 ret;

    ret = D_8005F1A4;
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
        arg1 = func_8001148C(s2);
        if (arg1 == 0) {
            return -0xF;
        }
    }
loop:
    CdIntToPos(arg0[0] & 0x7FFFFF, sp18);
    if (CdControl(2, sp18, sp10) == 0) {
        goto loop;
    }
    if (CdRead((u32)(((arg0[1] & 0x1FFFF) + 0x7FF) / 2048), arg1, 0x80) == 0 || func_80012ED0(1) == -0x10) {
        printf(D_8005F1A8);
        goto loop;
    }
    if (s2 != 0 && s2 != (arg0[1] & 0x1FFFF)) {
        arg1 = func_800116D4(arg1, arg0[1] & 0x1FFFF);
    }
    ret = arg1;
end:
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800123F0);
#endif

s32 func_8001254C(u8 *base) {
    s32 count;
    s32 i;
    s32 j;
    u32 *table;
    u32 *p;
    u32 *q;
    u8 *entry;
    u32 *data;

    table = (u32 *)(base + 0x10);
    count = *(s32 *)(base + 8);
    if (*(u16 *)(base + 2) == 1) {
        return -0xFF;
    }
    if (*(u16 *)base != 0x1234) {
        return -0x12;
    }
    i = count - 1;
    if (i >= 0) {
        p = table - (-i);
        do {
            entry = base + *p;
            *p = (u32)entry;
            j = *(s32 *)(entry + 0xC);
            data = (u32 *)(entry + 0x10);
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
    table[count] = (u32)base + table[count];
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800125F8);

#ifdef NON_MATCHING
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
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012A20);
#endif

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 f_e;
    s8 f_f;
    u8 pad10[4];
    s32 f_14;
    s32 f_18;
    u8 pad1c[0xC];
    s32 f_28;
} Unk80013BD8;
typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;

#ifdef NON_MATCHING
#define CHANNEL_TABLE_LIMIT_OFFSET 0x400
#define SLOT_STATE_IN_USE 2
#define SLOT_KEY_ANY -1
#define CHANNEL_MARKER_MAX -0xFE
#define CALL_MAGIC 0x6396347F

extern s32 D_80067FB0[];
extern Unk80013BD8Table D_80067950;
extern Unk80013BD8Table D_800675B0;
extern u8 *D_800675A0;
void func_80013200(Unk80013BD8 *, s32);

void func_80012BC4(s32 arg0) {
    s32 sp18;
    s32 key;
    s32 found;
    s32 i;
    s32 v;
    s32 lim;
    s32 idx;
    s32 nxt;
    s32 *tbl;
    Unk80013BD8 *p;

    if (D_80067FB0[0] != 0) {
        found = -1;
        if (D_80067FB4[0] == 0) {
            for (i = 0; i < D_80067950.count; i++) {
                p = &D_80067950.e[i];
                if (p->state == SLOT_STATE_IN_USE) {
                    v = p->f_14;
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
                if (D_8005F1A4 != 0) {
                    idx = key >> 1;
                    lim = *(s32 *)(D_800675A0 + CHANNEL_TABLE_LIMIT_OFFSET) - 1;
                    if (idx < lim) {
                        tbl = D_80067FC4;
                        nxt = idx + 1;
                        do {
                            idx = (nxt * 2) >> 1;
                            v = tbl[idx];
                            if (v >= CHANNEL_MARKER_MAX) {
                                break;
                            }
                            if (((s16 *)v)[3] != 0) {
                                break;
                            }
                            sp18++;
                            nxt = idx + 1;
                        } while (idx < lim);
                    }
                }
                p = &D_80067950.e[found];
                func_80013C98(p, p->f_14, CALL_MAGIC, &sp18, ~arg0 != 0);
            }
        }
        for (i = 0; i < D_800675B0.count; i++) {
            func_80013200(&D_800675B0.e[i], arg0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012BC4);
#endif

extern s32 D_80067FB0[];
extern s32 D_80067FC8;
extern s16 D_8005F3E0;
extern u8 *D_800608CC;

void func_80012DC0(void) {
    D_80067FC8 = 1;
    while (*D_80067FB0 != 0) {
        if (D_800608CC != 0 && D_8005F3E0 != -1) {
            func_80034254(*(s32 *)(D_800608CC + 0x10) + 0x2A4);
        }
        func_80012BC4(-1);
    }
}

extern int D_80067FB4[];
extern int D_80069034[];

void func_80012E4C(s32 arg0) {
    char sp10[8];
    int v;

    if (D_8005F1A4 != 0 && D_80067FB4[0] == 0 && CdSync(1, 0) == 0) {
        v = D_80069034[v = arg0 * 3];
        v = 0x7FFFFF & v;
        CdIntToPos(v, sp10);
        CdControlF(0x15, sp10);
    }
}

s32 func_80012ED0(s32 wait) {
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
#define UNK_OFFSET_SECTOR_BASE 0x418
#define UNK_OFFSET_SECTOR_TABLE 0x420
#define SECTOR_TABLE_SHIFT 6
#define SECTORS_SHIFT 5
#define WORD_SIZE_SHIFT 2
#define SHIFT_2K 11
#define CD_READ_SECTORS 0x80
#define CD_CMD_SEEKL 2
#define CD_SYNC_POLL 1
#define CD_SYNC_BLOCK 0
#define CD_STATUS_DISK_ERROR 5
#define LOAD_RESULT -0xFF

extern u8 *D_800675A0;
extern s32 D_800675A8;

s32 CdControlF(s32 cmd, u8 *param);
s32 CdSync(s32 mode, u8 *result);

s32 func_80012F24(u8 *arg0, s32 arg1) {
    u8 pos[16];
    s32 sector;
    s32 count;
    s32 buf;
    s32 index;
    s32 poll;
    s32 block;

    index = *(s32 *)(arg0 + 0x14);
    if (*(s16 *)(arg0 + 0xA) != 0) {
        s32 off = *(s16 *)(arg0 + 8);

        count = (arg1 << SECTORS_SHIFT) - off;
        buf = *(s32 *)arg0 + (off << SHIFT_2K);
        sector = D_800675A8 +
                 (*(u32 *)(D_800675A0 + (index >> 1) * (1 << WORD_SIZE_SHIFT) + UNK_OFFSET_SECTOR_TABLE) >> SECTOR_TABLE_SHIFT);
    } else {
        count = arg1 << SECTORS_SHIFT;
        buf = *(s32 *)arg0;
        sector = D_800675A8 + *(s32 *)(D_800675A0 + UNK_OFFSET_SECTOR_BASE) + ((index >> 1) << SECTORS_SHIFT);
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
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012F24);
#endif

typedef struct {
    u8 pad0[0x14];
    s32 unk14;
} Unk80013038Sub;

typedef struct {
    u8 pad0[0x1C];
    s32 unk1C;
    u8 pad20[0x4];
    Unk80013038Sub *unk24;
} Unk80013038;

s32 func_80013038(Unk80013038 *arg0, s32 arg1) {
    s32 temp;

    if (arg1 == -1) {
        return 1;
    }
    arg1 >>= 1;
    temp = arg0->unk24->unk14 >> 1;
    if (temp >= arg1) {
        if (temp - arg0->unk1C < arg1) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013084);

typedef struct Unk800130A0 {
    /* 0x00 */ u8 *base;
    /* 0x04 */ s16 state;
    /* 0x06 */ s16 pad6;
    /* 0x08 */ s16 offset;
    /* 0x0A */ s16 flag;
    /* 0x0C */ s32 pad0C;
    /* 0x10 */ u16 *dest;
    /* 0x14 */ s32 index;
    /* 0x18 */ s32 pad18;
    /* 0x1C */ s32 count;
    /* 0x20 */ s32 pad20[3];
    /* 0x2C */ u16 *alt;
} Unk800130A0;

extern u8 *D_800675A0;

Unk800130A0 *func_800130A0(Unk800130A0 *arg0) {
    Unk800130A0 *p;
    s16 off;
    s32 state;
    s32 count;
    s32 i;
    s32 flag;
    Unk800130A0 *last;

    p = arg0;
    state = p->state;
    count = p->count;
    off = 0;
    if (state == 3 || state == 13) {
        p->state = 4;
        func_8001254C(p->base);
        return 0;
    }
    flag = 0;
    if ((p->index >> 1) < *(s32 *)(D_800675A0 + 0x41C)) {
        if (p) {
            flag = D_80067FC8 != 0;
        } else {
            flag = D_80067FC8 != 0;
        }
    }
    for (i = 0; i < count; i++) {
        if (state == 2) {
            p->flag = flag;
            if (flag != 0) {
                off = off + 32;
                off -= *(s32 *)(D_800675A0 + (p->index >> 1) * 4 + 0x420) & 0x3F;
                p->offset = off;
            }
            if (p->state == state) {
                p->state = 3;
            } else {
                p->state = 13;
                if (flag != 0) {
                    p->dest = (u16 *)(p->base + (((*(s32 *)(D_800675A0 + (p->index >> 1) * 4 + 0x420) & 0x3F) + p->offset) << 11));
                } else {
                    p->dest = p->alt;
                }
                *p->dest = 0x8765;
            }
        }
        last = p;
        p = (Unk800130A0 *)((u8 *)p - 0x2C);
    }
    return last;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013200);

#ifdef NON_MATCHING
#define SLOT_FREE 1
#define SLOT_STATE_FIRST_RELEASABLE 0x14
#define SLOT_STATE_RELEASABLE_COUNT 2
#define NO_RUN -1

extern Unk80013BD8Table D_800675B0;
extern char D_8005F1C8[];
s32 func_80012A20(Unk80012A20Obj *arg0);

s32 func_800136C8(s32 *arg0, s32 arg1) {
    s32 i, j, n, t, best, bestLen;
    u32 cost, bestCost;
    Unk80013BD8 *p;
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
                if ((u16)(p->state - SLOT_STATE_FIRST_RELEASABLE) < SLOT_STATE_RELEASABLE_COUNT && p->f_f <= 0) {
                    cost = cost + 1 + p->f_28;
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
            func_80012A20((Unk80012A20Obj *)&D_800675B0.e[j]);
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
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800136C8);
#endif

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
                func_80013968(i);
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

s32 func_80013968(s32 arg0) {
    s16 st = D_80062D70[arg0].state;

    if (st != 0x14) {
        if (st != 0x15) {
            return -0xE;
        }
        goto tail;
    }
    func_800170B4(D_80062D84[arg0].v, 0);
    *D_80062D8C[arg0].v = D_80062D80[arg0].v;
    D_80067FC4[D_80062D80[arg0].v >> 1] = -0x12;
tail:
    D_80062D70[arg0].state = 1;
    return -0xE;
}


void func_80013A54(s32 arg0, s32 arg1) {
    if (arg1 != 0) {
        func_80013968(arg0);
        D_80062D70[arg0].state = 0x1E;
    } else if (D_80062D70[arg0].state == 0x1E) {
        D_80062D70[arg0].state = 1;
    }
}

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

s32 func_80013AE8(s32 arg0, s32 arg1) {
    s32 i;
    Unk80062BB4 *p;

    for (i = 9; i >= 0; i--) {
        p = &D_80062BB4[i];
        if (p->unk4 == 1 && D_80062BB4[i].unk6 == arg0 &&
            (arg1 == -1 || arg1 == D_80062BB4[i].unkC)) {
            return i;
        }
    }
    if (arg0 == 5) {
        for (i = 9; i >= 0; i--) {
            p = &D_80062BB4[i];
            if ((u16)p->unk4 - 0x14 < 2U && D_80062BB4[i].unk6 == arg0 &&
                D_80062BB4[i].unkF == 0) {
                return i;
            }
        }
    }
    return -12;
}

extern s32 D_8003BEA4;
extern Unk80013BD8Table D_80067950;

void *func_80013BD8(s32 arg0, s32 arg1, s32 arg2, s8 arg3) {
    s32 i;
    Unk80013BD8 *p;

    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state == 1) {
            p->state = 2;
            p->f_14 = arg0;
            p->f_f = arg3;
            if (arg2 & 4) {
                p->f_e = arg3;
            } else {
                p->f_e = 0;
            }
            do {
                p->f_18 = arg1;
                p->f_28 = D_8003BEA4;
            } while (0);
            D_80067FC4[arg0 >> 1] = (s32)p;
            *(s32 *)((u8 *)&D_80067950 + 0x660) += 1;
            return p;
        }
    }
    return 0;
}

#ifdef NON_MATCHING
#define SLOT_SIZE 0x2C
#define ERR_NO_SLOT (-0xC)
#define KEEP_FIELD 0x6396347F
#define FLAG_LAST 2
#define FLAG_MID 0xC

extern char D_80067588[];

void *func_80013C98(void *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4) {
    s32 idx;
    s32 count;
    s32 i;
    s32 base;
    char *first;
    char *dst;
    char *src;

    idx = func_800136C8(arg3, arg4);
    if (idx == -1) {
        return (void *)ERR_NO_SLOT;
    }
    count = *arg3;
    dst = D_80067588 + (idx + count) * SLOT_SIZE;
    if (arg0 == 0) {
        return dst;
    }
    first = dst;
    i = count - 1;
    if (i >= 0) {
        base = *(s32 *)((char *)arg0 + 0x14) >> 1;
        do {
            src = (char *)D_80067FC4[base + i];
            *(s32 *)(dst + 0x14) = *(s32 *)(src + 0x14);
            if (arg2 != KEEP_FIELD) {
                *(s32 *)(dst + 0x18) = arg2;
            } else {
                *(s32 *)(dst + 0x18) = *(s32 *)(src + 0x18);
            }
            *(s16 *)(src + 4) = 1;
            *(u8 *)(dst + 0xF) = *(u8 *)(src + 0xF);
            *(u8 *)(src + 0xF) = 0;
            *(u8 *)(dst + 0xE) = *(u8 *)(src + 0xE);
            *(s32 *)(dst + 0x28) = *(s32 *)(src + 0x28);
            *(char **)(dst + 0x24) = first;
            *(s32 *)(dst + 0x1C) = *arg3;
            if (i == *arg3 - 1) {
                *(s16 *)(dst + 4) = FLAG_LAST;
            } else {
                *(s16 *)(dst + 4) = FLAG_MID;
            }
            D_80067FC4[*(s32 *)(dst + 0x14) >> 1] = (s32)dst;
            dst -= SLOT_SIZE;
            i--;
        } while (i >= 0);
    }
    return first;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013C98);
#endif

extern s32 D_80067FB0[];

void func_80013E10(void *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = *(s8 *)((char *)arg0 + 0xF);
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        *(s8 *)((char *)arg0 + 0xF) = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (*(s16 *)((char *)arg0 + 6) == 0)) {
            func_80013F28(arg0);
            D_80067FB0[0] -= 1;
        }
    }
}

void func_80013E7C(s8 *arg0) {
    s8 t = arg0[0xE];
    if (t != 0) {
        arg0[0xE] = t - 1;
        func_80013E10(arg0);
    }
}

typedef struct {
    u8 pad0[6];
    s16 f6;
    u8 pad8[6];
    s8 fE;
    s8 fF;
} Unk80013EB0;


void func_80013EB0(Unk80013EB0 *arg0) {
    s8 d = arg0->fE;

    if (d != 0) {
        arg0->fF = arg0->fF - d;
        arg0->fE = 0;
        if (arg0->fF <= 0 && arg0->f6 == 0) {
            func_80013F28(arg0);
            *D_80067FB0 -= 1;
        }
    }
}

void func_80013F28(void *arg0) {
    *(s16 *)((u8 *)arg0 + 4) = 1;
    *((u8 *)arg0 + 0xF) = 0;
    *((u8 *)arg0 + 0xE) = 0;
    D_80067FC4[*(s32 *)((u8 *)arg0 + 0x14) >> 1] = -0x12;
}

#ifdef NON_MATCHING
#define SLOT_EMPTY -0x12
#define SLOT_RESERVED -0xFF
#define FLAG_ADJUST_E 4

typedef struct Unk80013F5C {
    /* 0x00 */ u8 unk_00[0xE];
    /* 0x0E */ u8 unk_0E;
    /* 0x0F */ u8 unk_0F;
    /* 0x10 */ u8 unk_10[8];
    /* 0x18 */ s32 unk_18;
} Unk80013F5C;

u8 *func_80013F5C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Unk80013F5C *entry;

    entry = (Unk80013F5C *)D_80067FC4[arg0 >> 1];
    if ((s32)entry == SLOT_EMPTY) {
        return func_80013BD8(arg0, arg1, arg2, arg3);
    } else if ((s32)entry == SLOT_RESERVED) {
        return (u8 *)entry;
    } else {
        if (arg2 & FLAG_ADJUST_E) {
            entry->unk_0E += arg3;
        }
        entry->unk_18 = arg1;
        entry->unk_0F += arg3;
        return (u8 *)entry;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013F5C);
#endif

s8 func_80013FD8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *p;

loop:
    p = func_80013F5C(arg0, arg3, arg1, arg2);
    if (p == (u8 *)-0xFF) {
        return 1;
    }
    if ((arg1 & 1) && (u32)(*(u16 *)(p + 4) - 0x14) >= 2U) {
        func_80012BC4(arg0);
        arg2 = 0;
        goto loop;
    }
    return *(s8 *)(p + 0xF);
}

#ifdef NON_MATCHING
typedef struct Unk80014078 {
    s8 pad0[6];
    s16 f6;
    s8 pad8[7];
    s8 fF;
} Unk80014078;

s32 func_80014078(s32 arg0, s32 arg1) {
    Unk80014078 *p;
    s32 i;

    p = (Unk80014078 *)D_80067FC4[arg0 >> 1];
    if ((s32)p >= -0xFE) {
        return 0;
    }
    if ((s32)p == -0xFF || p->f6 == 3 || p->f6 == 5) {
        return 1;
    }
    if (arg1 < 0) {
        func_80013E7C((s8 *)p);
    } else {
        for (i = 0; i < arg1; i++) {
            func_80013E10(p);
        }
    }
    return p->fF + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014078);
#endif

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Unk80014148;

#ifdef NON_MATCHING
#define HANDLE_FLAG_SLOT 0x2
#define HANDLE_TYPE_MASK 0xC0000002
#define HANDLE_TYPE_SCAN 0x80000002
#define SCAN_LAST_INDEX 15
#define KIND_SCAN_FIRST 0x14
#define KIND_SCAN_COUNT 2
#define HANDLE_INDEX_SHIFT 16
#define HANDLE_INDEX_BIAS 0xFFFF
#define ENTRY_HANDLE_OFFSET 0x1C


Unk80014148 *func_80014148(u32);

extern Unk80013BD8Table D_800675B0;
extern Unk80013BD8 D_80063000;
extern s32 D_800675B4;

Unk80014148 *func_80014148(u32 arg0) {
    Unk80013BD8 *e;
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
                arg0 == *(u32 *)((u8 *)e + ENTRY_HANDLE_OFFSET)) {
                return (Unk80014148 *)e;
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
        return (Unk80014148 *)((u8 *)&D_800675B4 + idx * sizeof(Unk80013BD8));
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014148);
#endif


Unk80014148 *func_80014148(u32);

s32 func_80014220(u32 arg0) {
    Unk80014148 *e;

    e = func_80014148(arg0);
    if (e && e->kind == 1) {
        return e->value;
    }
    return 0;
}

#ifdef NON_MATCHING
void *func_80014260(void *a0, s32 arg1, s32 arg2) {
    u32 *arg0 = a0;
    u32 *p;
    u32 v;
    Unk80014148 *e;

    p = arg0;
    if (!(arg1 & 2)) {
        v = *p;
        if (v == 0x6396347F) {
            return (void *)-0xA;
        }
        if (v & 1) {
            p = (u32 *)func_80014B44(v);
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
                func_80013FD8(*(s32 *)((u8 *)e + 0x14), arg1, arg2, p[1]);
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
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014260);
#endif

s32 func_8001434C(u32 arg0, s32 arg1) {
    Unk80014148 *t;
    s32 a;
    s32 r;

    a = *(s32 *)arg0;
    if (!(a & 1)) {
        r = (s32)arg0;
        if (arg1 != 0) {
            t = func_80014148(a);
            if (t != 0) {
                a = *(s32 *)((u8 *)t + 0x14);
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

struct Unk80014B44 *func_80014B44(u32);

s32 func_800143C0(void *a, s32 arg1) {
    u32 *arg0 = a;
    u32 v;
    u32 t;

    t = *arg0;
    if (t & 1) {
        v = (u32)func_80014B44(t);
        *arg0 = v;
    } else {
        v = t;
    }
    return func_8001434C(v, arg1);
}

#ifdef NON_MATCHING
void func_80013EB0(Unk80013EB0 *arg0);
extern Unk80013BD8Table D_800675B0;
extern Unk80013BD8 D_80062D40;
extern Unk80013BD8 D_80063000;


void func_80014420(void) {
    Unk80013BD8 *e;
    Unk80013BD8 *p;
    Unk80013BD8 *q;
    s32 i;
    s32 pad[4];

    i = 0;
    if (D_80067950.count > 0) {
        e = D_80067950.e;
        q = e;
        do {
            p = q;
            if (p->state != 1) {
                func_80013EB0((Unk80013EB0 *)p);
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
                func_80013EB0((Unk80013EB0 *)p);
            }
            i++;
            p++;
        } while (i < ((s32 *)e)[-1]);
    }
    for (i = 15, p = &D_80063000; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0((Unk80013EB0 *)p);
        }
    }
    for (i = 9, p = &D_80062D40; i >= 0; i--, p--) {
        if ((u16)(*(u16 *)&p->state - 0x14) < 2) {
            func_80013EB0((Unk80013EB0 *)p);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014420);
#endif

#ifdef NON_MATCHING
s32 func_8001456C(void) {
    s32 i;
    s32 live;
    Unk80013BD8 *p;

    live = D_800675B0.count;
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->f_f != 0) {
            live--;
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->f_f != 0) {
            live--;
        }
    }
    return live;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001456C);
#endif

extern Unk80013BD8Table D_800675B0;

void func_80014638(u32 *arg0) {
    s32 i;
    s32 bit;
    Unk80013BD8 *p;

    for (i = 7; i >= 0; i--) {
        arg0[i] = 0;
    }
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
            /* The redundant self-assignment orders the load before the mask, as in the retail code. */
            arg0[bit / 32] = arg0[bit / 32];
            arg0[bit / 32] |= 1 << (bit % 32);
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
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
    Unk80013BD8 *p;
    for (i = 0; i < D_80067950.count; i++) {
        p = &D_80067950.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
            word0 = arg0[bit / 32];
            if (!(word0 & (1 << (bit % 32)))) {
                func_80013EB0((Unk80013EB0 *)p);
            }
        }
    }
    for (i = 0; i < D_800675B0.count; i++) {
        p = &D_800675B0.e[i];
        if (p->state != 1 && p->f_f != 0) {
            bit = p->f_14 >> 1;
            word = arg0[bit / 32];
            if (!(word & (1 << (bit % 32)))) {
                func_80013EB0((Unk80013EB0 *)p);
            }
        }
    }
}

#ifdef NON_MATCHING
s32 func_80014918(u32 **arg0, s32 arg1) {
    s32 sp10[42];
    Unk80014148 *e;
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
                r = (u32 *)func_80014B44((u32)r);
                **in = (u32)r;
            }
            w = *r;
            if (!(w & 1)) {
                e = func_80014148(w);
                if (e != 0 && e->kind != 2 && (e->pad == 1 || *((s8 *)e + 0xF) == 0)) {
                    for (j = 0; j < n; j++) {
                        if (*(s32 *)((u8 *)e + 0x14) == base[j]) break;
                    }
                    if (j == n) {
                        v = *(s32 *)((u8 *)e + 0x14);
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
                } else if (v != -0xFF && *(s16 *)(v + 6) < 2 && *(s8 *)(v + 0xF) == 0) {
                    for (j = 0; j < n; j++) {
                        if (*(s32 *)(v + 0x14) == base[j]) break;
                    }
                    if (j == n) {
                        v = *(s32 *)(v + 0x14);
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
INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014918);
#endif

typedef struct Unk80014B44 {
    s32 unk0;
    u32 unk4;
} Unk80014B44;

extern Unk80014B44 **D_80067590;

Unk80014B44 *func_80014B44(u32 arg0) {
    Unk80014B44 **table;
    Unk80014B44 *p;

    table = D_80067590;
    do {
        p = table[(arg0 >> 15) & 0xFF];
        while (p->unk4 != arg0) {
            p++;
        }
    } while (0);
    return p;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014B90);

s32 func_80014C08(void) {
    s32 temp_v0;

    temp_v0 = func_8001148C(4);
    if (temp_v0 != 0) {
        func_800116C0(temp_v0);
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

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014C68);

extern void func_80051328(int);
typedef struct { void (*fn)(); char pad[24]; } Entry;
extern Entry D_8005B1F0[];
extern int D_80067FB4[];
extern int D_80067FBC[];

int func_8001521C(int *p) {
    int i;
    int off;
    char buf[8];
    if (p[0] == 1) {
        if (D_8005F1A4 != 0 && D_80067FB4[0] != 0) func_80012ED0(1);
        if (D_80067FBC[0] != 0) func_80051328(1);
        i = 0;
        func_80034394();
        off = 0;
        func_80032588();
        do {
            void (*f)() = *(void (**)())((char *)D_8005B1F0 + off);
            if (f != 0) f();
            i++;
            off += 0x1C;
        } while (i < 22);
        i = 0;
        if (p[10] > 0) {
            do {
                func_800155E0(p, i);
                i++;
            } while (i < p[10]);
        }
        func_800116C0(p[11]);
        func_800116C0(p[0xA3C / 4]);
        func_800116C0(p[6]);
        i = -0xFF;
        p[0] = 0;
        return i;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80015344);

void *func_8001557C(s32 arg0) {
    s32 i;

    for (i = 0; i < 10; i++) {
        if (D_80062BB4[i].unk6 == 5 && D_80062BB4[i].unk18 == arg0) {
            return &D_80062BB4[i];
        }
    }
    return 0;
}

void func_800155E0(void *arg0, s32 arg1) {
    char *p;

    p = (char *)arg0 + arg1 * 0x2C;
    if (*(s16 *)(p + 0x30) != 0) {
        *(s16 *)(p + 0x30) = 0;
    }
}
