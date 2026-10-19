#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80027980Entry {
    s32 pad0[2];
    s32 state;
    s32 flag;
    s32 size;
    s32 pad14[16];
} Unk80027980Entry;

typedef struct Unk80027980Pool {
    s32 count;
    s32 pad4[2];
    s32 used;
    s32 free;
} Unk80027980Pool;

extern Unk80027980Pool D_80069334;
extern Unk80027980Entry *D_80069338;

void func_80027980(s32 arg0) {
    s32 *a = (s32 *)arg0;
    s32 i;
    Unk80027980Entry *e;

    if (a[2] != 0) {
        a[3] = 1;
        for (i = D_80069334.count - 1, e = &D_80069338[i]; i >= 0; i--, e--) {
            if (e->state == 1) {
                if (e->flag != 1) {
                    return;
                }
                e->state = 0;
                e->flag = 0;
                D_80069334.free -= e->size;
                D_80069334.used += e->size;
            }
        }
    }
}
