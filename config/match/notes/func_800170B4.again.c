#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 pad0[2]; s32 kind; } Obj170B4;
typedef struct { char pad[0x404]; s32 count; } Cnt170B4;
typedef struct { char pad[0x18]; Cnt170B4 *list; } Hdr170B4;
extern Hdr170B4 D_80067588;
extern Cnt170B4 *D_800675A0;

s32 Gfx_BindTexturePage(s32 arg0, s32 arg1) {
    s32 pad[2];
    s32 i;
    Obj170B4 *o;
    Hdr170B4 *h = &D_80067588;
    Hdr170B4 *new_var;

    i = 0;
    if (D_800675A0->count > 0) {
        new_var = h;
        do {
            o = *(Obj170B4 **)((char *)new_var->list + i * 8 + 0x520);
            if (!((s32)o & 3)) {
                if (o->kind != 2) {
                    if (o->kind == 3) {
                        Gfx_BindSceneryTextures((s32)o, arg1, arg0);
                    }
                } else {
                    Gfx_BindModelTextures(o, arg1, arg0);
                }
            }
            i++;
        } while (i < new_var->list->count);
    }
    return -0xFF;
}
