#include "common.h"
#include "sdk.h"
#include "game.h"

#define SCRATCHPAD_LAST_BYTE 0x1F8003FF
#define SCRATCHPAD_SIZE 0x400
#define ROOT_COUNTER_VBLANK 0xF2000002
#define EVENT_MODE_CALLBACK 0x1000
#define EVENT_SPEC_INTERRUPT 2
#define RCNT_TARGET 0x1000
#define TABLE_FILL 0x6396347F
#define FILL_WORDS 4
#define STRIDE_TABLE_LAST_OFFSET 0x294
#define STRIDE_TABLE_STRIDE 0x2C
#define GRAPH_TYPE_THRESHOLD 3
#define VIDEO_FLAG_LOW 0x1000
#define VIDEO_FLAG_HIGH 0x1016

extern s32 D_8005F1A4;
extern s32 D_8005F3DC;
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
void func_8003155C(void);
void func_800316EC(void);

void func_80015614(void) {
    s32 i;
    s32 t;
    s8 *p;
    s32 *q;

    i = SCRATCHPAD_SIZE - 1;
    p = (s8 *)SCRATCHPAD_LAST_BYTE;
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
    i = FILL_WORDS - 1;
    do {
        *q = TABLE_FILL;
        i--;
        q--;
    } while (i >= 0);
    i = STRIDE_TABLE_LAST_OFFSET;
    do {
        *(s32 *)(D_80062D84 + i) = TABLE_FILL;
        i -= STRIDE_TABLE_STRIDE;
    } while (i >= 0);
    func_80015828(2);
    func_80036164();
    EnterCriticalSection();
    StopRCnt(ROOT_COUNTER_VBLANK);
    D_8005F3DC = OpenEvent(ROOT_COUNTER_VBLANK, EVENT_SPEC_INTERRUPT, EVENT_MODE_CALLBACK, func_8003BE88);
    EnableEvent(D_8005F3DC);
    SetRCnt(ROOT_COUNTER_VBLANK, RCNT_TARGET, RCNT_TARGET);
    StartRCnt(ROOT_COUNTER_VBLANK);
    ExitCriticalSection();
    if (D_8005F1A4 != 0) {
        func_800316EC();
    }
    t = GetGraphType();
    if (t < GRAPH_TYPE_THRESHOLD) {
        t = VIDEO_FLAG_LOW;
    } else {
        t = VIDEO_FLAG_HIGH;
    }
    D_8006CD2C = t;
}
