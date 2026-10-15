#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F17C;
extern void *D_80062A2C;
extern void *D_80062A30;
extern void *D_80062A34;
extern void *D_80062A38;
extern void func_80042420(void);
extern void func_80042548(void);
extern void func_800426A0(void);
extern void func_80042818(void);

void func_80011E24(void) {
    if (D_8005F17C == 0) {
        D_80062A2C = &func_80042420;
        D_80062A30 = &func_800426A0;
        D_80062A34 = &func_80042548;
        D_80062A38 = &func_80042818;
    }
}
