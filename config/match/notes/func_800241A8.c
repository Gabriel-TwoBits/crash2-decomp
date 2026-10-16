#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80022224(s32);
extern s8 D_8005B994;
extern s8 D_8005B99A;
extern s32 D_8005B9AC;
extern s32 D_8005B9BC;
extern s32 D_8005B9C0;
extern s32 D_8005B9C4;
extern s32 D_8005B9E0;
extern s32 D_8005B9E4;
extern s32 D_8005B9F4;
extern s32 D_8005B9F8;
extern s32 D_8005B9FC;
extern s32 D_8005BA08;
extern s32 D_8005BA0C;
extern s32 D_8005BA10;
extern s32 D_800608F0;
extern s32 D_800608F8;
extern s32 D_80060918;
extern s32 D_8006CD98;

void func_800241A8(void) {
    D_8005B994 = 5;
    D_80060918 = 0x1001;
    D_8005B9AC = 0x1001;
    D_800608F0 = 5;
    D_800608F8 = 0;
    D_8005B9E0 = 0;
    D_8005B9E4 = 0;
    D_8005B9C4 = 0;
    D_8005B99A = 0;
    D_8005B9BC = 0;
    D_8005B9C0 = 0;
    D_8005B9FC = 0;
    D_8005B9F8 = 0;
    D_8005B9F4 = 0;
    D_8005BA10 = 0;
    D_8005BA0C = 0;
    D_8005BA08 = 0;
    D_8006CD98 = 0x100;
    func_80022224(1);
}
