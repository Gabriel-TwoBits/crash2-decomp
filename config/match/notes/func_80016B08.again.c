#include "common.h"
#include "sdk.h"
#include "game.h"

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

extern Unk80063458 *g_BuildBuffer[];
extern u8 D_8005B500[];
extern s32 D_8006CB94;
s32 GetTPage(s32, s32, s32, s32);
void SetDrawMode(void *, s32, s32, s32, s32 *);
void SetPolyF4(void *);
void SetSemiTrans(void *, s32);

void Gfx_DrawFade(u32 arg0) {
    u8 *p;
    u8 *q;
    u8 *t;
    u8 *r;
    s32 lo;
    s32 hi;

    q = (u8 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)(q + 0xC);
    p = (u8 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)(p + 0x18);
    if (arg0 != 0) {
        t = D_8005B500 + (arg0 >> 4);
        lo = t[0];
        hi = t[1];
        r = p;
        *(s16 *)(p + 8) = 0;
        *(s16 *)(p + 0x10) = 0;
        *(s16 *)(p + 0xA) = -0xC;
        *(s16 *)(p + 0xE) = -0xC;
        *(s16 *)(p + 0xC) = 0x200;
        *(s16 *)(p + 0x12) = 0xE4;
        *(s16 *)(p + 0x14) = 0x200;
        *(s16 *)(p + 0x16) = 0xE4;
        arg0 = ((u32)((hi - lo) * (arg0 & 0xF)) >> 4) + lo;
        p[4] = arg0;
        p[5] = arg0;
        p[6] = arg0;
        SetPolyF4(r);
        SetSemiTrans(p, 1);
        AddPrim(&g_BuildBuffer[0]->padC[0x2008], p);
        SetDrawMode(q, 1, 0, GetTPage(0, (D_8006CB94 & 0x800000) ? 1 : 2, 0, 0) & 0xFFFF, 0);
        AddPrim(&g_BuildBuffer[0]->padC[0x2008], q);
    }
}
