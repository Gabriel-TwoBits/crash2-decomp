#include "common.h"
#include "sdk.h"

INCLUDE_ASM("asm/nonmatchings/game", func_800111B0);

INCLUDE_ASM("asm/nonmatchings/game", func_800112A4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001144C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001148C);

INCLUDE_ASM("asm/nonmatchings/game", func_800116C0);

INCLUDE_ASM("asm/nonmatchings/game", func_800116D4);

INCLUDE_ASM("asm/nonmatchings/game", func_80011764);

INCLUDE_ASM("asm/nonmatchings/game", func_800117BC);

INCLUDE_ASM("asm/nonmatchings/game", func_80011800);

INCLUDE_ASM("asm/nonmatchings/game", func_80011E24);

void func_80011E80(void) {
}

void func_80011E88(void) {
}

void func_80011E90(void) {
}

extern char D_8005F3F4[];
extern char D_8005F198[];
extern unsigned char D_8005B198[];

/* An odd value becomes five characters of D_8005B198's 64, six bits each from the top, the low bit dropped; an even one is
   printed with the format at D_8005F198. Returns the static buffer */
char *func_80011E98(unsigned int value)
{
    int i;

    if (value & 1) {
        D_8005F3F4[5] = 0;
        value >>= 1;
        for (i = 4; i >= 0; i--) {
            D_8005F3F4[i] = D_8005B198[value & 0x3F];
            value >>= 6;
        }
    } else {
        sprintf(D_8005F3F4, D_8005F198, value);
    }
    return D_8005F3F4;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80011F1C);

INCLUDE_ASM("asm/nonmatchings/game", func_80011F78);

INCLUDE_ASM("asm/nonmatchings/game", func_80012010);

INCLUDE_ASM("asm/nonmatchings/game", func_800120EC);

INCLUDE_ASM("asm/nonmatchings/game", func_80012100);

INCLUDE_ASM("asm/nonmatchings/game", func_8001214C);

INCLUDE_ASM("asm/nonmatchings/game", func_80012170);

INCLUDE_ASM("asm/nonmatchings/game", func_80012250);

INCLUDE_ASM("asm/nonmatchings/game", func_80012294);

INCLUDE_ASM("asm/nonmatchings/game", func_8001232C);

INCLUDE_ASM("asm/nonmatchings/game", func_800123BC);

INCLUDE_ASM("asm/nonmatchings/game", func_800123F0);

INCLUDE_ASM("asm/nonmatchings/game", func_8001254C);

INCLUDE_ASM("asm/nonmatchings/game", func_800125F8);

INCLUDE_ASM("asm/nonmatchings/game", func_80012A20);

INCLUDE_ASM("asm/nonmatchings/game", func_80012BC4);

INCLUDE_ASM("asm/nonmatchings/game", func_80012DC0);

INCLUDE_ASM("asm/nonmatchings/game", func_80012E4C);

INCLUDE_ASM("asm/nonmatchings/game", func_80012ED0);

INCLUDE_ASM("asm/nonmatchings/game", func_80012F24);

typedef struct {
    u8 pad0[0x14];
    s32 unk14;
} Unk80013038Sub;

typedef struct {
    u8 pad0[0x1C];
    s32 unk1C;
    u8 pad20[0x4];
    Unk80013038Sub *unk24;
} Unk80013038;

