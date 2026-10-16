#include "common.h"
#include "sdk.h"
#define func_80015614 zz_unused
#include "game.h"
#undef func_80015614

extern s32 D_8005F1A4;
s32 D_8005F3DC;
extern s32 D_8006015C;
extern char D_80062D84[];
extern s32 D_800675AC;
extern s32 D_8006CD2C;
extern void func_8003BE88(void);

void CdInit(void);
void CdSetDebug(s32);
void EnableEvent(s32);
s32 GetGraphType(void);
void InitGeom(void);
s32 OpenEvent(u32, s32, s32, void (*)(void));
void ResetCallback(void);
void SetRCnt(u32, s32, s32);
void SetVideoMode(s32);
void StartRCnt(u32);
void func_80015828(s32);
void func_8003155C(void);
void func_800316EC(void);
void func_80036164(void);

s32 func_80015614(void) {
    s32 i;
    s32 t;
    s8 *p;
    s32 *q;

    i = 0x3FF;
    p = (s8 *)0x1F8003FF;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    SetVideoMode(0);
    InitGeom();
    if (D_8005F1A4 != 0) {
        ResetCallback();
        CdInit();
        CdSetDebug(1);
    }
    func_800160B0();
    func_8003155C();
    D_800675AC = 0;
    q = &D_8006015C;
    i = 3;
    do {
        *q = 0x6396347F;
        i--;
        q -= 1;
    } while (i >= 0);
    i = 0x294;
    do {
        *(s32 *)(D_80062D84 + i) = 0x6396347F;
        i -= 0x2C;
    } while (i >= 0);
    func_80015828(2);
    func_80036164();
    EnterCriticalSection();
    StopRCnt(0xF2000002);
    D_8005F3DC = OpenEvent(0xF2000002, 2, 0x1000, func_8003BE88);
    EnableEvent(D_8005F3DC);
    SetRCnt(0xF2000002, 0x1000, 0x1000);
    StartRCnt(0xF2000002);
    ExitCriticalSection();
    if (D_8005F1A4 != 0) {
        func_800316EC();
    }
    t = GetGraphType();
    if (t < 3) {
        t = 0x1000;
    } else {
        t = 0x1016;
    }
    D_8006CD2C = t;
    return -0xFF;
}
