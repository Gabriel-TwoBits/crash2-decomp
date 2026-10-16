#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 a; u32 key; } E80014B90;
typedef struct { u8 pad[0x404]; s32 count; } T80014B90;
extern E80014B90 **D_80067590;
extern E80014B90 *D_80067594;
extern T80014B90 *D_800675A0;

E80014B90 *func_80014B90(u32 arg0) {
    E80014B90 *p;

    p = *(E80014B90 **)((u8 *)D_80067590 + ((arg0 >> 13) & 0x3FC));
    if (p->key == arg0) {
        return p;
    }
    p++;
    for (;;) {
        if (p - D_80067594 >= D_800675A0->count) {
            return (E80014B90 *)-10;
        }
        if (p->key == arg0) {
            break;
        }
        p++;
    }
    return p - 1;
}
