#include "common.h"
#include "sdk.h"
#include "game.h"

/* Field of type T at byte offset OFF of the object P points to. */
#define FLD(T, P, OFF) (*(T *)((u8 *)(P) + (OFF)))

#define PACKET_LIST_HEAD 0x2088
#define PACKET_LIST_BASE 0x208C
#define PACKET_LIST_FREE 0x2090
#define PACKET_LIST_SLACK 0xC8
#define PACKET_LIST_LIMIT 0x1771

#define KIND_ONE 1
#define KIND_TWO 2
#define KIND_FOUR 4
#define KIND_FIVE 5

#define ROT_SHIFT 18
#define DIR_SHIFT 12
#define DIR_Y_DEFAULT 0xB50
#define FLAG_NEG_SIGN 0x80000000

#define MODE_SLOPE 12
#define MODE_A 8
#define MODE_B 19
#define MODE_C 6

extern void *D_80063458;
extern void *D_800608CC;
extern s32 D_8006CCE4;
extern s32 D_80060944;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80062A3C;
extern s32 D_80062A40;
extern s32 D_80062A44;
extern s32 D_80062ACC;
extern s32 D_80062AD0;
extern s32 D_80062AD4;
extern s32 D_80062AD8;
extern s32 D_80062ADC;
extern s32 D_8005B6D0[];
extern s32 D_8005B6EC[];
extern s32 D_8005B708[];
extern s32 D_8005B70C[];
extern s32 D_8005B71C;
extern s32 D_8005B75C[];
extern s32 D_8005B760[];
extern s32 D_8005B770;
extern char D_80041E50[];
extern void func_80041D14(void);

