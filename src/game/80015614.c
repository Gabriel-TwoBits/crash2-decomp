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

INCLUDE_ASM("asm/nonmatchings/game/80015614", func_80015828);
