#include "common.h"
#include "sdk.h"
#include "game.h"
extern void (*D_8005B650[])(void *, void *, void *, s32);
extern s32 D_8005B690[];
extern s32 D_80062AE0;
extern s32 D_80062AE4;

void func_8001AA80(GoolObject *obj, s32 *out) {
    u8 *state;
    s32 idx;
    s32 v;

    state = obj->anim;
    if (state != 0) {
        switch (*state) {
        case 2:
        case 5:
            v = obj->scale[0];
            if (v < 0) {
                v = -v;
            }
            out[1] = v / 27279;
            idx = obj->unk_120;
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE0;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](obj->unk_8, obj->rot, obj->scale, 0);
            break;
        case 1:
            idx = obj->unk_120;
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE4;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](obj->unk_8, obj->rot, obj->scale,
                            *(s32 *)(*(u8 **)((u8 *)obj->unk_8 + 0x28) + 0x10));
            break;
        default:
            break;
        }
    }
}
