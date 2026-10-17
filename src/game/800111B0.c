#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F17C = 0;
s32 D_8005F180 = 0;
s32 D_8005F184 = 0x1000;
s32 D_8005F188 = 0x200000;
s32 D_8005F18C = 0;
char D_8005F190[] = "dead\n";

#ifdef NON_MATCHING
extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;

s32 func_800111B0(u32 n) {
    s32 lim;
    s32 hi;
    s32 lo;

    if (n != 0) {
        hi = n;
        lo = n;
        if ((u32)D_8005F2AC < n) {
            lo = D_8005F2AC;
        } else {
            hi = D_8005F2AC;
        }
        lim = ((((char *)D_8005F2B4 - (char *)D_8005F2A4) >> 2) * 4) - 8;
        if (lim >= hi) {
            n = hi;
        } else if (lim >= lo) {
            n = lo;
        } else {
            return -1;
        }
    } else {
        n = 0;
        if ((u32)D_8005F2A4 >= (u32)D_8005F2B4) {
            return -1;
        }
    }
    D_8005F2A4[(s32)n / 4] = -2;
    D_8005F2A4[-1] = n | 1;
    D_8005F2A4 = &D_8005F2A4[((s32)n + 4) / 4];
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800111B0);
#endif

#ifdef NON_MATCHING
#define HEAP_END_MARK (-2)
#define HEAP_FREE_BIT 1
#define HEAP_SIZE_MASK (~3)
#define HEAP_FLAGS_MASK (~1)
#define HEAP_ALIGN_MASK 3
#define WORD_SIZE 4
#define WORD_SHIFT 2
#define HEAP_RESERVE_WORDS 4

