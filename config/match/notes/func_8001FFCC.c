#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;

struct Node;
typedef struct Unk8001FDDC {
    u8 pad[0x10];
    struct Node *items[1];
} Unk8001FDDC;

void func_8001FFCC(Unk8001FDDC *arg0) {
    s32 *flags;

    flags = (s32 *)((u8 *)arg0->items[0] + 0x27C);
    if (!(*flags & 1)) {
        D_8006CDF0 = 0;
        D_8006CDF8 = 0;
        D_8006CDF4 = 0x19000;
        *flags |= 1;
    }
}
