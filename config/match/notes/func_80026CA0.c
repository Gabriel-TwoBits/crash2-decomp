#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_800200A8(s32, s32, s32, s32);
extern void func_80021A08(s32 *, s32);
extern void func_80021F4C(s32 *, s32);
extern void func_80022314(s32 *, s32);
extern void func_800225C0(s32 *, s32);
extern void func_8002271C(s32 *, s32);
extern void func_80022878(s32 *);
extern void func_80022A5C(s32 *);
extern void func_80022E74(s32 *);
extern void func_80022F58(s32 *);
extern void func_800237F8(void);
extern void func_80025D18(s32 *, s32 *);
extern s8 D_8005B994;
extern s8 D_8005B99B;
extern s32 D_8005F38C;
extern s32 D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern u32 D_800608F0;
extern s32 D_80062AD8;
extern s32 D_80069948;
extern s32 D_8006CB94[];
extern s32 D_8006CBB4;

s32 func_80026CA0(void) {
    s32 sp18;
    s32 s1;
    s32 *s0;
    s32 *p;

    if ((D_80062AD8 & 0x400) && (D_80069948 & 0xE0)) {
        sp18 = 0x8000;
        func_80025FDC(0, &sp18);
        func_8001D194(0, 0x4300, 0, 0, 0);
    }
    if (D_8005F38C == 0) {
        return 0;
    }
    p = D_8006CB94;
    if (!(*p & 2)) {
        return 0;
    }
    D_8005B99B = 0;
    s1 = 0;
    if (*p & 0x20000) {
        s0 = p + 0x77;
    } else {
        s0 = (s32 *)(D_8005F38C + 0x60);
    }
    func_80025D18(s0, p);
    func_800237F8();
    func_8002609C();
    D_8006CBB4 = ((D_80062AD8 & 0x400) == 0) << 8;
    if (D_8005B994 != 5) {
        switch (D_8005B994) {
        case 6:
            func_80022A5C(s0);
            break;
        case 7:
            func_80022E74(s0);
            s1 = 1;
            break;
        case 9:
            func_80022F58(s0);
            s1 = 1;
            break;
        default:
            break;
        }
        if (D_8005B994 != 5) {
            return 1;
        }
    }
    switch (D_800608F0) {
    case 0: func_8002271C(s0, s1); break;
    case 2: func_80022878(s0); break;
    case 4: func_80021A08(s0, s1); break;
    case 10: func_80021F4C(s0, s1); break;
    case 3: func_80022314(s0, s1); break;
    case 8: func_800225C0(s0, s1); break;
    default: func_800200A8(D_800608CC, D_800608D4, D_800608E0, 0); break;
    }
    return 1;
}
