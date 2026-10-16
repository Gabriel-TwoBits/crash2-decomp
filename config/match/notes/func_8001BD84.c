typedef int s32; typedef unsigned int u32; typedef unsigned char u8;
int sprintf(char *buffer, const char *format, ...);
void *func_80014260(void *, s32, s32);
s32 func_8001B83C(void *, char *, s32, s32, void *, s32, s32);
s32 func_8003D3CC(void *, void *, s32, s32, void *, s32, s32, s32);
extern char D_8006083C[];
extern s32 D_80060884;

void func_8001BD84(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3, s32 arg4) {
    char buf[200];
    s32 i, off, v, scale;
    u32 idx;
    char *s, *p;
    s32 base;
    void *res;
    s32 n;

    n = *(s32 *)(arg0 + 0x78);
    if (n < 0) {
        n = -n;
    }
    scale = n / 27279;
    if (func_8003D3CC(arg0 + 0x60, D_8006083C, (u32)(arg0[0x120] - 6) < 2U, scale, D_8006083C - 0x68, D_80060884, 0x3C00, (*(u32 *)(arg0 + 0xAC) >> 18 ^ 1) & 1) != 0) {
        s = (char *)arg1 + 0xC;
        for (i = 0; i < arg2; ) {
            if (*s == 0) {
                i++;
            }
            s++;
        }
        idx = *(u32 *)(arg0 + 0x108) >> 8;
        off = idx * 4;
        if (idx == 0) {
            off = *(s32 *)(arg1 + 8) * 4;
        }
        base = *(s32 *)(*(u8 **)(arg0 + 0xC) + 0x24) + off;
        res = func_80014260((void *)(base + 4), 1, 0);
        sprintf(buf, s, *(s32 *)(*(u8 **)(arg0 + 0xBC) - 8), *(s32 *)(*(u8 **)(arg0 + 0xBC) - 0xC), *(s32 *)(*(u8 **)(arg0 + 0xBC) - 0x10), *(s32 *)(*(u8 **)(arg0 + 0xBC) - 0x14));
        s = buf;
        p = s;
        while (*p != 0) {
            p++;
        }
        if (p != s) {
            p--;
            while (p != s && *p == 0x20) {
                p--;
            }
            p[1] = 0;
        }
        v = 0;
        if (arg4 <= 0) {
            v = -func_8001B83C(arg0, s, 0, base, res, scale, 0);
            if (arg4 == 0) {
                v = v / 2;
            }
        }
        func_8001B83C(arg0, s, v, base, res, scale, arg3);
    }
}
