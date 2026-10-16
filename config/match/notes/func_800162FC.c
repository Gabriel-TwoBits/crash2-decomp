#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F3EC;
extern s32 D_800654E8;
extern s32 DrawSync(s32);
extern s32 func_800363D0(void);
typedef struct Unk800162FC {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    u8 pad[0x2094 - 12];
} Unk800162FC;
extern Unk800162FC D_80063458[];
extern s32 D_8006344C;

s32 func_800162FC(void) {
    s32 i;
    Unk800162FC *p;
    char *q;
    s32 pad[2];

    i = 0;
    func_800163B4((void *)D_80063458[0].unk0);
    DrawSync(0);
    func_800116C0(D_800654E8);
    if (D_8006344C > 0) {
        q = (char *)D_80063458 + 4;
        p = (Unk800162FC *)q;
        do {
            func_800116C0(p->unk4);
            p++;
            i++;
        } while (i < *(s32 *)(q - 0x10));
    }
    if (D_8005F3EC == 2) {
        func_800363D0();
    }
    return -0xFF;
}