s32 func_80013038(Unk80013038 *arg0, s32 arg1) {
    s32 temp;

    if (arg1 == -1) {
        return 1;
    }
    arg1 >>= 1;
    temp = arg0->unk24->unk14 >> 1;
    if (temp >= arg1) {
        if (temp - arg0->unk1C < arg1) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80013084);

INCLUDE_ASM("asm/nonmatchings/game", func_800130A0);

INCLUDE_ASM("asm/nonmatchings/game", func_80013200);

INCLUDE_ASM("asm/nonmatchings/game", func_800136C8);

INCLUDE_ASM("asm/nonmatchings/game", func_80013888);

INCLUDE_ASM("asm/nonmatchings/game", func_80013968);

INCLUDE_ASM("asm/nonmatchings/game", func_80013A54);

INCLUDE_ASM("asm/nonmatchings/game", func_80013AE8);

INCLUDE_ASM("asm/nonmatchings/game", func_80013BD8);

INCLUDE_ASM("asm/nonmatchings/game", func_80013C98);

void func_80013F28(void);
extern s32 D_80067FB0[];

void func_80013E10(void *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = *(s8 *)((char *)arg0 + 0xF);
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        *(s8 *)((char *)arg0 + 0xF) = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (*(s16 *)((char *)arg0 + 6) == 0)) {
            func_80013F28();
            D_80067FB0[0] -= 1;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game", func_80013E7C);

INCLUDE_ASM("asm/nonmatchings/game", func_80013EB0);

INCLUDE_ASM("asm/nonmatchings/game", func_80013F28);

INCLUDE_ASM("asm/nonmatchings/game", func_80013F5C);

INCLUDE_ASM("asm/nonmatchings/game", func_80013FD8);

INCLUDE_ASM("asm/nonmatchings/game", func_80014078);

INCLUDE_ASM("asm/nonmatchings/game", func_80014148);

INCLUDE_ASM("asm/nonmatchings/game", func_80014220);

INCLUDE_ASM("asm/nonmatchings/game", func_80014260);

INCLUDE_ASM("asm/nonmatchings/game", func_8001434C);

INCLUDE_ASM("asm/nonmatchings/game", func_800143C0);

INCLUDE_ASM("asm/nonmatchings/game", func_80014420);

INCLUDE_ASM("asm/nonmatchings/game", func_8001456C);

INCLUDE_ASM("asm/nonmatchings/game", func_80014638);

INCLUDE_ASM("asm/nonmatchings/game", func_80014790);

INCLUDE_ASM("asm/nonmatchings/game", func_80014918);

INCLUDE_ASM("asm/nonmatchings/game", func_80014B44);

INCLUDE_ASM("asm/nonmatchings/game", func_80014B90);

INCLUDE_ASM("asm/nonmatchings/game", func_80014C08);

extern s32 D_8005F184;
extern s32 D_8005F188;

s32 func_80014C48(void) {
    s32 a = D_8005F184;
    s32 b = D_8005F188;

    /* The loop notes keep the scheduler from hoisting `lui 0x8000` above the loads. */
    do {
        a -= 0x80000000;
    } while (0);
    return b - a;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80014C68);

INCLUDE_ASM("asm/nonmatchings/game", func_8001521C);

INCLUDE_ASM("asm/nonmatchings/game", func_80015344);

INCLUDE_ASM("asm/nonmatchings/game", func_8001557C);

INCLUDE_ASM("asm/nonmatchings/game", func_800155E0);

INCLUDE_ASM("asm/nonmatchings/game", func_80015614);

INCLUDE_ASM("asm/nonmatchings/game", func_800157A0);

INCLUDE_ASM("asm/nonmatchings/game", func_80015828);

INCLUDE_ASM("asm/nonmatchings/game", func_800158E0);

INCLUDE_ASM("asm/nonmatchings/game", func_8001601C);

INCLUDE_ASM("asm/nonmatchings/game", func_800160B0);

INCLUDE_ASM("asm/nonmatchings/game", func_800161F8);

INCLUDE_ASM("asm/nonmatchings/game", func_800162FC);

INCLUDE_ASM("asm/nonmatchings/game", func_800163B4);

extern void func_800163B4();

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4();
}

INCLUDE_ASM("asm/nonmatchings/game", func_800163E8);

INCLUDE_ASM("asm/nonmatchings/game", func_80016408);

typedef struct {
    u8 pad[0x2088];
    void *unk2088;
    u8 pad2[4];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];

void *func_80016418(s32 arg0) {
    void **temp;

    temp = (void **)D_80063458[0]->unk2090;
    D_80063458[0]->unk2090 = (char *)temp + arg0;
    *temp = D_80063458[0]->unk2088;
    D_80063458[0]->unk2088 = temp;
    return temp;
}

INCLUDE_ASM("asm/nonmatchings/game", func_8001645C);

INCLUDE_ASM("asm/nonmatchings/game", func_80016A60);

INCLUDE_ASM("asm/nonmatchings/game", func_80016B08);

INCLUDE_ASM("asm/nonmatchings/game", func_80016C58);

void func_80016EFC(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_80016F04);

INCLUDE_ASM("asm/nonmatchings/game", func_80016F44);

INCLUDE_ASM("asm/nonmatchings/game", func_800170B4);

INCLUDE_ASM("asm/nonmatchings/game", func_80017198);

void func_80016F44(int, int, int);

void func_800171C0(int a0) {
    func_80016F44(a0, 0, 0x6396347F);
}

INCLUDE_ASM("asm/nonmatchings/game", func_800171E8);

INCLUDE_ASM("asm/nonmatchings/game", func_80017358);

INCLUDE_ASM("asm/nonmatchings/game", func_80017380);

INCLUDE_ASM("asm/nonmatchings/game", func_800175D8);

INCLUDE_ASM("asm/nonmatchings/game", func_800176DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001798C);

INCLUDE_ASM("asm/nonmatchings/game", func_80017AF8);

INCLUDE_ASM("asm/nonmatchings/game", func_80017BC4);

INCLUDE_ASM("asm/nonmatchings/game", func_80018168);

INCLUDE_ASM("asm/nonmatchings/game", func_80018194);

INCLUDE_ASM("asm/nonmatchings/game", func_80018294);

INCLUDE_ASM("asm/nonmatchings/game", func_80018434);

INCLUDE_ASM("asm/nonmatchings/game", func_80018474);

INCLUDE_ASM("asm/nonmatchings/game", func_8001856C);

INCLUDE_ASM("asm/nonmatchings/game", func_800186D8);

INCLUDE_ASM("asm/nonmatchings/game", func_800189C4);

typedef struct Node {
    s32 unk0;
    struct Node *children4; /* the child list's head when unk0 == 2 */
    u8 pad08[0x44 - 0x08];
    struct Node *parent;
    struct Node *next;
    struct Node *children; /* the child list's head otherwise */
} Node;

/* Moves node from its parent's child list to the head of parent's. Returns -0x18 when node is missing from its old
 * parent's list. The success path has no return statement: $v0 is left holding the 2 of the last comparison. */
s32 func_800189CC(Node *node, Node *parent) {
    Node *old;
    Node *it;

    old = node->parent;
    if (old != 0) {
        it = old->unk0 == 2 ? old->children4 : old->children;
        if (it == node) {
            *(old->unk0 == 2 ? &old->children4 : &old->children) = node->next;
        } else {
            while (it != 0 ? it->next != node : 1) {
                if (it == 0) {
                    return -0x18;
                }
                it = it->next;
            }
            it->next = node->next;
        }
    }
    node->parent = parent;
    node->next = parent->unk0 == 2 ? parent->children4 : parent->children;
    *(parent->unk0 == 2 ? &parent->children4 : &parent->children) = node;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80018AA8);

INCLUDE_ASM("asm/nonmatchings/game", func_80018B50);

INCLUDE_ASM("asm/nonmatchings/game", func_80018BE4);

INCLUDE_ASM("asm/nonmatchings/game", func_80018CAC);

INCLUDE_ASM("asm/nonmatchings/game", func_80018D28);

INCLUDE_ASM("asm/nonmatchings/game", func_80018DD0);

INCLUDE_ASM("asm/nonmatchings/game", func_80018E48);

INCLUDE_ASM("asm/nonmatchings/game", func_80018E68);

INCLUDE_ASM("asm/nonmatchings/game", func_80019074);

INCLUDE_ASM("asm/nonmatchings/game", func_80019098);

INCLUDE_ASM("asm/nonmatchings/game", func_80019800);

INCLUDE_ASM("asm/nonmatchings/game", func_80019988);

INCLUDE_ASM("asm/nonmatchings/game", func_80019BBC);

INCLUDE_ASM("asm/nonmatchings/game", func_80019D30);

INCLUDE_ASM("asm/nonmatchings/game", func_80019E08);

INCLUDE_ASM("asm/nonmatchings/game", func_80019E28);

INCLUDE_ASM("asm/nonmatchings/game", func_80019E5C);

INCLUDE_ASM("asm/nonmatchings/game", func_80019F08);

INCLUDE_ASM("asm/nonmatchings/game", func_80019FA0);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A014);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A054);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A13C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A23C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A4A4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A710);

