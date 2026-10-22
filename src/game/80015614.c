#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define SCRATCHPAD_SIZE 0x400
#define SCRATCHPAD_LAST_BYTE (SCRATCHPAD + SCRATCHPAD_SIZE - 1)
#define ROOT_COUNTER_2 0xF2000002
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

extern s32 g_UseCd;
extern s32 D_8005F3DC;
extern s32 D_8006015C;
extern char D_80062D84[];
extern s32 D_800675AC;
extern s32 D_8006CD2C;

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

void Sys_Init(void) {
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
    if (g_UseCd != 0) {
        ResetCallback();
        CdInit();
        CdSetDebug(1);
    }
    Gfx_Init();
    Rand_ResetSeedAlt();
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
    Input_Init(2);
    Save_InitCard();
    EnterCriticalSection();
    StopRCnt(ROOT_COUNTER_2);
    D_8005F3DC = OpenEvent(ROOT_COUNTER_2, EVENT_SPEC_INTERRUPT, EVENT_MODE_CALLBACK, Sys_TimerHandler);
    EnableEvent(D_8005F3DC);
    SetRCnt(ROOT_COUNTER_2, RCNT_TARGET, RCNT_TARGET);
    StartRCnt(ROOT_COUNTER_2);
    ExitCriticalSection();
    if (g_UseCd != 0) {
        Nsf_ScanDisc();
    }
    t = GetGraphType();
    if (t < GRAPH_TYPE_THRESHOLD) {
        t = VIDEO_FLAG_LOW;
    } else {
        t = VIDEO_FLAG_HIGH;
    }
    D_8006CD2C = t;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80015614", Sys_Init);
#endif

extern s32 g_UseCd;
s32 D_8005F3DC;

s32 Sys_Shutdown(void) {
    if (g_UseCd != 0) {
        CdControl(CdlStop, 0, 0);
        StopCallback();
    }
    Save_StopCard();
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
extern Slot g_PadCount;
extern char D_80069944[];
extern int InitPAD(void *, int, void *, int);
extern void StartPAD(void);
extern void ChangeClearPAD(int);

void Input_Init(s32 arg0) {
    Slot *p;
    char *base;
    s32 i;
    s32 off;

    i = 0;
    g_PadCount.w[0] = arg0;
    if (arg0 > 0) {
        p = &g_PadCount;
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
INCLUDE_ASM("asm/nonmatchings/game/80015614", Input_Init);
#endif
