#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80033DE0 {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk80033DE0;

void func_80056760(s16, s16, s16, s16);
s16 D_8005F2F8;
s16 D_8005F3E4;
void SsSepPause(s16, s16);
void SsSepPlay(s16, s16, s32, s16);
void SsSepReplay(s16, s16);
void func_80056304(s16, s16);

s32 func_800344B0(Unk80033DE0 *obj, s32 cmd, s32 vol) {
    s32 i;
    s32 pad[2];

    if (obj != 0) {
        switch (cmd) {
        case 0:
            obj->state = 1;
            SsSepPlay(D_8005F3E4, obj->seq, 1, (s16)vol);
            break;
        case 1:
            obj->state = 2;
            func_80056304(D_8005F3E4, obj->seq);
            break;
        case 2:
            obj->state = 0;
            SsSepPause(D_8005F3E4, obj->seq);
            break;
        case 3:
            obj->state = 1;
            SsSepReplay(D_8005F3E4, obj->seq);
            break;
        case 4:
            i = 0;
            obj->volL = vol >> 16;
            obj->volR = vol;
            if (D_8005F2F8 > 0) {
                do {
                    i++;
                    func_80056760(D_8005F3E4, obj->seq, obj->volL, obj->volR);
                } while (i < D_8005F2F8);
            }
            break;
        default:
            break;
        }
    }
}
