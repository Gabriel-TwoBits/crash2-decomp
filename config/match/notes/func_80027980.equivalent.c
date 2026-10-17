#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_SIZE 0x54
#define ENTRY_STATE 2
#define ENTRY_FLAG 3
#define ENTRY_AMOUNT 4
#define POOL_FREE 3
#define POOL_USED 4
#define OWNER_ACTIVE 2
#define OWNER_FLAG 3
#define STATE_LIVE 1

extern s32 D_80069334;
extern s32 D_80069338;

void func_80027980(s32 arg0) {
    s32 *owner = (s32 *)arg0;
    s32 *pool = &D_80069334;
    s32 i;

    if (owner[OWNER_ACTIVE] != 0) {
        owner[OWNER_FLAG] = 1;
        for (i = pool[0] - 1; i >= 0; i--) {
            s32 *entry = (s32 *)(D_80069338 + i * ENTRY_SIZE);

            if (entry[ENTRY_STATE] == STATE_LIVE) {
                if (entry[ENTRY_FLAG] != STATE_LIVE) {
                    break;
                } else {
                    entry[ENTRY_STATE] = 0;
                    entry[ENTRY_FLAG] = 0;
                    pool[POOL_USED] -= entry[ENTRY_AMOUNT];
                    pool[POOL_FREE] += entry[ENTRY_AMOUNT];
                }
            } else {
                continue;
            }
        }
    } else {
        return;
    }
}