INCLUDE_ASM("asm/nonmatchings/game", func_8001A90C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001AA80);

INCLUDE_ASM("asm/nonmatchings/game", func_8001ABF8);

INCLUDE_ASM("asm/nonmatchings/game", func_8001B83C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001BD84);

INCLUDE_ASM("asm/nonmatchings/game", func_8001BFD4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001BFDC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001C29C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001C340);

INCLUDE_ASM("asm/nonmatchings/game", func_8001C620);

INCLUDE_ASM("asm/nonmatchings/game", func_8001C688);

INCLUDE_ASM("asm/nonmatchings/game", func_8001C718);

INCLUDE_ASM("asm/nonmatchings/game", func_8001CC10);

INCLUDE_ASM("asm/nonmatchings/game", func_8001CEDC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001D194);

INCLUDE_ASM("asm/nonmatchings/game", func_8001D1E0);

INCLUDE_ASM("asm/nonmatchings/game", func_8001D22C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001D2C4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001D63C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E3DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E544);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E5FC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E640);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E780);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E8E4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001E9EC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001EAD8);

INCLUDE_ASM("asm/nonmatchings/game", func_8001EC2C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001ED60);

INCLUDE_ASM("asm/nonmatchings/game", func_8001EE64);

INCLUDE_ASM("asm/nonmatchings/game", func_8001EFD8);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F018);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F054);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F858);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F8B4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F94C);

INCLUDE_ASM("asm/nonmatchings/game", func_8001F9C4);

INCLUDE_ASM("asm/nonmatchings/game", func_8001FDDC);

INCLUDE_ASM("asm/nonmatchings/game", func_8001FF30);

INCLUDE_ASM("asm/nonmatchings/game", func_8001FF50);

INCLUDE_ASM("asm/nonmatchings/game", func_8001FFCC);

extern void func_80019E28();
extern char *D_8005F38C;
extern int D_800608D0;

void func_8002001C(char *arg0) {
    char *s1 = *(char **)(arg0 + 0x10);

    do {
        if (*(unsigned *)(s1 + 0x27C) & 1) {
            func_80019E28();
            if (*(char **)(D_8005F38C + 0x18) == arg0) {
                *(int *)(0x18 + D_8005F38C) = D_800608D0;
            }
            *(unsigned *)(s1 + 0x27C) &= ~3;
        }
    } while (0);
}

INCLUDE_ASM("asm/nonmatchings/game", func_800200A8);

INCLUDE_ASM("asm/nonmatchings/game", func_80020998);

INCLUDE_ASM("asm/nonmatchings/game", func_80020A24);

INCLUDE_ASM("asm/nonmatchings/game", func_800217F8);

INCLUDE_ASM("asm/nonmatchings/game", func_80021900);

INCLUDE_ASM("asm/nonmatchings/game", func_80021A08);

INCLUDE_ASM("asm/nonmatchings/game", func_80021F4C);

INCLUDE_ASM("asm/nonmatchings/game", func_800221DC);

INCLUDE_ASM("asm/nonmatchings/game", func_80022224);

INCLUDE_ASM("asm/nonmatchings/game", func_80022314);

INCLUDE_ASM("asm/nonmatchings/game", func_800224A0);

