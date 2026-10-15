#include "common.h"
#include "sdk.h"
#include "game.h"
typedef struct { char b[4]; } V4;
typedef struct { char p[0x1C]; s32 f1c; char p2[4]; V4 f24; V4 f28; V4 f2c; } S;
void func_80034238(void *, s32, void *, void *);
extern s32 D_8003BEA4;
extern V4 D_80060948;
extern V4 D_8006094C;
extern V4 D_80060950;
extern s32 D_80060954;
extern s32 D_8006CBE8;

void func_80020998(S *arg0, s32 arg1) {
    D_80060954 = D_8003BEA4;
    D_80060948 = D_8006094C;
    D_80060950 = arg0->f2c;
    D_8006CBE8 = arg0->f1c;
    if ((arg1 & 4) == 0) {
        func_80034238(&arg0->f24, arg1 & 4, &D_80060950, arg0);
    }
}
