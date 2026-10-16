#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;
s32 D_8005F3BC;
s32 D_8005F3C0;
s32 D_8005F3C8;
s32 D_8005F3CC;

void EnableEvent(s32);
void InitCARD(s32);
s32 OpenEvent(s32, s32, s32, s32);
void StartCARD(void);
void _bu_init(void);

void func_80036164(void) {
    EnterCriticalSection();
    D_8005F3A0 = OpenEvent(0xF4000001, 4, 0x2000, 0);
    D_8005F3A4 = OpenEvent(0xF4000001, 0x8000, 0x2000, 0);
    D_8005F3A8 = OpenEvent(0xF4000001, 0x100, 0x2000, 0);
    D_8005F3AC = OpenEvent(0xF4000001, 0x2000, 0x2000, 0);
    D_8005F3BC = OpenEvent(0xF0000011, 4, 0x2000, 0);
    D_8005F3C0 = OpenEvent(0xF0000011, 0x8000, 0x2000, 0);
    D_8005F3C8 = OpenEvent(0xF0000011, 0x100, 0x2000, 0);
    D_8005F3CC = OpenEvent(0xF0000011, 0x2000, 0x2000, 0);
    ExitCriticalSection();
    VSync(5);
    InitCARD(1);
    VSync(5);
    StartCARD();
    VSync(5);
    _bu_init();
    VSync(5);
    EnableEvent(D_8005F3A0);
    EnableEvent(D_8005F3A4);
    EnableEvent(D_8005F3A8);
    EnableEvent(D_8005F3AC);
    EnableEvent(D_8005F3BC);
    EnableEvent(D_8005F3C0);
    EnableEvent(D_8005F3C8);
    EnableEvent(D_8005F3CC);
}
