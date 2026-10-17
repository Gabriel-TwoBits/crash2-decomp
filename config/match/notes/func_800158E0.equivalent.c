#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80069920 {
    /* 0x00 */ s8 unk_00;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u8 unk_04[2];
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 unk_08[0x1C];
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 unk_28;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s32 unk_3C;
    /* 0x40 */ s32 unk_40;
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ s32 unk_48;
} Unk80069920;

#define STICK_MIN (-0x80)
#define STICK_MAX 0x7F
#define STICK_SHIFT 7
#define ANGLE_MASK 0xFFF
#define ANGLE_OFFSET 0x8FA
#define DIR_SHIFT 9
#define DIR_NEUTRAL 8
#define MAG_MIN 0x66
#define MAG_SHIFT_UP 8
#define MAG_SHIFT_DOWN 7
#define ANALOG_ID_UPPER 0x53
#define ANALOG_ID_LOWER 0x73
#define BTN_ALL_MASK 0xFFFF
#define BTN_KEEP_MASK 0xF961
#define BTN_BIT_2 0x2
#define BTN_BIT_4 0x4
#define BTN_BIT_8 0x8
#define BTN_BIT_10 0x10
#define BTN_BIT_80 0x80
#define BTN_SEL_MASK 0x9F0
#define BTN_CLEAR_MASK 0xFFFF0F17
#define BTN_1000 0x1000
#define BTN_2000 0x2000
#define BTN_4000 0x4000
#define BTN_8000 0x8000
#define DIR_BITS_MASK 0xF000
#define DIR_BITS_SHIFT 12
#define PAD_FLAG_ENABLE 0x10000
#define CAMERA_FLAG 0x200000
#define CAMERA_FLAG_OFFSET 0x29C
#define CAMERA_PTR_OFFSET 0x10
#define SMOOTH_ARG1 0x200
#define SMOOTH_ARG2 0x5000
#define STATE_PLAYBACK_END 3
#define STATE_PLAYBACK_START 4
#define STATE_PLAYBACK 2
#define STREAM_ID 0xE00
#define STREAM_UNK_VALUE 0x108
#define STREAM_END_OFFSET 0x1090
#define STREAM_COUNT_SCALE 8
#define STREAM_RESTART_OFFSET 0x108C
#define STREAM_RESTART_DST 0x14
#define STREAM_RECORD_SIZE 8
#define STREAM_RECORD_BTN 4
#define FLAGS_80_CLEAR 0xFFFF6FF5
#define DEFAULT_D18 (-0x100)

extern Unk80069920 D_80069920[];
extern s32 D_8006991C;
extern s32 D_8006CDD0[6];
extern s32 D_8005B440[][3];
extern s32 D_8005F180;
extern u8 *D_8005F374;
extern u8 *D_8005F3D8;
extern s32 *D_800608CC;
extern u8 *D_80063450;
extern s32 D_8006CB80;
extern s32 D_8006CB94;
extern s32 D_8006CCA0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD60;
extern s32 D_8006CD64;
extern s32 D_8006CD6C;

static s32 clamp_stick(s32 v) {
    if (v < STICK_MAX) {
        if (v < STICK_MIN) {
            return STICK_MIN;
        } else {
            return v;
        }
    } else {
        return STICK_MAX;
    }
}

