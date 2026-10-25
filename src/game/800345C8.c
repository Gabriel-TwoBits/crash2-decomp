#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_8001067C);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_8001068C);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", g_SaveProductCode);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", D_800106AC);
INCLUDE_RODATA("asm/nonmatchings/game/800345C8", g_SaveTitle);

s32 *g_SaveState = 0;
s32 D_8005F268 = 1;
s32 D_8005F26C = -1;
s32 D_8005F270 = -1;
char g_SaveDevice[] = "bu00:";
char D_8005F27C[] = "%s%s";
char D_8005F284[] = "*";
char D_8005F288[] = "EMPTY";

extern s32 D_8006CC5C;
extern s32 D_8006CC64;

void Save_Update(void) {
    s32 v;
    s32 *new_var;

    if (D_8005F26C != func_80036150()) {
        D_8005F26C = func_80036150();
        func_80035A6C();
    }
    if (D_8005F270 != Save_GetState()) {
        D_8005F270 = Save_GetState();
        func_80035A74();
    }
    v = Save_GetState();
    switch (v) {
    case 0:
        Save_CheckCard();
        break;
    case 1:
        Save_LoadDirectory();
        break;
    case 5:
        Save_ReadFileHeader();
        break;
    case 6:
    case 7:
    case 8:
        Save_WriteFile(Save_GetState());
        break;
    case 3:
        Save_FormatCard();
        break;
    case 4:
        func_80034810();
        break;
    default:
        break;
    }

    v = func_80036150();
    D_8006CC5C = v;
    new_var = &g_SaveState[2];
    D_8006CC64 = *new_var;
}

extern void _card_clear(long);

