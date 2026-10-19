#include "common.h"
#include "sdk.h"
#include "game.h"

s32 Heap_Realloc(s32, s32);
s32 g_UseCd = 1;
extern char D_8005F1A8[];
int CdIntToPos(int, unsigned char *);
int CdRead(int, s32, int);
int printf(const char *, ...);

typedef struct {
    u32 sector : 23;
    u32 pad0 : 9;
    u32 size : 17;
    u32 pad1 : 15;
} Unk800123F0;

s32 Cd_ReadFile(s32 *a0, s32 arg1, s32 arg2) {
    u8 sp10[8];
    u8 sp18[8];
    s32 s2;
    Unk800123F0 *arg0;
    s32 ret;

    ret = g_UseCd;
    if (ret == 0) {
        goto end;
    }
    s2 = 0;
    if (arg2 != 1) {
        return arg1;
    }
    arg0 = (Unk800123F0 *)a0;
    if (arg1 == 0) {
        s2 = (((s32)arg0->size + 0x7FF) / 2048) << 11;
        arg1 = Heap_Alloc(s2);
        if (arg1 == 0) {
            return -0xF;
        }
    }
loop:
    CdIntToPos(arg0->sector, sp18);
    if (CdControl(2, sp18, sp10) == 0) {
        goto loop;
    }
    if (CdRead((u32)(((s32)arg0->size + 0x7FF) / 2048), arg1, 0x80) == 0 || Cd_SyncRead(1) == -0x10) {
        printf(D_8005F1A8);
        goto loop;
    }
    if (s2 != 0 && s2 != arg0->size) {
        arg1 = Heap_Realloc(arg1, arg0->size);
    }
    ret = arg1;
end:
    return ret;
}
