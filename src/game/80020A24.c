#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F1FC[] = "DirnD";

#ifdef NON_MATCHING
void D_80042698(void);
void D_80042AB8(void);
void D_80042B44(void);
void D_80042B7C(void);
void D_80042C2C(void);
void D_80042E84(void);
void D_80042F08(void);
void D_800439D4(void);
void D_80043A84(void);
void D_80043B74(void);
void D_80043C2C(void);
void D_80043C48(void);
void D_80043D34(void);
void D_80043D84(void);
void D_80043EA0(void);
void D_800449B0(void);
void D_80044A04(void);
void D_80044E48(void);
void D_80044EC0(void);
void D_800459A8(void);
void D_80045AB4(void);
void D_80045D88(void);
void D_80045F40(void);
extern u8 D_8004299C[];
extern u8 D_80042C70[];
extern u8 D_8004399C[];
extern u8 D_80043B5C[];
extern u8 D_80043D54[];
extern u8 D_80044980[];
extern u8 D_80044A70[];
extern u8 D_8004599C[];
extern u8 D_80045C18[];
extern s32 D_8005F180;
extern s32 D_8005F38C;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_800608DC;
extern s32 D_800608E0;
extern void *D_80062A18;
extern void *D_80062A1C;
extern void *D_80062A20;
extern void *D_80062A24;
extern void *D_80062A28;
extern void (*D_80062A2C)(void);
extern void (*D_80062A30)(void);
extern void (*D_80062A34)(void);
extern void (*D_80062A38)(void);
extern s32 D_80062A3C;
extern s32 D_80062A40;
extern s32 D_80062A44;
extern s32 D_80062A48;
extern s32 D_80062A4C;
extern s32 D_80062A50;
extern s32 D_80062A54;
extern s32 D_80062A58;
extern s32 D_80062A5C;
extern s32 D_80062A60;
extern s32 D_80062A64;
extern u16 D_80062A6C;
extern u16 D_80062A6E;
extern s32 D_80062A70;
extern u16 D_80062A74[];
extern s32 D_80062A78;
extern s32 D_80062A7C;
extern s32 D_80062A80;
extern s32 D_80062A84;
extern s32 D_80062A88;
extern s32 D_80062A8C;
extern s32 D_80062A90;
extern s32 D_80062A94;
extern s32 D_80062A98;
extern s32 D_80062A9C;
extern s32 D_80062AA0;
extern s16 D_80062AA4;
extern s16 D_80062AA6;
extern s32 D_80062AA8;
extern s32 D_80062AAC;
extern s32 D_80062AB0;
extern s32 D_80062AB4;
extern s32 D_80062AB8;
extern s32 D_80062ABC;
extern s32 D_80062AC0;
extern s32 D_80062AC4;
extern s32 D_80062AC8;
extern s32 D_80062ACC;
extern s32 D_80062AD0;
extern s32 D_80062AD4;
extern s32 D_80062AD8;
extern s32 D_80062ADC;
extern s32 D_80062AE0;
extern s32 D_80062AE4;
extern s32 D_8006CB94;
extern s32 D_8006CC38;
extern s32 D_8006CC3C;
extern void *D_8006CC48;
extern u32 D_8006CC4C;
extern s32 D_8006CCC4;
extern s32 D_8006CCC8;
extern s32 D_8006CCCC;
extern s32 D_8006CCD0;
extern s32 D_8006CCE4;
extern s32 D_8006CD48;
extern s32 D_8006CD4C;
extern void *D_8006CD50;
extern s32 D_8006CD90;
extern s32 D_8006CD94;
extern s32 *D_8006CDAC;
extern u32 D_8006CDB0;
extern s32 D_8006CDC4;
extern u8 func_8003DD0C[];

/* Offsets from D_80062A18 (which holds &D_800607F4) to the other table pointers. */
#define TABLE_A_OFFSET (-0x21F4)
#define TABLE_B_OFFSET (-0x20F4)
#define TABLE_C_OFFSET (-0x22A4)
#define TABLE_ORIGIN_OFFSET (-0x2224)

/* Object position fields, 24.8 fixed point. */
#define OBJ_X 0x60
#define OBJ_Y 0x64
#define OBJ_Z 0x68
#define FIELD_S32(base, off) (*(s32 *)((u8 *)(base) + (off)))
#define FIELD_S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define FIELD_U16(base, off) (*(u16 *)((u8 *)(base) + (off)))

#define FRAME_WORDS 8
#define FIXED_SHIFT 8
#define FIXED_UNPACK(v) ((s32)(v) >> FIXED_SHIFT)
#define PACK_Y_MASK 0xFFFE
#define PACK_X_MASK 0xFFFF
#define PACK_Y_SHIFT 16
#define POS_NONE_XY 0x80008000
#define POS_NONE_Z (-0x8000)
#define NO_POS_ARG 0
#define SCALE_SHIFT 4
#define ALIGN_MASK_16 (~0xF)

/* Position relative to the scene origin, with X/Y packed in one word. */
#define PACK_XY(obj) \
    ((((FIXED_UNPACK(FIELD_S32(obj, OBJ_Y) - D_800607F8)) & PACK_Y_MASK) << PACK_Y_SHIFT) | \
     (FIXED_UNPACK(FIELD_S32(obj, OBJ_X) - D_800607F4) & PACK_X_MASK))
#define REL_Z(obj) FIXED_UNPACK(FIELD_S32(obj, OBJ_Z) - D_800607FC)

#define SCENE_FLAG_ENTRY 0x1
#define SCENE_FLAG_4 0x4
#define SCENE_FLAG_8 0x8
#define SCENE_FLAG_10 0x10
#define SCENE_FLAG_20 0x20
#define SCENE_FLAG_40 0x40
#define SCENE_FLAG_100 0x100
#define SCENE_FLAG_200 0x200
#define SCENE_FLAG_1000 0x1000
#define SCENE_FLAG_8000 0x8000
#define SCENE_FLAG_200000 0x200000
#define SCENE_FLAG_400000 0x400000
#define SCENE_FLAG_800000 0x800000
#define GLOBAL_FLAG_10000000 0x10000000
#define GLOBAL_FLAG_1000000 0x01000000

