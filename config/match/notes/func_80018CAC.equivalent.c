#include "common.h"
#include "sdk.h"
#include "game.h"

#define NODE_KIND_LIST4 2

typedef struct Unk80018AA8 {
    s32 f0;
    u8 pad[0x44];
    struct Unk80018AA8 *next;
    struct Unk80018AA8 *child;
} Unk80018AA8;

typedef s32 (*Cb80018AA8)(Unk80018AA8 *, void *);

typedef struct Unk80018CAC {
    s32 kind;
    Unk80018AA8 *children4;
    u8 pad[0x44];
    Unk80018AA8 *children;
} Unk80018CAC;

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2);

s32 func_80018CAC(Unk80018CAC *arg0, Cb80018AA8 arg1, void *arg2) {
    Unk80018AA8 *node;
    Unk80018AA8 *next;

    if (arg0->kind == NODE_KIND_LIST4) {
        node = arg0->children4;
    } else {
        node = arg0->children;
    }
    while (node != 0) {
        next = node->next;
        func_80018AA8(node, arg1, arg2);
        node = next;
    }
    return -0xFF;
}
