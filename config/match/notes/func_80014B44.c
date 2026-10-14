#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    u32 unk4;
} Unk80014B44;

extern Unk80014B44 **D_80067590;

Unk80014B44 *func_80014B44(u32 arg0) {
    Unk80014B44 **table;
    Unk80014B44 *p;

    table = D_80067590;
    do {
        p = table[(arg0 >> 15) & 0xFF];
        while (p->unk4 != arg0) {
            p++;
        }
    } while (0);
    return p;
}
