#include "common.h"
#include "sdk.h"
#include "game.h"

#define POS_SHIFT 8
#define SCREEN_CENTER_X 0x100
#define SCREEN_CENTER_Y 0x6C
#define SXY_Y_SHIFT 16
#define RESULT_CONST (-0xFF)

extern s32 ReadSZfifo3(s32 *, s32 *, s32 *);
extern s32 RotTransPers(s16 *, s32 *, s32 *, s32 *);
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;

s32 func_8001ED60(s32 *arg0, s32 *arg1) {
    /* One struct pins the stack slots the callees see. */
    struct {
        s16 vec[3];
        s32 pos[4];
        s32 sxy;
        s32 p;
        s32 flag;
        s32 sz[3];
        s32 pad[2];
    } l;

    l.pos[0] = arg0[0];
    l.pos[1] = arg0[1];
    l.pos[2] = arg0[2];
    SetRotMatrix(&D_80060774);
    SetTransMatrix(&D_80060130);
    l.vec[2] = (l.pos[2] - D_800607FC) >> POS_SHIFT;
    l.vec[0] = (l.pos[0] - D_800607F4) >> POS_SHIFT;
    l.vec[1] = (l.pos[1] - D_800607F8) >> POS_SHIFT;
    RotTransPers(l.vec, &l.sxy, &l.p, &l.flag);
    arg1[0] = ((s16)l.sxy - SCREEN_CENTER_X) << POS_SHIFT;
    arg1[1] = (SCREEN_CENTER_Y - (l.sxy >> SXY_Y_SHIFT)) << POS_SHIFT;
    ReadSZfifo3(&l.sz[0], &l.sz[1], &l.sz[2]);
    arg1[2] = l.sz[2] << POS_SHIFT;
    return RESULT_CONST;
}