void Save_CheckCard(void) {
    if (g_SaveState[6] != 0) {
        g_SaveState[6] = 0;
        _card_info(0);
    }
    switch (Save_TestSwEvents()) {
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
        Save_ClearHwEvents();
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
    g_SaveState[6] = 0;
    if (Save_IsCardFormatted(0) == 1) {
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
void Save_SortDirectory(Unk80035D48 *arg0, s32 arg1);
extern char g_SaveDevice[];
s32 _card_load(s32);

void Save_LoadDirectory(void) {
    s32 r;
    s32 a0;

    if (g_SaveState[6] != 0) {
        g_SaveState[6] = 0;
        Save_ClearSwEvents();
        _card_load(0);
    }
    switch (Save_TestSwEvents()) {
    case 0:
        r = Save_ListFiles(g_SaveDevice, (char *)g_SaveState + 0x49C);
        Save_SortDirectory((Unk80035D48 *)((char *)g_SaveState + 0x49C), r);
        if (r <= 0) {
            g_SaveState[7]++;
            if (g_SaveState[7] < 11) {
                func_800360CC(1);
                return;
            }
        }
        func_800360AC(2);
        func_80036050();
        func_80036850();
        func_800360CC(5);
        func_80035A7C(0x20);
        g_SaveState[2] = 0;
        g_SaveState[8] = r;
        g_SaveState[9] = 0;
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

s32 Save_CheckRetries(s32 arg0, s32 arg1, void (*arg2)(s32), s32 arg3) {
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


void Save_AbortRead(s32 *arg0) {
    Unk80034A2C *p;

    if (func_80035A94() != 0) {
        g_SaveState[2] = 0;
        func_80036084(2);
        func_800360CC(0);
        p = (Unk80034A2C *)g_SaveState;
        p += arg0[1];
        if (p->f28 != 0) {
            func_80036084(0x80);
            func_800368C0();
            func_80036850();
        }
    }
    Save_CloseFile((s32 *)arg0[0]);
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

void Save_AbortWrite(Unk80034AC4 *arg0) {
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
    Save_CloseFile(arg0->f0);
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

#ifdef NON_MATCHING
extern char D_8001067C[];
s32 Save_CheckRetries(s32 arg0, s32 arg1, void (*arg2)(s32), s32 arg3);

typedef struct {
    u8 unk_00[0x28];
    s32 unk_28;
    s32 unk_2C;
    s32 unk_30;
    char unk_34[0x18];
} Unk80034BB8;

#define SAVE_RETRIES 3
#define SAVE_BLOCK_SIZE 0x80
#define SAVE_BLOCK_COUNT_MAX 3
#define SLOT_COPY_WORDS 19
#define SLOT_COPY_FIRST_WORD 10 /* word index of the slot field at 0x28 */
#define UNK_STATE_80 0x80
#define NAME_ENTRY_STRIDE 0x28
#define NAME_TABLE_OFFSET 0x49C
#define EXTRA_BLOCKS_OFFSET 0x6F4
#define EXTRA_BLOCK_COUNT 0x16
#define DEFAULT_NAME_SIZE 13
#define HEADER_KIND_BASE 0x10
#define HEADER_KIND_COUNT 3
#define HEADER_KIND 2
#define HEADER_SPAN 3
#define HEADER_NAME 4
#define HEADER_NAME_END 0x23
#define NAME_TOO_LONG 0x80
#define ERR_FULL 2

/* Words of the g_SaveState block. */
#define W_NEXT_SLOT 2
#define W_LIMIT_LO 4
#define W_LIMIT_HI 5
#define W_ENTRY_COUNT 8
#define W_ENTRY_INDEX 9

void Save_ReadFileHeader(void) {
    char path[0x40];
    u8 blocks[SAVE_BLOCK_COUNT_MAX * SAVE_BLOCK_SIZE];
    u8 header[SAVE_BLOCK_SIZE];
    s32 *ctx[4]; /* callee reads [0] and [1]; the rest keeps the frame layout */
    s32 fd;
    s32 slot;
    s32 entry;
    s32 failed;
    s32 span;
    s32 i;
    s32 j;
    s32 tries;
    s32 *src;
    s32 *dst;
    u8 *extra;
    char *entryName;
    Unk80034BB8 *s;

    slot = g_SaveState[W_NEXT_SLOT];
    ctx[0] = &fd;
    ctx[1] = (s32 *)slot;
    entry = g_SaveState[W_ENTRY_INDEX];
    failed = 0;
    fd = -1;
    if (entry >= g_SaveState[W_ENTRY_COUNT]) {
        func_800360AC(ERR_FULL);
        func_80035E7C();
        func_800360CC(0);
        return;
    } else {
        entryName = (char *)g_SaveState + entry * NAME_ENTRY_STRIDE + NAME_TABLE_OFFSET;
    }
    sprintf(path, D_8005F27C, g_SaveDevice, entryName);
    s = (Unk80034BB8 *)g_SaveState + slot;
    if (entry == g_SaveState[W_ENTRY_COUNT] - 1 && Save_IsGameFile(entryName) != 0) {
        s->unk_28 = 1;
    } else {
        s->unk_28 = 0;
    }
    tries = 0;
    do {
        fd = open(path, 1);
        if (fd >= 0) {
            break;
        } else {
            tries++;
        }
    } while (tries < SAVE_RETRIES);
    if (Save_CheckRetries(tries, SAVE_RETRIES, (void (*)(s32))Save_AbortRead, (s32)ctx) != 0) {
        return;
    } else {
    }
    tries = 0;
    do {
        if (read(fd, header, SAVE_BLOCK_SIZE) == SAVE_BLOCK_SIZE) {
            break;
        } else {
            tries++;
        }
    } while (tries < SAVE_RETRIES);
    if (Save_CheckRetries(tries, SAVE_RETRIES, (void (*)(s32))Save_AbortRead, (s32)ctx) != 0) {
        return;
    } else {
    }
    s = (Unk80034BB8 *)g_SaveState + slot;
    s->unk_2C = header[HEADER_KIND] - HEADER_KIND_BASE;
    if ((u32)(header[HEADER_KIND] - HEADER_KIND_BASE - 1) >= HEADER_KIND_COUNT) {
        failed = 1;
        s->unk_2C = 1;
        if (s->unk_28 != 0 && g_SaveState[W_LIMIT_HI] < g_SaveState[W_LIMIT_LO]) {
            Save_AbortRead((s32 *)ctx);
            return;
        } else {
        }
    } else {
    }
    span = 1;
    if (failed == 0) {
        span = header[HEADER_SPAN];
    } else {
    }
    s = (Unk80034BB8 *)g_SaveState + slot;
    s->unk_30 = entry;
    if (failed == 0) {
        s->unk_34[0] = 0;
        if (header[HEADER_NAME] >= NAME_TOO_LONG) {
            func_8003516C(header);
        } else {
        }
        header[HEADER_NAME_END] = 0;
        strcpy(((Unk80034BB8 *)g_SaveState + slot)->unk_34, (char *)header + HEADER_NAME);
    } else {
        for (i = 0; i < DEFAULT_NAME_SIZE; i++) {
            s->unk_34[i] = D_8001067C[i];
        }
    }
    s = (Unk80034BB8 *)g_SaveState + slot;
    func_80035F9C((u8 *)s->unk_34);
    if (s->unk_28 != 0) {
        if (failed == 0 && failed < s->unk_2C) {
            i = 0;
            do {
                tries = 0;
                do {
                    if (read(fd, blocks + i * SAVE_BLOCK_SIZE, SAVE_BLOCK_SIZE) == SAVE_BLOCK_SIZE) {
                        break;
                    } else {
                        tries++;
                    }
                } while (tries < SAVE_RETRIES);
                if (Save_CheckRetries(tries, SAVE_RETRIES, (void (*)(s32))Save_AbortRead, (s32)ctx) != 0) {
                    return;
                } else {
                }
                i++;
            } while (i < ((Unk80034BB8 *)g_SaveState + slot)->unk_2C);
        } else {
        }
        if (((Unk80034BB8 *)g_SaveState + slot)->unk_28 != 0 && failed == 0) {
            extra = (u8 *)g_SaveState + EXTRA_BLOCKS_OFFSET;
            i = 0;
            do {
                tries = 0;
                do {
                    if (read(fd, extra, SAVE_BLOCK_SIZE) == SAVE_BLOCK_SIZE) {
                        break;
                    } else {
                        tries++;
                    }
                } while (tries < SAVE_RETRIES);
                i++;
                if (Save_CheckRetries(tries, SAVE_RETRIES, (void (*)(s32))Save_AbortRead, (s32)ctx) != 0) {
                    return;
                } else {
                }
                extra += SAVE_BLOCK_SIZE;
            } while ((u32)i < EXTRA_BLOCK_COUNT);
            if (func_800369F4() != 0) {
                failed = 1;
                if (g_SaveState[W_LIMIT_HI] < g_SaveState[W_LIMIT_LO]) {
                    Save_AbortRead((s32 *)ctx);
                    return;
                } else {
                }
            } else {
                func_80036850();
            }
        } else {
        }
    } else {
    }
    Save_CloseFile(&fd);
    if (failed != 0) {
        if (((Unk80034BB8 *)g_SaveState + slot)->unk_28 != 0) {
            func_80036084(UNK_STATE_80);
            func_800368C0();
            func_80036850();
        } else {
        }
    } else {
        for (j = 1; j < span; j++) {
            src = (s32 *)((Unk80034BB8 *)g_SaveState + slot)->unk_00 + SLOT_COPY_FIRST_WORD;
            dst = (s32 *)((Unk80034BB8 *)g_SaveState + slot + j)->unk_00 + SLOT_COPY_FIRST_WORD;
            for (i = 0; i < SLOT_COPY_WORDS; i++) {
                dst[i] = src[i];
            }
        }
    }
    g_SaveState[W_ENTRY_INDEX] = g_SaveState[W_ENTRY_INDEX] + 1;
    g_SaveState[W_NEXT_SLOT] = g_SaveState[W_NEXT_SLOT] + span;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_ReadFileHeader);
#endif

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
void Save_TitleToShiftJis(s8 *arg0, s32 arg1) {
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
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_TitleToShiftJis);
#endif

#ifdef NON_MATCHING
#define MODE_ERASE 6
#define MODE_CREATE 7
#define MAX_TRIES 3
#define VSYNC_SETTLE 20
#define SAVE_BLOCK_SIZE 0x80
#define SAVE_ICON_BLOCKS 3
#define SAVE_DATA_BLOCKS 22
#define SAVE_DATA_OFFSET 0x6F4
#define ENTRY_SIZE 0x4C
#define ENTRY_BASE 0x28
#define NAME_SIZE 0x28
#define NAME_BASE 0x49C
#define NAME_COUNT_OFFSET 0x20
#define SLOT_SERIAL_START 0xF423F
#define OPEN_CREATE_FLAGS 0x10200
#define OPEN_RW_FLAGS 2
#define SERIAL_MAX 99
#define HEADER_TITLE_BYTES 0x20
#define HEADER_ICON_OFFSET 0x60
#define ASCII_ZERO 0x30
#define DECIMAL_BASE 10
#define SAVE_BASE ((char *)g_SaveState)

s32 open(char *, s32);
s32 printf(const char *, ...);
s32 write(s32, char *, s32);
extern char D_8001068C[];
extern char g_SaveProductCode[];
extern char D_800106AC[];
extern char g_SaveTitle[];
extern s32 D_8005F314;
extern s32 D_8005F318;
extern s32 D_8005F31C;
extern s32 D_8006CCB4;

void Save_WriteFile(s32 arg0) {
    s32 fd;
    s32 *ctx[6];
    char path[0x40];
    char icons[SAVE_ICON_BLOCKS][SAVE_BLOCK_SIZE];
    char header[SAVE_BLOCK_SIZE];
    char *entry;
    char *name;
    s32 mode;
    s32 count;
    s32 tries;
    s32 serial;
    s32 n;
    s32 k;
    s32 r;
    s32 noEntry;

    mode = arg0;
    count = *(s32 *)(SAVE_BASE + 8);
    fd = -1;
    if (count > 0 && *(s32 *)(SAVE_BASE + 0xC) < count) {
        entry = SAVE_BASE + *(s32 *)(SAVE_BASE + 0xC) * ENTRY_SIZE + ENTRY_BASE;
    } else {
        entry = 0;
    }
    if (mode == MODE_ERASE) {
        D_8005F31C = Rand_Below(SLOT_SERIAL_START);
        D_8005F314 = 0;
        D_8005F318 = 0;
    }
    ctx[0] = &fd;
    ctx[2] = &D_8005F314;
    ctx[3] = &D_8005F318;
    ctx[4] = (s32 *)path;

    if (entry != 0 && *(s32 *)entry == 0) {
        name = SAVE_BASE + *(s32 *)(entry + 8) * NAME_SIZE + NAME_BASE;
        sprintf(path, D_8005F27C, g_SaveDevice, name);
        if (mode == MODE_ERASE) {
            VSync(VSYNC_SETTLE);
            tries = 0;
            do {
                r = erase((s32)path);
                VSync(VSYNC_SETTLE);
                if (r == 1) {
                    break;
                } else {
                    tries++;
                }
            } while (tries < MAX_TRIES);
            if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) == 0) {
                func_800360CC(MODE_CREATE);
            }
            return;
        } else {
            /* falls through to the shared path below */
        }
    } else {
        if (mode == MODE_ERASE) {
            func_800360CC(MODE_CREATE);
            mode = MODE_CREATE;
        }
    }

    noEntry = entry == 0 || *(s32 *)entry == 0;
    if (!noEntry) {
        name = SAVE_BASE + *(s32 *)(entry + 8) * NAME_SIZE + NAME_BASE;
        sprintf(path, D_8005F27C, g_SaveDevice, name);
    } else {
        n = *(s32 *)(SAVE_BASE + NAME_COUNT_OFFSET) - 1;
        serial = 0;
        if (n >= 0) {
            name = SAVE_BASE + n * NAME_SIZE + NAME_BASE;
            if (Save_IsGameFile(name) != 0) {
                serial = (((u8)name[0xC] - ASCII_ZERO) * DECIMAL_BASE + (u8)name[0xD]) - (ASCII_ZERO - 1);
            } else {
                /* serial stays 0 */
            }
        } else {
            /* serial stays 0 */
        }
        if (serial >= SERIAL_MAX + 1) {
            serial = SERIAL_MAX;
        } else {
            /* in range */
        }
        sprintf(path, D_8001068C, g_SaveDevice, g_SaveProductCode, serial, D_8005F31C);
    }

    if (noEntry && mode == MODE_CREATE) {
        VSync(VSYNC_SETTLE);
        tries = 0;
        do {
            fd = open(path, OPEN_CREATE_FLAGS);
            if (fd >= 0) {
                break;
            } else {
                tries++;
            }
        } while (tries < MAX_TRIES);
        if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) == 0) {
            printf(D_800106AC, fd);
            Save_CloseFile(&fd);
            D_8005F314 = 1;
            func_800360CC(8);
        } else {
            /* open failed; handler ran */
        }
        return;
    } else {
        if (!noEntry && mode == MODE_CREATE) {
            func_800360CC(8);
        } else {
            /* no state change */
        }
    }

    tries = 0;
    do {
        fd = open(path, OPEN_RW_FLAGS);
        if (fd >= 0) {
            break;
        } else {
            tries++;
        }
    } while (tries < MAX_TRIES);
    if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) != 0) {
        return;
    } else {
        /* opened */
    }

    for (k = 0; k < SAVE_BLOCK_SIZE; k++) {
        header[k] = 0;
    }
    header[0] = 0x53;
    header[1] = 0x43;
    header[2] = 0x13;
    header[3] = 1;
    sprintf(header + 4, g_SaveTitle);
    Save_TitleToShiftJis((s8 *)header + 4, HEADER_TITLE_BYTES);
    Gfx_TransferAnimFrame(2, D_8006CCB4, 0, (u8 *)header + HEADER_ICON_OFFSET, (u8 *)icons[0], 0);
    Gfx_TransferAnimFrame(2, D_8006CCB4, 0, 0, (u8 *)icons[1], 0);
    Gfx_TransferAnimFrame(2, D_8006CCB4, 0x100, 0, (u8 *)icons[2], 0);

    tries = 0;
    do {
        r = write(fd, header, SAVE_BLOCK_SIZE);
        if (r == SAVE_BLOCK_SIZE) {
            break;
        } else {
            tries++;
        }
    } while (tries < MAX_TRIES);
    if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) != 0) {
        return;
    } else {
        /* header written */
    }

    for (k = 0; k < SAVE_ICON_BLOCKS; k++) {
        tries = 0;
        do {
            r = write(fd, icons[k], SAVE_BLOCK_SIZE);
            if (r == SAVE_BLOCK_SIZE) {
                break;
            } else {
                tries++;
            }
        } while (tries < MAX_TRIES);
        if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) != 0) {
            return;
        } else {
            /* block written */
        }
    }

    func_80036978();
    D_8005F318 = 1;
    for (k = 0; k < SAVE_DATA_BLOCKS; k++) {
        tries = 0;
        do {
            r = write(fd, SAVE_BASE + SAVE_DATA_OFFSET + k * SAVE_BLOCK_SIZE, SAVE_BLOCK_SIZE);
            if (r == SAVE_BLOCK_SIZE) {
                break;
            } else {
                tries++;
            }
        } while (tries < MAX_TRIES);
        if (Save_CheckRetries(tries, MAX_TRIES, (void (*)(s32))Save_AbortWrite, (s32)ctx) != 0) {
            return;
        } else {
            /* block written */
        }
    }
    func_800369F4();
    D_8005F318 = 0;
    Save_CloseFile(&fd);
    func_800360AC(2);
    func_800360CC(0);
    func_80035FD8();
    func_800360AC(0x10);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_WriteFile);
