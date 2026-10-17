#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F1D4 = 0;
s32 D_8005F1D8 = 0;
s32 D_8005F1DC = 0;
s32 D_8005F1E0 = 0;
s32 D_8005F1E4 = 0x808080;
s32 D_8005F1E8 = 0x8000;

#ifdef NON_MATCHING
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
extern s32 *D_8005F374;
extern u8 *D_8005F3D8;
extern s32 *D_800608CC;
extern u8 *g_DisplayBuffer;
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
                if ((u8 *)D_8005F374 == D_8005F3D8 + STREAM_END_OFFSET) {
                    *(s32 *)(g_DisplayBuffer + STREAM_RESTART_DST) = *(s32 *)(D_8005F3D8 + STREAM_RESTART_OFFSET);
                } else {
                    /* not at the first record */
                }
                p->unk_28 = *(s32 *)((u8 *)D_8005F374 + STREAM_RECORD_BTN);
                D_8005F374 = (s32 *)((u8 *)D_8005F374 + STREAM_RECORD_SIZE);
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
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800158E0);
#endif

void SetDefDrawEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void PutDrawEnv(void *env);
void TermPrim(void *p);
void DrawPrim(void *p);

typedef struct {
    u8 pad0[0x3];
    u8 len;
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    s16 w, h;
} Fill;

void Gfx_ClearVram(void) {
    u8 env[0x60];
    Fill f;

    SetDefDrawEnv(env, 0, 0, 0x400, 0x200);
    env[0x17] = 1;
    PutDrawEnv(env);
    TermPrim(&f);
    f.len = 3;
    f.code = 2;
    f.w = 0x3FF;
    f.h = 0x1FF;
    f.r0 = 0;
    f.g0 = 0;
    f.b0 = 0;
    f.x0 = 0;
    f.y0 = 0;
    DrawPrim(&f);
}

typedef struct {
    s32 idx;
    s32 pad4[2];
    s32 f0C;
    s32 pad10;
    s32 f14;
    u8 pad18[0x2018 - 0x18];
    u8 disp[0x14];
    u8 draw[0x2094 - 0x202C];
} Unk800160B0Entry;

typedef struct {
    s32 count;
    Unk800160B0Entry *cur;
    Unk800160B0Entry *other;
    Unk800160B0Entry *first;
    Unk800160B0Entry e[1];
} Unk800160B0Gfx;

extern Unk800160B0Gfx g_FrameBuffers;
extern s32 g_TimerTicks;
extern s32 D_8006CCDC;
extern s32 D_80060944;
void PutDispEnv(void *env);
void SetDefDispEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void SetDispMask(s32 mask);
void SetGraphDebug(s32 level);

void Gfx_Init(void) {
    s32 i;
    Unk800160B0Entry *e;
    D_80060944 = 0;
    ResetGraph(0);
    SetGraphDebug(0);
    Gfx_ClearVram();
    g_FrameBuffers.count = 2;
    for (i = 0; i < g_FrameBuffers.count; i++) {
        e = &g_FrameBuffers.e[i];
        e->idx = i;
        e->f0C = 0;
        SetDefDispEnv(e->disp, i << 9, 0, 0x200, 0xF0);
        D_8006CCDC = 0;
        *(s16 *)(e->disp + 8) = 0;
        SetDefDrawEnv(e->draw, i << 9, 0xC, 0x200, 0xD8);
        *(s16 *)(e->draw + 8) = i << 9;
        *(s16 *)(e->draw + 10) = 0xC;
        e->draw[0x16] = 1;
        e->draw[0x17] = 0;
        if (i == 0) {
            g_FrameBuffers.first = e;
            g_FrameBuffers.cur = e;
        }
        if (i == 1) {
            g_FrameBuffers.other = e;
        }
        e->f14 = 0x22;
        e->f0C = g_TimerTicks;
    }
    PutDispEnv(g_FrameBuffers.cur->disp);
    PutDrawEnv(g_FrameBuffers.other->draw);
    SetDispMask(1);
}

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

#ifdef NON_MATCHING
extern Unk80063458 *g_BuildBuffer[];
#define RETAIL_STATE_LOAD 2
#define RETAIL_STATE_PAL 0x28
#define CLEAR_OFFSET 0x18
#define CLEAR_BYTES 0x800
#define SHARED_BUF_BYTES 0x1770
#define ENTRY_BUF_BYTES 0x16400
#define ENTRY_BUF_BYTES_PAL 0x2C800
#define ENTRY_BUF_OFFSET 4
#define ENTRY_SHARED_OFFSET 0x208C
#define ALLOC_FAIL_EXIT_CODE -0xF
#define RETURN_VALUE -0xFF

extern void exit(s32);
extern s32 D_8005F3EC;
extern u8 *g_DrawBuffer;

