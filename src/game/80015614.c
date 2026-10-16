#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80015614", func_80015614);

extern s32 D_8005F1A4;
s32 D_8005F3DC;

s32 func_800157A0(void) {
    if (D_8005F1A4 != 0) {
        CdControl(8, 0, 0);
        StopCallback();
    }
    func_80036304();
    StopPAD();
    ResetGraph(3);
    EnterCriticalSection();
    StopRCnt(0xF2000002);
    DisableEvent(D_8005F3DC);
    CloseEvent(D_8005F3DC);
    ExitCriticalSection();
    return -0xFF;
}

#ifdef NON_MATCHING
typedef struct { s32 w[19]; } Slot;
extern Slot D_8006991C;
extern char D_80069944[];
extern int InitPAD(void *, int, void *, int);
extern void StartPAD(void);
extern void ChangeClearPAD(int);

void func_80015828(s32 arg0) {
    Slot *p;
    char *base;
    s32 i;
    s32 off;

    i = 0;
    D_8006991C.w[0] = arg0;
    if (arg0 > 0) {
        p = &D_8006991C;
        base = (char *)p;
        off = 0;
        do {
            p->w[14] = 0;
            p->w[12] = 0;
            p->w[11] = 0;
            *(s32 *)(D_80069944 + off) = 0;
            if (i == 0) {
                InitPAD(base + 4, 0x22, base + 0x50, 0x22);
                StartPAD();
                ChangeClearPAD(0);
            }
            p++;
            i++;
            off += 0x4C;
        } while (i < arg0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80015614", func_80015828);
#endif
