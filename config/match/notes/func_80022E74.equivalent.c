#include "common.h"
#include "sdk.h"
#include "game.h"

#define UNK_80014260_OFFSET 0x13D
#define UNK_80014260_FLAGS 1
#define UNK_FIELD_OFFSET 0x10
#define UNK_INDEX_OFFSET 0x184
#define UNK_TABLE_STRIDE 3
#define UNK_STATE_5 5
#define UNK_D_8005F38C_FIELD 0x18

typedef struct Unk80022E74Inner {
    char pad0[UNK_INDEX_OFFSET];
    s32 unk_184;
} Unk80022E74Inner;

typedef struct Unk80022E74 {
    char pad0[UNK_FIELD_OFFSET];
    Unk80022E74Inner *unk_10;
    s32 unk_14[1];
} Unk80022E74;

extern s8 D_8005B994;
extern s8 D_8005B99B;
extern s32 D_800608E0;
extern s32 D_8005F38C;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_800608CC;
extern s32 D_800608D4;

void func_80022E74(s32 *arg0) {
    s32 args[3];
    Unk80022E74 *p;

    args[0] = arg0[0];
    args[1] = arg0[1];
    args[2] = arg0[2];
    if (D_8005B99B != 0) {
        p = (Unk80022E74 *)func_80014260(&D_8005B99B + UNK_80014260_OFFSET, UNK_80014260_FLAGS, 0);
        func_800200A8((s32)p,
                      ((s32 *)p)[(UNK_FIELD_OFFSET / 4) + D_8005BADC * UNK_TABLE_STRIDE + p->unk_10->unk_184],
                      D_8005BAE0, 0);
        D_8005B994 = UNK_STATE_5;
        ((s32 *)D_8005F38C)[UNK_D_8005F38C_FIELD / 4] = D_800608CC;
        return;
    } else {
        func_800200A8(D_800608CC, D_800608D4, D_800608E0, 0);
        func_800231B0(args);
    }
}
