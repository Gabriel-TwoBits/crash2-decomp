#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001B83C(void *, char *, s32, s32, void *, s32, s32);
s32 func_8003D3CC(void *, void *, s32, s32, void *, s32, s32, s32);
extern char D_8006083C[];
extern s32 D_80060884;

void func_8001BD84(u8 *arg0, char *arg1, s32 arg2, s32 arg3, s32 arg4) {
    char buf[200];
    s32 i;
    s32 t;
    s32 cnt;
    s32 off;
    s32 neg;
    void *mem;
    s32 *q;
    char *p;
    char *base = arg1;

    t = *(s32 *)(arg0 + 0x78);
    if (t < 0) {
        t = -t;
    }
    cnt = t / 27279;
    if (func_8003D3CC(arg0 + 0x60, D_8006083C, (u32)(arg0[0x120] - 6) < 2U, cnt, D_8006083C - 0x68, D_80060884, 0x3C00,
                      ((*(u32 *)(arg0 + 0xAC) >> 18) ^ 1) & 1) != 0) {
        arg1 = base + 0xC;
        for (i = 0; i < arg2;) {
            if (*arg1 == 0) {
                i++;
            }
            arg1++;
        }
        off = *(u32 *)(arg0 + 0x108) >> 8;
        if (off == 0) {
            off = *(s32 *)(base + 8);
        }
        arg2 = *(s32 *)(*(u8 **)(arg0 + 0xC) + 0x24) + off * 4;
        mem = func_80014260((void *)(arg2 + 4), 1, 0);
        q = *(s32 **)(arg0 + 0xBC);
        sprintf(buf, arg1, q[-2], q[-3], q[-4], (*(s32 **)(arg0 + 0xBC))[-5]);
        arg1 = buf;
        for (p = arg1; *p != 0; p++) {
        }
        if (p != arg1) {
            p--;
            if (p != arg1) {
                do {
                    if (*p != 0x20) {
                        break;
                    }
                    p--;
                } while (p != arg1);
            }
            p[1] = 0;
        }
        neg = 0;
        if (arg4 <= 0) {
            neg = func_8001B83C(arg0, arg1, 0, arg2, mem, cnt, 0);
            neg = -neg;
            if (arg4 == 0) {
                neg = neg / 2;
            }
        }
        func_8001B83C(arg0, arg1, neg, arg2, mem, cnt, arg3);
    }
}
