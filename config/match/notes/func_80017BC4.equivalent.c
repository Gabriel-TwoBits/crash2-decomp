#include "common.h"
#include "sdk.h"
#include "game.h"

#define MATRIX_BYTES 0x20
#define CAMERA_MATRIX_OFFSET 0xA0
#define CAMERA_COPY_OFFSET 0x80
#define PREV_MATRIX_OFFSET 0x40
#define ONE_Q12 0x1000
#define FIXED_SHIFT 8
#define FAR_COLOR_BASE 0x2B4
#define FLAG_OFFSET 0x29C
#define CAM_FLAG_FIXED 0x1000
#define ANGLE_STEP_MASK 0x7F
#define ANGLE_STEP_SHIFT 7
#define ANGLE_CENTER 0x40
#define ANGLE_SCALE 0x320
#define ANGLE_BASE 0xDC1E0
#define TILT_ANGLE 0x7D
#define GEOM_OFFSET_X 0x100
#define GEOM_OFFSET_Y 0x6C
#define STATE_TYPE_OFFSET 8
#define STATE_TYPE_FIXED 3
#define QUERY_RESULT_DONE 4
#define ANGLE_MASK 0xFFF
#define SINCOS_SHIFT 4
#define QUERY_ARG1 0x29
#define QUERY_ARG5 2
#define LIGHT_BASE_OFFSET 0x10

extern void MulMatrix(Unk80017AF8 *, s16 *);
extern void TransMatrix(Unk80017AF8 *, s32 *);
extern void func_80020A24(void);
extern s32 D_8005F180;
extern s32 D_80060794;
extern Unk80017AF8 D_800607B4;
extern s32 D_800607F4;
extern s32 D_80060828;
extern s32 *D_800608CC;
extern s32 D_800608D4;
extern s32 D_80060914;
extern s32 D_80060930;
extern s32 D_80060934;
extern s32 D_80060938;
extern s32 *D_8006759C;
extern s32 D_8006CB78;
extern s32 D_8006CBAC;
extern s32 D_8006CC04;
extern s32 D_8006CC08;
extern s32 D_8006CC0C;
extern s32 D_8006CC10;
extern s32 D_8006CC14;
extern s32 D_8006CC18;

