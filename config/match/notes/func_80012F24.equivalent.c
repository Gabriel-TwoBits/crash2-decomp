#include "common.h"
#include "sdk.h"
#include "game.h"

#define UNK_OFFSET_SECTOR_BASE 0x418
#define UNK_OFFSET_SECTOR_TABLE 0x420
#define SECTOR_TABLE_SHIFT 6
#define SECTORS_SHIFT 5
#define WORD_SIZE_SHIFT 2
#define SHIFT_2K 11
#define CD_READ_SECTORS 0x80
#define CD_CMD_SEEKL 2
#define CD_SYNC_POLL 1
#define CD_SYNC_BLOCK 0
#define CD_STATUS_DISK_ERROR 5
#define LOAD_RESULT -0xFF

extern u8 *D_800675A0;
extern s32 D_800675A8;

s32 CdControlF(s32 cmd, u8 *param);
s32 CdSync(s32 mode, u8 *result);

s32 func_80012F24(u8 *arg0, s32 arg1) {
    u8 pos[16];
    s32 sector;
    s32 count;
    s32 buf;
    s32 index;
    s32 poll;
    s32 block;

    index = *(s32 *)(arg0 + 0x14);
    if (*(s16 *)(arg0 + 0xA) != 0) {
        s32 off = *(s16 *)(arg0 + 8);

        count = (arg1 << SECTORS_SHIFT) - off;
        buf = *(s32 *)arg0 + (off << SHIFT_2K);
        sector = D_800675A8 +
                 (*(u32 *)(D_800675A0 + (index >> 1) * (1 << WORD_SIZE_SHIFT) + UNK_OFFSET_SECTOR_TABLE) >> SECTOR_TABLE_SHIFT);
    } else {
        count = arg1 << SECTORS_SHIFT;
        buf = *(s32 *)arg0;
        sector = D_800675A8 + *(s32 *)(D_800675A0 + UNK_OFFSET_SECTOR_BASE) + ((index >> 1) << SECTORS_SHIFT);
    }

    /* Seek, wait for the seek to finish and issue the read; any failure retries from the seek. */
    for (;;) {
        CdIntToPos(sector, pos);
        if (CdControlF(CD_CMD_SEEKL, pos) == 0) {
            continue;
        }
        poll = CdSync(CD_SYNC_POLL, 0);
        block = CD_STATUS_DISK_ERROR;
        if (poll == 0) {
            block = CdSync(CD_SYNC_BLOCK, 0);
            poll = CD_STATUS_DISK_ERROR;
        }
        if (block == poll) {
            continue;
        }
        if (CdRead(count, buf, CD_READ_SECTORS) != 0) {
            return LOAD_RESULT;
        }
    }
}
