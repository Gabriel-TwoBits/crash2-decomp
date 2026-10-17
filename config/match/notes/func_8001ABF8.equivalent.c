#include "common.h"
#include "sdk.h"
#include "game.h"

/* Field of type T at byte offset OFF in the structure P points to. */
#define FLD(T, P, OFF) (*(T *)((u8 *)(P) + (OFF)))

#define RENDER_WINDOW_BIAS 0xC8
#define RENDER_WINDOW_SIZE 0x1771
#define POOL_NEXT 0x2090
#define POOL_BASE 0x208C
#define POOL_LIST 0x2088

#define KIND_MODEL 1
#define KIND_SPRITE 2
#define KIND_ANIM 4
#define KIND_TABLE 5

#define NODE_SIZE_TABLE 0x30
#define NODE_SIZE_SPRITE 0x2C
#define NODE_SIZE_MODEL 0x64
#define TABLE_ENTRY_SIZE 0x18
#define SPRITE_ENTRY_STRIDE 0x10
#define POOL_ANIM_OFFSET 0x18

#define FLAG_NEG_ONE 0x200000
#define FLAG_ZERO 0x400
#define FLAG_TINT 0x100000
#define FLAG_ALT_CALLBACK 0x40000

#define DEFAULT_SPRITE_IDX 3
#define MODE_A 6
#define MODE_B 0xD
#define MODE_C 0xA
#define MODE_D 0xF
#define SHADE_RAMP 12
#define SHADE_SPLIT_A 8
#define SHADE_SPLIT_B 19
#define SHADE_SPLIT_C 6
#define VARIANT_FIRST_ROT 4
#define VARIANT_LAST_ROT 5
#define ROT_SHIFT 18
#define FIXED_SHIFT 12
#define LIGHT_DIST_SHIFT 8
#define LIGHT_AXIS 0xB50
#define RAMP_SHIFT 5
#define RAMP_FLOOR_OFFSET 0x3200
#define RAMP_CEIL_OFFSET 0xC8
#define SIGN_BIT 0x80000000
#define FLAG_RAMP_ENABLE 0x1000
#define FLAG_NO_RAMP 0x800

extern void D_80041E50(void);
extern s32 D_8005B6D0[];
extern s32 D_8005B6EC[];
extern s32 D_8005B708[];
extern s32 D_8005B70C[];
extern s32 D_8005B71C;
extern s32 D_8005B75C[];
extern s32 D_8005B760[];
extern s32 D_8005B770;
extern s32 D_8005F38C;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern u8 *D_800608CC;
extern s32 D_80060944;
extern s32 D_80062A3C;
extern s32 D_80062A40;
extern s32 D_80062A44;
extern s32 D_80062ACC;
extern s32 D_80062AD0;
extern s32 D_80062AD4;
extern s32 D_80062AD8;
extern s32 D_80062ADC;
extern u8 *D_80063458;
extern s32 D_8006CCE4;
extern void func_80041D14(void);

