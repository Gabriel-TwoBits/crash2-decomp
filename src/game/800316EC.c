#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F228[] = "CD001";
char D_8005F230[] = "rfs\n";
char D_8005F238[] = ".";
char D_8005F23C[] = "..";
s32 D_8005F240 = 0;

#ifdef NON_MATCHING
#define SECTOR_SIZE 0x800
#define ISO_ID_OFFSET 1
#define ISO_ID_LENGTH 5
#define ISO_ROOT_RECORD_OFFSET 0x8C
#define PVD_SECTOR 0x10
#define NO_SECTOR (-1)
#define SECTOR_READ_OK 1
#define SECTOR_COUNT_TABLE 4
#define TABLE_BIAS_CHAR '0'
#define MAX_PATH_RECORDS 0x80
#define MAX_DIR_RECORDS 0x40
#define NAME_SIZE 0x20
#define REC_LENGTH 0
#define REC_EXTENT 2
#define REC_SIZE 10
#define REC_NAME_LENGTH 0x20
#define REC_NAME 0x21
#define REC_PATH_NAME 8
#define CHAR_UPPER_S 'S'
#define CHAR_LOWER_S 's'
#define CHAR_UPPER_D 'D'
#define CHAR_LOWER_D 'd'
#define HEX_LETTER_FIRST 0x41
#define HEX_DIGIT_BASE 0x330
#define HEX_LETTER_BASE 0x337
#define HEX_SHIFT 4
#define TOC_ENTRY_SIZE 12
#define EXTENT_MASK_A 0x7FFFFF
#define SIZE_MASK_A 0x1FFFF
#define EXTENT_MASK_B 0xFFFFFF
#define NAME_TYPE_INDEX 11
#define NAME_HI_INDEX 6
#define NAME_LO_INDEX 7

#define READ_U32(p) ((u32)(p)[0] | ((u32)(p)[1] << 8) | ((u32)(p)[2] << 16) | ((u32)(p)[3] << 24))

/* The locals as one block: the unchecked table index reaches its neighbours, as in the original frame. */
typedef struct {
    u8 name[NAME_SIZE];
    s32 table[SECTOR_COUNT_TABLE];
    u32 extent;
    u32 pathExtent;
    u32 dataSize;
} Func800316ECLocals;

extern u32 D_80069034;
extern u32 D_80069038;
extern u32 D_8006903C;
extern s32 strncmp(char *, char *, s32);
extern s32 strlen(char *);
extern void *memcpy(void *, void *, s32);
extern int printf(char *, ...);

void func_800316EC(void) {
    Func800316ECLocals l;
    u8 *sector;
    u8 *rec;
    s32 count;
    s32 i;
    s32 index;
    u32 *entry;

    sector = (u8 *)func_8001148C(SECTOR_SIZE);
    if (func_80031680(1, PVD_SECTOR, (unsigned long *)sector) != SECTOR_READ_OK
        || strncmp((char *)sector + ISO_ID_OFFSET, D_8005F228, ISO_ID_LENGTH) != 0) {
        func_800116C0((s32)sector);
        return;
    } else {
        printf(D_8005F230);
    }
    l.extent = READ_U32(sector + ISO_ROOT_RECORD_OFFSET);
    if (func_80031680(1, l.extent, (unsigned long *)sector) != SECTOR_READ_OK) {
        func_800116C0((s32)sector);
        return;
    }
    for (i = SECTOR_COUNT_TABLE - 1; i >= 0; i--) {
        l.table[i] = NO_SECTOR;
    }
    rec = sector;
    count = 0;
    if (rec < sector + SECTOR_SIZE) {
        do {
            if (rec[REC_LENGTH] == 0) {
                break;
            }
            memcpy(l.name, rec + REC_PATH_NAME, rec[REC_LENGTH]);
            l.name[rec[REC_LENGTH]] = 0;
            l.pathExtent = READ_U32(rec + REC_EXTENT);
            if (strlen((char *)l.name) == 2 && (l.name[0] == CHAR_UPPER_S || l.name[0] == CHAR_LOWER_S)) {
                l.table[l.name[1] - TABLE_BIAS_CHAR] = l.pathExtent;
            }
            count++;
            rec += rec[REC_LENGTH] + ((rec[REC_LENGTH] & 1) + REC_PATH_NAME);
        } while (count < MAX_PATH_RECORDS && rec < sector + SECTOR_SIZE);
    }
    for (i = 0; i < SECTOR_COUNT_TABLE; i++) {
        if (l.table[i] != NO_SECTOR && func_80031680(1, l.table[i], (unsigned long *)sector) != SECTOR_READ_OK) {
            func_800116C0((s32)sector);
            return;
        }
        rec = sector;
        count = 0;
        if (rec < sector + SECTOR_SIZE) {
            do {
                if (rec[REC_LENGTH] == 0) {
                    break;
                }
                l.extent = READ_U32(rec + REC_EXTENT);
                l.dataSize = READ_U32(rec + REC_SIZE);
                if (count == 0) {
                    l.name[0] = D_8005F238[0];
                    l.name[1] = D_8005F238[1];
                } else if (count == 1) {
                    l.name[0] = D_8005F23C[0];
                    l.name[1] = D_8005F23C[1];
                    l.name[2] = D_8005F23C[2];
                } else {
                    memcpy(l.name, rec + REC_NAME, rec[REC_NAME_LENGTH]);
                    l.name[rec[REC_NAME_LENGTH]] = 0;
                }
                if (l.name[0] == CHAR_UPPER_S || l.name[0] == CHAR_LOWER_S) {
                    if (l.name[NAME_LO_INDEX] > HEX_LETTER_FIRST - 1) {
                        index = (l.name[NAME_HI_INDEX] << HEX_SHIFT) - HEX_LETTER_BASE;
                    } else {
                        index = (l.name[NAME_HI_INDEX] << HEX_SHIFT) - HEX_DIGIT_BASE;
                    }
                    index += l.name[NAME_LO_INDEX];
                    if (l.name[NAME_TYPE_INDEX] == CHAR_UPPER_D || l.name[NAME_TYPE_INDEX] == CHAR_LOWER_D) {
                        entry = (u32 *)((u8 *)&D_80069034 + index * TOC_ENTRY_SIZE);
                        entry[0] = (entry[0] & ~EXTENT_MASK_A) | (l.extent & EXTENT_MASK_A);
                        entry[1] = (entry[1] & ~SIZE_MASK_A) | (l.dataSize & SIZE_MASK_A);
                    } else {
                        entry = (u32 *)((u8 *)&D_8006903C + index * TOC_ENTRY_SIZE);
                        entry[0] = (entry[0] & ~EXTENT_MASK_B) | (l.extent & EXTENT_MASK_B);
                    }
                } else {
                    /* not a sector-table name: nothing to record */
                }
                count++;
                rec += rec[REC_LENGTH];
            } while (count < MAX_DIR_RECORDS && rec < sector + SECTOR_SIZE);
        }
    }
    func_800116C0((s32)sector);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800316EC", func_800316EC);
#endif
