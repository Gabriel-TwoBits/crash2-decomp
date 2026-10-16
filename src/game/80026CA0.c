#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80026CA0);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80026EF4);

extern u8 *D_80069358;

extern u8 *D_80063454;

void func_8002706C(void) {
    RECT rect;
    u8 *p;
    s32 w;

    p = D_80069358;
    if (p != 0) {
        rect.x = 0;
        rect.y = 0x108;
        w = *(s32 *)(p + 0x18);
        rect.h = 0xF0;
        rect.w = w;
        MoveImage(&rect, *(s16 *)(D_80063454 + 0x202C), 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800270C4);


void func_8002719C(s32 *arg0) {
    arg0[0] = -0x100;
    arg0[5] = 0;
    func_800271D8(arg0, 1);
    arg0[0] = 0;
}

typedef struct Unk800271D8List {
    s32 pad0;
    s32 pad4;
    s32 pad8;
    s32 count;
    s32 *items[1];
} Unk800271D8List;

typedef struct Unk800271D8 {
    s32 pos;
    u8 pad4[0x10];
    s32 index;
    u8 pad18[0x38];
    Unk800271D8List *list;
} Unk800271D8;

s32 func_800272A4(Unk800271D8 *, u8 *);

s32 func_800271D8(void *object, s32 arg1) {
    Unk800271D8 *arg0 = object;
    s32 result;
    s32 count;
    u8 *item;
    s32 index;

    result = 0;
    count = arg0->list->count;
    while (arg1 > 0) {
        index = arg0->index;
        if (index >= count) {
            break;
        }
        item = (u8 *)arg0->list->items[index];
        arg0->index = index + 1;
        result |= func_800272A4(arg0, item);
        if ((*item >> 6) == 2) {
            arg1--;
            arg0->pos += 0x100;
        }
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800272A4);

void *memcpy(void *, const void *, u32);

u8 *func_80027420(u8 *src, u8 *dst, s32 mode, s32 *outLen, s32 noCopy, s32 arg5) {
    s32 len;

    switch (mode) {
    case 1:
        len = (src[0] << 8) | src[1];
        src += 2;
        if (noCopy != 0) {
            dst = src;
        } else {
            memcpy(dst, src, len);
        }
        break;
    case 2:
        len = func_80027C00(src, dst);
        break;
    case 3:
        len = func_80027CD0(src, dst, arg5);
        break;
    default:
        len = 0;
        break;
    }
    if (outLen != 0) {
        *outLen = len;
    }
    return dst;
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027500);

void func_80027600(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_t0;
    s32 var_v1;

    var_v1 = 0;
    var_t0 = 0;
    if (arg3 & 0x100) {
        var_v1 = *(s32 *)((char *)arg0 + 0x34);
    }
    if (arg3 & 0x200) {
        var_t0 = *(s32 *)((char *)arg0 + 0x38);
    }
    if ((var_t0 != 0) || (var_v1 != 0)) {
        func_800176DC(1, arg1, arg2, var_t0, var_v1, 1);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027664);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800277A8);


void func_800278F8(s32 arg0) {
    if (*(s32 *)(arg0 + 0xC) != 1) {
        func_800143C0((void *)(arg0 + 0x4C), 1);
        if (*(s32 *)(arg0 + 0x24) != 0) {
            func_80013A54(8, 0);
            func_80013A54(9, 0);
            func_80013A54(0xC, 0);
            func_80013A54(0xD, 0);
            D_80069358 = 0;
        }
        func_80027980(arg0);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027980);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027A38);

extern s32 D_80069334;
extern s32 D_80069338;
extern s32 D_8006933C;

s32 func_80027B9C(void) {
    D_80069334 = 0;
    if (D_8006933C != 0) {
        func_800116C0(D_8006933C);
        D_8006933C = 0;
    }
    if (D_80069338 != 0) {
        func_800116C0(D_80069338);
        D_80069338 = 0;
    }
    return -0xFF;
}

s32 func_80027C00(u8 *src, u8 *dst) {
    u8 *start;
    s8 c;
    s32 n;
    u8 b;
    s32 hi;

    start = dst;
    while (1) {
        c = *src++;
        if (c >= 0) {
            n = c + 1;
            if (c <= 0) {
                hi = *src++;
                n = (hi << 8) | *src++;
                if (n == 0) {
                    return dst - start;
                }
            }
            b = *src++;
            n--;
            while (n != -1) {
                *dst++ = b;
                n--;
            }
        } else {
            n = -c;
            if (c == -0x80) {
                hi = *src++;
                n = (hi << 8) | *src++;
            }
            n--;
            while (n != -1) {
                *dst++ = *src++;
                n--;
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027CD0);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027DEC);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027F3C);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028750);

extern s32 D_8005BB68[];

void func_8002890C(u8 *arg0, s32 arg1, s32 arg2) {
    s32 off;

    if (*(s32 *)(arg0 + 0xAC) & 0x04000000) {
        off = -((*(s32 *)(arg0 + 0x100) & 0x10) == 0) & 0x18;
        if (arg1 >= 0) {
            *(s32 *)(arg0 + 0x118) = off + D_8005BB68[arg1];
        } else {
            *(s32 *)(arg0 + 0x118) = off + arg2;
        }
    }
}

extern u8 D_8005BB78[];

void func_80028968(s32 arg0, s32 *arg1) {
    if (((u32) (arg0 - 0x30) < 0x10U) && (*arg1 = 0, ((arg0 < 0x40) != 0))) {
        if (arg0 >= 0x30) {
            *arg1 = 0x1000 - (D_8005BB78[arg0] * 0x14);
        }
    } else {
        *arg1 = 0;
    }
}

void func_800289C4(s16 *arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    func_80028968(arg1, &sp10);
    if (arg2 == 0) {
        arg0[0x11] = sp10;
    } else {
        arg0[0x11] = func_8001E5FC(arg0[0x11], sp10, 0x100);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028A28);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028C48);

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028FE0);

void func_800294B4(s16 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 var_a1;

    temp_a0 = *(s32 *)((u8 *)arg0 + 0xF0);
    if (temp_a0 != 0) {
        temp_v0 = func_800321D4(temp_a0, 0xF2, 0, 0, 0, 2, 0);
        if ((temp_v0 & 0xFFFF) != 0xFFFF) {
            var_a1 = (u32)(temp_v0 & 0x3F0) >> 4;
            if (var_a1 - 0x30 >= 0x10U) {
                var_a1 = -1;
            }
            func_800289C4(arg0, var_a1, 0);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80029538);

void func_8002973C(s32 arg0, s32 arg1) {
    func_80029538(arg0 + 0x60, arg0, arg1);
}

extern s32 D_8005F390;
extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_80029768(void) {
    s16 *p;

    p = (s16 *)func_8001148C(0xBE4);
    D_8005F390 = (s32)p;
    D_8005F3D0 = (s32)p;
    if (p != 0) {
        p[1] = 0;
        p = (s16 *)func_8001148C(0xBE4);
        D_8005F398 = (s32)p;
        if (p != 0) {
            p[1] = 0;
            return -0xFF;
        }
    }
    return -0xF;
}

extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_800297C8(void) {
    func_800116C0(D_8005F3D0);
    func_800116C0(D_8005F398);
    return -0xFF;
}

u16 *func_80029800(u16 *arg0, s32 arg1, u16 *arg2, s32 arg3) {
    if (!(arg0[1] & 1)) {
        func_8003B940(arg0 + 2, arg2 + 2, arg0[0]);
        *arg2 = arg0[0];
        return arg2;
    }
    if (arg3 == 0) {
        func_8003B24C(arg0, arg1, arg2);
        return arg2;
    }
    func_8003AE54(arg0, arg1, arg2);
    return arg2;
}
