#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Node { s32 kind; struct Node *first; u8 pad[0x40]; struct Node *nx; struct Node *head; } Node;
s32 func_80018AA8(Node *, s32, s32);

s32 func_80018CAC(Node *a, s32 b, s32 c) {
    Node *n;
    Node *nx;
    if (a->kind == 2) {
        n = a->first;
    } else {
        n = a->head;
    }
    while (n != 0) {
        nx = n->nx;
        func_80018AA8(n, b, c);
        n = nx;
    }
    return -0xFF;
}
