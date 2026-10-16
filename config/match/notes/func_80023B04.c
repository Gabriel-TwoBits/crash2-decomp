#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Arg80023B04;

typedef struct {
    s32 pad0[4];
    s32 unk10[1];
} Ctx80023B04;

extern s32 D_800608D4;
extern s32 D_800608D8;
extern s32 D_800608DC;
extern s32 D_800608F0;
extern s32 D_800608FC;
extern s32 D_8005BA28[];

void func_80023B04(Arg80023B04 *arg0, s32 *arg1, s32 *arg2) {
    s32 sp28;
    s32 key;
    s32 *tbl;
    s32 idx;
    s32 a;
    s16 *p;
    s32 x, y, w, x1, y1;
    s32 *obj;
    s32 arg;
    s32 unk8;

    if (arg0->unk4 == D_800608D4) {
        key = D_800608F0;
        arg = D_800608DC;
    } else {
        key = func_800321D4(arg0->unk4, 0x29, 0, 0, 0, 2, 0);
        idx = func_800321D4(arg0->unk4, 0x173, 0, 0, 0, 2, 0);
        obj = (s32 *)arg0->unk0;
        arg = *(s32 *)((u8 *)obj + (idx * 3 + D_800608FC + *(s32 *)(*(s32 *)((u8 *)obj + 0x10) + 0x184)) * 4 + 0x10);
    }
    tbl = D_8005BA28;
    unk8 = arg0->unk8;
    if (key == 0) {
        tbl = D_8005BA28 + 6;
    }
    p = (s16 *)func_80031AE8(arg, 0x142, unk8, 0, 0, 3, &sp28, 0, 0);
    if (p == 0) {
        if (arg0->unk4 == D_800608D4) {
            arg = D_800608D8;
        } else {
            obj = (s32 *)arg0->unk0;
            arg = *(s32 *)((u8 *)obj + (idx * 3 + *(s32 *)(*(s32 *)((u8 *)obj + 0x10) + 0x184)) * 4 + 0x14);
        }
        p = (s16 *)func_80031AE8(arg, 0x142, unk8, 0, 0, 3, &sp28, 0, 0);
    }
    switch (key) {
    case 0:
    case 3:
    case 4:
    case 10:
        w = tbl[3];
        x = p[0] << 8;
        y = p[1] << 8;
        if (w != 0) {
            x1 = p[2] << 8;
            y1 = p[3] << 8;
            if (w == 0x400) {
                x = x1;
                y = y1;
            } else {
                x += ((x1 - x) * w) >> 10;
                y += ((y1 - y) * w) >> 10;
            }
        }
        break;
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        x = p[0] << 8;
        y = p[1] << 8;
        break;
    }
    *arg1 = x;
    *arg2 = y;
}
