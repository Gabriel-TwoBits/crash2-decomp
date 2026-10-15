#include "common.h"
#include "sdk.h"
#include "game.h"

int MoveImage(RECT *rect, int x, int y);

extern u8 *D_80063454;
extern u8 *D_80069358;

void func_8002706C(void) {
    RECT rect;
    u8 *p;
    s32 w;

    p = D_80069358;
    if (p != 0) {
        rect.x = 0;
        rect.y = 0x108;
        w = *(s32 *)(p + 0x18);
        rect.h = 0xF0;
        rect.w = w;
        MoveImage(&rect, *(s16 *)(D_80063454 + 0x202C), 0);
    }
}