void func_800158E0(void) {
    s32 i;
    Unk80069920 *p;
    s32 dx;
    s32 dy;
    s32 span;
    s32 x;
    s32 y;
    s32 dir;
    s32 mag;
    s32 raw;
    s32 btn;
    s32 sel;
    s32 stream_flag;

    for (i = 0; i < D_8006991C; i++) {
        p = &D_80069920[i];
        p->unk_44 = 0;
        p->unk_34 = p->unk_2C;
        p->unk_2C = p->unk_28;
        p->unk_30 = p->unk_24;

        if (p->unk_00 == 0) {
            if (p->unk_01 == ANALOG_ID_UPPER || p->unk_01 == ANALOG_ID_LOWER) {
                dx = p->unk_06 - D_8006CDD0[0];
                if (dx < 0) {
                    span = D_8006CDD0[0] - D_8006CDD0[2];
                } else {
                    span = D_8006CDD0[4] - D_8006CDD0[0];
                }
                x = clamp_stick((dx << STICK_SHIFT) / span);

                dy = p->unk_07 - D_8006CDD0[1];
                if (dy < 0) {
                    span = D_8006CDD0[1] - D_8006CDD0[3];
                } else {
                    span = D_8006CDD0[5] - D_8006CDD0[1];
                }
                y = clamp_stick((dy * STICK_MAX) / span);

                if (D_800608CC != 0
                    && (*(s32 *)(*(u8 **)((u8 *)D_800608CC + CAMERA_PTR_OFFSET) + CAMERA_FLAG_OFFSET) & CAMERA_FLAG)) {
                    p->unk_38 = func_8001E640(ratan2(x, y) & ANGLE_MASK, SMOOTH_ARG1, SMOOTH_ARG2);
                } else {
                    p->unk_38 = ratan2(x, y) & ANGLE_MASK;
                }
                /* the masked value is never negative */
                p->unk_40 = ((ANGLE_OFFSET - p->unk_38) & ANGLE_MASK) >> DIR_SHIFT;
                mag = (func_80041A78(x * x + y * y) << MAG_SHIFT_UP) >> MAG_SHIFT_DOWN;
                p->unk_3C = mag;
                if (mag < MAG_MIN) {
                    p->unk_3C = 0;
                    p->unk_40 = DIR_NEUTRAL;
                } else {
                    /* magnitude above the dead zone */
                }
                raw = ~((p->unk_02 >> 8) | ((p->unk_02 & 0xFF) << 8));
                p->unk_28 = raw & BTN_ALL_MASK;
                if (p->unk_01 == ANALOG_ID_UPPER) {
                    btn = raw & BTN_KEEP_MASK;
                    if (raw & BTN_BIT_2) {
                        btn |= BTN_BIT_4;
                    }
                    if (raw & BTN_BIT_4) {
                        btn |= BTN_BIT_80;
                    }
                    if (raw & BTN_BIT_8) {
                        btn |= BTN_BIT_10;
                    }
                    if (raw & BTN_BIT_10) {
                        btn |= BTN_BIT_8;
                    }
                    if (raw & BTN_BIT_80) {
                        btn |= BTN_BIT_2;
                    }
                    p->unk_28 = btn;
                } else {
                    /* ANALOG_ID_LOWER keeps the plain button word */
                }
            } else {
                p->unk_3C = 0;
                p->unk_40 = DIR_NEUTRAL;
                p->unk_28 = ~((p->unk_02 >> 8) | ((p->unk_02 & 0xFF) << 8)) & BTN_ALL_MASK;
            }
        } else {
            /* controller not in the primary mode: state kept */
        }

        if (i == 0) {
            switch ((u32)D_8006CD14) {
            case STATE_PLAYBACK_END:
                p->unk_28 = 0;
                break;
            case STATE_PLAYBACK_START:
                if ((u32)(D_8006CD18 + 1) < 2U) {
                    sel = D_8006CCA0;
                    stream_flag = (u32)D_8005F374 >= (u32)(D_8005F3D8 + (*(s32 *)D_8005F3D8 * STREAM_COUNT_SCALE) + STREAM_END_OFFSET);
                    if (sel != 0) {
                        func_8001CC10(0, sel, STREAM_ID, 1, &stream_flag);
                        D_8006CB80 = STREAM_UNK_VALUE;
                        D_8006CD14 = STATE_PLAYBACK_END;
                    } else {
                        D_8006CD14 = 0;
                    }
                } else {
                    /* other values of D_8006CD18 leave the state alone */
                }
                break;
            case STATE_PLAYBACK:
                p->unk_3C = 0;
                p->unk_40 = DIR_NEUTRAL;
                sel = p->unk_28;
                if (D_8005F374 == D_8005F3D8 + STREAM_END_OFFSET) {
                    *(s32 *)(D_80063450 + STREAM_RESTART_DST) = *(s32 *)(D_8005F3D8 + STREAM_RESTART_OFFSET);
                } else {
                    /* not at the first record */
                }
                p->unk_28 = *(s32 *)(D_8005F374 + STREAM_RECORD_BTN);
                D_8005F374 += STREAM_RECORD_SIZE;
                if (((sel & BTN_SEL_MASK) && p->unk_00 == 0)
                    || (u32)D_8005F374 >= (u32)(D_8005F3D8 + (*(s32 *)D_8005F3D8 * STREAM_COUNT_SCALE) + STREAM_END_OFFSET)) {
                    if (D_8006CCA0 != 0) {
                        D_8006CD14 = STATE_PLAYBACK_START;
                        D_8006CB80 &= FLAGS_80_CLEAR;
                        if (D_8006CD18 == 0) {
                            D_8006CD18 = DEFAULT_D18;
                        } else {
                            /* D_8006CD18 already set */
                        }
                    } else {
                        D_8006CD14 = 0;
                    }
                } else {
                    /* playback continues */
                }
                break;
            default:
                break;
            }
        } else {
            /* only the first pad drives playback */
        }

        if (i == 1) {
            D_8006CD6C = p->unk_28;
            p->unk_3C = 0;
            p->unk_40 = DIR_NEUTRAL;
            p->unk_28 = 0;
        } else {
            /* second pad is the only one captured */
        }

        if ((!(D_8006CB94 & PAD_FLAG_ENABLE) && D_8005F180 == 0)
            || (p->unk_00 != 0 && D_8006CD14 == 0)) {
            p->unk_3C = 0;
            p->unk_40 = DIR_NEUTRAL;
            p->unk_28 &= BTN_CLEAR_MASK;
        } else {
            /* pad input is live */
        }

        if (p->unk_28 & BTN_1000) {
            p->unk_28 &= ~BTN_4000;
        } else {
            /* bit 0x4000 kept */
        }
        if (p->unk_28 & BTN_8000) {
            p->unk_28 &= ~BTN_2000;
        } else {
            /* bit 0x2000 kept */
        }

        dir = ((u32)(p->unk_28 & DIR_BITS_MASK)) >> DIR_BITS_SHIFT;
        if (dir != 0) {
            p->unk_40 = D_8005B440[dir][0];
            p->unk_38 = D_8005B440[dir][1];
            p->unk_3C = D_8005B440[dir][2];
        } else {
            switch ((u32)p->unk_40) {
            case 0: p->unk_28 |= BTN_1000; break;
            case 1: p->unk_28 |= BTN_1000 | BTN_2000; break;
            case 2: p->unk_28 |= BTN_2000; break;
            case 3: p->unk_28 |= BTN_2000 | BTN_4000; break;
            case 4: p->unk_28 |= BTN_4000; break;
            case 5: p->unk_28 |= BTN_4000 | BTN_8000; break;
            case 6: p->unk_28 |= BTN_8000; break;
            case 7: p->unk_28 |= BTN_8000 | BTN_1000; break;
            default: break;
            }
        }

        if (i == 0) {
            D_8006CD60 = p->unk_3C;
            D_8006CD64 = p->unk_38;
        } else {
            /* only the first pad is mirrored */
        }
        p->unk_24 = p->unk_28 & ~p->unk_2C & BTN_ALL_MASK;
    }
}
