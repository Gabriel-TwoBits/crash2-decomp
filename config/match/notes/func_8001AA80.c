#include "common.h"
#include "sdk.h"
#include "game.h"

extern void (*D_8005B650[])(void *, void *, void *, s32);
extern s32 D_8005B690[];
extern s32 D_80062AE0;
extern s32 D_80062AE4;

void func_8001AA80(u8 *obj, s32 *out) {
    u8 *state;
    s32 idx;
    s32 v;

    state = *(u8 **)(obj + 0xE8);
    if (state != 0) {
        switch (*state) {
        case 2:
        case 5:
            v = *(s32 *)(obj + 0x78);
            if (v < 0) {
                v = -v;
            }
            out[1] = v / 27279;
            idx = obj[0x120];
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE0;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](*(void **)(obj + 8), obj + 0x6C, obj + 0x78, 0);
            break;
        case 1:
            idx = obj[0x120];
            out[0] = D_8005B690[idx];
            if (out[0] == 0) {
                idx = D_80062AE4;
                out[0] = D_8005B690[idx];
            }
            D_8005B650[idx](*(void **)(obj + 8), obj + 0x6C, obj + 0x78,
                            *(s32 *)(*(u8 **)(*(u8 **)(obj + 8) + 0x28) + 0x10));
            break;
        default:
            break;
        }
    }
}
