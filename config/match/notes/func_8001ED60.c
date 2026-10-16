#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 ReadSZfifo3(s32 *, s32 *, s32 *);
extern s32 RotTransPers(s16 *, s16 *, s32 *, s32 *);
extern void SetRotMatrix(void *);
extern void SetTransMatrix(void *);
extern char D_80060130[];
extern char D_80060774[];
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;

s32 func_8001ED60(s32 *arg0, s32 *arg1) {
    s16 sp10[3];
    s32 sp18[3];
    s16 sxy;
    s32 p;
    s32 flag;
    s32 sz0;
    s32 sz1;
    s32 sz2;
    s32 pad[2];

    sp18[0] = arg0[0];
    sp18[1] = arg0[1];
    sp18[2] = arg0[2];
    SetRotMatrix(D_80060774);
    SetTransMatrix(D_80060130);
    sp10[2] = (sp18[2] - D_800607FC) >> 8;
    sp10[0] = (sp18[0] - D_800607F4) >> 8;
    sp10[1] = (sp18[1] - D_800607F8) >> 8;
    RotTransPers(sp10, &sxy, &p, &flag);
    arg1[0] = (sxy - 0x100) << 8;
    arg1[1] = (0x6C - (*(s32 *)&sxy >> 16)) << 8;
    ReadSZfifo3(&sz0, &sz1, &sz2);
    arg1[2] = sz2 << 8;
    return -0xFF;
}