s32 func_800161F8(void) {
    s32 i;
    s32 shared;
    s32 buf;
    s32 size;
    u8 *entry;

    if (D_8005F3EC == RETAIL_STATE_LOAD) {
        func_80036368();
    }
    func_8003BC5C((u8 *)g_BuildBuffer[0] + CLEAR_OFFSET, CLEAR_BYTES);
    func_8003BC5C(g_DrawBuffer + CLEAR_OFFSET, CLEAR_BYTES);
    i = 0;
    shared = Heap_Alloc(SHARED_BUF_BYTES);
    if (g_FrameBuffers.count > 0) {
        entry = (u8 *)g_FrameBuffers.e;
        do {
            size = ENTRY_BUF_BYTES;
            *(s32 *)(entry + ENTRY_SHARED_OFFSET) = shared;
            if (D_8005F3EC == RETAIL_STATE_PAL) {
                size = ENTRY_BUF_BYTES_PAL;
            }
            buf = Heap_Alloc(size);
            *(s32 *)(entry + ENTRY_BUF_OFFSET) = buf;
            if (buf == 0) {
                exit(ALLOC_FAIL_EXIT_CODE);
            }
            func_800163C4((s32 *)entry);
            i++;
            entry += sizeof(*g_FrameBuffers.e);
        } while (i < g_FrameBuffers.count);
    }
    return RETURN_VALUE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800161F8);
#endif

#ifdef NON_MATCHING
#define STATE_FLUSH 2
#define RESULT_DONE -0xFF

extern s32 D_800654E8;
extern s32 DrawSync(s32);

s32 func_800162FC(void) {
    s32 i;

    i = 0;
    func_800163B4(g_BuildBuffer[0]);
    DrawSync(0);
    Heap_Free(D_800654E8);
    if (g_FrameBuffers.count > 0) {
        do {
            Heap_Free(g_FrameBuffers.e[i].pad4[0]);
            i++;
        } while (i < g_FrameBuffers.count);
    }
    if (D_8005F3EC == STATE_FLUSH) {
        func_800363D0();
    }
    return RESULT_DONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800162FC);
#endif

void func_800163B4(void *arg0) {
    Unk80063458 *p = arg0;

    p->unk2088 = 0;
    p->unk2090 = p->unk208C;
}

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4(arg0);
}


extern Unk80063458 *g_BuildBuffer[];

void func_800163E8(s32 arg0) {
    s32 sum;

    sum = g_BuildBuffer[0]->unk8 + arg0;
    /* Both branches store the same value; the branch is what puts the sum in $a0. */
    if (g_BuildBuffer[0]->unk8) {
        g_BuildBuffer[0]->unk8 = sum;
    } else {
        g_BuildBuffer[0]->unk8 = sum;
    }
}

s32 *func_80016408(void) {
    return &g_BuildBuffer[0]->unk8;
}

void *func_80016418(s32 arg0) {
    void **temp;

    temp = (void **)g_BuildBuffer[0]->unk2090;
    g_BuildBuffer[0]->unk2090 = (char *)temp + arg0;
    *temp = g_BuildBuffer[0]->unk2088;
    g_BuildBuffer[0]->unk2088 = temp;
    return temp;
}

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

#ifdef NON_MATCHING
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

extern s32 D_80060800;
extern Unk80060884 D_80060884;
extern s32 D_8005F180;
extern s32 *D_8005F374;
extern s32 *D_800608CC;
extern s32 D_800608D8;
extern s32 D_800608E0;
extern u8 *g_DisplayBuffer;
extern s32 D_800675AC;
extern s32 D_8006CB80;
extern s32 D_8006CB94;
extern s32 D_8006CC1C;
extern s32 D_8006CC94;
extern s32 g_FrameCount;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8005F1D4;
extern s32 D_8005F1D8;

int Gfx_EndFrame(void) {
    s32 sp28;
    s32 *res;
    s32 mode;
    s32 y;
    u32 color;
    u32 color2;
    u8 *tmp;
    s32 fade;

    g_FrameCount += 1;
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
            y = ((D_80060884.a * (Math_Sin(D_80060800) * BAND_SCALE)) >> BAND_SHIFT) - (y - BAND_CENTER_OFFSET);
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
            Gfx_AddFillRect(0, 0, BAND_WIDTH, (s16)(y + 1), color & BYTE_MASK, (color >> BITS_PER_BYTE) & BYTE_MASK, (color >> BITS_PER_SHORT) & BYTE_MASK);
            Gfx_AddFillRect(0, (s16)(y + 1), BAND_WIDTH, BAND_LAST_ROW - y, color2 & BYTE_MASK, (color2 >> BITS_PER_BYTE) & BYTE_MASK, (color2 >> BITS_PER_SHORT) & BYTE_MASK);
            break;
        case BAND_MODE_FULL:
            Gfx_AddFillRect(0, 0, BAND_WIDTH, BAND_HEIGHT, color & BYTE_MASK, (color >> BITS_PER_BYTE) & BYTE_MASK, (color >> BITS_PER_SHORT) & BYTE_MASK);
            break;
        default:
            /* other modes draw nothing */
            break;
        }
    }
    func_800334A0();
    FRAME_FIELD(g_DrawBuffer, FRAME_END_TIME_OFFSET, s32) = g_TimerTicks;
    DrawSync(0);
    if (D_8006CD14 == 0) {
        D_8006CC1C = g_TimerTicks - FRAME_FIELD(g_DrawBuffer, FRAME_TIME_OFFSET, s32);
    } else {
        D_8006CC1C = FRAME_TIME_DEFAULT;
    }
    if (D_800675AC == 0) {
        tmp = g_DrawBuffer;
        g_DisplayBuffer = tmp;
        g_DrawBuffer = (u8 *)g_BuildBuffer[0];
        g_BuildBuffer[0] = (Unk80063458 *)tmp;
    }
    VSync(0);
    if ((u32)(g_TimerTicks - FRAME_FIELD(g_DisplayBuffer, FRAME_TIME_OFFSET, s32)) < FRAME_TIME_LIMIT) {
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
        g_TimerTicks += FRAME_TIME_PENALTY;
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
        g_TimerTicks = *D_8005F374;
    }
    FRAME_FIELD(g_DrawBuffer, FRAME_TIME_OFFSET, s32) = g_TimerTicks;
    ExitCriticalSection();
    FRAME_FIELD(g_DisplayBuffer, FRAME_DURATION_OFFSET, s32) = func_80016F04(FRAME_FIELD(g_DrawBuffer, FRAME_TIME_OFFSET, s32) - FRAME_FIELD(g_DisplayBuffer, FRAME_TIME_OFFSET, s32));
    FRAME_FIELD(g_DisplayBuffer, FRAME_DISP_FLAG_OFFSET, s16) = (s16)D_8006CCDC;
    PutDispEnv(g_DisplayBuffer + FRAME_DISP_ENV_OFFSET);
    PutDrawEnv(g_DrawBuffer + FRAME_DRAW_ENV_OFFSET);
    if (D_800675AC > 0) {
        D_800675AC -= 1;
    }
    if (D_800675AC == 0) {
        if (D_8006CB94 & FLAG_PRE_DRAW) {
            func_8002706C();
        }
        DrawOTag(g_DrawBuffer + FRAME_OT_OFFSET);
    }
    func_800163C4((s32 *)g_BuildBuffer[0]);
    D_8006CB94 = D_8006CB80;
    return D_8006CB80;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", Gfx_EndFrame);
