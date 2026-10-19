#include "common.h"
#include "sdk.h"
#include "game.h"

void Gool_OpEntityFieldValue(GoolObject *arg0, u32 arg1) {
    s32 *var_s1;
    s32 *var_v0;
    s32 temp_a0_2;
    s32 temp_a1;
    s32 temp_s2;
    u32 temp_a0;

    temp_a0 = arg1 >> 0xC;
    temp_a1 = temp_a0 & 0xFFF;
    if ((temp_a0 & 0xE00) == 0xE00) {
        if (temp_a1 == 0xE1F) {
            var_v0 = arg0->sp - 4;
            arg0->sp = var_v0;
        } else {
            var_v0 = (s32 *) &arg0->links[temp_a0 & 0x1FF];
        }
    } else {
        var_v0 = Gool_ResolveRef(arg0, temp_a1);
    }
    temp_s2 = *var_v0;
    temp_a0_2 = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a0_2 == 0xE1F) {
            var_s1 = arg0->sp;
            arg0->sp = var_s1 + 4;
        } else {
            var_s1 = (s32 *) &arg0->links[arg1 & 0x1FF];
        }
    } else {
        var_s1 = Gool_ResolveRef(arg0, temp_a0_2);
    }
    if (var_s1 != 0) {
        if (arg0->entity != 0) {
            *var_s1 = Ent_GetPropertyValue((s32) arg0->entity, (s32) ((u32) temp_s2 >> 8), 0, 0, 0, 2, 0);
            return;
        }
        *var_s1 = 0;
    }
}