void func_8001ABF8(u8 *arg0, u8 *arg1) {
    u8 *pool;
    u8 *src;
    u8 *node;
    u8 *spr;
    u8 *sp40;
    u8 *sp48;
    u32 scale;
    u32 packed;
    s32 i;
    s32 sign;
    s32 flags;
    s32 idx;
    s32 sp30;
    s32 sp38;
    s32 ox;
    s32 oy;
    s32 oz;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 nx;
    s32 ny;
    s32 nz;
    s32 sq;
    s32 len;
    s32 kind;
    s32 mode;
    s32 tmp;
    s16 rx;
    s16 ry;
    s16 rz;
    u8 shade;
    u8 variant;

    pool = D_80063458;
    node = FLD(u8 *, pool, POOL_NEXT);
    src = FLD(u8 *, arg0, 0xE8);
    scale = FLD(u32, arg0, 0xEC) >> 8;
    if (((u32)((s32)node - FLD(s32, pool, POOL_BASE) + RENDER_WINDOW_BIAS) < RENDER_WINDOW_SIZE) || ((s32)arg0 == D_8005F38C)) {
        kind = FLD(u8, src, 0);
        switch (kind) {
        case KIND_SPRITE:
            tmp = FLD(s32, arg1, 4);
            spr = src + (scale * SPRITE_ENTRY_STRIDE + 8);
            func_80018474(spr + 4, src + 4);
            node = FLD(u8 *, pool, POOL_NEXT);
            FLD(u8 *, pool, POOL_NEXT) = node + NODE_SIZE_SPRITE;
            FLD(u8 *, node, 0) = FLD(u8 *, pool, POOL_LIST);
            FLD(u8 *, pool, POOL_LIST) = node;
            packed = FLD(u32, arg0, 0x120);
            sp48 = arg0 + 0x20;
            mode = packed & 0xFF;
            idx = (packed >> 8) & 0xFF;
            if (idx == 0) {
                if ((mode == MODE_A) || (mode == MODE_B)) {
                    idx = DEFAULT_SPRITE_IDX;
                } else {
                    idx = D_80062ADC;
                }
            } else {
                /* explicit index */
            }
            FLD(u8 *, node, 0x14) = FLD(u8 *, arg0, 8);
            FLD(s32, node, 4) = FLD(s32, arg1, 0);
            FLD(s32, node, 8) = D_8005B75C[idx];
            FLD(u8 *, node, 0x10) = arg0;
            FLD(u8 *, node, 0x1C) = spr;
            FLD(s32, node, 0xC) = D_8005B708[idx];
            if (FLD(s16, sp48, 6) != 0) {
                FLD(s32, node, 0x28) = (FLD(s16, sp48, 0x1A) << 0x14) | (FLD(s16, sp48, 0x1C) << 0xC) | (FLD(s16, sp48, 0x1E) * 0x10);
                FLD(s32, node, 0x20) = FLD(s16, sp48, 6);
            } else {
                FLD(s32, node, 0x20) = 0;
            }
            FLD(s32, node, 0x24) = tmp;
            return;
        case KIND_ANIM:
            flags = FLD(s32, arg0, 0xAC);
            sign = 1;
            if (flags & FLAG_NEG_ONE) {
                sign = -1;
            }
            if (flags & FLAG_ZERO) {
                sign = 0;
            }
            func_8001BD84(arg0, (char *)src, (s32)scale, (s32)(D_80063458 + POOL_ANIM_OFFSET), sign);
            return;
        case KIND_MODEL:
            sp40 = FLD(u8 *, arg0, 8);
            sp30 = FLD(s32, sp40, 0x20);
            sp38 = FLD(s32, sp40, 0x24);
            FLD(u8 *, pool, POOL_NEXT) = node + NODE_SIZE_MODEL;
            FLD(u8 *, node, 0) = FLD(u8 *, pool, POOL_LIST);
            FLD(u8 *, pool, POOL_LIST) = node;
            sp40 = FLD(u8 *, arg0, 8);
            shade = FLD(u8, arg0, 0x121);
            sp40 = FLD(u8 *, sp40, 0x28);
            FLD(u8 *, node, 0x14) = FLD(u8 *, arg0, 8);
            FLD(s32, node, 4) = FLD(s32, arg1, 0);
            FLD(u8 *, node, 0x10) = arg0;
            FLD(s32, node, 0x50) = FLD(s32, arg0, 0xA0);
            sp48 = arg0 + 0x20;
            if (FLD(s32, arg0, 0xAC) & FLAG_TINT) {
                FLD(s32, node, 0x18) = FLD(s32, arg0, 0xE4);
            } else {
                FLD(s32, node, 0x18) = D_80060944;
            }
            if (FLD(s32, arg0, 0xAC) & FLAG_ALT_CALLBACK) {
                FLD(s32, node, 0x1C) = (s32)D_80041E50;
            } else {
                FLD(s32, node, 0x1C) = (s32)func_80041D14;
            }
            if (shade == 0) {
                shade = (u8)D_80062ACC;
            } else {
                /* explicit shade */
            }
            variant = FLD(u8, arg0, 0x122);
            FLD(s32, node, 0x20) = D_8005B6D0[variant];
            FLD(s32, node, 0x24) = D_8005B6EC[variant];
            if (((s32)variant <= VARIANT_LAST_ROT) && ((s32)variant >= VARIANT_FIRST_ROT)) {
                rx = FLD(s16, arg0, 0x2E);
                ry = FLD(s16, arg0, 0x30);
                rz = FLD(s16, arg0, 0x32);
                shade = 1;
                if (variant == VARIANT_LAST_ROT) {
                    ox = rx;
                    oy = ry;
                    oz = rz;
                    rx = (s16)((ox * FLD(s32, arg0, 0x90) + oy * FLD(s32, arg0, 0x94) + oz * FLD(s32, arg0, 0x98)) >> ROT_SHIFT);
                    ry = (s16)((ox * FLD(s32, arg0, 0x9C) + oy * FLD(s32, arg0, 0xA0) + oz * FLD(s32, arg0, 0xA4)) >> ROT_SHIFT);
                    rz = (s16)((ox * FLD(s32, arg0, 0x84) + oy * FLD(s32, arg0, 0x88) + oz * FLD(s32, arg0, 0x8C)) >> ROT_SHIFT);
                } else {
                    /* unrotated */
                }
                FLD(s32, node, 0x50) = (ry << 0x10) | (rx & 0xFFFF);
                FLD(s32, node, 0x54) = rz;
                mode = FLD(u8, arg0, 0x120);
                if ((mode == MODE_C) || (mode == MODE_D)) {
                    lx = (D_800607F4 - FLD(s32, arg0, 0x60)) >> LIGHT_DIST_SHIFT;
                    ly = (D_800607F8 - FLD(s32, arg0, 0x64)) >> LIGHT_DIST_SHIFT;
                    lz = (D_800607FC - FLD(s32, arg0, 0x68)) >> LIGHT_DIST_SHIFT;
                    sq = lx * lx + ly * ly + lz * lz;
                    if (sq == 0) {
                        lx = 0;
                        ly = 0;
                        lz = 0;
                    } else {
                        len = func_80041A78(sq);
                        lx = (lx << FIXED_SHIFT) / len;
                        ly = (ly << FIXED_SHIFT) / len;
                        lz = (lz << FIXED_SHIFT) / len;
                    }
                } else {
                    lx = -LIGHT_AXIS;
                    ly = 0;
                    lz = LIGHT_AXIS;
                }
                if (variant == VARIANT_LAST_ROT) {
                    nx = (lx * FLD(s32, arg0, 0x90) + ly * FLD(s32, arg0, 0x94) + lz * FLD(s32, arg0, 0x98)) >> ROT_SHIFT;
                    ny = (lx * FLD(s32, arg0, 0x9C) + ly * FLD(s32, arg0, 0xA0) + lz * FLD(s32, arg0, 0xA4)) >> ROT_SHIFT;
                    nz = (lx * FLD(s32, arg0, 0x84) + ly * FLD(s32, arg0, 0x88) + lz * FLD(s32, arg0, 0x8C)) >> ROT_SHIFT;
                    lx = nx;
                    ly = ny;
                    lz = nz;
                } else {
                    /* unrotated */
                }
                lx = lx + rx;
                ly = ly + ry + rz;
                sq = lx * lx + ly * ly + lz * lz;
                if (sq == 0) {
                    lx = 0;
                    ly = 0;
                    lz = 0;
                } else {
                    len = func_80041A78(sq);
                    lx = (lx << FIXED_SHIFT) / len;
                    ly = (ly << FIXED_SHIFT) / len;
                    lz = (lz << FIXED_SHIFT) / len;
                }
                FLD(s32, node, 0x58) = (ly << 0x10) | (lx & 0xFFFF);
                FLD(s32, node, 0x5C) = lz;
            } else {
                /* no rotation */
            }
            FLD(s32, node, 0x48) = FLD(s32, arg0, 0x118) + D_80062AD0;
            FLD(s32, node, 0x4C) = FLD(s32, arg0, 0x118) + D_80062AD4;
            switch (shade) {
            case SHADE_RAMP:
                if ((s32)arg0 == D_8005F38C) {
                    if (!(D_80062AD8 & FLAG_RAMP_ENABLE) || (FLD(s32, FLD(u8 *, D_800608CC, 0x10), 0x29C) & FLAG_NO_RAMP) || (FLD(s32, arg0, 0x64) < D_8006CCE4 - RAMP_FLOOR_OFFSET) || (FLD(s32, arg0, 0x13C) < D_8006CCE4 - RAMP_FLOOR_OFFSET)) {
                        shade = 1;
                    } else {
                        tmp = (D_8006CCE4 - FLD(s32, arg0, 0x64)) << RAMP_SHIFT;
                        FLD(s32, node, 0x50) = tmp;
                        FLD(s32, node, 0x50) = tmp / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                    }
                } else {
                    tmp = (FLD(s32, arg0, 0xFC) - FLD(s32, arg0, 0x64)) << RAMP_SHIFT;
                    FLD(s32, node, 0x50) = tmp;
                    FLD(s32, node, 0x50) = tmp / FLD(s32, FLD(u8 *, FLD(u8 *, FLD(u8 *, arg0, 8), 0x28), 0x10), 4);
                    FLD(s32, node, 0x4C) = FLD(s32, arg0, 0x118) - RAMP_CEIL_OFFSET;
                }
                break;
            case SHADE_SPLIT_A:
            case SHADE_SPLIT_B:
            case SHADE_SPLIT_C:
                if (FLD(s32, arg0, 0xAC) < 0) {
                    FLD(s32, node, 0x54) = FLD(s32, arg0, 0x9C) * 0x10;
                    FLD(s32, node, 0x58) = FLD(s32, arg0, 0xA0);
                    FLD(s32, node, 0x5C) = FLD(s32, arg0, 0xA4);
                } else {
                    FLD(s32, node, 0x54) = D_80062A3C;
                    FLD(s32, node, 0x58) = D_80062A40;
                    FLD(s32, node, 0x5C) = D_80062A44;
                }
                break;
            default:
                break;
            }
            if (sp30 == sp38) {
                FLD(s32, node, 8) = D_8005B75C[shade];
                tmp = D_8005B708[shade];
                FLD(s32, node, 0x28) = sp38;
            } else {
                FLD(s32, node, 8) = D_8005B760[shade];
                tmp = D_8005B70C[shade];
                FLD(s32, node, 0x28) = sp30;
                FLD(s32, node, 0x2C) = sp38;
            }
            FLD(s32, node, 0xC) = tmp;
            FLD(s32, node, 0x30) = FLD(s32, sp40, 0x14);
            FLD(s32, node, 0x34) = FLD(s32, sp40, 0x18);
            FLD(s32, node, 0x38) = FLD(s32, sp40, 0x1C);
            FLD(s32, node, 0x3C) = FLD(s32, sp40, 0x20);
            if (FLD(s32, sp40, 0xC) < 6) {
                FLD(s32, node, 0x60) = 0;
            } else {
                FLD(s32, node, 0x60) = FLD(s32, sp40, 0x24);
            }
            if (FLD(s16, sp48, 6) != 0) {
                FLD(s32, node, 0x44) = (FLD(s16, sp48, 0x1A) << 0x14) | (FLD(s16, sp48, 0x1C) << 0xC) | (FLD(s16, sp48, 0x1E) * 0x10);
                FLD(s32, node, 0x40) = FLD(s16, sp48, 6);
            } else {
                FLD(s32, node, 0x40) = 0;
            }
            tmp = FLD(s32, node, 0x40);
            if (FLD(s32, arg0, 0x78) < 0) {
                tmp |= SIGN_BIT;
            } else {
                /* sign bit clear */
            }
            FLD(s32, node, 0x40) = tmp;
            return;
        default:
            if (kind != KIND_TABLE) {
                return;
            } else {
                /* table entry handled below */
            }
            tmp = FLD(s32, src, 8);
            idx = FLD(s32, arg1, 4);
            spr = src + ((scale * tmp * TABLE_ENTRY_SIZE) + 0xC);
            if (tmp > 0) {
                i = 0;
                do {
                    func_80018474(spr + 4, src + 4);
                    i += 1;
                    spr += TABLE_ENTRY_SIZE;
                } while (i < FLD(s32, src, 8));
            } else {
                /* empty table */
            }
            node = FLD(u8 *, pool, POOL_NEXT);
            FLD(u8 *, pool, POOL_NEXT) = node + NODE_SIZE_TABLE;
            FLD(u8 *, node, 0) = FLD(u8 *, pool, POOL_LIST);
            FLD(u8 *, pool, POOL_LIST) = node;
            FLD(u8 *, node, 0x14) = FLD(u8 *, arg0, 8);
            FLD(s32, node, 4) = FLD(s32, arg1, 0);
            FLD(s32, node, 8) = D_8005B770;
            FLD(u8 *, node, 0x10) = arg0;
            FLD(u8 *, node, 0x1C) = src;
            FLD(s32, node, 0x20) = 0;
            FLD(s32, node, 0x24) = idx;
            FLD(u32, node, 0x2C) = scale;
            FLD(s32, node, 0xC) = D_8005B71C;
            return;
        }
    } else {
        /* outside the render window */
    }
}
