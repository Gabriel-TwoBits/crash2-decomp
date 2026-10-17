#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_8001067C);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_8001068C);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_8001069C);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_800106AC);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_800106C4);

s32 *D_8005F264 = 0;
s32 D_8005F268 = 1;
s32 D_8005F26C = -1;
s32 D_8005F270 = -1;
char D_8005F274[] = "bu00:";
char D_8005F27C[] = "%s%s";
char D_8005F284[] = "*";
char D_8005F288[] = "EMPTY";

extern s32 D_8006CC5C;
extern s32 D_8006CC64;

void func_800345C8(void) {
    s32 v;
    s32 *new_var;

    if (D_8005F26C != func_80036150()) {
        D_8005F26C = func_80036150();
        func_80035A6C();
    }
    if (D_8005F270 != func_8003613C()) {
        D_8005F270 = func_8003613C();
        func_80035A74();
    }
    v = func_8003613C();
    switch (v) {
    case 0:
        func_800346E8();
        break;
    case 1:
        func_80034898();
        break;
    case 5:
        func_80034BB8();
        break;
    case 6:
    case 7:
    case 8:
        func_80035444(func_8003613C());
        break;
    case 3:
        func_800359F0();
        break;
    case 4:
        func_80034810();
        break;
    default:
        break;
    }

    v = func_80036150();
    D_8006CC5C = v;
    new_var = &D_8005F264[2];
    D_8006CC64 = *new_var;
}

extern void _card_clear(long);

void func_800346E8(void) {
    if (D_8005F264[6] != 0) {
        D_8005F264[6] = 0;
        _card_info(0);
    }
    switch (func_80035AB8()) {
    case 1:
        func_800360CC(0);
        func_80036084(2);
        return;
    case 0:
        if (!(func_80036150() & 4)) {
            func_800360CC(0);
            return;
        }
    case 3:
        func_80035C74();
        _card_clear(0);
        if (func_80035C40() == 0) {
            func_800360AC(6);
            func_80036084(0x18);
            func_800360CC(4);
            func_80035A7C(0x96);
        } else {
    case 2:
            func_800360CC(0);
            func_80036084(4);
            func_800360AC(8);
        }
        func_80036028(0);
        return;
    }
}

s32 func_80034810(void) {
    D_8005F264[6] = 0;
    if (func_80036414(0) == 1) {
        func_800360AC(0xA);
        func_800360CC(0);
        func_80035FD8();
        return 1;
    }
    if (func_80035A94() != 0) {
        func_80036028(0);
        func_800360AC(6);
        func_80036084(8);
        func_800360CC(0);
        return 1;
    }
    return 0;
}

typedef struct { char b[0x28]; } Unk80035D48;
extern char D_8005F274[];
s32 _card_load(s32);

void func_80034898(void) {
    s32 r;
    s32 a0;

    if (D_8005F264[6] != 0) {
        D_8005F264[6] = 0;
        func_80035B70();
        _card_load(0);
    }
    switch (func_80035AB8()) {
    case 0:
        r = func_80035CBC(D_8005F274, (char *)D_8005F264 + 0x49C);
        func_80035D48((Unk80035D48 *)((char *)D_8005F264 + 0x49C), r);
        if (r <= 0) {
            D_8005F264[7]++;
            if (D_8005F264[7] < 11) {
                func_800360CC(1);
                return;
            }
        }
        func_800360AC(2);
        func_80036050();
        func_80036850();
        func_800360CC(5);
        func_80035A7C(0x20);
        D_8005F264[2] = 0;
        D_8005F264[8] = r;
        D_8005F264[9] = 0;
        break;
    case 2:
        if (func_80035A94() != 0) {
            goto block_15;
        }
        a0 = 1;
        goto block_16;
    case 1:
        if (func_80035A94() != 0) {
    case 3:
block_15:
            func_80036028(2);
            a0 = 0;
        } else {
            a0 = 1;
        }
block_16:
        func_800360CC(a0);
        break;
    default:
        break;
    }
}

s32 func_800349F8(s32 arg0, s32 arg1, void (*arg2)(s32), s32 arg3) {
    s32 ret;
    if (arg0 < arg1) {
        ret = 0;
    } else {
        arg2(arg3);
        ret = 1;
    }
    return ret;
}

typedef struct { s32 pad[10]; s32 f28; s32 pad2[8]; } Unk80034A2C;