#endif

typedef struct {
    u8 pad0[3];
    u8 len;
    u8 r, g, b;
    u8 code;
    s16 x, y;
    s16 w, h;
} Unk80016A60;


void Gfx_AddFillRect(s32 arg0, s32 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6) {
    Unk80016A60 *p;

    p = (Unk80016A60 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)((u8 *)p + 0x10);
    p->len = 3;
    p->code = 2;
    p->w = arg2;
    p->h = arg3;
    p->r = arg4;
    p->g = arg5;
    p->b = arg6;
    p->x = g_BuildBuffer[0]->unk202C + arg0;
    p->y = g_BuildBuffer[0]->unk202E + arg1;
    AddPrim(&g_BuildBuffer[0]->padC[0xC], p);
}

#ifdef NON_MATCHING
extern u8 D_8005B500[];
extern s32 D_8006CB94;
s32 GetTPage(s32, s32, s32, s32);
void SetDrawMode(void *, s32, s32, s32, s32 *);
void SetPolyF4(void *);
void SetSemiTrans(void *, s32);

void func_80016B08(u32 arg0) {
    u8 *p;
    u8 *q;
    u8 *t;
    s32 lo;
    s32 hi;

    q = (u8 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)(q + 0xC);
    p = (u8 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)(p + 0x18);
    if (arg0 != 0) {
        t = D_8005B500 + (arg0 >> 4);
        lo = t[0];
        hi = t[1];
        *(s16 *)(p + 8) = 0;
        *(s16 *)(p + 0x10) = 0;
        *(s16 *)(p + 0xA) = -0xC;
        *(s16 *)(p + 0xE) = -0xC;
        *(s16 *)(p + 0xC) = 0x200;
        *(s16 *)(p + 0x12) = 0xE4;
        *(s16 *)(p + 0x14) = 0x200;
        *(s16 *)(p + 0x16) = 0xE4;
        arg0 = ((u32)((hi - lo) * (arg0 & 0xF)) >> 4) + lo;
        p[4] = arg0;
        p[5] = arg0;
        p[6] = arg0;
        SetPolyF4(p);
        SetSemiTrans(p, 1);
        AddPrim(&g_BuildBuffer[0]->padC[0x2008], p);
        SetDrawMode(q, 1, 0, GetTPage(0, (D_8006CB94 & 0x800000) ? 1 : 2, 0, 0) & 0xFFFF, 0);
        AddPrim(&g_BuildBuffer[0]->padC[0x2008], q);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016B08);
#endif

#ifdef NON_MATCHING
typedef struct {
    u8 pad0[4];
    u8 r, g, b;
    u8 pad7;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
} PolyF4;

extern Unk80060884 D_80060884;
void SetGeomScreen(s32);

#define SCREEN_MODE_ALT 1
#define SCREEN_X_OFFSET 0x2034
#define SCREEN_Y_OFFSET 0x2036
#define PRIM_SIZE 0x18
#define OT_OFFSET 0x2008
#define SCREEN_CENTER_X 0x200
#define BAR_TOP -0xC
#define BAR_BOTTOM 0xE4

void func_80016C58(s32 arg0, s32 arg1) {
    PolyF4 *p;
    s32 w;
    u32 x;
    u32 y;
    s16 right, left, top, bot, mid;
    s32 ret;

    p = (PolyF4 *)g_BuildBuffer[0]->unk8;
    w = arg0 * 25;
    x = (u32)(arg0 * 100) >> 3;
    g_BuildBuffer[0]->unk8 += PRIM_SIZE;
    if (*(s32 *)g_DrawBuffer == SCREEN_MODE_ALT) {
        *(s16 *)(g_DrawBuffer + SCREEN_X_OFFSET) = SCREEN_CENTER_X - x;
    } else {
        *(s16 *)(g_DrawBuffer + SCREEN_X_OFFSET) = -x;
    }
    *(s16 *)(g_DrawBuffer + SCREEN_Y_OFFSET) =0xC - ((u32)(arg0 * 60) >> 3);
    if (arg0 != 0) {
        if (arg1 == 0) {
            y = 0;
            x = 0;
        } else {
            x = (u32)(arg1 * 100) >> 3;
            y = (u32)(arg1 * 60) >> 3;
        }
        right = x + SCREEN_CENTER_X;
        left = right - w;
        top = y + BAR_TOP;
        bot = y + BAR_BOTTOM;
        p->x0 = left; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = left; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&g_BuildBuffer[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)g_BuildBuffer[0]->unk8;
        g_BuildBuffer[0]->unk8 += PRIM_SIZE;
        mid = bot - arg0 * 15;
        p->x0 = x; p->y0 = mid; p->x1 = left; p->y1 = mid;
        p->x2 = x; p->y2 = bot; p->x3 = left; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&g_BuildBuffer[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)g_BuildBuffer[0]->unk8;
        w = (u32)(arg0 * 9) >> 3;
        g_BuildBuffer[0]->unk8 += PRIM_SIZE;
        right = x + w;
        p->x0 = x; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = x; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&g_BuildBuffer[0]->padC[OT_OFFSET], p);
    }
    ret = ((s32 (*)(void))func_80023D7C)();
    ret = ret - (ret / 24) * arg0;
    if (D_80060884.a != ret) {
        D_80060884.a = ret;
        SetGeomScreen(ret);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016C58);
#endif

void func_80016EFC(void) {
}

s32 func_80016F04(s32 arg0) {
    if (arg0 < 0) {
        goto L22;
    }
    if (arg0 < 0x13) {
        arg0 = 0x11;
    } else if (arg0 < 0x24) {
L22:
        arg0 = 0x22;
    } else if (arg0 < 0x35) {
        arg0 = 0x33;
    }
    return arg0;
}

s32 func_80016F44(s32 arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    idx = -1;
    do { s3 = *(u8 **)((u8 *)arg0 + 0x10); } while (0);
    s2 = *(u8 **)((u8 *)arg0 + 0x20);
    n = *(s32 *)(s3 + 0x1C);
    if (arg2 != 0x6396347F) {
        for (i = 0; i < *(u32 *)(s3 + 0x28); i++) {
            e = ((s32 *)(s3 + 0x2C))[i];
            if (e & 1) {
                idx = i;
                if (arg2 == e) {
                    goto found;
                }
            } else {
                idx = i;
                if (arg2 == *(s32 *)(e + 4)) {
                    goto found;
                }
            }
        }
        return -0xFF;
    }
found:
    if (arg1 != 0) {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                func_80018474(s2, s3 + 0x2C);
            }
            s2 += 0xC;
        }
    } else {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                *(u16 *)(s2 + 6) &= 0xFDFF;
            }
            s2 += 0xC;
        }
    }
    return -0xFF;
}

