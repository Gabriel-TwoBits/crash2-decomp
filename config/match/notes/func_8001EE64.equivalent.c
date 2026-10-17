#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_COUNT 0x60
#define ENTRY_SIZE 0x23C
#define POOL_BYTES 0xD680
#define HEADER_BYTES 0x33C
#define SLOT_COUNT 8
#define ERR_ALLOC (-0xF)
#define ERR_INIT (-0xFF)

extern s32 D_8005F38C;
extern s32 D_8006CB10;
extern s32 D_8006CB14;
s32 func_8001148C(s32);
extern s32 D_8006CB18[];
extern s32 D_8006CB58;
extern s32 D_8006CB60;
extern s32 D_8006CB64;

/* D_8006CB10 and D_8006CB14 are adjacent; the table offsets below are taken
   from their addresses, not from the blocks they point to. */
s32 func_8001EE64(void) {
    s32 i;
    s32 pool;
    s32 header;
    s32 entry;
    s32 next;
    s32 list;
    s32 ring;
    s32 *p;

    D_8005F38C = 0;
    D_8006CB14 = func_8001148C(POOL_BYTES);
    D_8006CB10 = func_8001148C(HEADER_BYTES);
    if (D_8006CB14 == 0 || D_8006CB10 == 0) {
        return ERR_ALLOC;
    }
    D_8006CB60 = 0;
    D_8006CB64 = 0;
    ring = (s32)&D_8006CB14 + 0x1468;
    *(s32 *)(D_8006CB10 + 8) = ring;
    D_8006CB58 = 2;
    *(s32 *)((s32)&D_8006CB14 + 0x48) = D_8006CB14;
    list = (s32)&D_8006CB10 + 0x14D8;
    next = ENTRY_SIZE;
    for (i = 0; i < ENTRY_COUNT; i++) {
        entry = D_8006CB14 + i * ENTRY_SIZE;
        *(s32 *)(entry + 8) = list;
        *(s32 *)entry = 0;
        *(s32 *)(entry + 0x44) = (s32)&D_8006CB10 + 0x48;
        *(s32 *)(entry + 0x4C) = 0;
        if (i < ENTRY_COUNT - 1) {
            *(s32 *)(entry + 0x48) = D_8006CB14 + next;
        } else {
            *(s32 *)(entry + 0x48) = 0;
        }
        next += ENTRY_SIZE;
        list += 0x30;
    }
    p = D_8006CB18;
    for (i = 0; i < SLOT_COUNT; i++) {
        p[0] = 2;
        p[1] = 0;
        p += 2;
    }
    *(s32 *)D_8006CB10 = 0;
    *(s32 *)(D_8006CB10 + 0x44) = 0;
    *(s32 *)(D_8006CB10 + 0x4C) = 0;
    *(s32 *)(D_8006CB10 + 0x48) = 0;
    return ERR_INIT;
}
