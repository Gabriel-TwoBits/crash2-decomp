#include "common.h"
#include "sdk.h"
#include "game.h"

#define KEY_MASK 0xFFFF00

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80063034;

extern s32 D_8005F3B8;
extern Unk80063034 D_8006302C[];
extern Unk80063034 D_80063034[];

/* Entries are 0xC bytes from D_8006302C: key, 0, value (D_80063034). */
void func_8001A054(s32 arg0, s32 arg1, s32 arg2) {
    s32 *src;
    s32 n;
    s32 i;

    n = arg2 - 1;
    src = (s32 *)(arg1 + n * (s32)sizeof(s32));
    for (; n >= 0; n--, src--) {
        for (i = 0; i < D_8005F3B8; i++) {
            if ((D_8006302C[i].unk0 & KEY_MASK) == (*src & KEY_MASK)) {
                D_8006302C[i].unk0 = *src;
                D_80063034[i].unk0 = arg0;
                break;
            }
        }
        if (i == D_8005F3B8) {
            D_80063034[i].unk0 = arg0;
            D_8006302C[i].unk4 = 0;
            D_8005F3B8 = i + 1;
            D_8006302C[i].unk0 = *src;
        }
    }
}
