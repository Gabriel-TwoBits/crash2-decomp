#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[0x8];
    u8 *unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
} Unk80063458;

typedef struct {
    u8 pad0[3];
    u8 len;
    u8 r, g, b;
    u8 code;
    s16 x, y;
    s16 w, h;
} Unk80016A60;

extern Unk80063458 *D_80063458[];
void AddPrim(void *, void *);

void func_80016A60(s32 arg0, s32 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6) {
    Unk80016A60 *p;

    p = (Unk80016A60 *)D_80063458[0]->unk8;
    D_80063458[0]->unk8 = (u8 *)p + 0x10;
    p->len = 3;
    p->code = 2;
    p->w = arg2;
    p->h = arg3;
    p->r = arg4;
    p->g = arg5;
    p->b = arg6;
    p->x = D_80063458[0]->unk202C + arg0;
    p->y = D_80063458[0]->unk202E + arg1;
    AddPrim(&D_80063458[0]->padC[0xC], p);
}