#ifdef NON_MATCHING
typedef struct { s32 pad0[2]; s32 kind; } Obj170B4;
typedef struct { char pad[0x404]; s32 count; } Cnt170B4;
typedef struct { char pad[0x18]; Cnt170B4 *list; } Hdr170B4;
extern Hdr170B4 D_80067588;
extern Cnt170B4 *D_800675A0;

s32 func_800170B4(s32 arg0, s32 arg1) {
    s32 pad[2];
    s32 i;
    Obj170B4 *o;
    Hdr170B4 *h = &D_80067588;
    Hdr170B4 *new_var;

    i = 0;
    if (D_800675A0->count > 0) {
        new_var = h;
        do {
            o = *(Obj170B4 **)((char *)new_var->list + i * 8 + 0x520);
            if (!((s32)o & 3)) {
                if (o->kind != 2) {
                    if (o->kind == 3) {
                        func_80016F44((s32)o, arg1, arg0);
                    }
                } else {
                    func_800171E8(o, arg1, arg0);
                }
            }
            i++;
        } while (i < new_var->list->count);
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800170B4);
#endif

void func_80017198(s32 arg0) {
    func_80016F44(arg0, 1, 0x6396347F);
}

void func_800171C0(int a0) {
    func_80016F44(a0, 0, 0x6396347F);
}

s32 func_800171E8(void *arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    idx = -1;
    do { s3 = *(u8 **)((u8 *)arg0 + 0x10); } while (0);
    s2 = *(u8 **)((u8 *)arg0 + 0x1C);
    n = *(s32 *)(s3 + 0x34);
    if (arg2 != 0x6396347F) {
        for (i = 0; i < *(s32 *)(s3 + 0x40); i++) {
            e = ((s32 *)(s3 + 0xC))[i];
            if (e & 1) {
                idx = i;
                if (arg2 == e) {
                    goto found;
                }
            } else {
                idx = i;
                if (arg2 == *(s32 *)(e + 4)) {
                    goto found;
                }
            }
        }
        return -0xFF;
    }
found:
    if (arg1 != 0) {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                func_80018474(s2, s3 + 0xC);
            }
            s2 += 0xC;
        }
    } else {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                *(u16 *)(s2 + 6) &= 0xFDFF;
            }
            s2 += 0xC;
        }
    }
    return -0xFF;
}

