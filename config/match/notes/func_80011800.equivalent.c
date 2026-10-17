#include "common.h"
#include "sdk.h"
#include "game.h"

#define POWER_BUTTON_MASK 0x800
#define SCRATCH_ENTRY_BASE 0x1F800280
#define SAVE_MAGIC 0x6396347F
#define SAVE_FLAGS_MASK 0x20000001
#define STATE_DEAD 0x3B
#define STATE_NONE -1
#define STATE_PENDING -2
#define STATE_PAUSE 2
#define LEVEL_A 0x200
#define LEVEL_B 0x300
#define LEVEL_C 0x400
#define FLAG_16 0x10
#define FLAG_400000 0x400000
#define PAUSE_BUF_ID 0xC00
#define PAUSE_BUF_SIZE 0x800
#define PAUSE_BUF_OFFSET 0x18
#define BUF_FIELD_2088 0x2088
#define CD14(off) (*(s32 *)(D_8006CD14 + (off)))

extern s32 D_8003BEA4;
extern s32 D_8005F180;
extern s32 D_8005F18C;
extern char D_8005F190[];
extern s32 D_8005F1D0;
extern s32 D_8005F1F4;
extern s32 D_8005F2C4;
extern s32 D_8005F2C8;
extern s32 D_8005F388;
extern s32 D_8005F38C;
extern s32 D_8005F390;
extern s32 D_8005F3B8;
extern s32 D_8005F3F0;
extern char D_80060774[];
extern void *D_800608CC;
extern s32 D_80060944;
extern char D_80060958[];
extern s32 D_80060988;
extern char D_80062A18[];
extern s32 D_80062A90;
extern s32 D_80062AA0;
extern s32 D_80062AD8;
extern void *D_80063454;
extern void *D_80063458;
extern char D_80067588[];
extern void *D_8006759C;
extern s32 D_80069944;
extern s32 D_8006CB94;
extern s32 D_8006CB98;
extern s32 D_8006CBA0;
extern s32 D_8006CBB4;
extern s32 D_8006CCB8;
extern char D_8006CD14[];
extern s32 D_8006CD7C;

void SetRotMatrix(void *);
void func_80011E80(void);
void func_80011E88(void);
void func_80011E90(void);
int func_8001645C(void);
void func_8002F6C8(void *);
void func_8003DB94(s32, void *, void *);
/* func_80041E5C stays undeclared: 800158E0.c declares it void, but its v0 is stored below (implicit int). */
void func_80044F54(s32, void *, void *);
void func_80045218(s32, void *, void *);
/* func_80014C68 stays undeclared: 800123F0.c defines it over a struct type unknown here (implicit int). */
int printf(const char *, ...);