void func_80017BC4(void) {
    s16 rot[9];
    s32 trans[3];
    char *base = (char *)&D_800607F4;
    Unk80017AF8 *view = (Unk80017AF8 *)(base - CAMERA_MATRIX_OFFSET);
    s32 *src;
    s32 *dst;
    u8 *light;
    s32 s, c, t, i, a;

    func_80020A24();
    s = func_8003D2EC(-D_80060808);
    c = func_8003D360(-D_80060808);
    view->a = c;
    view->b = -s;
    view->c = 0;
    view->d = s;
    view->e = c;
    view->f = 0;
    view->g = 0;
    view->h = 0;
    view->i = ONE_Q12;

    s = func_8003D2EC(-D_80060800);
    c = func_8003D360(-D_80060800);
    rot[0] = ONE_Q12;
    rot[1] = 0;
    rot[2] = 0;
    rot[3] = 0;
    rot[4] = c;
    rot[5] = -s;
    rot[6] = 0;
    rot[7] = s;
    rot[8] = c;
    MulMatrix(view, rot);

    s = func_8003D2EC(-D_80060804);
    c = func_8003D360(-D_80060804);
    rot[0] = c;
    rot[1] = 0;
    rot[2] = s;
    rot[3] = 0;
    rot[4] = ONE_Q12;
    rot[5] = 0;
    rot[6] = -s;
    rot[7] = 0;
    rot[8] = c;
    MulMatrix(view, rot);

    light = (u8 *)(*(s32 **)((char *)D_800608CC + LIGHT_BASE_OFFSET));
    SetFarColor(light[FAR_COLOR_BASE], light[FAR_COLOR_BASE + 1], light[FAR_COLOR_BASE + 2]);

    trans[0] = D_800607F4 >> FIXED_SHIFT;
    trans[1] = D_800607F8 >> FIXED_SHIFT;
    trans[2] = D_800607FC >> FIXED_SHIFT;
    TransMatrix(view, trans);
    func_80017AF8((Unk80017AF8 *)(base - CAMERA_COPY_OFFSET), view);

    src = (s32 *)(base - CAMERA_COPY_OFFSET);
    dst = &D_80060794;
    for (i = 0; i < MATRIX_BYTES / 4; i++) {
        dst[i] = src[i];
    }

    if (*(s32 *)(*(char **)((char *)D_800608CC + LIGHT_BASE_OFFSET) + FLAG_OFFSET) & CAM_FLAG_FIXED) {
        a = D_80060944;
        if (D_80060944 < 0) {
            a = D_80060944 + ANGLE_STEP_MASK;
        }
        t = ((D_80060944 - ((a >> ANGLE_STEP_SHIFT) << ANGLE_STEP_SHIFT)) - ANGLE_CENTER) * ANGLE_SCALE;
        if (t >= 0) {
            D_80060840 = t + ANGLE_BASE;
        } else {
            D_80060840 = ANGLE_BASE - t;
        }
        s = func_8003D2EC(TILT_ANGLE);
        c = func_8003D360(TILT_ANGLE);
        D_800607B4.a = ONE_Q12;
        D_800607B4.b = 0;
        D_800607B4.c = 0;
        D_800607B4.d = 0;
        D_800607B4.e = c;
        D_800607B4.f = -s;
        D_800607B4.g = 0;
        D_800607B4.h = s;
        D_800607B4.i = c;
        func_80017AF8(&D_800607B4 + 1, &D_800607B4);
    } else {
        src = (s32 *)(base - CAMERA_MATRIX_OFFSET);
        dst = (s32 *)(base - PREV_MATRIX_OFFSET);
        for (i = 0; i < (2 * MATRIX_BYTES) / 4; i++) {
            dst[i] = src[i];
        }
    }
    SetGeomOffset(D_8006CC20 + GEOM_OFFSET_X, D_8006CC24 + (D_8006CB78 >> FIXED_SHIFT) + GEOM_OFFSET_Y);
    if (D_8005F180 == 0) {
        t = D_8006CB78 >> FIXED_SHIFT;
        if (t == 0) {
            D_8006CB78 = 0;
        } else {
            if (t > 0) {
                a = -t + 1;
            } else {
                a = ~t;
            }
            D_8006CB78 = a << FIXED_SHIFT;
        }
    }
    D_8006CC04 = D_800607F4;
    D_8006CC08 = D_800607F8;
    D_8006CC0C = D_800607FC;
    D_8006CC10 = D_80060800;
    D_8006CC14 = D_80060804;
    D_8006CC18 = D_80060808;
    if (!(*(s32 *)(*(char **)((char *)D_800608CC + LIGHT_BASE_OFFSET) + FLAG_OFFSET) & CAM_FLAG_FIXED)) {
        D_8006083C = D_800607F4;
        D_80060840 = D_800607F8;
        *(s32 *)((char *)&D_8006083C + 8) = D_800607FC;
        if (func_800321D4(D_800608D4, QUERY_ARG1, 0, 0, 0, QUERY_ARG5, 0) != QUERY_RESULT_DONE) {
            D_80060914 = D_80060828 & ANGLE_MASK;
        } else {
            /* result 4: angle kept */
        }
        if (D_8006759C[STATE_TYPE_OFFSET / 4] == STATE_TYPE_FIXED) {
            D_80060914 = 0;
        } else {
            /* angle kept */
        }
    } else {
        D_80060914 = 0;
    }
    D_8006CBAC = D_80060914;
    D_80060930 = func_8003D2EC(D_80060914) >> SINCOS_SHIFT;
    D_80060934 = 0;
    D_80060938 = func_8003D360(D_80060914) >> SINCOS_SHIFT;
    func_80018294(*(s32 **)((char *)D_800608CC + LIGHT_BASE_OFFSET));
}
