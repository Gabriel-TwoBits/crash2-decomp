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

typedef struct {
    s32 pad0;
    s32 unk4;
    s32 pad8;
    s32 padC;
    s32 *unk10;
    s32 pad14;
    s32 unk18;
} Unk80033EE0;

void *Nsf_OpenEntry(void *, s32, s32);
s32 Nsf_CloseEntry(void *, s32);
s32 Snd_ControlSequence(Unk80033DE0 *, s32, s32);
void SsSepClose(s16);
s16 SsSepOpen(s32, s16, s16);
void SsStart(void);
s16 SsVabFakeBody(s16);
s16 SsVabFakeHead(s32, s16, s32);
void exit(s32);
void func_800343CC(Unk80033DE0 *, s32, s32, s32);
void func_800528C4(s16, s16, s32, s32);
extern Unk80033DE0 D_8005FCD0[];
s32 D_8005F254 = 0;
s32 D_8005F260 = 0;
u16 D_8005F2EC;
s32 D_8005F2F4;
s16 D_8005F2F8;
s32 D_8005F308;
s32 D_8005F39C;
s16 D_8005F3E0;
s16 g_SndSepAccess;
extern s32 D_80067FB8;

#define NONE 0x6396347F

s32 Snd_LoadSequence(s32 addr) {
    s32 *arg0 = (s32 *)addr;
    s32 tmp;
    s32 unused;
    s32 fresh;
    Unk80033EE0 *hdr;
    s32 i;
    s32 v;
    s32 vab;

    fresh = D_8005F3E0 == -1;
    if (D_8005F308 != 0) {
        return -1;
    }
    if (D_8005F39C == 1) {
        if (D_8005F260++ < 3) {
            return 0;
        }
        D_8005F260 = 0;
        if (D_8005F2F4 != NONE) {
            SsSepClose(g_SndSepAccess);
            tmp = D_8005F2F4;
            Nsf_CloseEntry(&tmp, 1);
        }
        if (*arg0 != NONE) {
            hdr = Nsf_OpenEntry(arg0, 1, 1);
            i = 0;
            v = hdr->unk4;
            D_8005F2F4 = v;
            for (; i < 7; i++) {
                if (hdr->unk10[i + 2] != NONE) {
                    Nsf_OpenEntry(&hdr->unk10[i + 2], 0, 1);
                }
            }
        } else {
            D_8005F2F4 = *arg0;
            return 0;
        }
    }
    if (*arg0 == NONE) {
        D_8005F2F4 = *arg0;
        D_8005F39C = 0;
        return 0;
    }
    D_8005F39C = 2;
    hdr = Nsf_OpenEntry(arg0, 1, 0);
    D_8005F254 = 0;
    for (i = 0; i < 7; i++) {
        if (hdr->unk10[i + 2] != NONE) {
            if (Nsf_CloseEntry(&hdr->unk10[i + 2], 0) == 0) {
                return 0;
            }
        }
    }
    D_8005F2F8 = hdr->unk10[0];
    vab = D_8005F3E0;
    D_8005F39C = 3;
    if (vab == -1) {
        D_8005F3E0 = SsVabFakeHead(((s32 *)Nsf_OpenEntry(&hdr->unk10[1], 1, 1))[5], -1, D_80067FB8);
        if (D_8005F3E0 == vab) {
            exit(-1);
        }
        D_8005F3E0 = SsVabFakeBody(D_8005F3E0);
        if (D_8005F3E0 == vab) {
            exit(-1);
        }
        SsStart();
    }
    g_SndSepAccess = SsSepOpen(hdr->unk18, D_8005F3E0, D_8005F2F8);
    if (g_SndSepAccess == -1) {
        exit(-1);
    }
    for (i = 0; i < D_8005F2F8; i++) {
        if (fresh) {
            v = (s16)D_8005F2EC >> 7;
            func_800343CC(&D_8005FCD0[i], (v << 16) | v, i, unused);
        } else {
            func_800343CC(&D_8005FCD0[i], 0, i, unused);
            v = (s16)D_8005F2EC >> 7;
            func_800528C4(g_SndSepAccess, D_8005FCD0[i].seq, v, 0x1E);
        }
    }
    Snd_ControlSequence(D_8005FCD0, 0, 0);
    return 0;
}
