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
extern void SpuSetReverbDepth(Unk80032588 *);

s32 func_800325DC(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s32 arg4) {
    Unk80032588 s;

    s.a = 7;
    s.b = arg0 | 0x100;
    s.c = arg1;
    s.d = arg2;
    if ((u32)(arg0 - 7) < 2U) {
        s.e = arg3;
        s.f = arg4;
    }
    func_800501A8(&s);
    SpuSetReverbDepth(&s);
    func_8004FE38(arg0 != 0);
    return 0;
}