INCLUDE_ASM("asm/nonmatchings/game", func_800225C0);

INCLUDE_ASM("asm/nonmatchings/game", func_8002271C);

INCLUDE_ASM("asm/nonmatchings/game", func_80022878);

INCLUDE_ASM("asm/nonmatchings/game", func_80022A5C);

INCLUDE_ASM("asm/nonmatchings/game", func_80022E74);

INCLUDE_ASM("asm/nonmatchings/game", func_80022F58);

INCLUDE_ASM("asm/nonmatchings/game", func_80023084);

INCLUDE_ASM("asm/nonmatchings/game", func_80023154);

INCLUDE_ASM("asm/nonmatchings/game", func_800231B0);

INCLUDE_ASM("asm/nonmatchings/game", func_800236DC);

INCLUDE_ASM("asm/nonmatchings/game", func_800237F8);

INCLUDE_ASM("asm/nonmatchings/game", func_80023B04);

INCLUDE_ASM("asm/nonmatchings/game", func_80023D7C);

INCLUDE_ASM("asm/nonmatchings/game", func_80023DC4);

INCLUDE_ASM("asm/nonmatchings/game", func_80023E60);

INCLUDE_ASM("asm/nonmatchings/game", func_80024188);

INCLUDE_ASM("asm/nonmatchings/game", func_800241A8);

s32 func_800322C0(s32, s32, s32, s32);

s32 func_80024268(s32 *p, s32 d, s32 *out) {
    s32 v;
    s32 lim;
    s32 over;
    s32 k;
    k = p[3];
    do {
        v = p[2];
        if (k == 2) {
            v += d;
            lim = (func_800322C0(p[1], 0x4B, 0, 2) - 1) << 8;
            over = v - lim;
            if (over > 0) {
                *out = lim;
                return over;
            }
        } else {
            v -= d;
            if (v < 0) {
                *out = 0;
                return -v;
            }
        }
    } while (0);
    *out = v;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game", func_800242F8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002433C);

INCLUDE_ASM("asm/nonmatchings/game", func_80024658);

INCLUDE_ASM("asm/nonmatchings/game", func_80024A48);

INCLUDE_ASM("asm/nonmatchings/game", func_80024B6C);

INCLUDE_ASM("asm/nonmatchings/game", func_800253B0);

INCLUDE_ASM("asm/nonmatchings/game", func_800254D4);

INCLUDE_ASM("asm/nonmatchings/game", func_80025D18);

INCLUDE_ASM("asm/nonmatchings/game", func_80025FDC);

INCLUDE_ASM("asm/nonmatchings/game", func_8002609C);

INCLUDE_ASM("asm/nonmatchings/game", func_800260F4);

INCLUDE_ASM("asm/nonmatchings/game", func_80026334);

INCLUDE_ASM("asm/nonmatchings/game", func_8002648C);

typedef struct { s32 a, b, c, d; } V;
typedef struct { V v; s32 pad[20]; } Buf;
s32 func_800267C4(Buf *, s32);

s32 func_80026764(s32 arg0, V *arg1) {
    Buf buf;

    if (func_800267C4(&buf, arg0) != 0) {
        *arg1 = buf.v;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game", func_800267C4);

INCLUDE_ASM("asm/nonmatchings/game", func_80026A08);

INCLUDE_ASM("asm/nonmatchings/game", func_80026CA0);

INCLUDE_ASM("asm/nonmatchings/game", func_80026EF4);

INCLUDE_ASM("asm/nonmatchings/game", func_8002706C);

INCLUDE_ASM("asm/nonmatchings/game", func_800270C4);

INCLUDE_ASM("asm/nonmatchings/game", func_8002719C);

INCLUDE_ASM("asm/nonmatchings/game", func_800271D8);

INCLUDE_ASM("asm/nonmatchings/game", func_800272A4);

INCLUDE_ASM("asm/nonmatchings/game", func_80027420);

INCLUDE_ASM("asm/nonmatchings/game", func_80027500);

INCLUDE_ASM("asm/nonmatchings/game", func_80027600);

INCLUDE_ASM("asm/nonmatchings/game", func_80027664);

INCLUDE_ASM("asm/nonmatchings/game", func_800277A8);

INCLUDE_ASM("asm/nonmatchings/game", func_800278F8);

INCLUDE_ASM("asm/nonmatchings/game", func_80027980);

INCLUDE_ASM("asm/nonmatchings/game", func_80027A38);

INCLUDE_ASM("asm/nonmatchings/game", func_80027B9C);

INCLUDE_ASM("asm/nonmatchings/game", func_80027C00);

INCLUDE_ASM("asm/nonmatchings/game", func_80027CD0);

INCLUDE_ASM("asm/nonmatchings/game", func_80027DEC);

INCLUDE_ASM("asm/nonmatchings/game", func_80027F3C);

INCLUDE_ASM("asm/nonmatchings/game", func_80028750);

INCLUDE_ASM("asm/nonmatchings/game", func_8002890C);

INCLUDE_ASM("asm/nonmatchings/game", func_80028968);

INCLUDE_ASM("asm/nonmatchings/game", func_800289C4);

INCLUDE_ASM("asm/nonmatchings/game", func_80028A28);

INCLUDE_ASM("asm/nonmatchings/game", func_80028C48);

INCLUDE_ASM("asm/nonmatchings/game", func_80028FE0);

INCLUDE_ASM("asm/nonmatchings/game", func_800294B4);

INCLUDE_ASM("asm/nonmatchings/game", func_80029538);

INCLUDE_ASM("asm/nonmatchings/game", func_8002973C);

INCLUDE_ASM("asm/nonmatchings/game", func_80029768);

extern void func_800116C0(s32);
extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_800297C8(void) {
    func_800116C0(D_8005F3D0);
    func_800116C0(D_8005F398);
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80029800);

INCLUDE_ASM("asm/nonmatchings/game", func_80029884);

INCLUDE_ASM("asm/nonmatchings/game", func_80029D0C);

INCLUDE_ASM("asm/nonmatchings/game", func_8002A1B8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002A3B0);

INCLUDE_ASM("asm/nonmatchings/game", func_8002A554);

INCLUDE_ASM("asm/nonmatchings/game", func_8002A8D0);

INCLUDE_ASM("asm/nonmatchings/game", func_8002B164);

INCLUDE_ASM("asm/nonmatchings/game", func_8002B57C);

INCLUDE_ASM("asm/nonmatchings/game", func_8002BC38);

INCLUDE_ASM("asm/nonmatchings/game", func_8002BF58);

INCLUDE_ASM("asm/nonmatchings/game", func_8002C0F0);

INCLUDE_ASM("asm/nonmatchings/game", func_8002C324);

INCLUDE_ASM("asm/nonmatchings/game", func_8002CA1C);

extern char D_8006CB68[];
extern s32 D_8006CB6C;

void func_8002CA98(void) {
    *(char **)D_8006CB68 = D_8006CB68 + 0x414;
    D_8006CB6C = 0;
}

INCLUDE_ASM("asm/nonmatchings/game", func_8002CAB8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002CB14);

INCLUDE_ASM("asm/nonmatchings/game", func_8002CB1C);

INCLUDE_ASM("asm/nonmatchings/game", func_8002CB68);

INCLUDE_ASM("asm/nonmatchings/game", func_8002CC50);

INCLUDE_ASM("asm/nonmatchings/game", func_8002D6E4);

INCLUDE_ASM("asm/nonmatchings/game", func_8002DE08);

INCLUDE_ASM("asm/nonmatchings/game", func_8002DF68);

INCLUDE_ASM("asm/nonmatchings/game", func_8002DFA8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002E280);

INCLUDE_ASM("asm/nonmatchings/game", func_8002E628);

INCLUDE_ASM("asm/nonmatchings/game", func_8002E6E0);

INCLUDE_ASM("asm/nonmatchings/game", func_8002E854);

INCLUDE_ASM("asm/nonmatchings/game", func_8002E884);

INCLUDE_ASM("asm/nonmatchings/game", func_8002EE48);

INCLUDE_ASM("asm/nonmatchings/game", func_8002EFE4);

INCLUDE_ASM("asm/nonmatchings/game", func_8002F0DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8002F228);

INCLUDE_ASM("asm/nonmatchings/game", func_8002F34C);

INCLUDE_ASM("asm/nonmatchings/game", func_8002F6C8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FB84);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FC78);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FCD0);

void func_8002FCE8(void) {
}

void func_8002FCF0(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_8002FCF8);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FDCC);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FE58);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FF54);

