#include "common.h"
#include "sdk.h"
#include "game.h"

#define STEP_RANGE 0x100
#define FIXED_SHIFT 10
#define STATE_WORDS 4
#define SLOT_STRIDE_WORDS 3
#define OBJ_CHILD_OFFSET 0x10
#define CHILD_INDEX_OFFSET 0x184
#define STATE_BASE_BACKOFF 0xC
#define MODE_DEFAULT 2
#define MODE_SAME_TARGET 1
#define TRIGGER_VALUE 5

extern s8 D_8005B994;
extern s32 D_8005BA14;
extern s32 D_8005BA18;
extern s32 D_8005BA1C;
extern s32 D_8005BA20;
extern s32 D_8005BA24;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_8005BAE4;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800608CC[];
extern s32 D_800608D4;

/* Locals in retail stack order; the pads are unused slots. */
typedef struct {
    s32 cur[STATE_WORDS];
    s32 next[STATE_WORDS];
    s32 pad0[STATE_WORDS];
    s32 out[3];
    s32 pad1;
    s32 path[3];
    s32 pad2;
    s32 dist[3];
    s32 pad3;
    s32 pos[3];
} Unk80022A5CFrame;

void func_80022A5C(s32 *arg0) {
    Unk80022A5CFrame f;
    s32 *obj;
    s32 *target;
    s32 ret;
    s32 mode;
    s32 *q;
    s32 scale;
    s32 i;

    f.path[0] = arg0[0];
    f.path[1] = arg0[1];
    f.cur[0] = D_800608CC[0];
    f.cur[1] = D_800608D4;
    f.cur[2] = D_800608E0;
    f.cur[3] = D_8005BAE4;
    f.path[2] = arg0[2];
    obj = (s32 *)func_80014260((char *)&D_8005BAE4 - STATE_BASE_BACKOFF, 1, 0);
    f.next[0] = (s32)obj;
    q = *(s32 **)((char *)obj + OBJ_CHILD_OFFSET);
    target = *(s32 **)((char *)obj + OBJ_CHILD_OFFSET +
                       (D_8005BADC * SLOT_STRIDE_WORDS + *(s32 *)((char *)q + CHILD_INDEX_OFFSET)) * 4);
    f.next[1] = (s32)target;
    if (D_800608D4 == (s32)target) {
        f.next[2] = D_800608E0;
    } else {
        f.next[2] = D_8005BAE0;
    }
    if (D_8005BAE0 == 0) {
        f.next[3] = 2;
    } else {
        f.next[3] = 1;
    }
    ret = func_8002433C(f.next, f.path, &f.dist[0], &f.dist[1], &f.dist[2]);
    if (D_8005BA14 != 0) {
        if (D_800608D4 == (s32)target) {
            mode = MODE_SAME_TARGET;
        } else {
            mode = MODE_DEFAULT;
        }
        if (ret > 0) {
            mode = func_80024658(f.cur, f.path, f.dist[1], 0, f.dist[2], mode);
        } else if (ret < 0) {
            mode = func_80024658(f.cur, f.path, f.dist[0], 1, f.dist[2], mode);
        } else {
            for (i = 0; i < STATE_WORDS; i++) {
                f.next[i] = f.cur[i];
            }
        }
        func_80026A08((s32 *)f.next[1], f.next[2], f.pos, 0);
        if (ret != 0) {
            func_80023E60(f.cur, mode, f.out, STEP_RANGE / D_8005BA14);
            func_800200A8(f.out[0], f.out[1], f.out[2], 0);
        } else {
            func_800200A8(D_800608CC[0], D_800608D4, D_800608E0, 0);
        }
        scale = ((D_8005BA18 - (D_8005BA14 - 1)) << FIXED_SHIFT) / D_8005BA18;
        D_800607F4 = (((f.pos[0] - D_8005BA1C) * scale) >> FIXED_SHIFT) + D_8005BA1C;
        D_800607F8 = (((f.pos[1] - D_8005BA20) * scale) >> FIXED_SHIFT) + D_8005BA20;
        D_800607FC = (((f.pos[2] - D_8005BA24) * scale) >> FIXED_SHIFT) + D_8005BA24;
        func_800231B0(f.path);
        D_8005BA14 = D_8005BA14 - 1;
        if (D_8005BA14 == 0) {
            D_8005B994 = TRIGGER_VALUE;
            func_80023084(D_800608D4, D_8005BAE0);
        } else {
            return;
        }
    } else {
        mode = MODE_DEFAULT;
        if (ret > 0) {
            mode = func_80024658(f.cur, f.path, f.dist[1], 0, f.dist[2], mode);
        } else if (ret < 0) {
            mode = func_80024658(f.cur, f.path, f.dist[0], 1, f.dist[2], mode);
        } else {
            /* mode stays at the default */
        }
        func_80023E60(f.cur, mode, f.out, -1);
        func_800200A8(f.out[0], f.out[1], f.out[2], 0);
        func_800231B0(f.path);
        if (D_800608D4 == (s32)target) {
            D_8005B994 = TRIGGER_VALUE;
            func_80023084(D_800608D4, D_8005BAE0);
        } else {
            return;
        }
    }
}