void func_80020A24(void) {
    s32 frame[FRAME_WORDS];
    s16 *p16;
    s32 *p;
    s32 *entry;
    s32 v;

    D_80062A18 = (u8 *)&D_80062A18 + TABLE_ORIGIN_OFFSET;
    D_80062A1C = (u8 *)&D_80062A18 + TABLE_A_OFFSET;
    D_80062A20 = (u8 *)&D_80062A18 + TABLE_B_OFFSET;
    D_80062A24 = (u8 *)&D_80062A18 + TABLE_C_OFFSET;
    D_80062ACC = 1;
    D_80062AE0 = 4;
    D_80062AE4 = 3;
    D_80062ADC = 3;
    D_80062A28 = func_8003DD0C;
    D_80062AA4 = (s16)(FIXED_UNPACK(FIELD_S32(D_8005F38C, OBJ_X) - D_800607F4));
    D_80062AA6 = (s16)(FIXED_UNPACK(FIELD_S32(D_8005F38C, OBJ_Y) - D_800607F8));
    D_80062AA8 = FIXED_UNPACK(FIELD_S32(D_8005F38C, OBJ_Z) - D_800607FC);
    p = (s32 *)func_80031AE8(D_800608DC, 0x185, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (p != 0) {
        D_80062AD8 = *p;
    } else {
        D_80062AD8 = 0;
    }
    D_8006CD4C = D_80062AD8;
    /* a0 still holds the scene pointer at this call in retail; the callee ignores it. */
    ((void (*)(s32))func_80011E24)(D_800608DC);
    entry = 0;
    if (D_80062AD8 & SCENE_FLAG_ENTRY) {
        entry = (s32 *)func_80031AE8(D_800608DC, 0x186, D_800608E0, 0, 0, 0, 0, 0, 0);
    }
    if (entry != 0) {
        D_80062ACC = 6;
        D_80062A3C = entry[0] << SCALE_SHIFT;
        D_80062A40 = entry[2];
        D_80062A44 = entry[1];
        D_80062AD0 = entry[3];
        D_80062AD4 = entry[4];
        v = entry[0];
    } else {
        v = 0x80000000;
        D_80062AD0 = 0;
        D_80062AD4 = 0;
    }
    D_8006CD48 = v;
    if (D_80062AD8 & SCENE_FLAG_1000) {
        p = (s32 *)func_80031AE8(D_800608DC, 0x297, D_800608E0, 0, 0, 0, 0, 0, 0);
        D_80062AD0 = p[1];
        D_80062AD4 = p[2];
        D_8006CCE4 = p[0];
    }
    if (D_80062AD8 & SCENE_FLAG_400000) {
        p16 = (s16 *)func_80031AE8(D_800608DC, 0x2AA, D_800608E0, 0, 0, 3, &frame[0], 0, 0);
        if (p16 != 0) {
            D_80062AA0 = p16[0];
            D_80062A94 = p16[1];
            D_80062A9C = p16[2];
            D_80062A98 = p16[3];
        }
    } else {
        D_80062AA0 = 0;
    }
    if (D_80062AD8 & SCENE_FLAG_10) {
        p16 = (s16 *)func_80031AE8(D_800608DC, 0x1B5, D_800608E0, 0, 0, 3, &frame[2], 0, 0);
        if (p16 != 0) {
            if (D_8005F180 == 0) {
                D_80062A6C = D_80062A74[0];
                D_80062A6E = D_80062A74[1];
                D_80062A70 = D_80062A78;
                if (D_8006CB94 & GLOBAL_FLAG_10000000) {
                    D_80062A74[0] = D_80062A74[0] - D_8006CCC4;
                    D_80062A74[1] = D_80062A74[1] - D_8006CCC8;
                    D_80062A78 -= D_8006CCCC;
                } else {
                    D_80062A74[0] = D_80062A74[0] - (u16)p16[0];
                    D_80062A74[1] = D_80062A74[1] - (u16)p16[1];
                    D_80062A78 -= p16[2];
                }
            }
            if (D_8006CB94 & GLOBAL_FLAG_10000000) {
                D_80062A90 = D_8006CCD0;
            } else {
                D_80062A90 = p16[3];
                D_8006CCD0 = p16[3];
            }
            D_80062A88 = p16[4];
            D_80062A8C = p16[5];
        }
        p = (s32 *)func_80031AE8(D_800608DC, 0x1B6, D_800608E0, 0, 0, 0, 0, 0, 0);
        if (p != 0) {
            D_80062A7C = p[0];
            D_80062A80 = p[1];
            D_80062A84 = p[2];
        }
    } else {
        D_80062A90 = 0;
    }
    if (D_80062AD8 & (SCENE_FLAG_4 | SCENE_FLAG_800000)) {
        func_80031AE8(D_800608DC, 0x1B7, D_800608E0, 0, 0, 0, 0, 0, 0);
        D_80062A28 = D_80044A70;
        D_80062A2C = func_80042420;
        D_80062A30 = D_80042698;
        D_80062A34 = D_80044E48;
        D_80062A38 = D_80044EC0;
        D_80062ACC = 0xA;
        D_80062ADC = 0x12;
        if (D_8006CD50 != 0) {
            D_80062A60 = PACK_XY(D_8006CD50);
            v = REL_Z(D_8006CD50);
        } else {
            D_80062A60 = POS_NONE_XY;
            v = POS_NONE_Z;
        }
        D_80062A64 = v;
        if (D_8006CC48 != 0) {
            D_80062A58 = PACK_XY(D_8006CC48);
            v = REL_Z(D_8006CC48);
        } else {
            D_80062A58 = POS_NONE_XY;
            v = POS_NONE_Z;
        }
        D_80062A5C = v;
    }
    if (D_80062AD8 & SCENE_FLAG_200) {
        D_80062A28 = D_80043B5C;
        D_80062A2C = D_80043B74;
        D_80062A30 = D_80043C48;
        D_80062A34 = D_80043C2C;
        D_80062A38 = D_80043D34;
    }
    if (D_80062AD8 & SCENE_FLAG_100) {
        D_80062A28 = D_80042C70;
        D_80062A2C = func_80042420;
        D_80062A30 = D_80042698;
        D_80062A34 = D_80042E84;
        D_80062A38 = D_80042F08;
        D_80062A50 = D_8006CC38 << SCALE_SHIFT;
        D_80062A54 = D_8006CC3C;
        D_80062AC0 = D_8006CD90 << SCALE_SHIFT;
        D_80062AC4 = D_8006CD94;
        if (D_8006CD50 != 0) {
            D_80062A60 = PACK_XY(D_8006CD50);
            v = REL_Z(D_8006CD50);
        } else {
            D_80062A60 = POS_NONE_XY;
            v = POS_NONE_Z;
        }
        D_80062A64 = v;
        if ((D_8006CC48 != 0) && ((D_8006CC4C >> FIXED_SHIFT) != 0)) {
            D_80062A58 = PACK_XY(D_8006CC48);
            v = REL_Z(D_8006CC48);
        } else {
            D_80062A58 = POS_NONE_XY;
            v = POS_NONE_Z;
        }
        D_80062A5C = v;
    }
    if (D_80062AD8 & SCENE_FLAG_8000) {
        D_80062A28 = D_80044980;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = D_800449B0;
        D_80062A38 = D_80044A04;
        D_80062AC0 = D_8006CD90 << SCALE_SHIFT;
        D_80062AC4 = D_8006CD94;
    }
    if (D_80062AD8 & SCENE_FLAG_8) {
        p = (s32 *)func_80031AE8(D_800608DC, 0x1B8, D_800608E0, 0, 0, 0, 0, 0, 0);
        if (p != 0) {
            D_80062A48 = p[0];
            D_80062A4C = p[1];
        }
        D_80062A28 = D_8004299C;
        D_80062A2C = D_80042AB8;
        D_80062A30 = D_80042B7C;
        D_80062A34 = D_80042B44;
        D_80062A38 = D_80042C2C;
        D_80062AC0 = D_8006CD90;
    }
    if (D_80062AD8 & SCENE_FLAG_20) {
        p = (s32 *)func_80031AE8(D_800608DC, 0x1DE, D_800608E0, 0, 0, 0, 0, 0, 0);
        if (p != 0) {
            D_80062AB4 = p[0];
            D_80062AB8 = p[1];
            D_80062ABC = p[2];
        }
        D_80062A28 = D_8004399C;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = D_800439D4;
        D_80062A38 = D_80043A84;
        D_80062AE4 = 8;
        D_80062AE0 = 0xC;
    }
    if (D_80062AD8 & SCENE_FLAG_200000) {
        if (D_8006CDC4 != 0) {
            D_80062A28 = D_8004599C;
            D_80062A2C = func_80042420;
            D_80062A30 = func_800426A0;
            D_80062A34 = D_800459A8;
            D_80062A38 = D_80045AB4;
            D_80062A54 = D_8006CDC4;
        } else {
            p = (s32 *)func_80031AE8(D_800608DC, 0x1DE, D_800608E0, 0, 0, 0, 0, 0, 0);
            if (p != 0) {
                D_80062AB4 = p[0];
                D_80062AB8 = p[1];
                D_80062ABC = p[2];
            }
            D_80062A28 = D_8004399C;
            D_80062A2C = func_80042420;
            D_80062A30 = func_800426A0;
            D_80062A34 = D_800439D4;
            D_80062A38 = D_80043A84;
            D_80062AE4 = 8;
            D_80062AE0 = 0xC;
        }
    }
    if (D_80062AD8 & SCENE_FLAG_40) {
        D_80062A28 = D_80043D54;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = D_80043D84;
        D_80062A38 = D_80043EA0;
    }
    if (D_8006CB94 & GLOBAL_FLAG_1000000) {
        frame[6] = func_80011F78(D_8005F1FC);
        D_80062A28 = D_80045C18;
        D_80062A2C = D_80045D88;
        D_80062A30 = D_80045F40;
        D_80062A34 = func_80042548;
        D_8006CDAC = &frame[6];
        D_80062A38 = func_80042818;
        D_80062AAC = func_8002FC78(&frame[6], D_8006CDB0 >> FIXED_SHIFT, 0);
        D_80062AB0 = func_8002FC78(D_8006CDAC, D_8006CDB0 >> FIXED_SHIFT, 1);
        D_80062AC8 = (func_8002FC78(D_8006CDAC, 8U, 0) + 4) & ALIGN_MASK_16;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80020A24);
#endif

#ifdef NON_MATCHING
#define RES_TYPE_A 0x208
#define RES_TYPE_B 0x209
#define SIDE_SELECT 2
#define OFFSET_DELTA 0x100
#define FLAGS_FIRST 0x14
#define FLAGS_SECOND 0xC

void func_800217F8(s32 arg0, s32 arg1, s32 arg2) {
    s32 count;
    s32 type;
    s32 offset;
    s32 p;

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_B;
        offset = arg1 - OFFSET_DELTA;
    } else {
        type = RES_TYPE_A;
        offset = arg1 + OFFSET_DELTA;
    }
    p = func_80031AE8(arg0, type, offset, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_FIRST, 1);
    }

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_A;
    } else {
        type = RES_TYPE_B;
    }
    p = func_80031AE8(arg0, type, arg1, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_SECOND, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800217F8);
