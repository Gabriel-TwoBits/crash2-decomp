#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 a;
    s32 b;
    s16 c;
    s16 d;
    s32 e;
    s32 f;
} Unk80032588;

extern void func_8004FE38(s32);
extern void func_800501A8(Unk80032588 *);

s32 D_8005F248;

void func_80032588(void) {
    Unk80032588 s;

    if (D_8005F248 != 0) {
        s.a = 7;
        s.b = 0;
        s.c = 0;
        s.d = 0;
        func_800501A8(&s);
        func_8004FE38(0);
        D_8005F248 = 0;
    }
}
