#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

void SetColorMatrix(void *);
void SetFarColor(s32, s32, s32);
void SetGeomOffset(s32, s32);
void SetGeomScreen(s32);
extern s32 D_80060144, D_80060148, D_8006014C, D_800607F8, D_800607FC, D_80060800, D_80060804, D_80060808, D_8006083C, D_80060840, D_80060844, D_80060860;
extern s16 D_80060864, D_80060866, D_80060868;
extern s32 D_80060878, D_8006087C, D_80060880;
extern Unk80060884 D_80060884;
extern s16 D_8006088E, D_80060890, D_80060892, D_80060894, D_80060896, D_80060898, D_8006089A, D_8006089C;
extern s32 D_8006CC20, D_8006CC24;

void func_8001798C(void) {
    D_8006014C = 0;
    D_80060148 = 0;
    D_80060144 = 0;
    D_80060884.a = 0x120;
    SetGeomScreen(0x120);
    SetGeomOffset(0, 0);
    D_80060868 = 0x1000;
    D_80060864 = 0;
    D_80060866 = 0;
    D_80060878 = 0;
    D_8006087C = 0;
    D_80060880 = 0;
    D_80060884.b = 0x200;
    D_80060892 = 0x200;
    D_80060898 = 0x200;
    D_8006088E = 0x200;
    D_80060894 = 0x200;
    D_8006089A = 0x200;
    D_80060890 = 0x200;
    D_80060896 = 0x200;
    D_8006089C = 0x200;
    SetFarColor(0, 0, 0);
    SetColorMatrix(&D_80060884.b);
    func_80018168((s32 *)((char *)&D_80060884 - 0x90));
    *(s32 *)((char *)&D_80060884 - 0x90) = 0;
    D_800607F8 = 0;
    D_800607FC = 0x1F400;
    D_80060800 = 0;
    D_80060804 = 0;
    D_80060808 = 0;
    D_8006083C = 0;
    D_80060840 = 0xE1000;
    D_80060844 = 0x5DC000;
    D_80060860 = 0;
    D_8006CC20 = 0;
    D_8006CC24 = 0;
}
