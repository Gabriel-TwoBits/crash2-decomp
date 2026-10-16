#include "common.h"
#include "sdk.h"
#include "game.h"

extern u32 D_8005FC84;
extern s32 D_8005FC8C;
extern s32 D_8005FC94;
extern s32 D_8005FC98;
extern u8 D_8005FC9C;
extern u8 D_8005FC9D;
extern s16 D_8005FC9E;
extern s16 D_8005FCA0;
extern s16 D_8005FCA2;
extern s32 D_8005FCA4;
extern s32 D_8005FCA8;
extern s32 D_8005FCAC;

void func_800323AC(void) {
    D_8005FC94 = 0x6396347F;
    D_8005FC9C = 1;
    D_8005FC9D = 0x80;
    D_8005FC9E = 0x3FFF;
    D_8005FCA2 = 0x1000;
    D_8005FC8C = 0;
    D_8005FCA0 = 0;
    D_8005FC98 = 0;
    D_8005FCA4 = 0;
    D_8005FCA8 = 0;
    D_8005FCAC = 0;
    D_8005FC84 &= ~2;
    D_8005FC84 &= ~1;
    D_8005FC84 &= ~4;
    D_8005FC84 &= ~8;
    D_8005FC84 &= ~0x10;
    D_8005FC84 &= ~0x20;
    D_8005FC84 &= ~0x40;
    D_8005FC84 &= ~0x80;
    D_8005FC84 &= ~0x100;
    D_8005FC84 |= 0x600;
    D_8005FC84 &= ~0x800;
    D_8005FC84 &= ~0x7000;
    D_8005FC84 &= 0xFFFF7FFF;
}
