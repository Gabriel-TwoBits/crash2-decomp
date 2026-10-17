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

extern s16 D_8005F2EC;
extern s16 D_8005F2F8;
extern s32 D_8005F39C;
extern s16 D_8005F3E4;
extern Unk80033DE0 D_8005FCD0[];

void func_80056760(s16, s16, s16, s16);

#define SEQ_MODE_ACTIVE 3
#define VOLUME_SHIFT 7

void func_80033C84(s32 arg0) {
    s32 i;
    s32 vol;

    D_8005F2EC = arg0;
    if (D_8005F39C == SEQ_MODE_ACTIVE) {
        vol = ((s16)arg0) >> VOLUME_SHIFT;
        for (i = 0; i < D_8005F2F8; i++) {
            func_80056760(D_8005F3E4, D_8005FCD0[i].seq, vol, vol);
        }
    } else {
        /* other modes: only the store above */
    }
}
