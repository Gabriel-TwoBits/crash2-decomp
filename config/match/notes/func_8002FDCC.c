#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80019800(void *, s32, s32, s32, s32 *, s32);
void func_8002FCF8(void);
extern s32 D_8005F3D8;
extern s32 D_800675AC;
extern s32 D_8006CB20;

void func_8002FDCC(s32 *arg0) {
    s32 buf[2];
    s32 *temp_v0;

    D_800675AC = 0;
    temp_v0 = func_80014260(arg0, 1, 1);
    *arg0 = 0x6396347F;
    D_8005F3D8 = temp_v0[4];
    buf[1] = 0x6400;
    buf[0] = 0;
    func_80019800(&D_8006CB20, 4, 8, 2, buf, 1);
    func_8002FCF8();
}
