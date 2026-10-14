#include "common.h"
#include "sdk.h"

typedef struct {
    u8 pad[0x2088];
    void *unk2088;
    u8 pad2[4];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];

void *func_80016418(s32 arg0) {
    void **temp;

    temp = (void **)D_80063458[0]->unk2090;
    D_80063458[0]->unk2090 = (char *)temp + arg0;
    *temp = D_80063458[0]->unk2088;
    D_80063458[0]->unk2088 = temp;
    return temp;
}