#endif

extern volatile u32 g_TimerTicks;
extern char g_SaveDevice[];
s32 format(char *);

void Save_FormatCard(void) {
    s32 r;
    g_TimerTicks;
    r = format(g_SaveDevice);
    g_TimerTicks;
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

s32 func_80035A7C(s32 arg0) {
    g_SaveState[5] = 0;
    g_SaveState[4] = arg0;
    return 0;
}

s32 func_80035A94(void) {
    s32 *p = g_SaveState;
    s32 n = p[5];
    p[5] = n + 1;
    return n >= p[4];
}

s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;

s32 Save_TestSwEvents(void) {
    Rand_Below(0x3E7);
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
        result = Save_TestSwEvents();
    } while (result == -1);
    return result;
}


void Save_ClearSwEvents(void) {
    TestEvent(D_8005F3A0);
    TestEvent(D_8005F3A4);
    TestEvent(D_8005F3A8);
    TestEvent(D_8005F3AC);
}

s32 D_8005F3BC;
s32 D_8005F3C0;
s32 D_8005F3C8;
s32 D_8005F3CC;

s32 Save_TestHwEvents(void) {
    Rand_Below(0x3E7);
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
        result = Save_TestHwEvents();
    } while (result == -1);
    return result;
}


void Save_ClearHwEvents(void) {
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

s32 Save_ListFiles(char *path, void *entries) {
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
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_ListFiles);
#endif

s32 strcmp(char *, char *);


void Save_SortDirectory(Unk80035D48 *arg0, s32 arg1) {
    Unk80035D48 tmp;
    s32 i;
    s32 j;
    s32 first;
    s32 r;
    char *a;

    for (i = 0; i < arg1; i++) {
        a = arg0[i].b;
        first = Save_IsGameFile(a);
        for (j = i + 1; j < arg1; j++) {
            r = Save_IsGameFile(arg0[j].b);
            if (first != 0 && (r == 0 || strcmp(a, arg0[j].b) > 0)) {
                Sys_Memcpy(&arg0[i], &tmp, 0x14);
                Sys_Memcpy(&arg0[j], &arg0[i], 0x14);
                Sys_Memcpy(&tmp, &arg0[j], 0x14);
                i--;
                break;
            }
        }
    }
}

void func_80035E7C(void) {
    s32 i;
    s32 n;

    if (g_SaveState[2] >= 15) {
        n = g_SaveState[8];
        for (i = 0; i < n; i++) {
            if (Save_IsGameFile((char *)g_SaveState + 0x49C + i * 0x28) != 0) {
                return;
            }
        }
        func_80036084(0x40);
    }
}

extern char g_SaveProductCode[];

s32 Save_IsGameFile(char *arg0) {
    return strncmp(g_SaveProductCode, arg0, 12) == 0;
}

void Save_CloseFile(s32 *arg0) {
    s32 temp_a0;

    temp_a0 = *arg0;
    if (temp_a0 >= 0) {
        close(temp_a0);
    }
    *arg0 = -1;
}

extern volatile u32 g_TimerTicks;

void func_80035F70(u32 arg0) {
    u32 start;

    start = g_TimerTicks;
    while (start - g_TimerTicks < arg0) {
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
        g_SaveState[7] = 0;
        return 0;
    }
    return 1;
}

s32 func_80036028(s32 arg0) {
    s32 *p = g_SaveState;

    p[2] = 0;
    p[5] = 0;
    func_80036084(arg0);
    return 0;
}

void func_80036050(void) {
    s8 *p;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        if (g_SaveState != 0) {
            p = ((i * 0x4C) + ((s8 *)g_SaveState)) + 0x34;
            *p = 0;
        }
    }
}

