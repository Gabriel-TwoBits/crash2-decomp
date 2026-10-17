#include "common.h"
#include "sdk.h"
#include "game.h"

#define HANDLE_FLAG_SLOT 0x2
#define HANDLE_TYPE_MASK 0xC0000002
#define HANDLE_TYPE_SCAN 0x80000002
#define SCAN_LAST_INDEX 15
#define KIND_SCAN_FIRST 0x14
#define KIND_SCAN_COUNT 2
#define HANDLE_INDEX_SHIFT 16
#define HANDLE_INDEX_BIAS 0xFFFF
#define ENTRY_HANDLE_OFFSET 0x1C
typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 f_e;
    s8 f_f;
    u8 pad10[4];
    s32 f_14;
    s32 f_18;
    u8 pad1c[0xC];
    s32 f_28;
} Unk80013BD8;

typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Unk80014148;
Unk80014148 *func_80014148(u32);

extern Unk80013BD8Table D_800675B0;
extern Unk80013BD8 D_80063000;
extern s32 D_800675B4;

Unk80014148 *func_80014148(u32 arg0) {
    Unk80013BD8 *e;
    s32 i;
    s32 d;
    s32 idx;

    if (arg0 & HANDLE_FLAG_SLOT) {
        if ((arg0 & HANDLE_TYPE_MASK) != HANDLE_TYPE_SCAN) {
            return 0;
        }
        e = &D_80063000;
        for (i = SCAN_LAST_INDEX; i >= 0; i--, e--) {
            if ((u32)((u16)e->state - KIND_SCAN_FIRST) < KIND_SCAN_COUNT &&
                arg0 == *(u32 *)((u8 *)e + ENTRY_HANDLE_OFFSET)) {
                return (Unk80014148 *)e;
            }
        }
        return 0;
    } else {
        d = arg0 - D_800675B4;
        if (d < 0) {
            d += HANDLE_INDEX_BIAS;
        }
        idx = d >> HANDLE_INDEX_SHIFT;
        if (idx < 0 || idx >= D_800675B0.count) {
            return 0;
        }
        return (Unk80014148 *)((u8 *)&D_800675B4 + idx * sizeof(Unk80013BD8));
    }
}