#endif

extern s32 D_80067590;

void func_80021900(s32 count, u32 *p, s32 flags, s32 arg3) {
    u32 *cur;
    u32 v;
    u32 *e;
    s32 neg;

    cur = 0;
    do {
        v = *p;
        if (v & 1) {
            e = *(u32 **)(((v >> 13) & 0x3FC) + D_80067590);
            if (cur == 0 || cur < e) {
                cur = e;
            }
            if (cur[1] != v) {
                while (cur[1] != v) {
                    cur += 2;
                }
                cur -= 2;
                cur += 2;
            }
        } else {
            cur = (u32 *)v;
        }
        if (flags & 8) {
            func_80014260(cur, flags | 2, arg3);
        } else {
            neg = arg3;
            if (flags & 4) {
                neg = -neg;
            }
            func_8001434C((u32)cur, neg);
        }
        count--;
        p++;
    } while (count != 0);
}

#ifdef NON_MATCHING
#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ANGLE_FULL 0x1000
#define ANGLE_UNSET 0x1001
#define NEAR_DIST_ENTER 0x1404F
#define FAR_DIST_LEAVE 0x209E0
#define LEAVE_BLEND_RESET 0x400
#define SMOOTH_BLEND 0x80
#define MAX_DIST 0xEB1B8
#define DIST_OFFSET 0x416E0
#define DIR_NONE 0
#define DIR_NEG 1
#define DIR_POS 2
#define DEAD_ZONE 2
#define DEAD_ZONE_SPAN 5U
#define TURN_SCALE 3536
#define BOB_SCALE 0x25800

