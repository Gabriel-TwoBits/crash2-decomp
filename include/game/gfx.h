#ifndef GAME_GFX_H
#define GAME_GFX_H

#include "common.h"
#include "sdk.h"

/* The frame buffers: Gfx_Init sets up two, Gfx_EndFrame turns them over each frame (the one built becomes the one
   drawn, the one drawn the one shown) */

/* An ordering table's entries: Gfx_AllocFrameBuffers clears that many with Gfx_ResetOt, DrawOTag takes the table */
#define GFX_OT_LENGTH 0x800

/* The frame buffers Gfx_Init sets up */
#define GFX_FRAME_COUNT 2

/* A frame buffer (0x2094 bytes): its primitives, its timing in g_TimerTicks, its ordering table, the environments
   Gfx_EndFrame puts, and the object packets Gfx_QueueObject appends for Gfx_DrawObjects */
typedef struct GfxFrame {
    /* 0x0000 */ s32 index;
    /* FRAME_PRIMITIVES_ALLOC's buffer, and the next free byte in it, where Gfx_AddFillRect builds its fill */
    /* 0x0004 */ u8 *primitives;
    /* 0x0008 */ u8 *next_primitive;
    /* When Gfx_EndFrame handed it to the GPU, and when it waited for the GPU to finish it */
    /* 0x000C */ s32 start_ticks;
    /* 0x0010 */ s32 end_ticks;
    /* How long it was shown, rounded to whole vertical blanks (0x11, 0x22, 0x33), which the objects' motion scales by */
    /* 0x0014 */ s32 shown_ticks;
    /* 0x0018 */ u32 ot[GFX_OT_LENGTH];
    /* 0x2018 */ DISPENV disp;
    /* 0x202C */ DRAWENV draw;
    /* The object packets' list, and the buffer both frames take them from (Gfx_AllocFrameBuffers' shared allocation)
       with its next free byte; Gfx_ClearObjectQueue empties them */
    /* 0x2088 */ void *object_queue;
    /* 0x208C */ u8 *queue_buffer;
    /* 0x2090 */ u8 *next_queue_packet;
} GfxFrame;

/* Members the C reaches at their raw offsets where the member would move GCC 2.7.2's code (docs/COMPILER.md,
   "Structure members and plain pointers") */
#define GFX_FRAME_START_TICKS 0xC
#define GFX_FRAME_SHOWN_TICKS 0x14
#define GFX_FRAME_QUEUE_BUFFER 0x208C

/* g_FrameBuffers: the frames' count, the frame shown (g_DisplayBuffer), drawn (g_DrawBuffer) and being built
   (g_BuildBuffer), each also a symbol of its own, then the frames */
typedef struct GfxFrameBuffers {
    s32 count;
    GfxFrame *display;
    GfxFrame *draw;
    GfxFrame *build;
    GfxFrame frames[GFX_FRAME_COUNT];
} GfxFrameBuffers;

#endif /* GAME_GFX_H */
