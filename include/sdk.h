#ifndef SDK_H
#define SDK_H

/* PsyQ 4.0's library functions the game calls (config/symbol_addrs.txt has their addresses), and their types */

/* libgpu: an area of VRAM */
typedef struct {
    short x, y;
    short w, h;
} RECT;

/* libc */
int sprintf(char *buffer, const char *format, ...);

#endif /* SDK_H */
