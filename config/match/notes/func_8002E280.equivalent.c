#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIELD(base, type, off) (*(type *)((u8 *)(base) + (off)))

#define SEARCH_RANGE 0x98
#define SLOT_COUNT 0x20
#define CENTER_COORD 0x10
#define DEPTH_LIMIT 0x100
#define FLAG_BLOCKED 0x100000
#define FLAG_RETRY 0x8000
#define CTX_FLAGS_OFFSET 0x29C
#define OBJ_NO_SEARCH 0x10000000
#define STATE_PTR_OFFSET 0x58
#define STATE_KIND_VALUE 0x22
#define EMPTY_SLOT -1

extern s8 D_8005B520[];
extern u8 D_80067FE4[];
extern void *D_800608CC;
extern s32 *D_8005F3C4;
s32 func_8002DFA8(s32, s32, s32 *, void *);

/* Nonzero when arg5's state object holds the special kind. */
static s32 has_special_state(void *obj) {
    void *state = FIELD(obj, void *, STATE_PTR_OFFSET);

    if (state == 0) {
        return 0;
    } else if (*FIELD(FIELD(state, void *, 0xC), s32 *, 0x10) == STATE_KIND_VALUE) {
        return 1;
    } else {
        return 0;
    }
}

s32 func_8002E280(s32 *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 *arg4, void *arg5, s32 arg6) {
    s32 x = arg1;
    s32 y = arg2;
    s32 depth = arg6;
    s32 i;
    s32 dx;
    s32 dy;
    s32 px;
    s32 py;
    s32 ny;
    s32 *slot;
    s8 *step;

    for (;;) {
        step = D_8005B520;
        if (func_8002DF68(x, y) != 0
            || (FIELD(FIELD(D_800608CC, void *, 0x10), s32, CTX_FLAGS_OFFSET) & FLAG_BLOCKED)
            || (FIELD(arg5, s32, 0x100) & OBJ_NO_SEARCH)) {
            *arg3 = x;
            *arg4 = y;
            return depth + 2;
        }

        for (i = 0; i < SEARCH_RANGE; i++) {
            dx = step[0];
            dy = step[1];
            step += 2;

            px = x + dx;
            py = y + dy;
            if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
            ny = y - dy;
            if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
            px = x - dx;
            if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
            if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }

            if (dx != dy) {
                px = x + dy;
                py = y + dx;
                if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
                ny = y - dx;
                if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
                px = x - dy;
                if (func_8002DF68(px, py) != 0) { *arg3 = px; *arg4 = py; return depth + 2; }
                if (func_8002DF68(px, ny) != 0) { *arg3 = px; *arg4 = ny; return depth + 2; }
            }
        }

        /* Nothing found: retry from the centre when any slot is in use. */
        if (!(x == CENTER_COORD && y == x) && !has_special_state(arg5)) {
            slot = D_8005F3C4;
            for (i = 0; i < SLOT_COUNT; i++) {
                if (slot[i] != EMPTY_SLOT) {
                    break;
                }
            }
            if (i < SLOT_COUNT) {
                x = CENTER_COORD;
                y = CENTER_COORD;
                depth++;
                continue;
            }
        }

        if (depth < DEPTH_LIMIT && !has_special_state(arg5)) {
            FIELD(arg5, s32, 0xA8) |= FLAG_RETRY;
            if (func_8002DFA8(0, 0, arg0, arg5) != 0) {
                func_8002DFA8(1, 1, arg0, arg5);
                func_8002D6E4(arg0, arg0, arg5, D_80067FE4, 0);
            }
            return func_8002E280(arg0, x, y, arg3, arg4, arg5, DEPTH_LIMIT);
        }

        FIELD(arg5, s32, 0xA8) |= FLAG_BLOCKED;
        *arg3 = CENTER_COORD;
        *arg4 = CENTER_COORD;
        return 0;
    }
}