void func_80017358(s32 a0) {
    func_800171E8((void *)a0, 1, 0x6396347F);
}

#ifdef NON_MATCHING
#define TYPE_A 2
#define TYPE_B 5
#define STRIDE_A 16
#define OFFSET_A 8
#define STRIDE_B 24
#define OFFSET_B 12
#define MODE_BOUNDS 1
#define MODE_VRAM 2

void func_80017380(s32 mode, u8 *base, s32 arg2, u16 *out) {
    u32 flags;
    s32 sub;
    s32 minx, miny, maxx, maxy;
    s32 t, u, w, h;
    u8 *p;

    /* p stays unassigned for other types; retail reads the caller's $s0 */
    if (base[0] == TYPE_A) {
        p = base + (arg2 >> 8) * STRIDE_A + OFFSET_A;
    } else if (base[0] == TYPE_B) {
        p = base + (arg2 >> 8) * STRIDE_B + OFFSET_B;
    }
    func_80018474(p + 4, base + 4);
    flags = *(u16 *)(p + 0xA);
    sub = (flags >> 7) & 3;
    if (mode == MODE_BOUNDS) {
        minx = p[4];
        miny = p[5];
        maxx = minx;
        maxy = miny;
        t = p[8];
        if (t < minx) minx = t;
        u = p[9];
        if (u < miny) miny = u;
        if (maxx < t) maxx = t;
        if (maxy < u) maxy = u;
        t = p[12];
        if (t < minx) minx = t;
        u = p[13];
        if (u < miny) miny = u;
        if (maxx < t) maxx = t;
        if (maxy < u) maxy = u;
        out[0] = (flags & 0xF) << 6;
        w = maxx - minx + 1;
        h = maxy - miny + 1;
        out[1] = ((*(u16 *)(p + 0xA) << 4) & 0x100) + miny;
        if (sub == mode) {
            out[2] = w / 2;
            out[0] = out[0] + (minx >> 1);
        } else if (sub < 2) {
            if (sub != 0) {
                out[3] = h;
                return;
            }
            out[0] = out[0] + minx / 4;
            out[2] = w / 4;
        } else if (sub == 2) {
            out[2] = w;
            out[0] = out[0] + minx;
        } else {
            out[3] = h;
            return;
        }
        out[3] = h;
    } else if (mode == MODE_VRAM) {
        out[0] = (*(u16 *)(p + 6) & 0x3F) * 16;
        out[1] = (*(u16 *)(p + 6) >> 6) & 0x1FF;
        if (sub == 0) {
            out[2] = 0x10;
        } else {
            out[2] = 0x100;
        }
        out[3] = 1;
    } else {
        /* other modes write nothing */
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017380);
#endif

#ifdef NON_MATCHING
#define TEXT_OFFSET 0xC
#define STRING_INDEX_SHIFT 8
#define ASCII_SPACE 0x20

void func_800175D8(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3, s32 arg4) {
    s32 skip;
    s32 i;
    s32 pos;
    u8 c;

    pos = 0;
    skip = arg1 >> STRING_INDEX_SHIFT;
    for (i = 0; i < skip; i++) {
        /* skip one NUL-terminated string */
        do {
            c = arg0[pos + TEXT_OFFSET];
            pos++;
        } while (c != 0);
    }

    i = 0;
    if (arg3 == 0) {
        return;
    }
    do {
        c = *arg2;
        if (c == 0) {
            break;
        }
        if (c < ASCII_SPACE) {
            c = ASCII_SPACE;
        }
        if (c == 0x7B) {
            c = 0x28;
        }
        if (c == 0x7D) {
            c = 0x29;
        }
        if (c == 0x7E) {
            c = 0x7B;
        }
        if (c == 0x25) {
            c = 0x7D;
        }
        if (c == 0x26) {
            c = 0x5F;
        }
        arg0[pos + i + TEXT_OFFSET] = c;
        arg2++;
        arg3--;
        i++;
    } while (arg3 != 0);

    while (arg3 != 0) {
        arg0[pos + i + TEXT_OFFSET] = ASCII_SPACE;
        arg3--;
        i++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800175D8);
#endif

#ifdef NON_MATCHING
typedef struct { s16 x, y, w, h; } Rect;

s32 func_800176DC(s32 mode, s32 a1, s32 a2, u8 *i3, u8 *i4, s32 flag) {
    Rect r;
    char buf[0x110];
    u8 *tmp = (u8 *)((s32)&buf[15] & ~0xF);
    u8 *s;
    u8 *d;
    s32 n;

    if (i3 != 0) {
        func_80017380(2, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i3 & 0xF) {
                flag = 0;
                s = i3; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i3);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i3 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i3;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i3);
            if (flag == 0) DrawSync(0);
        }
    }
    if (i4 != 0) {
        func_80017380(1, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i4 & 0xF) {
                flag = 0;
                s = i4; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i4);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i4 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i4;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i4);
            if (flag == 0) DrawSync(0);
        }
    }
    return flag;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800176DC);
#endif

void SetColorMatrix(void *);
void SetFarColor(s32, s32, s32);
void SetGeomOffset(s32, s32);
void SetGeomScreen(s32);
extern s32 D_80060144, D_80060148, D_8006014C, D_800607F8, D_800607FC, D_80060800, D_80060804, D_80060808, D_8006083C, D_80060840, D_80060844, D_80060860;
extern s16 D_80060864, D_80060866, D_80060868;
extern s32 D_80060878, D_8006087C, D_80060880;
extern Unk80060884 D_80060884;
extern s16 D_8006088E, D_80060890, D_80060892, D_80060894, D_80060896, D_80060898, D_8006089A, D_8006089C;
extern s32 D_8006CC20, D_8006CC24;