typedef struct { s32 a, b, c, d; } V;
s32 func_80026764(s32 arg0, V *arg1);
extern s8 D_8005B99A;
extern s32 D_8005B99C;
extern s32 D_8005B9A0;
extern s32 D_8005B9A4;
extern s32 D_8005B9A8;
extern s32 D_8005B9AC;
extern s32 D_8005B9EC;
extern s32 D_8005B9F0;
extern s32 D_8005B9F4;
extern s32 D_8005BA04;
extern s32 D_8005BA08;
extern s32 D_8005BB00;
extern s32 D_8005BB08;
extern s32 D_8005BB0C;
extern s32 D_8005BB1C;
extern s32 D_8005BB2C;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060800;
extern s32 D_80060804;
extern s32 D_80060818;
extern s32 D_80060820;
extern s32 D_800608CC[];
extern s32 D_800608D4;
extern s32 D_800608E0;
extern s32 D_800608F8;
extern s32 D_80060914;

void func_80021A08(s32 *arg0, s32 arg1) {
    s32 sp10[4];
    s32 sp20[4];
    s32 sp30[3];
    s32 sp40[3];
    s32 sp50[2];
    s32 dx;
    s32 dz;
    s32 ang;
    s32 diff;
    s32 dist;
    s32 d;
    s32 steps;
    s32 a0;
    s32 a1;
    s32 a2;

    if (arg1 != 0) {
        D_8005B9AC = ANGLE_UNSET;
    }
    func_800236DC(&D_8005B9F4, &D_8005BB0C, 0);
    func_800236DC(&D_8005BA08, &D_8005BB1C, 0);
    func_800236DC(&D_8005B99C, &D_8005BB2C, 0);

    sp30[0] = arg0[0];
    sp30[1] = arg0[1];
    dx = D_8005BB00 - arg0[0];
    dz = D_8005BB08 - arg0[2];
    sp30[2] = arg0[2];
    ang = ratan2(dx, dz) & ANGLE_MASK;
    dist = func_80041A78((dz >> 8) * (dz >> 8) + (dx >> 8) * (dx >> 8)) << 8;

    if (D_8005B99A != 0) {
        if (dist > FAR_DIST_LEAVE) {
            D_8005B99A = 0;
            D_8005B99C = LEAVE_BLEND_RESET;
            D_8005B9A0 = 0;
            D_8005B9A4 = -1;
        }
    } else if (dist <= NEAR_DIST_ENTER) {
        D_8005B99A = 1;
        D_8005B9A8 = ang;
    }

    if (D_8005B99A != 0) {
        ang = D_8005B9A8;
    } else if (D_8005B99C != 0) {
        ang = func_800312CC(ang, D_8005B9A8, D_8005B99C >> 2);
        D_8005B9A8 = ang;
    }
    if (D_8005BA08 != 0) {
        ang = (ang - ((D_8005BA04 * D_8005BA08) >> 10)) & ANGLE_MASK;
    }
    if (D_8005B9F4 != 0) {
        ang = func_800312CC(ang, D_8005B9EC, D_8005B9F4 >> 2);
    }
    D_80060914 = ang;
    if (D_8005B9AC != ANGLE_UNSET) {
        ang = func_800312CC(D_8005B9AC, ang, SMOOTH_BLEND);
    }
    D_8005B9AC = ang;

    diff = ang - (ratan2(D_80060818 - D_8005BB00, D_80060820 - D_8005BB08) & ANGLE_MASK);
    if (diff > ANGLE_HALF) {
        diff -= ANGLE_FULL;
    }
    if (diff < -ANGLE_HALF) {
        diff += ANGLE_FULL;
    }
    if ((u32)(diff + DEAD_ZONE) >= DEAD_ZONE_SPAN) {
        if (diff < 0) {
            D_800608F8 = DIR_NEG;
        } else {
            D_800608F8 = DIR_POS;
        }
    } else {
        D_800608F8 = DIR_NONE;
    }

    if (D_800608F8 != 0) {
        if (diff < 0) {
            diff = -diff;
        }
        sp10[0] = D_800608CC[0];
        sp10[1] = D_800608D4;
        sp10[2] = D_800608E0;
        sp10[3] = D_800608F8;
        steps = func_80024268(sp10, (diff * TURN_SCALE) >> 8, sp50);
        if (steps != 0 && func_80026764((s32)sp10, (V *)sp20) != 0) {
            D_800608F8 = sp20[3];
            func_80024268(sp20, steps, sp50);
            a0 = sp20[0];
            a1 = sp20[1];
            a2 = sp50[0];
        } else {
            a0 = D_800608CC[0];
            a1 = D_800608D4;
            a2 = sp50[0];
        }
    } else {
        a0 = D_800608CC[0];
        a1 = D_800608D4;
        a2 = D_800608E0;
    }
    func_800200A8(a0, a1, a2, 0);

    if (D_8005BA08 != 0) {
        D_800607F8 -= (D_8005BA08 * BOB_SCALE) >> 10;
    }
    func_800231B0(sp30);

    d = func_80041A78((arg0[0] >> 8) * (arg0[0] >> 8) + (arg0[2] >> 8) * (arg0[2] >> 8)) << 8;
    if (d > MAX_DIST) {
        d = MAX_DIST;
    }
    d = d - DIST_OFFSET;
    if (d < 0) {
        d = 0;
    }
    if (D_8005B9F4 != 0 && D_8005B9F0 >= 0) {
        d += ((D_8005B9F0 - d) * D_8005B9F4) >> 10;
    }
    func_8002FF54(sp40, -d, D_80060800, D_80060804);
    D_800607F4 += sp40[0];
    D_800607F8 += sp40[1];
    D_800607FC += sp40[2];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021A08);
#endif

#ifdef NON_MATCHING
extern s32 D_8005B9AC;
extern s32 D_8005BB3C;
extern s32 D_8005BB44;
extern s32 D_80060818;
extern s32 D_80060820;
extern s32 D_800608CC[];
extern s32 D_800608D4;
extern s32 D_800608E0;
extern s32 D_800608F8;
extern s32 D_80060914;

#define ANGLE_UNSET 0x1001
#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ANGLE_FULL 0x1000
#define ANGLE_LOW_LIMIT 0x200
#define ANGLE_HIGH_LIMIT 0xE00
#define ANGLE_SMOOTH_STEP 0x80
#define ANGLE_DEAD_ZONE 2
#define ANGLE_DEAD_SPAN 5
#define DIFF_SCALE 221
#define DIFF_SHIFT_PRE 16
#define DIFF_SHIFT_A 8
#define DIFF_SHIFT_B 4

