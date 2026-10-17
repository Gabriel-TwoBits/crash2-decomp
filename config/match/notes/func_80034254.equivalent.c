#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATE_IDLE 0
#define STATE_ACTIVE 1
#define STATE_PENDING 3
#define STATE_WAITING 4
#define WAIT_FRAMES 30
#define LOW_BIT_MASK 1
#define U16_TO_S32_SHIFT 16
#define FIELD_SHIFT 23
#define NO_ENTRY -1

void func_80033EE0(s32);
void func_80052B4C(s16, s16, s32, s32);
typedef struct Unk80033DE0 {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk80033DE0;

extern u16 D_8005F2EC;
extern s16 D_8005F250;
extern s32 D_8005F254;
extern s32 D_8005F308;
extern s32 D_8005F2F4;
extern s32 D_8005F39C;
extern s16 D_8005F3E4;
extern s16 D_8005F3E0;
extern Unk80033DE0 D_8005FCD0[];

void func_80034254(s32 arg0) {
    s32 word;
    s32 key;

    if (D_8005F39C == STATE_IDLE) {
        return;
    }
    if (D_8005F39C == STATE_WAITING) {
        if (D_8005F250 == 0) {
            D_8005F39C = STATE_ACTIVE;
        } else {
            D_8005F250 = D_8005F250 - 1;
            if (D_8005F250 == 0) {
                D_8005F39C = STATE_ACTIVE;
            } else {
                /* still waiting */
            }
        }
    } else {
        /* state unchanged */
    }
    if (D_8005F39C == STATE_PENDING) {
        word = *(s32 *)arg0;
        if (word & LOW_BIT_MASK) {
            key = word;
        } else {
            key = *(s32 *)(word + 4);
        }
        if (D_8005F2F4 != key && D_8005F308 == 0 && D_8005F3E0 != NO_ENTRY && D_8005F254 == 0) {
            func_80052B4C(D_8005F3E4, D_8005FCD0->seq,
                          (s32)(D_8005F2EC << U16_TO_S32_SHIFT) >> FIELD_SHIFT, WAIT_FRAMES);
            D_8005F250 = WAIT_FRAMES;
            D_8005F254 = arg0;
            D_8005F39C = STATE_WAITING;
        } else {
            /* nothing to start */
        }
    } else if (D_8005F39C < STATE_PENDING && D_8005F39C > 0) {
        func_80033EE0(arg0);
    } else {
        /* no per-frame update in this state */
    }
}
