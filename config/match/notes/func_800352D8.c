#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005C4B4[];

void func_800352D8(s8 *arg0, s32 arg1) {
    char buf[0x40];
    s16 *out;
    u8 *in;
    u8 *p;
    s32 done;
    s32 i;
    s32 j;
    u32 c;
    u8 t;

    done = 0;
    strcpy(buf, arg0);
    out = (s16 *)arg0;
    in = (u8 *)buf;
    for (i = 0; i < arg1; i++) {
        c = *in;
        if (done != 0) {
            *out = 0x8140;
        } else if (c - 0x61 < 0x1A) {
            *out = c - 0x7DE0;
        } else if (c - 0x41 < 0x1A || c - 0x30 < 0xA) {
            *out = c - 0x7DE1;
        } else {
            *out = 0x8140;
            if (D_8005C4B4[0] != 0xFF) {
                p = D_8005C4B4;
                j = 0;
                while (1) {
                    if (*p == (c & 0xFF)) {
                        *out = D_8005C4B4[j + 1] | 0x8100;
                        break;
                    }
                    p += 2;
                    if (*p == 0xFF) break;
                    j += 2;
                }
            }
            if (c == 0) {
                done = 1;
                *out = 0;
            }
        }
        in++;
        out++;
    }
    for (i = 0, p = (u8 *)arg0; i < arg1; i++, p += 2) {
        t = p[0];
        p[0] = p[1];
        p[1] = t;
    }
}
