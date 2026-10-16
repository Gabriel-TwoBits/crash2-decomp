typedef int s32;
typedef unsigned int u32;
typedef struct Obj {
    char pad0[0x40];
    s32 r[26];
    u32 flags;
    char padAC[0x10];
    s32 *sp;
    char padC0[0x14];
    s32 d4;
} Obj;
s32 *func_80036D0C(Obj *, s32);
s32 func_8001D194(Obj *, s32, s32, s32, s32 *);

s32 func_80039684(Obj *arg0, u32 arg1, s32 arg2) {
    s32 buf[18];
    s32 *p;
    s32 *q;
    s32 *dst;
    s32 n;
    s32 i;
    s32 idx;
    u32 t = arg1 & 0xFFF;

    if ((arg1 & 0xE00) == 0xE00) {
        if (t == 0xE1F) {
            p = arg0->sp - 1;
            arg0->sp = p;
        } else {
            p = &arg0->r[arg1 & 0x1FF];
        }
    } else {
        p = func_80036D0C(arg0, t);
    }
    arg0->flags &= 0xFFFDFFFF;
    t = (arg1 >> 12) & 0x3F;
    if (t == 0x1F) {
        q = arg0->sp - 1;
        arg0->sp = q;
    } else {
        q = &arg0->r[t];
    }
    if (*q != 0 && p != 0) {
        n = (arg1 >> 18) & 7;
        i = 0;
        if (n != 0) {
            dst = buf;
            do {
                *dst = arg0->sp[i - n];
                i++;
                dst++;
            } while (i < n);
        }
        func_8001D194(arg0, *p, (arg1 >> 21) & 7, n, buf);
    } else {
        arg0->d4 = 0;
    }
    if (!(arg0->flags & 0x20000)) {
        arg0->sp = (s32 *)((char *)arg0->sp - ((arg1 >> 16) & 0x1C));
        return 0;
    }
    if (!(arg2 & 2)) {
        arg0->sp = arg0->sp - 1;
        return 0;
    }
    return -0xFF;
}
