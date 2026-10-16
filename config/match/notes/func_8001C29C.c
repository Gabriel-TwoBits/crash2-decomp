#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005C514;
extern s32 D_8005F38C;
extern s32 D_80060944;
extern u32 *D_80063454;
extern u32 D_8006CB64;
extern s32 D_8006CCAC;
extern u8 func_80018CAC;
extern u8 func_8001C718;
void func_80019F08(void);

void func_8001C29C(void *arg0) {
    D_8006CB64 = D_80063454[3] / 34;
    func_8002CA98();
    D_8006CCAC = D_80060944;
    func_80019F08();
    if (D_8005F38C != 0) {
        func_80018DD0(&func_80018CAC, &func_8001C718, arg0);
    }
    *(u8 **)0x1F80005C = &D_8005C514;
}
