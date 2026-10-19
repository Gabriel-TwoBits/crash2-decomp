#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800608D0;

s32 func_80019D30(GoolObject *arg0, s32 arg1) {
    if (arg0->entity_id == 0) {
        if ((arg0->unk_0 != 0) && (arg0->zone == arg1) && !(arg0->status_b & 0x01000000) && !(arg0->state_flags & 0x40000) && ((Gool_SendEvent(0, (s32) arg0, 0x1A00, 0, 0), (arg0->zone == arg1)) || (D_800608D0 == -1))) {
            return Obj_Kill(&arg0->unk_0, 0);
        }
        return -0xFF;
    }
    arg1 = -0xFF;
    return arg1;
}
