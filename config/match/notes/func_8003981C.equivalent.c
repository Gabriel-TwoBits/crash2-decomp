#include "common.h"
#include "sdk.h"
#include "game.h"

#define CTX_REGS_OFFSET 0x40
#define CTX_SP_OFFSET 0xBC
#define CTX_PC_OFFSET 0xC0
#define REG_SP 0x1F
#define ERR_NOT_READY -0xE
#define ERR_UNHANDLED -0xFF
#define ERR_CODE_LIMIT -0xFE
#define STATUS_READY 0x8
#define STATUS_TAKEN 0x20
#define OPCODE_SKIP 0x3D
#define CODE_CALL 0xFF
#define COND_ALWAYS 0
#define COND_VALUE 1
#define COND_ZERO 2
#define ACTION_ADJUST 0
#define ACTION_POP_CALL 1
#define ACTION_CALL 2

s32 func_8003981C(u8 *arg0, u32 arg1, s32 arg2, s32 *arg3) {
    s32 action = (arg1 >> 0x16) & 3;
    s32 mode = (arg1 >> 0x14) & 3;
    s32 cond;
    u32 idx;
    s32 *p;
    s32 v;
    s32 n;
    s32 *sp;

    *(s32 *)0x1F800048 = arg2;
    if (!(arg2 & STATUS_READY)) {
        return ERR_NOT_READY;
    }
    switch (mode) {
    case COND_ALWAYS:
        cond = 1;
        break;
    case COND_VALUE:
    case COND_ZERO:
        idx = (arg1 >> 0xE) & 0x3F;
        if (idx == REG_SP) {
            p = *(s32 **)(arg0 + CTX_SP_OFFSET) - 1;
            *(s32 **)(arg0 + CTX_SP_OFFSET) = p;
        } else {
            p = (s32 *)(arg0 + idx * 4 + CTX_REGS_OFFSET);
        }
        if (mode == COND_VALUE) {
            cond = *p;
        } else {
            cond = *p == 0;
        }
        break;
    default:
        /* mode 3 leaves the condition register unset; retail tests the caller's $t1, taken as nonzero */
        cond = 1;
        break;
    }
    arg2 |= STATUS_TAKEN;
    if (cond != 0) {
        arg3[1] = (arg1 >> 0x18) != OPCODE_SKIP;
        *(s32 *)0x1F800048 = arg2;
        if (action == ACTION_POP_CALL) {
            n = (arg1 >> 0xA) & 0xF;
            for (v = n - 1; v >= 0; v--) {
                sp = *(s32 **)(arg0 + CTX_SP_OFFSET);
                *(s32 **)(arg0 + CTX_SP_OFFSET) = sp - 1;
                arg3[v + 3] = sp[-1];
            }
            arg3[2] = n;
            v = func_8001C688(arg0, 0);
            if (v < ERR_CODE_LIMIT) {
                arg3[0] = arg1 & 0x3FF;
                return ERR_UNHANDLED;
            }
            return v;
        } else if (action == ACTION_CALL) {
            v = func_8001C688(arg0, 0);
            if (v < ERR_CODE_LIMIT) {
                arg3[0] = CODE_CALL;
                return ERR_UNHANDLED;
            }
            return v;
        } else {
            return 0;
        }
    }
    if (action == ACTION_ADJUST) {
        *(s32 *)(arg0 + CTX_PC_OFFSET) += (s32)(arg1 << 0x16) >> 0x14;
        *(s32 *)(arg0 + CTX_SP_OFFSET) -= (arg1 >> 8) & 0x3C;
    }
    return 0;
}
