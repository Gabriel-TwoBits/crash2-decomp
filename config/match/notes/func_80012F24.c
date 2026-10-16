#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 CdControlF(s32, u8 *);
extern void *CdIntToPos(s32, u8 *);
extern s32 CdRead(s32, s32, s32);
extern s32 CdSync(s32, s32);
extern u8 *D_800675A0;
extern s32 D_800675A8;

typedef struct {
    s32 unk0;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s32 unkC;
    s32 unk10;
    s32 unk14;
} Unk80012F24;

s32 func_80012F24(Unk80012F24 *arg0, s32 arg1) {
    u8 sp10[16];
    s32 s0;
    s32 s1;
    s32 s2;
    s32 v0;
    s32 v1;
    s32 a3 = arg0->unk14;

    if (arg0->unkA != 0) {
        s16 t = arg0->unk8;
        s0 = D_800675A8 + ((*((u32 *)((D_800675A0 + ((a3 >> 1) * 4)) + 0x420))) >> 6);
        s2 = (arg1 << 5) - t;
        s1 = arg0->unk0 + (t << 11);
    } else {
        s2 = arg1 << 5;
        v0 = (D_800675A8 + (*((s32 *)(D_800675A0 + 0x418)))) + ((a3 >> 1) << 5);
        s1 = arg0->unk0;
        s0 = v0;
    }
loop:
    do {
        CdIntToPos(s0, sp10);
        if (CdControlF(2, sp10) == 0) {
            goto loop;
        }
        v1 = CdSync(1, 0);
        v0 = 5;
        if (v1 == 0) {
            v0 = CdSync(0, 0);
            v1 = 5;
        }
        if (v0 == v1) {
            goto loop;
        }
    } while (CdRead(s2, s1, 0x80) == 0);
    return -0xFF;
}
