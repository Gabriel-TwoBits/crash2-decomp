#include "common.h"
#include "sdk.h"
#include "game.h"

#define SLOT_STATE_ACTIVE 3
#define SLOT_STATE_OFFSET_WORDS 1
#define SLOT_CLEAR_OFFSET_BYTES 0x20
#define SLOT_CLEAR_SIZE_BYTES 0x20
#define SEARCH_SLOT_COUNT 8
#define SEARCH_SLOT_STRIDE 8
#define SEARCH_FLAGS 0x80000
#define LIST_TYPE_FREE 2
#define ERR_INVALID -0x16
#define RESULT_ERROR_LIMIT -0xFE

s32 func_80019988(s32 *, s32, s32, s32, s32);
extern s32 D_8005F38C;
extern s32 *D_8006CB10[];
extern s32 D_8006CB58[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;
extern u8 D_8006CB18;
extern char func_80019074[];

s32 func_80019800(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 *slot;
    s32 result;
    s32 found;
    s32 i;
    u8 *entry;

    if (arg1 == 0 && arg2 == 0) {
        slot = *D_8006CB10;
        D_8005F38C = (s32)slot;
    } else {
        while (1) {
            if (*D_8006CB58 == LIST_TYPE_FREE) {
                slot = (s32 *)D_8006CB5C;
            } else {
                slot = (s32 *)D_8006CBA4;
            }
            if (slot != 0 || arg5 == 0) {
                break;
            }
            found = 0;
            entry = &D_8006CB18;
            for (i = 0; i < SEARCH_SLOT_COUNT; i++) {
                found = func_80018BE4(entry, func_80019074, SEARCH_FLAGS);
                if (found != 0) {
                    break;
                }
                entry += SEARCH_SLOT_STRIDE;
            }
            if (found == 0) {
                break;
            }
            func_80019E08(found);
        }
    }
    if (slot == 0) {
        return ERR_INVALID;
    }
    func_800189CC(slot, arg0);
    slot[SLOT_STATE_OFFSET_WORDS] = SLOT_STATE_ACTIVE;
    result = func_80019988(slot, arg1, arg2, arg3, arg4);
    memset((u8 *)slot + SLOT_CLEAR_OFFSET_BYTES, 0, SLOT_CLEAR_SIZE_BYTES);
    if (result < RESULT_ERROR_LIMIT) {
        result = (s32)slot;
    }
    return result;
}
