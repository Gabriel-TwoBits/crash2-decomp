#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[4];
    s16 state;
    u8 pad6[9];
    s8 flag;
    u8 pad10[0x2C - 0x10];
} Unk8001456CEntry;

typedef struct {
    s32 count;
    Unk8001456CEntry entries[21];
    s32 count2;
    Unk8001456CEntry entries2[1];
} Unk8001456C;

extern Unk8001456C D_800675B0;
extern s32 D_80067950;

s32 func_8001456C(void) {
    Unk8001456CEntry *e;
    s32 i;
    s32 j;
    s32 n = D_800675B0.count;

    i = 0;
    if (D_80067950 > 0) {
        e = D_800675B0.entries2;
        do {
            if (e->state != 1 && e->flag != 0) n--;
            i++;
            e++;
        } while (i < D_800675B0.count2);
    }
    j = 0;
    if (D_800675B0.count > 0) {
        e = D_800675B0.entries;
        do {
            if (e->state != 1 && e->flag != 0) n--;
            j++;
            e++;
        } while (j < D_800675B0.count);
    }
    return n;
}
