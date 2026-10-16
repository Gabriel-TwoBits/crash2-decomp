#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
} Unk80063458;

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

typedef struct {
    s32 mode;
    u8 pad4[0x2034 - 4];
    s16 x;
    s16 y;
} Unk80063454;

typedef struct {
    u8 pad0[4];
    u8 r, g, b;
    u8 pad7;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
} PolyF4;

extern Unk80063458 *D_80063458[];
extern Unk80063454 *D_80063454;
extern Unk80060884 D_80060884;
void SetPolyF4(void *);
void SetGeomScreen(s32);
s32 func_80023D7C(void);

void func_80016C58(s32 arg0, s32 arg1) {
    PolyF4 *p;
    s32 w;
    u32 x;
    u32 y;
    s16 right, left, top, bot, mid;
    s32 ret;

    p = (PolyF4 *)D_80063458[0]->unk8;
    w = arg0 * 25;
    x = (u32)(arg0 * 100) >> 3;
    D_80063458[0]->unk8 += 0x18;
    if (D_80063454->mode == 1) {
        D_80063454->x = 0x200 - x;
    } else {
        D_80063454->x = -x;
    }
    D_80063454->y = 0xC - ((u32)(arg0 * 60) >> 3);
    if (arg0 != 0) {
        if (arg1 == 0) {
            y = 0;
            x = 0;
        } else {
            x = (u32)(arg1 * 100) >> 3;
            y = (u32)(arg1 * 60) >> 3;
        }
        right = x + 0x200;
        left = right - w;
        top = y - 0xC;
        bot = y + 0xE4;
        p->x0 = left; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = left; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[0x2008], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        D_80063458[0]->unk8 += 0x18;
        mid = bot - arg0 * 15;
        p->x0 = x; p->y0 = mid; p->x1 = left; p->y1 = mid;
        p->x2 = x; p->y2 = bot; p->x3 = left; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[0x2008], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        w = (u32)(arg0 * 9) >> 3;
        D_80063458[0]->unk8 += 0x18;
        right = x + w;
        p->x0 = x; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = x; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[0x2008], p);
    }
    ret = func_80023D7C();
    ret = ret - (ret / 24) * arg0;
    if (D_80060884.a != ret) {
        D_80060884.a = ret;
        SetGeomScreen(ret);
    }
}