void func_8001798C(void) {
    D_8006014C = 0;
    D_80060148 = 0;
    D_80060144 = 0;
    D_80060884.a = 0x120;
    SetGeomScreen(0x120);
    SetGeomOffset(0, 0);
    D_80060868 = 0x1000;
    D_80060864 = 0;
    D_80060866 = 0;
    D_80060878 = 0;
    D_8006087C = 0;
    D_80060880 = 0;
    D_80060884.b = 0x200;
    D_80060892 = 0x200;
    D_80060898 = 0x200;
    D_8006088E = 0x200;
    D_80060894 = 0x200;
    D_8006089A = 0x200;
    D_80060890 = 0x200;
    D_80060896 = 0x200;
    D_8006089C = 0x200;
    SetFarColor(0, 0, 0);
    SetColorMatrix(&D_80060884.b);
    func_80018168((s32 *)((char *)&D_80060884 - 0x90));
    *(s32 *)((char *)&D_80060884 - 0x90) = 0;
    D_800607F8 = 0;
    D_800607FC = 0x1F400;
    D_80060800 = 0;
    D_80060804 = 0;
    D_80060808 = 0;
    D_8006083C = 0;
    D_80060840 = 0xE1000;
    D_80060844 = 0x5DC000;
    D_80060860 = 0;
    D_8006CC20 = 0;
    D_8006CC24 = 0;
}

typedef struct Unk80017AF8 {
    u16 a, b, c;
    s16 d, e, f;
    s16 g, h, i;
    s16 pad;
    s32 x, y, z;
} Unk80017AF8;

void func_80017AF8(Unk80017AF8 *dst, Unk80017AF8 *src) {
    dst->a = src->a;
    dst->b = src->b;
    dst->c = src->c;
    dst->d = -(src->d * 5) >> 3;
    dst->e = -(src->e * 5) >> 3;
    dst->f = -(src->f * 5) >> 3;
    dst->g = -src->g;
    dst->h = -src->h;
    dst->i = -src->i;
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

#ifdef NON_MATCHING
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
    s = Math_Sin(-D_80060808);
    c = Math_Cos(-D_80060808);
    view->a = c;
    view->b = -s;
    view->c = 0;
    view->d = s;
    view->e = c;
    view->f = 0;
    view->g = 0;
    view->h = 0;
    view->i = ONE_Q12;

    s = Math_Sin(-D_80060800);
    c = Math_Cos(-D_80060800);
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

    s = Math_Sin(-D_80060804);
    c = Math_Cos(-D_80060804);
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
        s = Math_Sin(TILT_ANGLE);
        c = Math_Cos(TILT_ANGLE);
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
    D_80060930 = Math_Sin(D_80060914) >> SINCOS_SHIFT;
    D_80060934 = 0;
    D_80060938 = Math_Cos(D_80060914) >> SINCOS_SHIFT;
    func_80018294(*(s32 **)((char *)D_800608CC + LIGHT_BASE_OFFSET));
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017BC4);
#endif

void func_80018168(s32 *p) {
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[6] = 0x1000;
    p[7] = 0x1000;
    p[8] = 0x1000;
}

#ifdef NON_MATCHING
void func_80018194(s32 *v, s16 *m, s32 *out) {
    out[0] = (m[0] * v[0] + m[1] * v[1] + m[2] * v[2]) >> 12;
    out[1] = (m[3] * v[0] + m[4] * v[1] + m[5] * v[2]) >> 12;
    out[2] = (m[6] * v[0] + m[7] * v[1] + m[8] * v[2]) >> 12;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018194);
#endif

#ifdef NON_MATCHING
#define ENTRY_STRIDE 0x30
#define FIRST_ENTRY_OFFSET 4
#define POS_SHIFT 8
#define ACTIVE_MASK 0xFF
#define MASK_QUERY_ID 0x27F
#define MASK_QUERY_MODE 2
#define SHAPE_FLAG_ZERO_OFFSET 1

extern s32 D_800607F4;
extern s32 D_800608D4;
extern s32 D_800608E8;

