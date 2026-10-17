#include "common.h"
#include "sdk.h"
#include "game.h"

#define UNK_FLAGS_OFFSET 0x100
#define UNK_FLAG_FIXED_HEIGHT 0x40000000
#define UNK_PTR_OFFSET 8
#define UNK_PTR_HEIGHT_OFFSET 0x40
#define UNK_FIXED_HEIGHT 0x29900
#define UNK_BOX_OFFSET 0x1020
#define UNK_OUT_OFFSET 0x1008
#define UNK_BOX_NEAR_NUMERATOR (-0x9600)
#define UNK_BOX_FAR_NUMERATOR 0x9600
#define UNK_BOX_MARGIN 0xC80
#define UNK_BOX_DROP 0x4B00
#define UNK_CALL_KEY 0xC4653601

void func_8002A3B0(void *arg0, void *arg1, void *arg2, void *arg3, s32 arg4, s32 arg5) {
    s32 *src = (s32 *)arg2;
    s32 *box = (s32 *)((char *)arg3 + UNK_BOX_OFFSET);
    s32 base;
    s32 diff;
    s32 near;
    s32 far;
    s32 y;

    base = src[1] + D_8005F204;
    diff = ((s32 *)arg1)[1] - src[1];
    if (diff > 0) {
        base += diff;
    }
    near = UNK_BOX_NEAR_NUMERATOR / arg4;
    far = UNK_BOX_FAR_NUMERATOR / arg4;
    box[0] = src[0] + near + UNK_BOX_MARGIN;
    box[1] = src[1] - UNK_BOX_DROP;
    box[2] = src[2] + near + UNK_BOX_MARGIN;
    box[3] = src[0] + far - UNK_BOX_MARGIN;
    if (!(*(s32 *)((char *)arg0 + UNK_FLAGS_OFFSET) & UNK_FLAG_FIXED_HEIGHT)) {
        y = src[1] + *(s32 *)(*(char **)((char *)arg0 + UNK_PTR_OFFSET) + UNK_PTR_HEIGHT_OFFSET);
    } else {
        y = src[1] + UNK_FIXED_HEIGHT;
    }
    box[4] = y;
    box[5] = src[2] + far - UNK_BOX_MARGIN;
    func_8003CD0C(arg0, arg3, (char *)arg3 + UNK_OUT_OFFSET, box, base, arg5, UNK_CALL_KEY, &func_8002A1B8);
}
