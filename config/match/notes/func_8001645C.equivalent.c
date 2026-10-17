#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
    u8 pad2030[0x2088 - 0x2030];
    void *unk2088;
    char *unk208C;
    char *unk2090;
} Unk80063458;

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

#define FRAME_FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define FRAME_TIME_OFFSET 0xC
#define FRAME_END_TIME_OFFSET 0x10
#define FRAME_DURATION_OFFSET 0x14
#define FRAME_OT_OFFSET 0x18
#define FRAME_DISP_ENV_OFFSET 0x2018
#define FRAME_DISP_FLAG_OFFSET 0x2020
#define FRAME_DRAW_ENV_OFFSET 0x202C

#define FADE_STEP_MIN -2
#define FADE_BASE 0x100
#define FADE_DONE_IN -1
#define FADE_DONE_OUT -2
#define FLAG_COUNT_FRAMES 0x1000
#define FLAG_BAND 0x2000
#define FLAG_NO_CLEAR_FADE 0x400000
#define FLAG_RCNT_STOP 0x200000
#define FLAG_PRE_DRAW 0x40000
#define BAND_MODE_FULL 1
#define BAND_MODE_SPLIT 2
#define BAND_KEY_A 0x1FA
#define BAND_KEY_B 0x1F9
#define BAND_WIDTH 0x200
#define BAND_HEIGHT 0xD8
#define BAND_LAST_ROW 0xD7
#define BAND_CENTER_OFFSET 0x6C
#define BAND_SCALE 0xAA
#define BAND_SHIFT 20
#define PAN_SCALE 0x78
#define PAN_SHIFT 3
#define BYTE_MASK 0xFF
#define BITS_PER_BYTE 8
#define BITS_PER_SHORT 16
#define ARG_SLOT_SHORT 3
#define RCNT_SPEC 0xF2000002U
#define FRAME_TIME_LIMIT 0x19U
#define FRAME_TIME_DEFAULT 0x11
#define FRAME_TIME_PENALTY 0x22

extern s32 D_8003BEA4;
extern s32 D_8006CCDC;
extern s32 D_80060944;
extern Unk80063458 *D_80063458[];
extern s32 D_80060800;
extern Unk80060884 D_80060884;
extern s32 D_8005F180;
extern s32 *D_8005F374;
extern s32 *D_800608CC;
extern s32 D_800608D8;
extern s32 D_800608E0;
extern u8 *D_80063450;
extern u8 *D_80063454;
extern s32 D_800675AC;
extern s32 D_8006CB80;
extern s32 D_8006CB94;
extern s32 D_8006CC1C;
extern s32 D_8006CC94;
extern s32 D_8006CCF0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8005F1D4;
extern s32 D_8005F1D8;

