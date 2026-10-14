#include "common.h"
#include "sdk.h"

extern void func_80012ED0(int);
extern void func_800155E0(void *, int);
extern void func_800116C0(int);
extern void func_80032588();
extern void func_80034394();
extern void func_80051328(int);
typedef struct { void (*fn)(); char pad[24]; } Entry;
extern Entry D_8005B1F0[];
s32 D_8005F1A4 = 1;
extern int D_80067FB4[];
extern int D_80067FBC[];

int func_8001521C(int *p) {
    int i;
    int off;
    char buf[8];
    if (p[0] == 1) {
        if (D_8005F1A4 != 0 && D_80067FB4[0] != 0) func_80012ED0(1);
        if (D_80067FBC[0] != 0) func_80051328(1);
        i = 0;
        func_80034394();
        off = 0;
        func_80032588();
        do {
            void (*f)() = *(void (**)())((char *)D_8005B1F0 + off);
            if (f != 0) f();
            i++;
            off += 0x1C;
        } while (i < 22);
        i = 0;
        if (p[10] > 0) {
            do {
                func_800155E0(p, i);
                i++;
            } while (i < p[10]);
        }
        func_800116C0(p[11]);
        func_800116C0(p[0xA3C / 4]);
        func_800116C0(p[6]);
        i = -0xFF;
        p[0] = 0;
        return i;
    }
    return -0xFF;
}