INCLUDE_ASM("asm/nonmatchings/game", func_8002FFFC);

INCLUDE_ASM("asm/nonmatchings/game", func_80030498);

INCLUDE_ASM("asm/nonmatchings/game", func_80030680);

INCLUDE_ASM("asm/nonmatchings/game", func_800309A8);

INCLUDE_ASM("asm/nonmatchings/game", func_80030CD8);

INCLUDE_ASM("asm/nonmatchings/game", func_80030D74);

INCLUDE_ASM("asm/nonmatchings/game", func_80030D98);

INCLUDE_ASM("asm/nonmatchings/game", func_80030E1C);

INCLUDE_ASM("asm/nonmatchings/game", func_80030EC0);

INCLUDE_ASM("asm/nonmatchings/game", func_80030F58);

s32 func_80030FEC(s32 *arg0, s32 *arg1) {
    s32 d[3];
    s32 x, y, z;

    x = arg0[0] - arg1[0];
    if (x < 0) x = arg1[0] - arg0[0];
    d[0] = x;
    y = arg0[1] - arg1[1];
    if (y < 0) y = arg1[1] - arg0[1];
    d[1] = y;
    z = arg0[2] - arg1[2];
    if (z < 0) z = arg1[2] - arg0[2];
    d[2] = z;
    if (x >= y && x >= z) return x + (y + z) / 4;
    if (d[1] >= d[0] && d[1] >= d[2]) return d[1] + (d[0] + d[2]) / 4;
    return d[2] + (d[1] + d[0]) / 4;
}

INCLUDE_ASM("asm/nonmatchings/game", func_800310EC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003118C);

INCLUDE_ASM("asm/nonmatchings/game", func_80031208);

/* libgte */
long ratan2(long y, long x);

