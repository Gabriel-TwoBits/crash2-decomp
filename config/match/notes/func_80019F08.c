#include "common.h"
#include "sdk.h"

void func_8003B940(void *, void *, s32);
extern char D_8005C514[];
extern char D_8005C650[];
extern s32 D_80060944;
extern u32 *D_80063450;
extern char D_8006CB70[];

void func_80019F08(void) {
    u32 n;
    u32 *p;

    n = D_80063450[5];
    *(s32 *)0x1F800050 = D_80060944;
    p = (u32 *)0x1F800054;
    if (n >= 0x67) {
        n = 0x66;
    }
    *p = n;
    *(char **)0x1F800058 = D_8006CB70;
    func_8003B940(D_8005C514, (void *)0x1F800060, (D_8005C650 - D_8005C514) / 2);
    *(s32 *)0x1F80005C = 0x1F800060;
}