void func_80018294(s32 *arg0) {
    s32 mask;
    s32 i;
    s32 off;
    s32 *entry;
    s32 *src;
    s32 *shape;
    s32 dx;
    s32 dy;
    s32 dz;

    mask = func_800321D4(D_800608D4, MASK_QUERY_ID, -1, D_800608E8, 0, MASK_QUERY_MODE, 0);
    entry = arg0;
    off = FIRST_ENTRY_OFFSET;
    for (i = 0; i < *arg0; i++) {
        if (((mask & ACTIVE_MASK) >> i) & 1) {
            src = (s32 *)func_80014260((u8 *)arg0 + off, 1, 0);
            shape = (s32 *)src[4];
            if (shape[3] == SHAPE_FLAG_ZERO_OFFSET) {
                entry[2] = 0;
                entry[3] = 0;
                entry[4] = 0;
            } else {
                dx = shape[0] - (D_800607F4 >> POS_SHIFT);
                dy = shape[1] - ((u32)D_800607F8 >> POS_SHIFT);
                dz = shape[2] - (D_800607FC >> POS_SHIFT);
                entry[2] = dx;
                entry[3] = dy;
                entry[4] = dz;
            }
            entry[5] = (s32)shape;
            entry[8] = src[5];
            entry[6] = src[6];
            entry[7] = src[7];
            entry[11] = src[8];
            entry[9] = src[9];
            entry[12] = (s32)(shape + 11);
            entry[10] = src[10];
        }
        entry += ENTRY_STRIDE / 4;
        off += ENTRY_STRIDE;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018294);
#endif

extern s32 D_8005F390;
extern s32 D_80060944;
extern s32 D_80062A18;

void func_80041E5C(s32, Unk80063458 *, void *, s32);

void func_80018434(void) {
    func_80041E5C(D_8005F390, g_BuildBuffer[0], &D_80062A18, D_80060944);
}

#ifdef NON_MATCHING
typedef struct {
    u8 pad0;
    u8 b1;
    u16 h2;
    u8 pad4;
    u8 b5;
    u16 h6;
    u8 pad8;
    u8 b9;
    u8 padA;
    u8 bB;
} Unk80018474;

void func_80018474(void *a0, void *a1) {
    Unk80018474 *arg0 = a0;
    s32 **arg1 = a1;
    s32 idx;
    s32 v;
    u32 r;
    u32 flag;
    u32 hi;

    idx = arg0->h6 >> 10;
    if (!(arg0->h6 & 0x200)) {
        v = func_800143C0(arg1 + idx, 0);
        if (v != 0) {
            r = (u32)func_80014260((void *)v, 3, 0);
            hi = (r >> 4) & 0x6030;
            flag = r & 0x80;
            arg0->h6 = (arg0->h6 & 0xFFE3) | (r & 0x1C) | 0x200;
            arg0->b5 = (arg0->b5 & 0x7F) | flag;
            arg0->b9 = (arg0->b9 & 0x7F) | flag;
            arg0->bB = (arg0->bB & 0x7F) | flag;
            arg0->h2 = (arg0->h2 & 0x9FCF) | hi;
            arg0->b1 = (arg0->b1 & 0x7F) | flag;
        }
    }
    *(volatile s32 *)arg1[idx];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018474);
#endif

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001856C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800186D8);

s32 func_800189C4(void) {
    return SUCCESS;
}

typedef struct Node {
    s32 unk0;
    struct Node *children4; /* the child list's head when unk0 == 2 */
    u8 pad08[0x44 - 0x08];
    struct Node *parent;
    struct Node *next;
    struct Node *children; /* the child list's head otherwise */
} Node;

/* Moves node from its parent's child list to the head of parent's. Returns -0x18 when node is missing from its old
 * parent's list. The success path has no return statement: $v0 is left holding the 2 of the last comparison. */
s32 func_800189CC(Node *node, Node *parent) {
    Node *old;
    Node *it;

    old = node->parent;
    if (old != 0) {
        it = old->unk0 == 2 ? old->children4 : old->children;
        if (it == node) {
            *(old->unk0 == 2 ? &old->children4 : &old->children) = node->next;
        } else {
            while (it != 0 ? it->next != node : 1) {
                if (it == 0) {
                    return -0x18;
                }
                it = it->next;
            }
            it->next = node->next;
        }
    }
    node->parent = parent;
    node->next = parent->unk0 == 2 ? parent->children4 : parent->children;
    *(parent->unk0 == 2 ? &parent->children4 : &parent->children) = node;
}

typedef struct Unk80018AA8 {
    s32 f0;
    u8 pad[0x44];
    struct Unk80018AA8 *next;
    struct Unk80018AA8 *child;
} Unk80018AA8;

typedef s32 (*Cb80018AA8)(Unk80018AA8 *, void *);

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2) {
    s32 ret;
    Unk80018AA8 *p;
    Unk80018AA8 *nx;

    ret = arg1(arg0, arg2);
    if (ret == -0x19) {
        func_80019BBC((s32 *)arg0, 0);
        return -0x19;
    }
    p = arg0->child;
    while (p != 0 && p->f0 != 0) {
        nx = p->next;
        func_80018AA8(p, arg1, arg2);
        p = nx;
    }
    return ret;
}

s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2) {
    char *child;
    char *next;

    child = *(char **)((char *)arg0 + 0x4C);
    if (child != 0) {
        do {
            next = *(char **)(child + 0x48);
            if (func_80018B50(child, arg1, arg2) == -0x19) {
                func_80019BBC((s32 *)arg0, 0);
            }
            child = next;
        } while (child != 0);
    }
    return arg1(arg0, arg2);
}

s32 func_80018BE4(u8 *arg0, void *arg1, s32 arg2) {
    s32 r;
    u8 *c;
    u8 *next;

    r = 0;
    if (arg0 == 0) {
        return 0;
    }
    if (*(s32 *)arg0 != 2) {
        r = ((s32 (*)(u8 *, s32))arg1)(arg0, arg2);
        if (r != 0) {
            return r;
        }
    }
    if (*(s32 *)arg0 == 2) {
        c = *(u8 **)(arg0 + 4);
    } else {
        c = *(u8 **)(arg0 + 0x4C);
    }
    while (c != 0 && r == 0) {
        next = *(u8 **)(c + 0x48);
        r = func_80018BE4(c, arg1, arg2);
        c = next;
    }
    return r;
}