extern s32 D_8006CC5C;

s32 func_80036084(s32 a0) {
    s32 *p = g_SaveState;
    s32 v = p[1] | a0;
    p[1] = v;
    D_8006CC5C = v;
    return 0;
}

s32 func_800360AC(s32 arg0) {
    g_SaveState[1] &= ~arg0;
    return 0;
}

s32 func_800360CC(s32 arg0) {
    s32 *p = g_SaveState;
    *p = arg0;
    if (arg0 >= 0) {
        if (arg0 < 2 || arg0 == 4) {
            p[6] = 1;
        }
        if (*g_SaveState == 0) {
            func_800360AC(1);
        } else {
            func_80036084(1);
        }
    } else {
        func_80036084(1);
    }
    return 0;
}

s32 Save_GetState(void) {
    return g_SaveState[0];
}

s32 func_80036150(void) {
    return g_SaveState[1];
}

void EnableEvent(s32);
void InitCARD(s32);
s32 OpenEvent(s32, s32, s32, s32);
void StartCARD(void);
void _bu_init(void);

void Save_InitCard(void) {
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

s32 Save_StopCard(void) {
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

    p = Heap_AllocZeroed(1, 0x11F4);
    g_SaveState = p;
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
    do { } while (0); has_ptr = g_SaveState != 0;
    D_8005F268 = (v >> 4) & 1;
    if (has_ptr) {
        Heap_Free((s32)g_SaveState);
        g_SaveState = 0;
    }
    return 0;
}

#ifdef NON_MATCHING
s32 _card_read(s32, s32, u8 *);

s32 Save_IsCardFormatted(s32 arg0) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    Save_ClearHwEvents();
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
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_IsCardFormatted);
#endif