void func_8001ABF8(u8 *arg0, void *arg1) {
    u8 *list = D_80063458;
    u8 *prim = FLD(u8 *, list, PACKET_LIST_FREE);
    u8 *data = FLD(u8 *, arg0, 0xE8);
    u32 frame = FLD(u32, arg0, 0xEC) >> 8;
    u8 kind;
    s32 i;
    s32 n;

    if (!((u32)((prim - FLD(u8 *, list, PACKET_LIST_BASE)) + PACKET_LIST_SLACK) < PACKET_LIST_LIMIT) &&
        (s32)arg0 != D_8005F38C) {
        return;
    }

    kind = FLD(u8, data, 0);
    if (kind == KIND_TWO) {
        s32 arg1_4 = FLD(s32, arg1, 4);
        u32 flags;
        s32 idx;
        s32 sel;
        u8 *hdr;

        func_80018474(data + (frame * 0x10 + 8) + 4, data + 4);
        prim = FLD(u8 *, D_80063458, PACKET_LIST_FREE);
        FLD(u8 *, D_80063458, PACKET_LIST_FREE) = prim + 0x2C;
        FLD(s32, prim, 0) = FLD(s32, D_80063458, PACKET_LIST_HEAD);
        FLD(u8 *, D_80063458, PACKET_LIST_HEAD) = prim;
        flags = FLD(u32, arg0, 0x120);
        hdr = arg0 + 0x20;
        idx = (flags >> 8) & 0xFF;
        sel = flags & 0xFF;
        if (idx == 0) {
            if (sel == 6 || sel == 0xD) {
                idx = 3;
            } else {
                idx = D_80062ADC;
            }
        } else {
            /* explicit table index */
        }
        FLD(s32, prim, 0x14) = FLD(s32, arg0, 8);
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        FLD(s32, prim, 8) = D_8005B75C[idx];
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(u8 *, prim, 0x1C) = data + (frame * 0x10 + 8);
        FLD(s32, prim, 0xC) = D_8005B708[idx];
        if (FLD(s16, hdr, 6) != 0) {
            FLD(s32, prim, 0x28) = (FLD(s16, hdr, 0x1A) << 0x14) | (FLD(s16, hdr, 0x1C) << 0xC) |
                                   (FLD(s16, hdr, 0x1E) * 0x10);
            FLD(s32, prim, 0x20) = FLD(s16, hdr, 6);
        } else {
            FLD(s32, prim, 0x20) = 0;
        }
        FLD(s32, prim, 0x24) = arg1_4;
        return;
    } else if (kind == KIND_FOUR) {
        s32 dir = 1;
        s32 f = FLD(s32, arg0, 0xAC);
        if (f & 0x200000) {
            dir = -1;
        } else {
            /* keep forward */
        }
        if (f & 0x400) {
            dir = 0;
        } else {
            /* keep */
        }
        func_8001BD84(arg0, (char *)data, frame, (s32)((u8 *)D_80063458 + 0x18), dir);
        return;
    } else if (kind == KIND_FIVE) {
        u8 *src = data + (frame * FLD(s32, data, 8) * 0x18 + 0xC);
        s32 arg1_4 = FLD(s32, arg1, 4);

        n = FLD(s32, data, 8);
        i = 0;
        if (n > 0) {
            do {
                func_80018474(src + 4, data + 4);
                i += 1;
                src += 0x18;
            } while (i < FLD(s32, data, 8));
        } else {
            /* no entries to copy */
        }
        prim = FLD(u8 *, D_80063458, PACKET_LIST_FREE);
        FLD(u8 *, D_80063458, PACKET_LIST_FREE) = prim + 0x30;
        FLD(s32, prim, 0) = FLD(s32, D_80063458, PACKET_LIST_HEAD);
        FLD(u8 *, D_80063458, PACKET_LIST_HEAD) = prim;
        FLD(s32, prim, 0x14) = FLD(s32, arg0, 8);
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        FLD(s32, prim, 8) = D_8005B770;
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(u8 *, prim, 0x1C) = data;
        FLD(s32, prim, 0x20) = 0;
        FLD(s32, prim, 0x24) = arg1_4;
        FLD(u32, prim, 0x2C) = frame;
        FLD(s32, prim, 0xC) = D_8005B71C;
        return;
    } else if (kind == KIND_ONE) {
        u8 *obj = FLD(u8 *, arg0, 8);
        s32 lo = FLD(s32, obj, 0x20);
        s32 hi = FLD(s32, obj, 0x24);
        u8 *model;
        u8 *hdr;
        s32 mode;
        u8 sub;
        s32 flags;
        s32 v;

        FLD(u8 *, list, PACKET_LIST_FREE) = prim + 0x64;
        FLD(s32, prim, 0) = FLD(s32, D_80063458, PACKET_LIST_HEAD);
        FLD(u8 *, D_80063458, PACKET_LIST_HEAD) = prim;
        obj = FLD(u8 *, arg0, 8);
        mode = FLD(u8, arg0, 0x121);
        model = FLD(u8 *, obj, 0x28);
        FLD(u8 *, prim, 0x14) = obj;
        FLD(s32, prim, 4) = FLD(s32, arg1, 0);
        v = FLD(s32, arg0, 0xA0);
        FLD(u8 *, prim, 0x10) = arg0;
        FLD(s32, prim, 0x50) = v;
        hdr = arg0 + 0x20;
        flags = FLD(s32, arg0, 0xAC);
        if (flags & 0x100000) {
            FLD(s32, prim, 0x18) = FLD(s32, arg0, 0xE4);
        } else {
            FLD(s32, prim, 0x18) = D_80060944;
        }
        if (FLD(s32, arg0, 0xAC) & 0x40000) {
            FLD(char *, prim, 0x1C) = D_80041E50;
        } else {
            FLD(void *, prim, 0x1C) = (void *)func_80041D14;
        }
        if (mode == 0) {
            mode = D_80062ACC;
        } else {
            /* explicit mode */
        }
        sub = FLD(u8, arg0, 0x122);
        FLD(s32, prim, 0x20) = D_8005B6D0[sub];
        FLD(s32, prim, 0x24) = D_8005B6EC[sub];
        if ((s32)sub < 6 && (s32)sub >= 4) {
            s16 px = FLD(s16, arg0, 0x2E);
            s16 py = FLD(s16, arg0, 0x30);
            s16 pz = FLD(s16, arg0, 0x32);
            s32 dx;
            s32 dy;
            s32 dz;
            s32 sx;
            s32 sy;
            s32 len;
            u8 which;

            mode = 1;
            if (sub == 5) {
                s32 a = px * FLD(s32, arg0, 0x9C);
                s32 b = px * FLD(s32, arg0, 0x84);
                s32 c = py * FLD(s32, arg0, 0x88);
                px = (s16)((s32)((px * FLD(s32, arg0, 0x90)) + (py * FLD(s32, arg0, 0x94)) +
                                 (pz * FLD(s32, arg0, 0x98))) >> ROT_SHIFT);
                py = (s16)((s32)(a + (py * FLD(s32, arg0, 0xA0)) + (pz * FLD(s32, arg0, 0xA4))) >> ROT_SHIFT);
                pz = (s16)((s32)(b + c + (pz * FLD(s32, arg0, 0x8C))) >> ROT_SHIFT);
            } else {
                /* sub 4: vector used as is */
            }
            FLD(s32, prim, 0x50) = (py << 0x10) | (px & 0xFFFF);
            FLD(s32, prim, 0x54) = pz;
            which = FLD(u8, arg0, 0x120);
            if (which == 0xA || which == 0xF) {
                s32 ex = (D_800607F4 - FLD(s32, arg0, 0x60)) >> 8;
                s32 ey = (D_800607F8 - FLD(s32, arg0, 0x64)) >> 8;
                s32 ez = (D_800607FC - FLD(s32, arg0, 0x68)) >> 8;
                s32 sq = (ex * ex) + (ey * ey) + (ez * ez);
                if (sq == 0) {
                    dx = 0;
                    dy = 0;
                    dz = 0;
                } else {
                    len = func_80041A78(sq);
                    dx = (ex << DIR_SHIFT) / len;
                    dy = (ey << DIR_SHIFT) / len;
                    dz = (ez << DIR_SHIFT) / len;
                }
            } else {
                dx = -DIR_Y_DEFAULT;
                dy = 0;
                dz = DIR_Y_DEFAULT;
            }
            if (sub == 5) {
                s32 a = dx * FLD(s32, arg0, 0x9C);
                s32 b = dx * FLD(s32, arg0, 0x84);
                s32 c = dy * FLD(s32, arg0, 0x88);
                dx = (s32)((dx * FLD(s32, arg0, 0x90)) + (dy * FLD(s32, arg0, 0x94)) +
                           (dz * FLD(s32, arg0, 0x98))) >> ROT_SHIFT;
                dy = (s32)(a + (dy * FLD(s32, arg0, 0xA0)) + (dz * FLD(s32, arg0, 0xA4))) >> ROT_SHIFT;
                dz = (s32)(b + c + (dz * FLD(s32, arg0, 0x8C))) >> ROT_SHIFT;
            } else {
                /* sub 4: vector used as is */
            }
            sx = dx + px;
            sy = dy + py + pz;
            v = (sx * sx) + (sy * sy) + (dz * dz);
            if (v == 0) {
                dx = 0;
                dy = 0;
                dz = 0;
            } else {
                len = func_80041A78(v);
                dx = (sx << DIR_SHIFT) / len;
                dy = (sy << DIR_SHIFT) / len;
                dz = (dz << DIR_SHIFT) / len;
            }
            FLD(s32, prim, 0x58) = (dy << 0x10) | (dx & 0xFFFF);
            FLD(s32, prim, 0x5C) = dz;
        } else {
            /* not a vector sub-kind */
        }
        FLD(s32, prim, 0x48) = FLD(s32, arg0, 0x118) + D_80062AD0;
        FLD(s32, prim, 0x4C) = FLD(s32, arg0, 0x118) + D_80062AD4;
        switch (mode) {
        case MODE_SLOPE:
            if ((s32)arg0 == D_8005F38C) {
                s32 floor;
                s32 y;
                if (!(D_80062AD8 & 0x1000) || (FLD(s32, FLD(u8 *, D_800608CC, 0x10), 0x29C) & 0x800) ||
                    (y = FLD(s32, arg0, 0x64), floor = D_8006CCE4 - 0x3200, (y < floor)) ||
                    (FLD(s32, arg0, 0x13C) < floor)) {
                    mode = 1;
                } else {
                    s32 t = (D_8006CCE4 - y) << 5;
                    FLD(s32, prim, 0x50) = t;
                    FLD(s32, prim, 0x50) = t / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                }
            } else {
                s32 t = (FLD(s32, arg0, 0xFC) - FLD(s32, arg0, 0x64)) << 5;
                FLD(s32, prim, 0x50) = t;
                FLD(s32, prim, 0x50) = t / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                FLD(s32, prim, 0x4C) = FLD(s32, arg0, 0x118) - 0xC8;
            }
            break;
        case MODE_A:
        case MODE_B:
        case MODE_C:
            if (FLD(s32, arg0, 0xAC) < 0) {
                FLD(s32, prim, 0x54) = FLD(s32, arg0, 0x9C) * 0x10;
                FLD(s32, prim, 0x58) = FLD(s32, arg0, 0xA0);
                FLD(s32, prim, 0x5C) = FLD(s32, arg0, 0xA4);
            } else {
                FLD(s32, prim, 0x54) = D_80062A3C;
                FLD(s32, prim, 0x58) = D_80062A40;
                FLD(s32, prim, 0x5C) = D_80062A44;
            }
            break;
        default:
            break;
        }
        if (lo == hi) {
            FLD(s32, prim, 8) = D_8005B75C[mode];
            v = D_8005B708[mode];
            FLD(s32, prim, 0x28) = hi;
        } else {
            FLD(s32, prim, 8) = D_8005B760[mode];
            v = D_8005B70C[mode];
            FLD(s32, prim, 0x28) = lo;
            FLD(s32, prim, 0x2C) = hi;
        }
        FLD(s32, prim, 0xC) = v;
        FLD(s32, prim, 0x30) = FLD(s32, model, 0x14);
        FLD(s32, prim, 0x34) = FLD(s32, model, 0x18);
        FLD(s32, prim, 0x38) = FLD(s32, model, 0x1C);
        FLD(s32, prim, 0x3C) = FLD(s32, model, 0x20);
        if (FLD(s32, model, 0xC) < 6) {
            FLD(s32, prim, 0x60) = 0;
        } else {
            FLD(s32, prim, 0x60) = FLD(s32, model, 0x24);
        }
        if (FLD(s16, hdr, 6) != 0) {
            FLD(s32, prim, 0x44) = (FLD(s16, hdr, 0x1A) << 0x14) | (FLD(s16, hdr, 0x1C) << 0xC) |
                                   (FLD(s16, hdr, 0x1E) * 0x10);
            FLD(s32, prim, 0x40) = FLD(s16, hdr, 6);
        } else {
            FLD(s32, prim, 0x40) = 0;
        }
        v = FLD(s32, prim, 0x40);
        if (FLD(s32, arg0, 0x78) < 0) {
            v |= FLAG_NEG_SIGN;
        } else {
            /* sign flag stays clear */
        }
        FLD(s32, prim, 0x40) = v;
        return;
    } else {
        return;
    }
}
