#include "common.h"
#include "sdk.h"
#include "game.h"

#define SAVE_MAGIC 0x6396347F
#define GPU_CMD_TERMINATOR 0x0C01FFFF
#define NO_ENTRY 0x63
#define STATE_MENU 2
#define ENTRY_STRIDE 0x18
#define ENTRY_BASE_OFFSET 0x1DC
#define ENTRY_SEL_OFFSET 0x1E0
#define CLEAR_BYTES 0x1000
#define FLAG_BIT_3 0x8
#define FLAG_BIT_6 0x40
#define FLAG_BIT_8 0x100
#define FLAG_BIT_9 0x200
#define MODE_300 0x300
#define MODE_600 0x600
#define RETURN_CODE (-0xFF)
#define SLOT_AFTER_BIT 4
#define SLOT_NOT_FOUND 3
#define SLOT_FIRST 1

extern s32 D_8003BEA4;
extern s32 D_8005B914[];
extern s32 D_8005F388;
extern s32 D_8005F3B0;
extern s32 D_8005F3B4;
extern s32 D_8005F3EC;
extern s32 D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern s8 D_8006094C;
extern s8 D_8006094D;
extern s8 D_8006094E;
extern s8 D_8006094F;
extern s32 D_80060954;
extern s32 D_8006097C;
extern s32 D_80060980;
extern s32 D_80060984;
extern s32 *D_80067598;
extern u8 *D_8006759C;
extern u8 D_8006B1C4[];
extern s32 D_8006CB80;
extern s32 D_8006CB94;
extern s32 D_8006CB98;
extern s32 D_8006CBB4;
extern s32 D_8006CBC0;
extern s32 D_8006CBDC;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 D_8006CD14;
extern s32 D_8006CD7C;

#define FIELD(base, off) (*(s32 *)((base) + (off)))

s32 func_8001F9C4(void) {
    s32 *table;
    s32 entry;
    s32 key;
    s32 slot;
    s32 extra;
    s32 off;
    u8 *obj;

    D_8006CD14 = 0;
    if (*D_80067598 == SAVE_MAGIC) {
        FIELD(D_8006759C, 4) = 0;
        D_8005F1F8 = 0;
        return RETURN_CODE;
    }
    D_8006CB80 = GPU_CMD_TERMINATOR;
    D_8006CB94 = GPU_CMD_TERMINATOR;
    D_8005F3B0 = 0;
    D_8005F3B4 = 0;
    slot = D_8006CD7C;
    extra = 0;
    if (FIELD(D_8006759C, 8) != D_8005F3EC) {
        for (;;) {
        }
    }
    if (FIELD(D_8006759C, 8) == STATE_MENU) {
        if (D_8006CCD8 & 1) {
            slot = SLOT_AFTER_BIT;
        } else {
            key = D_8006CCD4;
            slot = SLOT_FIRST;
            if (key == NO_ENTRY) {
                key = D_8006CBC0;
            }
            if (key != NO_ENTRY) {
                key = key >> 8;
            }
            table = D_8005B914;
            switch (key) {
            case 37:
                key = 0x17;
                break;
            case 6:
                if (D_8006CBDC & FLAG_BIT_6) {
                    key = 0x11;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x11;
                } else {
                    key = 0x1E;
                }
                break;
            case 8:
                if (D_8006CBDC & FLAG_BIT_8) {
                    key = 0x21;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x21;
                } else {
                    key = 0x11;
                }
                break;
            case 3:
                if (D_8006CBDC & FLAG_BIT_3) {
                    key = 0xD;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0xD;
                } else {
                    key = 0x21;
                }
                break;
            case 9:
                if (D_8006CBDC & FLAG_BIT_9) {
                    key = 0x10;
                } else if (D_8006CB98 == MODE_300) {
                    key = 0x10;
                } else {
                    key = 0xD;
                }
                break;
            case 7:
                key = 0x10;
                break;
            case 39:
                key = 0xD;
                break;
            default:
                break;
            }
            for (;;) {
                entry = *table++;
                if (entry < 0) {
                    slot = SLOT_NOT_FOUND;
                    break;
                } else if (entry == key) {
                    break;
                } else {
                    slot++;
                }
            }
        }
    }
    D_8006CD7C = slot;
    if (slot >= FIELD(D_8006759C, 0)) {
        slot = 0;
        D_8006CD7C = 0;
    }
    memset(D_8006B1C4, 0, CLEAR_BYTES);
    func_8001F858(FIELD(D_8006759C, 8));
    func_8001798C();
    if (D_8005F1F4 == 1) {
        off = slot * ENTRY_STRIDE;
        FIELD(D_8006759C + off, ENTRY_BASE_OFFSET) = D_8006097C;
        D_8005F1F4 = 0;
        FIELD(D_8006759C + off, ENTRY_SEL_OFFSET) = D_80060980;
        extra = D_80060984;
    }
    D_800608CC = 0;
    D_800608D4 = 0;
    D_800608E0 = 0;
    off = slot * ENTRY_STRIDE;
    obj = (u8 *)func_80014260(D_8006759C + off + ENTRY_BASE_OFFSET, 1, 1);
    D_8005F388 = SAVE_MAGIC;
    D_8006094C = 0;
    D_8006094D = 0;
    D_8006094E = 0;
    D_8006094F = 0;
    D_80060954 = D_8003BEA4;
    if (D_8006CBB4 == MODE_600) {
        func_8002FE58(&D_8005F388);
    }
    func_800200A8((s32)obj,
                  FIELD(obj, (FIELD(D_8006759C + off, ENTRY_SEL_OFFSET) * 3 + FIELD((u8 *)FIELD(obj, 0x10), 0x184)) * 4 + 0x10),
                  extra, 0);
    func_800143C0(D_8006759C + off + ENTRY_BASE_OFFSET, 1);
    D_8005F1F8 = 0;
    return RETURN_CODE;
}
