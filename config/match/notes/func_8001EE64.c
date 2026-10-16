#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F38C;
extern s32 D_8006CB10;
extern s32 D_8006CB14[];
extern s32 D_8006CB18[];
extern s32 D_8006CB58;
extern s32 D_8006CB60;
extern s32 D_8006CB64;

typedef struct {
    s32 f0;
    s32 pad4;
    s32 f8;
    u8 pad[0x3C];
    s32 f44;
    s32 f48;
    s32 f4C;
    u8 pad2[0x23C - 0x50];
} E;

s32 func_8001EE64(void) {
    s32 i;
    s32 off;
    s32 *p;
    s32 *q;
    s32 *a;
    E *e;
    s32 *t;

    D_8005F38C = 0;
    D_8006CB14[0] = func_8001148C(0xD680);
    D_8006CB14[-1] = func_8001148C(0x33C);
    t = D_8006CB14 - 1;
    if (D_8006CB14[0] == 0 || D_8006CB14[-1] == 0) {
        return -0xF;
    }
    D_8006CB60 = 0;
    D_8006CB64 = 0;
    *(s32 *)(D_8006CB10 + 8) = (s32)D_8006CB14 + 0x1468;
    D_8006CB58 = 2;
    D_8006CB14[0x12] = D_8006CB14[0];
    a = (s32 *)((s32)t + 0x14D8);
    for (i = 0, off = 0; i < 0x60; i++, off += 0x23C, a += 0xC) {
        e = (E *)(D_8006CB14[0] + off);
        e->f8 = (s32)a;
        e->f0 = 0;
        e->f44 = (s32)t + 0x48;
        e->f4C = 0;
        if (i < 0x5F) {
            e->f48 = D_8006CB14[0] + off + 0x23C;
        } else {
            e->f48 = 0;
        }
    }
    p = D_8006CB18;
    q = D_8006CB18 + 1;
    for (i = 0; i < 8; i++) {
        *p = 2;
        *q = 0;
        q += 2;
        p += 2;
    }
    *(s32 *)D_8006CB10 = 0;
    *(s32 *)(D_8006CB10 + 0x44) = 0;
    *(s32 *)(D_8006CB10 + 0x4C) = 0;
    *(s32 *)(D_8006CB10 + 0x48) = 0;
    return -0xFF;
}
