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

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

typedef struct {
    u8 pad0[4];
    u8 r, g, b;
    u8 pad7;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
} PolyF4;

extern Unk80063458 *D_80063458[];
extern Unk80060884 D_80060884;
void SetPolyF4(void *);
void SetGeomScreen(s32);

#define SCREEN_MODE_ALT 1
#define SCREEN_X_OFFSET 0x2034
#define SCREEN_Y_OFFSET 0x2036
#define PRIM_SIZE 0x18
#define OT_OFFSET 0x2008
#define SCREEN_CENTER_X 0x200
#define BAR_TOP -0xC
#define BAR_BOTTOM 0xE4

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
    D_80063458[0]->unk8 += PRIM_SIZE;
    if (*(s32 *)D_80063454 == SCREEN_MODE_ALT) {
        *(s16 *)(D_80063454 + SCREEN_X_OFFSET) = SCREEN_CENTER_X - x;
    } else {
        *(s16 *)(D_80063454 + SCREEN_X_OFFSET) = -x;
    }
    *(s16 *)(D_80063454 + SCREEN_Y_OFFSET) =0xC - ((u32)(arg0 * 60) >> 3);
    if (arg0 != 0) {
        if (arg1 == 0) {
            y = 0;
            x = 0;
        } else {
            x = (u32)(arg1 * 100) >> 3;
            y = (u32)(arg1 * 60) >> 3;
        }
        right = x + SCREEN_CENTER_X;
        left = right - w;
        top = y + BAR_TOP;
        bot = y + BAR_BOTTOM;
        p->x0 = left; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = left; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        D_80063458[0]->unk8 += PRIM_SIZE;
        mid = bot - arg0 * 15;
        p->x0 = x; p->y0 = mid; p->x1 = left; p->y1 = mid;
        p->x2 = x; p->y2 = bot; p->x3 = left; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        w = (u32)(arg0 * 9) >> 3;
        D_80063458[0]->unk8 += PRIM_SIZE;
        right = x + w;
        p->x0 = x; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = x; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
    }
    ret = ((s32 (*)(void))func_80023D7C)();
    ret = ret - (ret / 24) * arg0;
    if (D_80060884.a != ret) {
        D_80060884.a = ret;
        SetGeomScreen(ret);
    }
}
