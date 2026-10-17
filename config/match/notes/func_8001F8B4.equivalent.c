#include "common.h"
#include "sdk.h"
#include "game.h"

#define SLOT_FREE 1
#define OWNER_OFFSET 0x10
#define FLAGS_OFFSET 0x29C
#define FLAG_LOCKED 0x2000
#define KEY_HI_SHIFT 9
#define KEY_LO_SHIFT 8

extern char *D_800608CC;
extern void *D_8006759C;
extern u16 D_8006935C[];

void func_8001F8B4(s32 arg0) {
    u16 *p;
    u16 *free_slot;
    u16 key;

    key = (arg0 >> KEY_LO_SHIFT) | (*(s32 *)((char *)D_8006759C + 8) << KEY_HI_SHIFT);
    free_slot = 0;
    p = D_8006935C;
    if (*(s32 *)(*(char **)(D_800608CC + OWNER_OFFSET) + FLAGS_OFFSET) & FLAG_LOCKED) {
        return;
    }
    for (; *p != 0; p++) {
        if (*p == key) {
            return;
        }
        if (*p == SLOT_FREE) {
            free_slot = p;
        }
    }
    if (free_slot == 0) {
        free_slot = p;
    }
    *free_slot = key;
}
