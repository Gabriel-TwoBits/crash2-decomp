#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80063034;

#define ENTRY_ANY (-1)
#define ENTRY_KIND_PLAY 0x13B
#define ENTRY_KIND_LOAD 0x13C
#define ADDR_MASK 0xFFFF00
#define ADDR_FLAG_SHIFT 6
#define ADDR_FLAG_READY 1
#define SLOT_MASK 0xFF
#define SLOT_COUNT 8
#define OWNER_SLOTS_OFFSET 0x10
#define SLOT_TABLE_OFFSET 0x194
#define SLOT_STRIDE_SHIFT 2
#define LOAD_FAILED_BELOW (-0xFE)
#define OBJ_FLAGS_A_OFFSET 0xAC
#define OBJ_FLAGS_A_MASK 0x01000000
#define OBJ_FLAGS_B_OFFSET 0x100
#define OBJ_FLAGS_B_MASK 0x40000
#define ENTRY_BYTE3_SHIFT 24
#define INIT_D_8006CDF4 0x19000

extern s32 D_8005F3B8;
extern Unk80063034 D_8006302C[];
extern u8 D_8006B1C4[];
extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;
s32 func_80019098(void *, u8);

void func_8001A23C(void *arg0, s32 arg1) {
    s32 i;
    s32 j;
    s32 remove;
    u32 addr;
    u32 slot;
    s32 obj;
    Unk80063034 *e;

    D_8006CDF0 = 0;
    D_8006CDF8 = 0;
    D_8006CDF4 = INIT_D_8006CDF4;
    for (i = D_8005F3B8 - 1; i >= 0; i--) {
        e = &D_8006302C[i];
        if (arg1 != ENTRY_ANY && arg1 != e->unk8) {
            continue;
        }
        if (e->unk8 == ENTRY_KIND_PLAY) {
            addr = (u32)e->unk0 & ADDR_MASK;
            if (*(u32 *)(D_8006B1C4 + (addr >> ADDR_FLAG_SHIFT)) & ADDR_FLAG_READY) {
                if (e->unk4 == 0) {
                    e->unk4 = func_80019FA0((s32)addr);
                }
                obj = e->unk4;
                if (obj != 0 &&
                    !(*(u32 *)(obj + OBJ_FLAGS_A_OFFSET) & OBJ_FLAGS_A_MASK) &&
                    !(*(u32 *)(obj + OBJ_FLAGS_B_OFFSET) & OBJ_FLAGS_B_MASK)) {
                    func_80019BBC((s32 *)obj, 1);
                }
            } else {
                /* not ready: dropped without starting */
            }
            remove = 1; /* a play entry is always consumed */
        } else if (e->unk8 == ENTRY_KIND_LOAD) {
            addr = (u32)e->unk0 & ADDR_MASK;
            slot = (u32)e->unk0 & SLOT_MASK;
            remove = 0;
            if (!(*(u32 *)(D_8006B1C4 + (addr >> ADDR_FLAG_SHIFT)) & ADDR_FLAG_READY) && slot < SLOT_COUNT) {
                e->unk4 = func_80019098(
                    func_80014260((u8 *)*(s32 *)((u8 *)arg0 + OWNER_SLOTS_OFFSET) + (slot << SLOT_STRIDE_SHIFT) + SLOT_TABLE_OFFSET, 1, 0),
                    (u32)e->unk0 >> ENTRY_BYTE3_SHIFT);
                if (e->unk4 >= LOAD_FAILED_BELOW) {
                    remove = 1;
                }
            }
            if (!remove) {
                if (e->unk4 == 0) {
                    e->unk4 = func_80019FA0((s32)addr);
                }
                e->unk8 = 0;
            }
        } else {
            remove = 0;
        }
        if (remove) {
            for (j = i + 1; j < D_8005F3B8; j++) {
                D_8006302C[j - 1] = D_8006302C[j];
            }
            D_8005F3B8 -= 1;
        }
    }
}
