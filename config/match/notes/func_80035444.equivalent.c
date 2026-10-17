#include "common.h"
#include "sdk.h"
#include "game.h"

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
#define SAVE_BASE ((char *)D_8005F264)

s32 open(char *, s32);
s32 printf(const char *, ...);
s32 write(s32, char *, s32);
extern char D_8001068C[];
extern char D_8001069C[];
extern char D_800106AC[];
extern char D_800106C4[];
extern s32 D_8005F314;
extern s32 D_8005F318;
extern s32 D_8005F31C;
extern s32 D_8006CCB4;

void func_80035444(s32 arg0) {
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
        D_8005F31C = func_80031578(SLOT_SERIAL_START);
        D_8005F314 = 0;
        D_8005F318 = 0;
    }
    ctx[0] = &fd;
    ctx[2] = &D_8005F314;
    ctx[3] = &D_8005F318;
    ctx[4] = (s32 *)path;

    if (entry != 0 && *(s32 *)entry == 0) {
        name = SAVE_BASE + *(s32 *)(entry + 8) * NAME_SIZE + NAME_BASE;
        sprintf(path, D_8005F27C, D_8005F274, name);
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
            if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) == 0) {
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
        sprintf(path, D_8005F27C, D_8005F274, name);
    } else {
        n = *(s32 *)(SAVE_BASE + NAME_COUNT_OFFSET) - 1;
        serial = 0;
        if (n >= 0) {
            name = SAVE_BASE + n * NAME_SIZE + NAME_BASE;
            if (func_80035F00(name) != 0) {
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
        sprintf(path, D_8001068C, D_8005F274, D_8001069C, serial, D_8005F31C);
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
        if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) == 0) {
            printf(D_800106AC, fd);
            func_80035F2C(&fd);
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
    if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) != 0) {
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
    sprintf(header + 4, D_800106C4);
    func_800352D8((s8 *)header + 4, HEADER_TITLE_BYTES);
    func_800176DC(2, D_8006CCB4, 0, (u8 *)header + HEADER_ICON_OFFSET, (u8 *)icons[0], 0);
    func_800176DC(2, D_8006CCB4, 0, 0, (u8 *)icons[1], 0);
    func_800176DC(2, D_8006CCB4, 0x100, 0, (u8 *)icons[2], 0);

    tries = 0;
    do {
        r = write(fd, header, SAVE_BLOCK_SIZE);
        if (r == SAVE_BLOCK_SIZE) {
            break;
        } else {
            tries++;
        }
    } while (tries < MAX_TRIES);
    if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) != 0) {
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
        if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) != 0) {
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
        if (func_800349F8(tries, MAX_TRIES, (void (*)(s32))func_80034AC4, (s32)ctx) != 0) {
            return;
        } else {
            /* block written */
        }
    }
    func_800369F4();
    D_8005F318 = 0;
    func_80035F2C(&fd);
    func_800360AC(2);
    func_800360CC(0);
    func_80035FD8();
    func_800360AC(0x10);
}
