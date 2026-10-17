#include "common.h"
#include "sdk.h"
#include "game.h"

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
