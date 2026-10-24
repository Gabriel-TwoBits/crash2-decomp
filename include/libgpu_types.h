#ifndef LIBGPU_TYPES_H
#define LIBGPU_TYPES_H

/* PsyQ 4.0's libgpu types the game's structures hold, apart from sdk.h's function declarations so that a host's code
   may include them beside its C library */

/* An area of VRAM */
typedef struct {
    short x, y;
    short w, h;
} RECT;

/* The display environment PutDispEnv takes (0x14 bytes) */
typedef struct {
    RECT disp;
    RECT screen;
    unsigned char isinter, isrgb24;
    unsigned char pad0, pad1;
} DISPENV;

/* The drawing environment PutDrawEnv takes (0x5C bytes), the GPU packet it builds from the rest last (PsyQ's DR_ENV,
   16 words) */
typedef struct {
    RECT clip;
    short ofs[2];
    RECT tw;
    unsigned short tpage;
    unsigned char dtd, dfe;
    unsigned char isbg;
    unsigned char r0, g0, b0;
    unsigned int dr_env[16];
} DRAWENV;

#endif /* LIBGPU_TYPES_H */
