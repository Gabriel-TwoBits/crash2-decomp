#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 g_Player;

typedef struct {
    u8 pad0[0x20];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 f30[6];
} Unk8001A4A4Inner;

/* What GoolObject.unk_8 points to, as this function reads it */
#define INNER(obj) ((Unk8001A4A4Inner *)(obj)->unk_8)

void func_8001A4A4(GoolObject *arg0, s32 *arg1) {
    s32 sp[6];
    s32 *s;
    s32 neg;
    s32 r;
    u8 *a;

    INNER(arg0)->f28 = 0;
    a = arg0->anim;
    if (a != 0) {
        if (*a != 4) {
            if (((s32)(a + 4) & 0xFF800003) != 0x80000000) {
                return;
            }
            r = (s32)Nsf_OpenEntry(a + 4, 1, 0);
            INNER(arg0)->f28 = r;
            if (r >= -0xFE) {
                INNER(arg0)->f28 = 0;
                return;
            }
        }
        if (*arg0->anim == 1) {
            do {
                INNER(arg0)->f2C = func_8001A90C(arg0, (s32 *)&s, &INNER(arg0)->f24);
            } while (0);
            INNER(arg0)->f28 = (s32)Nsf_OpenEntry((s32 *)(INNER(arg0)->f20 = (s32)s) + 4, 1, 0);
            if ((s32)arg0 == g_Player) {
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
                INNER(arg0)->f30[0] = (sp[0] >> 8) * arg1[0] >> 4;
                INNER(arg0)->f30[1] = (sp[1] >> 8) * arg1[1] >> 4;
                INNER(arg0)->f30[2] = (sp[2] >> 8) * arg1[2] >> 4;
                INNER(arg0)->f30[3] = (sp[3] >> 8) * arg1[0] >> 4;
                INNER(arg0)->f30[4] = (sp[4] >> 8) * arg1[1] >> 4;
                INNER(arg0)->f30[5] = (sp[5] >> 8) * arg1[2] >> 4;
                arg1[0] = neg;
            }
        } else {
            INNER(arg0)->f28 = -0xE;
        }
    }
}
