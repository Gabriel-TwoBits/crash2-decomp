#include "common.h"
#include "sdk.h"

typedef struct {
    s32 a;
    s32 flags;
    s32 pad8;
    s32 key;
    u8 pad10[0xD];
    u8 b;
    u8 pad1E[0x32];
} Ent;

extern s32 func_80032F18(s32, s32, s32 *, s32);
s32 D_8005F244 = 1;
s32 D_8005F304;
extern Ent D_8005F4EC[];

void func_80032808(s32 arg0) {
    s32 sp10;
    s32 i;
    Ent *p;

    if (arg0 != 0) {
        i = D_8005F304;
        p = &D_8005F4EC[i];
        for (; i < 0x18; i++, p++) {
            if ((p->flags & 8) && p->key == arg0) {
                if (i) {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                } else {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                }
            }
        }
    }
}
