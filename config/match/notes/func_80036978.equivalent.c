#include "common.h"
#include "sdk.h"
#include "game.h"

#define POOL_SLOT_BASE 0x6F4
#define POOL_SLOT_SIZE 0x2A4
#define POOL_SLOT_COUNT_WORD 0x6F4
#define POOL_COUNT_INIT 8
#define POOL_BUFFER_PTR 0x954
#define POOL_SLOT_HANDLE 0x718
#define POOL_BUFFER_SIZE 0xF423F

extern s32 *D_8005F264;
s32 func_80036A8C(s32 *arg0, s32 arg1);

void func_80036A84(s32 *arg0, s32 arg1, s32 arg2, s32 arg3);

void func_80036978(void) {
    s32 *pool = D_8005F264;
    s32 *slots;
    s32 buffer;

    *(s32 *)((char *)pool + POOL_SLOT_BASE) = POOL_COUNT_INIT;
    buffer = func_80031578(POOL_BUFFER_SIZE);
    slots = (s32 *)((char *)pool + POOL_SLOT_BASE);
    *(s32 *)((char *)pool + POOL_BUFFER_PTR) = buffer;
    *(s32 *)((char *)pool + POOL_SLOT_HANDLE) = 0;
    *(s32 *)((char *)pool + POOL_SLOT_HANDLE) = func_80036A8C(slots, POOL_SLOT_SIZE);
    func_80036A84(slots, POOL_SLOT_SIZE, buffer, 1);
}