/* Angle (12-bit, 0x1000 per turn) from arg0 to arg1 on the plane of words 0 and 2 */
s32 func_80031264(s32 *arg0, s32 *arg1) {
    return ratan2(arg1[0] - arg0[0], arg1[2] - arg0[2]) & 0xFFF;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80031298);

INCLUDE_ASM("asm/nonmatchings/game", func_800312CC);

s32 func_800312F0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 d;

    d = (s32)((arg1 - arg0) << 20) >> 20;
    if (d > 0 && arg3 < d) {
        goto clamp;
    }
    if (d < 0) {
        arg3 = -arg3;
        if (d < arg3) {
clamp:
            d = arg3;
        }
    }
    return (arg0 + ((arg2 * d) >> 8)) & 0xFFF;
}

INCLUDE_ASM("asm/nonmatchings/game", func_80031340);

INCLUDE_ASM("asm/nonmatchings/game", func_8003145C);

void func_80031474(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_8003147C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003155C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003156C);

INCLUDE_ASM("asm/nonmatchings/game", func_80031578);

INCLUDE_ASM("asm/nonmatchings/game", func_80031658);

INCLUDE_ASM("asm/nonmatchings/game", func_80031668);

INCLUDE_ASM("asm/nonmatchings/game", func_80031680);

INCLUDE_ASM("asm/nonmatchings/game", func_800316EC);

INCLUDE_ASM("asm/nonmatchings/game", func_80031AE8);

INCLUDE_ASM("asm/nonmatchings/game", func_800321D4);

INCLUDE_ASM("asm/nonmatchings/game", func_800322C0);

INCLUDE_ASM("asm/nonmatchings/game", func_8003230C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032314);

INCLUDE_ASM("asm/nonmatchings/game", func_8003231C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032324);

INCLUDE_ASM("asm/nonmatchings/game", func_8003232C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032334);

INCLUDE_ASM("asm/nonmatchings/game", func_8003233C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032344);

INCLUDE_ASM("asm/nonmatchings/game", func_8003234C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032354);

INCLUDE_ASM("asm/nonmatchings/game", func_8003235C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032364);

INCLUDE_ASM("asm/nonmatchings/game", func_8003236C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032374);

INCLUDE_ASM("asm/nonmatchings/game", func_8003237C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032384);

INCLUDE_ASM("asm/nonmatchings/game", func_8003238C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032394);

INCLUDE_ASM("asm/nonmatchings/game", func_8003239C);

INCLUDE_ASM("asm/nonmatchings/game", func_800323A4);

INCLUDE_ASM("asm/nonmatchings/game", func_800323AC);

void func_80032498(void) {
}

void func_800324A0(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_800324A8);

INCLUDE_ASM("asm/nonmatchings/game", func_80032588);

INCLUDE_ASM("asm/nonmatchings/game", func_800325D0);

INCLUDE_ASM("asm/nonmatchings/game", func_800325DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003264C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032808);

INCLUDE_ASM("asm/nonmatchings/game", func_800328D4);

INCLUDE_ASM("asm/nonmatchings/game", func_80032A98);

INCLUDE_ASM("asm/nonmatchings/game", func_80032B7C);

INCLUDE_ASM("asm/nonmatchings/game", func_80032F18);

INCLUDE_ASM("asm/nonmatchings/game", func_80033448);

INCLUDE_ASM("asm/nonmatchings/game", func_800334A0);

INCLUDE_ASM("asm/nonmatchings/game", func_80033C84);

INCLUDE_ASM("asm/nonmatchings/game", func_80033D10);

INCLUDE_ASM("asm/nonmatchings/game", func_80033DD0);

INCLUDE_ASM("asm/nonmatchings/game", func_80033DE0);

INCLUDE_ASM("asm/nonmatchings/game", func_80033EE0);

INCLUDE_ASM("asm/nonmatchings/game", func_80034238);

INCLUDE_ASM("asm/nonmatchings/game", func_80034254);

INCLUDE_ASM("asm/nonmatchings/game", func_80034394);

INCLUDE_ASM("asm/nonmatchings/game", func_800343CC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003444C);

INCLUDE_ASM("asm/nonmatchings/game", func_800344B0);

INCLUDE_ASM("asm/nonmatchings/game", func_800345C8);

INCLUDE_ASM("asm/nonmatchings/game", func_800346E8);

INCLUDE_ASM("asm/nonmatchings/game", func_80034810);

INCLUDE_ASM("asm/nonmatchings/game", func_80034898);

INCLUDE_ASM("asm/nonmatchings/game", func_800349F8);

INCLUDE_ASM("asm/nonmatchings/game", func_80034A2C);

INCLUDE_ASM("asm/nonmatchings/game", func_80034AC4);

INCLUDE_ASM("asm/nonmatchings/game", func_80034BB8);

INCLUDE_ASM("asm/nonmatchings/game", func_8003516C);

INCLUDE_ASM("asm/nonmatchings/game", func_800352D8);

INCLUDE_ASM("asm/nonmatchings/game", func_80035444);

INCLUDE_ASM("asm/nonmatchings/game", func_800359F0);

void func_80035A64(void) {
}

void func_80035A6C(void) {
}

void func_80035A74(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_80035A7C);

INCLUDE_ASM("asm/nonmatchings/game", func_80035A94);

INCLUDE_ASM("asm/nonmatchings/game", func_80035AB8);

