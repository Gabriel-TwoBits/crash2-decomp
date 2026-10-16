typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;
typedef struct Ent { u32 a; s32 b; s32 c; } Ent;
s32 D_8005F3B8 = 0;
extern Ent D_8006302C[];
extern char D_8006B1C4[];
extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;
void *func_80014260(void *, s32, s32);
s32 func_80019098(void *, s32);
s32 func_80019BBC(s32 *, s32);
s32 func_80019FA0(s32);

void func_8001A23C(void *arg0, s32 arg1) {
    s32 i;
    s32 j;
    s32 t;
    u32 key;
    u32 idx;
    Ent *e;
    Ent *dst;
    Ent *src;
    s32 *p;

    D_8006CDF0 = 0;
    D_8006CDF8 = 0;
    D_8006CDF4 = 0x19000;
    for (i = D_8005F3B8 - 1; i >= 0; i--) {
        e = &D_8006302C[i];
        if (arg1 != -1 && arg1 != e->c) continue;
        t = e->c;
        if (t == 0x13B) {
            key = e->a & 0xFFFF00;
            if (*(s32 *)(D_8006B1C4 + (key >> 6)) & 1) {
                if (e->b != 0 || (e->b = func_80019FA0(key)) != 0) {
                    p = (s32 *)e->b;
                    if (!(p[0xAC / 4] & 0x1000000) && !(p[0x100 / 4] & 0x40000)) {
                        func_80019BBC(p, 1);
                    }
                }
            }
            goto remove;
        } else if (t == 0x13C) {
            key = e->a & 0xFFFF00;
            if (!(*(s32 *)(D_8006B1C4 + (key >> 6)) & 1)) {
                idx = e->a & 0xFF;
                if (idx < 8) {
                    e->b = func_80019098(func_80014260((char *)*(s32 *)((char *)arg0 + 0x10) + idx * 4 + 0x194, 1, 0), *((u8 *)e + 3));
                    if (e->b >= -0xFE) goto remove;
                }
            }
            if (e->b == 0) e->b = func_80019FA0(key);
            e->c = 0;
        }
        continue;
remove:
        j = i + 1;
        if (j < D_8005F3B8) {
            dst = &D_8006302C[j] - 1;
            src = &D_8006302C[j];
            do {
                *dst = *src;
                dst++; j++; src++;
            } while (j < D_8005F3B8);
        }
        D_8005F3B8--;
    }
}
