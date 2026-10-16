#include "common.h"
#include "sdk.h"
#include "game.h"

struct Unk80039310 {
    u8 pad0[0x40];
    s32 regs[32];
    u8 padC0[0xA8 - 0xC0 + 0x18];
};

s32 func_80039310(u8 *o, u32 arg1, s32 arg2) {
    s32 args[16];
    s32 *src;
    s32 *p;
    s32 *q;
    s32 a;
    s32 n;
    s32 i;
    s32 sel;
    s32 idx;
    s32 first;

    first = *(s32 *)(o + 0x40 + ((arg1 >> 19) & 0x1C));
    idx = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            src = *(s32 **)(o + 0xBC) - 1;
            *(s32 **)(o + 0xBC) = src;
        } else {
            src = (s32 *)(o + 0x40 + (arg1 & 0x1FF) * 4);
        }
    } else {
        src = func_80036D0C((struct Obj *)o, idx);
    }
    *(u32 *)(o + 0xA8) &= 0xFFFDFFFF;
    sel = (arg1 >> 12) & 0x3F;
    if (sel == 0x1F) {
        q = *(s32 **)(o + 0xBC) - 1;
        *(s32 **)(o + 0xBC) = q;
    } else {
        q = (s32 *)(o + 0x40 + sel * 4);
    }
    if (*q != 0 && first != 0 && src != 0) {
        n = (arg1 >> 18) & 7;
        if (n != 0) {
            p = args;
            for (i = 0; i < n; i++) {
                *p++ = (*(s32 **)(o + 0xBC))[i - n];
            }
        }
        func_8001CC10((s32)o, first, *src, n, args);
    } else {
        *(s32 *)(o + 0xD4) = 0;
    }
    if (!(*(u32 *)(o + 0xA8) & 0x20000)) {
        *(s32 **)(o + 0xBC) = (s32 *)((u8 *)*(s32 **)(o + 0xBC) - ((arg1 >> 16) & 0x1C));
        return 0;
    }
    if (!(arg2 & 2)) {
        *(s32 **)(o + 0xBC) = *(s32 **)(o + 0xBC) - 1;
        return 0;
    }
    return -0xFF;
}
