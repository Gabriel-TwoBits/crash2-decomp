#include "common.h"
#include "sdk.h"
#include "game.h"

#define TEXT_CMD '~'
#define TEXT_NEWLINE 0xA
#define TEXT_END '&'
#define DEFAULT_ADVANCE 0x190
#define BOUNDS_INIT 0xF423F
#define BOUNDS_FLAG 0x04000000
#define BOUNDS_MARGIN 0x64
#define BOUNDS_BASELINE 0xA
#define GLYPH_STRIDE 0x14
#define GLYPH_BASE_BIAS 0x278
#define PALETTE_BASE 0x2E
#define WORD_SIZE 4
#define OPT_BUF_SIZE 8
#define POS_X 0
#define POS_Y_TOP 1
#define POS_X_RIGHT 3
#define POS_Y_BOTTOM 4
#define POS_COUNT 5
#define BOUNDS_COUNT 5

#define FIELD(p, T, o) (*(T *)((u8 *)(p) + (o)))

s32 atoi(u8 *, s32);
void func_8001856C(s32, s32, void *, s32 *, s32);
void func_800186D8(void *, void *, s32, void *, s32 *, s32, s32);

/* Reads a "~"-terminated token into buf and returns its value. */
static s32 read_token(u8 **p, u8 *buf) {
    s32 n;
    u8 c;

    n = 0;
    c = *(*p)++;
    while (c != TEXT_CMD) {
        buf[n++] = c;
        c = *(*p)++;
    }
    buf[n] = 0;
    return atoi(buf, n);
}

s32 func_8001B83C(void *arg0, char *arg1, s32 arg2, s32 arg3, void *arg4, s32 arg5, s32 arg6) {
    s32 pos[POS_COUNT];
    s32 bounds[BOUNDS_COUNT];
    u8 buf_s[OPT_BUF_SIZE];
    u8 buf_x[OPT_BUF_SIZE];
    u8 *s;
    u8 *font;
    u8 *glyph;
    u8 *palette;
    s32 right;
    s32 bottom;
    s32 top;
    s32 adv_x;
    s32 adv_y;
    s32 y;
    s32 max_x;
    s32 has_bounds;
    s32 flag_c;
    u8 c;
    u8 kind;

    font = (u8 *)arg3;
    s = (u8 *)arg1;
    flag_c = 1;
    y = 0;
    max_x = 0;
    has_bounds = 0;
    if (arg6 != 0) {
        has_bounds = FIELD(arg0, s32, 0xAC) & BOUNDS_FLAG;
        if (has_bounds != 0) {
            bounds[0] = BOUNDS_INIT;
            bounds[1] = BOUNDS_INIT;
            bounds[3] = -BOUNDS_INIT;
            bounds[4] = -BOUNDS_INIT;
        }
    }
    pos[POS_X] = arg2;
    adv_x = DEFAULT_ADVANCE;
    while (*s != 0) {
        adv_y = DEFAULT_ADVANCE;
        for (;;) {
            if (*s == TEXT_CMD) {
                s++;
                c = *s++;
                switch (c) {
                case 'p':
                    palette = FIELD(arg0, u8 *, 0xBC) - (*s - PALETTE_BASE) * WORD_SIZE;
                    s++;
                    if (*(s32 *)palette == 1) {
                        s++;
                    }
                    if (*s == 0) {
                        goto done;
                    }
                    continue;
                case 'c':
                    kind = *s++;
                    flag_c = kind == '1';
                    continue;
                case 's':
                    kind = *s++;
                    if (kind == 'x') {
                        adv_x = read_token(&s, buf_s);
                    } else {
                        adv_y = read_token(&s, buf_s);
                    }
                    continue;
                case 'x':
                    pos[POS_X] += read_token(&s, buf_x);
                    continue;
                case 'y':
                    y += read_token(&s, buf_x);
                    continue;
                case 'l':
                    if (*s == 0) {
                        goto done;
                    }
                    y -= FIELD(font, s16, 0x1A) << arg5;
                    continue;
                case '%':
                case 'n':
                    if (*s == 0) {
                        goto done;
                    }
                    pos[POS_X] = arg2;
                    y -= FIELD(font, s16, 0x1A) << arg5;
                    continue;
                default:
                    break;
                }
            }
            break;
        }
        c = *s;
        if (c == TEXT_NEWLINE) {
            pos[POS_X] = arg2;
            y -= FIELD(font, s16, 0x1A) << arg5;
        } else if (c == TEXT_END) {
            goto done;
        } else {
            right = pos[POS_X] + (adv_x << arg5);
            glyph = font + c * GLYPH_STRIDE - GLYPH_BASE_BIAS;
            pos[POS_X_RIGHT] = right;
            bottom = y + (FIELD(glyph, s16, 0x12) << arg5);
            top = bottom - (adv_y << arg5);
            pos[POS_Y_BOTTOM] = bottom;
            pos[POS_Y_TOP] = top;
            if (max_x < right) {
                max_x = right;
            }
            if (arg6 != 0 && FIELD(glyph, s32, 0) != 0) {
                if (has_bounds != 0) {
                    if (pos[POS_X] < bounds[0]) {
                        bounds[0] = pos[POS_X];
                    }
                    if (top < bounds[1]) {
                        bounds[1] = top;
                    }
                    if (bounds[3] < right) {
                        bounds[3] = right;
                    }
                    if (bounds[4] < bottom) {
                        bounds[4] = bottom;
                    }
                }
                func_80018474(glyph + 4, font + 4);
                func_800186D8(arg0, glyph, FIELD(arg0, s32, 0x118), arg4, pos, arg6, flag_c);
            }
            pos[POS_X] += FIELD(glyph, s16, 0x10) << arg5;
        }
        s++;
        adv_x = DEFAULT_ADVANCE;
    }
done:
    if (has_bounds != 0 && arg6 != 0 && FIELD(font, s32, 0x80) != 0) {
        bounds[0] -= BOUNDS_MARGIN;
        bounds[3] += BOUNDS_MARGIN;
        func_80018474(font + 0x84, font + 4);
        func_8001856C((s32)(font + 0x80), FIELD(arg0, s32, 0x118) - BOUNDS_BASELINE, arg4, bounds, arg6);
    }
    return max_x;
}
