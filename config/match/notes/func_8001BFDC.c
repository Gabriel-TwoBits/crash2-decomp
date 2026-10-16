#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80060944;
extern s32 D_8006CB64;
s32 func_8001BFD4(void *, s32);

typedef struct { u8 b[4]; } Bytes4;

s32 func_8001BFDC(u8 *arg0) {
    s32 sp18;
    s16 *s1;
    s32 v;
    u8 *a0;
    s16 t;

    s1 = (s16 *)(arg0 + 0x20);
    switch (*(s32 *)(arg0 + 0x108)) {
    case 6:
        if ((u32)(D_8006CB64 - *(s32 *)(arg0 + 0x10C)) >= 0x10) {
            goto clear;
        }
        break;
    case 7:
        if ((u32)(D_8006CB64 - *(s32 *)(arg0 + 0x10C)) >= 0x10 || (*(s32 *)(arg0 + 0xA8) & 1)) {
    clear:
            *(s32 *)(arg0 + 0x108) = 0;
            *(s32 *)(arg0 + 0xAC) |= 0x80;
        }
        break;
    case 4:
        if ((u32)(D_8006CB64 - *(s32 *)(arg0 + 0x10C)) >= 0x3D) {
            *(s32 *)(arg0 + 0x108) = 0;
        }
        break;
    case 3:
        if (*(s32 *)(arg0 + 0x108) == 3) {
            if ((u32)(D_8006CB64 - *(s32 *)(arg0 + 0x10C)) >= 0x1C4) {
                *(s32 *)(arg0 + 0x108) = 0;
            } else {
                goto b13;
            }
        } else {
    b13:
            if (*(s32 *)(arg0 + 0x108) == 4) {
                u8 *p = *(u8 **)(arg0 + 0x58);
                if (p != 0 && *(s32 *)(*(u8 **)(*(u8 **)(p + 0xC) + 0x10) + 4) == 0x300) {
                    sp18 = 0;
                    func_8001CC10((s32)arg0, *(s32 *)(arg0 + 0x58), 0xA00, 1, &sp18);
                }
            }
        }
    case 5:
        if (*(s32 *)(arg0 + 0x108) == 5 && ((u32)(D_8006CB64 - *(s32 *)(arg0 + 0x10C)) >= 0x25B || (*(s32 *)(arg0 + 0x100) & 0x20000000))) {
            *(s32 *)(arg0 + 0x108) = 0;
        }
    case 2:
        v = D_80060944;
        if (D_80060944 < 0) {
            v = D_80060944 + 3;
        }
        s1[0x14 / 2] = 0x3C;
        s1[0x16 / 2] = 0x3C;
        s1[0x18 / 2] = 0x3C;
        s1[2] = (D_80060944 - ((v >> 2) * 4)) << 10;
        break;
    case 1:
    default:
        func_8001BFD4(arg0, 1);
        break;
    }
    a0 = *(u8 **)(arg0 + 0xE8);
    if (a0 != 0) {
        if (*a0 != 4) {
            s1[3] = s1[1];
            *(Bytes4 *)((u8 *)s1 + 0x1A) = *(Bytes4 *)((u8 *)s1 + 0xE);
            s1[0xF] = s1[9];
            t = s1[0];
            if (t != 0) {
                s1[3] = t;
                *(Bytes4 *)((u8 *)s1 + 0x1A) = *(Bytes4 *)((u8 *)s1 + 8);
                s1[0xF] = s1[6];
            }
            t = s1[2];
            if (t != 0) {
                s1[3] = t;
                *(Bytes4 *)((u8 *)s1 + 0x1A) = *(Bytes4 *)((u8 *)s1 + 0x14);
                s1[0xF] = s1[0xC];
            }
        }
    }
    return -0xFF;
}
