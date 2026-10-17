#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_SIZE 0x2094
#define STATE_FLUSH 2
#define RESULT_DONE -0xFF

typedef struct {
    s32 idx;
    s32 unk4;
    u8 pad8[ENTRY_SIZE - 8];
} Unk800162FCEntry;

typedef struct {
    s32 count;
    void *cur;
    void *other;
    void *first;
    Unk800162FCEntry e[1];
} Unk800162FCGfx;

extern Unk800162FCGfx D_8006344C;
extern s32 *D_80063458[];
extern s32 D_8005F3EC;
extern s32 D_800654E8;
extern s32 DrawSync(s32);
extern s32 func_800363D0(void);

s32 func_800162FC(void) {
    s32 i;

    i = 0;
    func_800163B4((void *)D_80063458[0]);
    DrawSync(0);
    func_800116C0(D_800654E8);
    if (D_8006344C.count > 0) {
        do {
            func_800116C0(D_8006344C.e[i].unk4);
            i++;
        } while (i < D_8006344C.count);
    }
    if (D_8005F3EC == STATE_FLUSH) {
        func_800363D0();
    }
    return RESULT_DONE;
}