void func_80034A2C(s32 *arg0) {
    Unk80034A2C *p;

    if (func_80035A94() != 0) {
        D_8005F264[2] = 0;
        func_80036084(2);
        func_800360CC(0);
        p = (Unk80034A2C *)D_8005F264;
        p += arg0[1];
        if (p->f28 != 0) {
            func_80036084(0x80);
            func_800368C0();
            func_80036850();
        }
    }
    func_80035F2C((s32 *)arg0[0]);
}

s32 VSync(s32);
s32 erase(s32);

typedef struct {
    s32 *f0;
    s32 f4;
    s32 *f8;
    s32 *fC;
    s32 f10;
} Unk80034AC4;

void func_80034AC4(Unk80034AC4 *arg0) {
    s32 i;
    s32 r;

    if (*arg0->fC != 0) {
        *arg0->fC = 0;
        func_800369F4();
    }
    if (func_80035A94() != 0) {
        func_80036084(2);
        func_800360CC(0);
    }
    func_80035F2C(arg0->f0);
    if (*arg0->f8 != 0) {
        VSync(0x3C);
        for (i = 0; i < 9; i++) {
            r = erase(arg0->f10);
            VSync(0x14);
            if (r == 1) {
                break;
            }
        }
        if (i < 3) {
            *arg0->f8 = 0;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034BB8);

#ifdef NON_MATCHING
extern u8 D_8005C4B4[];
extern u8 D_8005C4B5[];

void func_8003516C(u8 *arg0) {
    s32 var_a1;
    s32 var_t1;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;
    u32 temp_v1;
    u8 *var_a0;
    u8 *var_t2;
    u8 temp_a1;
    u8 temp_a3;
    u8 var_a2;

    var_a0 = arg0 + 4;
    var_t2 = var_a0;
    var_t1 = 0;
loop_1:
    temp_a1 = var_a0[0];
    temp_a3 = var_a0[1];
    temp_v1 = temp_a1 & 0xFF;
    var_a2 = temp_a1;
    if (temp_v1 < 0x81U) {
        var_a0 -= 1;
        goto block_20;
    }
    switch (temp_v1) {
    case 0xA0:
        var_a2 = 0x20;
        var_a0 -= 1;
block_20:
        var_v0 = var_a2 & 0xFF;
        break;
    case 0x81:
        var_a2 = 0x2E;
        if (D_8005C4B4[0] != 0xFF) {
            var_a1 = 0;
            var_v1 = 0;
loop_8:
            if (D_8005C4B5[var_v1] != (temp_a3 & 0xFF)) {
                var_a1 = var_v1 + 2;
                var_v1 = var_a1;
                if (D_8005C4B4[var_v1] == 0xFF) {
                    var_v0 = 0x2E & 0xFF;
                } else {
                    goto loop_8;
                }
            } else {
                var_a2 = D_8005C4B4[var_a1];
                var_v0 = var_a2 & 0xFF;
            }
        } else {
            goto block_20;
        }
        break;
    case 0x82:
        var_a2 = 0x3F;
        if (((u32) (temp_a3 - 0x60) < 0x1AU) || (var_v0_2 = temp_a3 + 0x7F, (((u32) (temp_a3 - 0x4F) < 0xAU) != 0))) {
            var_a2 = temp_a3 - 0x1F;
            var_v0_2 = temp_a3 + 0x7F;
        }
        var_v0 = var_a2 & 0xFF;
        if ((u32) (var_v0_2 & 0xFF) < 0x1AU) {
            var_a2 = temp_a3 - 0x20;
            goto block_20;
        }
        break;
    default:
        var_a2 = 0x3F;
        if ((u32) ((temp_a1 + 0x5F) & 0xFF) < 0x3FU) {
            var_a0 -= 1;
        }
        goto block_20;
    }
    if ((var_v0 != 0) && ((u32) ((var_a2 - 0x20) & 0xFF) >= 0x5FU)) {
        var_a2 = 0x20;
    }
    *var_t2 = var_a2;
    var_t2 += 1;
    if (var_a2 & 0xFF) {
        var_t1 += 2;
        var_a0 += 2;
        if (var_t1 < 0x40) {
            goto loop_1;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003516C);
#endif

#ifdef NON_MATCHING
void func_800352D8(s8 *arg0, s32 arg1) {
    char buf[0x40];
    s16 *out;
    u8 *in;
    u8 *p;
    s32 done;
    s32 i;
    s32 j;
    u32 c;
    u8 t;

    done = 0;
    strcpy(buf, arg0);
    out = (s16 *)arg0;
    in = (u8 *)buf;
    for (i = 0; i < arg1; i++) {
        c = *in;
        if (done != 0) {
            *out = 0x8140;
        } else if (c - 0x61 < 0x1A) {
            *out = c - 0x7DE0;
        } else if (c - 0x41 < 0x1A || c - 0x30 < 0xA) {
            *out = c - 0x7DE1;
        } else {
            *out = 0x8140;
            if (D_8005C4B4[0] != 0xFF) {
                p = D_8005C4B4;
                j = 0;
                while (1) {
                    if (*p == (c & 0xFF)) {
                        *out = D_8005C4B4[j + 1] | 0x8100;
                        break;
                    }
                    p += 2;
                    if (*p == 0xFF) break;
                    j += 2;
                }
            }
            if (c == 0) {
                done = 1;
                *out = 0;
            }
        }
        in++;
        out++;
    }
    for (i = 0, p = (u8 *)arg0; i < arg1; i++, p += 2) {
        t = p[0];
        p[0] = p[1];
        p[1] = t;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800352D8);
#endif

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035444);

extern volatile u32 D_8003BEA4;
extern char D_8005F274[];
s32 format(char *);

void func_800359F0(void) {
    s32 r;
    D_8003BEA4;
    r = format(D_8005F274);
    D_8003BEA4;
    if (r == 1) {
        func_800360AC(2);
        func_80036084(4);
        func_800360CC(0);
    } else if (func_80035A94() != 0) {
        func_80036084(2);
        func_800360CC(0);
    }
}

void func_80035A64(void) {
}

void func_80035A6C(void) {
}

void func_80035A74(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035A7C);

s32 func_80035A94(void) {
    s32 *p = D_8005F264;
    s32 n = p[5];
    p[5] = n + 1;
    return n >= p[4];
}

s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;

s32 func_80035AB8(void) {
    func_80031578(0x3E7);
    if (TestEvent(D_8005F3A0) == 1) {
        return 0;
    }
    if (TestEvent(D_8005F3A4) == 1) {
        return 1;
    }
    if (TestEvent(D_8005F3A8) == 1) {
        return 2;
    }
    if (TestEvent(D_8005F3AC) != 1) {
        return -1;
    }
    func_80036084(0x10);
    return 3;
}

s32 func_80035B3C(void) {
    s32 result;

    do {
        result = func_80035AB8();
    } while (result == -1);
    return result;
}


void func_80035B70(void) {
    TestEvent(D_8005F3A0);
    TestEvent(D_8005F3A4);
    TestEvent(D_8005F3A8);
    TestEvent(D_8005F3AC);
}

s32 D_8005F3BC;
s32 D_8005F3C0;
s32 D_8005F3C8;
s32 D_8005F3CC;

s32 func_80035BB8(void) {
    func_80031578(0x3E7);
    if (TestEvent(D_8005F3BC) == 1) {
        return 0;
    }
    if (TestEvent(D_8005F3C0) == 1) {
        return 1;
    }
    if (TestEvent(D_8005F3C8) == 1) {
        return 2;
    }
    if (TestEvent(D_8005F3CC) == 1) {
        return 3;
    }
    return -1;
}

s32 func_80035C40(void) {
    s32 result;

    do {
        result = func_80035BB8();
    } while (result == -1);
    return result;
}


void func_80035C74(void) {
    TestEvent(D_8005F3BC);
    TestEvent(D_8005F3C0);
    TestEvent(D_8005F3C8);
    TestEvent(D_8005F3CC);
}

#ifdef NON_MATCHING
#define MAX_DIR_ENTRIES 16
#define DIR_ENTRY_SIZE 0x28

extern char D_8005F284[];
s32 firstfile(char *, void *);
s32 nextfile(void *);
char *strcat(char *, const char *);

s32 func_80035CBC(char *path, void *entries) {
    char pattern[0x80];
    char *entry;
    s32 count;

    entry = entries;
    strcpy(pattern, path);
    strcat(pattern, D_8005F284);
    if ((void *)firstfile(pattern, entry) != entry) {
        return 0;
    }
    count = 1;
    while (1) {
        entry += DIR_ENTRY_SIZE;
        if (count >= MAX_DIR_ENTRIES) {
            break;
        }
        count++;
        if ((void *)nextfile(entry) != entry) {
            count--;
            break;
        }
    }
    return count;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035CBC);
#endif

s32 strcmp(char *, char *);


void func_80035D48(Unk80035D48 *arg0, s32 arg1) {
    Unk80035D48 tmp;
    s32 i;
    s32 j;
    s32 first;
    s32 r;
    char *a;

    for (i = 0; i < arg1; i++) {
        a = arg0[i].b;
        first = func_80035F00(a);
        for (j = i + 1; j < arg1; j++) {
            r = func_80035F00(arg0[j].b);
            if (first != 0 && (r == 0 || strcmp(a, arg0[j].b) > 0)) {
                func_8003B940(&arg0[i], &tmp, 0x14);
                func_8003B940(&arg0[j], &arg0[i], 0x14);
                func_8003B940(&tmp, &arg0[j], 0x14);
                i--;
                break;
            }
        }
    }
}

void func_80035E7C(void) {
    s32 i;
    s32 n;

    if (D_8005F264[2] >= 15) {
        n = D_8005F264[8];
        for (i = 0; i < n; i++) {
            if (func_80035F00((char *)D_8005F264 + 0x49C + i * 0x28) != 0) {
                return;
            }
        }
        func_80036084(0x40);
    }
}

extern char D_8001069C[];

s32 func_80035F00(char *arg0) {
    return strncmp(D_8001069C, arg0, 12) == 0;
}

void func_80035F2C(s32 *arg0) {
    s32 temp_a0;

    temp_a0 = *arg0;
    if (temp_a0 >= 0) {
        close(temp_a0);
    }
    *arg0 = -1;
}

extern volatile u32 D_8003BEA4;

void func_80035F70(u32 arg0) {
    u32 start;

    start = D_8003BEA4;
    while (start - D_8003BEA4 < arg0) {
    }
}

void func_80035F9C(u8 *arg0) {
    while (*arg0 != 0) {
        if ((u8)(*arg0 - 0x61) < 0x1A) {
            *arg0 = *arg0 - 0x20;
        }
        arg0++;
    }
}

s32 func_80035FD8(void) {
    if (!(func_80036150() & 0xD)) {
        func_800368C0();
        func_800360CC(1);
        func_80035A7C(0x40);
        D_8005F264[7] = 0;
        return 0;
    }
    return 1;
}

s32 func_80036028(s32 arg0) {
    s32 *p = D_8005F264;

    p[2] = 0;
    p[5] = 0;
    func_80036084(arg0);
    return 0;
}

void func_80036050(void) {
    s8 *p;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        if (D_8005F264 != 0) {
            p = ((i * 0x4C) + ((s8 *)D_8005F264)) + 0x34;
            *p = 0;
        }
    }
}

extern s32 D_8006CC5C;

s32 func_80036084(s32 a0) {
    s32 *p = D_8005F264;
    s32 v = p[1] | a0;
    p[1] = v;
    D_8006CC5C = v;
    return 0;
}

s32 func_800360AC(s32 arg0) {
    D_8005F264[1] &= ~arg0;
    return 0;
}

s32 func_800360CC(s32 arg0) {
    s32 *p = D_8005F264;
    *p = arg0;
    if (arg0 >= 0) {
        if (arg0 < 2 || arg0 == 4) {
            p[6] = 1;
        }
        if (*D_8005F264 == 0) {
            func_800360AC(1);
        } else {
            func_80036084(1);
        }
    } else {
        func_80036084(1);
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003613C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036150);

void EnableEvent(s32);
void InitCARD(s32);
s32 OpenEvent(s32, s32, s32, s32);
void StartCARD(void);
void _bu_init(void);

void func_80036164(void) {
    EnterCriticalSection();
    D_8005F3A0 = OpenEvent(0xF4000001, 4, 0x2000, 0);
    D_8005F3A4 = OpenEvent(0xF4000001, 0x8000, 0x2000, 0);
    D_8005F3A8 = OpenEvent(0xF4000001, 0x100, 0x2000, 0);
    D_8005F3AC = OpenEvent(0xF4000001, 0x2000, 0x2000, 0);
    D_8005F3BC = OpenEvent(0xF0000011, 4, 0x2000, 0);
    D_8005F3C0 = OpenEvent(0xF0000011, 0x8000, 0x2000, 0);
    D_8005F3C8 = OpenEvent(0xF0000011, 0x100, 0x2000, 0);
    D_8005F3CC = OpenEvent(0xF0000011, 0x2000, 0x2000, 0);
    ExitCriticalSection();
    VSync(5);
    InitCARD(1);
    VSync(5);
    StartCARD();
    VSync(5);
    _bu_init();
    VSync(5);
    EnableEvent(D_8005F3A0);
    EnableEvent(D_8005F3A4);
    EnableEvent(D_8005F3A8);
    EnableEvent(D_8005F3AC);
    EnableEvent(D_8005F3BC);
    EnableEvent(D_8005F3C0);
    EnableEvent(D_8005F3C8);
    EnableEvent(D_8005F3CC);
}

s32 func_80036304(void) {
    StopCARD();
    return 0;
}

void func_80036324(s32 arg0) {
    func_80036028(0);
    func_800360CC(0);
    func_800360AC(0xDF);
    func_80036084(arg0);
}

extern s32 *D_8006CCE8;

s32 func_80036368(void) {
    s32 *p;

    p = func_80011764(1, 0x11F4);
    D_8005F264 = p;
    D_8006CCE8 = p + 0x1BD;
    if (D_8005F268 != 0) {
        func_80036084(0x10);
    } else {
        func_800360AC(0x10);
    }
    func_80036324(4);
    func_800368C0();
    return 0;
}

s32 func_800363D0(void) {
    u32 v = func_80036150();
    int has_ptr;
    do { } while (0); has_ptr = D_8005F264 != 0;
    D_8005F268 = (v >> 4) & 1;
    if (has_ptr) {
        func_800116C0((s32)D_8005F264);
        D_8005F264 = 0;
    }
    return 0;
}

#ifdef NON_MATCHING
s32 _card_read(s32, s32, u8 *);

s32 func_80036414(s32 arg0) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    func_80035C74();
    _new_card();
    _card_read(arg0, 0, buf);
    if (func_80035C40() == 0) {
        if (buf[0] == 0x4D) {
            return buf[1] == 0x43;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036414);
#endif

s32 func_8003649C(s32 arg0) {
    s32 i;
    s32 *p;

    if (func_80036150() & 0xD) {
        func_80036084(2);
        return 1;
    }
    i = 0;
    p = D_8005F264;
loop:
    if (*(s32 *)((s8 *)p - -(i * 0x4C) + 0x28) == 0) {
        i++;
        if (i < p[2]) {
            goto loop;
        }
        i = p[2];
    }
    func_800360CC(6);
    func_80035A7C(0x20);
    D_8005F264[3] = i;
    return 0;
}

s32 func_8003653C(void) {
    s32 v;
    if (D_8005F264 != 0) {
        v = func_80036150() & 0x10;
    } else {
        v = D_8005F268;
    }
    if (v == 0) {
        func_80035B70();
        _card_info(0);
        return func_80035B3C() != 0;
    }
    return 1;
}

#ifdef NON_MATCHING
extern char D_8005C500[];


s32 func_80036598(s32 arg0) {
    return strcmp(D_8005C500, (char *)(arg0 * 0x2A4 + (int)D_8005F264) + 0x700) == 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036598);
#endif

#ifdef NON_MATCHING
s32 func_800365E4(void) {
    func_80035B70();
    _card_info(0);
    if (func_80035B3C() == 2) {
        return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800365E4);
#endif

s32 func_80036628(void) {
    if (func_8003613C() != 0) {
        func_80036084(2);
        return 1;
    }
    func_800360CC(3);
    func_80035A7C(3);
    return 0;
}

s32 func_80036674(void) {
    s32 ret;
    if (D_8005F264 != 0) {
        func_80036324((func_80036150() & 0x10) | 4);
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 func_800366B4(void) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    buf[1] = 0xFF;
    buf[0] = 0xFF;
    func_80035C74();
    _new_card();
    _card_write(0, 0, buf);
    return -(func_80035C40() != 0);
}

s32 func_8003670C(u32 arg0, s32 arg1) {
    switch (arg0) {
    case 2:
        func_800360AC(0x80);
        return 1;
    case 3:
        return func_8003649C(arg1);
    case 5:
        return func_80036628();
    case 7:
        return func_8003653C();
    case 8:
        return func_800365E4();
    case 10:
        return func_80036674();
    case 12:
        return func_800366B4();
    case 13:
        return func_80036AC8(arg1);
    case 14:
        return func_80036C40(arg1);
    case 15:
        if (D_8005F264 != 0) {
            func_8003690C((char *)D_8005F264 + (arg1 * 0x2A4 + 0x6F4));
            func_80036850();
            return 0;
        }
        return 1;
    case 16:
        return func_80036B68(arg1);
    case 17:
        return func_80036BD8(arg1);
    case 18:
        return func_80036598(arg1);
    case 0:
    default:
        return 1;
    }
}

extern s32 D_8006CCB0;

void func_80036850(void) {
    s32 i;
    char *p;

    for (i = 0; i < 4; i++) {
        p = (char *)D_8005F264 + (i * 0x2A4 + 0x6F4) + 0xC;
        func_800175D8(D_8006CCB0, i << 8, p, 0x28, 1);
    }
}

void func_800368C0(void) {
    s32 i;
    s32 offset;

    i = 0;
    offset = 0x6F4;
    do {
        i++;
        func_8003690C((s8 *)D_8005F264 + offset);
        offset += 0x2A4;
    } while (i < 4);
}

struct Unk8005F288 {
    s32 a;
    s16 b;
};
struct Unk8003690C {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s16 f10;
    s16 pad;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 pad2[8];
    s32 f50;
    s32 f54;
    s32 f58;
    s32 f5C;
};

void func_8003690C(void *p)
{
  struct Unk8003690C *o = p;
  s32 u;
  memset(o, 0, 0x2A4);
  o->f0 = 8;
  do { o->f4 = 1; u = ((struct Unk8005F288 *)D_8005F288)->b; o->fC = ((struct Unk8005F288 *)D_8005F288)->a; o->f10 = u; } while (0);
  o->f20 = 0x63;
  o->f2C = 5;
  o->f58 = 0xFF;
  o->f5C = 0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036978);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800369F4);

void func_80036A84(void) {
}

s32 func_80036A8C(s32 *arg0, s32 arg1) {
    s32 sum;
    s32 i;
    s32 pad[2];

    sum = 0x12345678;
    i = arg1 - 1;
    if (arg1 != 0) {
        do {
            sum += *arg0++;
            i--;
        } while (i != -1);
    }
    return sum;
}

extern s32 D_80069718[];

s32 func_80036AC8(s32 arg0) {
    s32 *a1;
    s32 *a2;
    s32 *v1;
    s32 a0;
    s32 a3;
    char *base;

    if (D_8005F264 == 0) {
        return 1;
    }
    base = (char *)D_8005F264 + (arg0 * 0x2A4 + 0x6F4);
    *(s32 *)(base + 4) = 0;
    a1 = (s32 *)(base + 0x60);
    a3 = 0;
    a2 = D_80069718;
    do {
        a0 = 0;
        v1 = a2;
        do {
            *a1++ = *v1++;
            a0++;
        } while (a0 < 2);
        a3++;
        a2 += 2;
    } while (a3 < 0x40);
    func_80036850();
    return 0;
}

extern char D_8005C500[];
extern char *D_8006CBCC;

s32 func_80036B68(s32 arg0) {
    s32 ret;
    char *src;
    if (D_8005F264 != 0) {
        src = (char *)(arg0 * 0x2A4 + (s32)D_8005F264 + 0x700);
        strcpy(D_8005C500, src);
        D_8006CBCC = D_8005C500;
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 func_80036BD8(s32 arg0) {
    s32 ret;

    if (D_8005F264 != 0) {
        strcpy((char *)(arg0 * 0x2A4 + (s32)D_8005F264 + 0x700), D_8005C500);
        func_80036850();
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 func_80036C40(s32 arg0) {
    s32 *src;
    s32 *row;
    s32 *dst;
    s32 i;
    s32 j;
    char *base;

    if (D_8005F264 == 0) {
        return 1;
    }
    base = (char *)D_8005F264 + (arg0 * 0x2A4 + 0x6F4);
    src = (s32 *)(base + 0x60);
    i = 0;
    row = D_80069718;
    src = (s32 *)(base + 0x60);
    do {
        j = 0;
        dst = row;
        do {
            *dst = *src;
            src++;
            j++;
            dst++;
        } while (j < 2);
        i++;
        row += 2;
    } while (i < 0x40);
    strcpy(D_8005C500, base + 0xC);
    D_8006CBCC = D_8005C500;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036CF8);