int func_8001645C(void) {
    s32 sp28;
    s32 *res;
    s32 mode;
    s32 y;
    u32 color;
    u32 color2;
    u8 *tmp;
    s32 fade;

    D_8006CCF0 += 1;
    if ((D_8005F180 == 0) && (D_8006CB94 & FLAG_COUNT_FRAMES)) {
        D_80060944 += 1;
    }
    fade = D_8006CD18;
    if (fade != 0) {
        if (fade > 0) {
            D_8006CD18 = fade - D_8006CD1C;
            if (D_8006CD18 < 0) {
                D_8006CD18 = 0;
            }
            func_80016B08(D_8006CD18);
        } else if (fade >= FADE_STEP_MIN) {
            func_80016B08(FADE_BASE);
            D_8006CD18 = FADE_DONE_IN;
        } else {
            D_8006CD18 = fade + D_8006CD1C;
            func_80016B08(D_8006CD18 + FADE_BASE);
            if (D_8006CD18 == 0) {
                if (!(D_8006CB94 & FLAG_NO_CLEAR_FADE)) {
                    D_8006CD18 = FADE_DONE_OUT;
                }
            }
        }
    }
    func_80016C58(D_8006CC94, D_8005F1D4);
    D_8005F1D4 = D_8006CC94;
    if (D_8006CB94 & FLAG_BAND) {
        mode = BAND_MODE_FULL;
        color = 0;
        y = 0;
        color2 = 0;
        res = 0;
        if (D_800608CC != 0) {
            res = (s32 *)func_80031AE8(D_800608D8, BAND_KEY_A, D_800608E0, 0, 0, 0, 0, 0, 0);
            if (res != 0) {
                mode = res[0];
                color = res[1];
                if (mode == BAND_MODE_SPLIT) {
                    color2 = res[2];
                    res = (s32 *)func_80031AE8(D_800608D8, BAND_KEY_B, D_800608E0, 0, 0, ARG_SLOT_SHORT, &sp28, 0, 0);
                    y = 0;
                    if (res != 0) {
                        y = *(s16 *)res;
                    }
                }
            }
        }
        if (mode == BAND_MODE_SPLIT) {
            y = ((D_80060884.a * (func_8003D2EC(D_80060800) * BAND_SCALE)) >> BAND_SHIFT) - (y - BAND_CENTER_OFFSET);
            if (D_8006CC94 != 0) {
                y -= (D_8006CC94 * PAN_SCALE) >> PAN_SHIFT;
            }
            if (y < 0) {
                color = color2;
                mode = BAND_MODE_FULL;
            } else if (y >= BAND_LAST_ROW) {
                mode = BAND_MODE_FULL;
            } else {
                /* band stays split */
            }
        }
        switch (mode) {
        case BAND_MODE_SPLIT:
            func_80016A60(0, 0, BAND_WIDTH, (s16)(y + 1), color & BYTE_MASK, (color >> BITS_PER_BYTE) & BYTE_MASK, (color >> BITS_PER_SHORT) & BYTE_MASK);
            func_80016A60(0, (s16)(y + 1), BAND_WIDTH, BAND_LAST_ROW - y, color2 & BYTE_MASK, (color2 >> BITS_PER_BYTE) & BYTE_MASK, (color2 >> BITS_PER_SHORT) & BYTE_MASK);
            break;
        case BAND_MODE_FULL:
            func_80016A60(0, 0, BAND_WIDTH, BAND_HEIGHT, color & BYTE_MASK, (color >> BITS_PER_BYTE) & BYTE_MASK, (color >> BITS_PER_SHORT) & BYTE_MASK);
            break;
        default:
            /* other modes draw nothing */
            break;
        }
    }
    func_800334A0();
    FRAME_FIELD(D_80063454, FRAME_END_TIME_OFFSET, s32) = D_8003BEA4;
    DrawSync(0);
    if (D_8006CD14 == 0) {
        D_8006CC1C = D_8003BEA4 - FRAME_FIELD(D_80063454, FRAME_TIME_OFFSET, s32);
    } else {
        D_8006CC1C = FRAME_TIME_DEFAULT;
    }
    if (D_800675AC == 0) {
        tmp = D_80063454;
        D_80063450 = tmp;
        D_80063454 = (u8 *)D_80063458[0];
        D_80063458[0] = (Unk80063458 *)tmp;
    }
    VSync(0);
    if ((u32)(D_8003BEA4 - FRAME_FIELD(D_80063450, FRAME_TIME_OFFSET, s32)) < FRAME_TIME_LIMIT) {
        VSync(0);
    }
    if (D_8006CB94 & FLAG_RCNT_STOP) {
        if (D_8005F1D8 == 0) {
            EnterCriticalSection();
            StopRCnt(RCNT_SPEC);
            ExitCriticalSection();
        }
        D_8005F1D8 = 1;
        func_800345C8();
        D_8003BEA4 += FRAME_TIME_PENALTY;
    } else {
        if (D_8005F1D8 != 0) {
            EnterCriticalSection();
            StartRCnt(RCNT_SPEC);
            ExitCriticalSection();
        }
        D_8005F1D8 = 0;
    }
    EnterCriticalSection();
    if (D_8006CD14 == 2) {
        D_8003BEA4 = *D_8005F374;
    }
    FRAME_FIELD(D_80063454, FRAME_TIME_OFFSET, s32) = D_8003BEA4;
    ExitCriticalSection();
    FRAME_FIELD(D_80063450, FRAME_DURATION_OFFSET, s32) = func_80016F04(FRAME_FIELD(D_80063454, FRAME_TIME_OFFSET, s32) - FRAME_FIELD(D_80063450, FRAME_TIME_OFFSET, s32));
    FRAME_FIELD(D_80063450, FRAME_DISP_FLAG_OFFSET, s16) = (s16)D_8006CCDC;
    PutDispEnv(D_80063450 + FRAME_DISP_ENV_OFFSET);
    PutDrawEnv(D_80063454 + FRAME_DRAW_ENV_OFFSET);
    if (D_800675AC > 0) {
        D_800675AC -= 1;
    }
    if (D_800675AC == 0) {
        if (D_8006CB94 & FLAG_PRE_DRAW) {
            func_8002706C();
        }
        DrawOTag(D_80063454 + FRAME_OT_OFFSET);
    }
    func_800163C4((s32 *)D_80063458[0]);
    D_8006CB94 = D_8006CB80;
    return D_8006CB80;
}
