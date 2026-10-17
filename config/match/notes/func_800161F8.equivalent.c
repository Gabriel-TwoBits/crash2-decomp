#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 idx;
    s32 pad4[2];
    s32 f0C;
    s32 pad10;
    s32 f14;
    u8 pad18[0x2018 - 0x18];
    u8 disp[0x14];
    u8 draw[0x2094 - 0x202C];
} Unk800160B0Entry;
typedef struct {
    s32 count;
    Unk800160B0Entry *cur;
    Unk800160B0Entry *other;
    Unk800160B0Entry *first;
    Unk800160B0Entry e[1];
} Unk800160B0Gfx;
extern Unk800160B0Gfx D_8006344C;
typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
    u8 pad2030[0x2088 - 0x2030];
    void *unk2088;
    u8 pad208C[0x2090 - 0x208C];
    char *unk2090;
} Unk80063458;
extern Unk80063458 *D_80063458[];
#define RETAIL_STATE_LOAD 2
#define RETAIL_STATE_PAL 0x28
#define CLEAR_OFFSET 0x18
#define CLEAR_BYTES 0x800
#define SHARED_BUF_BYTES 0x1770
#define ENTRY_BUF_BYTES 0x16400
#define ENTRY_BUF_BYTES_PAL 0x2C800
#define ENTRY_BUF_OFFSET 4
#define ENTRY_SHARED_OFFSET 0x208C
#define ALLOC_FAIL_EXIT_CODE -0xF
#define RETURN_VALUE -0xFF

extern void exit(s32);
extern s32 func_80036368(void);
extern s32 func_8003BC5C(u8 *, s32);
extern s32 D_8005F3EC;
extern u8 *D_80063454;

s32 func_800161F8(void) {
    s32 i;
    s32 shared;
    s32 buf;
    s32 size;
    u8 *entry;

    if (D_8005F3EC == RETAIL_STATE_LOAD) {
        func_80036368();
    }
    func_8003BC5C((u8 *)D_80063458[0] + CLEAR_OFFSET, CLEAR_BYTES);
    func_8003BC5C(D_80063454 + CLEAR_OFFSET, CLEAR_BYTES);
    i = 0;
    shared = func_8001148C(SHARED_BUF_BYTES);
    if (D_8006344C.count > 0) {
        entry = (u8 *)D_8006344C.e;
        do {
            size = ENTRY_BUF_BYTES;
            *(s32 *)(entry + ENTRY_SHARED_OFFSET) = shared;
            if (D_8005F3EC == RETAIL_STATE_PAL) {
                size = ENTRY_BUF_BYTES_PAL;
            }
            buf = func_8001148C(size);
            *(s32 *)(entry + ENTRY_BUF_OFFSET) = buf;
            if (buf == 0) {
                exit(ALLOC_FAIL_EXIT_CODE);
            }
            func_800163C4((s32 *)entry);
            i++;
            entry += sizeof(*D_8006344C.e);
        } while (i < D_8006344C.count);
    }
    return RETURN_VALUE;
}
