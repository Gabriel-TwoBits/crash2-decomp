#include "common.h"
#include "sdk.h"
#include "game.h"

#define OBJ_OFF_ACTIVE 0x00
#define OBJ_OFF_KEY 0x18
#define OBJ_OFF_BUSY 0xB8
#define OBJ_OFF_FLAGS_A 0xAC
#define OBJ_OFF_FLAGS_B 0x100
#define FLAGS_A_BLOCK 0x1000000
#define FLAGS_B_BLOCK 0x40000
#define CALL_ARG_1A00 0x1A00
#define RESULT_REJECTED -0xFF
#define NO_VALUE -1

extern s32 D_800608D0;

#define FIELD(p, off) (*(s32 *)((u8 *)(p) + (off)))

s32 func_80019D30(s32 *obj, s32 key) {
    if (FIELD(obj, OBJ_OFF_BUSY) != 0) {
        return RESULT_REJECTED;
    }
    if (FIELD(obj, OBJ_OFF_ACTIVE) == 0 || FIELD(obj, OBJ_OFF_KEY) != key ||
        (FIELD(obj, OBJ_OFF_FLAGS_A) & FLAGS_A_BLOCK) != 0 ||
        (FIELD(obj, OBJ_OFF_FLAGS_B) & FLAGS_B_BLOCK) != 0) {
        return RESULT_REJECTED;
    }
    func_8001CC10(0, (s32)obj, CALL_ARG_1A00, 0, 0);
    if (FIELD(obj, OBJ_OFF_KEY) == key || D_800608D0 == NO_VALUE) {
        return func_80019BBC(obj, 0);
    }
    return RESULT_REJECTED;
}