INCLUDE_ASM("asm/nonmatchings/game", func_80035B3C);

INCLUDE_ASM("asm/nonmatchings/game", func_80035B70);

INCLUDE_ASM("asm/nonmatchings/game", func_80035BB8);

INCLUDE_ASM("asm/nonmatchings/game", func_80035C40);

INCLUDE_ASM("asm/nonmatchings/game", func_80035C74);

INCLUDE_ASM("asm/nonmatchings/game", func_80035CBC);

INCLUDE_ASM("asm/nonmatchings/game", func_80035D48);

INCLUDE_ASM("asm/nonmatchings/game", func_80035E7C);

INCLUDE_ASM("asm/nonmatchings/game", func_80035F00);

INCLUDE_ASM("asm/nonmatchings/game", func_80035F2C);

INCLUDE_ASM("asm/nonmatchings/game", func_80035F70);

INCLUDE_ASM("asm/nonmatchings/game", func_80035F9C);

INCLUDE_ASM("asm/nonmatchings/game", func_80035FD8);

INCLUDE_ASM("asm/nonmatchings/game", func_80036028);

INCLUDE_ASM("asm/nonmatchings/game", func_80036050);

INCLUDE_ASM("asm/nonmatchings/game", func_80036084);

INCLUDE_ASM("asm/nonmatchings/game", func_800360AC);

INCLUDE_ASM("asm/nonmatchings/game", func_800360CC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003613C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036150);

INCLUDE_ASM("asm/nonmatchings/game", func_80036164);

INCLUDE_ASM("asm/nonmatchings/game", func_80036304);

INCLUDE_ASM("asm/nonmatchings/game", func_80036324);

INCLUDE_ASM("asm/nonmatchings/game", func_80036368);

INCLUDE_ASM("asm/nonmatchings/game", func_800363D0);

INCLUDE_ASM("asm/nonmatchings/game", func_80036414);

INCLUDE_ASM("asm/nonmatchings/game", func_8003649C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003653C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036598);

INCLUDE_ASM("asm/nonmatchings/game", func_800365E4);

INCLUDE_ASM("asm/nonmatchings/game", func_80036628);

INCLUDE_ASM("asm/nonmatchings/game", func_80036674);

INCLUDE_ASM("asm/nonmatchings/game", func_800366B4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003670C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036850);

INCLUDE_ASM("asm/nonmatchings/game", func_800368C0);

INCLUDE_ASM("asm/nonmatchings/game", func_8003690C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036978);

INCLUDE_ASM("asm/nonmatchings/game", func_800369F4);

void func_80036A84(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_80036A8C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036AC8);

INCLUDE_ASM("asm/nonmatchings/game", func_80036B68);

INCLUDE_ASM("asm/nonmatchings/game", func_80036BD8);

INCLUDE_ASM("asm/nonmatchings/game", func_80036C40);

INCLUDE_ASM("asm/nonmatchings/game", func_80036CF8);

INCLUDE_ASM("asm/nonmatchings/game", func_80036D0C);

INCLUDE_ASM("asm/nonmatchings/game", func_80036E30);

INCLUDE_ASM("asm/nonmatchings/game", func_80036FAC);

void func_80037440(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_80037448);

INCLUDE_ASM("asm/nonmatchings/game", func_8003756C);

INCLUDE_ASM("asm/nonmatchings/game", func_80037698);

INCLUDE_ASM("asm/nonmatchings/game", func_8003817C);

INCLUDE_ASM("asm/nonmatchings/game", func_80038700);

INCLUDE_ASM("asm/nonmatchings/game", func_80038AFC);

INCLUDE_ASM("asm/nonmatchings/game", func_80038C14);

INCLUDE_ASM("asm/nonmatchings/game", func_80038D08);

INCLUDE_ASM("asm/nonmatchings/game", func_80038E14);

INCLUDE_ASM("asm/nonmatchings/game", func_80038F00);

INCLUDE_ASM("asm/nonmatchings/game", func_8003900C);

INCLUDE_ASM("asm/nonmatchings/game", func_80039104);

INCLUDE_ASM("asm/nonmatchings/game", func_800391D8);

INCLUDE_ASM("asm/nonmatchings/game", func_80039310);

INCLUDE_ASM("asm/nonmatchings/game", func_800394C4);

INCLUDE_ASM("asm/nonmatchings/game", func_80039684);

INCLUDE_ASM("asm/nonmatchings/game", func_8003981C);

INCLUDE_ASM("asm/nonmatchings/game", func_80039A14);

INCLUDE_ASM("asm/nonmatchings/game", func_80039AA4);

INCLUDE_ASM("asm/nonmatchings/game", func_80039B14);

INCLUDE_ASM("asm/nonmatchings/game", func_80039C08);

INCLUDE_ASM("asm/nonmatchings/game", func_80039D2C);

INCLUDE_ASM("asm/nonmatchings/game", func_80039E10);

INCLUDE_ASM("asm/nonmatchings/game", func_80039E80);

INCLUDE_ASM("asm/nonmatchings/game", func_80039F74);

INCLUDE_ASM("asm/nonmatchings/game", func_80039F9C);

INCLUDE_ASM("asm/nonmatchings/game", func_80039FC4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003A014);

