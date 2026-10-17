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
void func_80011800(s32 arg0);
extern s32 D_8005F1A4;

s32 func_800117BC(void) {
    __main();
    D_8005F1A4 = 1;
    func_80015614();
    func_80011800(0x3C);
    func_800157A0();
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011800);
