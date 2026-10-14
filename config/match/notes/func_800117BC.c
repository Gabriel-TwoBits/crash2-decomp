#include "common.h"
#include "sdk.h"
#include "game.h"

void __main(void);
void func_80011800(s32 arg0);
void func_80015614(void);
void func_800157A0(void);
extern s32 D_8005F1A4;

s32 func_800117BC(void) {
    __main();
    D_8005F1A4 = 1;
    func_80015614();
    func_80011800(0x3C);
    func_800157A0();
    return 0;
}