s32 func_8003649C(s32 arg0) {
    s32 i;
    s32 *p;

    if (func_80036150() & 0xD) {
        func_80036084(2);
        return 1;
    }
    i = 0;
    p = g_SaveState;
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
    g_SaveState[3] = i;
    return 0;
}

s32 func_8003653C(void) {
    s32 v;
    if (g_SaveState != 0) {
        v = func_80036150() & 0x10;
    } else {
        v = D_8005F268;
    }
    if (v == 0) {
        Save_ClearSwEvents();
        _card_info(0);
        return func_80035B3C() != 0;
    }
    return 1;
}

#ifdef NON_MATCHING
extern char g_SaveName[];


s32 Save_IsSlotNamed(s32 arg0) {
    return strcmp(g_SaveName, (char *)(arg0 * 0x2A4 + (int)g_SaveState) + 0x700) == 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", Save_IsSlotNamed);
#endif

#ifdef NON_MATCHING
s32 func_800365E4(void) {
    Save_ClearSwEvents();
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
    if (Save_GetState() != 0) {
        func_80036084(2);
        return 1;
    }
    func_800360CC(3);
    func_80035A7C(3);
    return 0;
}

s32 func_80036674(void) {
    s32 ret;
    if (g_SaveState != 0) {
        func_80036324((func_80036150() & 0x10) | 4);
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 Save_UnformatCard(void) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    buf[1] = 0xFF;
    buf[0] = 0xFF;
    Save_ClearHwEvents();
    _new_card();
    _card_write(0, 0, buf);
    return -(func_80035C40() != 0);
}

s32 Save_RunCommand(u32 arg0, s32 arg1) {
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
        return Save_UnformatCard();
    case 13:
        return func_80036AC8(arg1);
    case 14:
        return Save_LoadSlot(arg1);
    case 15:
        if (g_SaveState != 0) {
            Save_InitSlot((char *)g_SaveState + (arg1 * 0x2A4 + 0x6F4));
            func_80036850();
            return 0;
        }
        return 1;
    case 16:
        return Save_GetSlotName(arg1);
    case 17:
        return Save_SetSlotName(arg1);
    case 18:
        return Save_IsSlotNamed(arg1);
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
        p = (char *)g_SaveState + (i * 0x2A4 + 0x6F4) + 0xC;
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
        Save_InitSlot((s8 *)g_SaveState + offset);
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

void Save_InitSlot(void *p)
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

typedef void (*Unk80036A84Fn)(s32 *, s32, s32, s32);

void func_80036978(void) {
    s32 *p = g_SaveState;
    s32 n;

    p[0x6F4 / 4] = 8;
    p++;
    p--;
    n = Rand_Below(0xF423F);
    p[0x954 / 4] = n;
    p[0x718 / 4] = 0;
    p[0x718 / 4] = func_80036A8C(p + 0x6F4 / 4, 0x2A4);
    /* game.h declares func_80036A84 as void(void); the assembly passes four arguments. */
    ((Unk80036A84Fn)func_80036A84)(p + 0x6F4 / 4, 0x2A4, n, 1);
}

#ifdef NON_MATCHING
#define OFF_STATE 0x6F4
#define OFF_SAVED 0x718
#define OFF_ARG 0x954
#define STATE_SIZE 0x2A4
#define STATE_IDLE 8
#define RESULT_OK 0
#define RESULT_MISMATCH 1
#define RESULT_RESET 2

typedef void (*Func80036A84)(s32 *, s32, s32, s32);


s32 func_800369F4(void) {
    s32 *base = g_SaveState;
    s32 *state = base + OFF_STATE / 4;
    s32 saved;
    s32 ret;

    ((Func80036A84)func_80036A84)(state, STATE_SIZE, base[OFF_ARG / 4], 0);
    saved = base[OFF_SAVED / 4];
    base[OFF_SAVED / 4] = 0;
    if (saved == func_80036A8C(state, STATE_SIZE)) {
        if (base[OFF_STATE / 4] == STATE_IDLE) {
            return RESULT_OK;
        } else {
            ret = RESULT_RESET;
        }
    } else {
        ret = RESULT_MISMATCH;
    }
    func_800368C0();
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800369F4);
#endif

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

    if (g_SaveState == 0) {
        return 1;
    }
    base = (char *)g_SaveState + (arg0 * 0x2A4 + 0x6F4);
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

extern char g_SaveName[];
extern char *D_8006CBCC;

s32 Save_GetSlotName(s32 arg0) {
    s32 ret;
    char *src;
    if (g_SaveState != 0) {
        src = (char *)(arg0 * 0x2A4 + (s32)g_SaveState + 0x700);
        strcpy(g_SaveName, src);
        D_8006CBCC = g_SaveName;
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 Save_SetSlotName(s32 arg0) {
    s32 ret;

    if (g_SaveState != 0) {
        strcpy((char *)(arg0 * 0x2A4 + (s32)g_SaveState + 0x700), g_SaveName);
        func_80036850();
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

s32 Save_LoadSlot(s32 arg0) {
    s32 *src;
    s32 *row;
    s32 *dst;
    s32 i;
    s32 j;
    char *base;

    if (g_SaveState == 0) {
        return 1;
    }
    base = (char *)g_SaveState + (arg0 * 0x2A4 + 0x6F4);
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
    strcpy(g_SaveName, base + 0xC);
    D_8006CBCC = g_SaveName;
    return 0;
}

#define BITS_PER_WORD 32

/* The low `bits` bits of value, sign-extended */
s32 func_80036CF8(s32 value, s32 bits) {
    return (value << (BITS_PER_WORD - bits)) >> (BITS_PER_WORD - bits);
}
