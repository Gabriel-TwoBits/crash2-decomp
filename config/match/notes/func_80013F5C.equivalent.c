#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80067FC4;

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
