#include "common.h"
#include "sdk.h"
#include "game.h"

#define KIND_SHIFT 6
#define KIND_COPY_A 1
#define KIND_COPY_B 2
#define MODE_MASK 0x38
#define MODE_SHIFT 3
#define MODE_PLAIN 0
#define MODE_MASKED 1
#define LOW_BITS_MASK 7
#define FLIP_FLAG 8
#define RESULT_KIND1 0x200
#define RESULT_KIND2 0x100
#define HEADER_SIZE 4
#define OFS_WIDTH 0x18
#define OFS_STRIDE 0x1C
#define OFS_DST_PLAIN 0x34
#define OFS_DST_KIND1 0x38
#define OFS_DST_DATA 0x3C
#define OFS_DST_MASK 0x40
#define OFS_ARG5 0x44

#define FIELD_S32(base, ofs) (*(s32 *)((u8 *)(base) + (ofs)))
#define FIELD_PTR(base, ofs) (*(u8 **)((u8 *)(base) + (ofs)))

s32 func_800272A4(Unk800271D8 *arg0, u8 *arg1) {
    s32 outLen;
    s32 n;
    u32 mode;
    u8 *mask;
    u8 *data;
    s32 result;

    result = 0;
    switch (arg1[0] >> KIND_SHIFT) {
    case KIND_COPY_A:
        func_80027420(arg1 + HEADER_SIZE, FIELD_PTR(arg0, OFS_DST_KIND1), arg1[0] & LOW_BITS_MASK, 0, 0, FIELD_S32(arg0, OFS_ARG5));
        result = RESULT_KIND1;
        break;
    case KIND_COPY_B:
        mode = (arg1[0] & MODE_MASK) >> MODE_SHIFT;
        n = arg1[0] & LOW_BITS_MASK;
        switch (mode) {
        case MODE_PLAIN:
            if (n != 0) {
                func_80027420(arg1 + 1, FIELD_PTR(arg0, OFS_DST_PLAIN), n, 0, 0, FIELD_S32(arg0, OFS_ARG5));
                result = RESULT_KIND2;
            }
            break;
        case MODE_MASKED:
            mask = func_80027420(arg1 + ((arg1[2] << 8) | arg1[3]), FIELD_PTR(arg0, OFS_DST_MASK), arg1[1] & LOW_BITS_MASK, 0, mode, FIELD_S32(arg0, OFS_ARG5));
            data = func_80027420(arg1 + HEADER_SIZE, FIELD_PTR(arg0, OFS_DST_DATA), n, &outLen, mode, FIELD_S32(arg0, OFS_ARG5));
            func_80027500(data, mask, FIELD_PTR(arg0, OFS_DST_PLAIN), outLen, arg1[1] & FLIP_FLAG, FIELD_S32(arg0, OFS_WIDTH), FIELD_S32(arg0, OFS_STRIDE));
            result = RESULT_KIND2;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return result;
}
