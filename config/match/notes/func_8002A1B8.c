#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80062AD8;

typedef struct Unk8002A1B8 {
    u8 pad0[0xA8];
    u32 flags;
    u8 padAC[0x100 - 0xAC];
    u32 f100;
    u8 pad104[0x11C - 0x104];
    s32 f11C;
} Unk8002A1B8;

s32 func_8002A1B8(Unk8002A1B8 *p, s32 arg1) {
    s32 a3;
    u32 t;
    s32 r;

    if (arg1 != 0) {
        a3 = ((u32)(arg1 & 0xE) >> 1) + 1;
    } else {
        a3 = 0;
    }
    switch ((u32)(arg1 & 0x3F0) >> 4) {
    case 1: p->f11C = 0x700; p->flags |= 0x400; break;
    case 2: p->f11C = 0xC00; p->flags |= 0x400; break;
    case 5: p->f11C = 0x1E00; p->flags |= 0x400; break;
    case 4:
        p->f11C = 0x1F00;
        p->flags |= 0x400;
        if ((p->f100 & 0x40000000) && a3 == 1) {
            p->f11C = 0x900;
            return 1;
        }
        break;
    case 3: p->f11C = 0x2100; p->flags |= 0x400; break;
    case 12: p->f11C = 0x2300; p->flags |= 0x400; break;
    case 11:
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    case 6: p->f11C = 0xD00; p->flags |= 0x400; break;
    case 13: p->f11C = 0x3300; p->flags |= 0x400; break;
    case 18: p->f11C = 0x4200; p->flags |= 0x400; break;
    case 14: p->f11C = 0x3500; p->flags |= 0x02000400; break;
    case 15: p->flags |= 0x04000000; break;
    case 7: case 8: case 9: case 10:
        p->f11C = 0x1200; p->flags |= 0x2000; break;
    case 27: p->f11C = 0x3000; p->flags |= 0x400; break;
    case 28: p->f11C = 0x3100; p->flags |= 0x400; break;
    case 29: case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 37: case 38:
        p->f11C = ((u32)(arg1 & 0x3F0) >> 4) + 0x40000; p->flags |= 0x400; break;
    default:
        break;
    }
    if (a3 == 4) {
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    } else if (a3 == 6) {
        p->f11C = 0x4000;
        p->flags |= 0x400;
        return 0;
    } else if (a3 == 5) {
        return 1;
    } else if (a3 == 3) {
        if (!(D_80062AD8 & 0x200)) {
            return 1;
        }
        r = 1;
        if ((p->f100 & 0x24000000) == 0x24000000) {
            r = 0;
        }
        return r;
    }
    return 0;
}
