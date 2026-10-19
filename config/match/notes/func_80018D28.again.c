#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2);

typedef struct Unk80018D28 {
    s32 kind;
    struct Unk80018D28 *first;
    char pad[0x40];
    struct Unk80018D28 *next;
    struct Unk80018D28 *head;
} Unk80018D28;

s32 Obj_ForEachDescendant(void *arg0, void *arg1, void *arg2) {
    Unk80018D28 *node;
    Unk80018D28 *next;
    int is_kind2;

    if (is_kind2 = (((Unk80018D28 *)arg0)->kind == 2)) {
        node = ((Unk80018D28 *)arg0)->first;
    } else {
        node = ((Unk80018D28 *)arg0)->head;
    }
    while (node != 0) {
        next = node->next;
        if (func_80018B50(node, (s32 (*)(void *, s32))arg1, (s32)arg2) == -0x19) {
            Obj_Kill((s32 *)arg0, 0);
        }
        node = next;
    }
    return -0xFF;
}
