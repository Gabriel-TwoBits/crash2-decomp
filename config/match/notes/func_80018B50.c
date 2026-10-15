#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2) {
    char *child;
    char *next;

    child = *(char **)((char *)arg0 + 0x4C);
    if (child != 0) {
        do {
            next = *(char **)(child + 0x48);
            if (func_80018B50(child, arg1, arg2) == -0x19) {
                func_80019BBC((s32)arg0, 0);
            }
            child = next;
        } while (child != 0);
    }
    return arg1(arg0, arg2);
}
