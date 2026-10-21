#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80011E24(void);
void func_80042698(void);
void func_80042AB8(void);
void func_80042B44(void);
void func_80042B7C(void);
void func_80042C2C(void);
void func_80042E84(void);
void func_80042F08(void);
void func_800439D4(void);
void func_80043A84(void);
void func_80043B74(void);
void func_80043C2C(void);
void func_80043C48(void);
void func_80043D34(void);
void func_80043D84(void);
void func_80043EA0(void);
void func_800449B0(void);
void func_80044A04(void);
void func_80044E48(void);
void func_80044EC0(void);
void func_800459A8(void);
void func_80045AB4(void);
void func_80045D88(void);
void func_80045F40(void);
s32 func_8002FC78(void *, s32, s32);
void func_8004299C(void);
void func_80042C70(void);
void func_8004399C(void);
void func_80043B5C(void);
void func_80043D54(void);
void func_80044980(void);
void func_80044A70(void);
void func_8004599C(void);
void func_80045C18(void);
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
        D_80062A28 = func_80044A70;
        D_80062A2C = func_80042420;
        D_80062A30 = func_80042698;
        D_80062A34 = func_80044E48;
        D_80062A38 = func_80044EC0;
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
        D_80062A28 = func_80043B5C;
        D_80062A2C = func_80043B74;
        D_80062A30 = func_80043C48;
        D_80062A34 = func_80043C2C;
        D_80062A38 = func_80043D34;
    }
    if (D_80062AD8 & SCENE_FLAG_100) {
        D_80062A28 = func_80042C70;
        D_80062A2C = func_80042420;
        D_80062A30 = func_80042698;
        D_80062A34 = func_80042E84;
        D_80062A38 = func_80042F08;
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
        D_80062A28 = func_80044980;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = func_800449B0;
        D_80062A38 = func_80044A04;
        D_80062AC0 = D_8006CD90 << SCALE_SHIFT;
        D_80062AC4 = D_8006CD94;
    }
    if (D_80062AD8 & SCENE_FLAG_8) {
        p = (s32 *)func_80031AE8(D_800608DC, 0x1B8, D_800608E0, 0, 0, 0, 0, 0, 0);
        if (p != 0) {
            D_80062A48 = p[0];
            D_80062A4C = p[1];
        }
        D_80062A28 = func_8004299C;
        D_80062A2C = func_80042AB8;
        D_80062A30 = func_80042B7C;
        D_80062A34 = func_80042B44;
        D_80062A38 = func_80042C2C;
        D_80062AC0 = D_8006CD90;
    }
    if (D_80062AD8 & SCENE_FLAG_20) {
        p = (s32 *)func_80031AE8(D_800608DC, 0x1DE, D_800608E0, 0, 0, 0, 0, 0, 0);
        if (p != 0) {
            D_80062AB4 = p[0];
            D_80062AB8 = p[1];
            D_80062ABC = p[2];
        }
        D_80062A28 = func_8004399C;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = func_800439D4;
        D_80062A38 = func_80043A84;
        D_80062AE4 = 8;
        D_80062AE0 = 0xC;
    }
    if (D_80062AD8 & SCENE_FLAG_200000) {
        if (D_8006CDC4 != 0) {
            D_80062A28 = func_8004599C;
            D_80062A2C = func_80042420;
            D_80062A30 = func_800426A0;
            D_80062A34 = func_800459A8;
            D_80062A38 = func_80045AB4;
            D_80062A54 = D_8006CDC4;
        } else {
            p = (s32 *)func_80031AE8(D_800608DC, 0x1DE, D_800608E0, 0, 0, 0, 0, 0, 0);
            if (p != 0) {
                D_80062AB4 = p[0];
                D_80062AB8 = p[1];
                D_80062ABC = p[2];
            }
            D_80062A28 = func_8004399C;
            D_80062A2C = func_80042420;
            D_80062A30 = func_800426A0;
            D_80062A34 = func_800439D4;
            D_80062A38 = func_80043A84;
            D_80062AE4 = 8;
            D_80062AE0 = 0xC;
        }
    }
    if (D_80062AD8 & SCENE_FLAG_40) {
        D_80062A28 = func_80043D54;
        D_80062A2C = func_80042420;
        D_80062A30 = func_800426A0;
        D_80062A34 = func_80043D84;
        D_80062A38 = func_80043EA0;
    }
    if (D_8006CB94 & GLOBAL_FLAG_1000000) {
        frame[6] = func_80011F78(D_8005F1FC);
        D_80062A28 = func_80045C18;
        D_80062A2C = func_80045D88;
        D_80062A30 = func_80045F40;
        D_80062A34 = func_80042548;
        D_8006CDAC = &frame[6];
        D_80062A38 = func_80042818;
        D_80062AAC = func_8002FC78(&frame[6], D_8006CDB0 >> FIXED_SHIFT, 0);
        D_80062AB0 = func_8002FC78(D_8006CDAC, D_8006CDB0 >> FIXED_SHIFT, 1);
        D_80062AC8 = (func_8002FC78(D_8006CDAC, 8U, 0) + 4) & ALIGN_MASK_16;
    }
}
