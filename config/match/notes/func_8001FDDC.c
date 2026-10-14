#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Header {
    u8 pad[0x184];
    s32 unk184;
    s32 unk188;
    s32 unk18C;
} Header;

typedef struct Node {
    s32 unk0;
    struct Unk8001FDDC *unk4;
    struct Node *unk8;
} Node;

typedef struct Unk8001FDDC {
    u8 pad[0x10];
    Node *items[1];
} Unk8001FDDC;

s32 func_8001FF50(s32, Unk8001FDDC *);
void func_8001FFCC(Unk8001FDDC *);
extern s32 D_800608CC;

s32 func_8001FDDC(Unk8001FDDC *obj) {
    s32 n;
    s32 i;
    s32 j;
    Node *first;
    Node *node;
    s32 count;

    count = ((Header *)obj->items[0])->unk188;
    /* This empty loop and the next are scheduling barriers decomp-permuter found; the original's shape there is
       unknown */
    do {
    } while (0);
    n = count / 3;
    for (i = 0; i < n; i++) {
        first = obj->items[i * 3 + ((Header *)obj->items[0])->unk184];
        do {
        } while (0);
        j = 1;
        first->unk4 = obj;
        for (; j < 3; j++) {
            node = obj->items[i * 3 + j + ((Header *)obj->items[0])->unk184];
            node->unk4 = obj;
            node->unk8 = first;
        }
    }
    for (i = 0; i < ((Header *)obj->items[0])->unk18C; i++) {
        obj->items[i + ((Header *)obj->items[0])->unk184 + ((Header *)obj->items[0])->unk188]->unk4 = obj;
    }
    if (D_800608CC != 0 && func_8001FF50(D_800608CC, obj) != 0) {
        func_8001FFCC(obj);
    }
    return -0xFF;
}
