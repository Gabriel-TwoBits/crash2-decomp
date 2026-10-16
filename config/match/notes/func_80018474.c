#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0;
    u8 b1;
    u16 h2;
    u8 pad4;
    u8 b5;
    u16 h6;
    u8 pad8;
    u8 b9;
    u8 padA;
    u8 bB;
} Unk80018474;

void func_80018474(void *a0, void *a1) {
    Unk80018474 *arg0 = a0;
    s32 **arg1 = a1;
    s32 idx;
    s32 v;
    u32 r;
    u32 flag;
    u32 hi;

    idx = arg0->h6 >> 10;
    if (!(arg0->h6 & 0x200)) {
        v = func_800143C0(arg1 + idx, 0);
        if (v != 0) {
            r = (u32)func_80014260((void *)v, 3, 0);
            hi = (r >> 4) & 0x6030;
            flag = r & 0x80;
            arg0->h6 = (arg0->h6 & 0xFFE3) | (r & 0x1C) | 0x200;
            arg0->b5 = (arg0->b5 & 0x7F) | flag;
            arg0->b9 = (arg0->b9 & 0x7F) | flag;
            arg0->bB = (arg0->bB & 0x7F) | flag;
            arg0->h2 = (arg0->h2 & 0x9FCF) | hi;
            arg0->b1 = (arg0->b1 & 0x7F) | flag;
        }
    }
    *(volatile s32 *)arg1[idx];
}
