#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800116D4(s32, s32);
s32 D_8005F1A4 = 1;
extern char D_8005F1A8[];
int CdIntToPos(int, unsigned char *);
int CdRead(int, s32, int);
int printf(const char *, ...);

s32 func_800123F0(s32 *a0, s32 arg1, s32 arg2) {
    u8 sp10[8];
    u8 sp18[8];
    s32 s2;
    s32 *arg0;
    s32 ret;

    ret = D_8005F1A4;
    if (ret == 0) {
        goto end;
    }
    s2 = 0;
    if (arg2 != 1) {
        return arg1;
    }
    arg0 = a0;
    if (arg1 == 0) {
        s2 = (((arg0[1] & 0x1FFFF) + 0x7FF) / 2048) << 11;
        arg1 = func_8001148C(s2);
        if (arg1 == 0) {
            return -0xF;
        }
    }
loop:
    CdIntToPos(arg0[0] & 0x7FFFFF, sp18);
    if (CdControl(2, sp18, sp10) == 0) {
        goto loop;
    }
    if (CdRead((u32)(((arg0[1] & 0x1FFFF) + 0x7FF) / 2048), arg1, 0x80) == 0 || func_80012ED0(1) == -0x10) {
        printf(D_8005F1A8);
        goto loop;
    }
    if (s2 != 0 && s2 != (arg0[1] & 0x1FFFF)) {
        arg1 = func_800116D4(arg1, arg0[1] & 0x1FFFF);
    }
    ret = arg1;
end:
    return ret;
}
