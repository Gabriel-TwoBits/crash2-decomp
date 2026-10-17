#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060828;
extern s32 D_800608D8;
extern s32 D_800608E0;

#define ENTRY_TYPE_Y_RANGE 0x1E3
#define ENTRY_TYPE_X_RANGE 0x1E2
#define ENTRY_TYPE_Z_RANGE 0x1E4
#define FIXED_SHIFT 8
#define TRIG_SHIFT 4

typedef struct Unk80025D18Range {
    /* 0x0 */ s16 min;
    /* 0x2 */ s16 max;
    /* 0x4 */ s16 value;
} Unk80025D18Range;

typedef struct Unk80025D18Pos {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk80025D18Pos;

void func_80025D18(Unk80025D18Pos *arg0) {
    s32 sp28;
    Unk80025D18Range *yRange;
    Unk80025D18Range *xRange;
    Unk80025D18Range *zRange;
    s32 xMin;
    s32 xMax;
    s32 zMin;
    s32 zMax;
    s32 dy;
    s32 dx;
    s32 dz;
    s32 sin;
    s32 cos;
    s32 negAngle;
    s32 rotX;
    s32 rotZ;

    yRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_Y_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (yRange != 0) {
        xMin = yRange->min;
        xMax = yRange->max;
        if (xMin <= xMax) {
            dy = arg0->y - D_800607F8;
            if (dy < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < dy) {
                sp28 = yRange->value << FIXED_SHIFT;
                func_80025FDC(0, &sp28);
            }
        }
    }
    xRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_X_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (xRange != 0) {
        xMin = xRange->min;
        xMax = xRange->max;
        if (xMax < xMin) {
            xRange = 0;
        }
    }
    zRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_Z_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (zRange != 0) {
        zMin = zRange->min;
        zMax = zRange->max;
        if (zMax < zMin) {
            zRange = 0;
        }
    }
    if (xRange != 0 || zRange != 0) {
        dx = arg0->x - D_800607F4;
        dz = arg0->z - D_800607FC;
        negAngle = -D_80060828;
        sin = func_8003D360(negAngle);
        cos = func_8003D2EC(negAngle);
        rotX = (((dz >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) + (((dx >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        rotZ = (((dx >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) - (((dz >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        if (xRange != 0 && (rotX < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < rotX)) {
            sp28 = xRange->value << FIXED_SHIFT;
            func_80025FDC(0, &sp28);
        }
        if (zRange != 0 && (rotZ < (zMin << FIXED_SHIFT) || (zMax << FIXED_SHIFT) < rotZ)) {
            sp28 = zRange->value << FIXED_SHIFT;
            func_80025FDC(0, &sp28);
        }
    }
}
