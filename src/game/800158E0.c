#include "common.h"
#include "sdk.h"

s32 D_8005F1D4 = 0;
s32 D_8005F1D8 = 0;
s32 D_8005F1DC = 0;
s32 D_8005F1E0 = 0;
s32 D_8005F1E4 = 0x808080;
s32 D_8005F1E8 = 0x8000;

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800158E0);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001601C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800160B0);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800161F8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800162FC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800163B4);

extern void func_800163B4();

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4();
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800163E8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016408);

typedef struct {
    u8 pad[0x2088];
    void *unk2088;
    u8 pad2[4];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];

void *func_80016418(s32 arg0) {
    void **temp;

    temp = (void **)D_80063458[0]->unk2090;
    D_80063458[0]->unk2090 = (char *)temp + arg0;
    *temp = D_80063458[0]->unk2088;
    D_80063458[0]->unk2088 = temp;
    return temp;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001645C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016A60);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016B08);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016C58);

void func_80016EFC(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016F04);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016F44);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800170B4);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017198);

void func_80016F44(int, int, int);

void func_800171C0(int a0) {
    func_80016F44(a0, 0, 0x6396347F);
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800171E8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017358);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017380);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800175D8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800176DC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001798C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017AF8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017BC4);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018168);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018194);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018294);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018434);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018474);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001856C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800186D8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800189C4);

typedef struct Node {
    s32 unk0;
    struct Node *children4; /* the child list's head when unk0 == 2 */
    u8 pad08[0x44 - 0x08];
    struct Node *parent;
    struct Node *next;
    struct Node *children; /* the child list's head otherwise */
} Node;

/* Moves node from its parent's child list to the head of parent's. Returns -0x18 when node is missing from its old
 * parent's list. The success path has no return statement: $v0 is left holding the 2 of the last comparison. */
s32 func_800189CC(Node *node, Node *parent) {
    Node *old;
    Node *it;

    old = node->parent;
    if (old != 0) {
        it = old->unk0 == 2 ? old->children4 : old->children;
        if (it == node) {
            *(old->unk0 == 2 ? &old->children4 : &old->children) = node->next;
        } else {
            while (it != 0 ? it->next != node : 1) {
                if (it == 0) {
                    return -0x18;
                }
                it = it->next;
            }
            it->next = node->next;
        }
    }
    node->parent = parent;
    node->next = parent->unk0 == 2 ? parent->children4 : parent->children;
    *(parent->unk0 == 2 ? &parent->children4 : &parent->children) = node;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018AA8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018B50);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018BE4);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018CAC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018D28);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018DD0);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018E48);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018E68);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80019074);
