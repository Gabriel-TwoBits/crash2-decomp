#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001A90C(void *, void **, void *);
extern s32 D_8005F38C;
void *func_80014260(void *, s32, s32);

typedef struct {
    u8 pad0[0x20];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30[6];
} Unk8001A4A4Inner;

typedef struct {
    u8 pad0[8];
    Unk8001A4A4Inner *inner;
    u8 pad1[0xDC];
    u8 *e8;
} Unk8001A4A4Obj;

void func_8001A4A4(Unk8001A4A4Obj *arg0, s32 *arg1) {
    s32 sp[6];
    s32 *s;
    s32 neg;
    s32 r;
    u8 *a;

    arg0->inner->f28 = 0;
    a = arg0->e8;
    if (a != 0) {
        if (*a != 4) {
            if (((s32)(a + 4) & 0xFF800003) != 0x80000000) {
                return;
            }
            r = (s32)func_80014260(a + 4, 1, 0);
            arg0->inner->f28 = r;
            if (r >= -0xFE) {
                arg0->inner->f28 = 0;
                return;
            }
        }
        if (*arg0->e8 == 1) {
            do {
                arg0->inner->f2C = func_8001A90C(arg0, (void **)&s, &arg0->inner->f24);
            } while (0);
            arg0->inner->f28 = (s32)func_80014260((s32 *)(arg0->inner->f20 = (s32)s) + 4, 1, 0);
            if ((s32)arg0 == D_8005F38C) {
                neg = arg1[0];
                if (neg < 0) {
                    arg1[0] = -neg;
                }
                sp[0] = s[7] + s[10];
                sp[1] = s[8] + s[11];
                sp[2] = s[9] + s[12];
                sp[3] = s[7] + s[13];
                sp[4] = s[8] + s[14];
                sp[5] = s[9] + s[15];
                arg0->inner->f30[0] = (sp[0] >> 8) * arg1[0] >> 4;
                arg0->inner->f30[1] = (sp[1] >> 8) * arg1[1] >> 4;
                arg0->inner->f30[2] = (sp[2] >> 8) * arg1[2] >> 4;
                arg0->inner->f30[3] = (sp[3] >> 8) * arg1[0] >> 4;
                arg0->inner->f30[4] = (sp[4] >> 8) * arg1[1] >> 4;
                arg0->inner->f30[5] = (sp[5] >> 8) * arg1[2] >> 4;
                arg1[0] = neg;
            }
        } else {
            arg0->inner->f28 = -0xE;
        }
    }
}
