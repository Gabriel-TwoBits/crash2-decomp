#ifndef GAME_GFX_H
#define GAME_GFX_H

#include "common.h"
#include "libgpu_types.h"

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
    /* 0x2088 */ struct DrawPacket *object_queue;
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

/* A draw packet, as Gfx_QueueObject builds them in the frame's packet list: the next packet, its three steps (the
   GTE's rotation and translation for the object, a light or scene setup, the drawer), the object, its matrix (a
   rotation's five words, then the scale), and what the steps read; a model's packet (kind 1) holds them all, a sprite's
   (kind 2) to 0x2C, kind 5's to 0x30 */
struct DrawPass;
typedef void (*DrawStep)(struct DrawPass *);

typedef s32 (*DrawTest)(struct DrawPass *);

typedef struct DrawPacket {
    struct DrawPacket *next;
    DrawStep place;
    DrawStep prepare;
    DrawStep draw;
    u8 *object;
    u32 *matrix;
    u32 clock;
    union {
        /* A sprite's record (its colour and code, then a corner's texture words; kind 5's list of them), fade, size,
         * far colour, and kind 5's frame */
        struct {
            u32 *record;
            s16 fade;
            s16 unk_22;
            u32 size_shift;
            u32 far_colour;
            u32 frame;
        } sprite;
        /* A model's visibility test, far colour step, colour routine, and its frame (two to blend) */
        struct {
            DrawTest visible;
            DrawStep far_setup;
            DrawStep shade;
            u32 *frames[2];
        } model;
    } kind;
    u32 *strips;
    u32 *colours;
    u8 *textures;
    u32 *animations;
    s16 model_fade;
    s16 model_fade_high;
    u32 model_far_colour;
    u32 far_depths[2];
    s32 offset_y;
    s32 level;
    u32 colour_shift;
    u32 colour_mask;
    u32 *widths;
} DrawPacket;

#endif /* GAME_GFX_H */
