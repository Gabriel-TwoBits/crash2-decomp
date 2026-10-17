#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_8001067C[];
s32 func_800349F8(s32 arg0, s32 arg1, void (*arg2)(s32), s32 arg3);
void func_80034A2C(s32 *arg0);

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

/* Words of the D_8005F264 block. */
#define W_NEXT_SLOT 2
#define W_LIMIT_LO 4
#define W_LIMIT_HI 5
#define W_ENTRY_COUNT 8
#define W_ENTRY_INDEX 9

void func_80034BB8(void) {
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

    slot = D_8005F264[W_NEXT_SLOT];
    ctx[0] = &fd;
    ctx[1] = (s32 *)slot;
    entry = D_8005F264[W_ENTRY_INDEX];
    failed = 0;
    fd = -1;
    if (entry >= D_8005F264[W_ENTRY_COUNT]) {
        func_800360AC(ERR_FULL);
        func_80035E7C();
        func_800360CC(0);
        return;
    } else {
        entryName = (char *)D_8005F264 + entry * NAME_ENTRY_STRIDE + NAME_TABLE_OFFSET;
    }
    sprintf(path, D_8005F27C, D_8005F274, entryName);
    s = (Unk80034BB8 *)D_8005F264 + slot;
    if (entry == D_8005F264[W_ENTRY_COUNT] - 1 && func_80035F00(entryName) != 0) {
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
    if (func_800349F8(tries, SAVE_RETRIES, (void (*)(s32))func_80034A2C, (s32)ctx) != 0) {
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
    if (func_800349F8(tries, SAVE_RETRIES, (void (*)(s32))func_80034A2C, (s32)ctx) != 0) {
        return;
    } else {
    }
    s = (Unk80034BB8 *)D_8005F264 + slot;
    s->unk_2C = header[HEADER_KIND] - HEADER_KIND_BASE;
    if ((u32)(header[HEADER_KIND] - HEADER_KIND_BASE - 1) >= HEADER_KIND_COUNT) {
        failed = 1;
        s->unk_2C = 1;
        if (s->unk_28 != 0 && D_8005F264[W_LIMIT_HI] < D_8005F264[W_LIMIT_LO]) {
            func_80034A2C((s32 *)ctx);
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
    s = (Unk80034BB8 *)D_8005F264 + slot;
    s->unk_30 = entry;
    if (failed == 0) {
        s->unk_34[0] = 0;
        if (header[HEADER_NAME] >= NAME_TOO_LONG) {
            func_8003516C(header);
        } else {
        }
        header[HEADER_NAME_END] = 0;
        strcpy(((Unk80034BB8 *)D_8005F264 + slot)->unk_34, (char *)header + HEADER_NAME);
    } else {
        for (i = 0; i < DEFAULT_NAME_SIZE; i++) {
            s->unk_34[i] = D_8001067C[i];
        }
    }
    s = (Unk80034BB8 *)D_8005F264 + slot;
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
                if (func_800349F8(tries, SAVE_RETRIES, (void (*)(s32))func_80034A2C, (s32)ctx) != 0) {
                    return;
                } else {
                }
                i++;
            } while (i < ((Unk80034BB8 *)D_8005F264 + slot)->unk_2C);
        } else {
        }
        if (((Unk80034BB8 *)D_8005F264 + slot)->unk_28 != 0 && failed == 0) {
            extra = (u8 *)D_8005F264 + EXTRA_BLOCKS_OFFSET;
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
                if (func_800349F8(tries, SAVE_RETRIES, (void (*)(s32))func_80034A2C, (s32)ctx) != 0) {
                    return;
                } else {
                }
                extra += SAVE_BLOCK_SIZE;
            } while ((u32)i < EXTRA_BLOCK_COUNT);
            if (func_800369F4() != 0) {
                failed = 1;
                if (D_8005F264[W_LIMIT_HI] < D_8005F264[W_LIMIT_LO]) {
                    func_80034A2C((s32 *)ctx);
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
    func_80035F2C(&fd);
    if (failed != 0) {
        if (((Unk80034BB8 *)D_8005F264 + slot)->unk_28 != 0) {
            func_80036084(UNK_STATE_80);
            func_800368C0();
            func_80036850();
        } else {
        }
    } else {
        for (j = 1; j < span; j++) {
            src = (s32 *)((Unk80034BB8 *)D_8005F264 + slot)->unk_00 + SLOT_COPY_FIRST_WORD;
            dst = (s32 *)((Unk80034BB8 *)D_8005F264 + slot + j)->unk_00 + SLOT_COPY_FIRST_WORD;
            for (i = 0; i < SLOT_COPY_WORDS; i++) {
                dst[i] = src[i];
            }
        }
    }
    D_8005F264[W_ENTRY_INDEX] = D_8005F264[W_ENTRY_INDEX] + 1;
    D_8005F264[W_NEXT_SLOT] = D_8005F264[W_NEXT_SLOT] + span;
}
