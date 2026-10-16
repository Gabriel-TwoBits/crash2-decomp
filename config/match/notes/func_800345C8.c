#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800346E8(void);
void func_80034898(void);
void func_80034BB8(void);
void func_80035444(s32);

s32 *D_8005F264 = 0;
extern s32 D_8006CC5C;
extern s32 D_8006CC64;
s32 D_8005F26C = -1;
s32 D_8005F270 = -1;

void func_800345C8(void) {
    s32 v;
    s32 *new_var;

    if (D_8005F26C != func_80036150()) {
        D_8005F26C = func_80036150();
        func_80035A6C();
    }
    if (D_8005F270 != func_8003613C()) {
        D_8005F270 = func_8003613C();
        func_80035A74();
    }
    v = func_8003613C();
    switch (v) {
    case 0:
        func_800346E8();
        break;
    case 1:
        func_80034898();
        break;
    case 5:
        func_80034BB8();
        break;
    case 6:
    case 7:
    case 8:
        func_80035444(func_8003613C());
        break;
    case 3:
        func_800359F0();
        break;
    case 4:
        func_80034810();
        break;
    default:
        break;
    }

    v = func_80036150();
    D_8006CC5C = v;
    new_var = &D_8005F264[2];
    D_8006CC64 = *new_var;
}
