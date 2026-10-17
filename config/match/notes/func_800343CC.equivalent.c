#include "common.h"
#include "sdk.h"
#include "game.h"

#define SOUND_REVERB_THRESHOLD 2
#define SEQ_FORCE_FLAT_VOLUME 1
#define SEQ_NO_NEXT (-1)
#define HALF_SHIFT 16

typedef struct Unk800343CC {
    s16 unk_0;
    s16 pad2;
    s32 unk_4;
    s32 pad8[2];
    s16 unk_10;
    s16 unk_12;
} Unk800343CC;

extern s16 D_8005F2F8;
extern s32 D_8005F308;
extern s16 D_8005F3E4;
void func_80056760(s16, s16, s16, s16);

void func_800343CC(Unk800343CC *arg0, s32 arg1, s32 arg2) {
    if (D_8005F2F8 >= SOUND_REVERB_THRESHOLD && arg2 == SEQ_FORCE_FLAT_VOLUME) {
        arg0->unk_12 = D_8005F308;
        arg0->unk_10 = D_8005F308;
    } else {
        arg0->unk_10 = arg1 >> HALF_SHIFT;
        arg0->unk_12 = arg1;
    }
    arg0->unk_0 = arg2;
    arg0->unk_4 = SEQ_NO_NEXT;
    func_80056760(D_8005F3E4, arg2, arg0->unk_10, arg0->unk_12);
}
