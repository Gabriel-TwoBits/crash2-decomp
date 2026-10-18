#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F210 = 0xA;
s32 D_8005F214 = 0x7F;

#ifdef NON_MATCHING
extern s32 D_8005F180;
extern s32 D_8005F4AC[16];

void func_8002FB84(s32 i) {
    s32 *p;
    s32 v;

    if (i != 0) {
        i = 0;
        do {
            D_8005F4AC[i] = ((D_8005F214 + 1) / 8) * i - D_8005F214;
            i++;
        } while (i < 16);
        return;
    }
    if (D_8005F180 == 0) {
        do {
            D_8005F4AC[i] += D_8005F210;
            if (D_8005F214 < D_8005F4AC[i]) {
                D_8005F4AC[i] = -D_8005F214 + 1;
            }
            i++;
        } while (i < 16);
        i = 0;
    }
    p = D_8005F4AC;
    do {
        v = *p;
        if (v < 0) {
            v = -v;
        }
        ((s8 *)0x1F800100)[i] = v;
        i++;
        p++;
    } while (i < 16);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FB84);
#endif

s32 func_8002FC78(void *arg0, s32 arg1, s32 arg2) {
    s32 *p = (s32 *)((char *)func_80014260(arg0, 1, 0) + 0x14);
    return p[arg1 * 2 + arg2] + 3;
}

extern s32 D_8005F3D8;
extern s32 D_8006CD14;

void func_8002FCD0(void) {
    D_8005F3D8 = 0;
    D_8006CD14 = 0;
}

void func_8002FCE8(void) {
}

void func_8002FCF0(void) {
}

#ifdef NON_MATCHING
#define STATE_LOADING 2
#define NO_ENTRY -1

typedef struct Unk8002FCF8Cam {
    /* 0x00 */ s32 unk_00[3];
    /* 0x0C */ s32 unk_0C;
} Unk8002FCF8Cam;

/* What GoolObject.unk_8 points to, as func_8002FCF8 reads it */
typedef struct Unk8002FCF8Dst {
    /* 0x00 */ char unk_00[0x30];
    /* 0x30 */ s32 unk_30[6];
} Unk8002FCF8Dst;

typedef struct Unk8002FCF8Block {
    /* 0x0000 */ s32 unk_0000;
    /* 0x0004 */ s32 unk_0004;
    /* 0x0008 */ s32 unk_0008;
    /* 0x000C */ s32 unk_000C;
    /* 0x0010 */ u8 unk_0010[0x1060];
    /* 0x1070 */ s32 unk_1070;
    /* 0x1074 */ s32 unk_1074[6];
    /* 0x108C */ char unk_108C[4];
    /* 0x1090 */ char unk_1090[1];
} Unk8002FCF8Block;

extern void *D_8005F374;
extern GoolObject *g_Player;
#define BLOCK ((Unk8002FCF8Block *)D_8005F3D8)
extern s32 D_80060944;
extern Unk8002FCF8Cam *g_DrawBuffer;
extern s32 D_8006CBF0;
extern s32 D_8006CC00;
extern s32 D_8006CC84;

void func_8002FCF8(void) {
    Unk8002FCF8Block *block;
    GoolObject *holder;
    Unk8002FCF8Dst *dst;
    s32 w0, w1, w2, w3, w4, w5;

    D_8006CD14 = STATE_LOADING;
    D_8005F374 = BLOCK->unk_1090;
    Rand_SetSeedAlt(BLOCK->unk_0004);
    block = BLOCK;
    D_80060944 = block->unk_000C;
    holder = g_Player;
    g_DrawBuffer->unk_0C = block->unk_1070;
    dst = holder->unk_8;
    w0 = block->unk_1074[0];
    w1 = block->unk_1074[1];
    w2 = block->unk_1074[2];
    w3 = block->unk_1074[3];
    dst->unk_30[0] = w0;
    dst->unk_30[1] = w1;
    dst->unk_30[2] = w2;
    dst->unk_30[3] = w3;
    w4 = block->unk_1074[4];
    w5 = block->unk_1074[5];
    dst->unk_30[4] = w4;
    dst->unk_30[5] = w5;
    D_8006CC84 = NO_ENTRY;
    func_8002F6C8(BLOCK->unk_0010);
    D_8006CC00 = 0;
    D_8006CBF0 = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FCF8);
#endif

#ifdef NON_MATCHING
extern s32 D_8005F3D8;
extern s32 D_800675AC;
extern s32 D_8006CB20;

void func_8002FDCC(s32 *arg0) {
    int new_var2;
    s32 buf[2];
    s32 *new_var;
    s32 *temp_v0;

    D_800675AC = 0;
    temp_v0 = func_80014260(arg0, 1, 1);
    do { new_var2 = 0x6396347F; new_var = arg0; } while (0);
    *new_var = new_var2;
    D_8005F3D8 = temp_v0[4];
    buf[1] = 0x6400;
    buf[0] = 0;
    Obj_Spawn(&D_8006CB20, 4, 8, 2, (s32)buf, 1);
    func_8002FCF8();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002FB84", func_8002FDCC);
#endif
