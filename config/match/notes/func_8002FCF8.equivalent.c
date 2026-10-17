#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATE_LOADING 2
#define NO_ENTRY -1

typedef struct Unk8002FCF8Cam {
    /* 0x00 */ s32 unk_00[3];
    /* 0x0C */ s32 unk_0C;
} Unk8002FCF8Cam;

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

typedef struct Unk8002FCF8Holder {
    /* 0x00 */ char unk_00[8];
    /* 0x08 */ Unk8002FCF8Dst *unk_08;
} Unk8002FCF8Holder;

extern void *D_8005F374;
extern Unk8002FCF8Holder *D_8005F38C;
#define BLOCK ((Unk8002FCF8Block *)D_8005F3D8)
extern s32 D_80060944;
extern Unk8002FCF8Cam *D_80063454;
extern s32 D_8006CBF0;
extern s32 D_8006CC00;
extern s32 D_8006CC84;

void func_8002FCF8(void) {
    Unk8002FCF8Block *block;
    Unk8002FCF8Holder *holder;
    Unk8002FCF8Dst *dst;
    s32 w0, w1, w2, w3, w4, w5;

    D_8006CD14 = STATE_LOADING;
    D_8005F374 = BLOCK->unk_1090;
    func_8003156C(BLOCK->unk_0004);
    block = BLOCK;
    D_80060944 = block->unk_000C;
    holder = D_8005F38C;
    D_80063454->unk_0C = block->unk_1070;
    dst = holder->unk_08;
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
