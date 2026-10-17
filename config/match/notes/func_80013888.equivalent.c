#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80062D70 {
    s16 state;
    u8 pad[0x2A];
} Unk80062D70;

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
