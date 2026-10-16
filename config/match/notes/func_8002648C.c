#include "common.h"
#include "sdk.h"
#include "game.h"

extern s8 D_8005B994;
extern s8 D_8005B99B;
extern s32 D_8005BA14;
extern s32 D_8005BA18;
extern s32 D_8005BA1C;
extern s32 D_8005BA20;
extern s32 D_8005BA24;
extern s32 D_8005BAD8;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_8005BAE4;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 *D_800608D4;
extern s32 D_800608E4;
extern s32 D_800608F8;
extern s32 D_800608FC;
extern s32 D_80060900;
extern s32 D_80060904;

typedef struct { s8 a; s8 b; s8 c; } Arg;
typedef struct { u8 a; u8 b; u8 c; u8 d; } Ent;

void func_8002648C(Arg *arg0, s32 arg1) {
    Ent *e;
    s32 *p;
    s32 *v;
    s32 s;
    s32 a2;

    switch ((s8)(arg0->c - 1)) {
    case 3:
        if (D_800608FC != arg0->b) {
            func_80022224(arg0->b);
            func_80023154();
        }
        break;
    case 1:
        if (arg1 == 1) {
            D_80060900 = (u8)arg0->b;
        } else {
            D_80060904 = (u8)arg0->b;
        }
        break;
    case 5:
        D_8005B99B = 1;
        break;
    case 0:
        if (D_8005B994 != 7) {
            D_8005B994 = 7;
            goto common;
        }
        break;
    case 2:
        if (D_8005B994 != 6) {
            D_8005B994 = 6;
    common:
        a2 = 0;
        if (arg1 != 1) a2 = D_800608E4;
        e = (Ent *)((u8 *)func_80031AE8((s32)D_800608D4, 0x109, a2, 0, 0, 1, 0, 0, 0) + arg0->b * 4);
        p = (s32 *)(((s32 *)D_800608D4[1])[4] + (e->c * 4 + 0x194));
        v = (s32 *)func_80014260(p, 1, 0);
        s = *(s32 *)((u8 *)v + (e->b * 3 + *(s32 *)(v[4] + 0x184)) * 4 + 0x10);
        D_8005BAD8 = *p;
        D_8005BADC = e->b;
        if (e->a == 1) {
            D_8005BAE0 = 0;
        } else {
            D_8005BAE0 = (func_800322C0(s, 0x4B, 0, 2) - 1) << 8;
        }
        D_8005BAE4 = arg1;
        if (arg0->c == 3) {
            if (arg0->a != 0) {
                D_8005BA18 = 0x10;
                D_8005BA14 = 0x10;
                D_8005BA1C = D_800607F4;
                D_8005BA20 = D_800607F8;
                D_8005BA24 = D_800607FC;
            }
        }
        if (arg0->c == 1) {
            func_80023084(s, D_8005BAE0);
        }
        }
        break;
    case 4:
        if (D_8005B994 != 9) {
            D_8005B994 = 9;
            D_800608F8 = arg1;
        }
        break;
    default:
        break;
    }
}
