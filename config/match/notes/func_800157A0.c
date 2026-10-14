#include "common.h"
#include "sdk.h"
#include "m2c_macros.h"

M2C_UNK CdControl(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK CloseEvent(s32);                            /* extern */
M2C_UNK DisableEvent(s32);                          /* extern */
M2C_UNK EnterCriticalSection();                     /* extern */
M2C_UNK ExitCriticalSection();                      /* extern */
M2C_UNK ResetGraph(M2C_UNK);                        /* extern */
M2C_UNK StopCallback();                             /* extern */
M2C_UNK StopPAD();                                  /* extern */
M2C_UNK StopRCnt(M2C_UNK);                          /* extern */
M2C_UNK func_80036304();                            /* extern */
extern s32 D_8005F1A4;
s32 D_8005F3DC = 1;

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
