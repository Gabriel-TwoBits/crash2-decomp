#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_SIZE 0x2C
extern char D_80062BB8[];
extern char D_80062BBA[];
extern char D_80062BC0[];
extern char D_80062BCC[];
extern char D_80062BD0[];
extern s32 D_80067FB8;

#define F16(sym, o) (*(s16 *)((sym) + (o)))
#define F32(sym, o) (*(s32 *)((sym) + (o)))

void func_80015344(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 pos;
    s32 i;
    s32 last;
    s32 count;
    s32 rem;
    s32 off;
    s32 t;
    s16 *p;

    pos = 0x80000 - arg1;
    count = (arg2 + 0xFF8F) / 65424;
    last = count - 1;
    i = last;
    if (i >= 0) {
        rem = arg2 % 65424;
        off = i * ENTRY_SIZE;
    loop:
        F16(D_80062BC0, off) = i;
        F16(D_80062BB8, off) = 1;
        F16(D_80062BBA, off) = 4;
        if (i == last) {
            pos -= ((rem + 0xF) / 16) * 16;
        } else {
            pos += 0xFFFF0070;
        }
        F32(D_80062BD0, off) = pos;
        i--;
        off -= ENTRY_SIZE;
        if (i >= 0) {
            goto loop;
        }
    }
    D_80067FB8 = pos;
    rem = pos;
    i = count;
    while (i < count + (arg4 < 0 ? -arg4 : arg4)) {
        pos -= arg3;
        F16(D_80062BC0, i * ENTRY_SIZE) = i;
        F16(D_80062BB8, i * ENTRY_SIZE) = 1;
        F16(D_80062BBA, i * ENTRY_SIZE) = 5;
        F32(D_80062BCC, i * ENTRY_SIZE) = 0x6396347F;
        F32(D_80062BD0, i * ENTRY_SIZE) = pos;
        i++;
    }
    if (arg4 < 0) {
        pos = rem;
    }
    p = (s16 *)(D_80062BB8 + i * ENTRY_SIZE);
    for (; i < 10; i++, p += ENTRY_SIZE / 2) {
        t = pos + 0xFFFF0000;
        F16(D_80062BC0, i * ENTRY_SIZE) = i;
        F16(D_80062BBA, i * ENTRY_SIZE) = 3;
        if (t < 0x1010) {
            *p = 0x1E;
        } else {
            pos = t;
            *p = 1;
            F32(D_80062BD0, i * ENTRY_SIZE) = pos;
        }
    }
}
