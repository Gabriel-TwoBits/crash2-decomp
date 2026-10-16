#include "common.h"
#include "sdk.h"

s32 func_8001CC10(s32, s32, s32, s32, void *);
s32 D_8005F38C;
extern s32 D_8006B1C4[];
extern s32 *D_8006CB10[];
extern s32 D_8006CB58[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;
s32 func_800189CC(s32 *, s32 *);
void func_80032808(s32 *);
s32 func_80019BBC(s32 *, s32);

s32 func_80019BBC(s32 *arg0, s32 arg1) {
    s32 *child;
    s32 *next;
    s32 *slot;
    s32 *base;
    s32 cur;

    if (arg0 != 0 && (s32)arg0 != D_8005F38C) {
        if (arg1 != 0) {
            func_8001CC10(0, (s32)arg0, 0x1A00, 0, 0);
        }
        child = (s32 *)arg0[0x4C / 4];
        while (child != 0) {
            next = (s32 *)child[0x48 / 4];
            func_80019BBC(child, arg1);
            child = next;
        }
        func_80032808(arg0);
        cur = D_8005F38C;
        D_8006B1C4[arg0[0xB8 / 4] >> 8] &= ~1;
        arg0[0] = 0;
        if ((s32)arg0 == cur) {
            if (D_8006CB58[0] == 2) {
                arg1 = D_8006CB5C;
            } else {
                arg1 = D_8006CBA4;
            }
            base = D_8006CB58;
            func_800189CC(arg0, base);
            /* the block wrapper changes the store scheduling */
            do {
                slot = base + 0x13;
                if (base[0] == 2) {
                    slot = base + 1;
                }
                *slot = arg1;
                D_8006CB10[0][0x44 / 4] = 0;
                D_8006CB10[0][0x4C / 4] = 0;
                D_8006CB10[0][0x48 / 4] = 0;
            } while (0);
            D_8005F38C = 0;
            return -0xFF;
        }
        func_800189CC(arg0, D_8006CB58);
    }
    return -0xFF;
}