void func_80011800(s32 arg0) {
    s32 spC0;
    s32 state;
    s32 pending;
    s32 count;
    s32 *entry;
    s32 *out;
    s32 *list;

    func_8002EFE4();
    func_80014C68(D_80067588, arg0);
    func_8002EE48();
    do {
        if (((D_80069944 & POWER_BUTTON_MASK) || ((D_8005F180 == 1) && (D_8006CBA0 == 0))) && (CD14(0) == 0) && (CD14(4) == 0) && (CD14(-0x7C) != 0)) {
            D_8005F180 = D_8005F180 == 0;
            if (D_8005F180 != 0) {
                if (CD14(-0x174) == 0) {
                    CD14(-0x174) = func_80019800(D_8006CD14 - 0x1C4, 4, 4, 0, 0, 0);
                    if (CD14(-0x174) >= -0xFE) {
                        D_8005F180 = 0;
                        CD14(-0x174) = 0;
                        D_8005F18C = 0;
                    } else {
                        D_8005F18C = 1;
                        D_8005F2C4 = D_8003BEA4;
                        D_8005F2C8 = *(s32 *)((char *)D_80063454 + 0xC);
                    }
                } else {
                    /* flag state kept; nothing to do */
                }
            } else if (CD14(-0x174) != 0) {
                spC0 = 0;
                func_8001CC10(0, CD14(-0x174), PAUSE_BUF_ID, 1, &spC0);
                D_8005F180 = 1;
            } else {
                D_8005F18C = -1;
                D_8003BEA4 = D_8005F2C4;
                *(s32 *)((char *)D_80063454 + 0xC) = D_8005F2C8;
            }
        } else {
            D_8005F18C = 0;
        }
        func_80011E80();
        if ((D_8005F38C != 0) && (D_8005F388 != SAVE_MAGIC)) {
            func_8002FDCC(&D_8005F388);
        }
        if (D_8005F3F0 == STATE_NONE) {
            if ((*(s32 *)((char *)D_8006759C + 8) != 2) && ((D_8006CBB4 == LEVEL_A) || (D_8006CBB4 == LEVEL_B) || (D_8006CBB4 == LEVEL_C))) {
                printf(D_8005F190);
                D_8005F3F0 = STATE_DEAD;
                D_8006CD7C = 0;
            } else {
                /* no death transition */
            }
        } else {
            /* a state change is pending */
        }
        if (D_8005F3F0 != STATE_NONE) {
            state = D_8005F3F0;
            func_8001D194(0, 0x2900, 0, 0, 0);
            pending = 0;
            if (D_8005F3F0 == STATE_PENDING) {
                state = D_80060988;
                pending = 1;
                D_8005F1F4 = 1;
            } else {
                D_8005F1F4 = 0;
            }
            *(s32 *)(D_80067588 + 0x24) = 2;
            func_8001521C((int *)D_80067588);
            D_8005F180 = 0;
            D_8005F3B8 = 0;
            if ((state == 2) || ((u32)(state - STATE_DEAD) < 2U)) {
                D_8006CB98 = D_8006CBB4;
            } else {
                /* other states keep the saved level */
            }
            func_80014C68(D_80067588, state);
            func_8002EE48();
            if (pending != 0) {
                D_8005F3F0 = STATE_NONE;
                func_8002F6C8(D_80060958);
            } else {
                /* nothing to clear */
            }
            D_8005F1F4 = 0;
        } else {
            /* no pending state */
        }
        func_80012BC4(-1);
        if (D_8005F180 == 0) {
            func_80026CA0();
        } else {
            /* paused */
        }
        func_80017BC4();
        if ((D_8006CB94 & 1) && (**(s32 **)((char *)D_800608CC + 0x10) != 0)) {
            SetRotMatrix(D_80060774);
            list = *(s32 **)((char *)D_800608CC + 0x10);
            count = *list;
            out = (s32 *)SCRATCH_ENTRY_BASE;
            if (count != 0) {
                entry = list + 0x2C / 4;
                do {
                    s32 *owner = (s32 *)entry[-6];
                    out[0] = ((entry[-8] & 0xFFFE) << 16) | *(u16 *)((char *)entry - 0x24);
                    out[1] = entry[-7];
                    out[2] = entry[-5] + owner[0x14 / 4] * 4;
                    out[3] = entry[-4];
                    out[4] = entry[-3] + owner[0x10 / 4] * 4;
                    out[5] = entry[-2];
                    out[6] = entry[-1];
                    out[7] = entry[0];
                    out += 8;
                    count -= 1;
                    entry += 12;
                } while (count != 0);
            } else {
                /* empty list */
            }
            func_8002FB84(0);
            D_8006CCB8 = func_80041E5C(D_8005F390, D_80063458, D_80062A18, D_80060944);
        } else {
            func_8003BC5C((u8 *)D_80063458 + PAUSE_BUF_OFFSET, PAUSE_BUF_SIZE);
        }
        if (D_8006CB94 & SAVE_FLAGS_MASK) {
            if ((D_80062A90 != 0) && (D_80062AD8 & FLAG_16)) {
                func_80044F54(D_80062A90, D_80063458, D_80062A18);
            } else {
                /* skipped */
            }
            if ((D_80062AA0 != 0) && (D_80062AD8 & FLAG_400000)) {
                func_80045218(D_80062AA0, D_80063458, D_80062A18);
            } else {
                /* skipped */
            }
        } else {
            /* skipped */
        }
        func_8001C29C((void *)(D_8005F180 == 0));
        func_80011E88();
        func_8003DB94(*(s32 *)((char *)D_80063458 + BUF_FIELD_2088), D_80063458, D_80062A18);
        func_8001645C();
        func_80011E90();
    } while (D_8005F1D0 == 0);
    D_8006CB94 = 0;
    func_8001645C();
    func_8001645C();
    func_8001521C((int *)D_80067588);
}