void func_80021F4C(s32 *arg0, s32 arg1) {
    s32 a[4];
    s32 b[4];
    s32 c[3];
    s32 d;
    s32 s0;
    s32 v;
    s32 t;
    s32 a0, a1, a2;

    if (arg1 != 0) {
        D_8005B9AC = ANGLE_UNSET;
    }
    c[0] = arg0[0];
    c[1] = arg0[1];
    c[2] = arg0[2];
    s0 = (ratan2(D_8005BB3C - c[0], D_8005BB44 - c[2]) + ANGLE_HALF) & ANGLE_MASK;
    if (s0 > ANGLE_HALF) {
        if (s0 < ANGLE_HIGH_LIMIT) {
            s0 = ANGLE_HIGH_LIMIT;
        }
    } else if (s0 > ANGLE_LOW_LIMIT) {
        s0 = ANGLE_LOW_LIMIT;
    }
    D_80060914 = s0;
    if (D_8005B9AC != ANGLE_UNSET) {
        s0 = func_800312CC(D_8005B9AC, s0, ANGLE_SMOOTH_STEP);
    }
    D_8005B9AC = s0;
    v = s0 - (ratan2(D_80060818 - D_8005BB3C, D_80060820 - D_8005BB44) & ANGLE_MASK);
    if (v > ANGLE_HALF) {
        v -= ANGLE_FULL;
    }
    if (v < -ANGLE_HALF) {
        v += ANGLE_FULL;
    }
    if ((u32)(v + ANGLE_DEAD_ZONE) >= ANGLE_DEAD_SPAN) {
        if (v < 0) {
            D_800608F8 = 1;
        } else {
            D_800608F8 = 2;
        }
    } else {
        D_800608F8 = 0;
    }
    if (D_800608F8 != 0) {
        if (v < 0) {
            v = -v;
        }
        t = (s32)(v * DIFF_SCALE * DIFF_SHIFT_PRE) >> DIFF_SHIFT_A;
        a[0] = D_800608CC[0];
        a[1] = D_800608D4;
        a[2] = D_800608E0;
        a[3] = D_800608F8;
        t = func_80024268(a, t >> DIFF_SHIFT_B, &d);
        if (t != 0 && func_80026764((s32)a, (void *)b) != 0) {
            D_800608F8 = b[3];
            func_80024268(b, t, &d);
            a0 = b[0];
            a1 = b[1];
            a2 = d;
        } else {
            a0 = D_800608CC[0];
            a1 = D_800608D4;
            a2 = d;
        }
    } else {
        a0 = D_800608CC[0];
        a1 = D_800608D4;
        a2 = D_800608E0;
    }
    func_800200A8(a0, a1, a2, 0);
    func_800231B0(c);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021F4C);
#endif

extern s8 D_8005B99B;
extern s32 D_8005B9C4;
extern s32 D_800608F8;

void func_800221DC(void) {
    D_800608F8 = 0;
    if (D_8005B99B != 0) {
        D_8005B9C4 = 0;
        return;
    }
    func_80023154();
}

#ifdef NON_MATCHING
#define UNK_CALL_ARG 0x173
#define UNK_CALL_MODE 2
#define SLOT_STRIDE_WORDS 3
#define SUBTABLE_OFFSET 0x10
#define BASE_INDEX_OFFSET 0x184
#define ENTRY_DEFAULT_OFFSET 0x14
#define ENTRY_VARIANT_OFFSET 0x10
#define VARIANT_DEFAULT 1

extern s32 D_800608CC[];
extern s32 D_800608D4;
extern s32 D_800608D8[];
extern s32 D_800608DC;
extern s32 D_800608FC;

