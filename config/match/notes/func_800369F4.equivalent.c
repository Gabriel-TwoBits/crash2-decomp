#include "common.h"
#include "sdk.h"
#include "game.h"

#define OFF_STATE 0x6F4
#define OFF_SAVED 0x718
#define OFF_ARG 0x954
#define STATE_SIZE 0x2A4
#define STATE_IDLE 8
#define RESULT_OK 0
#define RESULT_MISMATCH 1
#define RESULT_RESET 2

typedef void (*Func80036A84)(s32 *, s32, s32, s32);

s32 func_80036A8C(s32 *, s32);

s32 func_800369F4(void) {
    s32 *base = D_8005F264;
    s32 *state = base + OFF_STATE / 4;
    s32 saved;
    s32 ret;

    ((Func80036A84)func_80036A84)(state, STATE_SIZE, base[OFF_ARG / 4], 0);
    saved = base[OFF_SAVED / 4];
    base[OFF_SAVED / 4] = 0;
    if (saved == func_80036A8C(state, STATE_SIZE)) {
        if (base[OFF_STATE / 4] == STATE_IDLE) {
            return RESULT_OK;
        } else {
            ret = RESULT_RESET;
        }
    } else {
        ret = RESULT_MISMATCH;
    }
    func_800368C0();
    return ret;
}