#ifdef NON_MATCHING
#define NODE_KIND_LIST4 2



typedef struct Unk80018CAC {
    s32 kind;
    Unk80018AA8 *children4;
    u8 pad[0x44];
    Unk80018AA8 *children;
} Unk80018CAC;

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2);

s32 func_80018CAC(Unk80018CAC *arg0, Cb80018AA8 arg1, void *arg2) {
    Unk80018AA8 *node;
    Unk80018AA8 *next;

    if (arg0->kind == NODE_KIND_LIST4) {
        node = arg0->children4;
    } else {
        node = arg0->children;
    }
    while (node != 0) {
        next = node->next;
        func_80018AA8(node, arg1, arg2);
        node = next;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018CAC);
#endif

#ifdef NON_MATCHING
s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2);

typedef struct Unk80018D28 {
    s32 kind;
    struct Unk80018D28 *first;
    char pad[0x40];
    struct Unk80018D28 *next;
    struct Unk80018D28 *head;
} Unk80018D28;

s32 func_80018D28(void *arg0, void *arg1, void *arg2) {
    int is_kind2;
    Unk80018D28 *node;
    Unk80018D28 *next;

    is_kind2 = ((Unk80018D28 *)arg0)->kind == 2;
    if (is_kind2) {
        node = ((Unk80018D28 *)arg0)->first;
    } else {
        node = ((Unk80018D28 *)arg0)->head;
    }
    while (node != 0) {
        next = node->next;
        if (func_80018B50(node, (s32 (*)(void *, s32))arg1, (s32)arg2) == -0x19) {
            func_80019BBC((s32)arg0, 0);
        }
        node = next;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018D28);
#endif

extern u8 D_8006CB18[];

void func_80018DD0(void *arg0, void *arg1, void *arg2) {
    u8 *var_s0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = D_8006CB18;
    do {
        ((void (*)(void *, void *, void *))arg0)(var_s0, arg1, arg2);
        var_s1 += 1;
        var_s0 += 8;
    } while (var_s1 < 8);
}

void *func_80018E48(void *arg0, s32 arg1) {
    if (*(s32 *)((u8 *)arg0 + 0xB8) == arg1) {
        return arg0;
    }
    return 0;
}

#ifdef NON_MATCHING
#define F(p, T, off) (*(T *)((u8 *)(p) + (off)))


s32 func_80018E68(void *arg0, void *arg1) {
    s32 sp10;
    s32 d;
    s32 sh;
    s32 new_var;
    s32 t;
    s32 st;
    s32 m;
    s32 s0;
    void *tbl;

    if (arg0 == F(arg1, void *, 4)) {
        return 0;
    }
    sh = F(F(F(arg0, void *, 0xC), void *, 0x10), s32, 4) >> 8;
    if (!((1 << sh) & F(arg1, u32, 0))) {
        return 0;
    }
    d = func_80030FEC((s32 *)((u8 *)arg0 + 0x60), (s32 *)((u8 *)F(arg1, void *, 4) + 0x60));
    if (d < F(arg1, s32, 0xC)) {
        t = F(arg1, s32, 0x10);
        if (t == 0xFF) {
            s0 = 0xFE;
        } else {
            tbl = F(arg0, void *, 0xC);
            s0 = *(u16 *)(F(tbl, u8 *, 0x1C) + (t >> 8) * 2);
            if (s0 == 0xFF) {
                if (t == 0x300) {
                    st = F(arg0, s32, 0x108);
                    new_var = F(arg0, s32, 0xB0);
                    if ((u32)(st - 3) >= 2 && st != 2 && !(new_var & 2)) {
                        s0 = 0xFE;
                        goto cmp;
                    }
                }
                if (F(arg1, s32, 0x10) == 0xA00 && !(F(arg0, s32, 0x100) & 0x800)) {
                    s0 = 0xFE;
                    goto cmp;
                }
                if (F(arg1, s32, 0x10) != 0x1D00) {
                    return 0;
                }
                if (F(arg0, s32, 0x100) & 0x02000000) {
                    return 0;
                }
                s0 = 0xFE;
                goto cmp;
            }
            if (!(s0 & 0x8000)) {
                m = F(arg0, s32, 0xB0);
                st = F(arg0, s32, 0x108);
                t = *(s32 *)(F(tbl, u8 *, 0x20) + s0 * 16);
                if ((u32)(st - 3) < 2 || st == 2) {
                    m |= 0x1002;
                }
                if (m & t) {
                    return 0;
                }
                if (!(s0 & 0x8000)) {
                    goto cmp;
                }
            }
            if (F(arg1, s32, 0x10) == 0xF00) {
                sp10 = 0x100;
                F(arg0, void *, 0x5C) = F(arg1, void *, 4);
                func_8001D22C(arg0, (u16)s0, 1, &sp10);
                if (F(F(arg1, void *, 4), s32, 0xD8) == 0) {
                    return 0;
                }
            }
        }
cmp:
        if (s0 != 0xFF) {
            F(arg1, void *, 8) = arg0;
            F(arg1, s32, 0xC) = d;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018E68);
#endif

typedef struct { char pad[0x100]; s32 flags; } T;

T *func_80019074(T *a, s32 m) {
    if (a->flags & m) {
        return a;
    }
    return 0;
}
