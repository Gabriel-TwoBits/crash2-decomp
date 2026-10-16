#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001CC10(s32, s32, s32, s32, void *);
extern s32 D_8005F38C;
s32 func_80031AE8(s32, s32, s32, s32, s32 *, s32, s32 *, s32, s32);
s32 func_800322C0(s32, s32, s32, s32);
void func_80012DC0(void);

void func_80023084(s32 arg0, s32 arg1) {
    s32 *p;
    s32 n;
    s32 len;
    s32 kind;
    s32 step;

    p = (s32 *)func_80031AE8(arg0, 0x198, arg1, 0, 0, 1, 0, 0, 0);
    if (p != 0) {
        n = func_800322C0(arg0, 0x198, arg1, 1);
        if (n != 0) {
            do {
                kind = p[1];
                len = p[0];
                if (kind >= 0) {
                    func_8001CC10(0, D_8005F38C, kind, len - 2, p + 2);
                    step = len * 4;
                } else {
                    func_80012DC0();
                    step = len * 4;
                }
                n -= len;
                p = (s32 *)((char *)p + step);
            } while (n != 0);
        }
    }
}