s32 func_800112A4(s32 arg0, s32 arg1) {
    u32 size;
    u32 acc;
    u32 need;
    u32 gap;
    u32 hdr;
    u32 next;
    u32 *head;
    u32 *cur;
    u32 *blk;
    u32 *rest;

    need = (arg1 + HEAP_ALIGN_MASK) & HEAP_SIZE_MASK;
    head = (u32 *)(arg0 - WORD_SIZE);
    if (*head == (u32)HEAP_END_MARK) {
        return 0;
    }
    acc = *head & HEAP_FLAGS_MASK;
    if (acc < need) {
        cur = head;
        while (1) {
            blk = (u32 *)((u8 *)cur + (*cur & HEAP_SIZE_MASK));
            next = blk[1];
            cur = blk + 1;
            if (next == (u32)HEAP_END_MARK) {
                gap = need - acc;
                if ((u32)(((s32)((u8 *)D_8005F2B4 - (u8 *)cur) >> WORD_SHIFT) - HEAP_RESERVE_WORDS) < gap) {
                    break;
                }
                blk[1] = gap | HEAP_FREE_BIT;
                D_8005F2A4 = (s32 *)((u8 *)cur + (gap & HEAP_SIZE_MASK));
                *D_8005F2A4 = HEAP_END_MARK;
                D_8005F2A4 = (s32 *)((u8 *)D_8005F2A4 + WORD_SIZE);
            } else if (!(next & HEAP_FREE_BIT)) {
                break;
            } else {
                /* free neighbour: merge below */
            }
            *head += *cur & HEAP_FLAGS_MASK;
            acc += *cur & HEAP_FLAGS_MASK;
            if (!(acc < need)) {
                break;
            }
        }
    }
    hdr = *head;
    size = hdr & HEAP_SIZE_MASK;
    if (need < size) {
        rest = (u32 *)((u8 *)head + need);
        rest[1] = (hdr & HEAP_FLAGS_MASK) - need - WORD_SIZE;
        rest[1] |= HEAP_FREE_BIT;
        *head = need | (*head & HEAP_FREE_BIT);
        return arg0;
    } else if ((hdr & HEAP_FLAGS_MASK) < need) {
        return 0;
    } else {
        return arg0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800112A4);
#endif

extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;
extern s32 D_8005F2BC;

void func_8001144C(s32 *arg0, u32 arg1) {
    D_8005F2A4 = arg0;
    do { *arg0 = 0; D_8005F2B4 = &arg0[arg1 >> 2] + 1; } while (0);
    D_8005F2AC = arg1 - 4;
    D_8005F2BC = 0;
}

#ifdef NON_MATCHING
extern s32 *D_8006074C;
extern s32 *D_80062BAC;

#define HEAP_FREE_END (-2)
#define HEAP_USED_BIT 1
#define HEAP_HEADER_SIZE 4
#define HEAP_ALIGN_MASK (~3)
#define HEAP_PASSES 2

/* Heap block header at p: bit 0 set = free, upper bits = payload size in bytes; -2 ends a chunk. */
#define HEAP_AT(p, off) ((s32 *)((u8 *)(p) + (off)))

s32 func_8001148C(s32 arg0) {
    s32 passes;
    u32 size;
    u32 want;
    s32 hdr;
    u32 cur;
    s32 *next;
    s32 nextHdr;

    passes = HEAP_PASSES;
    size = (arg0 + 3) & HEAP_ALIGN_MASK;
    want = size | HEAP_USED_BIT;

    if (D_8005F2BC == 0) {
        if (!((u32) D_8005F2A4 < (u32) D_8005F2B4)) {
            return 0;
        }
        *D_8005F2A4 = HEAP_FREE_END;
        D_8006074C = D_8005F2A4;
        D_8005F2A4 = HEAP_AT(D_8005F2A4, HEAP_HEADER_SIZE);
        if (func_800111B0(size) != 0) {
            return 0;
        }
        D_8005F2BC = 1;
        D_80062BAC = D_8006074C;
    }
    D_8006074C = D_80062BAC;

    for (;;) {
        hdr = *D_8006074C;
        if ((hdr & HEAP_USED_BIT) == 0) {
            if (hdr != HEAP_FREE_END) {
                D_8006074C = HEAP_AT(D_8006074C, ((u32) hdr >> 2 << 2) + HEAP_HEADER_SIZE);
            } else {
                passes--;
                if (passes > 0) {
                    D_8006074C = D_80062BAC;
                } else {
                    if (func_800111B0(size) != 0) {
                        return 0;
                    }
                }
            }
        } else if ((u32) hdr == want) {
            *D_8006074C = hdr & ~HEAP_USED_BIT;
            return (s32) HEAP_AT(D_8006074C, HEAP_HEADER_SIZE);
        } else {
            cur = *D_8006074C;
            if (want < cur) {
                *HEAP_AT(D_8006074C, size + HEAP_HEADER_SIZE) = cur - size - HEAP_HEADER_SIZE;
                *D_8006074C = size;
                return (s32) HEAP_AT(D_8006074C, HEAP_HEADER_SIZE);
            } else if (cur < size) {
                next = HEAP_AT(D_8006074C, (cur >> 2 << 2) + HEAP_HEADER_SIZE);
                nextHdr = *next;
                if (nextHdr == HEAP_FREE_END) {
                    *D_8006074C = nextHdr;
                    D_8005F2A4 = HEAP_AT(D_8006074C, HEAP_HEADER_SIZE);
                } else if (nextHdr & HEAP_USED_BIT) {
                    *D_8006074C = cur + HEAP_HEADER_SIZE + (nextHdr & ~HEAP_USED_BIT);
                } else {
                    D_8006074C = next;
                }
            } else {
                /* cur == size: no merge or split possible; rescan from the same block. */
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_8001148C);
#endif

void func_800116C0(s32 arg0) {
    ((s32 *)arg0)[-1] |= 1;
}

#ifdef NON_MATCHING
void bcopy(s32, s32, s32);

s32 func_800116D4(s32 arg0, s32 arg1) {
    s32 r;

    if (arg0 == 0) {
        r = func_8001148C(arg1);
    } else {
        if (arg1 == 0) {
            func_800116C0(arg0);
        } else {
            r = func_800112A4(arg0, arg1);
            if (r == 0) {
                r = func_8001148C(arg1);
                if (r != 0) {
                    bcopy(arg0, r, arg1);
                    func_800116C0(arg0);
                }
            }
        }
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116D4);
#endif

#ifdef NON_MATCHING
/* count elements of size bytes from func_8001148C, zeroed; 0 when it returns none */
s32 *func_80011764(s32 count, s32 size) {
    s32 total;
    s32 *block;

    total = size * count;
    block = (s32 *)func_8001148C(total);
    if (block == 0) {
        return 0;
    }
    bzero((u8 *)block, total);
    return block;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011764);
#endif

void __main(void);
extern s32 D_8005F1A4;

s32 func_800117BC(void) {
    __main();
    D_8005F1A4 = 1;
    func_80015614();
    func_80011800(0x3C);
    func_800157A0();
    return 0;
}

#ifdef NON_MATCHING
#define POWER_BUTTON_MASK 0x800
#define SCRATCH_ENTRY_BASE 0x1F800280
#define SAVE_MAGIC 0x6396347F
#define SAVE_FLAGS_MASK 0x20000001
#define STATE_DEAD 0x3B
#define STATE_NONE -1
#define STATE_PENDING -2
#define STATE_PAUSE 2
#define LEVEL_A 0x200
#define LEVEL_B 0x300
#define LEVEL_C 0x400
#define FLAG_16 0x10
#define FLAG_400000 0x400000
#define PAUSE_BUF_ID 0xC00
#define PAUSE_BUF_SIZE 0x800
#define PAUSE_BUF_OFFSET 0x18
#define BUF_FIELD_2088 0x2088
#define CD14(off) (*(s32 *)(D_8006CD14 + (off)))

extern s32 D_8003BEA4;
extern s32 D_8005F180;
extern s32 D_8005F18C;
extern char D_8005F190[];
extern s32 D_8005F1D0;
extern s32 D_8005F1F4;
extern s32 D_8005F2C4;
extern s32 D_8005F2C8;
extern s32 D_8005F388;
extern s32 D_8005F38C;
extern s32 D_8005F390;
extern s32 D_8005F3B8;
extern s32 D_8005F3F0;
extern char D_80060774[];
extern void *D_800608CC;
extern s32 D_80060944;
extern char D_80060958[];
extern s32 D_80060988;
extern char D_80062A18[];
extern s32 D_80062A90;
extern s32 D_80062AA0;
extern s32 D_80062AD8;
extern void *D_80063454;
extern void *D_80063458;
extern char D_80067588[];
extern void *D_8006759C;
extern s32 D_80069944;
extern s32 D_8006CB94;
extern s32 D_8006CB98;
extern s32 D_8006CBA0;
extern s32 D_8006CBB4;
extern s32 D_8006CCB8;
extern char D_8006CD14[];
extern s32 D_8006CD7C;

void SetRotMatrix(void *);
/* func_80041E5C stays undeclared: 800158E0.c declares it void, but its v0 is stored below (implicit int). */
/* func_80014C68 stays undeclared: 800123F0.c defines it over a struct type unknown here (implicit int). */
int printf(const char *, ...);

void func_80011800(s32 arg0) {
    s32 spC0;
    s32 state;
    s32 pending;
    s32 count;
    s32 *entry;
    s32 *out;
    s32 *list;

    func_8002EFE4();
    func_80014C68(D_80067588, arg0);
    func_8002EE48();
    do {
        if (((D_80069944 & POWER_BUTTON_MASK) || ((D_8005F180 == 1) && (D_8006CBA0 == 0))) && (CD14(0) == 0) && (CD14(4) == 0) && (CD14(-0x7C) != 0)) {
            D_8005F180 = D_8005F180 == 0;
            if (D_8005F180 != 0) {
                if (CD14(-0x174) == 0) {
                    CD14(-0x174) = func_80019800(D_8006CD14 - 0x1C4, 4, 4, 0, 0, 0);
                    if (CD14(-0x174) >= -0xFE) {
                        D_8005F180 = 0;
                        CD14(-0x174) = 0;
                        D_8005F18C = 0;
                    } else {
                        D_8005F18C = 1;
                        D_8005F2C4 = D_8003BEA4;
                        D_8005F2C8 = *(s32 *)((char *)D_80063454 + 0xC);
                    }
                } else {
                    /* flag state kept; nothing to do */
                }
            } else if (CD14(-0x174) != 0) {
                spC0 = 0;
                func_8001CC10(0, CD14(-0x174), PAUSE_BUF_ID, 1, &spC0);
                D_8005F180 = 1;
            } else {
                D_8005F18C = -1;
                D_8003BEA4 = D_8005F2C4;
                *(s32 *)((char *)D_80063454 + 0xC) = D_8005F2C8;
            }
        } else {
            D_8005F18C = 0;
        }
        func_80011E80();
        if ((D_8005F38C != 0) && (D_8005F388 != SAVE_MAGIC)) {
            func_8002FDCC(&D_8005F388);
        }
        if (D_8005F3F0 == STATE_NONE) {
            if ((*(s32 *)((char *)D_8006759C + 8) != 2) && ((D_8006CBB4 == LEVEL_A) || (D_8006CBB4 == LEVEL_B) || (D_8006CBB4 == LEVEL_C))) {
                printf(D_8005F190);
                D_8005F3F0 = STATE_DEAD;
                D_8006CD7C = 0;
            } else {
                /* no death transition */
            }
        } else {
            /* a state change is pending */
        }
        if (D_8005F3F0 != STATE_NONE) {
            state = D_8005F3F0;
            func_8001D194(0, 0x2900, 0, 0, 0);
            pending = 0;
            if (D_8005F3F0 == STATE_PENDING) {
                state = D_80060988;
                pending = 1;
                D_8005F1F4 = 1;
            } else {
                D_8005F1F4 = 0;
            }
            *(s32 *)(D_80067588 + 0x24) = 2;
            func_8001521C((int *)D_80067588);
            D_8005F180 = 0;
            D_8005F3B8 = 0;
            if ((state == 2) || ((u32)(state - STATE_DEAD) < 2U)) {
                D_8006CB98 = D_8006CBB4;
            } else {
                /* other states keep the saved level */
            }
            func_80014C68(D_80067588, state);
            func_8002EE48();
            if (pending != 0) {
                D_8005F3F0 = STATE_NONE;
                func_8002F6C8(D_80060958);
            } else {
                /* nothing to clear */
            }
            D_8005F1F4 = 0;
        } else {
            /* no pending state */
        }
        func_80012BC4(-1);
        if (D_8005F180 == 0) {
            func_80026CA0();
        } else {
            /* paused */
        }
        func_80017BC4();
        if ((D_8006CB94 & 1) && (**(s32 **)((char *)D_800608CC + 0x10) != 0)) {
            SetRotMatrix(D_80060774);
            list = *(s32 **)((char *)D_800608CC + 0x10);
            count = *list;
            out = (s32 *)SCRATCH_ENTRY_BASE;
            if (count != 0) {
                entry = list + 0x2C / 4;
                do {
                    s32 *owner = (s32 *)entry[-6];
                    out[0] = ((entry[-8] & 0xFFFE) << 16) | *(u16 *)((char *)entry - 0x24);
                    out[1] = entry[-7];
                    out[2] = entry[-5] + owner[0x14 / 4] * 4;
                    out[3] = entry[-4];
                    out[4] = entry[-3] + owner[0x10 / 4] * 4;
                    out[5] = entry[-2];
                    out[6] = entry[-1];
                    out[7] = entry[0];
                    out += 8;
                    count -= 1;
                    entry += 12;
                } while (count != 0);
            } else {
                /* empty list */
            }
            func_8002FB84(0);
            D_8006CCB8 = func_80041E5C(D_8005F390, D_80063458, D_80062A18, D_80060944);
        } else {
            func_8003BC5C((u8 *)D_80063458 + PAUSE_BUF_OFFSET, PAUSE_BUF_SIZE);
        }
        if (D_8006CB94 & SAVE_FLAGS_MASK) {
            if ((D_80062A90 != 0) && (D_80062AD8 & FLAG_16)) {
                func_80044F54(D_80062A90, D_80063458, D_80062A18);
            } else {
                /* skipped */
            }
            if ((D_80062AA0 != 0) && (D_80062AD8 & FLAG_400000)) {
                func_80045218(D_80062AA0, D_80063458, D_80062A18);
            } else {
                /* skipped */
            }
        } else {
            /* skipped */
        }
        func_8001C29C((void *)(D_8005F180 == 0));
        func_80011E88();
        func_8003DB94(*(s32 *)((char *)D_80063458 + BUF_FIELD_2088), D_80063458, D_80062A18);
        func_8001645C();
        func_80011E90();
    } while (D_8005F1D0 == 0);
    D_8006CB94 = 0;
    func_8001645C();
    func_8001645C();
    func_8001521C((int *)D_80067588);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011800);
#endif
