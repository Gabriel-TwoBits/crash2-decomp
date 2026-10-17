#include "common.h"
#include "sdk.h"
#include "game.h"

#define CMD_SET_LEVEL 0x0
#define CMD_CHANGE_LEVEL 0x100
#define CMD_FLAGS_SET 0x200
#define CMD_FLAGS_CLEAR 0x300
#define CMD_SET_FLAG_B99B 0x400
#define CMD_SET_PAIR 0x500
#define CMD_SET_SLOT 0x600
#define CMD_START_TIMER 0x700
#define CMD_STOP_TIMER 0x800
#define CMD_NOP 0x900
#define ARG_SHIFT 8
#define ENTRY_STRIDE_WORDS 3

extern char D_80010400[];
extern s8 D_8005B99B;
extern s32 D_8005B9E8[];
extern s32 D_8005B9F0[];
extern s32 D_8005BA00;
extern s32 D_8005BA04;
extern s32 D_800608FC;

void func_800260F4(Unk8005BA58 *arg0) {
    s32 *cmd;
    s32 *p;
    s32 *q;
    s32 i;

    cmd = (s32 *)arg0;
    switch (cmd[0]) {
    case CMD_SET_LEVEL:
        func_80026334(cmd[1] >> ARG_SHIFT);
        break;
    case CMD_CHANGE_LEVEL:
        if (D_800608FC != (cmd[1] >> ARG_SHIFT)) {
            func_80022224(cmd[1] >> ARG_SHIFT);
            func_80023154();
        }
        break;
    case CMD_FLAGS_SET:
        D_8005B9E4 |= cmd[1] >> ARG_SHIFT;
        break;
    case CMD_FLAGS_CLEAR:
        D_8005B9E4 &= ~(cmd[1] >> ARG_SHIFT);
        break;
    case CMD_SET_FLAG_B99B:
        D_8005B99B = 1;
        break;
    case CMD_SET_PAIR:
        p = D_8005B9E8 + cmd[1] * ENTRY_STRIDE_WORDS;
        p[0] = cmd[2];
        p[1] = cmd[3];
        break;
    case CMD_SET_SLOT:
        i = cmd[1] * ENTRY_STRIDE_WORDS;
        q = (s32 *)&D_8005B9F4 + i;
        if (cmd[2] != 0) {
            q[2] = 1;
            D_8005B9F0[i] = cmd[3];
            q[1] = 0;
        } else {
            q[2] = -1;
            q[1] = 0;
        }
        break;
    case CMD_START_TIMER:
        D_8005BA00 = cmd[1];
        D_8005BA10 = 1;
        D_8005BA0C = 0;
        D_8005BA04 = cmd[2];
        break;
    case CMD_STOP_TIMER:
        D_8005BA10 = -1;
        D_8005BA0C = 0;
        break;
    case CMD_NOP:
        break;
    default:
        printf(D_80010400, cmd[0]);
        break;
    }
}