void func_80022224(s32 arg0) {
    s32 slot;
    s32 value;
    u8 *table;

    D_800608FC = arg0;
    if (D_800608D4 == 0) {
        return;
    }
    table = (u8 *)D_800608CC[0];
    if (table == 0) {
        return;
    }
    slot = func_800321D4(D_800608D4, UNK_CALL_ARG, 0, 0, 0, UNK_CALL_MODE, 0) * SLOT_STRIDE_WORDS;
    table = (u8 *)D_800608CC[0];
    value = *(s32 *)(table + (slot + *(s32 *)(*(u8 **)(table + SUBTABLE_OFFSET) + BASE_INDEX_OFFSET)) * 4 + ENTRY_DEFAULT_OFFSET);
    D_800608D8[0] = value;
    if (arg0 == VARIANT_DEFAULT) {
        D_800608DC = value;
    } else {
        D_800608DC = *(s32 *)(table + (slot + arg0 + *(s32 *)(*(u8 **)(table + SUBTABLE_OFFSET) + BASE_INDEX_OFFSET)) * 4 + ENTRY_VARIANT_OFFSET);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022224);
#endif

#ifdef NON_MATCHING
extern s32 D_800608CC[];
extern s32 *D_8006759C;

void func_80022314(s32 *arg0, s32 arg1) {
    s32 a[3];
    s32 b[8];
    s32 out[4];
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 r;
    s32 a3;
    s32 x, y, z;

    a[0] = arg0[0];
    a[1] = arg0[1];
    a[2] = arg0[2];
    if (D_8006759C[2] == 3) {
        a[0] = (a[0] * 5) >> 3;
    }
    b[0] = D_800608CC[0];
    b[1] = D_800608CC[2];
    b[2] = D_800608CC[5];
    r = func_8002433C(b, a, &sp58, &sp5C, &sp60);
    if (r > 0) {
        r = func_80024658(b, a, sp5C, 0, sp60, 1);
        func_80023E60(b, r, out, arg1 ? 0x100 : -1);
        x = out[0];
        y = out[1];
        z = out[2];
    } else if (r < 0) {
        s32 r2 = func_80024658(b, a, sp58, 1, sp60, 1);
        s32 a3b = -1;
        if (arg1 != 0) {
            a3b = 0x100;
        }
        func_80023E60(b, r2, out, a3b);
        x = out[0];
        y = out[1];
        z = out[2];
    } else {
        x = D_800608CC[0];
        y = D_800608CC[2];
        z = D_800608CC[5];
    }
    func_800200A8(x, y, z, 0);
    func_800224A0(a);
    func_800231B0(a);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022314);
#endif

extern s32 D_800607FC;
extern s32 D_800608D8[];
extern s32 D_800608E0;
extern s32 D_8006CD98;

void func_800224A0(s32 *arg0) {
    s32 sp28;
    s16 *p;
    s32 new_var;
    s32 lo;
    s32 hi;
    s32 scale;
    s32 d;

    if (func_80031AE8(D_800608D8[0], 0x252, D_800608E0, 0, 0, 0, 0, 0, 0) != 0) {
        p = (s16 *)func_80031AE8(D_800608D8[0], 0x254, D_800608E0, 0, 0, 3, &sp28, 0, 0);
        if (p != 0) {
            lo = p[0] << 8;
            hi = p[1];
            hi = hi << 8;
            new_var = hi;
            scale = p[2];
            if (D_8006CD98 != 0x100) {
                scale = (scale * D_8006CD98) >> 8;
            }
            d = D_800607FC - arg0[2];
            if (new_var < d) {
                d = new_var;
            }
            d -= lo;
            if (d < 0) {
                d = 0;
            }
            D_800607FC -= (d * scale) >> 8;
        }
    }
}

#ifdef NON_MATCHING
void func_800225C0(s32 *arg0, s32 arg1) {
    s32 sp18[4];
    s32 sp28[4];
    s32 pad[4];
    s32 sp48[4];
    s32 p[4];
    s32 r;

    sp18[0] = arg0[0];
    do {
        sp18[1] = arg0[1];
        sp18[2] = arg0[2];
        sp28[0] = D_800608CC[0];
        sp28[1] = D_800608CC[2];
        sp28[2] = D_800608CC[5];
        r = func_8002433C(sp28, sp18, &p[0], &p[1], &p[2]);
        if (r > 0) {
            r = func_80024658(sp28, sp18, p[1], 0, p[2], 1);
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else if (r < 0) {
            r = func_80024658(sp28, sp18, p[0], 1, p[2], 1) + 0;
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else {
            func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
        }
    } while (0);
    func_800231B0(sp18);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800225C0);
#endif

#ifdef NON_MATCHING
void func_8002271C(s32 *arg0, s32 arg1) {
    s32 v18[3];
    s32 v28[8];
    s32 v48[3];
    s32 v58, v5C, v60;
    s32 r;
    s32 t;
    s32 t2;

    v18[0] = arg0[0];
    v18[1] = arg0[1];
    t2 = arg0[2];
    v28[0] = D_800608CC[0];
    v28[1] = D_800608CC[2];
    v28[2] = D_800608CC[5];
    v18[2] = t2;
    r = func_8002433C(v28, v18, &v58, &v5C, &v60);
    if (r > 0) {
        t = func_80024658(v28, v18, v5C, 0, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else if (r < 0) {
        t = func_80024658(v28, v18, v58, 1, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else {
        func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
    }
    func_800231B0(v18);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_8002271C);
#endif

#ifdef NON_MATCHING
#define DIR_NONE 0
#define DIR_FIRST 1
#define DIR_SECOND 2
#define QUERY_SELECTOR 0x160
#define QUERY_SELECTOR_B 0x29
#define QUERY_MODE_A 3
#define QUERY_LIMIT 0x100
#define QUERY_MODE_B 2

s32 func_80026764(s32 arg0, V *arg1);
extern s32 D_800608E8;
extern s32 D_800608EC;

void func_80022878(s32 *arg0) {
    s32 sp20[4];
    s32 sp30[4];
    s32 sp40[3];
    s32 sp50;
    s32 temp_v0;
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;

    sp40[0] = arg0[0];
    sp40[1] = arg0[1];
    sp40[2] = arg0[2];
    if (D_800608F8 == DIR_NONE) {
        if (D_800608EC < (D_800608E8 * 2)) {
            D_800608F8 = DIR_FIRST;
        } else {
            D_800608F8 = DIR_SECOND;
        }
    }
    var_a1 = func_800321D4(D_800608DC, QUERY_SELECTOR, D_800608E0, 0, 0, QUERY_MODE_A, QUERY_LIMIT);
    if (var_a1 < 0) {
        var_a1 = -var_a1;
        if (D_800608F8 == DIR_FIRST) {
            D_800608F8 = DIR_SECOND;
        } else {
            D_800608F8 = DIR_FIRST;
        }
    }
    sp20[0] = D_800608CC[0];
    sp20[1] = D_800608CC[2];
    sp20[2] = D_800608E0;
    sp20[3] = D_800608F8;
    temp_v0 = func_80024268(sp20, var_a1, &sp50);
    if ((temp_v0 != 0) && (func_80026764((s32)sp20, (V *)sp30) != 0)) {
        sp50 = sp30[2];
        D_800608F8 = sp30[3];
        if (func_800321D4(sp30[1], QUERY_SELECTOR_B, 0, 0, 0, QUERY_MODE_B, 0) == QUERY_MODE_B) {
            func_80024268(sp30, temp_v0, &sp50);
        } else {
            func_80023084(sp30[1], sp30[2]);
        }
        var_a0 = sp30[0];
        var_v1 = sp30[1];
    } else {
        var_a0 = D_800608CC[0];
        var_v1 = D_800608CC[2];
    }
    func_800200A8(var_a0, var_v1, sp50, 0);
    func_800224A0(sp40);
    func_800231B0(sp40);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022878);
#endif

#ifdef NON_MATCHING
#define STEP_RANGE 0x100
#define FIXED_SHIFT 10
#define STATE_WORDS 4
#define SLOT_STRIDE_WORDS 3
#define OBJ_CHILD_OFFSET 0x10
#define CHILD_INDEX_OFFSET 0x184
#define STATE_BASE_BACKOFF 0xC
#define MODE_DEFAULT 2
#define MODE_SAME_TARGET 1
#define TRIGGER_VALUE 5

extern s8 D_8005B994;
extern s32 D_8005BA14;
extern s32 D_8005BA18;
extern s32 D_8005BA1C;
extern s32 D_8005BA20;
extern s32 D_8005BA24;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_8005BAE4;
extern s32 D_800607F4;
extern s32 D_800607F8;

/* Locals in retail stack order; the pads are unused slots. */
typedef struct {
    s32 cur[STATE_WORDS];
    s32 next[STATE_WORDS];
    s32 pad0[STATE_WORDS];
    s32 out[3];
    s32 pad1;
    s32 path[3];
    s32 pad2;
    s32 dist[3];
    s32 pad3;
    s32 pos[3];
} Unk80022A5CFrame;

void func_80022A5C(s32 *arg0) {
    Unk80022A5CFrame f;
    s32 *obj;
    s32 *target;
    s32 ret;
    s32 mode;
    s32 *q;
    s32 scale;
    s32 i;

    f.path[0] = arg0[0];
    f.path[1] = arg0[1];
    f.cur[0] = D_800608CC[0];
    f.cur[1] = D_800608D4;
    f.cur[2] = D_800608E0;
    f.cur[3] = D_8005BAE4;
    f.path[2] = arg0[2];
    obj = (s32 *)func_80014260((char *)&D_8005BAE4 - STATE_BASE_BACKOFF, 1, 0);
    f.next[0] = (s32)obj;
    q = *(s32 **)((char *)obj + OBJ_CHILD_OFFSET);
    target = *(s32 **)((char *)obj + OBJ_CHILD_OFFSET +
                       (D_8005BADC * SLOT_STRIDE_WORDS + *(s32 *)((char *)q + CHILD_INDEX_OFFSET)) * 4);
    f.next[1] = (s32)target;
    if (D_800608D4 == (s32)target) {
        f.next[2] = D_800608E0;
    } else {
        f.next[2] = D_8005BAE0;
    }
    if (D_8005BAE0 == 0) {
        f.next[3] = 2;
    } else {
        f.next[3] = 1;
    }
    ret = func_8002433C(f.next, f.path, &f.dist[0], &f.dist[1], &f.dist[2]);
    if (D_8005BA14 != 0) {
        if (D_800608D4 == (s32)target) {
            mode = MODE_SAME_TARGET;
        } else {
            mode = MODE_DEFAULT;
        }
        if (ret > 0) {
            mode = func_80024658(f.cur, f.path, f.dist[1], 0, f.dist[2], mode);
        } else if (ret < 0) {
            mode = func_80024658(f.cur, f.path, f.dist[0], 1, f.dist[2], mode);
        } else {
            for (i = 0; i < STATE_WORDS; i++) {
                f.next[i] = f.cur[i];
            }
        }
        func_80026A08((s32 *)f.next[1], f.next[2], f.pos, 0);
        if (ret != 0) {
            func_80023E60(f.cur, mode, f.out, STEP_RANGE / D_8005BA14);
            func_800200A8(f.out[0], f.out[1], f.out[2], 0);
        } else {
            func_800200A8(D_800608CC[0], D_800608D4, D_800608E0, 0);
        }
        scale = ((D_8005BA18 - (D_8005BA14 - 1)) << FIXED_SHIFT) / D_8005BA18;
        D_800607F4 = (((f.pos[0] - D_8005BA1C) * scale) >> FIXED_SHIFT) + D_8005BA1C;
        D_800607F8 = (((f.pos[1] - D_8005BA20) * scale) >> FIXED_SHIFT) + D_8005BA20;
        D_800607FC = (((f.pos[2] - D_8005BA24) * scale) >> FIXED_SHIFT) + D_8005BA24;
        func_800231B0(f.path);
        D_8005BA14 = D_8005BA14 - 1;
        if (D_8005BA14 == 0) {
            D_8005B994 = TRIGGER_VALUE;
            func_80023084(D_800608D4, D_8005BAE0);
        } else {
            return;
        }
    } else {
        mode = MODE_DEFAULT;
        if (ret > 0) {
            mode = func_80024658(f.cur, f.path, f.dist[1], 0, f.dist[2], mode);
        } else if (ret < 0) {
            mode = func_80024658(f.cur, f.path, f.dist[0], 1, f.dist[2], mode);
        } else {
            /* mode stays at the default */
        }
        func_80023E60(f.cur, mode, f.out, -1);
        func_800200A8(f.out[0], f.out[1], f.out[2], 0);
        func_800231B0(f.path);
        if (D_800608D4 == (s32)target) {
            D_8005B994 = TRIGGER_VALUE;
            func_80023084(D_800608D4, D_8005BAE0);
        } else {
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022A5C);
#endif

#ifdef NON_MATCHING
#define UNK_80014260_OFFSET 0x13D
#define UNK_80014260_FLAGS 1
#define UNK_FIELD_OFFSET 0x10
#define UNK_INDEX_OFFSET 0x184
#define UNK_TABLE_STRIDE 3
#define UNK_STATE_5 5
#define UNK_D_8005F38C_FIELD 0x18

typedef struct Unk80022E74Inner {
    char pad0[UNK_INDEX_OFFSET];
    s32 unk_184;
} Unk80022E74Inner;

typedef struct Unk80022E74 {
    char pad0[UNK_FIELD_OFFSET];
    Unk80022E74Inner *unk_10;
    s32 unk_14[1];
} Unk80022E74;

extern s8 D_8005B994;
extern s32 D_8005F38C;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_800608D4;

void func_80022E74(s32 *arg0) {
    s32 args[3];
    Unk80022E74 *p;

    args[0] = arg0[0];
    args[1] = arg0[1];
    args[2] = arg0[2];
    if (D_8005B99B != 0) {
        p = (Unk80022E74 *)func_80014260(&D_8005B99B + UNK_80014260_OFFSET, UNK_80014260_FLAGS, 0);
        func_800200A8((s32)p,
                      ((s32 *)p)[(UNK_FIELD_OFFSET / 4) + D_8005BADC * UNK_TABLE_STRIDE + p->unk_10->unk_184],
                      D_8005BAE0, 0);
        D_8005B994 = UNK_STATE_5;
        ((s32 *)D_8005F38C)[UNK_D_8005F38C_FIELD / 4] = D_800608CC[0];
        return;
    } else {
        func_800200A8(D_800608CC[0], D_800608D4, D_800608E0, 0);
        func_800231B0(args);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022E74);
#endif

#ifdef NON_MATCHING
#define STATUS_DONE 5
#define SEARCH_FOUND 1

typedef struct {
    s32 unk_0;
    s32 unk_4;
    s32 unk_8;
    s32 unk_C;
} Unk80022F58;

s32 func_800267C4(s32 *, Unk80022F58 *, s32, s32);

/* Word indices into D_800608CC. */
#define IDX_A 0
#define IDX_B 2
#define IDX_C 5
#define IDX_EXPECTED 9
#define IDX_D 11

#define ARG_41 0x29
#define ARG_75 0x4B
#define ARG_FLAG 2
#define SHIFT_8 8

void func_80022F58(s32 *arg0) {
    s32 out[3];
    Unk80022F58 in;

    do {
        in.unk_0 = D_800608CC[IDX_A];
        in.unk_4 = D_800608CC[IDX_B];
        in.unk_8 = D_800608CC[IDX_C];
        in.unk_C = D_800608CC[IDX_D];
        if (func_800267C4(out, &in, D_800608CC[IDX_C], D_800608CC[IDX_D]) == SEARCH_FOUND) {
            func_800200A8(out[0], out[1], out[2], 0);
            if (func_800321D4(out[1], ARG_41, 0, 0, 0, ARG_FLAG, 0) != D_800608CC[IDX_EXPECTED]) {
                D_8005B994 = STATUS_DONE;
            }
        } else {
            func_800200A8(D_800608CC[IDX_A], D_800608CC[IDX_B],
                          (func_800322C0(D_800608CC[IDX_B], ARG_75, 0, ARG_FLAG) - 1) << SHIFT_8, 0);
            D_8005B994 = STATUS_DONE;
        }
        func_800231B0(arg0);
    } while (D_8005B994 != STATUS_DONE);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022F58);
#endif

extern s32 D_8005F38C;

void func_80023084(s32 arg0, s32 arg1) {
    s32 *p;
    s32 n;
    s32 len;
    s32 kind;
    s32 step;

    p = (s32 *)func_80031AE8(arg0, 0x198, arg1, 0, 0, 1, 0, 0, 0);
    if (p != 0) {
        n = func_800322C0(arg0, 0x198, arg1, 1);
        if (n != 0) {
            do {
                kind = p[1];
                len = p[0];
                if (kind >= 0) {
                    func_8001CC10(0, D_8005F38C, kind, len - 2, p + 2);
                    step = len * 4;
                } else {
                    func_80012DC0();
                    step = len * 4;
                }
                n -= len;
                p = (s32 *)((char *)p + step);
            } while (n != 0);
        }
    }
}

typedef struct Unk80060918 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80060918;

typedef struct Unk8005B9C4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    Unk80060918 unkC;
} Unk8005B9C4;

extern s32 D_8003BEA4;
extern Unk80060918 D_80060918;

void func_80023154(void) {
    Unk8005B9C4 *state = (Unk8005B9C4 *)&D_8005B9C4;

    state->unk0 = 1;
    state->unk8 = 0x155;
    state->unk4 = D_8003BEA4;
    state->unkC = D_80060918;
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800231B0);

#ifdef NON_MATCHING
s32 func_800236DC(s32 *arg0, s32 *arg1, s32 arg2) {
    s32 cur;
    s32 goal;
    s32 diff;
    s32 step;
    s32 vel;
    s32 limit;
    s32 t;

    cur = arg0[0];
    if (arg2 == 0) {
        arg2 = arg0[2];
    }
    if (arg2 != 0) {
        goal = (~arg2 >> 31) & 0x400;
        diff = goal - cur;
        if (cur != goal) {
            step = diff;
            if (diff < 0) {
                step = -step;
            }
            vel = arg0[1];
            if (arg1[0] >= step) {
                step = step >> arg1[3];
                if (step == 0) {
                    step = 1;
                }
                if (diff < 0) {
                    t = -step;
                } else {
                    t = step;
                }
                diff = t;
            }
            limit = arg1[1];
            if (limit < step) {
                t = limit;
                if (diff < 0) {
                    t = -t;
                }
                diff = t;
            }
            if (vel < diff) {
                vel += arg1[2];
                if (diff < vel) {
                    vel = diff;
                }
            } else if (diff < vel) {
                vel -= arg1[2];
                if (vel < diff) {
                    vel = diff;
                }
            }
            cur += vel;
            if (cur < 0) {
                cur = 0;
            } else if (cur >= 0x401) {
                cur = 0x400;
            }
            arg0[1] = vel;
            arg0[0] = cur;
        } else {
            arg0[1] = 0;
        }
    }
    return cur;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800236DC);
#endif

typedef struct Unk800237F8 {
    s32 cur;
    s32 want;
    s32 count;
    s32 rest[3];
} Unk800237F8;

extern s32 D_8005B9BC[];
extern s32 D_8005B9C0;
extern Unk800237F8 D_8005BA28[];
extern char D_8005BB48[];
extern s32 D_80060914;
extern s32 D_80069948;
extern s32 D_8005F38C;

void func_800237F8(void) {
    s32 notFlag;
    s32 bits;
    s32 d;
    s32 i;
    Unk800237F8 *e;
    char *q;

    notFlag = (u32)*(s32 *)(D_8005F38C + 0xAC) >> 7;
    notFlag = notFlag ^ 1;
    notFlag = notFlag & 1;
    bits = 0;
    if (notFlag == 0) {
        d = (s32)((*(s32 *)(D_8005F38C + 0x70) - D_80060914) << 20) >> 20;
        if ((u32)(d - 0x71) < 0x71E) {
            bits = 2;
        } else if ((u32)(d + 0x78E) < 0x71E) {
            bits = 8;
        }
        if ((u32)(d + 0x38D) < 0x71B) {
            bits |= 4;
        } else if ((u32)(d + 0x471) >= 0x8E3) {
            bits |= 1;
        }
        if (D_8005B9BC[0] != bits) {
            D_8005B9BC[0] = bits;
            D_8005B9C0 = 0;
            bits = 0;
        } else {
            D_8005B9C0 = D_8005B9C0 + 1;
            if (D_8005B9C0 >= 16) {
                D_8005B9C0 = 15;
            } else {
                bits = 0;
            }
        }
        if (*(s32 *)(D_8005F38C + 0x100) & 0x04000000) {
            if (D_80069948 & 0x40) {
                bits |= 1;
            } else if (D_80069948 & 0x20) {
                bits |= 4;
            }
        } else {
            if (D_80069948 & 0x1000) {
                bits |= 1;
            } else if (D_80069948 & 0x4000) {
                bits |= 4;
            }
            if (D_80069948 & 0x2000) {
                bits |= 2;
            } else if (D_80069948 & 0x8000) {
                bits |= 8;
            }
        }
    }
    e = D_8005BA28;
    q = D_8005BB48;
    for (i = 0; i < 2; i++) {
        if (notFlag == 0) {
            if (i == 1) {
                if (bits & 1) {
                    if (e->want == i) {
                        e->want = 0;
                        e->count = 0;
                    }
                } else if (bits & 4) {
                    if (e->want == 0) {
                        e->want = i;
                        e->count = 0;
                    }
                }
            } else if (bits & 2) {
                if (e->want == 1) {
                    e->want = 0;
                    e->count = 0;
                }
            } else if (bits & 8) {
                if (e->want == 0) {
                    e->want = 1;
                    e->count = 0;
                }
            }
        }
        if (e->want != e->cur) {
            e->count = e->count + 1;
            if (e->count > 0) {
                e->cur = e->want;
                e->rest[1] = 0;
            }
        }
        func_800236DC(&e->rest[0], q, e->cur == 0 ? -1 : 1);
        e++;
        q += 0x10;
    }
}
