#include "common.h"
#include "sdk.h"
#include "game.h"

#define OWNER_OFFSET 0x18
#define COUNT_OFFSET 0x404
#define ENTRIES_OFFSET 0x520
#define ENTRY_SIZE 8

s32 func_80012294(char *arg0, s32 arg1) {
    s32 i;
    s32 matches;
    s32 offset;
    char *owner;

    matches = 0;
    offset = ENTRIES_OFFSET;
    for (i = 0; i < *(s32 *)((char *)(*(char **)(arg0 + OWNER_OFFSET)) + COUNT_OFFSET); i++) {
        owner = *(char **)(arg0 + OWNER_OFFSET);
        if (func_80012250((s32 *)(owner + offset)) == arg1) {
            matches++;
        }
        offset += ENTRY_SIZE;
    }
    return matches;
}
