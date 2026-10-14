#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_80023154(void);
extern s8 D_8005B99B;
extern s32 D_8005B9C4;
extern s32 D_800608F8;

void func_800221DC(void) {
    D_800608F8 = 0;
    if (D_8005B99B != 0) {
        D_8005B9C4 = 0;
        return;
    }
    func_80023154();
}
