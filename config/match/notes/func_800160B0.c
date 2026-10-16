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
extern s32 D_8003BEA4;
extern s32 D_8006CCDC;
extern s32 D_80060944;
void PutDispEnv(void *env);
void SetDefDispEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void SetDispMask(s32 mask);
void SetGraphDebug(s32 level);

void func_800160B0(void) {
    s32 i;
    Unk800160B0Entry *e;
    D_80060944 = 0;
    ResetGraph(0);
    SetGraphDebug(0);
    func_8001601C();
    D_8006344C.count = 2;
    for (i = 0; i < D_8006344C.count; i++) {
        e = &D_8006344C.e[i];
        e->idx = i;
        e->f0C = 0;
        SetDefDispEnv(e->disp, i << 9, 0, 0x200, 0xF0);
        D_8006CCDC = 0;
        *(s16 *)(e->disp + 8) = 0;
        SetDefDrawEnv(e->draw, i << 9, 0xC, 0x200, 0xD8);
        *(s16 *)(e->draw + 8) = i << 9;
        *(s16 *)(e->draw + 10) = 0xC;
        e->draw[0x16] = 1;
        e->draw[0x17] = 0;
        if (i == 0) {
            D_8006344C.first = e;
            D_8006344C.cur = e;
        }
        if (i == 1) {
            D_8006344C.other = e;
        }
        e->f14 = 0x22;
        e->f0C = D_8003BEA4;
    }
    PutDispEnv(D_8006344C.cur->disp);
    PutDrawEnv(D_8006344C.other->draw);
    SetDispMask(1);
}
