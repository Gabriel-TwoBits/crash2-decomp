#include "common.h"
#include "sdk.h"
#include "game.h"

s32 Zone_IsLinked(s32 arg0, s32 *arg1) {
    s32 key;
    s32 count;
    s32 n;
    s32 v;
    s32 i;
    char *obj;
    char *slot;
    s32 pad[2];

    obj = *(char **)(arg0 + 0x10);
    count = *(s32 *)(obj + 0x190);
    i = 0;
    if (count > 0) {
        key = arg1[1];
        n = count;
        slot = obj;
        do {
            v = *(s32 *)(slot + 0x194);
            if (v & 1) {
                if (v == key) return 1;
                i++;
            } else {
                i++;
                if (((s32 *)v)[1] == key) {
                    do { return 1; } while (0);
                }
            }
            slot += 4;
        } while (i < n);
    }
    return 0;
}
