#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80026334 {
    u8 pad0[3];
    s8 size;
    s32 key;
} Unk80026334;

extern s32 D_8005B9E4;
extern s32 D_800608D4[];
void func_8002648C(Unk80026334 *, s32);

void func_80026334(s32 arg0) {
    s32 key;
    Unk80026334 *p;

    key = arg0 & ~D_8005B9E4;
    if (key != 0) {
        p = (Unk80026334 *)func_80031AE8(D_800608D4[0], 0x1A8, (func_800322C0(D_800608D4[0], 0x4B, 0, 2) - 1) << 8, 0, 0, 1, 0, 0, 0);
        if (p != 0 && p->size != 0) {
            do {
                if (p->key == key) {
                    func_8002648C(p, 2);
                }
                p = (Unk80026334 *)((u8 *)p + p->size);
            } while (p->size != 0);
        }
        p = (Unk80026334 *)func_80031AE8(D_800608D4[0], 0x1A8, 0, 0, 0, 1, 0, 0, 0);
        if (p != 0 && p->size != 0) {
            do {
                if (p->key == key) {
                    func_8002648C(p, 1);
                }
                p = (Unk80026334 *)((u8 *)p + p->size);
            } while (p->size != 0);
        }
    }
}