INCLUDE_ASM("asm/nonmatchings/game", func_8003A680);

INCLUDE_ASM("asm/nonmatchings/game", func_8003A718);

INCLUDE_ASM("asm/nonmatchings/game", func_8003A7CC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003A9B8);

INCLUDE_ASM("asm/nonmatchings/game", func_8003AE54);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B24C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B89C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B8BC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B8DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B8FC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B91C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003B940);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BBB4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BBD4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BBF4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BC14);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BC34);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BC5C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BE88);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BEA8);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BF04);

INCLUDE_ASM("asm/nonmatchings/game", func_8003BF90);

INCLUDE_ASM("asm/nonmatchings/game", func_8003C88C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003CBE4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003CBF0);

INCLUDE_ASM("asm/nonmatchings/game", func_8003CD0C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003D040);

INCLUDE_ASM("asm/nonmatchings/game", func_8003D2EC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003D360);

INCLUDE_ASM("asm/nonmatchings/game", func_8003D3CC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003DA00);

INCLUDE_ASM("asm/nonmatchings/game", func_8003DB94);

void func_8003DD0C(void) {
}

INCLUDE_ASM("asm/nonmatchings/game", func_8003DD14);

INCLUDE_ASM("asm/nonmatchings/game", func_8003DED8);

INCLUDE_ASM("asm/nonmatchings/game", func_8003E8DC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003E940);

INCLUDE_ASM("asm/nonmatchings/game", func_8003EA1C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003EAD4);

INCLUDE_ASM("asm/nonmatchings/game", func_8003ED14);

INCLUDE_ASM("asm/nonmatchings/game", func_8003F59C);

INCLUDE_ASM("asm/nonmatchings/game", func_8003F988);

INCLUDE_ASM("asm/nonmatchings/game", func_8003F9EC);

INCLUDE_ASM("asm/nonmatchings/game", func_8003FB24);

INCLUDE_ASM("asm/nonmatchings/game", func_8003FD14);

INCLUDE_ASM("asm/nonmatchings/game", func_80040D14);

INCLUDE_ASM("asm/nonmatchings/game", func_80041218);

INCLUDE_ASM("asm/nonmatchings/game", func_80041600);

INCLUDE_ASM("asm/nonmatchings/game", func_8004164C);

INCLUDE_ASM("asm/nonmatchings/game", func_80041784);

INCLUDE_ASM("asm/nonmatchings/game", func_80041A78);

INCLUDE_ASM("asm/nonmatchings/game", func_80041B80);

INCLUDE_ASM("asm/nonmatchings/game", func_80041D14);

INCLUDE_ASM("asm/nonmatchings/game", func_80041E20);

INCLUDE_ASM("asm/nonmatchings/game", func_80041E5C);

INCLUDE_ASM("asm/nonmatchings/game", func_80042420);

INCLUDE_ASM("asm/nonmatchings/game", func_800429D0);

INCLUDE_ASM("asm/nonmatchings/game", func_80042D10);

INCLUDE_ASM("asm/nonmatchings/game", func_80042FA8);

INCLUDE_ASM("asm/nonmatchings/game", func_80042FD0);

INCLUDE_ASM("asm/nonmatchings/game", func_80043074);

INCLUDE_ASM("asm/nonmatchings/game", func_80043090);

INCLUDE_ASM("asm/nonmatchings/game", func_80043110);

INCLUDE_ASM("asm/nonmatchings/game", func_80043118);

INCLUDE_ASM("asm/nonmatchings/game", func_80043270);

INCLUDE_ASM("asm/nonmatchings/game", func_800433A4);

INCLUDE_ASM("asm/nonmatchings/game", func_800435F4);

INCLUDE_ASM("asm/nonmatchings/game", func_80043658);

INCLUDE_ASM("asm/nonmatchings/game", func_800436C8);

INCLUDE_ASM("asm/nonmatchings/game", func_800436F4);

INCLUDE_ASM("asm/nonmatchings/game", func_80043750);

INCLUDE_ASM("asm/nonmatchings/game", func_80043764);

INCLUDE_ASM("asm/nonmatchings/game", func_80043778);

INCLUDE_ASM("asm/nonmatchings/game", func_8004378C);

INCLUDE_ASM("asm/nonmatchings/game", func_8004379C);

INCLUDE_ASM("asm/nonmatchings/game", func_800437BC);

INCLUDE_ASM("asm/nonmatchings/game", func_800437D8);

INCLUDE_ASM("asm/nonmatchings/game", func_800437F4);

INCLUDE_ASM("asm/nonmatchings/game", func_80044410);

INCLUDE_ASM("asm/nonmatchings/game", func_800444BC);

INCLUDE_ASM("asm/nonmatchings/game", func_800445B8);

INCLUDE_ASM("asm/nonmatchings/game", func_80044BB4);

INCLUDE_ASM("asm/nonmatchings/game", func_80044F54);

INCLUDE_ASM("asm/nonmatchings/game", func_80045218);

INCLUDE_ASM("asm/nonmatchings/game", func_80045878);

INCLUDE_ASM("asm/nonmatchings/game", func_80045C5C);

INCLUDE_ASM("asm/nonmatchings/game", func_80045CAC);

INCLUDE_ASM("asm/nonmatchings/game", func_800461E0);
