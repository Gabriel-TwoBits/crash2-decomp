#include "common.h"
#include "sdk.h"
#include "game.h"

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
