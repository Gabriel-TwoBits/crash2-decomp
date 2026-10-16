#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk800344B0;

s16 D_8005F2F8;
s16 D_8005F3E4;
void SsSepPlay(s16, s16, s16, s16);
void SsSepPause(s16, s16);
void SsSepReplay(s16, s16);
void func_80056304(s16, s16);

void func_800344B0(Unk800344B0 *arg0, u32 cmd, s32 vol) {
    s32 i;

    if (arg0 != 0 && cmd < 5) {
        switch (cmd) {
        case 0:
            arg0->state = 1;
            SsSepPlay(D_8005F3E4, arg0->seq, 1, vol);
            break;
        case 1:
            arg0->state = 2;
            func_80056304(D_8005F3E4, arg0->seq);
            break;
        case 2:
            arg0->state = 0;
            SsSepPause(D_8005F3E4, arg0->seq);
            break;
        case 3:
            arg0->state = 1;
            SsSepReplay(D_8005F3E4, arg0->seq);
            break;
        case 4:
            arg0->volL = vol >> 16;
            arg0->volR = vol;
            for (i = 0; i < D_8005F2F8; i++) {
                func_80056760(D_8005F3E4, arg0->seq, arg0->volL, arg0->volR);
            }
            break;
        }
    }
}
