#include "common.h"
#include "sdk.h"
#include "game.h"

void SetDefDrawEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void PutDrawEnv(void *env);
void TermPrim(void *p);
void DrawPrim(void *p);

typedef struct {
    u8 pad0[0x3];
    u8 len;
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    s16 w, h;
} Fill;

void func_8001601C(void) {
    u8 env[0x60];
    Fill f;

    SetDefDrawEnv(env, 0, 0, 0x400, 0x200);
    env[0x17] = 1;
    PutDrawEnv(env);
    TermPrim(&f);
    f.len = 3;
    f.code = 2;
    f.w = 0x3FF;
    f.h = 0x1FF;
    f.r0 = 0;
    f.g0 = 0;
    f.b0 = 0;
    f.x0 = 0;
    f.y0 = 0;
    DrawPrim(&f);
}
