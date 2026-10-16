#include "common.h"
#include "sdk.h"

s32 func_8001CC10(s32, s32, s32, s32, void *);
s32 D_8005F38C;
extern s32 D_8006B1C4[];
typedef struct {
    s32 pad[0x44 / 4];
    s32 f44;
    s32 f48;
    s32 f4C;
} Unk80019BBC;
extern Unk80019BBC *D_8006CB10;
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
        D_8006B1C4[arg0[0xB8 / 4] >> 8] &= ~1;
        if ((s32)arg0 == D_8005F38C) {
            arg0[0] = 0;
            if (D_8006CB58[0] == 2) {
                arg1 = D_8006CB5C;
            } else {
                arg1 = D_8006CBA4;
            }
            base = D_8006CB58;
            func_800189CC(arg0, base);
            slot = base + 0x13;
            if (base[0] == 2) {
                slot = base + 1;
            }
            *slot = arg1;
            D_8006CB10->f44 = 0;
            D_8006CB10->f4C = 0;
            D_8006CB10->f48 = 0;
            D_8005F38C = 0;
            return -0xFF;
        }
        arg0[0] = 0;
        func_800189CC(arg0, D_8006CB58);
    }
    return -0xFF;
}
