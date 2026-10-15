#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80062D70 {
    s16 state;
    u8 pad[0x2A];
} Unk80062D70;

extern Unk80062D70 D_80062D70[];

void func_80013968(void);

void func_80013A54(s32 arg0, s32 arg1) {
    if (arg1 != 0) {
        func_80013968();
        D_80062D70[arg0].state = 0x1E;
    } else if (D_80062D70[arg0].state == 0x1E) {
        D_80062D70[arg0].state = 1;
    }
}
